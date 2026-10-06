#!/usr/bin/env python3
"""Validate NPC poison/disease/paralysis status mappings against DATs/CODE.2."""
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

SPELLS = {
    100: (0xBF6C, 215, "CLERIC SCROLL OF 'CURE DISEASE'", 0x64),
    104: (0xBFA2, 219, "CLERIC SCROLL OF 'REMOVE PARALYSIS'", 0x68),
    113: (0xBF87, 223, "CLERIC SCROLL OF 'NEUTRALIZE POISON'", 0x71),
}


def parse_counted(path: Path, stride: int) -> list[bytes]:
    data = path.read_bytes()
    count = struct.unpack_from("<I", data, 0)[0]
    expected = 4 + count * stride
    if len(data) != expected:
        raise ValueError(f"{path}: {len(data)} != {expected}")
    return [data[4+i*stride:4+(i+1)*stride] for i in range(count)]


def cstr(raw: bytes) -> str:
    return raw.split(b"\0", 1)[0].decode("ascii", "strict")


def verify_npcs(path: Path) -> None:
    records = parse_counted(path, NPC_RECORD_SIZE)
    poison = [cstr(r[:20]) for r in records if r[0x39] & 0x01]
    disease = [cstr(r[:20]) for r in records if r[0x3A] & 0x10]
    paralysis = [cstr(r[:20]) for r in records if r[0x39] & 0x20]

    expected_poison = ["DARKLING"]
    expected_disease = ["ZOMBIE LORD", "GHOUL LORD", "INQUISITOR"]
    if poison != expected_poison:
        raise AssertionError(f"poison set {poison} != {expected_poison}")
    if disease != expected_disease:
        raise AssertionError(f"disease set {disease} != {expected_disease}")

    print(f"poison_attack count={len(poison)} names={poison}")
    print(f"disease_attack count={len(disease)} names={disease}")
    print(f"paralysis_attack count={len(paralysis)} names={paralysis}")


def verify_objects(path: Path) -> None:
    records = parse_counted(path, OBJ_RECORD_SIZE)
    for effect_id, (_target, object_id, expected_name, code) in SPELLS.items():
        actual = cstr(records[object_id][:40])
        if actual != expected_name:
            raise AssertionError(f"object {object_id}: {actual!r} != {expected_name!r}")
        print(f"effect={effect_id} object={object_id} code=0x{code:02x} spell={actual}")
    print("OBJECTS cure-name cross-check: PASS")


def verify_code2(path: Path) -> None:
    full = path.read_bytes()
    sha = hashlib.sha256(full).hexdigest()
    if sha != CODE2_SHA256:
        raise AssertionError(f"CODE.2 sha256 {sha} != certified {CODE2_SHA256}")
    obj = full[OBJ1_FILE_OFF:OBJ1_FILE_OFF+OBJ1_SIZE]
    if len(obj) != OBJ1_SIZE:
        raise AssertionError("object-1 extraction truncated")

    for effect_id, (target, _object_id, _name, code) in SPELLS.items():
        actual_target = struct.unpack_from("<I", obj, JUMP_TABLE + effect_id*4)[0]
        if actual_target != target:
            raise AssertionError(
                f"effect {effect_id}: target 0x{actual_target:x} != 0x{target:x}"
            )
        # Each handler pushes its internal character-effect code immediately before
        # the final -1 argument and CALL A254. Search a tight handler window.
        window = obj[target:target+0x40]
        signature = b"\x6a" + bytes([code]) + b"\x6a\xff"
        if signature not in window:
            raise AssertionError(
                f"effect {effect_id}: internal code push 0x{code:02x} not found"
            )

    # NPC attack-side setters.
    attack_sigs = {
        0xD947: bytes.fromhex("f6 47 39 01"),
        0xD960: bytes.fromhex("80 88 f1 18 02 00 08"),
        0xD967: bytes.fromhex("f6 47 3a 10"),
        0xD98C: bytes.fromhex("80 8b f1 18 02 00 04"),
    }
    for site, sig in attack_sigs.items():
        if obj[site:site+len(sig)] != sig:
            raise AssertionError(f"attack signature mismatch at 0x{site:x}")

    # Character effect dispatcher maps internal code to exact status clear.
    clear_sigs = {
        0xA18C: bytes.fromhex("80 a0 f1 18 02 00 f7"),  # code 0x71 -> clear 0x08
        0xA1C0: bytes.fromhex("80 a6 f1 18 02 00 fb"),  # code 0x64 -> clear 0x04
        0xA1E4: bytes.fromhex("80 a0 f1 18 02 00 ef"),  # code 0x68 -> clear 0x10
    }
    for site, sig in clear_sigs.items():
        if obj[site:site+len(sig)] != sig:
            raise AssertionError(f"clear signature mismatch at 0x{site:x}")

    # Confirm dispatcher comparisons route these exact internal codes.
    cmp_sigs = {
        0x9E24: bytes.fromhex("83 fa 64"),
        0x9DA8: bytes.fromhex("83 fa 68"),
        0x9DFC: bytes.fromhex("83 fa 71"),
    }
    for site, sig in cmp_sigs.items():
        if obj[site:site+len(sig)] != sig:
            raise AssertionError(f"dispatcher comparison mismatch at 0x{site:x}")

    print("CODE.2 status setter/clearer verification: PASS")


def main() -> None:
    ap = argparse.ArgumentParser()
    ap.add_argument("npcs", type=Path)
    ap.add_argument("objects", type=Path)
    ap.add_argument("--code2", type=Path)
    args = ap.parse_args()
    verify_npcs(args.npcs)
    verify_objects(args.objects)
    if args.code2:
        verify_code2(args.code2)


if __name__ == "__main__":
    main()
