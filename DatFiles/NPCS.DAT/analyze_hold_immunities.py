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
SCROLL_TABLE = 0x25DFC
SCROLL_OBJECT_BASE = 0xA7

# dispatch id -> (dispatcher target, mask site, mask, direct serialized scroll object or None, label)
CASES = {
    24: (0xC123, 0xC15F, 0x0100, 184, "Slow"),
    25: (0xC201, 0xC23C, 0x0108, 185, "Hold Person shared/mage path"),
    26: (0xC179, 0xC1B4, 0x0102, 186, "Hold Undead"),
    41: (0xC2E0, 0xC317, 0x0110, 195, "Hold Monster"),
    92: (0xC273, 0xC2AF, 0x0108, None, "prepared-priest Hold Person path"),
}

SCROLL_NAMES = {
    184: "MAGE SCROLL OF 'SLOW'",
    185: "MAGE SCROLL OF 'HOLD PERSON'",
    186: "MAGE SCROLL OF 'HOLD UNDEAD'",
    195: "MAGE SCROLL OF 'HOLD MONSTER'",
    211: "CLERIC SCROLL OF 'HOLD PERSON'",
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
    for object_id, expected in SCROLL_NAMES.items():
        actual = cstr(records[object_id][:40])
        if actual != expected:
            raise AssertionError(f"object {object_id}: {actual!r} != {expected!r}")
        print(f"object={object_id:3d} spell={actual}")
    print("OBJECTS scroll-name cross-check: PASS")


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


def scroll_target(obj: bytes, object_id: int) -> int:
    index = object_id - SCROLL_OBJECT_BASE
    return struct.unpack_from("<I", obj, SCROLL_TABLE + index * 4)[0]


def verify_code2(path: Path) -> None:
    full = path.read_bytes()
    sha = hashlib.sha256(full).hexdigest()
    if sha != CODE2_SHA256:
        raise AssertionError(f"CODE.2 sha256 {sha} != certified {CODE2_SHA256}")

    obj = full[OBJ1_FILE_OFF: OBJ1_FILE_OFF + OBJ1_SIZE]
    if len(obj) != OBJ1_SIZE:
        raise AssertionError("CODE.2 object-1 extraction truncated")

    for dispatch_id, (target, site, mask, _object_id, label) in CASES.items():
        actual_target = struct.unpack_from("<I", obj, JUMP_TABLE + dispatch_id * 4)[0]
        if actual_target != target:
            raise AssertionError(
                f"dispatch {dispatch_id}: target 0x{actual_target:x} != expected 0x{target:x}"
            )

        if mask == 0x0100:
            expected_sig = bytes.fromhex("f6 44 02 32 01")
        else:
            expected_sig = b"\x66\xf7\x44\x02\x31" + struct.pack("<H", mask)
        actual_sig = obj[site: site + len(expected_sig)]
        if actual_sig != expected_sig:
            raise AssertionError(
                f"dispatch {dispatch_id}: mask instruction mismatch at 0x{site:x}: "
                f"{actual_sig.hex()}"
            )
        print(
            f"dispatch={dispatch_id:3d} target=0x{target:05x} mask=0x{mask:04x} "
            f"site=0x{site:05x} label={label}"
        )

    # Direct scroll mappings: these four named mage scrolls pass the same compact IDs.
    direct_scroll_dispatch = {184: 24, 185: 25, 186: 26, 195: 41}
    for object_id, dispatch_id in direct_scroll_dispatch.items():
        target = scroll_target(obj, object_id)
        if target >= len(obj):
            raise AssertionError(f"object {object_id}: scroll target 0x{target:x} outside object1")
        # Common scroll stubs begin `push edi; push esi; push imm8`.
        sig = obj[target:target + 4]
        expected = bytes((0x57, 0x56, 0x6A, dispatch_id))
        if sig != expected:
            raise AssertionError(
                f"object {object_id}: scroll dispatch stub {sig.hex()} != {expected.hex()}"
            )
        print(f"scroll object={object_id:3d} -> dispatch={dispatch_id:3d} target=0x{target:05x}")

    # Cleric Hold Person scroll intentionally aliases the dispatch-25 stub rather than ID 92.
    mage_hold_target = scroll_target(obj, 185)
    cleric_hold_target = scroll_target(obj, 211)
    if cleric_hold_target != mage_hold_target:
        raise AssertionError(
            f"cleric Hold Person target 0x{cleric_hold_target:x} != mage/shared target 0x{mage_hold_target:x}"
        )
    if obj[cleric_hold_target:cleric_hold_target + 4] != bytes.fromhex("57 56 6a 19"):
        raise AssertionError("cleric Hold Person shared stub does not push dispatch 25")
    print(
        f"cleric Hold Person object=211 reuses object=185 target=0x{cleric_hold_target:05x} "
        "and pushes dispatch=25: PASS"
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
    print("CODE.2 dispatcher/mask/scroll verification: PASS")


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
