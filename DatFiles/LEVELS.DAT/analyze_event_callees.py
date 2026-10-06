#!/usr/bin/env python3
"""Audit event-opcode callee encodings and cross-DAT links."""
from __future__ import annotations

import argparse
import collections
import struct
from pathlib import Path

SCRIPT_BLOCK_SIZE = 0x514
SLOT_SIZE = 26
SLOT_COUNT = 50
COMMAND_SIZE = 5


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


def analyze(levels_path: Path, info_path: Path | None) -> None:
    records = archive_records(levels_path.read_bytes())
    music_ids = collections.Counter()
    rows: dict[int, list[tuple[int, int, int, bytes, int | None]]] = collections.defaultdict(list)

    for level_id, record in enumerate(records):
        header = record[0x1D : 0x1D + 0x2A4]
        music_ids[header[0x29C]] += 1

        block = script_block(record)
        for slot_index in range(SLOT_COUNT):
            slot_start = slot_index * SLOT_SIZE
            active_count = block[slot_start]
            if active_count > 5:
                raise ValueError(
                    f"level {level_id} slot {slot_index}: active count {active_count} > 5"
                )
            for command_index in range(active_count):
                command_start = slot_start + 1 + command_index * COMMAND_SIZE
                command = block[command_start : command_start + COMMAND_SIZE]
                opcode = command[0]
                if opcode not in (5, 6, 7, 8, 9, 10, 11):
                    continue
                unaligned = None
                if opcode in (5, 6):
                    # CODE.2 reads a dword beginning at physical command byte +3.
                    unaligned = struct.unpack_from("<I", block, command_start + 3)[0]
                rows[opcode].append(
                    (level_id, slot_index, command_index, command, unaligned)
                )

    print(f"levels={len(records)}")
    print(
        "level_music_ids="
        + " ".join(f"{value}:{count}" for value, count in sorted(music_ids.items()))
    )

    for opcode in (5, 6, 7, 8, 9, 10, 11):
        matches = rows[opcode]
        print(f"opcode_{opcode}_count={len(matches)}")
        if opcode in (5, 6):
            values = sorted({value for *_, value in matches if value is not None})
            print(
                f"opcode_{opcode}_unaligned_dwords="
                + ",".join(hex(value) for value in values)
            )
        if opcode in (8, 9, 10, 11):
            print(
                f"opcode_{opcode}_arg3="
                + ",".join(hex(command[3]) for *_, command, _ in matches)
            )
        for level_id, slot_index, command_index, command, unaligned in matches:
            suffix = "" if unaligned is None else f" unaligned_dword={unaligned:#x}"
            print(
                f"  level={level_id} slot={slot_index} command={command_index} "
                f"bytes={command.hex(' ')}{suffix}"
            )

    if info_path is not None:
        info = info_path.read_bytes()
        if len(info) < 15:
            raise ValueError("INFO.DAT shorter than 15-byte fixed prefix")
        path_length = struct.unpack_from("<H", info, 13)[0]
        path_bytes = info[15 : 15 + path_length]
        if len(path_bytes) != path_length:
            raise ValueError("INFO.DAT resource path is truncated")
        print(f"info_config13={info[:13].decode('ascii', errors='replace')}")
        print(f"info_path_length={path_length}")
        print(f"info_resource_path={path_bytes.decode('latin1')}")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("levels", type=Path)
    parser.add_argument("--info", type=Path)
    args = parser.parse_args()
    analyze(args.levels, args.info)


if __name__ == "__main__":
    main()
