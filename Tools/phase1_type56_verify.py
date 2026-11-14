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
            if len(row["anomaly_examples"]) < 15:
                row["anomaly_examples"].append({
                    "index": idx, "target": target, "payload_bytes": len(payload),
                    "reason": str(ex)})
    out = {}
    for k, row in summaries.items():
        row["compression_methods"] = dict(sorted(row["compression_methods"].items()))
        row["decoded_target_histogram"] = {
            str(v): n for v,n in sorted(row["decoded_target_histogram"].items())
        }
        out[f"EH_type{k}"] = row
    return {"archive_file": path.name, "sha256": hashlib.sha256(data).hexdigest(),
            "original_entries": len(entries), "decoded_types": out}


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
                      "example", row["decoded_hash_examples"][:1])
    # First-pass diagnostic; reproducibility/coverage reported explicitly.
    # If a codec is incompatible, do not discard its evidence on an exit failure.


if __name__ == "__main__":
    main()
