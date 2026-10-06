#!/usr/bin/env python3
"""Audit remaining LEVELS packed-cell flags against 10-byte records and neighbors."""
from __future__ import annotations

import argparse
import collections
import struct
from pathlib import Path

MASKS = (0x0080, 0x0100, 0x0200, 0x0800, 0x1000, 0x2000, 0xC000)


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


def parse_record(record: bytes):
    bounds = struct.unpack_from("<7I", record, 1)
    header = record[bounds[0] : bounds[0] + 0x2A4]
    width = s16(header, 1)
    height = s16(header, 3)
    cells = struct.unpack_from(
        f"<{width * height}H", record, bounds[0] + 0x2A4
    )

    cursor = bounds[1]
    records_10: list[tuple[int, int, int]] = []
    for column in range(width):
        count = s16(record, cursor)
        cursor += 2
        for _ in range(count):
            item = record[cursor : cursor + 10]
            cursor += 10
            records_10.append((item[0], item[4], item[5]))
    if cursor != bounds[2]:
        raise ValueError(
            f"10-byte list end {cursor:#x} != boundary_2 {bounds[2]:#x}"
        )
    return width, height, cells, records_10


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("levels", type=Path)
    args = parser.parse_args()

    records = archive_records(args.levels.read_bytes())
    flag_count = collections.Counter()
    kind_total = collections.Counter()
    kind_bit1000 = collections.Counter()
    selector_counts = collections.Counter()
    frame_to_center_neighbors = collections.Counter()
    center_to_frame_neighbors = collections.Counter()
    bit80_roles = collections.Counter()
    bit800_roles = collections.Counter()

    total_cells = 0
    record_cells = 0
    group1 = 0
    group1_bit1000 = 0
    kind1 = 0
    kind1_bit80 = 0
    bit1000_without_record = []
    record_without_bit1000 = []

    for level_id, record in enumerate(records):
        width, height, cells, records_10 = parse_record(record)
        total_cells += len(cells)
        records_at_cell = collections.defaultdict(list)

        for kind, row, column in records_10:
            index = row * width + column
            records_at_cell[index].append(kind)
            kind_total[kind] += 1
            if cells[index] & 0x1000:
                kind_bit1000[kind] += 1
            record_cells += 1
            if kind != 1:
                group1 += 1
                if cells[index] & 0x1000:
                    group1_bit1000 += 1
            else:
                kind1 += 1
                if cells[index] & 0x0080:
                    kind1_bit80 += 1
            if not (cells[index] & 0x1000):
                record_without_bit1000.append(
                    (level_id, row, column, kind, cells[index])
                )

        for index, word in enumerate(cells):
            x = index % width
            y = index // width
            neighbors = []
            if x:
                neighbors.append(index - 1)
            if x + 1 < width:
                neighbors.append(index + 1)
            if y:
                neighbors.append(index - width)
            if y + 1 < height:
                neighbors.append(index + width)

            for mask in MASKS:
                if word & mask:
                    flag_count[mask] += 1
            selector_counts[(word & 0xC000) >> 14] += 1

            kinds = tuple(sorted(records_at_cell.get(index, ())))
            if word & 0x0080:
                bit80_roles[kinds] += 1
                center_to_frame_neighbors[
                    sum(bool(cells[n] & 0x0100) for n in neighbors)
                ] += 1
            if word & 0x0100:
                frame_to_center_neighbors[
                    sum(bool(cells[n] & 0x0080) for n in neighbors)
                ] += 1
            if word & 0x0800:
                bit800_roles[kinds] += 1
            if word & 0x1000 and not kinds:
                bit1000_without_record.append(
                    (level_id, x, y, word & 0x003F)
                )

    print(f"levels={len(records)}")
    print(f"cells={total_cells}")
    for mask in MASKS:
        print(f"{mask:#06x}={flag_count[mask]}")
    print(
        "render_selector_counts="
        + " ".join(f"{value}:{selector_counts[value]}" for value in range(4))
    )
    print(f"10byte_records={record_cells}")
    print(f"group1_records(kind!=1)={group1} bit1000={group1_bit1000}")
    print(f"kind1_dynamic_records={kind1} bit0080={kind1_bit80}")
    print(
        "kind_bit1000="
        + " ".join(
            f"{kind}:{kind_bit1000[kind]}/{kind_total[kind]}"
            for kind in sorted(kind_total)
        )
    )
    print("bit0080_record_roles=" + repr(bit80_roles.most_common()))
    print(
        "bit0100_neighbor_0080_counts="
        + " ".join(
            f"{count}:{frame_to_center_neighbors[count]}"
            for count in sorted(frame_to_center_neighbors)
        )
    )
    print(
        "bit0080_neighbor_0100_counts="
        + " ".join(
            f"{count}:{center_to_frame_neighbors[count]}"
            for count in sorted(center_to_frame_neighbors)
        )
    )
    print("bit0800_record_roles=" + repr(bit800_roles.most_common()))
    print(
        f"bit1000_without_10byte_record={len(bit1000_without_record)} "
        f"{bit1000_without_record}"
    )
    print(
        f"10byte_record_without_bit1000={len(record_without_bit1000)} "
        f"{record_without_bit1000}"
    )


if __name__ == "__main__":
    main()
