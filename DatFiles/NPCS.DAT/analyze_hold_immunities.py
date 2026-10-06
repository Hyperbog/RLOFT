#!/usr/bin/env python3
"""Validate NPC hold-status immunity aliases against DATs and certified CODE.2."""
from __future__ import annotations

import argparse
import hashlib
import struct
from pathlib import Path

NPC_RECORD_SIZE = 0x45
OBJ_RECORD_SIZE = 0x41
CODE2_SHA256 = "853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314"
OBJ1_FILE_OFF = 0x3AF24
OBJ1_SIZE = 0x3A852
JUMP_TABLE = 0xB740

# effect id -> (dispatcher target, mask site, mask, serialized spell-scroll object id, name)
CASES = {
    24: (0xC123, 0xC15F, 0x0100, 184, "MAGE SCROLL OF 'SLOW'"),
    25: (0xC201, 0xC23C, 0x0108, 185, "MAGE SCROLL OF 'HOLD PERSON'"),
    26: (0xC179, 0xC1B4, 0x0102, 186, "MAGE SCROLL OF 'HOLD UNDEAD'"),
    41: (0xC2E0, 0xC317, 0x0110, 195, "MAGE SCROLL OF 'HOLD MONSTER'"),
    92: (0xC273, 0xC2AF, 0x0108, 211, "CLERIC SCROLL OF 'HOLD PERSON'"),
}

ALIASES = {
    0x0002: "hold_undead_effect_immunity",
    0x0008: "hold_person_effect_immunity",
    0x0010: "hold_monster_effect_immunity",
}


def parse_counted(path: Path, stride: int) -> list[bytes]:
    data = path.read_bytes()
    if len(data) < 4:
        raise ValueError(f"{path}: shorter than count")
    count = struct.unpack_from("<I", data, 0)[0]
    expected = 4 + count * stride
    if len(data) != expected:
        raise ValueError(f"{path}: {len(data)} != 4 + {count}*{stride} = {expected}")
    return [data[4 + i * stride: 4 + (i + 1) * stride] for i in range(count)]


def cstr(raw: bytes) -> str:
    return raw.split(b"\0", 1)[0].decode("ascii", "strict")


def verify_objects(path: Path) -> None:
    records = parse_counted(path, OBJ_RECORD_SIZE)
    for effect_id, (_target, _site, _mask, object_id, expected) in CASES.items():
        actual = cstr(records[object_id][:40])
        if actual != expected:
            raise AssertionError(
                f"effect {effect_id}: object {object_id} {actual!r} != {expected!r}"
            )
        print(f"effect={effect_id:3d} object={object_id:3d} spell={actual}")
    print("OBJECTS spell-order cross-check: PASS")


def verify_npcs(path: Path) -> None:
    records = parse_counted(path, NPC_RECORD_SIZE)
    print(f"NPCS records={len(records)}")
    for bit, label in ALIASES.items():
        names = [
            cstr(r[:20])
            for r in records
            if struct.unpack_from("<I", r, 0x31)[0] & bit
        ]
        print(f"{label} bit=0x{bit:04x} count={len(names)}")
        print("  " + ", ".join(names))


def verify_code2(path: Path) -> None:
    full = path.read_bytes()
    sha = hashlib.sha256(full).hexdigest()
    if sha != CODE2_SHA256:
        raise AssertionError(f"CODE.2 sha256 {sha} != certified {CODE2_SHA256}")

    obj = full[OBJ1_FILE_OFF: OBJ1_FILE_OFF + OBJ1_SIZE]
    if len(obj) != OBJ1_SIZE:
        raise AssertionError("CODE.2 object-1 extraction truncated")

    for effect_id, (target, site, mask, _object_id, expected_name) in CASES.items():
        actual_target = struct.unpack_from("<I", obj, JUMP_TABLE + effect_id * 4)[0]
        if actual_target != target:
            raise AssertionError(
                f"effect {effect_id}: target 0x{actual_target:x} != expected 0x{target:x}"
            )

        if mask == 0x0100:
            expected_sig = bytes.fromhex("f6 44 02 32 01")
        else:
            expected_sig = b"\x66\xf7\x44\x02\x31" + struct.pack("<H", mask)
        actual_sig = obj[site: site + len(expected_sig)]
        if actual_sig != expected_sig:
            raise AssertionError(
                f"effect {effect_id}: mask instruction mismatch at 0x{site:x}: "
                f"{actual_sig.hex()}"
            )
        print(
            f"effect={effect_id:3d} target=0x{target:05x} mask=0x{mask:04x} "
            f"site=0x{site:05x} spell={expected_name}"
        )

    # All four hold handlers set the same low live runtime-state bit.
    hold_or_signatures = {
        0xC1CA: bytes.fromhex("80 88 88 4d 01 00 08"),
        0xC252: bytes.fromhex("80 88 88 4d 01 00 08"),
        0xC2BF: bytes.fromhex("80 8f 88 4d 01 00 08"),
        0xC32D: bytes.fromhex("80 88 88 4d 01 00 08"),
    }
    for site, sig in hold_or_signatures.items():
        if obj[site: site + len(sig)] != sig:
            raise AssertionError(f"hold-state OR mismatch at 0x{site:x}")

    print("shared live held-state bit (+8 |= 0x08): PASS")
    print("CODE.2 dispatcher/mask verification: PASS")


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
