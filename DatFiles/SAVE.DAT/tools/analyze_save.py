#!/usr/bin/env python3
"""Structural anchor analyzer for Ravenloft: Strahd's Possession SAVE.DAT.

This is deliberately conservative: it reports only boundaries supported by the
original 9,288-byte fixture plus the audited CODE.2 save/load path.  Unknown
regions remain opaque rather than being assigned speculative semantics.
"""

from __future__ import annotations

import argparse
from pathlib import Path

EXPECTED_SIZE = 0x2448  # 9,288 bytes
CHARACTER_SIZE = 0x248  # CODE.2 party/roster character record size
PARTY_OFFSET = 0x0077
PARTY_COUNT = 4
PARTY_SIZE = CHARACTER_SIZE * PARTY_COUNT  # 0x920
MIDDLE_OFFSET = PARTY_OFFSET + PARTY_SIZE   # 0x0997
CHARGES_OFFSET = 0x0D59
CHARGES_SIZE = 15
ROSTER_OFFSET = 0x0D68
ROSTER_COUNT = 10
ROSTER_SIZE = CHARACTER_SIZE * ROSTER_COUNT
TAIL_OFFSET = ROSTER_OFFSET + ROSTER_SIZE   # 0x2438
TAIL_SIZE = 0x10


def c_string(data: bytes, offset: int, limit: int = 32) -> str:
    raw = data[offset : offset + limit].split(b"\0", 1)[0]
    return raw.decode("cp437", errors="replace")


def hex_preview(data: bytes, offset: int, size: int, limit: int = 32) -> str:
    chunk = data[offset : offset + min(size, limit)]
    suffix = " ..." if size > limit else ""
    return chunk.hex(" ") + suffix


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("save", type=Path, help="path to SAVE.DAT")
    args = parser.parse_args()

    data = args.save.read_bytes()
    print(f"file: {args.save}")
    print(f"size: {len(data)} bytes (0x{len(data):X})")
    if len(data) != EXPECTED_SIZE:
        print(f"WARNING: reference fixture is {EXPECTED_SIZE} bytes (0x{EXPECTED_SIZE:X})")

    print("\nConfirmed structural anchors")
    print(f"  prefix/engine state : 0x0000-0x{PARTY_OFFSET-1:04X} ({PARTY_OFFSET} bytes)")
    print(f"  active party block  : 0x{PARTY_OFFSET:04X}-0x{MIDDLE_OFFSET-1:04X} "
          f"({PARTY_COUNT} x 0x{CHARACTER_SIZE:X} = 0x{PARTY_SIZE:X})")

    for i in range(PARTY_COUNT):
        off = PARTY_OFFSET + i * CHARACTER_SIZE
        print(f"    party[{i}] @ 0x{off:04X}: {c_string(data, off)!r}")

    middle_size = CHARGES_OFFSET - MIDDLE_OFFSET
    print(f"  opaque middle state : 0x{MIDDLE_OFFSET:04X}-0x{CHARGES_OFFSET-1:04X} "
          f"(0x{middle_size:X} bytes)")
    print(f"  charges vector      : 0x{CHARGES_OFFSET:04X}-0x{CHARGES_OFFSET+CHARGES_SIZE-1:04X} "
          f"({CHARGES_SIZE} bytes)")
    print(f"    bytes: {hex_preview(data, CHARGES_OFFSET, CHARGES_SIZE)}")
    print(f"  saved roster block  : 0x{ROSTER_OFFSET:04X}-0x{TAIL_OFFSET-1:04X} "
          f"({ROSTER_COUNT} x 0x{CHARACTER_SIZE:X} = 0x{ROSTER_SIZE:X})")

    for i in range(ROSTER_COUNT):
        off = ROSTER_OFFSET + i * CHARACTER_SIZE
        print(f"    roster[{i:02d}] @ 0x{off:04X}: {c_string(data, off)!r}")

    print(f"  trailing state      : 0x{TAIL_OFFSET:04X}-0x{TAIL_OFFSET+TAIL_SIZE-1:04X} "
          f"(0x{TAIL_SIZE:X} bytes)")
    print(f"    bytes: {hex_preview(data, TAIL_OFFSET, TAIL_SIZE)}")

    expected_end = TAIL_OFFSET + TAIL_SIZE
    if expected_end != len(data):
        print(f"WARNING: structural anchors end at 0x{expected_end:X}, file ends at 0x{len(data):X}")
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
