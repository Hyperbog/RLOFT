#!/usr/bin/env python3
"""Audit proved NPCS.DAT behavior/capability flags without guessing unresolved bits."""
from __future__ import annotations

import argparse
import collections
import struct
from pathlib import Path

RECORD_SIZE = 0x45

PROVEN = (
    ("behavior_flags", 0x00000002, "hold_undead_effect_immunity"),
    ("behavior_flags", 0x00000008, "hold_person_effect_immunity"),
    ("behavior_flags", 0x00000010, "hold_monster_effect_immunity"),
    ("behavior_flags", 0x00000100, "common_spell_effect_immunity"),
    ("behavior_flags", 0x00000800, "cold_effect_immunity"),
    ("behavior_flags", 0x00002000, "fire_effect_immunity"),
    ("behavior_flags", 0x00004000, "weapon_category10_damage_floor_one"),
    ("behavior_flags", 0x00008000, "weapon_category9_damage_floor_one"),
    ("behavior_flags_39", 0x01, "poison_attack"),
    ("behavior_flags_39", 0x20, "paralysis_attack"),
    ("behavior_flags_39", 0x40, "dynamic_barrier_activation"),
    ("behavior_flags_39", 0x80, "obstacle_passthrough"),
    ("behavior_flags_3A", 0x02, "level_drain_attack"),
    ("behavior_flags_3A", 0x04, "strength_drain_attack"),
    ("behavior_flags_3A", 0x10, "disease_attack"),
    ("behavior_flags_3A", 0x20, "undead_classification"),
)


def parse(path: Path):
    data = path.read_bytes()
    if len(data) < 4:
        raise ValueError("file shorter than count")
    count = struct.unpack_from("<I", data, 0)[0]
    expected = 4 + count * RECORD_SIZE
    if len(data) != expected:
        raise ValueError(f"size {len(data)} != 4 + {count}*0x45 = {expected}")

    records = []
    for index in range(count):
        raw = data[4 + index * RECORD_SIZE : 4 + (index + 1) * RECORD_SIZE]
        name = raw[:20].split(b"\0", 1)[0].decode("ascii")
        records.append(
            {
                "index": index,
                "name": name,
                "behavior_flags": struct.unpack_from("<I", raw, 0x31)[0],
                "behavior_flags_39": raw[0x39],
                "behavior_flags_3A": raw[0x3A],
            }
        )
    return records


def bit_counts(records, field: str, width: int):
    counts = collections.Counter()
    for record in records:
        value = record[field]
        for bit in range(width):
            mask = 1 << bit
            if value & mask:
                counts[mask] += 1
    return counts


def print_counts(label: str, counts, digits: int):
    print(
        label
        + "="
        + " ".join(f"0x{mask:0{digits}X}:{count}" for mask, count in sorted(counts.items()))
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("npcs", type=Path)
    args = parser.parse_args()

    records = parse(args.npcs)
    print(f"records={len(records)} record_size=0x{RECORD_SIZE:02X}")

    print_counts("behavior_flags_bits", bit_counts(records, "behavior_flags", 32), 8)
    print_counts("behavior_flags_39_bits", bit_counts(records, "behavior_flags_39", 8), 2)
    print_counts("behavior_flags_3A_bits", bit_counts(records, "behavior_flags_3A", 8), 2)

    for field, mask, label in PROVEN:
        hits = [record for record in records if record[field] & mask]
        print(
            f"{label}={len(hits)} "
            + ", ".join(f"{record['index']}:{record['name']}" for record in hits)
        )


if __name__ == "__main__":
    main()
