#!/usr/bin/env python3
"""Analyze the proven 50 x 26-byte LEVELS event-script table."""
from __future__ import annotations

import argparse
import collections
import struct
from pathlib import Path

PREAMBLE = 0x1D
HEADER_SIZE = 0x2A4
SCRIPT_BLOCK_SIZE = 0x514
SLOT_SIZE = 0x1A
SLOT_COUNT = 50
COMMAND_SIZE = 5
COMMAND_CAPACITY = 5


def u32(buf: bytes, off: int) -> int:
    return struct.unpack_from("<I", buf, off)[0]


def archive_records(data: bytes) -> list[bytes]:
    count = u32(data, 0)
    offsets = struct.unpack_from(f"<{count}I", data, 4)
    return [
        data[start : offsets[i + 1] if i + 1 < count else len(data)]
        for i, start in enumerate(offsets)
    ]


def script_block(record: bytes) -> bytes:
    bounds = struct.unpack_from("<7I", record, 1)
    block = record[bounds[5] : bounds[6]]
    if len(block) != SCRIPT_BLOCK_SIZE:
        raise ValueError(f"script block size {len(block):#x} != 0x514")
    return block


def iter_slots(block: bytes):
    for slot_index in range(SLOT_COUNT):
        start = slot_index * SLOT_SIZE
        yield slot_index, block[start : start + SLOT_SIZE]


def iter_active_commands(slot: bytes):
    count = slot[0]
    if count > COMMAND_CAPACITY:
        raise ValueError(f"active count {count} exceeds {COMMAND_CAPACITY}")
    for command_index in range(count):
        start = 1 + command_index * COMMAND_SIZE
        cmd = slot[start : start + COMMAND_SIZE]
        yield command_index, cmd


def analyze(path: Path) -> None:
    records = archive_records(path.read_bytes())
    active_slots = 0
    commands = 0
    opcode_counts = collections.Counter()
    inactive_nonzero = []

    for level_id, record in enumerate(records):
        block = script_block(record)
        for slot_index, slot in iter_slots(block):
            count = slot[0]
            if count > COMMAND_CAPACITY:
                raise ValueError(
                    f"level {level_id} slot {slot_index}: count {count} > 5"
                )
            if count:
                active_slots += 1
            for _, cmd in iter_active_commands(slot):
                opcode = cmd[0]
                if opcode > 12:
                    raise ValueError(
                        f"level {level_id} slot {slot_index}: active opcode {opcode} > 12"
                    )
                opcode_counts[opcode] += 1
                commands += 1

            inactive_start = 1 + count * COMMAND_SIZE
            if any(slot[inactive_start:]):
                inactive_nonzero.append((level_id, slot_index, count, slot[inactive_start:]))

    print(f"levels={len(records)}")
    print(f"physical_slots={len(records) * SLOT_COUNT}")
    print(f"active_slots={active_slots}")
    print(f"active_commands={commands}")
    print("opcode_counts=" + " ".join(
        f"{opcode}:{opcode_counts[opcode]}" for opcode in range(13)
    ))
    print(f"slots_with_nonzero_inactive_tail={len(inactive_nonzero)}")
    for level_id, slot_index, count, tail in inactive_nonzero:
        print(
            f"  level={level_id} slot={slot_index} active_count={count} "
            f"inactive_tail={tail.hex(' ')}"
        )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("levels", type=Path)
    args = parser.parse_args()
    analyze(args.levels)


if __name__ == "__main__":
    main()
