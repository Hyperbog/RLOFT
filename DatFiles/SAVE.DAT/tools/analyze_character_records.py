#!/usr/bin/env python3
"""Inspect the 0x248-byte SAVE.DAT character records without guessing semantics."""
from __future__ import annotations

import argparse
import struct
from pathlib import Path

RECORD_SIZE = 0x248
PARTY_OFFSET, PARTY_COUNT = 0x77, 4
ROSTER_OFFSET, ROSTER_COUNT = 0xD68, 10
ITEM_OFFSET, ITEM_COUNT = 0x48, 21
POST_START, POST_END = 0x72, 0x248
PACKED_NIBBLE_OFFSET = 0x0C2
PACKED_BANKS, PACKED_BANK_SIZE = 8, 10
PACKED_NIBBLE_END = PACKED_NIBBLE_OFFSET + PACKED_BANKS * PACKED_BANK_SIZE
STATE_FLAGS_OFFSET = 0x13A


def u16(r: bytes, off: int) -> int:
    return struct.unpack_from("<H", r, off)[0]


def s16(r: bytes, off: int) -> int:
    return struct.unpack_from("<h", r, off)[0]


def u32(r: bytes, off: int) -> int:
    return struct.unpack_from("<I", r, off)[0]


def name16(r: bytes) -> str:
    raw = r[:0x10].split(b"\0", 1)[0]
    if not raw or all(b == 0xFF for b in raw):
        return "<unused/raw>"
    return raw.decode("cp437", errors="replace")


def sparse_post(r: bytes) -> str:
    """Report bytes in +0x72..+0x247 that are neither common fill value 00 nor FF."""
    cells = [(off, r[off]) for off in range(POST_START, POST_END) if r[off] not in (0x00, 0xFF)]
    if not cells:
        return "<none>"
    return ", ".join(f"+0x{off:03X}=0x{value:02X}" for off, value in cells)


def packed_banks(r: bytes) -> list[str]:
    """Render the executable-proved 8 x 10 packed-nibble array."""
    rows: list[str] = []
    for bank in range(PACKED_BANKS):
        start = PACKED_NIBBLE_OFFSET + bank * PACKED_BANK_SIZE
        cells = []
        for entry, value in enumerate(r[start:start + PACKED_BANK_SIZE]):
            cells.append(f"{entry}:{value >> 4:X}/{value & 0x0F:X}")
        rows.append(f"bank{bank} " + " ".join(cells))
    return rows


def show(label: str, index: int, base: int, r: bytes) -> None:
    print(f"{label}[{index:02d}] file=0x{base:04X} name={name16(r)!r}")
    print(f"  meta10_17 : {r[0x10:0x18].hex(' ')}")
    print(f"  status    : 0x{u16(r, 0x18):04X}")
    print(f"  core1A_25 : {r[0x1A:0x26].hex(' ')}")
    print("  dword26   : " + ", ".join(f"0x{u32(r, o):08X}" for o in (0x26, 0x2A, 0x2E)))
    print("  pairs32   : " + ", ".join(f"{r[o]}/{r[o+1]}" for o in range(0x32, 0x40, 2)))
    print("  words40   : " + ", ".join(str(s16(r, o)) for o in range(0x40, 0x48, 2)))
    print("  item[21]  : " + ", ".join(str(s16(r, ITEM_OFFSET + i * 2)) for i in range(ITEM_COUNT)))
    print(f"  packed    : +0x{PACKED_NIBBLE_OFFSET:03X}..+0x{PACKED_NIBBLE_END - 1:03X} (8 banks x 10 bytes; hi/lo nibbles)")
    for row in packed_banks(r):
        print(f"    {row}")
    print(f"  flags13A  : 0x{u32(r, STATE_FLAGS_OFFSET):08X}")
    print(f"  post!=fill: {sparse_post(r)}")


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("save", type=Path)
    args = ap.parse_args()
    data = args.save.read_bytes()

    for label, start, count in (("party", PARTY_OFFSET, PARTY_COUNT), ("roster", ROSTER_OFFSET, ROSTER_COUNT)):
        for i in range(count):
            base = start + i * RECORD_SIZE
            r = data[base:base + RECORD_SIZE]
            if len(r) != RECORD_SIZE:
                raise SystemExit(f"short record at 0x{base:X}")
            show(label, i, base, r)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
