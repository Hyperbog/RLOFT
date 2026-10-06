#!/usr/bin/env python3
"""Analyze the serializer-certified Ravenloft: Strahd's Possession SAVE.DAT layout."""
from __future__ import annotations

import argparse
import struct
from pathlib import Path

CHARACTER_SIZE = 0x248
PREFIX_SIZE = 0x77
PARTY_COUNT = 4
PARTY_OFFSET = PREFIX_SIZE
MIDDLE_OFFSET = PARTY_OFFSET + CHARACTER_SIZE * PARTY_COUNT
WORLD_OBJECT_SIZE = 0x16
WORLD_OBJECT_LIMIT = 0x96

# CODE.2 descriptor table 0x7330, recovered from initialized image at physical 0x7D254.
CONTAINER_SLOT_COUNTS = (
    4, 8, 8, 8, 6, 6, 8, 6, 8, 8, 4, 8, 8, 6, 8, 4,
    8, 4, 8, 8, 8, 8, 8, 8, 6, 8, 8, 4, 6, 8, 8, 8,
    8, 8, 8, 8, 8, 8, 4, 8, 8, 8, 4, 8, 8, 6, 6, 8,
    8, 8, 4, 8, 8, 8, 6, 8, 4, 6, 8, 8, 6, 8,
)
CONTAINER_STATE_COUNT = len(CONTAINER_SLOT_COUNTS)
CONTAINER_MUTABLE_PAYLOAD_SIZE = sum(2 * n + 1 for n in CONTAINER_SLOT_COUNTS)
POST_CONTAINER_GLOBALS_SIZE = 8
CHARGES_SIZE = 15
ROSTER_COUNT = 10
ROSTER_SIZE = CHARACTER_SIZE * ROSTER_COUNT
TAIL_CONTEXT_COUNT = 4
TAIL_CONTEXT_SIZE = 4
TAIL_SIZE = TAIL_CONTEXT_COUNT * TAIL_CONTEXT_SIZE
REFERENCE_SIZE = 0x2448


def u16(data: bytes, offset: int) -> int:
    return struct.unpack_from("<H", data, offset)[0]


def s16(data: bytes, offset: int) -> int:
    return struct.unpack_from("<h", data, offset)[0]


def u32(data: bytes, offset: int) -> int:
    return struct.unpack_from("<I", data, offset)[0]


def s8(value: int) -> int:
    return value - 256 if value & 0x80 else value


def c_string(data: bytes, offset: int, limit: int = 50) -> str:
    return data[offset : offset + limit].split(b"\0", 1)[0].decode("cp437", errors="replace")


def require(data: bytes, end: int, label: str) -> None:
    if end > len(data):
        raise SystemExit(f"short SAVE while reading {label}: need 0x{end:X}, file is 0x{len(data):X}")


def hex_preview(data: bytes, offset: int, size: int, limit: int = 32) -> str:
    chunk = data[offset : offset + min(size, limit)]
    return chunk.hex(" ") + (" ..." if size > limit else "")


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("save", type=Path)
    args = ap.parse_args()
    data = args.save.read_bytes()
    require(data, MIDDLE_OFFSET + 4, "fixed prefix/party/world-object count")

    print(f"file: {args.save}")
    print(f"size: {len(data)} bytes (0x{len(data):X})")
    print(f"reference quick-start size: 0x{REFERENCE_SIZE:X} (not universal)")

    print("\nFixed global prefix")
    print(f"  description      : {c_string(data, 0, 50)!r}")
    print(f"  event flags      : {data[0x32:0x52].hex(' ')}")
    print(f"  level/map        : {u32(data, 0x52)}")
    print(f"  coordinate A     : 0x{u32(data, 0x56):08X}")
    print(f"  coordinate B     : 0x{u32(data, 0x5A):08X}")
    print(f"  facing           : {u32(data, 0x5E)}")
    print(f"  shared F03C      : {u16(data, 0x62)}")
    print(f"  shared F03E      : {u16(data, 0x64)}")
    print(f"  shared FD08      : {u32(data, 0x66)}")
    print(f"  shared 6DAE      : {u16(data, 0x6A)}")
    print(f"  interrupt ticks  : {u32(data, 0x6C)}")
    print(f"  state CF4C       : 0x{u32(data, 0x70):08X}")
    print(f"  state 8600       : {s16(data, 0x74)}")
    print(f"  state 8602       : {data[0x76]}")

    print("\nActive party")
    for i in range(PARTY_COUNT):
        offset = PARTY_OFFSET + i * CHARACTER_SIZE
        print(f"  party[{i}] @ 0x{offset:04X}: {c_string(data, offset, 16)!r}")

    world_count = u32(data, MIDDLE_OFFSET)
    if world_count > WORLD_OBJECT_LIMIT:
        raise SystemExit(f"invalid live-world-object count {world_count}; executable limit is {WORLD_OBJECT_LIMIT}")
    world_start = MIDDLE_OFFSET + 4
    world_end = world_start + world_count * WORLD_OBJECT_SIZE
    require(data, world_end, "world objects")

    spell_ui_off = world_end
    gameplay_ticks_off = spell_ui_off + 8
    container_bank_off = gameplay_ticks_off + 4

    print("\nDynamic middle state")
    print(f"  live-world-object count       : {world_count}")
    if world_count:
        print(f"  live-world-object array       : 0x{world_start:04X}-0x{world_end - 1:04X}")
    else:
        print(f"  live-world-object array       : empty at 0x{world_start:04X}")
    print(
        "  spell UI selected characters : "
        f"[{u32(data, spell_ui_off)}, {u32(data, spell_ui_off + 4)}]"
    )
    print(f"  gameplay tick counter         : {u32(data, gameplay_ticks_off)}")
    print(f"  container descriptor slots    : {sum(CONTAINER_SLOT_COUNTS)}")
    print(f"  container serialized bytes    : 0x{CONTAINER_MUTABLE_PAYLOAD_SIZE:X}")

    cursor = container_bank_off
    nonempty = 0
    for state_index, slot_count in enumerate(CONTAINER_SLOT_COUNTS):
        end = cursor + 2 * slot_count + 1
        require(data, end, f"container state {state_index}")
        object_ids = [s16(data, cursor + 2 * j) for j in range(slot_count)]
        state_bits = data[cursor + 2 * slot_count]
        if any(value != -1 for value in object_ids) or state_bits:
            nonempty += 1
        print(
            f"    state[{state_index:02d}] @ 0x{cursor:04X} slots={slot_count} "
            f"ids={object_ids} bits=0x{state_bits:02X}"
        )
        cursor = end

    post_container_off = cursor
    if post_container_off - container_bank_off != CONTAINER_MUTABLE_PAYLOAD_SIZE:
        raise SystemExit("container width mismatch")
    print(f"  nonempty container states     : {nonempty}/{CONTAINER_STATE_COUNT}")
    print(
        f"  environment shading level     : 0x{post_container_off:04X} = "
        f"{u32(data, post_container_off)}"
    )
    print(
        f"  level light adjustment        : 0x{post_container_off + 4:04X} = "
        f"{u32(data, post_container_off + 4)}"
    )

    charges_off = post_container_off + POST_CONTAINER_GLOBALS_SIZE
    roster_off = charges_off + CHARGES_SIZE
    tail_off = roster_off + ROSTER_SIZE
    expected_end = tail_off + TAIL_SIZE
    require(data, expected_end, "roster/tail")

    print("\nLate serializer blocks")
    print(
        f"  charges : 0x{charges_off:04X}-0x{charges_off + CHARGES_SIZE - 1:04X} "
        f"{hex_preview(data, charges_off, CHARGES_SIZE)}"
    )
    print(f"  roster  : 0x{roster_off:04X}-0x{tail_off - 1:04X}")
    for i in range(ROSTER_COUNT):
        offset = roster_off + i * CHARACTER_SIZE
        print(f"    roster[{i:02d}] @ 0x{offset:04X}: {c_string(data, offset, 16)!r}")

    print(f"  party container UI contexts: 0x{tail_off:04X}-0x{expected_end - 1:04X}")
    for i in range(TAIL_CONTEXT_COUNT):
        offset = tail_off + i * TAIL_CONTEXT_SIZE
        subtype, state_index, phase, object_id = (s8(value) for value in data[offset : offset + 4])
        print(
            f"    context[{i}] subtype={subtype} state={state_index} "
            f"phase={phase} object_id={object_id}"
        )

    if expected_end != len(data):
        print(f"WARNING: derived end 0x{expected_end:X}, file 0x{len(data):X}")
        return 1

    predicted = REFERENCE_SIZE + world_count * WORLD_OBJECT_SIZE
    print(f"\nserializer-derived size: 0x{expected_end:X}")
    print(f"reference-size formula : 0x{REFERENCE_SIZE:X} + {world_count}*0x16 = 0x{predicted:X}")
    return 0 if predicted == expected_end else 1


if __name__ == "__main__":
    raise SystemExit(main())
