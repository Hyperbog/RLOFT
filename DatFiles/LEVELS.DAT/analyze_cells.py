#!/usr/bin/env python3
"""Inspect proven 16-bit Ravenloft LEVELS/LEV cell-state semantics.

The labels here are deliberately behavioral. Only tile_code, current_visible,
and automap_explored have sufficiently direct CODE.2 evidence for semantic names.
Other flags remain named by observed engine behavior rather than guessed gameplay
concepts.
"""
from __future__ import annotations

import argparse
import collections
import struct
from pathlib import Path
from typing import Iterable, Sequence

PREAMBLE_SIZE = 0x1D
BASE_HEADER_SIZE = 0x2A4

# Direct CODE.2 consumers of DGROUP 0x198E0.
FLAG_FIELDS = (
    (0x0040, "current_visible"),
    (0x0080, "special_cell_gate"),
    (0x0100, "neighbor_substitution_gate"),
    (0x0200, "render_attribute"),
    (0x0400, "automap_explored"),
    (0x0800, "conditional_passthrough"),
    (0x1000, "cell_lookup_augmentation"),
    (0x2000, "unresolved_2000"),
    (0x4000, "render_selector_bit0"),
    (0x8000, "render_selector_bit1"),
)


def s16(buf: bytes, off: int) -> int:
    return struct.unpack_from("<h", buf, off)[0]


def u32(buf: bytes, off: int) -> int:
    return struct.unpack_from("<I", buf, off)[0]


def parse_bounds(record: bytes) -> tuple[int, ...]:
    if len(record) < PREAMBLE_SIZE:
        raise ValueError("record shorter than common preamble")
    bounds = struct.unpack_from("<7I", record, 1)
    if bounds[0] != PREAMBLE_SIZE:
        raise ValueError(f"unexpected boundary_0 {bounds[0]:#x}")
    return bounds


def dimensions(record: bytes, bounds: Sequence[int]) -> tuple[int, int]:
    header = record[bounds[0] : bounds[0] + BASE_HEADER_SIZE]
    if len(header) != BASE_HEADER_SIZE:
        raise ValueError("truncated base header")
    width = s16(header, 1)
    height = s16(header, 3)
    if width < 0 or height < 0:
        raise ValueError(f"negative dimensions {width}x{height}")
    return width, height


def cell_words(record: bytes) -> tuple[int, int, tuple[int, ...]]:
    bounds = parse_bounds(record)
    width, height = dimensions(record, bounds)
    start = bounds[0] + BASE_HEADER_SIZE
    count = width * height
    end = start + count * 2
    if end != bounds[1]:
        raise ValueError(
            f"16-bit layer end {end:#x} != boundary_1 {bounds[1]:#x}"
        )
    words = struct.unpack_from(f"<{count}H", record, start)
    return width, height, words


def tail_byte_grid(record: bytes) -> tuple[int, int, bytes]:
    bounds = parse_bounds(record)
    width, height = dimensions(record, bounds)
    start = bounds[6]
    if start + 4 > len(record):
        raise ValueError("truncated tail dimensions")
    tail_width, tail_height = struct.unpack_from("<hh", record, start)
    if (tail_width, tail_height) != (width, height):
        raise ValueError("tail dimensions differ from header")
    start += 4
    end = start + width * height
    if end > len(record):
        raise ValueError("truncated final byte layer")
    return width, height, record[start:end]


def archive_records(data: bytes) -> list[bytes]:
    if len(data) < 4:
        raise ValueError("archive too short")
    count = u32(data, 0)
    directory_end = 4 + count * 4
    if directory_end > len(data):
        raise ValueError("archive directory runs past EOF")
    offsets = list(struct.unpack_from(f"<{count}I", data, 4))
    if not offsets:
        return []
    if offsets[0] != directory_end:
        raise ValueError("first record does not follow archive directory")
    if offsets != sorted(offsets) or len(offsets) != len(set(offsets)):
        raise ValueError("archive offsets are not strictly increasing")
    return [
        data[start : offsets[i + 1] if i + 1 < count else len(data)]
        for i, start in enumerate(offsets)
    ]


def count_fields(words: Iterable[int]) -> tuple[int, collections.Counter, collections.Counter]:
    words = tuple(words)
    low6 = collections.Counter(word & 0x003F for word in words)
    flags = collections.Counter()
    for word in words:
        for mask, name in FLAG_FIELDS:
            if word & mask:
                flags[name] += 1
    return len(words), low6, flags


def print_field_counts(words: Iterable[int]) -> None:
    total, low6, flags = count_fields(words)
    nonzero_codes = total - low6[0]
    print(f"cells={total}")
    print(f"tile_code_nonzero={nonzero_codes}")
    for mask, name in FLAG_FIELDS:
        print(f"{name:28s} mask={mask:#06x} set={flags[name]}")


def inspect_archive(path: Path) -> None:
    records = archive_records(path.read_bytes())
    all_words: list[int] = []
    print(f"{path.name}: records={len(records)}")
    for level_id, record in enumerate(records):
        width, height, words = cell_words(record)
        all_words.extend(words)
        _, _, flags = count_fields(words)
        print(
            f"{level_id:02d} {width:02d}x{height:<2d} "
            f"code={sum((w & 0x3F) != 0 for w in words):4d} "
            f"vis={flags['current_visible']:4d} "
            f"explored={flags['automap_explored']:4d}"
        )
    print("-- aggregate --")
    print_field_counts(all_words)


def inspect_direct(path: Path) -> None:
    data = path.read_bytes()
    width, height, words = cell_words(data)
    _, _, final_grid = tail_byte_grid(data)
    print(f"{path.name}: {width}x{height}")
    print_field_counts(words)
    print("final_byte_grid=" + " ".join(
        f"{value:#04x}:{count}"
        for value, count in sorted(collections.Counter(final_grid).items())
    ))


def correlate(archive: Path, direct: Path, level_id: int) -> None:
    records = archive_records(archive.read_bytes())
    if not (0 <= level_id < len(records)):
        raise ValueError(f"level {level_id} outside archive")
    _, _, before = cell_words(records[level_id])
    _, _, after = cell_words(direct.read_bytes())
    if len(before) != len(after):
        raise ValueError("archive/direct cell counts differ")

    print(f"level={level_id} cells={len(before)}")
    for mask, name in FLAG_FIELDS:
        b = sum(bool(word & mask) for word in before)
        a = sum(bool(word & mask) for word in after)
        changed = sum(bool(x & mask) != bool(y & mask) for x, y in zip(before, after))
        print(f"{name:28s} {b:5d} -> {a:5d} changed={changed:5d}")

    code_changed = sum((x & 0x003F) != (y & 0x003F) for x, y in zip(before, after))
    print(f"{'tile_code':28s} changed={code_changed:5d}")

    _, _, final_grid = tail_byte_grid(direct.read_bytes())
    explored = sum(bool(word & 0x0400) for word in after)
    visible = sum(bool(word & 0x0040) for word in after)
    print(f"saved current_visible={visible}")
    print(f"saved automap_explored={explored}")
    print("final_byte_grid=" + " ".join(
        f"{value:#04x}:{count}"
        for value, count in sorted(collections.Counter(final_grid).items())
    ))


def main() -> None:
    parser = argparse.ArgumentParser()
    sub = parser.add_subparsers(dest="mode", required=True)

    p_archive = sub.add_parser("archive")
    p_archive.add_argument("path", type=Path)

    p_direct = sub.add_parser("direct")
    p_direct.add_argument("path", type=Path)

    p_corr = sub.add_parser("correlate")
    p_corr.add_argument("archive", type=Path)
    p_corr.add_argument("direct", type=Path)
    p_corr.add_argument("level_id", type=int)

    args = parser.parse_args()
    if args.mode == "archive":
        inspect_archive(args.path)
    elif args.mode == "direct":
        inspect_direct(args.path)
    else:
        correlate(args.archive, args.direct, args.level_id)


if __name__ == "__main__":
    main()
