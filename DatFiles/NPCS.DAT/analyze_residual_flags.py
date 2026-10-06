#!/usr/bin/env python3
"""Audit unresolved NPCS.DAT behavior-flag capacity without inventing semantics."""
from __future__ import annotations

import argparse
import hashlib
import struct
from pathlib import Path

NPC_RECORD_SIZE = 0x45
FLAGS_OFF = 0x31
FLAGS39_OFF = 0x39
FLAGS3A_OFF = 0x3A

CODE2_SHA256 = "853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314"
OBJ1_FILE_OFF = 0x3AF24
OBJ1_SIZE = 0x3A852

PROVED_DWORD = (
    0x00000001 | 0x00000002 | 0x00000004 | 0x00000008 | 0x00000010 |
    0x00000080 | 0x00000100 | 0x00000200 | 0x00000800 | 0x00002000 |
    0x00004000 | 0x00008000
)
PROVED_39 = 0x01 | 0x20 | 0x40 | 0x80
PROVED_3A = 0x02 | 0x04 | 0x10 | 0x20

EXPECTED_POPULATED_DWORD = 0x000FFFFF
EXPECTED_RESIDUAL_DWORD = 0x000F1460
EXPECTED_POPULATED_39 = 0xFF
EXPECTED_RESIDUAL_39 = 0x1E
EXPECTED_POPULATED_3A = 0x36
EXPECTED_RESIDUAL_3A = 0x00

SIGNATURES = {
    0xB2FB: bytes.fromhex("8b4831854c2448"),
    0xC919: bytes.fromhex("8b4931"),
    0xC95F: bytes.fromhex("f6c580"),
    0xC966: bytes.fromhex("f6c540"),
    0xC96D: bytes.fromhex("f7c100000200"),
    0xC980: bytes.fromhex("f7c100000400"),
}

AREA_MASK_PUSHES = {
    0xB9AA: 0x2100,
    0xBA0C: 0x2100,
    0xBA86: 0x0900,
    0xBAE7: 0x0900,
    0x27373: 0x2100,
}


def parse_npcs(path: Path):
    data = path.read_bytes()
    count = struct.unpack_from("<I", data, 0)[0]
    expected = 4 + count * NPC_RECORD_SIZE
    if len(data) != expected:
        raise ValueError(f"size {len(data)} != {expected}")
    records = []
    for i in range(count):
        raw = data[4 + i * NPC_RECORD_SIZE : 4 + (i + 1) * NPC_RECORD_SIZE]
        name = raw[:20].split(b"\0", 1)[0].decode("ascii")
        records.append(
            (
                i,
                name,
                struct.unpack_from("<I", raw, FLAGS_OFF)[0],
                raw[FLAGS39_OFF],
                raw[FLAGS3A_OFF],
            )
        )
    return records


def populated_mask(records, field_index: int) -> int:
    value = 0
    for record in records:
        value |= record[field_index]
    return value


def member_list(records, field_index: int, mask: int):
    return [(record[0], record[1]) for record in records if record[field_index] & mask]


def check_code2(path: Path) -> None:
    data = path.read_bytes()
    digest = hashlib.sha256(data).hexdigest()
    if digest != CODE2_SHA256:
        raise ValueError(f"CODE.2 SHA256 {digest} != certified {CODE2_SHA256}")
    obj = data[OBJ1_FILE_OFF : OBJ1_FILE_OFF + OBJ1_SIZE]
    if len(obj) != OBJ1_SIZE:
        raise ValueError("CODE.2 object-1 extraction short")
    for off, signature in SIGNATURES.items():
        actual = obj[off : off + len(signature)]
        if actual != signature:
            raise ValueError(
                f"signature mismatch at logical 0x{off:X}: "
                f"{actual.hex()} != {signature.hex()}"
            )
    for off, mask in AREA_MASK_PUSHES.items():
        expected = b"\x68" + struct.pack("<I", mask)
        actual = obj[off : off + 5]
        if actual != expected:
            raise ValueError(
                f"area-mask push mismatch at 0x{off:X}: "
                f"{actual.hex()} != {expected.hex()}"
            )
    print("CODE2_certified_sha=PASS")
    print("full_dword_consumers=B2FB_generic_effect_mask,C919_weapon_gate")
    print("generic_effect_masks=0x2100,0x2100,0x0900,0x0900,0x2100")
    print("weapon_gate_raw_high_bits=0x00020000,0x00040000")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("npcs", type=Path)
    parser.add_argument("--code2", type=Path)
    args = parser.parse_args()

    records = parse_npcs(args.npcs)
    pop_dword = populated_mask(records, 2)
    pop_39 = populated_mask(records, 3)
    pop_3a = populated_mask(records, 4)
    residual_dword = pop_dword & ~PROVED_DWORD
    residual_39 = pop_39 & ~PROVED_39
    residual_3a = pop_3a & ~PROVED_3A

    checks = [
        ("populated_behavior_flags", pop_dword, EXPECTED_POPULATED_DWORD),
        ("residual_behavior_flags", residual_dword, EXPECTED_RESIDUAL_DWORD),
        ("populated_flags39", pop_39, EXPECTED_POPULATED_39),
        ("residual_flags39", residual_39, EXPECTED_RESIDUAL_39),
        ("populated_flags3A", pop_3a, EXPECTED_POPULATED_3A),
        ("residual_flags3A", residual_3a, EXPECTED_RESIDUAL_3A),
    ]
    print(f"records={len(records)}")
    for label, actual, expected in checks:
        status = "PASS" if actual == expected else "FAIL"
        print(f"{label}=0x{actual:08X} expected=0x{expected:08X} {status}")
        if actual != expected:
            raise SystemExit(1)

    for mask in (0x20, 0x40, 0x400, 0x1000, 0x10000, 0x20000, 0x40000, 0x80000):
        hits = member_list(records, 2, mask)
        print(
            f"raw_dword_0x{mask:08X}={len(hits)} "
            + ", ".join(f"{index}:{name}" for index, name in hits)
        )
    for mask in (0x02, 0x04, 0x08, 0x10):
        hits = member_list(records, 3, mask)
        print(
            f"raw_flags39_0x{mask:02X}={len(hits)} "
            + ", ".join(f"{index}:{name}" for index, name in hits)
        )

    if args.code2:
        check_code2(args.code2)


if __name__ == "__main__":
    main()
