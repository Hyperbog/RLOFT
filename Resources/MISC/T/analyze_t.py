#!/usr/bin/env python3
"""Validate and inspect Ravenloft's indexed T text archive."""
from __future__ import annotations

import argparse
import hashlib
import json
import struct
from pathlib import Path

XOR_KEY = 0xDC


def parse_t(data: bytes):
    if len(data) < 4:
        raise ValueError("T is shorter than stored count")
    count = struct.unpack_from("<I", data, 0)[0]
    table_end = 4 + count * 4
    if table_end > len(data):
        raise ValueError("offset table exceeds file")
    offsets = list(struct.unpack_from(f"<{count}I", data, 4)) if count else []
    if any(b <= a for a, b in zip(offsets, offsets[1:])):
        raise ValueError("active offsets are not strictly increasing")
    if offsets and offsets[0] < table_end:
        raise ValueError("first record overlaps active offset table")

    records = []
    gaps = []
    zero_length = 0
    max_payload = 0
    exact_next = 0
    for i, off in enumerate(offsets):
        if off + 2 > len(data):
            raise ValueError(f"record {i} length word exceeds EOF")
        length = struct.unpack_from("<H", data, off)[0]
        end = off + 2 + length
        limit = offsets[i + 1] if i + 1 < count else len(data)
        if end > limit:
            raise ValueError(f"record {i} payload overlaps next record/EOF")
        enc = data[off + 2:end]
        dec = bytes(b ^ XOR_KEY for b in enc)
        gap = data[end:limit]
        if length == 0:
            zero_length += 1
        max_payload = max(max_payload, length)
        if end == limit:
            exact_next += 1
        if gap:
            gaps.append({"after_index": i, "start": end, "end": limit, "size": len(gap), "sha256": hashlib.sha256(gap).hexdigest()})
        records.append({
            "index": i,
            "offset": off,
            "length": length,
            "encoded": enc,
            "decoded": dec,
            "end": end,
            "gap_size": len(gap),
        })

    pre_slack = data[table_end:(offsets[0] if offsets else len(data))]
    rebuilt = bytearray(data)
    struct.pack_into("<I", rebuilt, 0, count)
    for i, off in enumerate(offsets):
        struct.pack_into("<I", rebuilt, 4 + i * 4, off)
        r = records[i]
        struct.pack_into("<H", rebuilt, off, r["length"])
        rebuilt[off + 2:r["end"]] = bytes(b ^ XOR_KEY for b in r["decoded"])

    return {
        "file_bytes": len(data),
        "sha256": hashlib.sha256(data).hexdigest(),
        "stored_count": count,
        "table_end": table_end,
        "first_record_offset": offsets[0] if offsets else None,
        "pre_payload_slack_bytes": len(pre_slack),
        "pre_payload_slack_sha256": hashlib.sha256(pre_slack).hexdigest() if pre_slack else None,
        "zero_length_records": zero_length,
        "maximum_payload_bytes": max_payload,
        "records_ending_at_next_offset_or_eof": exact_next,
        "inter_record_gap_count": len(gaps),
        "inter_record_gap_bytes": sum(g["size"] for g in gaps),
        "largest_gap_bytes": max((g["size"] for g in gaps), default=0),
        "last_record_ends_at_eof": (records[-1]["end"] == len(data)) if records else table_end == len(data),
        "decoded_records_with_nul": sum(b"\x00" in r["decoded"] for r in records),
        "round_trip_exact": bytes(rebuilt) == data,
        "gaps": gaps,
        "records": records,
    }


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("t_file", type=Path)
    ap.add_argument("--json", action="store_true")
    ap.add_argument("--dump", type=int, metavar="INDEX")
    args = ap.parse_args()
    result = parse_t(args.t_file.read_bytes())

    if args.dump is not None:
        if args.dump < 0 or args.dump >= result["stored_count"]:
            raise SystemExit("record index out of range")
        r = result["records"][args.dump]
        print(r["decoded"].decode("cp437", errors="replace"))
        return 0

    if args.json:
        out = {k: v for k, v in result.items() if k != "records"}
        print(json.dumps(out, indent=2))
    else:
        keys = [
            "file_bytes", "sha256", "stored_count", "table_end", "first_record_offset",
            "pre_payload_slack_bytes", "zero_length_records", "maximum_payload_bytes",
            "records_ending_at_next_offset_or_eof", "inter_record_gap_count",
            "inter_record_gap_bytes", "largest_gap_bytes", "last_record_ends_at_eof",
            "decoded_records_with_nul", "round_trip_exact",
        ]
        for k in keys:
            print(f"{k}: {result[k]}")

    return 0 if result["round_trip_exact"] and result["last_record_ends_at_eof"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
