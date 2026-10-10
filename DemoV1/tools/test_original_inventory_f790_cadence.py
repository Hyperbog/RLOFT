"""Original CODE.2 inventory F790 event cadence and native redraw guard.

Default mode reads the checked-in native sources (suitable for CTest).
Use --original-code2 /path/to/CODE.2 for independent shipped binary
signatures and complete direct E8-call inventories. --binary-only permits
private original-file validation without a local repository checkout.
"""
from __future__ import annotations

import argparse
import hashlib
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
CODE2_SHA256 = "853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314"
BASE = 0x3AF24
CALLS_F790 = (
    0x1134C, 0x1173B, 0x1178E, 0x1184A, 0x11882, 0x118FF,
    0x119FC, 0x11B63, 0x11F47, 0x127EC, 0x128A3, 0x12AFA,
    0x12BA4, 0x12D50, 0x12DFD, 0x130DE, 0x131BB,
)
# Each original direct E8 F790 call has one of four source argument
# families. These byte-backed groups are intentionally not yet conflated
# with native Win32 event dispatch.
GLOBAL_HELD_8600_8602 = (
    0x1134C, 0x1173B, 0x1178E, 0x11B63, 0x127EC, 0x128A3,
    0x12AFA, 0x12BA4, 0x12D50, 0x12DFD, 0x131BB,
)
CONSTANT_EMPTY_HAND = (0x1184A, 0x11882, 0x118FF)
STACK_LOCAL_HELD = (0x119FC, 0x11F47)
INDEX_SELECTED_HELD = (0x130DE,)

# Original execution paths and exact machine-code windows, not a synthetic
# 70-Hz time model. 13118->112EC->F790 draws one initial modal ribbon;
# 13184 checks an UNSIGNED deadline, zeroes pending/deadline then redraws.
WINDOWS = {
    0x13118: "6a00c705a86d000000000000c605ac6d000000e89c6b000083c404e8b4e1ffff",
    0x13184: "803dac6d0000007436e86ed501003b05a86d000076290fb60502860000500fbf050086000050c705a86d000000000000c605ac6d000000e8d0c5ffff83c408",
    0x21492: "f6442424017418668b80f018020030c080e4800fb7c085c07505e8671cffff",
    0x11717: "c605ac6d000001e8ddef010083c036a3a86d00000fb60502860000500fbf050086000050e850e0ffff",
    0x1133C: "0fb60502860000500fbf050086000050e83fe4ffff",
    0x130CB: "0fb60550c0000050e8d860010083c4046a0053e8adc6ffff83c408",
    # F790 pending draws without reseed; ordinary held brackets F66C
    # with 30CC4(held ID) and 30CC4(30700 current tick).
    0xF7B4: "803dac6d000000740f689c750000e8a5feffffe9ad0500006a3f8d44240450e890cb000083c40883fbff0f8498050000",
    0xFD58: "53e8660f020083c404689c750000e801f9ffff83c404e88d09020050e84b0f020083c40483c4105d5f5e5bc3",
    0x30CC4: "e8d3ffffff85c074068b5424048910c3",
}

def direct_rel32_calls(binary: bytes, target: int) -> tuple[int, ...]:
    # Expected callsites below were independently decoded at boundaries.
    return tuple(
        i for i in range(len(binary) - 4)
        if binary[i] == 0xE8 and
        i + 5 + int.from_bytes(binary[i + 1:i + 5], "little", signed=True) == target
    )

def check_binary(path: Path) -> None:
    file = path.read_bytes()
    assert len(file) == 558689, "wrong original CODE.2 length"
    assert hashlib.sha256(file).hexdigest() == CODE2_SHA256, "wrong original CODE.2 hash"
    binary = file[BASE:BASE + 0x3A852]
    for logical, encoded in WINDOWS.items():
        expected = bytes.fromhex(encoded)
        actual = binary[logical:logical + len(expected)]
        assert actual == expected, f"original CODE.2 machine window 0x{logical:05X}"
    assert direct_rel32_calls(binary, 0xF790) == CALLS_F790, "F790 direct calls changed"
    groups = (
        GLOBAL_HELD_8600_8602, CONSTANT_EMPTY_HAND,
        STACK_LOCAL_HELD, INDEX_SELECTED_HELD)
    assert tuple(sorted(at for group in groups for at in group)) == CALLS_F790, (
        "original F790 argument source classification incomplete")
    for at in GLOBAL_HELD_8600_8602:
        window = binary[at - 55:at]
        assert b"\x0f\xb6\x05\x02\x86\x00\x00" in window, (
            f"global held known byte 8602 missing at {at:05X}")
        assert b"\x0f\xbf\x05\x00\x86\x00\x00" in window, (
            f"global held item word 8600 missing at {at:05X}")
    for at in CONSTANT_EMPTY_HAND:
        assert binary[at - 4:at] == bytes.fromhex("6a006aff"), (
            f"empty-hand F790 immediate argument pair missing at {at:05X}")
    assert binary[0x119FC - 5:0x119FC] == bytes.fromhex("ff342455")
    assert binary[0x11F47 - 8:0x11F47] == bytes.fromhex("ff742408ff742414")
    assert binary[0x130DE - 3:0x130DE] == bytes.fromhex("6a0053")
    assert direct_rel32_calls(binary, 0x112EC) == (0x13133,), "112EC direct call changed"
    assert direct_rel32_calls(binary, 0x13118) == (0x214AC,), "13118 entry changed"
    print("PASS: immutable CODE.2 9 cadence/RNG windows + 17 F790 event callsites (11 global, 3 empty, 2 local, 1 selected) + entry links")

def check_source() -> None:
    host = (ROOT / "DemoV1/src/win32_main.cpp").read_text(encoding="utf-8")
    inventory = (ROOT / "DemoV1/src/inventory_runtime.cpp").read_text(encoding="utf-8")
    modal = (ROOT / "DemoV1/src/party_modal_presenter.cpp").read_text(encoding="utf-8")
    cmake = (ROOT / "DemoV1/CMakeLists.txt").read_text(encoding="utf-8")
    epoch = (ROOT / "DemoV1/include/rloft/original_feedback_ribbon_epoch.hpp").read_text(encoding="utf-8")
    random = (ROOT / "DemoV1/include/rloft/original_random.hpp").read_text(encoding="utf-8")
    assert "ui.open_party_panels(*actor);" in host
    assert "runtime.inventory().reset_scribe_feedback_13118();" in host
    assert "runtime.inventory().expire_scribe_feedback_13184(" in host
    assert "code1_tick_count(exploration_epoch));" in host
    assert "observed_irq_tick_30700<=" in inventory
    assert "*mage_scribe_feedback->deadline_6da8" in inventory
    assert "mage_scribe_feedback.reset();" in inventory
    assert "OriginalRandom" not in modal and "next15()" not in modal, (
        "renderer may not create separate RNG or consume RNG on VGA refresh")
    assert "add_test(NAME original_feedback_ribbon_epoch_tests COMMAND original_feedback_ribbon_epoch_tests)" in cmake
    assert "src/original_random.cpp" in cmake
    assert "if (!pending_6dac && held_id == -1)" in epoch
    assert "shared_game_rng.seed(static_cast<std::uint32_t>(held_id));" in epoch
    assert "shared_game_rng.next15()" in epoch
    assert "std::forward<ObserveGameTick30700>(observe_tick_30700)()" in epoch
    assert "void seed(std::uint32_t value) noexcept" in random
    print("PASS: native modal entry and unsigned feedback expiration, no presentation-local RNG")

def main() -> None:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--original-code2", type=Path)
    ap.add_argument("--binary-only", action="store_true")
    args = ap.parse_args()
    if not args.binary_only:
        check_source()
    if args.original_code2:
        check_binary(args.original_code2)
    elif args.binary_only:
        ap.error("--binary-only requires --original-code2")

if __name__ == "__main__":
    main()
