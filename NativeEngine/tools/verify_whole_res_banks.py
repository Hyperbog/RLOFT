#!/usr/bin/env python3
"""Fail-closed certification of independent native parser on all shipped RES banks.

Input: metadata-only JSON emitted by rloft_native_bank_audit.exe; no
copyrighted data, source media bytes, or private paths embedded here.
"""
from __future__ import annotations

import json
import sys
from pathlib import Path

EXPECTED = {
    "RES0": (141, {1: 126, 2: 15}),
    "RES1": (428, {1: 405}),
    "RES2": (38, {1: 38}),
    "RES3": (4234, {7: 4234}),
    "RES4": (667, {6: 667}),
    "RES5": (98, {3: 98}),
    "RES6": (266, {4: 266}),
    "RES7": (62, {1: 62}),
    "RES8": (73, {1: 73}),
    "RES9": (555, {1: 517}),
    "RES10": (219, {4: 219}),
    "RES11": (5114, {3: 74, 4: 260, 6: 652, 7: 4128}),
    "RES12": (667, {1: 370, 4: 260}),
}
EXPECTED_TYPES = {1: 1591, 2: 15, 3: 172, 4: 1005, 5: 0, 6: 1319, 7: 8362}


def certify(data: dict) -> None:
    def expect(condition: bool, why: str) -> None:
        if not condition:
            raise ValueError("whole original-bank certification FAIL: " + why)

    expect(data["schema"] == "rloft-native-bank-audit/v1",
           "unexpected output schema")
    expect(data["legacy_included"] is True, "legacy RES11/RES12 skipped")
    expect(data["records"] == 12562, "wrong corpus cardinality")
    expect(data["validated"] == 12562, "not all records validated")
    expect(data["recognized_eh"] == 12464,
           "expected all 12,464 shipped typed EH records")
    expect(data["failed"] == 0 and data["absent_banks"] == 0,
           "invalid or absent original resources")
    expect(not data["issues"], "original RES records produced issues")
    banks = {b["name"]: b for b in data["banks"]}
    expect(set(banks) == set(EXPECTED), "unexpected/missing archive names")

    totals = {t: 0 for t in EXPECTED_TYPES}
    for name, (count, types) in EXPECTED.items():
        bank = banks[name]
        expect(bank["records"] == count, f"{name} wrong record count")
        expect(bank["validated"] == count and bank["failed"] == 0,
               f"{name} has invalid/skipped entries")
        expect(bank["bytes"] > 0, f"{name} empty original source file")
        for type_id in EXPECTED_TYPES:
            actual = int(bank["types"][str(type_id)])
            expect(actual == types.get(type_id, 0),
                   f"{name} has wrong EH{type_id} family distribution")
            totals[type_id] += actual

    expect(totals == EXPECTED_TYPES, "wrong cross-archive EH type totals")
    expect(sum(b["empty"] for b in banks.values()) == 21,
           "RES1 zero-length slots were not preserved")
    expect(sum(b["non_eh"] for b in banks.values()) == 77,
           "serialized raw/scripting records were not preserved")
    expect(banks["RES9"]["raw_wordcode"] == 38,
           "active interaction/wordcode bank not checked")
    expect(banks["RES12"]["raw_wordcode"] == 37,
           "legacy wordcode bank not checked")
    expect(sum(b["retained_shipped_anomalies"] for b in banks.values()) == 2,
           "known VOC length anomalies must be preserved and recognized")
    expect(banks["RES11"]["retained_shipped_anomalies"] == 1,
           "RES11 original VOC anomaly missing")
    expect(banks["RES12"]["retained_shipped_anomalies"] == 1,
           "RES12 original VOC anomaly missing")
    expect(sum(b["inert_legacy_eh1"] for b in banks.values()) == 5,
           "the five zero-pixel inactive RES12 image descriptors were not retained")
    expect(banks["RES12"]["inert_legacy_eh1"] == 5,
           "zero-sized historical RES12 EH1 records wrongly rendered/repaired")
    expect(sum(b["image_eh1"] + b["inert_legacy_eh1"]
               for b in banks.values()) == 1591,
           "original raster descriptors were lost")
        expect(sum(b["font_eh2"] for b in banks.values()) == 15,
           "original fonts not decoded")
    expect(sum(b["texture_eh6"] for b in banks.values()) == 1319,
           "native texture validation incomplete")
    expect(sum(b["shape_eh7"] for b in banks.values()) == 8362,
           "native compiled-shape display-list validation incomplete")
    expect(sum(b["xmidi_eh3"] for b in banks.values()) == 172,
           "native music envelope validation incomplete")
    expect(sum(b["voc_eh4"] for b in banks.values()) == 1005,
           "native audio container validation incomplete")


def main() -> int:
    if len(sys.argv) != 2:
        print("usage: verify_whole_res_banks.py <native audit JSON>", file=sys.stderr)
        return 64
    try:
        data = json.loads(Path(sys.argv[1]).read_text(encoding="utf-8-sig"))
        certify(data)
    except (OSError, ValueError, KeyError, TypeError) as err:
        print(str(err), file=sys.stderr)
        return 1
    print("ORIGINAL_ALL_13_RES_BANKS_CERTIFIED")
    print("12,562/12,562 original records: PASS")
    print("12,464 typed EH records (types 1/2/3/4/6/7): PASS")
    print("8,362 original compiled-shape display lists: PASS")
    print("1,319 decoded original textures: PASS")
    print("15 fonts + 172 XMIDI + 1,005 VOC: PASS")
    print("38 active and 37 legacy raw scripts: PASS")
    print("21 empty slots + 5 inert legacy EH1 + 2 preserved VOC anomalies: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
