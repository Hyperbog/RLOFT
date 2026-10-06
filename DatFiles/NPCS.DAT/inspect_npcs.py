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
    scene_sprite_base: int
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
    render_aux_27: int
    directional_offsets: tuple[int, int, int, int]
    behavior_flags: int
    initial_runtime_flags: int
    behavior_flags_39: int
    behavior_flags_3a: int
    unresolved_3b: int
    unresolved_3c: int
    ambient_sound_id: int
    attack_sound_id: int
    sound_event_41_id: int
    death_sound_id: int


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
        scene_sprite_base=u16(raw, 0x14),
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
        render_aux_27=u16(raw, 0x27),
        directional_offsets=struct.unpack_from("<4H", raw, 0x29),
        behavior_flags=u32(raw, 0x31),
        initial_runtime_flags=u32(raw, 0x35),
        behavior_flags_39=raw[0x39],
        behavior_flags_3a=raw[0x3A],
        unresolved_3b=raw[0x3B],
        unresolved_3c=raw[0x3C],
        ambient_sound_id=u16(raw, 0x3D),
        attack_sound_id=u16(raw, 0x3F),
        sound_event_41_id=u16(raw, 0x41),
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


def encode_file(records: list[NpcRecord]) -> bytes:
    # The original record bytes remain the preservation authority. Semantic edit
    # support can be layered later without normalizing unknown flag bits.
    return struct.pack("<I", len(records)) + b"".join(record.raw for record in records)


def inspect(path: Path) -> None:
    source = path.read_bytes()
    records = parse_file(source)
    if encode_file(records) != source:
        raise ValueError("parse -> encode round-trip mismatch")

    print(f"records={len(records)}")
    print(f"record_size=0x{RECORD_SIZE:02X}")
    print("roundtrip=PASS")
    print(f"max_name_length={max(len(record.name) for record in records)}")
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
    print(f"render_aux_27_nonzero={sum(record.render_aux_27 != 0 for record in records)}")
    print(f"reserved_3b_nonzero={sum(record.unresolved_3b != 0 for record in records)}")
    print(f"reserved_3c_nonzero={sum(record.unresolved_3c != 0 for record in records)}")

    print("-- first definitions --")
    for index, record in enumerate(records[:12]):
        damage = (
            f"{record.melee_damage_dice}d{record.melee_damage_sides}"
            + (f"+{record.melee_damage_bonus}" if record.melee_damage_bonus else "")
            if record.melee_damage_dice and record.melee_damage_sides
            else "-"
        )
        print(
            f"{index:02d} {record.name:18s} sprite={record.scene_sprite_base:4d} "
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
