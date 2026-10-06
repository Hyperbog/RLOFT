#!/usr/bin/env python3
"""Inspect 0x248-byte SAVE.DAT character records using the audited legacy layout."""
from __future__ import annotations

import argparse
import struct
from pathlib import Path

RECORD_SIZE = 0x248
PARTY_OFFSET, PARTY_COUNT = 0x77, 4
ROSTER_OFFSET, ROSTER_COUNT = 0xD68, 10
STATUS_OFFSET = 0x10
PORTRAIT_OFFSET, SEX_OFFSET, RACE_OFFSET = 0x12, 0x13, 0x14
CLASS_OFFSET, CLASS_COUNT = 0x15, 3
AGE_OFFSET, ALIGNMENT_OFFSET, BASE_AC_OFFSET = 0x18, 0x1A, 0x1B
CURRENT_HP_OFFSET, MAX_HP_OFFSET = 0x1C, 0x1E
LEVEL_OFFSET, LEVEL_COUNT = 0x20, 3
PROGRESS_OFFSET, PROGRESS_COUNT = 0x26, 3
ABILITY_PAIRS = (
    ("STR", 0x32),
    ("DEX", 0x34),
    ("CON", 0x36),
    ("INT", 0x38),
    ("WIS", 0x3A),
    ("CHA", 0x3C),
)
EXCEPTIONAL_STR_OFFSET = 0x3E
ITEM_OFFSET, ITEM_COUNT = 0x40, 21
SPELL_MATRIX_A_OFFSET = 0x06A
SPELL_MATRIX_B_OFFSET = 0x0BA
SPELL_BANKS, SPELL_BANK_SIZE = 8, 10
SPELL_MATRIX_SIZE = SPELL_BANKS * SPELL_BANK_SIZE
POST_START, POST_END = 0x06A, 0x248
AUX_DWORD_OFFSET = 0x127
ITEM_KNOWN_OFFSET = 0x12B
STATE_FLAGS_OFFSET = 0x132
FINAL_RESERVED_OFFSET = 0x1F6

SEX_NAMES = {0: "Male", 1: "Female"}
RACE_NAMES = {
    0: "Human", 1: "Half-Elf", 2: "Halfling",
    3: "Gnome", 4: "Dwarf", 5: "Elf",
}
CLASS_NAMES = {
    0: "Fighter", 1: "Paladin", 2: "Ranger",
    3: "Cleric", 4: "Mage", 5: "Thief",
}
ALIGNMENT_NAMES = {
    0: "Lawful Good", 1: "Lawful Neutral", 2: "Neutral Good",
    3: "True Neutral", 4: "Chaotic Good", 5: "Chaotic Neutral",
}


def u16(r: bytes, off: int) -> int:
    return struct.unpack_from("<H", r, off)[0]


def s16(r: bytes, off: int) -> int:
    return struct.unpack_from("<h", r, off)[0]


def u32(r: bytes, off: int) -> int:
    return struct.unpack_from("<I", r, off)[0]


def s32(r: bytes, off: int) -> int:
    return struct.unpack_from("<i", r, off)[0]


def s8(r: bytes, off: int) -> int:
    return struct.unpack_from("<b", r, off)[0]


def enum_text(value: int, names: dict[int, str]) -> str:
    return names.get(value, f"unknown({value})")


def name16(r: bytes) -> str:
    raw = r[:0x10].split(b"\0", 1)[0]
    if not raw or all(b == 0xFF for b in raw):
        return "<unused/raw>"
    return raw.decode("cp437", errors="replace")


def sparse_post(r: bytes) -> str:
    cells = [(off, r[off]) for off in range(POST_START, POST_END) if r[off] not in (0x00, 0xFF)]
    return "<none>" if not cells else ", ".join(f"+0x{off:03X}=0x{value:02X}" for off, value in cells)


def terminal_summary(r: bytes) -> str:
    tail = r[FINAL_RESERVED_OFFSET:RECORD_SIZE]
    if tail and all(value == tail[0] for value in tail):
        return f"{len(tail)} bytes, uniform 0x{tail[0]:02X}"
    return tail.hex(" ")


def spell_banks(r: bytes, matrix_offset: int) -> list[str]:
    rows: list[str] = []
    for bank in range(SPELL_BANKS):
        start = matrix_offset + bank * SPELL_BANK_SIZE
        cells = []
        for entry, value in enumerate(r[start:start + SPELL_BANK_SIZE]):
            ready = value >> 4
            selected = value & 0x0F
            selected_text = "unavailable" if selected == 0x0F else str(selected)
            cells.append(f"{entry}:ready={ready},selected={selected_text}")
        rows.append(f"level{bank + 1} " + " | ".join(cells))
    return rows


def show_spell_matrix(r: bytes, label: str, offset: int) -> None:
    end = offset + SPELL_MATRIX_SIZE - 1
    print(f"  {label:<10}: +0x{offset:03X}..+0x{end:03X}")
    for row in spell_banks(r, offset):
        print(f"    {row}")


def show(label: str, index: int, base: int, r: bytes) -> None:
    classes = [s8(r, CLASS_OFFSET + i) for i in range(CLASS_COUNT)]
    class_text = ["absent" if value == -1 else enum_text(value, CLASS_NAMES) for value in classes]
    levels = [s16(r, LEVEL_OFFSET + i * 2) for i in range(LEVEL_COUNT)]
    progress = [s32(r, PROGRESS_OFFSET + i * 4) for i in range(PROGRESS_COUNT)]
    abilities = ", ".join(f"{ability}={r[off]}/{r[off+1]}" for ability, off in ABILITY_PAIRS)

    print(f"{label}[{index:02d}] file=0x{base:04X} name={name16(r)!r}")
    print(f"  status    : 0x{u16(r, STATUS_OFFSET):04X}")
    print(f"  identity  : portrait={s8(r, PORTRAIT_OFFSET)} sex={enum_text(r[SEX_OFFSET], SEX_NAMES)} "
          f"race={enum_text(r[RACE_OFFSET], RACE_NAMES)} age={u16(r, AGE_OFFSET)} "
          f"alignment={enum_text(r[ALIGNMENT_OFFSET], ALIGNMENT_NAMES)} base_ac={s8(r, BASE_AC_OFFSET)}")
    print(f"  classes   : {classes} -> {class_text}")
    print(f"  hp        : {s16(r, CURRENT_HP_OFFSET)}/{s16(r, MAX_HP_OFFSET)}")
    print(f"  levels    : {levels}")
    print(f"  progress  : {progress}")
    print(f"  abilities : {abilities}; STR%={r[EXCEPTIONAL_STR_OFFSET]}/{r[EXCEPTIONAL_STR_OFFSET+1]}")
    print("  item[21]  : " + ", ".join(str(s16(r, ITEM_OFFSET + i * 2)) for i in range(ITEM_COUNT)))
    show_spell_matrix(r, "spells_A", SPELL_MATRIX_A_OFFSET)
    show_spell_matrix(r, "spells_B", SPELL_MATRIX_B_OFFSET)
    print(f"  aux127    : 0x{u32(r, AUX_DWORD_OFFSET):08X}")
    print(f"  known12B  : 0x{u32(r, ITEM_KNOWN_OFFSET):08X}")
    print(f"  flags132  : 0x{u32(r, STATE_FLAGS_OFFSET):08X}")
    print(f"  tail1F6   : {terminal_summary(r)}")
    print(f"  post!=fill: {sparse_post(r)}")


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("save", type=Path)
    args = ap.parse_args()
    data = args.save.read_bytes()

    for label, start, count in (("party", PARTY_OFFSET, PARTY_COUNT), ("roster", ROSTER_OFFSET, ROSTER_COUNT)):
        for i in range(count):
            base = start + i * RECORD_SIZE
            r = data[base:base + RECORD_SIZE]
            if len(r) != RECORD_SIZE:
                raise SystemExit(f"short record at 0x{base:X}")
            show(label, i, base, r)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
