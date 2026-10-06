#!/usr/bin/env python3
"""Validate NPCS.DAT creature sound fields against the supplied table and certified CODE.2."""
from __future__ import annotations

import argparse
import hashlib
import struct
from pathlib import Path

RECORD_SIZE = 0x45
CODE2_SHA256 = "853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314"
OBJ1_FILE_OFF = 0x3AF24
OBJ1_SIZE = 0x3A852

FIELDS = (
    (0x3D, "ambient_or_proximity_sound_id"),
    (0x3F, "attack_sound_id"),
    (0x41, "damage_reaction_sound_id"),
    (0x43, "death_sound_id"),
)


def parse_npcs(path: Path):
    data = path.read_bytes()
    if len(data) < 4:
        raise ValueError("NPCS.DAT shorter than count")
    count = struct.unpack_from("<I", data, 0)[0]
    expected = 4 + count * RECORD_SIZE
    if len(data) != expected:
        raise ValueError(f"size {len(data)} != 4 + {count}*0x45 = {expected}")

    records = []
    for index in range(count):
        raw = data[4 + index * RECORD_SIZE : 4 + (index + 1) * RECORD_SIZE]
        name = raw[:20].split(b"\0", 1)[0].decode("ascii", "strict")
        values = {
            label: struct.unpack_from("<H", raw, offset)[0]
            for offset, label in FIELDS
        }
        records.append((index, name, values))
    return records


def verify_code2(path: Path) -> None:
    full = path.read_bytes()
    sha = hashlib.sha256(full).hexdigest()
    if sha != CODE2_SHA256:
        raise AssertionError(f"CODE.2 sha256 {sha} != certified {CODE2_SHA256}")

    obj = full[OBJ1_FILE_OFF : OBJ1_FILE_OFF + OBJ1_SIZE]
    if len(obj) != OBJ1_SIZE:
        raise AssertionError("CODE.2 object-1 extraction truncated")

    signatures = {
        # Ambient/proximity behavior: +0x3D, creature-sound base, queue.
        0x1B9A7: bytes.fromhex("0f b7 44 02 3d"),
        0x1B9B1: bytes.fromhex("03 05 c0 58 02 00"),
        0x1B9B8: bytes.fromhex("e8 53 c7 00 00"),
        # Main attack path: +0x3F, creature-sound base, queue.
        0x0DA05: bytes.fromhex("0f b7 47 3f"),
        0x0DA0E: bytes.fromhex("03 05 c0 58 02 00"),
        0x0DA15: bytes.fromhex("e8 f6 a6 01 00"),
        # E284 central damage/effect handler: positive HP jumps to E553.
        0x0E31F: bytes.fromhex("66 83 7e 12 00"),
        0x0E324: bytes.fromhex("0f 8f 29 02 00 00"),
        # Surviving damage/effect branch: +0x41 and creature-sound base.
        0x0E55B: bytes.fromhex("0f b7 40 41"),
        0x0E564: bytes.fromhex("03 05 c0 58 02 00"),
        0x0E56B: bytes.fromhex("e8 a0 9b 01 00"),
        # Death branch: +0x43 and distinct death-sound base/fallback.
        0x0E4D7: bytes.fromhex("66 83 78 43 00"),
        0x0E4E2: bytes.fromhex("0f b7 40 43"),
        0x0E4EB: bytes.fromhex("03 05 bc 58 02 00"),
        0x0E4FD: bytes.fromhex("ff 35 bc 58 02 00"),
        0x0E503: bytes.fromhex("e8 08 9c 01 00"),
    }

    for offset, expected in signatures.items():
        actual = obj[offset : offset + len(expected)]
        if actual != expected:
            raise AssertionError(
                f"CODE.2 signature mismatch at 0x{offset:05X}: "
                f"{actual.hex()} != {expected.hex()}"
            )

    print("CODE.2 sound-field signatures: PASS")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("npcs", type=Path)
    parser.add_argument("--code2", type=Path)
    args = parser.parse_args()

    records = parse_npcs(args.npcs)
    print(f"records={len(records)} record_size=0x{RECORD_SIZE:X}")

    for _offset, label in FIELDS:
        values = [record[2][label] for record in records]
        print(
            f"{label}: nonzero={sum(value != 0 for value in values)} "
            f"unique={len(set(values))} min={min(values)} max={max(values)}"
        )

    triples = []
    for index, name, values in records:
        ambient = values["ambient_or_proximity_sound_id"]
        attack = values["attack_sound_id"]
        reaction = values["damage_reaction_sound_id"]
        if ambient and attack == ambient + 1 and reaction == ambient + 2:
            triples.append((index, name, ambient, attack, reaction))

    print(f"consecutive ambient/attack/reaction triples={len(triples)}")
    for index, name, ambient, attack, reaction in triples:
        print(f"  {index:2d} {name}: {ambient}/{attack}/{reaction}")

    if args.code2:
        verify_code2(args.code2)


if __name__ == "__main__":
    main()
