#!/usr/bin/env python3
"""Independent original-byte vs native typed-world fingerprints gate.

Checks all 32 source levels and their exact section SHA-256s. Native runtime,
binary provenance and original DOS execution are NOT certified by hash parity.
"""
from __future__ import annotations

import argparse
import hashlib
import json
from pathlib import Path
import re
import sys

from original_world_oracle import OracleError, audit
from workflow import atomic_json

SCHEMA = "rloft-native-world-payload-fingerprints/v1"
RESULT_SCHEMA = "rloft-original-native-payload-diff/v1"
SOURCE_LIMIT = 8 * 1024 * 1024
REPORT_LIMIT = 1024 * 1024
SHA = re.compile(r"[0-9a-f]{64}\Z")
HASH_FIELDS = ("header_sha256", "cells_sha256", "controls_sha256",
               "events_sha256", "automap_sha256", "annotations_sha256",
               "family_a_sha256", "family_b_sha256", "family_c_sha256")


def require(value, message):
    if not value:
        raise OracleError(message)


def u32(src, offset):
    require(0 <= offset <= len(src)-4, "original level directory truncated")
    return int.from_bytes(src[offset:offset+4], "little")


def expected_hashes(src: bytes, level: int, source_row: dict) -> dict:
    start = u32(src, 4+4*level)
    end = u32(src, 8+4*level) if level < 31 else len(src)
    require(132 <= start < end <= len(src), "original level record bounds")
    rec = src[start:end]
    bounds = [u32(rec, 1+4*i) for i in range(7)]
    # The independently validated original_world_oracle.audit() already
    # checks every section boundary; calculate family SHA from raw source.
    evidence = {
        # New 0x2A4 fixed header includes original renderer resource
        # mapping tables and unresolved authoring bytes. It is hashed
        # directly from original media, never copied from native metadata.
        "header_sha256": hashlib.sha256(
            rec[bounds[0]:bounds[0]+0x2a4]).hexdigest(),
        "cells_sha256": source_row["cells_sha256"],
        "controls_sha256": source_row["controls_sha256"],
        "events_sha256": source_row["events_sha256"],
        "automap_sha256": source_row["automap_sha256"],
        "annotations_sha256": source_row["annotations_sha256"],
    }
    for family,name in enumerate(("family_a_sha256", "family_b_sha256",
                                  "family_c_sha256")):
        evidence[name] = hashlib.sha256(
            rec[bounds[2+family]:bounds[3+family]]).hexdigest()
    return evidence


def _equals(changes: list, name: str, expected, actual):
    if type(expected) is not type(actual) or expected != actual:
        changes.append({"field": name, "original": expected,
                        "native": actual})


def compare_payload(src: bytes, sintab: bytes, native: dict,
                    require_known: bool = True) -> dict:
    original = audit(src, sintab, require_known=require_known)
    require(native.get("schema") == SCHEMA, "wrong native payload report schema")
    require(native.get("decoded_section_roundtrip_equal") is True,
            "native decoded section roundtrip is not certified")
    for forbidden in ("original_file_identity_independently_attested",
                      "original_dos_execution_observed",
                      "native_gameplay_parity_verified"):
        require(native.get(forbidden) is False,
                f"unsupported native runtime/source attestation in {forbidden}")
    changes = []
    _equals(changes, "input_levels_sha256",
            original["original_sha256"]["LEVELS.DAT"],
            native.get("input_levels_sha256"))
    _equals(changes, "input_levels_bytes", len(src),
            native.get("input_levels_bytes"))
    for source_key, target_key in (("level_cells", "total_cells"),
                                    ("controls", "total_controls"),
                                    ("event_commands", "total_commands")):
        _equals(changes, target_key, original["summary"][source_key],
                native.get(target_key))
    rows = native.get("levels")
    require(isinstance(rows, list) and len(rows) == 32,
            "native must enumerate exactly 32 original levels")
    for id, (row, original_row) in enumerate(zip(rows, original["levels"])):
        require(isinstance(row, dict), f"native level {id} not an object")
        for key, other in (("id", "id"), ("cells", "cells"),
                           ("controls", "controls"), ("commands", "commands")):
            _equals(changes, f"levels/{id}/{key}",
                    original_row[other], row.get(key))
        hashes = expected_hashes(src, id, original_row)
        for key in HASH_FIELDS:
            observed = row.get(key)
            require(isinstance(observed, str) and
                    SHA.fullmatch(observed) is not None,
                    f"native level {id} missing SHA256 {key}")
            _equals(changes, f"levels/{id}/{key}",
                    hashes[key], observed)
    matched = not changes
    return {
        "schema": RESULT_SCHEMA,
        "status": "ALL_32_ORIGINAL_SECTIONS_MATCH" if matched else "DIFFERENT",
        "original_identity_pinned": original["pinned_original"],
        "original_levels_sha256": original["original_sha256"]["LEVELS.DAT"],
        "original_sintab_sha256": original["original_sha256"]["SINTAB"],
        "native_report_source_sha256_matches_original": (
            original["original_sha256"]["LEVELS.DAT"] ==
            native.get("input_levels_sha256")),
        "verified_original_levels": 32,
        "section_hashes_checked": 32*len(HASH_FIELDS),
        "first_divergence": changes[0] if changes else None,
        "difference_count": len(changes),
        "differences": changes[:64],
        "compiled_native_binary_verified": False,
        "dos_original_execution_verified": False,
        "native_gameplay_parity_verified": False,
        "note": "Independently parsed original sections vs reported decoded "
                "native section hashes; no DOS playback or field UI accepted."}


def read_limited(path: Path, limit: int) -> bytes:
    require(0 < path.stat().st_size <= limit,
            "file exceeds bounded evidence input size")
    return path.read_bytes()


def read_report(path: Path) -> dict:
    def unique(pairs):
        out = {}
        for key, value in pairs:
            require(key not in out, "duplicate JSON property")
            out[key] = value
        return out
    def reject_number(value):
        raise OracleError("nonfinite JSON number")
    doc = json.loads(read_limited(path, REPORT_LIMIT).decode("utf-8-sig"),
                     object_pairs_hook=unique, parse_constant=reject_number)
    require(isinstance(doc, dict), "native payload report must be JSON object")
    return doc


def main(argv=None):
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument("--levels", type=Path, required=True)
    p.add_argument("--sintab", type=Path, required=True)
    p.add_argument("--native-report", type=Path, required=True)
    p.add_argument("--out", type=Path, required=True)
    p.add_argument("--allow-other-version", action="store_true")
    args = p.parse_args(argv)
    try:
        result = compare_payload(
            read_limited(args.levels, SOURCE_LIMIT),
            read_limited(args.sintab, SOURCE_LIMIT),
            read_report(args.native_report),
            require_known=not args.allow_other_version)
        atomic_json(args.out, result)
        print(json.dumps({"status": result["status"],
                          "section_hashes_checked": result["section_hashes_checked"],
                          "native_gameplay_parity_verified": False}, sort_keys=True))
        return 0 if result["status"]=="ALL_32_ORIGINAL_SECTIONS_MATCH" else 1
    except (OSError, ValueError, TypeError, KeyError, UnicodeError) as exc:
        print("NATIVE PAYLOAD EVIDENCE ERROR:", exc, file=sys.stderr)
        return 2


if __name__ == "__main__":
    raise SystemExit(main())
