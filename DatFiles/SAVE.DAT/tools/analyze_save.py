#!/usr/bin/env python3
"""Analyze the serializer-certified Ravenloft: Strahd's Possession SAVE.DAT layout.

The supplied quick-start fixture is 0x2448 bytes because its live-world-object
count is zero. Later offsets are derived from that serialized count. The 938-byte
fixture bank is persistent category-1 container contents/state, not level state.
"""
from __future__ import annotations

import argparse
import struct
from pathlib import Path

CHARACTER_SIZE = 0x248
PREFIX_SIZE = 0x77
PARTY_COUNT = 4
PARTY_OFFSET = PREFIX_SIZE
PARTY_SIZE = CHARACTER_SIZE * PARTY_COUNT
MIDDLE_OFFSET = PARTY_OFFSET + PARTY_SIZE
WORLD_OBJECT_SIZE = 0x16
WORLD_OBJECT_LIMIT = 0x96
POST_OBJECT_FIXED_BEFORE_CONTAINER_BANK = 12  # D8A8[2] + A7E4
CONTAINER_STATE_COUNT = 62
CONTAINER_MUTABLE_PAYLOAD_SIZE = 0x3AA  # quick-start/build-fixed 62-state payload
POST_CONTAINER_GLOBALS_SIZE = 8          # F920 + F91C
CHARGES_SIZE = 15
ROSTER_COUNT = 10
ROSTER_SIZE = CHARACTER_SIZE * ROSTER_COUNT
TAIL_SIZE = 0x10
REFERENCE_SIZE = 0x2448


def u16(data: bytes, off: int) -> int:
    return struct.unpack_from("<H", data, off)[0]


def s16(data: bytes, off: int) -> int:
    return struct.unpack_from("<h", data, off)[0]


def u32(data: bytes, off: int) -> int:
    return struct.unpack_from("<I", data, off)[0]


def c_string(data: bytes, offset: int, limit: int = 50) -> str:
    raw = data[offset:offset + limit].split(b"\0", 1)[0]
    return raw.decode("cp437", errors="replace")


def hex_preview(data: bytes, offset: int, size: int, limit: int = 32) -> str:
    chunk = data[offset:offset + min(size, limit)]
    return chunk.hex(" ") + (" ..." if size > limit else "")


def require(data: bytes, end: int, label: str) -> None:
    if end > len(data):
        raise SystemExit(f"short SAVE while reading {label}: need 0x{end:X}, file is 0x{len(data):X}")


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("save", type=Path)
    args = ap.parse_args()
    data = args.save.read_bytes()
    require(data, MIDDLE_OFFSET + 4, "fixed prefix/party/world-object count")

    print(f"file: {args.save}")
    print(f"size: {len(data)} bytes (0x{len(data):X})")
    print(f"reference quick-start size: 0x{REFERENCE_SIZE:X} (not a universal constant)")

    print("\nFixed global prefix")
    print(f"  description : {c_string(data, 0, 50)!r}")
    print(f"  event flags : {data[0x32:0x52].hex(' ')}")
    print(f"  level/map   : {u32(data, 0x52)}")
    print(f"  coordinate A: 0x{u32(data, 0x56):08X}")
    print(f"  coordinate B: 0x{u32(data, 0x5A):08X}")
    print(f"  facing      : {u32(data, 0x5E)}")
    print(f"  shared F03C : {u16(data, 0x62)}")
    print(f"  shared F03E : {u16(data, 0x64)}")
    print(f"  shared FD08 : {u32(data, 0x66)}")
    print(f"  shared 6DAE : {u16(data, 0x6A)}")
    print(f"  game clock  : {u32(data, 0x6C)}")
    print(f"  state CF4C  : 0x{u32(data, 0x70):08X}")
    print(f"  state 8600  : {s16(data, 0x74)}")
    print(f"  state 8602  : {data[0x76]}")

    print("\nActive party")
    for i in range(PARTY_COUNT):
        off = PARTY_OFFSET + i * CHARACTER_SIZE
        print(f"  party[{i}] @ 0x{off:04X}: {c_string(data, off, 16)!r}")

    count = u32(data, MIDDLE_OFFSET)
    if count > WORLD_OBJECT_LIMIT:
        raise SystemExit(f"invalid live-world-object count {count}; executable limit is {WORLD_OBJECT_LIMIT}")
    world_start = MIDDLE_OFFSET + 4
    world_end = world_start + count * WORLD_OBJECT_SIZE
    require(data, world_end, "live-world-object array")

    links_off = world_end
    container_counter_off = links_off + 8
    container_bank_off = container_counter_off + 4
    post_container_off = container_bank_off + CONTAINER_MUTABLE_PAYLOAD_SIZE
    charges_off = post_container_off + POST_CONTAINER_GLOBALS_SIZE
    roster_off = charges_off + CHARGES_SIZE
    tail_off = roster_off + ROSTER_SIZE
    expected_end = tail_off + TAIL_SIZE
    require(data, expected_end, "roster/trailing state")

    print("\nDynamic middle state")
    print(f"  live-world-object count : {count}")
    if count:
        print(f"  live-world-object array : 0x{world_start:04X}-0x{world_end-1:04X} ({count} x 0x{WORLD_OBJECT_SIZE:X})")
    else:
        print(f"  live-world-object array : empty at 0x{world_start:04X}")
    print(f"  D8A8 dwords             : 0x{links_off:04X}-0x{links_off+7:04X} [{u32(data, links_off)}, {u32(data, links_off+4)}]")
    print(f"  A7E4 state              : 0x{container_counter_off:04X} = {u32(data, container_counter_off)}")
    print(f"  {CONTAINER_STATE_COUNT}-container payload    : 0x{container_bank_off:04X}-0x{post_container_off-1:04X} (0x{CONTAINER_MUTABLE_PAYLOAD_SIZE:X} bytes)")
    print(f"  F920                    : 0x{post_container_off:04X} = {u32(data, post_container_off)}")
    print(f"  F91C                    : 0x{post_container_off+4:04X} = {u32(data, post_container_off+4)}")

    print("\nLate serializer blocks")
    print(f"  charges : 0x{charges_off:04X}-0x{charges_off+CHARGES_SIZE-1:04X} {hex_preview(data, charges_off, CHARGES_SIZE)}")
    print(f"  roster  : 0x{roster_off:04X}-0x{tail_off-1:04X}")
    for i in range(ROSTER_COUNT):
        off = roster_off + i * CHARACTER_SIZE
        print(f"    roster[{i:02d}] @ 0x{off:04X}: {c_string(data, off, 16)!r}")
    print(f"  trailing: 0x{tail_off:04X}-0x{expected_end-1:04X} {hex_preview(data, tail_off, TAIL_SIZE)}")

    if expected_end != len(data):
        print(f"WARNING: serializer-derived end is 0x{expected_end:X}; file ends at 0x{len(data):X}")
        return 1
    predicted = REFERENCE_SIZE + count * WORLD_OBJECT_SIZE
    print(f"\nserializer-derived size: 0x{expected_end:X}")
    print(f"reference-size formula : 0x{REFERENCE_SIZE:X} + {count}*0x16 = 0x{predicted:X}")
    return 0 if predicted == expected_end else 1


if __name__ == "__main__":
    raise SystemExit(main())
