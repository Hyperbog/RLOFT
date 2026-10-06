#!/usr/bin/env python3
"""Audit proved NPCS.DAT spell/effect immunity aliases."""
from __future__ import annotations

import argparse
import struct
from pathlib import Path

RECORD_SIZE = 0x45
NAME_SIZE = 20
FLAGS_OFF = 0x31

COMMON_SPELL_EFFECT_IMMUNITY = 0x00000100
COLD_EFFECT_IMMUNITY = 0x00000800
FIRE_EFFECT_IMMUNITY = 0x00002000

EXPECTED_COUNTS = {
    COMMON_SPELL_EFFECT_IMMUNITY: 4,
    COLD_EFFECT_IMMUNITY: 12,
    FIRE_EFFECT_IMMUNITY: 6,
}


def parse(path: Path) -> list[bytes]:
    data = path.read_bytes()
    if len(data) < 4:
        raise ValueError("file shorter than count field")
    count = struct.unpack_from("<I", data, 0)[0]
    expected = 4 + count * RECORD_SIZE
    if len(data) != expected:
        raise ValueError(
            f"size {len(data)} != 4 + {count}*0x{RECORD_SIZE:02x} = {expected}"
        )
    return [
        data[4 + i * RECORD_SIZE : 4 + (i + 1) * RECORD_SIZE]
        for i in range(count)
    ]


def name(record: bytes) -> str:
    raw = record[:NAME_SIZE]
    nul = raw.find(b"\0")
    if nul < 0:
        raise ValueError("NPC name lacks NUL within fixed 20-byte field")
    return raw[:nul].decode("ascii", errors="strict")


def flags(record: bytes) -> int:
    return struct.unpack_from("<I", record, FLAGS_OFF)[0]


def members(records: list[bytes], mask: int) -> list[tuple[int, str]]:
    return [
        (index, name(record))
        for index, record in enumerate(records)
        if flags(record) & mask
    ]


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("npcs", type=Path)
    args = parser.parse_args()

    records = parse(args.npcs)
    print(f"file={args.npcs.name} records={len(records)} record_size=0x{RECORD_SIZE:02x}")

    aliases = [
        ("common_spell_effect_immunity", COMMON_SPELL_EFFECT_IMMUNITY),
        ("cold_effect_immunity", COLD_EFFECT_IMMUNITY),
        ("fire_effect_immunity", FIRE_EFFECT_IMMUNITY),
    ]

    for label, mask in aliases:
        found = members(records, mask)
        expected = EXPECTED_COUNTS[mask]
        status = "PASS" if len(found) == expected else "FAIL"
        print(f"{label} mask=0x{mask:08x} count={len(found)} expected={expected} {status}")
        for object_id, npc_name in found:
            print(f"  {object_id:02d} {npc_name}")
        if len(found) != expected:
            raise SystemExit(1)

    # The executable builds these compound masks in the spell/effect dispatcher.
    fire_mask = COMMON_SPELL_EFFECT_IMMUNITY | FIRE_EFFECT_IMMUNITY
    cold_mask = COMMON_SPELL_EFFECT_IMMUNITY | COLD_EFFECT_IMMUNITY
    print(f"fire_dispatch_mask=0x{fire_mask:04x} expected=0x2100")
    print(f"cold_dispatch_mask=0x{cold_mask:04x} expected=0x0900")
    if fire_mask != 0x2100 or cold_mask != 0x0900:
        raise SystemExit(1)

    status_masks = [0x0102, 0x0108, 0x0110]
    if any((mask & COMMON_SPELL_EFFECT_IMMUNITY) == 0 for mask in status_masks):
        raise SystemExit(1)
    print("status_mask_common_bit=PASS masks=" + ",".join(f"0x{x:04x}" for x in status_masks))


if __name__ == "__main__":
    main()
