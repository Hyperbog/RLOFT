#!/usr/bin/env python3
"""Offline original-byte audit for CODE.2 combat/hand entry points.

No GitHub Actions, network, build, emulator or redistributed DOS assets.
Usage: python DemoV1/tools/verify_combat_entry_bytes.py /path/to/CODE.2
"""
from __future__ import annotations

import hashlib
import sys
from pathlib import Path

EXPECTED_SHA256 = "853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314"
CODE_OBJECT_START = 0x3AF24

# Offsets are CODE.2 object-1 logical addresses, not 0x10000-adjusted
# objdump virtual addresses. These are independently disassembled x86.
ANCHORS = {
    "214B8 actor status": (0x214CD, "668b81f0180200"),
    "214B8 cursor swap flag08": (0x214E4, "f644242808"),
    "214B8 attack/use flag01": (0x216C6, "f644242801"),
    "215E9 hand-ready dword store": (0x215E9, "c784b77e1a020000000000"),
    "26018 action-state dword compare": (0x26037, "83bcb8eb19020000"),
    "26018 unarmed category 0A": (0x26064, "bb0a000000"),
    "26018 category 09 minimum": (0x260B2, "83fb09"),
    "26018 category 0B maximum": (0x260BB, "83fb0b"),
    "23F50 hardcoded special ID 2E": (0x23F54, "83f82e"),
    "23F50 special category 0F": (0x23FA8, "83f80f"),
    "23F50 special category 0D": (0x23FB3, "83f80d"),
    "DB94 group priority bit": (0x0DB94, "f644240401"),
    "DACC projection-masked test": (0x0DB27, "6683780800"),
    "DACC blocked actor flag02": (0x0DB35, "f6828b4d010002"),
    "DACC horizontal split at 80": (0x0DB45, "81fa80000000"),
    "DACC projection delta 7D00": (0x0DB61, "81ea007d0000"),
    "DACC vertical threshold 3B": (0x0DB6A, "83fa3b"),
    "DACC eligible actor flag10": (0x0DB76, "f6808b4d010010"),
}


def verify(original: bytes) -> int:
    actual_sha = hashlib.sha256(original).hexdigest()
    if actual_sha != EXPECTED_SHA256:
        raise ValueError(
            "CODE.2 SHA-256 does not match the audited original: "
            f"{actual_sha}"
        )
    for title, (logical, hex_bytes) in ANCHORS.items():
        expected = bytes.fromhex(hex_bytes)
        start = CODE_OBJECT_START + logical
        actual = original[start : start + len(expected)]
        if actual != expected:
            raise ValueError(
                f"{title}: original opcode mismatch at {start:#x}: "
                f"expected={expected.hex()} actual={actual.hex()}"
            )

    base = 0x218E0
    assert 0x21A7E - base == 0x19E, "215E9 hand-ready offset"
    assert 0x219EB - base == 0x10B, "26037 per-hand action-state offset"
    assert 0x21A0B - base == 0x12B, "117A0 known-bit offset"

    return len(ANCHORS)


if __name__ == "__main__":
    if len(sys.argv) != 2:
        raise SystemExit("usage: verify_combat_entry_bytes.py CODE.2")
    total = verify(Path(sys.argv[1]).read_bytes())
    print(
        f"PASS: {total} original CODE.2 combat/hand opcode anchors; "
        "party offsets 19E/10B/12B; no build executed"
    )
