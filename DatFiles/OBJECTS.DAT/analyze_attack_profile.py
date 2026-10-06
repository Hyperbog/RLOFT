#!/usr/bin/env python3
"""Audit OBJECTS.DAT packed attack-profile hand-pair and speed fields."""
from __future__ import annotations

import argparse
import collections
import struct
from pathlib import Path

RECORD_SIZE = 0x41
WEAPON_CATEGORIES = {9, 10, 11}
EXPECTED_PAIR_EXCLUSIVE_NAMES = {
    "QUARTERSTAFF",
    "TWO-HANDED SWORD",
    "HALBERD",
    "COMPOSITE BOW",
    "JAVELIN OF LIGHTNING",
}


def records(path: Path):
    data = path.read_bytes()
    if len(data) < 4:
        raise ValueError("OBJECTS.DAT too short")
    count = struct.unpack_from("<I", data, 0)[0]
    if len(data) != 4 + count * RECORD_SIZE:
        raise ValueError("OBJECTS.DAT size mismatch")
    for object_id in range(count):
        raw = data[4 + object_id * RECORD_SIZE : 4 + (object_id + 1) * RECORD_SIZE]
        nul = raw[:40].find(b"\0")
        if nul < 0:
            raise ValueError(f"object {object_id}: unterminated name")
        name = raw[:nul].decode("ascii")
        profile = struct.unpack_from("<H", raw, 0x35)[0]
        yield object_id, name, raw[0x2F], raw[0x30], profile


def analyze(path: Path) -> None:
    rows = list(records(path))
    classes = collections.Counter(profile & 3 for *_, profile in rows)
    speeds = collections.Counter((profile >> 2) & 0xF for *_, profile in rows)
    high_bits = [(i, name, profile) for i, name, _, _, profile in rows if profile & ~0x3F]

    print(f"records={len(rows)}")
    print("hand_pair_class=" + " ".join(
        f"{value}:{classes[value]}" for value in range(4)
    ))
    print("speed_factor=" + " ".join(
        f"{value}:{count}" for value, count in sorted(speeds.items())
    ))
    print(f"profiles_with_bits_6_15={len(high_bits)}")

    class2 = [(i, name, cat, (profile >> 2) & 0xF) for i, name, cat, _, profile in rows if (profile & 3) == 2]
    print(f"class2_records={len(class2)}")
    for i, name, category, speed in class2:
        print(f"  id={i:3d} category={category:2d} speed={speed:2d} name={name}")

    class2_names = {name for _, name, _, _ in class2}
    if class2_names != EXPECTED_PAIR_EXCLUSIVE_NAMES:
        raise ValueError(
            "class-2 name set changed: "
            + repr(sorted(class2_names))
        )

    weapon_classes = collections.Counter(
        profile & 3 for _, _, category, _, profile in rows if category in WEAPON_CATEGORIES
    )
    print("weapon_categories_9_11=" + " ".join(
        f"class{value}:{weapon_classes[value]}" for value in range(4)
    ))

    for pair_class in (0, 1):
        named = [
            f"{i}:{name}"
            for i, name, category, _, profile in rows
            if category in WEAPON_CATEGORIES and (profile & 3) == pair_class
        ]
        print(f"weapon_class_{pair_class}=" + "; ".join(named))

    print("PASS")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("objects", type=Path)
    args = parser.parse_args()
    analyze(args.objects)


if __name__ == "__main__":
    main()
