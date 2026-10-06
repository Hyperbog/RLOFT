#!/usr/bin/env python3
"""Validate NPC weapon-category damage gates against NPCS/OBJECTS/CODE.2."""
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

ALIASES = {
    0x00004000: "weapon_category10_damage_floor_one",
    0x00008000: "weapon_category9_damage_floor_one",
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


def verify_objects(path: Path) -> None:
    records = parse_counted(path, OBJECT_RECORD_SIZE)
    for category in (9, 10, 11):
        hits = [
            (index, cstr(record[:40]), record[0x37])
            for index, record in enumerate(records)
            if record[0x2F] == category
        ]
        print(f"OBJECT category {category}: {len(hits)} records")
        print(
            "  "
            + ", ".join(
                f"{index}:{name}(ench={enchantment})"
                for index, name, enchantment in hits
            )
        )

    category9_names = {cstr(r[:40]) for r in records if r[0x2F] == 9}
    required9 = {"MACE", "QUARTERSTAFF", "WARHAMMER"}
    if not required9.issubset(category9_names):
        raise AssertionError(f"category 9 missing expected shipped family: {required9-category9_names}")

    required10 = {
        "BATTLE AXE",
        "SHORT SWORD",
        "LONG SWORD",
        "BROAD SWORD",
        "DAGGER",
        "TWO-HANDED SWORD",
        "HALBERD",
        "SPEAR",
        "THROWING KNIFE",
    }
    category10_names = {cstr(r[:40]) for r in records if r[0x2F] == 10}
    if not required10.issubset(category10_names):
        raise AssertionError(
            f"category 10 missing expected shipped family: {required10-category10_names}"
        )

    print("OBJECTS category/enchantment cross-check: PASS")


def verify_npcs(path: Path) -> None:
    records = parse_counted(path, NPC_RECORD_SIZE)
    print(f"NPCS records={len(records)}")

    for bit, label in ALIASES.items():
        names = [
            cstr(record[:20])
            for record in records
            if struct.unpack_from("<I", record, 0x31)[0] & bit
        ]
        print(f"{label} bit=0x{bit:08x} count={len(names)}")
        print("  " + ", ".join(names))

    for bit in (0x00020000, 0x00040000, 0x00080000):
        names = [
            cstr(record[:20])
            for record in records
            if struct.unpack_from("<I", record, 0x31)[0] & bit
        ]
        print(f"unresolved bit=0x{bit:08x} count={len(names)}")
        print("  " + ", ".join(names))


def verify_code2(path: Path) -> None:
    full = path.read_bytes()
    actual_sha = hashlib.sha256(full).hexdigest()
    if actual_sha != CODE2_SHA256:
        raise AssertionError(f"CODE.2 sha256 {actual_sha} != certified {CODE2_SHA256}")

    obj = full[OBJ1_FILE_OFF : OBJ1_FILE_OFF + OBJ1_SIZE]
    if len(obj) != OBJ1_SIZE:
        raise AssertionError("CODE.2 object-1 extraction truncated")

    signatures = {
        # behavior flags, category and enchantment loads
        0xC919: bytes.fromhex("8b4931"),
        0xC936: bytes.fromhex("0fb65a2f"),
        0xC93A: bytes.fromhex("807a3700"),
        # nonzero enchantment -> synthetic category 11; weapon id -1 -> category 9
        0xC940: bytes.fromhex("bb0b000000"),
        0xC947: bytes.fromhex("bb09000000"),
        # category-9/category-10 flag tests (CH bits 7/6 => dword bits 15/14)
        0xC95F: bytes.fromhex("f6c580"),
        0xC966: bytes.fromhex("f6c540"),
        # category-11 high-bit branches and zero-extended enchantment loads
        0xC96D: bytes.fromhex("f7c100000200"),
        0xC975: bytes.fromhex("0fb65a37"),
        0xC979: bytes.fromhex("83fbfe"),
        0xC980: bytes.fromhex("f7c100000400"),
        0xC988: bytes.fromhex("0fb65237"),
        0xC98C: bytes.fromhex("83fafe"),
        # final minimum-damage clamp
        0xC993: bytes.fromhex("83f801"),
        0xC998: bytes.fromhex("b801000000"),
        # call immediately before E284 damage application
        0xE947: bytes.fromhex("55ff74240856e8bedfffff"),
    }

    for offset, expected in signatures.items():
        actual = obj[offset : offset + len(expected)]
        if actual != expected:
            raise AssertionError(
                f"CODE.2 signature mismatch at 0x{offset:x}: "
                f"{actual.hex()} != {expected.hex()}"
            )

    print("CODE.2 weapon damage gate signatures: PASS")
    print("category-11 signed-immediate/zero-extend oddity preserved: PASS")
    print("minimum returned damage clamp (>=1): PASS")


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
