#!/usr/bin/env python3
"""Audit OBJECTS.DAT category-dependent state and intrinsic-magic semantics."""
from __future__ import annotations

import argparse
import collections
import struct
from pathlib import Path

RECORD_SIZE = 0x41
CHARGED_OBJECTS = {
    74: 0,    # ROD OF SMITING
    75: 1,    # WAND OF ENEMY DETECTION
    76: 2,    # WAND OF FIREBALLS
    330: 3,
    331: 4,
    332: 5,
    333: 6,
    77: 7,    # WAND OF FROST
    334: 8,
    335: 9,
    78: 10,   # WAND OF MAGIC MISSILES
    79: 11,   # WAND OF PARALYZATION
}
CHARGE_VECTOR_SIZE = 15
INITIAL_CHARGE = 12


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


def analyze(path: Path, save_path: Path | None) -> None:
    records = parse(path)
    category_counts = collections.Counter()
    marker_counts = collections.Counter()
    enchantment_counts = collections.Counter()
    marker_by_category: dict[int, collections.Counter[int]] = collections.defaultdict(collections.Counter)
    state_by_category: dict[int, list[tuple[int, int, str]]] = collections.defaultdict(list)

    for object_id, record in enumerate(records):
        category = record[0x2F]
        state_index = s8(record[0x31])
        marker = struct.unpack_from("<H", record, 0x32)[0]
        enchantment = record[0x37]
        category_counts[category] += 1
        marker_counts[marker] += 1
        enchantment_counts[enchantment] += 1
        marker_by_category[category][marker] += 1
        if state_index != 0:
            state_by_category[category].append((object_id, state_index, name(record)))

    print(f"records={len(records)}")
    print("category_counts=" + " ".join(
        f"{category}:{category_counts[category]}"
        for category in sorted(category_counts)
    ))
    print("intrinsic_magic_marker=" + " ".join(
        f"{value}:{marker_counts[value]}" for value in sorted(marker_counts)
    ))
    print("enchantment_bonus=" + " ".join(
        f"{value}:{enchantment_counts[value]}" for value in sorted(enchantment_counts)
    ))

    for category in sorted(marker_by_category):
        print(
            f"category_{category}_magic_marker="
            + " ".join(
                f"{value}:{marker_by_category[category][value]}"
                for value in sorted(marker_by_category[category])
            )
        )

    for category in sorted(state_by_category):
        print(f"category_{category}_nonzero_state_index={len(state_by_category[category])}")
        for object_id, state_index, obj_name in state_by_category[category]:
            print(f"  id={object_id:3d} state={state_index:3d} name={obj_name}")

    # Container-state identity: every shipped category-1 object has a unique state slot.
    container_values = [s8(r[0x31]) for r in records if r[0x2F] == 1]
    print(f"container_records={len(container_values)}")
    print(f"container_state_unique={len(set(container_values))}")
    print(
        "container_state_values="
        + ",".join(map(str, sorted(container_values)))
    )
    expected_container_values = [*range(0, 61), 62]
    print(f"container_state_matches_0_60_plus_62={sorted(container_values) == expected_container_values}")

    # Charged-device definitions select the persistent 15-byte charge vector.
    print(f"charge_vector_slots={CHARGE_VECTOR_SIZE}")
    charged_ok = True
    for object_id, expected_slot in CHARGED_OBJECTS.items():
        record = records[object_id]
        actual = s8(record[0x31])
        if actual != expected_slot:
            charged_ok = False
        print(
            f"charged id={object_id:3d} slot={actual:2d} "
            f"category={record[0x2F]:2d} name={name(record)}"
        )
    print(f"charged_object_state_slots_match={charged_ok}")

    # Duplicate FIREBALL/FROST definitions intentionally differ only at +0x31.
    duplicate_ok = True
    for base, duplicates in ((76, (330, 331, 332, 333)), (77, (334, 335))):
        base_record = records[base]
        for object_id in duplicates:
            differing = [
                off for off in range(RECORD_SIZE)
                if records[object_id][off] != base_record[off]
            ]
            if differing != [0x31]:
                duplicate_ok = False
            print(
                f"duplicate_compare_{base}_{object_id}="
                + ",".join(f"0x{off:02X}" for off in differing)
            )
    print(f"charged_duplicates_differ_only_at_0x31={duplicate_ok}")

    # The marker and numerical enchantment are independent serialized concepts.
    combos = collections.Counter(
        (struct.unpack_from("<H", r, 0x32)[0], r[0x37])
        for r in records
    )
    print(
        "magic_marker_enchantment_pairs="
        + " ".join(
            f"{marker}/{enchantment}:{count}"
            for (marker, enchantment), count in sorted(combos.items())
        )
    )
    marker_families = (5, 6, 7, 11, 14)
    print(
        "all_magic_marker_families_marked="
        + str(all(
            struct.unpack_from("<H", r, 0x32)[0] == 1
            for r in records
            if r[0x2F] in marker_families
        ))
    )

    if save_path is not None:
        save = save_path.read_bytes()
        needle = bytes([INITIAL_CHARGE]) * CHARGE_VECTOR_SIZE
        starts = []
        pos = 0
        while True:
            pos = save.find(needle, pos)
            if pos < 0:
                break
            starts.append(pos)
            pos += 1
        print(
            "save_initial_charge_vector_offsets="
            + ",".join(f"0x{x:X}" for x in starts)
        )
        print(f"save_has_unique_initial_charge_vector={starts == [0x0D59]}")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("objects", type=Path)
    parser.add_argument("--save", type=Path)
    args = parser.parse_args()
    analyze(args.objects, args.save)


if __name__ == "__main__":
    main()
