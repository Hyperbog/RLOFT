#!/usr/bin/env python3
"""Validate Ravenloft RES type-7 compiled scene-shape display lists."""
from __future__ import annotations

import argparse
import struct
from collections import Counter
from pathlib import Path

from analyze_res import lzss, parse_res


def validate_body(body: bytes) -> dict[str, int]:
    if len(body) < 10:
        raise ValueError("decoded body shorter than 10-byte geometry header")

    source_width, x_min, x_max, y_min, y_max = struct.unpack_from("<Hhhhh", body, 0)

    if source_width <= 0 or source_width & 1:
        raise ValueError(f"source_width is not positive/even: {source_width}")
    if not (0 <= x_min <= x_max < source_width):
        raise ValueError(f"bad x bounds {x_min}..{x_max} for width {source_width}")
    if not (0 <= y_min <= y_max <= 128):
        raise ValueError(f"bad y bounds {y_min}..{y_max}")

    column_count = x_max - x_min + 1
    table_end = 10 + 2 * column_count
    if table_end > len(body):
        raise ValueError("column-pointer table exceeds decoded body")

    offsets = struct.unpack_from(f"<{column_count}H", body, 10)
    descriptor_count = 0

    for off in set(offsets):
        if off < table_end or off >= len(body):
            raise ValueError(
                f"column pointer {off} outside list region {table_end}..{len(body)-1}"
            )

        pos = off
        while True:
            if pos + 2 > len(body):
                raise ValueError("list terminator exceeds decoded EOF")

            stop_selector = struct.unpack_from("<h", body, pos)[0]
            if stop_selector == 0:
                pos += 2
                break

            if pos + 6 > len(body):
                raise ValueError("6-byte descriptor exceeds decoded EOF")

            stop_selector, signed_adjustment, entry_selector = struct.unpack_from(
                "<hhh", body, pos
            )
            if not (0 <= stop_selector <= 0x0100 and 0 <= entry_selector <= 0x0100):
                raise ValueError(
                    f"renderer selector outside 0x0000..0x0100 stop={stop_selector} entry={entry_selector}"
                )
            if stop_selector & 1 or entry_selector & 1:
                raise ValueError(
                    f"odd renderer selector stop={stop_selector} entry={entry_selector}"
                )

            descriptor_count += 1
            pos += 6

    return {
        "source_width": source_width,
        "x_min": x_min,
        "x_max": x_max,
        "y_min": y_min,
        "y_max": y_max,
        "column_count": column_count,
        "unique_column_lists": len(set(offsets)),
        "descriptor_count": descriptor_count,
    }


def inspect_archive(path: Path) -> tuple[int, Counter[int], int, int]:
    _, entries = parse_res(path.read_bytes())
    count = 0
    widths: Counter[int] = Counter()
    descriptors = 0
    unique_lists = 0

    for index, entry in enumerate(entries):
        if len(entry) < 7 or entry[:2] != b"EH" or entry[2] != 7:
            continue

        decoded_size = struct.unpack_from("<H", entry, 5)[0]
        body, used = lzss(entry[7:], decoded_size)
        if used != len(entry) - 7:
            raise ValueError(
                f"{path.name}:{index}: compressed stream has {len(entry)-7-used} trailing bytes"
            )

        try:
            shape = validate_body(body)
        except ValueError as exc:
            raise ValueError(f"{path.name}:{index}: {exc}") from exc

        count += 1
        widths[shape["source_width"]] += 1
        descriptors += shape["descriptor_count"]
        unique_lists += shape["unique_column_lists"]

    return count, widths, descriptors, unique_lists


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("files", nargs="+", type=Path)
    args = parser.parse_args()

    total = 0
    widths: Counter[int] = Counter()
    descriptors = 0
    unique_lists = 0

    for path in args.files:
        count, local_widths, local_descriptors, local_lists = inspect_archive(path)
        total += count
        widths.update(local_widths)
        descriptors += local_descriptors
        unique_lists += local_lists
        print(
            f"{path.name}: type7={count} widths={dict(sorted(local_widths.items()))} "
            f"descriptors={local_descriptors} unique_lists={local_lists}"
        )

    print(
        f"TOTAL: type7={total} widths={dict(sorted(widths.items()))} "
        f"descriptors={descriptors} unique_lists={unique_lists}"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
