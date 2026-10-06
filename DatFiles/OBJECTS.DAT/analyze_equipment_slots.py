#!/usr/bin/env python3
"""Audit OBJECTS.DAT equipment-slot routing proved by CODE.2 12180..1246E."""
from __future__ import annotations

import argparse
import struct
from pathlib import Path

RECORD_SIZE = 0x41

QUIVER_IDS = (
    0x006, 0x109, 0x10E, 0x120, 0x127, 0x131, 0x134,
    0x135, 0x13B, 0x13D, 0x141, 0x146, 0x147,
)
ARMOR_CLASS5_WHITELIST = (0x00C, 0x010, 0x05F, 0x0EC, 0x0EE)
LOCKED_TOUCH_IDS = tuple(range(0x07E, 0x087))

DEDICATED_CATEGORY_SLOTS = {
    3: (17,),
    4: (16,),
    5: (12, 13),
    6: (18,),
    7: (20,),
    8: (16,),
}


def parse_records(path: Path) -> list[tuple[int, str, int]]:
    data = path.read_bytes()
    if len(data) < 4:
        raise ValueError("OBJECTS.DAT shorter than count")
    count = struct.unpack_from("<I", data, 0)[0]
    if len(data) != 4 + count * RECORD_SIZE:
        raise ValueError("OBJECTS.DAT size does not match count * 0x41")

    records: list[tuple[int, str, int]] = []
    for object_id in range(count):
        raw = data[4 + object_id * RECORD_SIZE : 4 + (object_id + 1) * RECORD_SIZE]
        nul = raw[:40].find(b"\0")
        if nul < 0:
            raise ValueError(f"object {object_id}: name has no NUL")
        name = raw[:nul].decode("ascii")
        records.append((object_id, name, raw[0x2F]))
    return records


def analyze(path: Path) -> None:
    records = parse_records(path)
    print(f"records={len(records)}")

    for category, slots in DEDICATED_CATEGORY_SLOTS.items():
        rows = [record for record in records if record[2] == category]
        print(
            f"category_{category}_records={len(rows)} "
            f"equipment_slots={'/'.join(map(str, slots))}"
        )

    quivers = [records[object_id] for object_id in QUIVER_IDS]
    if not all(name == "QUIVER" and category == 1 for _, name, category in quivers):
        raise ValueError("hard-coded quiver predicate does not resolve solely to category-1 QUIVER records")
    print(
        "quiver_exceptions=" + str(len(quivers)) + " ids="
        + ",".join(str(object_id) for object_id, _, _ in quivers)
    )

    armor = [records[object_id] for object_id in ARMOR_CLASS5_WHITELIST]
    if not all(category == 4 for _, _, category in armor):
        raise ValueError("class-code-5 armor whitelist contains a non-category-4 record")
    print(
        "class5_armor_whitelist="
        + "; ".join(f"{object_id}:{name}" for object_id, name, _ in armor)
    )

    touch = [records[object_id] for object_id in LOCKED_TOUCH_IDS]
    if not all(category == 0 for _, _, category in touch):
        raise ValueError("0x7E..0x86 transient-item range contains a non-category-0 record")
    print(
        "locked_touch_items="
        + "; ".join(f"{object_id}:{name}" for object_id, name, _ in touch)
    )

    ring_names = [name for _, name, category in records if category == 5]
    print("category5_names=" + "; ".join(ring_names))
    print("PASS")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("objects", type=Path)
    args = parser.parse_args()
    analyze(args.objects)


if __name__ == "__main__":
    main()
