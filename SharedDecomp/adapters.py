#!/usr/bin/env python3
"""Adapt actual RLOFT tool outputs to bounded, provenance-honest evidence.

This is metadata normalization, NOT independent original binary, CFG or DOS
execution certification. Uses Python 3.10+ standard library exclusively.
"""
from __future__ import annotations

import argparse
import hashlib
import json
import re
import sys
from pathlib import Path

from workflow import EvidenceError, atomic_json, require

SCHEMA = "rloft-structural-observation/v1"
SHA = re.compile(r"[0-9a-fA-F]{64}\Z")
MAX_SOURCE = 32 * 1024 * 1024
MAX_EVENTS = 16384


def _pairs(items):
    result = {}
    for key, value in items:
        require(key not in result, "duplicate JSON object member")
        result[key] = value
    return result


def _nonfinite(value):
    raise EvidenceError("nonfinite numeric JSON value rejected")


def _read(path: Path) -> tuple[dict, str]:
    raw = path.read_bytes()
    require(0 < len(raw) <= MAX_SOURCE, "input outside bounded metadata size")
    doc = json.loads(raw.decode("utf-8-sig"), object_pairs_hook=_pairs,
                     parse_constant=_nonfinite)
    require(isinstance(doc, dict), "input must be a JSON object")
    return doc, hashlib.sha256(raw).hexdigest()


def _unsigned(obj: dict, key: str, maximum: int = 1_000_000_000) -> int:
    value = obj.get(key)
    require(type(value) is int and 0 <= value <= maximum,
            f"invalid bounded integer {key}")
    return value


def _string(obj: dict, key: str) -> str:
    value = obj.get(key)
    require(isinstance(value, str) and 0 < len(value) <= 256,
            f"missing/invalid text {key}")
    return value


def _sha(obj: dict, key: str) -> str:
    value = _string(obj, key)
    require(SHA.fullmatch(value) is not None, f"invalid SHA-256 {key}")
    return value.lower()


def _unique_names(rows: list, name: str, limit: int) -> None:
    require(len(rows) <= limit, f"too many {name} records")
    keys = [_string(row, name) for row in rows]
    require(len(keys) == len(set(keys)), f"duplicate {name} identifier")


def _envelope(domain: str, original_id: str, input_sha: str, fields: dict,
              status: str, producer: str) -> dict:
    return {
        "schema": SCHEMA, "domain": domain, "producer": producer,
        "original_identity_declared": original_id,
        "original_identity_attested": False,
        "input_report_sha256": input_sha,
        "evidence": "STATIC_SOURCE_REPORT_ONLY",
        "status": status,
        "observations": dict(sorted(fields.items())),
        "runtime_parity_verified": False,
        "original_code_verified": False,
    }


def adapt_bank(doc: dict, digest: str) -> dict:
    require(doc.get("schema") == "rloft-native-bank-audit/v1",
            "unsupported NativeEngine RES audit schema")
    rows = doc.get("banks")
    require(isinstance(rows, list) and rows, "native bank list absent")
    _unique_names(rows, "name", 13)
    fields = {}
    required = ["records", "validated", "failed", "absent_banks", "recognized_eh"]
    for key in required:
        fields["total/" + key] = _unsigned(doc, key)
    require(fields["total/validated"] + fields["total/failed"] <=
            fields["total/records"] + fields["total/absent_banks"],
            "native RES totals inconsistent")
    sums = {"records": 0, "validated": 0}
    columns = ["bytes", "records", "validated", "failed", "empty", "non_eh",
               "raw_wordcode", "image_eh1", "inert_legacy_eh1", "font_eh2",
               "xmidi_eh3", "voc_eh4", "texture_eh6", "shape_eh7",
               "shipped_anomalies"]
    for row in rows:
        name = _string(row, "name")
        require(re.fullmatch(r"RES(?:[0-9]|1[012])", name) is not None,
                "unknown original RES bank name")
        for col in columns:
            fields[f"bank/{name}/{col}"] = _unsigned(row, col)
        require(row["validated"] + row["failed"] <= row["records"] or
                row["records"] == 0, f"{name}: invalid record partition")
        sums["records"] += row["records"]
        sums["validated"] += row["validated"]
    require(sums["records"] == doc["records"] and
            sums["validated"] == doc["validated"],
            "RES per-bank totals disagree with report")
    issues = doc.get("issues")
    require(isinstance(issues, list) and len(issues) <= 256,
            "missing or excessive RES issue ledger")
    fields["total/issue_count"] = len(issues)
    require(fields["total/absent_banks"] <= len(rows), "invalid absent bank count")
    status = "STRUCTURAL_REPORT_PASS" if not (
        doc["failed"] or doc["absent_banks"] or issues) else "STRUCTURAL_REPORT_FAIL"
    return _envelope("original_res_bank_inventory", "unattributed", digest,
                     fields, status, "NativeEngine/rloft_native_bank_audit")


def adapt_world(doc: dict, digest: str) -> dict:
    require(doc.get("schema") == "rloft-native-world-system/v1",
            "unsupported NativeEngine world audit schema")
    columns = ["level_count", "level_cells", "controls", "event_slots",
               "event_commands", "level_archive_bytes", "sintab_bytes",
               "sintab_scales", "sintab_scale_bytes"]
    fields = {"total/" + k: _unsigned(doc, k) for k in columns}
    rows = doc.get("levels")
    require(isinstance(rows, list) and len(rows) == fields["total/level_count"]
            and 0 < len(rows) <= 64, "world level enumeration incomplete")
    sums = {"cells": 0, "controls": 0, "events": 0, "commands": 0}
    for n, row in enumerate(rows):
        require(_unsigned(row, "id") == n, "original level IDs unordered")
        w, h = _unsigned(row, "w", 512), _unsigned(row, "h", 512)
        require(w > 0 and h > 0, "invalid world level dimensions")
        require(w * h == _unsigned(row, "cells"), "level cell dimensions disagree")
        for k in ("w", "h", "cells", "controls", "a", "b", "c", "events", "commands"):
            fields[f"level/{n:02d}/{k}"] = _unsigned(row, k)
        for k in sums:
            sums[k] += row[k]
    for level_key, aggregate in (("cells", "level_cells"),
                                 ("controls", "controls"),
                                 ("events", "event_slots"),
                                 ("commands", "event_commands")):
        require(sums[level_key] == doc[aggregate],
                f"world totals inconsistent for {level_key}")
    return _envelope("original_world_and_sintab_inventory", "unattributed",
                     digest, fields, "STRUCTURAL_REPORT_PASS",
                     "NativeEngine/rloft_native_world_audit")


def adapt_dosdis(doc: dict, digest: str) -> dict:
    fmt = doc.get("format")
    require(fmt in ("dosdis-analysis-report",
                    "dosdis-mz-16-candidate-analysis"),
            "unsupported DOSDis analysis export")
    require(doc.get("schemaVersion") == 1, "unknown DOSDis export version")
    require(str(doc.get("analysisStatus", "")).startswith("candidate-unverified"),
            "DOSDis report attempts unsupported proof claim")
    source = doc.get("source")
    require(isinstance(source, dict), "DOSDis source metadata missing")
    source_sha = _sha(source, "sha256")
    require(source.get("sourceBinaryIncluded") is False
            and source.get("sourcePathIncluded") is False,
            "DOSDis report includes private source binary/path")
    fields = {"format/mz16": fmt == "dosdis-mz-16-candidate-analysis"}
    if fmt == "dosdis-analysis-report":
        counts = doc.get("counts")
        require(isinstance(counts, dict), "DOSDis candidate counts missing")
        for name, field in (("instructions", "instructions"),
                            ("edges", "edges"), ("diagnostics", "diagnostics"),
                            ("functions", "functions"), ("references", "references"),
                            ("jumpTables", "jump_tables")):
            fields["candidate/" + field] = _unsigned(counts, name)
        truncated = counts.get("truncated")
        require(type(truncated) is bool, "DOSDis truncated flag missing")
        for name, field in (("instructions", "instructions"),
                            ("edges", "edges"), ("diagnostics", "diagnostics")):
            require(isinstance(doc.get(name), list) and
                    len(doc[name]) == counts[name],
                    f"DOSDis {name} cardinality mismatch")
    else:
        graph = doc.get("graph")
        require(isinstance(graph, dict), "DOSDis MZ graph metadata missing")
        for name, key in (("instructionCount", "instructions"),
                          ("edgeCount", "edges"), ("diagnosticCount", "diagnostics"),
                          ("UnknownFileBackedBytes", "unknown_file_backed_bytes"),
                          ("CandidateInstructionBytes", "candidate_instruction_bytes")):
            fields["candidate/" + key] = _unsigned(graph, name)
        truncated = graph.get("Truncated")
        require(type(truncated) is bool, "DOSDis MZ truncation flag missing")
        for col, count in (("instructions", "instructionCount"),
                           ("edges", "edgeCount"), ("diagnostics", "diagnosticCount")):
            require(isinstance(doc.get(col), list) and
                    len(doc[col]) == graph[count],
                    f"DOSDis MZ {col} cardinality mismatch")
    fields["candidate/truncated"] = truncated
    return _envelope("dosdis_candidate_analysis", source_sha, digest,
                     fields, "CANDIDATE_UNVERIFIED",
                     "DOSDis/" + fmt)


def compare(left: dict, right: dict) -> dict:
    for side in (left, right):
        require(side.get("schema") == SCHEMA, "incompatible observation schema")
    require(left["domain"] == right["domain"], "different observation domains")
    require(left["original_identity_declared"] == right["original_identity_declared"],
            "different declared original identities")
    a, b = left["observations"], right["observations"]
    require(isinstance(a, dict) and isinstance(b, dict),
            "observations must be JSON objects")
    keys = sorted(set(a) | set(b))
    changes = [{"key": k, "reference": a.get(k), "candidate": b.get(k),
                "reference_present": k in a, "candidate_present": k in b}
               for k in keys if k not in a or k not in b or a[k] != b[k]]
    return {"schema": "rloft-metadata-diff/v1",
            "status": "EQUAL_METADATA_UNATTESTED" if not changes else "DIFFERENT",
            "domain": left["domain"],
            "compared_fields": len(keys),
            "difference_count": len(changes),
            "first_differences": changes[:30],
            "original_identity_attested": False,
            "runtime_parity_verified": False,
            "note": "Metadata comparison only. No original execution, source byte or gameplay parity."}


def adapt_demo_events(path: Path, manifest: dict) -> tuple[list[dict], dict]:
    require(manifest.get("format") == "rloft-level22-diagnostic-v1" and
            manifest.get("provenance") == "native-preview-not-original-DOS",
            "unsupported DemoV1 diagnostic manifest")
    revision = _string(manifest, "source_revision")
    raw = path.read_bytes()
    require(0 < len(raw) <= MAX_SOURCE, "DemoV1 events outside bounded size")
    frames = []
    for line in raw.decode("utf-8").splitlines():
        if not line.strip():
            continue
        require(len(frames) < MAX_EVENTS, "DemoV1 event count exceeds bound")
        obj = json.loads(line, object_pairs_hook=_pairs, parse_constant=_nonfinite)
        require(isinstance(obj, dict), "DemoV1 event must be object")
        n = _unsigned(obj, "seq", MAX_EVENTS)
        require(n == len(frames) + 1, "DemoV1 event sequence gap, duplicate or reorder")
        name = _string(obj, "event")
        a, b = obj.get("a"), obj.get("b")
        require(type(a) is int and type(b) is int and
                -(2**63) <= a < 2**63 and -(2**63) <= b < 2**63,
                "DemoV1 event arguments must be signed int64")
        frames.append({"step": n - 1, "event": name, "a": a, "b": b})
    require(frames, "no DemoV1 diagnostic events")
    summary = {
        "schema": "rloft-demo-native-event-adapter/v1",
        "source_revision_declared": revision,
        "source_kind": "demo_native_diagnostic_only",
        "original_dos_capture": False,
        "runtime_parity_verified": False,
        "input_sha256": hashlib.sha256(raw).hexdigest(),
        "events": len(frames),
        "truncated_by_original_logger": None,
        "note": "Unknown: original logger has a finite cap and cannot prove no dropped events.",
    }
    return frames, summary


def main(argv=None) -> int:
    p = argparse.ArgumentParser(description=__doc__)
    sub = p.add_subparsers(dest="command", required=True)
    a = sub.add_parser("adapt")
    a.add_argument("kind", choices=["native-bank", "native-world", "dosdis"])
    a.add_argument("input", type=Path)
    a.add_argument("--output", type=Path, required=True)
    a = sub.add_parser("compare")
    a.add_argument("reference", type=Path)
    a.add_argument("candidate", type=Path)
    a.add_argument("--output", type=Path, required=True)
    a = sub.add_parser("demo-events")
    a.add_argument("events", type=Path)
    a.add_argument("--manifest", type=Path, required=True)
    a.add_argument("--output", type=Path, required=True)
    a.add_argument("--report", type=Path, required=True)
    args = p.parse_args(argv)
    try:
        if args.command == "adapt":
            data, digest = _read(args.input)
            result = {"native-bank": adapt_bank, "native-world": adapt_world,
                      "dosdis": adapt_dosdis}[args.kind](data, digest)
            atomic_json(args.output, result)
        elif args.command == "compare":
            left, _ = _read(args.reference)
            right, _ = _read(args.candidate)
            result = compare(left, right)
            atomic_json(args.output, result)
        else:
            manifest, _ = _read(args.manifest)
            frames, result = adapt_demo_events(args.events, manifest)
            args.output.parent.mkdir(parents=True, exist_ok=True)
            args.output.write_text("".join(json.dumps(f, sort_keys=True) + "\n"
                                           for f in frames), encoding="utf-8")
            atomic_json(args.report, result)
        print(json.dumps({"status": result.get("status", "NORMALIZED_NATIVE_ONLY"),
                          "domain": result.get("domain", "demo_native_events"),
                          "runtime_parity_verified": False}, sort_keys=True))
        return int(result.get("status") == "DIFFERENT")
    except (ValueError, TypeError, OSError, KeyError, UnicodeError,
            json.JSONDecodeError) as exc:
        print(f"ADAPTER ERROR: {exc}", file=sys.stderr)
        return 2


if __name__ == "__main__":
    raise SystemExit(main())
