"""Verify original CODE.2 12E70 -> 11AD8 -> 11B63 actor-pane F790 event.

Source-only mode checks the existing native hit-test/catalog without claiming
live mouse flags, sprite equivalence or DOS acceptance. Optional private
CODE.2 mode checks instruction windows and all relevant near-call targets.
No original game bytes are stored in this repository.
"""
from __future__ import annotations

import argparse
import hashlib
import struct
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
CODE2_SHA256 = "853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314"
CODE2_LENGTH = 558689
OBJECT1_DISK_BASE = 0x3AF24
OBJECT1_LENGTH = 0x3A852

# Small source-owned instruction signatures, never a copied disassembly.
WINDOWS = {
    0x22D49: "8b5c241066899a40a60000", # queued signed WORD flags
    0x22DC1: "0fbf8840a60000",       # dequeue sign-extended WORD flags
    0x22E1D: "515356e84b00ffff", # flags passed to CODE.2 modal
    0x12F76: "83fe457c2083fe607f1b83fb4d7f11",
    0x12F8A: "ff74241857e844ebffff",  # flags, actor; call 11AD8
    0x11AE9: "f6442418027440",       # test original flags bit 2
    0x11B02: "6683f701e8a5760100", # low WORD xor once in broadcast branch
    0x11B0E: "6bc35089be906f00005383c604a36c7f0000e843f7ff",
    0x11B30: "80b0906f000001",       # single actor byte xor
    0x11B47: "ff742414e818f7ffff", # repaint selected actor
    0x11B53: "0fb60502860000500fbf050086000050e828dcffff",
    0x11B6B: "f6442418027447",    # mode-dependent follow-on AFTER F790
    0x11BA2: "ff3550c00000e8038a0000803550c0000001",  # extra global path
}
CALLS = {
    0x22E20: 0x12E70,
    0x12F8F: 0x11AD8,
    0x11B06: 0x291B0,
    0x11B20: 0x11268,
    0x11B3F: 0x291B0,
    0x11B4B: 0x11268,
    0x11B63: 0xF790,
    0x11BA8: 0x1A5B0,
}


def verify_original(path: Path) -> None:
    original = path.read_bytes()
    if len(original) != CODE2_LENGTH or hashlib.sha256(original).hexdigest() != CODE2_SHA256:
        raise AssertionError("original CODE.2 length/SHA-256 mismatch")
    code = original[OBJECT1_DISK_BASE:OBJECT1_DISK_BASE + OBJECT1_LENGTH]
    if len(code) != OBJECT1_LENGTH:
        raise AssertionError("truncated original CODE.2 executable object")
    for offset, hex_pattern in WINDOWS.items():
        pattern = bytes.fromhex(hex_pattern)
        if code[offset:offset+len(pattern)] != pattern:
            raise AssertionError(f"CODE.2 original window mismatch 0x{offset:05X}")
    for offset, target in CALLS.items():
        if code[offset] != 0xE8:
            raise AssertionError(f"expected near CALL at 0x{offset:05X}")
        relative = struct.unpack_from("<i", code, offset+1)[0]
        if offset+5+relative != target:
            raise AssertionError(f"incorrect source target at 0x{offset:05X}")
    # The broadcast loop jumps BACK to 11B0E, skipping 11B02's XOR.
    # The XOR must apply once, not once per actor or once per repaint.
    if code[0x11B2E:0x11B30] != bytes.fromhex("ebde"):
        raise AssertionError("broadcast loop must skip repeated toggles")
    if code[0x11B2C:0x11B30] != bytes.fromhex("7d25ebde"):
        raise AssertionError("broadcast loop exit bound not original")
    if code[0x11AEE:0x11AF0] != bytes.fromhex("7440"):
        raise AssertionError("flag-2 absent must branch to single actor")
    print("PASS: 13 actor-pane/input-queue instruction windows, 8 original near-call targets")
    print("PASS: original input flags enqueued, dequeued, forwarded unmodified")
    print("PASS: one XOR then four actor redraws versus one actor redraw")
    print("PASS: one 11B63 F790 event before bit-2-only follow-on")


def verify_native_contract() -> None:
    header = (ROOT / "DemoV1/include/rloft/party_panel_controls.hpp").read_text()
    control = (ROOT / "DemoV1/src/party_panel_controls.cpp").read_text()
    tests = (ROOT / "DemoV1/tests/party_panel_controls_tests.cpp").read_text()
    caller = (ROOT / "DemoV1/include/rloft/original_f790_caller_catalog.hpp").read_text()
    # Existing native rectangle corresponds to the original 12F76 inclusive
    # bounds, but must not be mistaken for a fully wired 11AD8 controller.
    for part in ("modal_toggle_actor_at(", "kModalColumnWidth", "kActorCount"):
        assert part in header, part
    for part in ("row < 0x45", "row > 0x60", "local_column < 0x39",
                 "local_column > 0x4D", "return actor;"):
        assert part in control, part
    for part in ("modal_toggle_actor_at(", "0x45, 0x39", "0x60, 0x4D",
                 "0x45, 0x38"):
        assert part in tests, part
    assert ("{0x11b63U,OriginalF790ArgumentOrigin::GlobalHeld8600_8602}"
            in caller), "11B63 must read source-held WORD/known BYTE"
    print("PASS: native actor hit rectangle and 11B63 argument origin")
    print("NOTE: live bit-2 input forwarding, post-F790 side effects and DOS parity remain open")


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--original-code2", type=Path)
    parser.add_argument("--binary-only", action="store_true")
    args = parser.parse_args()
    if args.binary_only and args.original_code2 is None:
        parser.error("--binary-only requires --original-code2")
    if not args.binary_only:
        verify_native_contract()
    if args.original_code2 is not None:
        verify_original(args.original_code2)


if __name__ == "__main__":
    main()
