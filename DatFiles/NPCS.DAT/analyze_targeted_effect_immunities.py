#!/usr/bin/env python3
"""Validate Lightning Bolt and Death Spell NPC immunity aliases against DATs/CODE.2."""
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
SCROLL_TABLE = 0x25DFC
SPELL_TABLE = 0xB740

CASES = {
    "lightning_bolt_effect_immunity": {
        "mask": 0x00000200,
        "object_id": 188,
        "object_name": "MAGE SCROLL OF 'LIGHTNING BOLT'",
        "scroll_target": 0x2634B,
        "effect_id": 28,
        "effect_target": 0xC370,
    },
    "death_spell_effect_immunity": {
        "mask": 0x00000080,
        "object_id": 197,
        "object_name": "MAGE SCROLL OF 'DEATH SPELL'",
        "scroll_target": 0x2642C,
        "effect_id": 51,
        "effect_target": 0xC5EA,
    },
}


def parse_counted(path: Path, stride: int) -> list[bytes]:
    data = path.read_bytes()
    count = struct.unpack_from("<I", data, 0)[0]
    expected = 4 + count * stride
    if len(data) != expected:
        raise ValueError(f"{path}: {len(data)} != 4 + {count}*{stride} = {expected}")
    return [data[4 + i * stride : 4 + (i + 1) * stride] for i in range(count)]


def cstr(raw: bytes) -> str:
    return raw.split(b"\0", 1)[0].decode("ascii", "strict")


def verify_dat_files(npcs_path: Path, objects_path: Path) -> None:
    npcs = parse_counted(npcs_path, NPC_RECORD_SIZE)
    objects = parse_counted(objects_path, OBJ_RECORD_SIZE)
    if len(npcs) != 83:
        raise AssertionError(f"NPC count {len(npcs)} != 83")
    if len(objects) != 336:
        raise AssertionError(f"OBJECTS count {len(objects)} != 336")

    for label, case in CASES.items():
        object_name = cstr(objects[case["object_id"]][:40])
        if object_name != case["object_name"]:
            raise AssertionError(f"{label}: object name {object_name!r}")

        hits = []
        for index, record in enumerate(npcs):
            flags = struct.unpack_from("<I", record, 0x31)[0]
            if flags & case["mask"]:
                hits.append((index, cstr(record[:20])))

        print(f"{label}=0x{case['mask']:08X} count={len(hits)}")
        print("  " + ", ".join(f"{index}:{name}" for index, name in hits))
        print(
            f"  object={case['object_id']} effect={case['effect_id']} "
            f"spell={object_name}"
        )


def verify_code2(path: Path) -> None:
    full = path.read_bytes()
    actual_sha = hashlib.sha256(full).hexdigest()
    if actual_sha != CODE2_SHA256:
        raise AssertionError(f"CODE.2 sha256 {actual_sha} != certified {CODE2_SHA256}")

    obj = full[OBJ1_FILE_OFF : OBJ1_FILE_OFF + OBJ1_SIZE]
    if len(obj) != OBJ1_SIZE:
        raise AssertionError("CODE.2 object-1 extraction truncated")

    # Serialized mage-scroll object 188 -> effect 28.
    case = CASES["lightning_bolt_effect_immunity"]
    index = case["object_id"] - 0xA7
    target = struct.unpack_from("<I", obj, SCROLL_TABLE + index * 4)[0]
    if target != case["scroll_target"]:
        raise AssertionError(f"Lightning scroll target 0x{target:X}")
    if obj[0x2634D:0x26354] != bytes.fromhex("6a 1c e8 78 f7 ff ff"):
        raise AssertionError("Lightning scroll effect-ID stub mismatch")
    target = struct.unpack_from("<I", obj, SPELL_TABLE + case["effect_id"] * 4)[0]
    if target != case["effect_target"]:
        raise AssertionError(f"Lightning effect target 0x{target:X}")

    # Dispatcher initializes ESI=1; effect 28 forwards ESI as constructor arg1.
    if obj[0xB95A:0xB961] != bytes.fromhex("6a 00 be 01 00 00 00"):
        raise AssertionError("spell dispatcher ESI=1 initialization mismatch")
    lightning_call = bytes.fromhex(
        "ff 35 d0 55 02 00 ff 35 d4 55 02 00 53 50 56 e8 f3 c6 ff ff"
    )
    if obj[0xC379:0xC38D] != lightning_call:
        raise AssertionError("effect-28 moving-record call mismatch")

    # 8A80 stores arg1 into transient record +0x0E.
    if obj[0x8AE3:0x8AEB] != bytes.fromhex("8b 44 24 34 66 89 43 0e"):
        raise AssertionError("moving-record type store mismatch")

    # Transient type 1 dispatches to 8E77 and tests NPC byte +0x32 mask 0x03.
    if struct.unpack_from("<I", obj, 0x8CD8 + 4)[0] != 0x8E77:
        raise AssertionError("moving-effect type-1 dispatch mismatch")
    lightning_mask_sig = bytes.fromhex(
        "0f b7 87 8e 4d 01 00 6b c0 45 8b 15 30 22 02 00 f6 44 02 32 03"
    )
    if obj[0x8E77:0x8E8C] != lightning_mask_sig:
        raise AssertionError("Lightning NPC immunity-mask instruction mismatch")

    # Serialized mage-scroll object 197 -> effect 51 -> AED8.
    case = CASES["death_spell_effect_immunity"]
    index = case["object_id"] - 0xA7
    target = struct.unpack_from("<I", obj, SCROLL_TABLE + index * 4)[0]
    if target != case["scroll_target"]:
        raise AssertionError(f"Death Spell scroll target 0x{target:X}")
    if obj[0x2642E:0x26435] != bytes.fromhex("6a 33 e8 97 f6 ff ff"):
        raise AssertionError("Death Spell effect-ID stub mismatch")
    target = struct.unpack_from("<I", obj, SPELL_TABLE + case["effect_id"] * 4)[0]
    if target != case["effect_target"]:
        raise AssertionError(f"Death Spell effect target 0x{target:X}")
    if obj[0xC5EA:0xC5F3] != bytes.fromhex("53 e8 e8 e8 ff ff 83 c4 04"):
        raise AssertionError("effect-51 -> AED8 call mismatch")

    # AED8 tests NPC definition +0x31 bit 0x80 and skips target when set.
    death_mask_sig = bytes.fromhex(
        "6b d0 45 a1 30 22 02 00 01 d0 f6 40 31 80 75 1e"
    )
    if obj[0xAFC4:0xAFD4] != death_mask_sig:
        raise AssertionError("Death Spell NPC immunity-mask instruction mismatch")

    print("CODE.2 targeted-effect chain verification: PASS")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("npcs", type=Path)
    parser.add_argument("objects", type=Path)
    parser.add_argument("--code2", type=Path)
    args = parser.parse_args()

    verify_dat_files(args.npcs, args.objects)
    if args.code2:
        verify_code2(args.code2)


if __name__ == "__main__":
    main()
