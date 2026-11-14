#!/usr/bin/env python3
"""Cross-title RES Type-5/6 decoder validation via the unchanged Strahd LZSS oracle.

Original RES files read-only; decoded payload bytes never written to reports.
"""
from __future__ import annotations
import argparse
import collections
import hashlib
import json
import runpy
import struct
from pathlib import Path

GAME_DIRS = {
    "menzo": ("Menzoberranzan", "RES3", "RES4"),
    "stone": ("Ravenloft - Stone Prophet", "PROPHET/RES3", "PROPHET/RES4"),
}


def inspect(path: Path, parser: dict):
    data = path.read_bytes()
    _, entries = parser["parse_res"](data)
    summaries = collections.defaultdict(
        lambda: {"records": 0, "compression_methods": collections.Counter(),
                 "decoded_target_histogram": collections.Counter(),
                 "lzss_exact": 0, "trailing_payload": 0,
                 "decode_failures": 0, "anomaly_examples": [],
                 "alternative_model_matches": collections.Counter(),
                 "decoded_hash_examples": []})
    for idx, entry in enumerate(entries):
        if len(entry) < 5 or entry[:2] != b"EH" or entry[2] not in (5, 6):
            continue
        kind = entry[2]
        row = summaries[str(kind)]
        row["records"] += 1
        header_bytes = struct.unpack_from("<h", entry, 3)[0]
        if header_bytes != 5 or len(entry) < 10:
            row["decode_failures"] += 1
            row["anomaly_examples"].append({
                "index": idx, "reason": "not five-byte type header"})
            continue
        method = entry[5]
        target = struct.unpack_from("<I", entry, 6)[0]
        row["compression_methods"][method] += 1
        row["decoded_target_histogram"][target] += 1
        payload = entry[10:]
        if method != 3 or not 0 < target <= 32 * 1024 * 1024:
            row["decode_failures"] += 1
            row["anomaly_examples"].append({
                "index": idx, "method": method, "target": target,
                "reason": "unsupported method or output size"})
            continue
        try:
            decoded, consumed = parser["lzss"](payload, target)
            if len(decoded) != target:
                raise ValueError("decoded length not equal target")
            if consumed != len(payload):
                row["trailing_payload"] += 1
                if len(row["anomaly_examples"]) < 15:
                    row["anomaly_examples"].append({
                        "index": idx, "target": target, "payload_bytes": len(payload),
                        "consumed": consumed, "reason": "trailing encoded bytes"})
            else:
                row["lzss_exact"] += 1
            if len(row["decoded_hash_examples"]) < 3:
                row["decoded_hash_examples"].append({
                    "index": idx, "decoded_bytes": target,
                    "encoded_bytes": len(payload),
                    "decoded_sha256": hashlib.sha256(decoded).hexdigest()})
        except (ValueError, IndexError) as ex:
            row["decode_failures"] += 1
            diagnostics = {}
            if kind == 5 and target == 4098:
                # Hypothesis tests, NOT a substitute for executable semantics.
                # Try whether an internal prefix or an off-by-two decoded
                # size can explain all members of the same observed subgroup.
                candidates = {
                    "minus2": (payload, target-2),
                    "minus4": (payload, target-4),
                    "minus16": (payload, target-16),
                    "skip2": (payload[2:], target),
                    "skip4": (payload[4:], target),
                    "skip2_minus2": (payload[2:], target-2),
                    "skip4_minus2": (payload[4:], target-2),
                }
                for label,(encoded,decoded_size) in candidates.items():
                    try:
                        _,n = parser["lzss"](encoded, decoded_size)
                        if n == len(encoded):
                            row["alternative_model_matches"][label] += 1
                            diagnostics[label] = "full input consumption"
                    except (ValueError, IndexError):
                        pass
            if len(row["anomaly_examples"]) < 15:
                row["anomaly_examples"].append({
                    "index": idx, "target": target, "payload_bytes": len(payload),
                    "reason": str(ex), "alternatives": diagnostics})
    out = {}
    for k, row in summaries.items():
        row["compression_methods"] = dict(sorted(row["compression_methods"].items()))
        row["alternative_model_matches"] = dict(
            sorted(row["alternative_model_matches"].items()))
        row["decoded_target_histogram"] = {
            str(v): n for v,n in sorted(row["decoded_target_histogram"].items())
        }
        out[f"EH_type{k}"] = row
    return {"archive_file": path.name, "sha256": hashlib.sha256(data).hexdigest(),
            "original_entries": len(entries), "decoded_types": out}


# These digests are from the independent Phase 0 checkout audit, not this
# script's expected results. Reconstructed code/resource bytes are never bundled.
EXPECTED = {
    "menzo": {
        "RES3": ("7b31032e76bb8c7635736b168f9eb2052bb859576c125d1b7de20a932b57a7e6",
                 {"EH_type6": (3167, 3167, 0, 0, 0)}),
        "RES4": ("62b09c60881322c035567b58bc09c755e9f905ee0b620e5f3a3f49580ea4eafd",
                 {"EH_type5": (559, 474, 0, 85, 85),
                  "EH_type6": (502, 502, 0, 0, 0)}),
    },
    "stone": {
        "PROPHET/RES3": (
            "09440cb00e4ca741a3abc99bae810a6c600d10a474378efa8a8362fedf6b0534",
            {"EH_type6": (2542, 2542, 0, 0, 0)}),
        "PROPHET/RES4": (
            "17a2b3569806af98a030f3d653b79736e623b4b703531306b457d0c5d0a1c7e4",
            {"EH_type5": (963, 900, 0, 63, 63),
             "EH_type6": (410, 409, 1, 0, 0)}),
    },
}


def verify_expected(output):
    for title, archives in EXPECTED.items():
        if title not in output:
            raise ValueError(f"missing title: {title}")
        for name, (hash_expected, expected_types) in archives.items():
            row = output[title][name]
            if row["sha256"] != hash_expected:
                raise ValueError(f"source asset SHA-256 changed: {title}/{name}")
            if set(row["decoded_types"]) != set(expected_types):
                raise ValueError(f"unexpected typed records in {title}/{name}")
            for kind, expected in expected_types.items():
                detail = row["decoded_types"][kind]
                actual = (detail["records"], detail["lzss_exact"],
                          detail["trailing_payload"], detail["decode_failures"],
                          detail["alternative_model_matches"].get("minus2", 0))
                if actual != expected:
                    raise ValueError(f"{title}/{name}/{kind}: {actual} != {expected}")
    return True


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--root", type=Path, required=True)
    ap.add_argument("--repo-root", type=Path, default=Path("."))
    ap.add_argument("--json", type=Path, required=True)
    args = ap.parse_args()
    parser = runpy.run_path(str(
        args.repo_root / "Resources/RES/analyze_res.py"))
    output = {}
    for title, (folder, *archives) in GAME_DIRS.items():
        base = args.root / folder
        output[title] = {
            name: inspect(base / name, parser) for name in archives
        }
    verify_expected(output)
    print("Pinned Type-5/6 corpus regression: PASS")
    args.json.parent.mkdir(parents=True, exist_ok=True)
    args.json.write_text(json.dumps(output, indent=2) + "\n", encoding="utf-8")
    for title, archives in output.items():
        for name, file in archives.items():
            for kind, row in file["decoded_types"].items():
                print(title, name, kind,
                      "records", row["records"], "exact", row["lzss_exact"],
                      "trailing", row["trailing_payload"],
                      "failed", row["decode_failures"],
                      "methods", row["compression_methods"],
                      "alternatives", row["alternative_model_matches"],
                      "example", row["decoded_hash_examples"][:1])
    # First-pass diagnostic; reproducibility/coverage reported explicitly.
    # If a codec is incompatible, do not discard its evidence on an exit failure.


if __name__ == "__main__":
    main()
