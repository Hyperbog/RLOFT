#!/usr/bin/env python3
"""Inspect the proved LEVELS.DAT fixed-header tail fields.

This tool reports archive-wide distributions without normalizing any stored bytes.
Behavioral names follow HEADER_TAIL.md and certified CODE.2 consumers.
"""
from __future__ import annotations

import argparse
import collections
import struct
from pathlib import Path

PREAMBLE = 0x1D
HEADER_SIZE = 0x2A4
TAIL_OFF = 0x290


def u32(buf: bytes, off: int) -> int:
    return struct.unpack_from("<I", buf, off)[0]


def s16(buf: bytes, off: int) -> int:
    return struct.unpack_from("<h", buf, off)[0]


def archive_records(data: bytes) -> list[bytes]:
    count = u32(data, 0)
    offsets = list(struct.unpack_from(f"<{count}I", data, 4))
    if offsets[0] != 4 + count * 4:
        raise ValueError("unexpected archive directory extent")
    return [
        data[start : offsets[i + 1] if i + 1 < count else len(data)]
        for i, start in enumerate(offsets)
    ]


def header(record: bytes) -> bytes:
    bounds = struct.unpack_from("<7I", record, 1)
    if bounds[0] != PREAMBLE:
        raise ValueError("unexpected payload start")
    h = record[PREAMBLE : PREAMBLE + HEADER_SIZE]
    if len(h) != HEADER_SIZE:
        raise ValueError("truncated fixed header")
    return h


def describe(h: bytes) -> dict[str, object]:
    return {
        "environment_profile": h[0x290],
        "reserved_291": h[0x291],
        "npc_filter_0": s16(h, 0x292),
        "npc_filter_1": s16(h, 0x294),
        "npc_filter_2": s16(h, 0x296),
        "renderer_fill_A": h[0x298],
        "renderer_fill_B": h[0x299],
        "render_threshold_floor": s16(h, 0x29A),
        "resource_set_id": h[0x29C],
        "tail_29d_2a3": bytes(h[0x29D:0x2A4]),
    }


def main() -> None:
    ap = argparse.ArgumentParser()
    ap.add_argument("levels", type=Path)
    ap.add_argument("--lev", type=Path, help="optional standalone LEVnnn.DAT to compare")
    ap.add_argument("--level-id", type=int, default=22)
    args = ap.parse_args()

    records = archive_records(args.levels.read_bytes())
    rows = [describe(header(r)) for r in records]
    print(f"records={len(rows)}")

    for key in (
        "environment_profile",
        "npc_filter_0",
        "npc_filter_1",
        "npc_filter_2",
        "renderer_fill_A",
        "renderer_fill_B",
        "render_threshold_floor",
        "resource_set_id",
    ):
        counts = collections.Counter(row[key] for row in rows)
        print(f"{key}: " + " ".join(f"{value!r}:{count}" for value, count in sorted(counts.items())))

    nonzero_reserved = [
        i for i, row in enumerate(rows)
        if row["reserved_291"] != 0 or any(row["tail_29d_2a3"])
    ]
    print(f"records_with_nonzero_unresolved_tail={nonzero_reserved}")

    if args.lev is not None:
        if not (0 <= args.level_id < len(records)):
            raise ValueError("level id outside archive")
        base = header(records[args.level_id])
        direct = header(args.lev.read_bytes())
        diffs = [i for i, (a, b) in enumerate(zip(base, direct)) if a != b]
        print(f"header_differences={len(diffs)} offsets={[hex(i) for i in diffs]}")
        if diffs == [0x29A] and s16(base, 0x29A) == 0 and s16(direct, 0x29A) == 1:
            print("level22_normalization_match=yes (archive 0 -> live/save 1 at +0x29A)")
        else:
            print("level22_normalization_match=no")


if __name__ == "__main__":
    main()
