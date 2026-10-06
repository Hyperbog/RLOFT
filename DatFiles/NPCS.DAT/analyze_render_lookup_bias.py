#!/usr/bin/env python3
"""Validate NPCS.DAT +0x27 render lookup-index bias against NPCS/CODE.2."""
from __future__ import annotations

import argparse
import hashlib
import struct
from pathlib import Path

NPC_RECORD_SIZE = 0x45
CODE2_SHA256 = "853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314"
OBJ1_FILE_OFF = 0x3AF24
OBJ1_SIZE = 0x3A852

SIGNATURES = {
    # Serialized NPC definition +0x27 -> generated scene record +0x0C.
    0x2BACE: bytes.fromhex("66 8b 50 27"),
    0x2BAD5: bytes.fromhex("66 89 50 0c"),
    # Generated NPC scene records are class 0.
    0x2BB4E: bytes.fromhex("66 c7 40 08 00 00"),
    # Common scene path reads +0x0C and enters rasterizer 36CEC.
    0x2C649: bytes.fromhex("0f b7 40 0c"),
    0x2C665: bytes.fromhex("e8 82 a6 00 00"),
    # Rasterizer adds argument 4 to the depth/scale-derived lookup index,
    # then indexes the packed table at runtime pointer 0x258C4.
    0x36D63: bytes.fromhex("8b 5d e0"),
    0x36D66: bytes.fromhex("03 5d 14"),
    0x36D7D: bytes.fromhex("8b 3d c4 58 02 00"),
    0x36D83: bytes.fromhex("0f b7 04 5f"),
}


def parse_npcs(path: Path) -> list[tuple[int, str, int]]:
    data = path.read_bytes()
    if len(data) < 4:
        raise ValueError("NPCS.DAT shorter than count")
    count = struct.unpack_from("<I", data, 0)[0]
    expected = 4 + count * NPC_RECORD_SIZE
    if len(data) != expected:
        raise ValueError(f"size {len(data)} != 4 + {count}*0x45 = {expected}")

    records: list[tuple[int, str, int]] = []
    for index in range(count):
        raw = data[
            4 + index * NPC_RECORD_SIZE : 4 + (index + 1) * NPC_RECORD_SIZE
        ]
        name = raw[:20].split(b"\0", 1)[0].decode("ascii", "strict")
        bias = struct.unpack_from("<H", raw, 0x27)[0]
        records.append((index, name, bias))
    return records


def verify_code2(path: Path) -> None:
    full = path.read_bytes()
    actual_sha = hashlib.sha256(full).hexdigest()
    if actual_sha != CODE2_SHA256:
        raise AssertionError(f"CODE.2 sha256 {actual_sha} != certified {CODE2_SHA256}")

    obj = full[OBJ1_FILE_OFF : OBJ1_FILE_OFF + OBJ1_SIZE]
    if len(obj) != OBJ1_SIZE:
        raise AssertionError("CODE.2 object-1 extraction truncated")

    for offset, expected in SIGNATURES.items():
        actual = obj[offset : offset + len(expected)]
        if actual != expected:
            raise AssertionError(
                f"CODE.2 signature mismatch at 0x{offset:05X}: "
                f"{actual.hex()} != {expected.hex()}"
            )

    print("CODE.2 render-bias signatures: PASS")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("npcs", type=Path)
    parser.add_argument("--code2", type=Path)
    args = parser.parse_args()

    records = parse_npcs(args.npcs)
    nonzero = [record for record in records if record[2] != 0]

    print(f"records={len(records)} record_size=0x{NPC_RECORD_SIZE:X}")
    print(f"render_lookup_index_bias_nonzero={len(nonzero)}")
    for index, name, value in nonzero:
        print(f"  {index:2d} {name}: {value}")

    expected_nonzero = [(3, "PYRE ELEMENTAL", 3)]
    if nonzero != expected_nonzero:
        raise AssertionError(f"unexpected +0x27 nonzero set: {nonzero!r}")

    if args.code2:
        verify_code2(args.code2)

    print("render_lookup_index_bias corpus/signature audit: PASS")


if __name__ == "__main__":
    main()
