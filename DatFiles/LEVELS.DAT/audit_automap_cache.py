#!/usr/bin/env python3
"""Audit deterministic CODE.2 01558 automap-cache rules against a LEV file.

This intentionally does not emulate special 10-byte-record classes or party-state
selection. It reports only cases whose output class is determined without those
unknowns.
"""
from __future__ import annotations

import argparse
import collections
import struct
from pathlib import Path

PREAMBLE = 0x1D
HEADER_SIZE = 0x2A4
PRIMARY_TABLE_OFF = 0x00A
PRIMARY2_TABLE_OFF = 0x10A


def s16(buf: bytes, off: int) -> int:
    return struct.unpack_from("<h", buf, off)[0]


def parse_lev(data: bytes):
    if len(data) < PREAMBLE + HEADER_SIZE:
        raise ValueError("file too short")
    bounds = struct.unpack_from("<7I", data, 1)
    if bounds[0] != PREAMBLE:
        raise ValueError(f"unexpected payload start {bounds[0]:#x}")

    header = data[PREAMBLE : PREAMBLE + HEADER_SIZE]
    width, height = s16(header, 1), s16(header, 3)
    if width <= 0 or height <= 0:
        raise ValueError(f"invalid dimensions {width}x{height}")

    count = width * height
    cell_start = PREAMBLE + HEADER_SIZE
    cells = struct.unpack_from(f"<{count}H", data, cell_start)

    tail = bounds[6]
    tail_width, tail_height = struct.unpack_from("<hh", data, tail)
    if (tail_width, tail_height) != (width, height):
        raise ValueError("tail dimensions differ from header")
    grid = data[tail + 4 : tail + 4 + count]
    if len(grid) != count:
        raise ValueError("truncated cache grid")

    primary0 = struct.unpack_from("<64H", header, PRIMARY_TABLE_OFF)
    primary1 = struct.unpack_from("<64H", header, PRIMARY2_TABLE_OFF)
    return width, height, cells, grid, primary0, primary1


def boundary_cells(
    width: int,
    height: int,
    cells: tuple[int, ...],
    solid_codes: set[int],
) -> set[int]:
    result: set[int] = set()
    for y in range(height):
        for x in range(width):
            index = y * width + x
            word = cells[index]
            code = word & 0x003F
            if (word & 0x0400) or code not in solid_codes:
                continue

            for dy in (-1, 0, 1):
                for dx in (-1, 0, 1):
                    if dx == 0 and dy == 0:
                        continue
                    nx, ny = x + dx, y + dy
                    if not (0 <= nx < width and 0 <= ny < height):
                        continue
                    neighbor = cells[ny * width + nx]
                    if (neighbor & 0x0400) and ((neighbor & 0x003F) == 0):
                        result.add(index)
                        break
                if index in result:
                    break
    return result


def audit(path: Path) -> int:
    width, height, cells, grid, primary0, primary1 = parse_lev(path.read_bytes())
    contradictions = []
    deterministic = 0

    for index, (word, cached) in enumerate(zip(cells, grid)):
        code = word & 0x003F
        explored = bool(word & 0x0400)
        special = bool(word & 0x0080)
        class5 = bool(word & 0x0800)
        primary = primary0[code]

        expected: set[int] | None = None
        reason = ""
        if code == 0 and explored:
            expected = {0x2E, 0x2F}
            reason = "explored code-0 cell"
        elif code != 0 and primary == 0x0093:
            expected = {0x2E, 0x2F}
            reason = "primary lookup is 0x0093"
        elif explored and class5:
            expected = {0x05}
            reason = "explored 0x0800 cell"
        elif explored and not special and code != 0 and primary != 0x0093:
            expected = {0x01}
            reason = "explored ordinary non-0x0093 cell"

        if expected is not None:
            deterministic += 1
            if cached not in expected:
                contradictions.append(
                    (index, word, primary, cached, expected, reason)
                )

    code_counts = collections.Counter(word & 0x003F for word in cells)
    cache_counts = collections.Counter(grid)

    print(f"{path.name}: {width}x{height} cells={len(cells)}")
    print(
        f"primary_bank_0[1]={primary0[1]:#06x} "
        f"primary_bank_1[1]={primary1[1]:#06x}"
    )
    print(f"deterministic_cells_checked={deterministic}")
    print(f"deterministic_contradictions={len(contradictions)}")
    print(
        "cache_counts="
        + " ".join(f"{value:#04x}:{count}" for value, count in sorted(cache_counts.items()))
    )

    if contradictions:
        grouped = collections.Counter(
            (reason, cached) for _, _, _, cached, _, reason in contradictions
        )
        for (reason, cached), count in grouped.items():
            print(f"  contradiction {count:3d} cached={cached:#04x}: {reason}")
        print(
            "  first_indices="
            + ",".join(str(item[0]) for item in contradictions[:16])
        )

    serialized_solid = {
        code for code in range(1, 64) if primary0[code] != 0x0093
    }
    predicted = boundary_cells(width, height, cells, serialized_solid)
    actual = {index for index, value in enumerate(grid) if value == 0x01}
    print(
        "boundary_rule_with_serialized_table "
        f"predicted={len(predicted)} actual={len(actual)} "
        f"overlap={len(predicted & actual)}"
    )

    # Diagnostic for the supplied Level 22 inconsistency: treat code 1 as an
    # ordinary solid and see whether the encoded boundary footprint matches.
    if code_counts[1]:
        forced_solid = set(serialized_solid)
        forced_solid.add(1)
        forced = boundary_cells(width, height, cells, forced_solid)
        print(
            "boundary_rule_if_code1_solid "
            f"predicted={len(forced)} actual={len(actual)} exact={forced == actual}"
        )

    return 1 if contradictions else 0


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("lev", type=Path)
    args = parser.parse_args()
    raise SystemExit(audit(args.lev))


if __name__ == "__main__":
    main()
