#!/usr/bin/env python3
"""Validate NPCS.DAT +0x27 projected-scale lookup bias against CODE.2."""
from __future__ import annotations

import argparse
import hashlib
import struct
from pathlib import Path

NPC_RECORD_SIZE = 0x45
CODE2_SHA256 = "853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314"
OBJ1_FILE_OFF = 0x3AF24
OBJ1_SIZE = 0x3A852


def parse_counted(path: Path) -> list[bytes]:
    data = path.read_bytes()
    if len(data) < 4:
        raise ValueError(f"{path}: shorter than count")
    count = struct.unpack_from("<I", data, 0)[0]
    expected = 4 + count * NPC_RECORD_SIZE
    if len(data) != expected:
        raise ValueError(
            f"{path}: {len(data)} != 4 + {count}*0x45 = {expected}"
        )
    return [
        data[4 + i * NPC_RECORD_SIZE : 4 + (i + 1) * NPC_RECORD_SIZE]
        for i in range(count)
    ]


def cstr(raw: bytes) -> str:
    return raw.split(b"\0", 1)[0].decode("ascii", "strict")


def verify_npcs(path: Path) -> None:
    records = parse_counted(path)
    nonzero = []
    for i, raw in enumerate(records):
        value = struct.unpack_from("<H", raw, 0x27)[0]
        if value:
            nonzero.append((i, cstr(raw[:20]), value))
    print(f"NPCS records={len(records)}")
    print(f"projected_scale_lookup_bias nonzero={len(nonzero)}")
    for i, name, value in nonzero:
        print(f"  {i}:{name} -> {value}")
    expected = [(3, "PYRE ELEMENTAL", 3)]
    if nonzero != expected:
        raise AssertionError(f"unexpected +0x27 nonzero set: {nonzero!r}")
    print("NPCS +0x27 corpus audit: PASS")


def require(obj: bytes, site: int, hex_bytes: str, label: str) -> None:
    expected = bytes.fromhex(hex_bytes)
    actual = obj[site : site + len(expected)]
    if actual != expected:
        raise AssertionError(
            f"{label} mismatch at 0x{site:x}: {actual.hex()} != {expected.hex()}"
        )


def verify_code2(path: Path) -> None:
    full = path.read_bytes()
    sha = hashlib.sha256(full).hexdigest()
    if sha != CODE2_SHA256:
        raise AssertionError(f"CODE.2 sha256 {sha} != certified {CODE2_SHA256}")
    obj = full[OBJ1_FILE_OFF : OBJ1_FILE_OFF + OBJ1_SIZE]
    if len(obj) != OBJ1_SIZE:
        raise AssertionError("CODE.2 object-1 extraction truncated")

    # Family-A scene emission copies NPC definition +0x27 to scene record +0x0C.
    require(obj, 0x2BACE, "66 8b 50 27", "NPC +0x27 load")
    require(obj, 0x2BAD2, "8b 45 f0 66 89 50 0c", "scene +0x0C store")

    # The scene renderer forwards scene +0x0C as argument 4 to 36CEC.
    require(obj, 0x2C4B0, "0f b7 40 0c 50", "scene +0x0C renderer argument")
    require(obj, 0x2C4E1, "e8 06 a8 00 00", "36CEC renderer call")

    # 36CEC derives a base lookup index from projected size, adds arg4, then
    # indexes the renderer table or emits zero if beyond the loaded limit.
    require(obj, 0x36CFA, "8b 45 10 25 f8 ff 00 00 d1 e8", "projected-size base index")
    require(obj, 0x36D63, "8b 5d e0 03 5d 14", "scale lookup bias add")
    require(obj, 0x36D69, "3b 1d 30 c7 00 00", "lookup limit compare")
    require(obj, 0x36D7D, "8b 3d c4 58 02 00 0f b7 04 5f", "renderer lookup table read")
    require(obj, 0x36D8F, "3d 08 08 00 00", "renderer value cap compare")

    print("NPC definition -> scene record +0x0C: PASS")
    print("scene +0x0C -> 36CEC argument: PASS")
    print("projected-scale lookup-index bias behavior: PASS")
    print("CODE.2 render-scale bias verification: PASS")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("npcs", type=Path)
    parser.add_argument("--code2", type=Path)
    args = parser.parse_args()
    verify_npcs(args.npcs)
    if args.code2:
        verify_code2(args.code2)


if __name__ == "__main__":
    main()
