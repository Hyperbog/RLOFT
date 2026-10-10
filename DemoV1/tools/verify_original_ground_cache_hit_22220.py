#!/usr/bin/env python3
"""Original CODE.2 335C8 -> 32954 cache-hit/nearest-depth boundary.

This proves instruction-byte-local control flow, not frame timing, DOS RAM,
cache validity, VGA equivalence, or native gameplay integration.
Run with --original-code2 pointing to the immutable Strahd CODE.2.
"""
from __future__ import annotations

import argparse
import hashlib
import struct
from pathlib import Path

SOURCE_SIZE = 558689
SOURCE_SHA256 = "853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314"
OBJECT1_FILE_OFFSET = 0x3AF24
# Whole instruction-boundary windows, including all branch gates, RET and call.
CACHE_WINDOW = (0x32954, 0x329A8, "049bc71f58d6919032a0f72dc8cc2532efbbfe00ae529f965f021885491d8fb8")
RESET_WINDOW = (0x335C8, 0x335DC, "fca0370c85dccd15efc4ec9bc7e5096e7c823168a4266184256fa75d2aa2bae8")


def verify(binary: bytes) -> None:
    if len(binary) != SOURCE_SIZE or hashlib.sha256(binary).hexdigest() != SOURCE_SHA256:
        raise ValueError("incorrect original CODE.2 size, hash or patch identity")
    code = memoryview(binary)[OBJECT1_FILE_OFFSET:]
    for left, right, expected in (CACHE_WINDOW, RESET_WINDOW):
        actual = hashlib.sha256(code[left:right]).hexdigest()
        if actual != expected:
            raise AssertionError(f"whole bounded body {left:05X}..{right:05X} differs: {actual}")

    def at(addr: int, hexdigits: str) -> None:
        expected = bytes.fromhex(hexdigits)
        actual = bytes(code[addr:addr + len(expected)])
        if actual != expected:
            raise AssertionError(f"opcode at {addr:05X} differs: {actual.hex()}")

    # Every call enters with nearest-depth 22220 overwritten by 0x800.
    at(0x335C8, "c7052022020000080000")
    at(0x335D2, "e87df3ffff")
    assert 0x335D2 + 5 + struct.unpack_from("<i", code, 0x335D3)[0] == 0x32954
    at(0x335D7, "5e5f5bc9c3")

    # Callee unconditionally invalidates TWO auxiliary caches before predicate.
    at(0x32954, "c8000000")
    at(0x32958, "c705e8fc0000ffffffff")
    at(0x32962, "c705ecfc0000ffffffff")

    # Four-stage cache-hit predicate (all comparisons are DWORD except immediate).
    at(0x3296C, "833d50cf0000007527")  # [CF50] != 0 -> recompute
    at(0x32975, "a158c60000390504fd0000751a")  # facing C658 == FD04
    at(0x32982, "a1fcfc00003905d0550200750d")  # FCFC == 255D0
    at(0x3298F, "a100fd00003905d4550200740a")  # FD00 == 255D4
    at(0x3299C, "53515657eb06")  # recompute saves regs, jumps to 329A8
    at(0x329A2, "5f5e595b")
    at(0x329A6, "c9c3")  # cache-hit immediate LEAVE/RET

    # Verify branch destinations independently of their mnemonics.
    for address, displacement_offset, next_ip, expected_target in (
        (0x32973, 1, 0x32975, 0x3299C),
        (0x32980, 1, 0x32982, 0x3299C),
        (0x3298D, 1, 0x3298F, 0x3299C),
        (0x3299A, 1, 0x3299C, 0x329A6),
        (0x329A0, 1, 0x329A2, 0x329A8),
    ):
        offset = struct.unpack_from("<b", code, address + displacement_offset)[0]
        if next_ip + offset != expected_target:
            raise AssertionError(f"branch {address:05X} target is not {expected_target:05X}")

    # Consequence: hit returns before 329A8 per-ray traversal/materialization.
    # 22220 remains the reset value 0x800 *at this boundary*; it is NOT
    # recalculated by replaying the previous 256 cached source ray records.


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--original-code2", required=True, type=Path)
    args = parser.parse_args()
    verify(args.original_code2.read_bytes())
    print("ORIGINAL_PASS CODE.2 335C8 reset + 32954 cache-hit early-return boundary")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
