#!/usr/bin/env python3
"""Validate NPC Detect Undead exclusion and turning-roll modifier flags."""
from __future__ import annotations

import argparse
import hashlib
import struct
from pathlib import Path

NPC_RECORD_SIZE = 0x45
OBJECT_RECORD_SIZE = 0x41
CODE2_SHA256 = "853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314"
OBJ1_FILE_OFF = 0x3AF24
OBJ1_SIZE = 0x3A852
SPELL_EFFECT_JUMP_TABLE = 0xB740
SCROLL_JUMP_TABLE = 0x25DFC

ALIASES = {
    0x00000001: "detect_undead_effect_exclusion",
    0x00000004: "turn_undead_roll_penalty_4",
}


def parse_counted(path: Path, stride: int) -> list[bytes]:
    data = path.read_bytes()
    if len(data) < 4:
        raise ValueError(f"{path}: shorter than count")
    count = struct.unpack_from("<I", data, 0)[0]
    expected = 4 + count * stride
    if len(data) != expected:
        raise ValueError(f"{path}: {len(data)} != 4 + {count}*{stride} = {expected}")
    return [data[4 + i * stride : 4 + (i + 1) * stride] for i in range(count)]


def cstr(raw: bytes) -> str:
    return raw.split(b"\0", 1)[0].decode("ascii", "strict")


def verify_npcs(path: Path) -> None:
    records = parse_counted(path, NPC_RECORD_SIZE)
    print(f"NPCS records={len(records)}")
    for mask, label in ALIASES.items():
        hits = []
        for i, raw in enumerate(records):
            flags = struct.unpack_from("<I", raw, 0x31)[0]
            if flags & mask:
                hits.append(f"{i}:{cstr(raw[:20])}")
        print(f"{label} mask=0x{mask:08X} count={len(hits)}")
        print("  " + ", ".join(hits))

    undead = []
    both = []
    for i, raw in enumerate(records):
        name = cstr(raw[:20])
        flags = struct.unpack_from("<I", raw, 0x31)[0]
        if raw[0x3A] & 0x20:
            undead.append(f"{i}:{name}")
            if flags & 0x00000004:
                both.append(f"{i}:{name}")
    print(f"undead_classification count={len(undead)}")
    print(f"undead + turn_penalty_4 intersection={len(both)}")
    if both:
        print("  " + ", ".join(both))


def verify_objects(path: Path) -> None:
    records = parse_counted(path, OBJECT_RECORD_SIZE)
    expected = "MAGE SCROLL OF 'DETECT UNDEAD'"
    actual = cstr(records[170][:40])
    if actual != expected:
        raise AssertionError(f"object 170 {actual!r} != {expected!r}")
    print(f"object=170 spell={actual}")


def require(obj: bytes, site: int, hex_bytes: str, label: str) -> None:
    expected = bytes.fromhex(hex_bytes)
    actual = obj[site : site + len(expected)]
    if actual != expected:
        raise AssertionError(
            f"{label} signature mismatch at 0x{site:x}: {actual.hex()} != {expected.hex()}"
        )


def verify_code2(path: Path) -> None:
    full = path.read_bytes()
    sha = hashlib.sha256(full).hexdigest()
    if sha != CODE2_SHA256:
        raise AssertionError(f"CODE.2 sha256 {sha} != certified {CODE2_SHA256}")
    obj = full[OBJ1_FILE_OFF : OBJ1_FILE_OFF + OBJ1_SIZE]
    if len(obj) != OBJ1_SIZE:
        raise AssertionError("CODE.2 object-1 extraction truncated")

    # Object 170 is mage-scroll table index 3 relative to object base 167.
    scroll_target = struct.unpack_from("<I", obj, SCROLL_JUMP_TABLE + 3 * 4)[0]
    if scroll_target != 0x2618A:
        raise AssertionError(f"Detect Undead scroll stub 0x{scroll_target:x} != 0x2618a")
    require(obj, 0x2618A, "57 56 6a 03 e8 39 f9 ff ff", "Detect Undead scroll stub")

    effect_target = struct.unpack_from("<I", obj, SPELL_EFFECT_JUMP_TABLE + 3 * 4)[0]
    if effect_target != 0xBC2A:
        raise AssertionError(f"effect 3 target 0x{effect_target:x} != 0xbc2a")
    require(obj, 0xBC5E, "66 ff 05 3c f0 00 00", "Detect Undead active-counter increment")
    require(obj, 0x88A8, "66 ff 0d 3c f0 00 00", "Detect Undead active-counter decrement")

    # With the alternate detection source absent, bit 0x1 excludes the NPC from
    # the Detect Undead marker path.
    require(obj, 0x2C6BE, "66 83 3d 3c f0 00 00 00", "Detect Undead render-state test")
    require(obj, 0x2C6FE, "66 83 3d 3e f0 00 00 00", "alternate detection-state test")
    require(obj, 0x2C724, "f6 40 31 01", "Detect Undead exclusion flag")

    # Internal effect 88 reaches the undead-turning routine. That routine first
    # filters on the proved undead bit; behavior_flags bit 0x4 then subtracts
    # four from the roll before the decisive table-threshold comparison.
    turn_target = struct.unpack_from("<I", obj, SPELL_EFFECT_JUMP_TABLE + 88 * 4)[0]
    if turn_target != 0xC68F:
        raise AssertionError(f"effect 88 target 0x{turn_target:x} != 0xc68f")
    require(obj, 0xC69D, "6a 00 53 e8 73 ee ff ff", "turning-routine call")
    require(obj, 0xB5C7, "f6 44 02 3a 20", "undead-classification filter")
    require(obj, 0xB6B0, "f6 44 02 31 04", "turn-roll penalty flag")
    require(obj, 0xB6B7, "83 6c 24 1c 04", "turn-roll subtract four")
    require(obj, 0xB6C3, "3b 44 24 1c", "turn threshold comparison")

    print("Detect Undead scroll/effect/render exclusion chain: PASS")
    print("Undead-turning roll -4 modifier chain: PASS")
    print("CODE.2 detection/turning verification: PASS")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("npcs", type=Path)
    parser.add_argument("objects", type=Path)
    parser.add_argument("--code2", type=Path)
    args = parser.parse_args()
    verify_npcs(args.npcs)
    verify_objects(args.objects)
    if args.code2:
        verify_code2(args.code2)


if __name__ == "__main__":
    main()
