#!/usr/bin/env python3
"""Structural validator for Ravenloft LEVELS.DAT and standalone LEVnnn.DAT files.

Field semantics are intentionally conservative. Names tied to CODE.2 DGROUP addresses
remain address-based until their gameplay meaning is separately proved.
"""
from __future__ import annotations

import argparse
import hashlib
import struct
from dataclasses import dataclass
from pathlib import Path
from typing import List, Tuple

PREAMBLE_SIZE = 0x1D
BASE_HEADER_SIZE = 0x2A4
TAIL_BLOB_SIZE = 0x514
LEGACY_RECORD_SIZE = 0x0B
DIRECT_RECORD_SIZE = 0x16


def u32(buf: bytes, off: int) -> int:
    return struct.unpack_from("<I", buf, off)[0]


def s16(buf: bytes, off: int) -> int:
    return struct.unpack_from("<h", buf, off)[0]


@dataclass(frozen=True)
class CoreHeader:
    width: int
    height: int
    count_258a2: int
    count_258a4: int
    count_258a6: int


@dataclass(frozen=True)
class RecordLayout:
    marker: int
    bounds: Tuple[int, int, int, int, int, int, int]
    core: CoreHeader
    tail_text_length: int


def parse_preamble(record: bytes) -> Tuple[int, Tuple[int, ...]]:
    if len(record) < PREAMBLE_SIZE:
        raise ValueError("record shorter than 0x1D-byte preamble")
    marker = record[0]
    bounds = struct.unpack_from("<7I", record, 1)
    if bounds[0] != PREAMBLE_SIZE:
        raise ValueError(f"first boundary is {bounds[0]:#x}, expected 0x1D")
    if list(bounds) != sorted(bounds):
        raise ValueError(f"non-monotonic preamble boundaries: {bounds}")
    return marker, bounds


def parse_core(record: bytes, start: int) -> CoreHeader:
    end = start + BASE_HEADER_SIZE
    if end > len(record):
        raise ValueError("truncated 0x2A4-byte base header")
    h = record[start:end]
    return CoreHeader(
        width=s16(h, 0x001),
        height=s16(h, 0x003),
        count_258a2=s16(h, 0x28A),
        count_258a4=s16(h, 0x28C),
        count_258a6=s16(h, 0x28E),
    )


def require_nonnegative(core: CoreHeader) -> None:
    values = (
        ("width", core.width),
        ("height", core.height),
        ("count_258a2", core.count_258a2),
        ("count_258a4", core.count_258a4),
        ("count_258a6", core.count_258a6),
    )
    for name, value in values:
        if value < 0:
            raise ValueError(f"negative {name}: {value}")


def parse_column_lists(record: bytes, start: int, width: int) -> Tuple[int, List[int]]:
    pos = start
    counts: List[int] = []
    for column in range(width):
        if pos + 2 > len(record):
            raise ValueError(f"truncated column count at column {column}")
        count = s16(record, pos)
        pos += 2
        if count < 0:
            raise ValueError(f"negative column count {count} at column {column}")
        counts.append(count)
        pos += count * 10
        if pos > len(record):
            raise ValueError(f"column {column} records run past EOF")
    return pos, counts


def validate_tail(record: bytes, blob_end: int, core: CoreHeader) -> int:
    pos = blob_end
    if pos + 4 > len(record):
        raise ValueError("missing duplicated width/height tail words")
    tail_width, tail_height = struct.unpack_from("<hh", record, pos)
    pos += 4
    if (tail_width, tail_height) != (core.width, core.height):
        raise ValueError(
            "tail dimensions disagree with base header: "
            f"{tail_width}x{tail_height} vs {core.width}x{core.height}"
        )
    pos += core.width * core.height
    if pos + 4 > len(record):
        raise ValueError("missing tail text length")
    text_length = u32(record, pos)
    pos += 4
    end = pos + text_length
    if end != len(record):
        raise ValueError(
            f"tail ends at {end:#x}, record/file ends at {len(record):#x}"
        )
    return text_length


def validate_legacy_record(record: bytes) -> RecordLayout:
    marker, bounds = parse_preamble(record)
    core = parse_core(record, bounds[0])
    require_nonnegative(core)
    expected_grid_end = bounds[0] + BASE_HEADER_SIZE + 2 * core.width * core.height
    if bounds[1] != expected_grid_end:
        raise ValueError(
            f"grid boundary {bounds[1]:#x} != expected {expected_grid_end:#x}"
        )
    dynamic_end, _ = parse_column_lists(record, bounds[1], core.width)
    if dynamic_end != bounds[2]:
        raise ValueError(
            f"legacy dynamic-list end {dynamic_end:#x} != boundary {bounds[2]:#x}"
        )
    if bounds[3] - bounds[2] != core.count_258a2 * LEGACY_RECORD_SIZE:
        raise ValueError("legacy family at 0x258A2 does not match 11-byte count")
    if bounds[4] - bounds[3] != core.count_258a6 * LEGACY_RECORD_SIZE:
        raise ValueError("legacy family at 0x258A6 does not match 11-byte count")
    if bounds[5] - bounds[4] != core.count_258a4 * LEGACY_RECORD_SIZE:
        raise ValueError("legacy family at 0x258A4 does not match 11-byte count")
    if bounds[6] - bounds[5] != TAIL_BLOB_SIZE:
        raise ValueError("legacy 0x514-byte block boundary mismatch")
    text_length = validate_tail(record, bounds[6], core)
    return RecordLayout(marker, bounds, core, text_length)


def validate_direct_record(record: bytes) -> RecordLayout:
    marker, bounds = parse_preamble(record)
    core = parse_core(record, bounds[0])
    require_nonnegative(core)
    expected_grid_end = bounds[0] + BASE_HEADER_SIZE + 2 * core.width * core.height
    if bounds[1] != expected_grid_end:
        raise ValueError(
            f"grid boundary {bounds[1]:#x} != expected {expected_grid_end:#x}"
        )
    pos, _ = parse_column_lists(record, bounds[1], core.width)
    if pos >= len(record):
        raise ValueError("missing direct-save single-byte field between column groups")
    pos += 1
    pos, _ = parse_column_lists(record, pos, core.width)
    if pos != bounds[2]:
        raise ValueError(
            f"direct dynamic-list end {pos:#x} != boundary {bounds[2]:#x}"
        )
    if bounds[3] - bounds[2] != core.count_258a2 * DIRECT_RECORD_SIZE:
        raise ValueError("direct family at 0x258A2 does not match 22-byte count")
    expected_second = 2 + core.count_258a6 * DIRECT_RECORD_SIZE
    if bounds[4] - bounds[3] != expected_second:
        raise ValueError("direct family at 0x258A6 does not match count-word + 22-byte records")
    serialized_count = s16(record, bounds[3])
    if serialized_count != core.count_258a6:
        raise ValueError(
            f"serialized 0x258A6 count {serialized_count} != header count {core.count_258a6}"
        )
    if bounds[5] - bounds[4] != core.count_258a4 * DIRECT_RECORD_SIZE:
        raise ValueError("direct family at 0x258A4 does not match 22-byte count")
    if bounds[6] - bounds[5] != TAIL_BLOB_SIZE:
        raise ValueError("direct 0x514-byte block boundary mismatch")
    text_length = validate_tail(record, bounds[6], core)
    return RecordLayout(marker, bounds, core, text_length)


def inspect_archive(path: Path) -> None:
    data = path.read_bytes()
    if len(data) < 4:
        raise ValueError("archive too short")
    count = u32(data, 0)
    directory_end = 4 + count * 4
    if directory_end > len(data):
        raise ValueError("directory runs past EOF")
    offsets = list(struct.unpack_from(f"<{count}I", data, 4))
    if not offsets:
        raise ValueError("empty archive")
    if offsets[0] != directory_end:
        raise ValueError(
            f"first record starts at {offsets[0]:#x}, directory ends at {directory_end:#x}"
        )
    if offsets != sorted(offsets) or len(set(offsets)) != len(offsets):
        raise ValueError("archive offsets are not strictly increasing")
    print(f"{path.name}: {len(data)} bytes, sha256={hashlib.sha256(data).hexdigest()}")
    print(f"records={count}, directory_size={directory_end:#x}")
    print("id  file_off  size    map      counts(258A2/258A6/258A4)  text")
    for i, start in enumerate(offsets):
        end = offsets[i + 1] if i + 1 < count else len(data)
        record = data[start:end]
        layout = validate_legacy_record(record)
        c = layout.core
        print(
            f"{i:02d}  {start:08x}  {len(record):06x}  "
            f"{c.width:02d}x{c.height:<2d}  "
            f"{c.count_258a2:4d}/{c.count_258a6:4d}/{c.count_258a4:4d}"
            f"                 {layout.tail_text_length}"
        )
    print("archive structural validation: PASS")


def inspect_direct(path: Path) -> None:
    data = path.read_bytes()
    layout = validate_direct_record(data)
    c = layout.core
    print(f"{path.name}: {len(data)} bytes, sha256={hashlib.sha256(data).hexdigest()}")
    print(f"marker_byte={layout.marker:#04x} (loader does not consume byte 0)")
    print("boundaries=" + ",".join(f"{v:#x}" for v in layout.bounds))
    print(
        f"map={c.width}x{c.height}, counts(258A2/258A6/258A4)="
        f"{c.count_258a2}/{c.count_258a6}/{c.count_258a4}, "
        f"tail_text_length={layout.tail_text_length}"
    )
    print("standalone structural validation: PASS")


def main() -> None:
    parser = argparse.ArgumentParser()
    sub = parser.add_subparsers(dest="mode", required=True)
    p_archive = sub.add_parser("archive", help="validate LEVELS.DAT")
    p_archive.add_argument("path", type=Path)
    p_direct = sub.add_parser("direct", help="validate a standalone LEVnnn.DAT")
    p_direct.add_argument("path", type=Path)
    args = parser.parse_args()
    if args.mode == "archive":
        inspect_archive(args.path)
    else:
        inspect_direct(args.path)


if __name__ == "__main__":
    main()
