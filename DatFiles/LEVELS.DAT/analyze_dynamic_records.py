#!/usr/bin/env python3
"""Report proved dynamic 10-byte cell-record state for LEVELS/LEV files."""
from __future__ import annotations

import argparse
import collections
import struct
from pathlib import Path

PREAMBLE = 0x1D
HEADER_SIZE = 0x2A4


def u32(buf: bytes, off: int) -> int:
    return struct.unpack_from("<I", buf, off)[0]


def s16(buf: bytes, off: int) -> int:
    return struct.unpack_from("<h", buf, off)[0]


def archive_records(data: bytes) -> list[bytes]:
    count = u32(data, 0)
    offsets = struct.unpack_from(f"<{count}I", data, 4)
    return [
        data[start : offsets[i + 1] if i + 1 < count else len(data)]
        for i, start in enumerate(offsets)
    ]


def dimensions(record: bytes) -> tuple[int, int]:
    header = record[PREAMBLE : PREAMBLE + HEADER_SIZE]
    return s16(header, 1), s16(header, 3)


def legacy_lists(record: bytes) -> tuple[list[bytes], list[bytes]]:
    bounds = struct.unpack_from("<7I", record, 1)
    width, _ = dimensions(record)
    pos = bounds[1]
    first: list[bytes] = []
    second: list[bytes] = []

    for _ in range(width):
        count = s16(record, pos)
        pos += 2
        for _ in range(count):
            item = record[pos : pos + 10]
            pos += 10
            (second if item[0] == 1 else first).append(item)

    if pos != bounds[2]:
        raise ValueError("legacy list cursor mismatch")
    return first, second


def direct_lists(record: bytes) -> tuple[int, list[bytes], list[bytes]]:
    bounds = struct.unpack_from("<7I", record, 1)
    width, _ = dimensions(record)
    pos = bounds[1]
    groups: list[list[bytes]] = []
    active_count = 0

    for group_index in range(2):
        items: list[bytes] = []
        for _ in range(width):
            count = s16(record, pos)
            pos += 2
            for _ in range(count):
                items.append(record[pos : pos + 10])
                pos += 10
        groups.append(items)
        if group_index == 0:
            active_count = record[pos]
            pos += 1

    if pos != bounds[2]:
        raise ValueError("direct list cursor mismatch")
    return active_count, groups[0], groups[1]


def signed_byte(value: int) -> int:
    return value - 0x100 if value & 0x80 else value


def summarize(items: list[bytes], label: str) -> None:
    positions = collections.Counter(item[6] for item in items)
    deltas = collections.Counter(signed_byte(item[7]) for item in items)
    modes = collections.Counter(item[1] & 0x03 for item in items)
    flags = collections.Counter(item[1] for item in items)

    print(f"{label}: records={len(items)}")
    print("  byte6_position=" + repr(dict(sorted(positions.items()))))
    print("  byte7_delta=" + repr(dict(sorted(deltas.items()))))
    print("  byte1_low2=" + repr(dict(sorted(modes.items()))))
    print("  byte1_full=" + repr(dict(sorted(flags.items()))))


def inspect_archive(path: Path) -> None:
    first_all: list[bytes] = []
    second_all: list[bytes] = []
    records = archive_records(path.read_bytes())

    for record in records:
        first, second = legacy_lists(record)
        first_all.extend(first)
        second_all.extend(second)

    print(f"{path.name}: levels={len(records)}")
    summarize(first_all, "runtime-group-1 candidates")
    summarize(second_all, "runtime-group-2 candidates (legacy byte0 == 1)")


def inspect_direct(path: Path) -> None:
    active_count, first, second = direct_lists(path.read_bytes())
    print(f"{path.name}: active_animation_count={active_count}")
    summarize(first, "runtime-group-1")
    summarize(second, "runtime-group-2")


def main() -> None:
    parser = argparse.ArgumentParser()
    sub = parser.add_subparsers(dest="mode", required=True)

    archive = sub.add_parser("archive")
    archive.add_argument("path", type=Path)

    direct = sub.add_parser("direct")
    direct.add_argument("path", type=Path)

    args = parser.parse_args()
    if args.mode == "archive":
        inspect_archive(args.path)
    else:
        inspect_direct(args.path)


if __name__ == "__main__":
    main()
