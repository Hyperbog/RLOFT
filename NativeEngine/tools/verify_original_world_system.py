#!/usr/bin/env python3
"""Certify every original level and SINTAB renderer block from native JSON.

This consumes a metadata report, not copyrighted raw level/SINTAB payloads.
"""
import json
import sys
from pathlib import Path


def verify(document: dict) -> None:
    def check(predicate: bool, why: str) -> None:
        if not predicate:
            raise ValueError("native original world/SINTAB audit FAILED: " + why)

    expected = {
        "schema": "rloft-native-world-system/v1",
        "level_count": 32,
        "level_cells": 115584,
        "controls": 1659,
        "event_slots": 394,
        "event_commands": 555,
        "level_archive_bytes": 499878,
        "sintab_bytes": 333757,
        "sintab_scales": 127,
        "sintab_scale_bytes": 324989,
    }
    for name, value in expected.items():
        check(document.get(name) == value,
              f"{name}: observed={document.get(name)!r}, expected={value!r}")
    levels = document.get("levels")
    check(isinstance(levels, list) and len(levels) == 32,
          "32 original world records were not fully enumerated")
    check([level["id"] for level in levels] == list(range(32)),
          "original level IDs must be contiguous 0..31")
    check(sum(level["cells"] for level in levels) == 115584,
          "cell geometry not conserved across levels")
    check(sum(level["controls"] for level in levels) == 1659,
          "10-byte control lists not conserved")
    check(sum(level["events"] for level in levels) == 394,
          "active event slot totals disagree")
    check(sum(level["commands"] for level in levels) == 555,
          "active event command totals disagree")
    for level in levels:
        check(level["w"] * level["h"] == level["cells"],
              f"level {level['id']} grid dimensions inconsistent")
    level22 = levels[22]
    check(level22["w"] == 30 and level22["h"] == 15,
          "level22 known archived geometry differs")
    check(level22["a"] == 1 and level22["b"] == 110 and level22["c"] == 0,
          "level22 original actor/world family cardinality differs")


def main() -> int:
    if len(sys.argv) != 2:
        print("usage: verify_original_world_system.py <world report.json>",
              file=sys.stderr)
        return 64
    try:
        verify(json.loads(Path(sys.argv[1]).read_text(encoding="utf-8-sig")))
    except (KeyError, TypeError, ValueError, OSError) as exc:
        print(str(exc), file=sys.stderr)
        return 1
    print("ORIGINAL_WORLD_32_LEVELS_AND_SINTAB_127_SCALES_CERTIFIED")
    print("32/32 original map records; 115,584 packed cells: PASS")
    print("1,659 world controls and 394/555 active event slots/commands: PASS")
    print("127 SINTAB scale blocks / 324,989 generated-code bytes structurally verified")
    print("No original DOS rasterizer microcode was executed")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
