#!/usr/bin/env python3
"""Replay the ordinary-cell portion of CODE.2 01558 against a LEVnnn.DAT.

This is an audit tool, not a replacement implementation. It intentionally rejects
cells using the 0x0080 special-record path, because that branch depends on the
runtime 10-byte record lists and is documented separately.

For the supplied LEV022.DAT there are no 0x0080 cells, so the replay covers all
450 cells. The tool reports both the exact certified CODE.2 branch behavior and
a diagnostic variant that suppresses only the unexplored-cell 0x0093 shortcut.
The latter exists solely to characterize the supplied save discrepancy.
"""
from __future__ import annotations

import argparse
import collections
import struct
from pathlib import Path
from typing import Sequence

PREAMBLE_SIZE = 0x1D
HEADER_SIZE = 0x2A4
CELL_TABLE_OFFSET = 0x0A  # DGROUP 0x25622 relative to 0x25618 header base
MODE_OFFSET = 0x290       # DGROUP 0x258A8 relative to 0x25618


def bounds(record: bytes) -> tuple[int, ...]:
    if len(record) < PREAMBLE_SIZE:
        raise ValueError("record shorter than common preamble")
    result = struct.unpack_from("<7I", record, 1)
    if result[0] != PREAMBLE_SIZE:
        raise ValueError(f"unexpected boundary_0 {result[0]:#x}")
    return result


def parse(record: bytes) -> tuple[int, int, bytes, tuple[int, ...], bytes]:
    b = bounds(record)
    header = record[b[0] : b[0] + HEADER_SIZE]
    if len(header) != HEADER_SIZE:
        raise ValueError("truncated 0x2A4 header")

    width, height = struct.unpack_from("<hh", header, 1)
    if width <= 0 or height <= 0:
        raise ValueError(f"invalid dimensions {width}x{height}")

    count = width * height
    cell_start = b[0] + HEADER_SIZE
    cell_end = cell_start + count * 2
    if cell_end != b[1]:
        raise ValueError("cell layer does not terminate at boundary_1")
    cells = struct.unpack_from(f"<{count}H", record, cell_start)

    tail = b[6]
    tail_width, tail_height = struct.unpack_from("<hh", record, tail)
    if (tail_width, tail_height) != (width, height):
        raise ValueError("tail dimensions do not match header")
    grid = record[tail + 4 : tail + 4 + count]
    if len(grid) != count:
        raise ValueError("truncated final byte grid")

    return width, height, header, cells, grid


def neighbor_has_explored_raw_zero(
    cells: Sequence[int], width: int, height: int, row: int, column: int
) -> bool:
    # Same eight positions tested by 016C8..01804.
    for dr, dc in (
        (-1, -1),
        (0, -1),
        (1, -1),
        (-1, 0),
        (1, 0),
        (-1, 1),
        (0, 1),
        (1, 1),
    ):
        rr = row + dr
        cc = column + dc
        if not (0 <= rr < height and 0 <= cc < width):
            continue
        word = cells[rr * width + cc]
        if (word & 0x0400) and (word & 0x003F) == 0:
            return True
    return False


def replay(
    width: int,
    height: int,
    header: bytes,
    cells: Sequence[int],
    *,
    suppress_unexplored_0093: bool,
) -> bytes:
    table = struct.unpack_from("<64h", header, CELL_TABLE_OFFSET)
    mode = header[MODE_OFFSET]
    common_class = 0x2F if mode == 0xFF else 0x2E

    if any(word & 0x0080 for word in cells):
        raise ValueError(
            "0x0080 special-cell path present; ordinary replay is incomplete for this level"
        )

    out = bytearray([0xFF] * (width * height))

    # CODE.2 loops column outside and row inside, then computes row*width+column.
    for column in range(width):
        for row in range(height):
            index = row * width + column
            word = cells[index]
            tile_code = word & 0x003F
            explored = bool(word & 0x0400)
            value = 0xFF

            if explored:
                if tile_code == 0 or table[tile_code] == 0x0093:
                    value = common_class
                elif word & 0x0800:
                    value = 0x05
                else:
                    # 0x0080 was rejected above, so this is the encoded 01804 path.
                    value = 0x01
            elif tile_code != 0:
                if table[tile_code] == 0x0093 and not suppress_unexplored_0093:
                    value = common_class
                elif neighbor_has_explored_raw_zero(
                    cells, width, height, row, column
                ):
                    value = 0x01

            out[index] = value

    return bytes(out)


def summarize(label: str, predicted: bytes, actual: bytes) -> None:
    mismatches = [i for i, (a, b) in enumerate(zip(predicted, actual)) if a != b]
    print(f"{label}: mismatches={len(mismatches)}")
    print(
        "  predicted="
        + " ".join(
            f"{value:#04x}:{count}"
            for value, count in sorted(collections.Counter(predicted).items())
        )
    )
    if mismatches:
        pairs = collections.Counter((predicted[i], actual[i]) for i in mismatches)
        print(
            "  mismatch_pairs="
            + " ".join(
                f"{a:#04x}->{b:#04x}:{count}"
                for (a, b), count in sorted(pairs.items())
            )
        )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("path", type=Path)
    args = parser.parse_args()

    record = args.path.read_bytes()
    width, height, header, cells, actual = parse(record)
    table = struct.unpack_from("<64h", header, CELL_TABLE_OFFSET)

    print(f"{args.path.name}: {width}x{height} cells={width * height}")
    print(f"table_25622[1]={table[1]:#06x}")
    print(
        "actual="
        + " ".join(
            f"{value:#04x}:{count}"
            for value, count in sorted(collections.Counter(actual).items())
        )
    )

    exact = replay(
        width, height, header, cells, suppress_unexplored_0093=False
    )
    summarize("certified_CODE2_01558", exact, actual)

    diagnostic = replay(
        width, height, header, cells, suppress_unexplored_0093=True
    )
    summarize("diagnostic_without_unexplored_0093", diagnostic, actual)


if __name__ == "__main__":
    main()
