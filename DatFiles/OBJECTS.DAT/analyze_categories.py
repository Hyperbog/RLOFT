#!/usr/bin/env python3
"""Audit OBJECTS.DAT category-dependent +0x31/+0x32 semantics."""
from __future__ import annotations

import argparse
import collections
import struct
from pathlib import Path

RECORD_SIZE = 0x41


def s8(value: int) -> int:
    return value - 256 if value & 0x80 else value


def parse(path: Path) -> list[bytes]:
    data = path.read_bytes()
    if len(data) < 4:
        raise ValueError("file too short")
    count = struct.unpack_from("<I", data, 0)[0]
    expected = 4 + count * RECORD_SIZE
    if len(data) != expected:
        raise ValueError(f"size {len(data)} != expected {expected}")
    return [
        data[4 + i * RECORD_SIZE : 4 + (i + 1) * RECORD_SIZE]
        for i in range(count)
    ]


def name(record: bytes) -> str:
    raw = record[:40]
    nul = raw.find(b"\0")
    if nul < 0:
        raise ValueError("name lacks NUL")
    return raw[:nul].decode("ascii", errors="strict")


def analyze(path: Path) -> None:
    records = parse(path)
    category_counts = collections.Counter()
    marker_counts = collections.Counter()
    marker_by_category: dict[int, collections.Counter[int]] = collections.defaultdict(collections.Counter)
    nonzero_aux: dict[int, list[tuple[int, int, str]]] = collections.defaultdict(list)

    for object_id, record in enumerate(records):
        category = record[0x2F]
        aux = s8(record[0x31])
        marker = struct.unpack_from("<H", record, 0x32)[0]
        category_counts[category] += 1
        marker_counts[marker] += 1
        marker_by_category[category][marker] += 1
        if aux != 0:
            nonzero_aux[category].append((object_id, aux, name(record)))

    print(f"records={len(records)}")
    print("category_counts=" + " ".join(
        f"{category}:{category_counts[category]}"
        for category in sorted(category_counts)
    ))
    print("special_property_marker=" + " ".join(
        f"{value}:{marker_counts[value]}" for value in sorted(marker_counts)
    ))

    for category in sorted(marker_by_category):
        print(
            f"category_{category}_markers="
            + " ".join(
                f"{value}:{marker_by_category[category][value]}"
                for value in sorted(marker_by_category[category])
            )
        )

    for category in sorted(nonzero_aux):
        print(f"category_{category}_nonzero_aux={len(nonzero_aux[category])}")
        for object_id, aux, obj_name in nonzero_aux[category]:
            print(f"  id={object_id:3d} aux={aux:3d} name={obj_name}")

    # Supplied-corpus invariants supporting category-specific interpretation.
    container = [(i, s8(r[0x31])) for i, r in enumerate(records) if r[0x2F] == 1]
    container_values = [value for _, value in container]
    print(f"container_records={len(container)}")
    print(f"container_aux_unique={len(set(container_values))}")

    category0_aux = [
        (i, s8(r[0x31]), name(r))
        for i, r in enumerate(records)
        if r[0x2F] == 0 and s8(r[0x31]) != 0
    ]
    wand_slots = sorted(value for _, value, _ in category0_aux)
    print("category0_nonzero_aux_values=" + ",".join(map(str, wand_slots)))

    expected_wand_slots = list(range(1, 12))
    print(f"category0_aux_is_exact_1_to_11={wand_slots == expected_wand_slots}")
    print(
        "category0_nonzero_aux_all_wands="
        + str(all("WAND" in obj_name for _, _, obj_name in category0_aux))
    )

    for base, duplicates in ((76, (330, 331, 332, 333)), (77, (334, 335))):
        base_record = records[base]
        for object_id in duplicates:
            differing = [
                off for off in range(RECORD_SIZE)
                if records[object_id][off] != base_record[off]
            ]
            print(
                f"duplicate_compare_{base}_{object_id}="
                + ",".join(f"0x{off:02X}" for off in differing)
            )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("objects", type=Path)
    args = parser.parse_args()
    analyze(args.objects)


if __name__ == "__main__":
    main()
