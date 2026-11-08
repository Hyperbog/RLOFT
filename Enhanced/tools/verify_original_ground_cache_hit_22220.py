#!/usr/bin/env python3
"""Bounded CODE.2 334C4 -> 335C8 -> 32954 ray-cache boundary.

Source-only mode tests a pure gate/side-effect model, not original bytes.
Original mode also checks the immutable shipped CODE.2 and instruction edges.
No executable, game pixels, or changed native gameplay state are produced.
"""
from __future__ import annotations

import argparse
from dataclasses import dataclass
import hashlib
import struct
from pathlib import Path

SOURCE_ID = (558689, "853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314")
OBJECT1 = 0x3AF24
WINDOWS = (
    (0x32954, 0x329A8, "049bc71f58d6919032a0f72dc8cc2532efbbfe00ae529f965f021885491d8fb8"),
    (0x334C4, 0x334F8, "aa569615352648e60537a55be7dc9be8921a4d3cc1897885fa0a7d7c6a42f83b"),
    (0x335C8, 0x335DC, "fca0370c85dccd15efc4ec9bc7e5096e7c823168a4266184256fa75d2aa2bae8"),
)


def dword(value: int) -> int:
    return value & 0xFFFFFFFF


@dataclass(frozen=True)
class CacheGate:
    # Original direct DWORD comparisons, not byte or signed conditions.
    cf50: int
    facing_c658: int
    cached_facing_fd04: int
    pos_x_fcfc: int
    saved_pos_x_255d0: int
    pos_y_fd00: int
    saved_pos_y_255d4: int


@dataclass(frozen=True)
class CacheBoundary:
    cache_hit: bool
    enters_recompute: bool
    nearest_at_caller_return: int | None
    invalidated_fce8: int = 0xFFFFFFFF
    invalidated_fcec: int = 0xFFFFFFFF
    initial_nearest_22220: int = 0x800


def evaluate_boundary(state: CacheGate) -> CacheBoundary:
    """Model ONLY the proven branch/early-return; miss-result is unknown."""
    hit = (dword(state.cf50) == 0
           and dword(state.facing_c658) == dword(state.cached_facing_fd04)
           and dword(state.pos_x_fcfc) == dword(state.saved_pos_x_255d0)
           and dword(state.pos_y_fd00) == dword(state.saved_pos_y_255d4))
    return CacheBoundary(cache_hit=hit, enters_recompute=not hit,
                         nearest_at_caller_return=0x800 if hit else None)


def test_source_model() -> None:
    from dataclasses import replace

    baseline = CacheGate(0, 7, 7, 500, 500, 901, 901)
    hit = evaluate_boundary(baseline)
    assert hit.cache_hit and not hit.enters_recompute
    assert hit.initial_nearest_22220 == hit.nearest_at_caller_return == 0x800
    assert (hit.invalidated_fce8, hit.invalidated_fcec) == (0xFFFFFFFF, 0xFFFFFFFF)
    for change in (dict(cf50=1), dict(facing_c658=8),
                   dict(pos_x_fcfc=501), dict(pos_y_fd00=902)):
        miss = evaluate_boundary(replace(baseline, **change))
        assert not miss.cache_hit and miss.enters_recompute
        assert miss.nearest_at_caller_return is None  # no guessed min/depth
        assert miss.initial_nearest_22220 == 0x800
        assert miss.invalidated_fce8 == miss.invalidated_fcec == 0xFFFFFFFF
    # Equality uses 32-bit operands and does not treat signed negatives specially.
    assert evaluate_boundary(replace(baseline, facing_c658=-1,
                                     cached_facing_fd04=0xFFFFFFFF)).cache_hit
    assert not evaluate_boundary(replace(baseline, cf50=0x100)).cache_hit


def verify_original(binary: bytes) -> None:
    if len(binary) != SOURCE_ID[0] or hashlib.sha256(binary).hexdigest() != SOURCE_ID[1]:
        raise ValueError("incorrect original CODE.2 size, hash or patch identity")
    code = memoryview(binary)[OBJECT1:]
    for left, right, expected_sha in WINDOWS:
        actual = hashlib.sha256(code[left:right]).hexdigest()
        if actual != expected_sha:
            raise AssertionError(f"bounded original body {left:05X}..{right:05X} differs")

    def exact(addr: int, hexbytes: str) -> None:
        expected = bytes.fromhex(hexbytes)
        if bytes(code[addr:addr + len(expected)]) != expected:
            raise AssertionError(f"opcode window mismatch {addr:05X}")

    # Original frame entry calls camera/ray preparation before FF50 decision.
    exact(0x1F1F0, "e8cf420100")
    assert 0x1F1F0 + 5 + struct.unpack_from("<i", code, 0x1F1F1)[0] == 0x334C4
    # 334C4's global side effects execute even when 32954 later hits cache.
    exact(0x334CB, "0fb705195602000fb71d1b5602000fafc3a30cfd0000")
    exact(0x334E1, "bb000000008b0d0cfd000080245de0980100bf434975f4")
    assert 0x334F6 + 2 + struct.unpack_from("<b", code, 0x334F7)[0] == 0x334EC
    # Reset and call occur on both cache-hit and recomputation paths.
    exact(0x335C8, "c7052022020000080000")
    exact(0x335D2, "e87df3ffff")
    assert 0x335D2 + 5 + struct.unpack_from("<i", code, 0x335D3)[0] == 0x32954
    exact(0x335D7, "5e5f5bc9c3")
    exact(0x32958, "c705e8fc0000ffffffff")
    exact(0x32962, "c705ecfc0000ffffffff")
    exact(0x3296C, "833d50cf0000007527")
    exact(0x32975, "a158c60000390504fd0000751a")
    exact(0x32982, "a1fcfc00003905d0550200750d")
    exact(0x3298F, "a100fd00003905d4550200740a")
    exact(0x3299C, "53515657eb06")
    exact(0x329A2, "5f5e595b")
    exact(0x329A6, "c9c3")
    for address, next_ip, target in (
        (0x32973, 0x32975, 0x3299C),
        (0x32980, 0x32982, 0x3299C),
        (0x3298D, 0x3298F, 0x3299C),
        (0x3299A, 0x3299C, 0x329A6),
        (0x329A0, 0x329A2, 0x329A8),
    ):
        if next_ip + struct.unpack_from("<b", code, address + 1)[0] != target:
            raise AssertionError(f"branch destination mismatch {address:05X}")
    # If 22220 stays 0x800, the original frame uses 128 upper FF50 rows.
    # This only verifies its DIRECT guard; intervening indirect writes are open.
    exact(0x1F255, "813d20220200000800007444")
    exact(0x1F2A5, "c70550ff000080000000")


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--original-code2", type=Path)
    args = parser.parse_args()
    test_source_model()
    if args.original_code2 is not None:
        verify_original(args.original_code2.read_bytes())
        print("ORIGINAL_PASS: 334C4 prepass, 335C8 reset, 32954 cache gate and model")
    else:
        print("SOURCE_ONLY_PASS: 32954 cache gate model (no original binary proof)")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
