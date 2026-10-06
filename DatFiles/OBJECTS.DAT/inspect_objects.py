#!/usr/bin/env python3
"""Inspect and losslessly re-encode Ravenloft OBJECTS.DAT."""
from __future__ import annotations

import argparse
import collections
import struct
from dataclasses import dataclass
from pathlib import Path

RECORD_SIZE = 0x41
NAME_SIZE = 40


def u16(buf: bytes, off: int) -> int:
    return struct.unpack_from("<H", buf, off)[0]


def s8(value: int) -> int:
    return value - 256 if value & 0x80 else value


@dataclass(frozen=True)
class ObjectRecord:
    raw: bytes
    name: str
    world_sprite_base: int
    world_heading_stride: int
    world_aux_selector: int
    unknown_2c: int
    ui_graphic_index: int
    item_category: int
    weight_half_pounds: int
    aux_index: int
    special_property_marker: int
    armor_class_adjustment: int
    attack_profile: int
    enchantment_bonus: int
    damage_sm_dice: int
    damage_sm_sides: int
    damage_sm_bonus: int
    damage_lg_dice: int
    damage_lg_sides: int
    damage_lg_bonus: int
    range_short: int
    range_medium: int
    range_long: int

    @property
    def attack_mode(self) -> int:
        return self.attack_profile & 0x3

    @property
    def weapon_speed_factor(self) -> int:
        return (self.attack_profile >> 2) & 0xF


def parse_record(raw: bytes) -> ObjectRecord:
    if len(raw) != RECORD_SIZE:
        raise ValueError(f"record size {len(raw)} != {RECORD_SIZE}")

    name_raw = raw[:NAME_SIZE]
    nul = name_raw.find(b"\0")
    if nul < 0:
        raise ValueError("name lacks NUL inside fixed 40-byte field")
    name = name_raw[:nul].decode("ascii", errors="strict")

    return ObjectRecord(
        raw=raw,
        name=name,
        world_sprite_base=u16(raw, 0x28),
        world_heading_stride=raw[0x2A],
        world_aux_selector=raw[0x2B],
        unknown_2c=raw[0x2C],
        ui_graphic_index=u16(raw, 0x2D),
        item_category=raw[0x2F],
        weight_half_pounds=raw[0x30],
        aux_index=s8(raw[0x31]),
        special_property_marker=u16(raw, 0x32),
        armor_class_adjustment=s8(raw[0x34]),
        attack_profile=u16(raw, 0x35),
        enchantment_bonus=raw[0x37],
        damage_sm_dice=raw[0x38],
        damage_sm_sides=raw[0x39],
        damage_sm_bonus=raw[0x3A],
        damage_lg_dice=raw[0x3B],
        damage_lg_sides=raw[0x3C],
        damage_lg_bonus=raw[0x3D],
        range_short=raw[0x3E],
        range_medium=raw[0x3F],
        range_long=raw[0x40],
    )


def encode_record(record: ObjectRecord) -> bytes:
    # Start from the exact source record so any unresolved bytes remain lossless.
    out = bytearray(record.raw)

    name = record.name.encode("ascii")
    if len(name) >= NAME_SIZE:
        raise ValueError("encoded name must leave room for NUL within 40 bytes")
    out[:NAME_SIZE] = b"\0" * NAME_SIZE
    out[: len(name)] = name

    struct.pack_into("<H", out, 0x28, record.world_sprite_base)
    out[0x2A] = record.world_heading_stride
    out[0x2B] = record.world_aux_selector
    out[0x2C] = record.unknown_2c
    struct.pack_into("<H", out, 0x2D, record.ui_graphic_index)
    out[0x2F] = record.item_category
    out[0x30] = record.weight_half_pounds
    out[0x31] = record.aux_index & 0xFF
    struct.pack_into("<H", out, 0x32, record.special_property_marker)
    out[0x34] = record.armor_class_adjustment & 0xFF
    struct.pack_into("<H", out, 0x35, record.attack_profile)
    out[0x37] = record.enchantment_bonus
    out[0x38] = record.damage_sm_dice
    out[0x39] = record.damage_sm_sides
    out[0x3A] = record.damage_sm_bonus
    out[0x3B] = record.damage_lg_dice
    out[0x3C] = record.damage_lg_sides
    out[0x3D] = record.damage_lg_bonus
    out[0x3E] = record.range_short
    out[0x3F] = record.range_medium
    out[0x40] = record.range_long
    return bytes(out)


def parse_file(data: bytes) -> list[ObjectRecord]:
    if len(data) < 4:
        raise ValueError("file shorter than count")
    count = struct.unpack_from("<I", data, 0)[0]
    expected = 4 + count * RECORD_SIZE
    if len(data) != expected:
        raise ValueError(
            f"file size {len(data)} != 4 + {count}*0x41 = {expected}"
        )
    return [
        parse_record(data[4 + i * RECORD_SIZE : 4 + (i + 1) * RECORD_SIZE])
        for i in range(count)
    ]


def encode_file(records: list[ObjectRecord]) -> bytes:
    return struct.pack("<I", len(records)) + b"".join(
        encode_record(record) for record in records
    )


def analyze(path: Path) -> None:
    data = path.read_bytes()
    records = parse_file(data)
    rebuilt = encode_file(records)

    print(
        f"file={path.name} bytes={len(data)} records={len(records)} "
        f"record_size=0x{RECORD_SIZE:02x}"
    )
    print("roundtrip=" + ("PASS" if rebuilt == data else "FAIL"))

    blanks = [index for index, record in enumerate(records) if not record.name]
    print(f"blank_names={len(blanks)} ids={blanks}")
    print(f"max_name_length={max(len(record.name) for record in records)}")

    def counter_line(label: str, values) -> None:
        counts = collections.Counter(values)
        print(label + "=" + " ".join(f"{key}:{value}" for key, value in sorted(counts.items())))

    counter_line("categories", (r.item_category for r in records))
    counter_line("world_aux_selector", (r.world_aux_selector for r in records))
    counter_line("unknown_2c", (r.unknown_2c for r in records))
    counter_line("special_property_marker", (r.special_property_marker for r in records))
    counter_line("attack_profiles", (r.attack_profile for r in records))
    counter_line("enchantment_bonus", (r.enchantment_bonus for r in records))
    counter_line("ac_adjustment", (r.armor_class_adjustment for r in records))

    print(
        "nonzero_range_records="
        + str(
            sum(
                bool(r.range_short or r.range_medium or r.range_long)
                for r in records
            )
        )
    )
    category1_aux = [r.aux_index for r in records if r.item_category == 1]
    print(f"category1_aux_unique={len(set(category1_aux))}")

    sample_ids = [0, 20, 40, 57, 63, 74, 87, 155, 164, 167, 268, 335]
    for object_id in sample_ids:
        r = records[object_id]
        print(
            f"{object_id:03d} {r.name or '<blank>'} "
            f"cat={r.item_category} weight={r.weight_half_pounds}/2lb "
            f"ui={r.ui_graphic_index} "
            f"world={r.world_sprite_base}+heading*{r.world_heading_stride} "
            f"aux={r.aux_index} attack_mode={r.attack_mode} "
            f"speed={r.weapon_speed_factor} ench={r.enchantment_bonus} "
            f"dmg={r.damage_sm_dice}d{r.damage_sm_sides}+{r.damage_sm_bonus}/"
            f"{r.damage_lg_dice}d{r.damage_lg_sides}+{r.damage_lg_bonus} "
            f"range={r.range_short}/{r.range_medium}/{r.range_long}"
        )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("objects", type=Path)
    args = parser.parse_args()
    analyze(args.objects)


if __name__ == "__main__":
    main()
