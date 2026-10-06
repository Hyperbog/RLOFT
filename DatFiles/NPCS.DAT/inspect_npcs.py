#!/usr/bin/env python3
"""Inspect and losslessly re-encode Ravenloft NPCS.DAT."""
from __future__ import annotations

import argparse
import collections
import struct
from dataclasses import dataclass
from pathlib import Path

RECORD_SIZE = 0x45
NAME_SIZE = 20


def u16(buf: bytes, off: int) -> int:
    return struct.unpack_from("<H", buf, off)[0]


def u32(buf: bytes, off: int) -> int:
    return struct.unpack_from("<I", buf, off)[0]


def s8(value: int) -> int:
    return value - 256 if value & 0x80 else value


@dataclass(frozen=True)
class NpcRecord:
    raw: bytes
    name: str
    local_shape_base: int
    collision_half_extent: int
    scene_frame_stride: int
    animation_cycle_divisor: int
    animation_span: int
    scene_influence_step: int
    damage_reaction_animation_flag: int
    hit_dice_count: int
    melee_damage_dice: int
    melee_damage_sides: int
    melee_damage_bonus: int
    armor_class: int
    thac0: int
    experience_award: int
    action_delay: int
    render_scale_lookup_bias: int
    directional_offsets: tuple[int, int, int, int]
    behavior_flags: int
    initial_runtime_flags: int
    behavior_flags_39: int
    behavior_flags_3a: int
    unresolved_3b: int
    unresolved_3c: int
    ambient_or_proximity_sound_id: int
    attack_sound_id: int
    damage_reaction_sound_id: int
    death_sound_id: int

    # Compatibility aliases for earlier provisional schema names.
    @property
    def scene_sprite_base(self) -> int:
        return self.local_shape_base

    @property
    def projected_scale_lookup_bias(self) -> int:
        return self.render_scale_lookup_bias

    @property
    def ambient_sound_id(self) -> int:
        return self.ambient_or_proximity_sound_id


def parse_record(raw: bytes) -> NpcRecord:
    if len(raw) != RECORD_SIZE:
        raise ValueError(f"record size {len(raw)} != {RECORD_SIZE}")

    name_raw = raw[:NAME_SIZE]
    nul = name_raw.find(b"\0")
    if nul < 0:
        raise ValueError("name lacks NUL inside fixed 20-byte field")
    name = name_raw[:nul].decode("ascii", errors="strict")

    return NpcRecord(
        raw=raw,
        name=name,
        local_shape_base=u16(raw, 0x14),
        collision_half_extent=u16(raw, 0x16),
        scene_frame_stride=raw[0x18],
        animation_cycle_divisor=raw[0x19],
        animation_span=raw[0x1A],
        scene_influence_step=raw[0x1B],
        damage_reaction_animation_flag=raw[0x1C],
        hit_dice_count=raw[0x1D],
        melee_damage_dice=raw[0x1E],
        melee_damage_sides=raw[0x1F],
        melee_damage_bonus=raw[0x20],
        armor_class=s8(raw[0x21]),
        thac0=raw[0x22],
        experience_award=u16(raw, 0x23),
        action_delay=u16(raw, 0x25),
        render_scale_lookup_bias=u16(raw, 0x27),
        directional_offsets=struct.unpack_from("<4H", raw, 0x29),
        behavior_flags=u32(raw, 0x31),
        initial_runtime_flags=u32(raw, 0x35),
        behavior_flags_39=raw[0x39],
        behavior_flags_3a=raw[0x3A],
        unresolved_3b=raw[0x3B],
        unresolved_3c=raw[0x3C],
        ambient_or_proximity_sound_id=u16(raw, 0x3D),
        attack_sound_id=u16(raw, 0x3F),
        damage_reaction_sound_id=u16(raw, 0x41),
        death_sound_id=u16(raw, 0x43),
    )


def parse_file(data: bytes) -> list[NpcRecord]:
    if len(data) < 4:
        raise ValueError("file shorter than count")
    count = struct.unpack_from("<I", data, 0)[0]
    expected = 4 + count * RECORD_SIZE
    if len(data) != expected:
        raise ValueError(f"file size {len(data)} != expected {expected}")
    return [
        parse_record(data[4 + i * RECORD_SIZE : 4 + (i + 1) * RECORD_SIZE])
        for i in range(count)
    ]


def encode_record(record: NpcRecord) -> bytes:
    # Start from the exact source record so unresolved flag bits, post-NUL name
    # residue, and other bounded raw state remain lossless under semantic edits.
    out = bytearray(record.raw)

    name = record.name.encode("ascii")
    if len(name) >= NAME_SIZE:
        raise ValueError("encoded name must leave room for NUL within 20 bytes")
    out[: len(name)] = name
    out[len(name)] = 0

    struct.pack_into("<H", out, 0x14, record.local_shape_base)
    struct.pack_into("<H", out, 0x16, record.collision_half_extent)
    out[0x18] = record.scene_frame_stride
    out[0x19] = record.animation_cycle_divisor
    out[0x1A] = record.animation_span
    out[0x1B] = record.scene_influence_step
    out[0x1C] = record.damage_reaction_animation_flag
    out[0x1D] = record.hit_dice_count
    out[0x1E] = record.melee_damage_dice
    out[0x1F] = record.melee_damage_sides
    out[0x20] = record.melee_damage_bonus
    out[0x21] = record.armor_class & 0xFF
    out[0x22] = record.thac0
    struct.pack_into("<H", out, 0x23, record.experience_award)
    struct.pack_into("<H", out, 0x25, record.action_delay)
    struct.pack_into("<H", out, 0x27, record.render_scale_lookup_bias)
    struct.pack_into("<4H", out, 0x29, *record.directional_offsets)
    struct.pack_into("<I", out, 0x31, record.behavior_flags)
    struct.pack_into("<I", out, 0x35, record.initial_runtime_flags)
    out[0x39] = record.behavior_flags_39
    out[0x3A] = record.behavior_flags_3a
    out[0x3B] = record.unresolved_3b
    out[0x3C] = record.unresolved_3c
    struct.pack_into("<H", out, 0x3D, record.ambient_or_proximity_sound_id)
    struct.pack_into("<H", out, 0x3F, record.attack_sound_id)
    struct.pack_into("<H", out, 0x41, record.damage_reaction_sound_id)
    struct.pack_into("<H", out, 0x43, record.death_sound_id)
    return bytes(out)


def encode_file(records: list[NpcRecord]) -> bytes:
    return struct.pack("<I", len(records)) + b"".join(
        encode_record(record) for record in records
    )


def inspect(path: Path) -> None:
    source = path.read_bytes()
    records = parse_file(source)
    if encode_file(records) != source:
        raise ValueError("parse -> encode round-trip mismatch")

    print(f"records={len(records)}")
    print(f"record_size=0x{RECORD_SIZE:02X}")
    print("roundtrip=PASS")
    print(f"max_name_length={max(len(record.name) for record in records)}")
    nonzero_name_tail = 0
    for record in records:
        end = record.raw[:NAME_SIZE].find(b"\0")
        if any(record.raw[end + 1 : NAME_SIZE]):
            nonzero_name_tail += 1
    print(f"nonzero_post_nul_name_tail_records={nonzero_name_tail}")
    print(
        "collision_half_extent="
        + " ".join(
            f"{value:#06x}:{count}"
            for value, count in sorted(
                collections.Counter(record.collision_half_extent for record in records).items()
            )
        )
    )
    print(f"combat_hit_dice_nonzero={sum(record.hit_dice_count != 0 for record in records)}")
    print(f"scene_influence_nonzero={sum(record.scene_influence_step != 0 for record in records)}")
    print(
        "render_scale_lookup_bias_nonzero="
        f"{sum(record.render_scale_lookup_bias != 0 for record in records)}"
    )
    print(f"unresolved_3b_nonzero={sum(record.unresolved_3b != 0 for record in records)}")
    print(f"unresolved_3c_nonzero={sum(record.unresolved_3c != 0 for record in records)}")

    print("-- first definitions --")
    for index, record in enumerate(records[:12]):
        damage = (
            f"{record.melee_damage_dice}d{record.melee_damage_sides}"
            + (f"+{record.melee_damage_bonus}" if record.melee_damage_bonus else "")
            if record.melee_damage_dice and record.melee_damage_sides
            else "-"
        )
        print(
            f"{index:02d} {record.name:18s} shape={record.local_shape_base:4d} "
            f"HD={record.hit_dice_count:2d} AC={record.armor_class:3d} "
            f"THAC0={record.thac0:2d} dmg={damage:7s} XP={record.experience_award:5d}"
        )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("npcs", type=Path)
    args = parser.parse_args()
    inspect(args.npcs)


if __name__ == "__main__":
    main()
