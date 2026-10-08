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
    # CODE.2 24478 direct two-hand checks plus its original helpers.
    "23EAC signed class byte0": (0x23EB4, "0fbe90f5180200"),
    "23EAC signed class byte1": (0x23EC1, "0fbe90f6180200"),
    "23EAC signed class byte2": (0x23ECE, "0fbe80f7180200"),
    "23EE4 paired exception 2B": (0x23EE8, "83f82b"),
    "23EE4 paired exception 103": (0x23F23, "3d03010000"),
    "23E58 pseudo-item lower bound": (0x23E58, "837c24047e"),
    "23E58 original OBJECTS category": (0x23E80, "0fb644022f"),
    "23E58 weapon category9": (0x23E85, "83f809"),
    "23E58 object30 exception": (0x23E9A, "837c240430"),
    "24478 first hand item": (0x24489, "0fbfa83c190200"),
    "24478 second hand item": (0x24490, "0fbf803e190200"),
    "24478 actor status mask": (0x245A9, "2540720000"),
    "24478 hand0 action state": (0x24A5E, "83b8eb19020000"),
    "24478 hand1 action state": (0x24A6F, "83beef19020000"),
    "24478 selected hand flag": (0x24A7A, "837c243800"),
    # Original 240A8 single-hand candidate: class/level/party-mode.
    "240A8 candidate prologue": (0x240A8, "53565755"),
    "240BE call attack predicate": (0x240BE, "e895fdffff"),
    "240C6 store candidate classification": (0x240C6, "894500"),
    "240CE call special item predicate": (0x240CE, "e87dfeffff"),
    "240E3 compares empty item": (0x240E3, "83feff"),
    "240EC checks class4": (0x240EC, "6a04"),
    "2411D checks class1": (0x2411D, "6a01"),
    "24179 calls 34060 list": (0x24179, "56"),
    "24185 calls 33FC4 list": (0x24185, "e83afeffff"),
    "24201 reads object category": (0x24201, "0fb644282f"),
    "24214 checks party mode FF": (0x24214, "3dff000000"),
    "2425D reads item qualifier byte35": (0x2425D, "668b440335"),
    "24264 qualifier low bits": (0x24264, "2403"),
    "24266 qualifier equals two": (0x24266, "663d0200"),
    "2428A spell lower bound A7": (0x2428A, "81fea7000000"),
    "24296 spell upper bound C9": (0x24296, "81fec9000000"),
    "242DD signed class level word": (0x242DD, "0fbf8600190200"),
    "2433E spell second range CA": (0x2433E, "81feca000000"),
    "2434E special spell 12D": (0x2434E, "81fe2d010000"),
    "3407F 34060 list item31": (0x2407F, "83fb31"),
    "34005 33FC4 list item29": (0x24005, "83fb29"),
    # Original 24478 final ordinary pair arbitration and empty sentinel.
    "24478 same item qualifiers": (0x24742, "6639c20f8511"),
    "24478 matched qualifier two": (0x2474B, "663d0200"),
    "24478 paired exception first": (0x247A2, "e83df7ffff"),
    "24478 paired exception second": (0x247B2, "e82df7ffff"),
    "24478 weight comparison": (0x247E2, "3a4830"),
    "24478 unequal qualifier branch": (0x2485C, "0f86d9000000"),
    "24478 first qualifier equals two": (0x24862, "6683fa02"),
    "24478 classified first only": (0x24A0A, "837c240400"),
    "24478 classified second only": (0x24A30, "833c2400"),
    "240A8 empty hand before class": (0x240E3, "83feff"),
    "240A8 empty hand party veto": (0x24248, "83feff"),
    # Loader 268C8 + 4104C allocator; -1 category is NOT serialized.
    "OBJECTS loader resource 0704": (0x168C9, "6804070000"),
    "OBJECTS count times record 41": (0x168F7, "89d0c1e00601d050"),
    "OBJECTS allocated pointer store": (0x1690C, "a3d8550200"),
    "OBJECTS read count times 41": (0x1691E, "e8c9a90100"),
    "heap wrapper adds ten bytes": (0x31053, "30db83c70a"),
    "heap wrapper returns base plus ten": (0x310A2, "8d460a"),
    "heap wrapper writes magic dword": (0x310A5, "c7460678563412"),
    # 244CE..246E9 empty and temporary spell-hand early gates.
    "244CE first empty hand": (0x244CE, "83fdff"),
    "244D3 both empty hands": (0x244D3, "3b6c241c"),
    "244EC first actor status fetch": (0x244EC, "0fb705f0180200"),
    "244F3 status threshold 4000h": (0x244F3, "3d00400000"),
    "24501 second status threshold 4000h": (0x24501, "3d00400000"),
    "24522 first pseudo lower bound": (0x24522, "83fd7e"),
    "24540 second pseudo helper": (0x24540, "e8eff9ffff"),
    "2454C paired pseudo forces first true": (0x2454C, "bb01000000"),
    "245A9 actor condition mask 7240h": (0x245A9, "2540720000"),
    "245BE empty-first party mode": (0x245BE, "0fb605a8580200"),
    "24641 first pseudo in general path": (0x24641, "83fd7e"),
    "2465B first pseudo party-mode override": (0x2465B, "0fb605a8580200"),
    "24693 second pseudo in general path": (0x24693, "837c241c7e"),
    "246E9 classified first branching": (0x246E9, "837c240400"),
    "23F34 pseudo helper lower bound": (0x23F34, "837c24047e"),
    # E5F0 original player attack: exact target and maintenance-clock
    # pacing. Addresses are logical CODE.2 object-1 offsets.
    "E5F0 explicit target sentinel": (0xE5FB, "837c2434ff"),
    "E5F0 fallback DB94": (0xE606, "e889f5ffff"),
    "E5F0 party mode byte": (0xE621, "0fb605a8580200"),
    "E5F0 rear actor index bit": (0xE639, "f644242802"),
    "E5F0 unarmed item test": (0xE640, "83fdff"),
    "E5F0 rear qualifier original word": (0xE652, "668b440235"),
    "E5F0 rear qualifier equals two": (0xE65B, "663d0200"),
    "E5F0 target suppressed": (0xE661, "c7442410ffffffff"),
    "E5F0 C80C original hit call": (0xE6AB, "e85ce1ffff"),
    "E5F0 unarmed recovery": (0xE9D0, "83fdff"),
    "E5F0 temporary hand lower bound": (0xE9D5, "83fd7e"),
    "E5F0 recovery full word +35": (0xE9F5, "668b440235"),
    "E5F0 recovery SHL 10": (0xE9FA, "c1e00a"),
    "E5F0 recovery SHR AX 12": (0xE9FD, "66c1e80c"),
    "E5F0 actor action modifier mask": (0xEA15, "f780121a020010000001"),
    "E5F0 hit-gated deadline": (0xEA2E, "837c240800"),
    "E5F0 calls 29CC4 maintenance clock": (0xEA35, "e88ab20000"),
    "E5F0 stores hand ready at +19E": (0xEA55, "899c827e1a0200"),
    "E5F0 stores damage at +1AA": (0xEA5C, "89b4828a1a0200"),
    "E5F0 invokes original action animation": (0xEA6E, "e8d5600100"),
    "29CC4 reads pauseable gameplay A7E4": (0x19CC4, "a1e4a70000"),
    # CODE.2 logical 24B48 (objdump virtual 34B48) is the
    # action presentation/scheduling stage CALLED BY E5F0.
    "24B48 action presenter entry": (0x24B48, "5383ec08"),
    "24B48 receives selected hand": (0x24B50, "8b542414"),
    "24B48 presents action graphic": (0x24BA6, "e8e59effff"),
    "24B48 gates scheduling on nonzero interval": (0x24BC9, "837c241c00"),
    "24B48 reads maintenance tick after present": (0x24BD0, "e8ef50ffff"),
    "24B48 gets full original interval": (0x24BD5, "8b54241c"),
    "24B48 adds full interval": (0x24BD9, "01c2"),
    "24B48 computes actor stride": (0x24BDB, "69c348020000"),
    "24B48 writes hand action-state +10B": (0x24BE5, "899498eb190200"),
    # E5F0 calls CODE.2 logical E578 (objdump virtual 1E578),
    # an actor effect/redraw service BEFORE NPC target hit checks.
    "E578 prehit status service entry": (0xE578, "31d231c0"),
    "E578 tests all actor bit20": (0xE57C, "f680121a020020"),
    "E578 marks affected panel redraw": (0xE585, "c6825acf000001"),
    "E578 clears actor bit20": (0xE58C, "80a0121a0200df"),
    "E578 addresses attacking actor": (0xE59E, "6944240448020000"),
    "E578 tests selected actor bit02": (0xE5A6, "f680131a020002"),
    "E578 clears selected dword +15A": (0xE5AF, "c7803a1a020000000000"),
    "E578 clears selected actor bit02": (0xE5B9, "80a0131a0200fd"),
    "E578 marks selected panel redraw": (0xE5C4, "c6805acf000001"),
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
