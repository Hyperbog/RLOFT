#!/usr/bin/env python3
"""Validate NPCS.DAT +0x27 render-scale lookup bias against DAT/CODE.2."""
from __future__ import annotations

import argparse
import hashlib
import struct
from pathlib import Path

NPC_RECORD_SIZE = 0x45
CODE2_SHA256 = "853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314"
OBJ1_FILE_OFF = 0x3AF24
OBJ1_SIZE = 0x3A852


def parse_npcs(path: Path):
    data = path.read_bytes()
    if len(data) < 4:
        raise ValueError("NPCS.DAT shorter than count")
    count = struct.unpack_from("<I", data, 0)[0]
    expected = 4 + count * NPC_RECORD_SIZE
    if len(data) != expected:
        raise ValueError(f"size {len(data)} != 4 + {count}*0x45 = {expected}")

    records = []
    for index in range(count):
        raw = data[4 + index * NPC_RECORD_SIZE : 4 + (index + 1) * NPC_RECORD_SIZE]
        name = raw[:20].split(b"\0", 1)[0].decode("ascii", "strict")
        bias = struct.unpack_from("<H", raw, 0x27)[0]
        records.append((index, name, bias))
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
        # NPC definition +0x27 copied into generated scene-record +0x0C.
        0x2BACE: bytes.fromhex("66 8b 50 27 8b 45 f0 66 89 50 0c"),
        # Other principal scene-source builders clear the same scene field.
        0x2BC5A: bytes.fromhex("66 c7 40 0c 00 00"),
        0x2BEAF: bytes.fromhex("66 c7 40 0c 00 00"),
        0x2C0A4: bytes.fromhex("66 c7 40 0c 00 00"),
        # Final scene draw loop passes scene +0x0C as argument 4 to 36CEC.
        0x2C4AD: bytes.fromhex("8b 45 f8 0f b7 40 0c 50"),
        0x2C4F3: bytes.fromhex("8b 55 f8 0f b7 42 0c 50"),
        # 36CEC derives a lookup index from projected-size argument 3.
        0x36CFA: bytes.fromhex("8b 45 10 25 f8 ff 00 00 d1 e8 89 45 e0"),
        # Then adds argument 4 directly to that derived index.
        0x36D63: bytes.fromhex("8b 5d e0 03 5d 14"),
        # The biased index selects a 16-bit entry through runtime table 0x258C4.
        0x36D7D: bytes.fromhex("8b 3d c4 58 02 00 0f b7 04 5f"),
    }

    for offset, expected in signatures.items():
        actual = obj[offset : offset + len(expected)]
        if actual != expected:
            raise AssertionError(
                f"CODE.2 signature mismatch at 0x{offset:05X}: "
                f"{actual.hex()} != {expected.hex()}"
            )

    print("CODE.2 render-scale bias signatures: PASS")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("npcs", type=Path)
    parser.add_argument("--code2", type=Path)
    args = parser.parse_args()

    records = parse_npcs(args.npcs)
    nonzero = [(i, name, bias) for i, name, bias in records if bias != 0]

    print(f"records={len(records)} record_size=0x{NPC_RECORD_SIZE:X}")
    print(f"nonzero_render_scale_lookup_bias={len(nonzero)}")
    for index, name, bias in nonzero:
        print(f"  {index:2d} {name}: {bias}")

    expected = [(3, "PYRE ELEMENTAL", 3)]
    if nonzero != expected:
        raise AssertionError(f"unexpected nonzero +0x27 set: {nonzero!r}")

    print("NPCS +0x27 corpus check: PASS")
    if args.code2:
        verify_code2(args.code2)


if __name__ == "__main__":
    main()
