#!/usr/bin/env python3
"""Verify SAVE spell/effect catalogues against raw T and CODE.2."""
from __future__ import annotations

import argparse
import hashlib
import struct
from pathlib import Path

T_SHA256 = "a3f1e19e40f226c8cf150a43aacd9c378ee35a1294d2c9dd72870bfdf1e16315"
CODE2_SHA256 = "853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314"
XOR_KEY = 0xDC

PRIEST_COUNT_FILE_OFFSET = 0x7C344
MAGE_COUNT_FILE_OFFSET = 0x7C364
PRIEST_COUNTS = (7, 6, 5, 4, 4, 2, 0, 0)
MAGE_COUNTS = (7, 6, 10, 4, 2, 6, 0, 0)

EFFECT_LABELS = (
    "Armor", "Light", "Shield", "Blur", "Haste", "Invisibility",
    "Stoneskin", "Dragon Scales", "Bless", "Invis. to Undead", "Light",
    "Aid", "Draw Strength", "Draw Dexterity", "Neg. Plane Prot.", "Fortify",
    "Slow Poison", "True Seeing", "Detect Undead", "Fire Resistance",
    "Detect Magic", "Prayer", "Lightning Prot.", "Giant Strength", "Speed",
    "Detect Enemy",
)

MAGE_LEVELS = (
    ("ARMOR", "BURNING HANDS", "CHILL TOUCH", "DETECT UNDEAD", "LIGHT", "MAGIC MISSILE", "SHIELD"),
    ("AGANNAZER'S SCORCHER", "BLUR", "IMPROVED IDENTIFY", "KNOCK", "WIZARD LOCK", "ICE KNIFE"),
    ("DISPEL MAGIC", "FIREBALL", "FLAME ARROW", "HASTE", "SLOW", "HOLD PERSON", "HOLD UNDEAD", "INVISIBILITY 10' RAD", "LIGHTNING BOLT", "VAMPIRIC TOUCH"),
    ("DETECT SCRYING", "ICE STORM", "STONESKIN", "WIZARD EYE"),
    ("CONE OF COLD", "HOLD MONSTER"),
    ("CLAWS OF UMBER HULK", "DEATH SPELL", "LICH TOUCH", "OTILUKE'S FREEZING SPHERE", "TRUE SEEING", "DRAGON SCALES"),
    (),
    (),
)

PRIEST_LEVELS = (
    ("BLESS", "CURE LIGHT WOUNDS", "CAUSE LIGHT WOUNDS", "DETECT MAGIC", "INVISIBILITY TO UNDEAD", "LIGHT", "MAGICAL STONE"),
    ("AID", "FLAME BLADE", "HOLD PERSON", "SLOW POISON", "DRAW STRENGTH", "DRAW DEXTERITY"),
    ("CURE DISEASE", "DISPEL MAGIC", "NEGATIVE PLANE PROT.", "PRAYER", "REMOVE PARALYSIS"),
    ("CURE SERIOUS WOUNDS", "CAUSE SERIOUS WOUNDS", "FORTIFY", "NEUTRALIZE POISON"),
    ("CURE CRITICAL WOUNDS", "CAUSE CRITICAL WOUNDS", "FLAME STRIKE", "RAISE DEAD "),
    ("HEAL", "HARM"),
    (),
    (),
)


def parse_t(data: bytes) -> list[bytes]:
    count = struct.unpack_from("<I", data, 0)[0]
    offsets = struct.unpack_from(f"<{count}I", data, 4)
    records: list[bytes] = []
    for i, off in enumerate(offsets):
        if off + 2 > len(data):
            raise AssertionError(f"T record {i}: length outside file")
        length = struct.unpack_from("<H", data, off)[0]
        end = off + 2 + length
        limit = offsets[i + 1] if i + 1 < count else len(data)
        if end > limit:
            raise AssertionError(f"T record {i}: overlaps next record/EOF")
        records.append(bytes(b ^ XOR_KEY for b in data[off + 2:end]))
    return records


def text(records: list[bytes], index: int) -> str:
    return records[index].decode("cp437", errors="strict")


def verify_t(path: Path) -> None:
    data = path.read_bytes()
    sha = hashlib.sha256(data).hexdigest()
    if sha != T_SHA256:
        raise AssertionError(f"T SHA-256 {sha} != {T_SHA256}")
    records = parse_t(data)
    if len(records) != 1073:
        raise AssertionError(f"T record count {len(records)} != 1073")

    actual_effects = tuple(text(records, 588 + i) for i in range(26))
    if actual_effects != EFFECT_LABELS:
        raise AssertionError("T[588..613] primary-effect labels differ from certified catalogue")

    for bank, expected in enumerate(MAGE_LEVELS):
        actual = tuple(text(records, 0x42 + bank * 10 + i) for i in range(len(expected)))
        if actual != expected:
            raise AssertionError(f"Mage level {bank + 1}: {actual!r} != {expected!r}")

    for bank, expected in enumerate(PRIEST_LEVELS):
        actual = tuple(text(records, 0x92 + bank * 10 + i) for i in range(len(expected)))
        if actual != expected:
            raise AssertionError(f"priest level {bank + 1}: {actual!r} != {expected!r}")

    if text(records, 153) != "LAY HANDS" or text(records, 154) != "TURN UNDEAD":
        raise AssertionError("priest level-1 resource-only action labels changed")

    print(f"T SHA-256: {sha}")
    print("T[588..613] primary-effect labels: PASS")
    print(f"Mage prepared entries: {sum(map(len, MAGE_LEVELS))}")
    print(f"priest prepared entries: {sum(map(len, PRIEST_LEVELS))}")
    print("priest resource-only actions T[153]/T[154]: LAY HANDS / TURN UNDEAD")


def read_dwords(data: bytes, off: int, count: int) -> tuple[int, ...]:
    return struct.unpack_from(f"<{count}I", data, off)


def verify_code2(path: Path) -> None:
    data = path.read_bytes()
    sha = hashlib.sha256(data).hexdigest()
    if sha != CODE2_SHA256:
        raise AssertionError(f"CODE.2 SHA-256 {sha} != {CODE2_SHA256}")

    priest = read_dwords(data, PRIEST_COUNT_FILE_OFFSET, 8)
    mage = read_dwords(data, MAGE_COUNT_FILE_OFFSET, 8)
    if priest != PRIEST_COUNTS:
        raise AssertionError(f"priest count table {priest} != {PRIEST_COUNTS}")
    if mage != MAGE_COUNTS:
        raise AssertionError(f"Mage count table {mage} != {MAGE_COUNTS}")

    print(f"CODE.2 SHA-256: {sha}")
    print(f"priest entry counts: {priest}")
    print(f"Mage entry counts:   {mage}")
    print("CODE.2 spell entry-count tables: PASS")


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("t_file", type=Path)
    ap.add_argument("code2", type=Path)
    args = ap.parse_args()
    verify_t(args.t_file)
    verify_code2(args.code2)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
