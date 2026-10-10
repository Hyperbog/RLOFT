"""Verify original CODE.2 wall-depth FF50 terrain clipping contract.

Source-only CTest keeps the native bounded FF50 helper present and
isolated from the Windows renderer. Optional --original-code2 and
--original-sintab independently check exact source instruction windows
and original scale-block C738 input. No original assets are committed.
"""
from __future__ import annotations

import argparse
import hashlib
import struct
from pathlib import Path

ROOT=Path(__file__).resolve().parents[2]
CODE2_SHA="853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314"
SINTAB_SHA="19b6bc15f1be616bb8ff49dc9029995a1a0413fae06addc9fd41ddef761d0e87"
OBJECT1_BASE=0x3AF24

# Source CODE.2 physical file offsets = 0x3AF24 + logical address.
# Each is an instruction-aligned, independently disassembled window.
WINDOWS={
    0x039D0:"c7051856010000000000",  # table[0] = 0
    0x03A20:"899b18560100",          # table[EBX/4] = EBX
    0x1F255:"813d20220200000800007444", # nearest 22220 == 800?
    0x1F273:"8b1c8518560100",       # read DWORD [15618+index*4]
    0x1F27A:"89da89d8c1fa1f2bc2d1f8b98000000089da29c189d8c1fa1fc1e2031bc2c1f80301c1",
    0x1F29D:"890d50ff0000",         # first FF50 = upper
    0x1F2A5:"c70550ff00008000000031db", # open sentinel, EBX=0
    0x1F372:"b800010000031d50ff000029d8a350ff000085c07e52", # lower
    0x1F384:"85c07e52",             # <=0 lower: skip 34A74
    0x1F3C3:"e8ac560100",           # original 34A74 lower caller
    0x34AB9:"8b0d50ff000083f9507c05b950000000", # min 80 rows
    0x34B47:"8b0d50ff000083e95083f9007e55", # FF50-80 paired rows
}

def verify_original_code2(path:Path)->None:
    original=path.read_bytes()
    if len(original)!=558689 or hashlib.sha256(original).hexdigest()!=CODE2_SHA:
        raise AssertionError("not immutable original Strahd CODE.2")
    code=original[OBJECT1_BASE:]
    for addr,encoding in WINDOWS.items():
        expected=bytes.fromhex(encoding)
        if code[addr:addr+len(expected)]!=expected:
            raise AssertionError(f"source CODE.2 FF50 mismatch 0x{addr:05X}")
    assert code[0x1F398]==0xE8
    target=0x1F398+5+struct.unpack_from("<i",code,0x1F399)[0]
    assert target==0x2F29C,"selected original RES4 loader"
    assert code[0x1F3C3]==0xE8
    target=0x1F3C3+5+struct.unpack_from("<i",code,0x1F3C4)[0]
    assert target==0x34A74,"lower 34A74 caller"
    print(f"PASS: {len(WINDOWS)} original FF50 instruction windows, 2 calls")

def verify_original_sintab(path:Path)->None:
    source=path.read_bytes()
    if len(source)!=333757 or hashlib.sha256(source).hexdigest()!=SINTAB_SHA:
        raise AssertionError("not immutable original supplied SINTAB")
    c738=struct.unpack_from("<I",source,0x1B38)[0]
    lookup_max=struct.unpack_from("<I",source,0x1B3C)[0]
    assert c738==127 and lookup_max==508
    # CODE.2 03A20 writes exactly [15618+4*i] = 4*i from
    # i=1 through C738. 15618[0] is set by 039D0.
    for index in (0,1,2,50,51,52,c738):
        ebx=4*index
        upper=128-ebx//2+ebx//8
        lower=256-(ebx+upper)
        assert upper==128-2*index+index//2
        assert lower==128-2*index-index//2
    print("PASS: immutable SINTAB C738=127 and original lookup_max=508")

def verify_native()->None:
    header=(ROOT/"DemoV1/include/rloft/original_ground_ff50_source_span.hpp").read_text()
    tests=(ROOT/"DemoV1/tests/original_ground_ff50_source_span_tests.cpp").read_text()
    cmake=(ROOT/"DemoV1/CMakeLists.txt").read_text()
    renderer=(ROOT/"DemoV1/src/exploration_presenter.cpp").read_text()
    for fragment in (
        "OriginalGroundFF50SourceModel",
        "kNoWallSentinel=0x800",
        "const auto ebx=static_cast<std::int32_t>(index*4U)",
        "const auto upper=128-ebx/2+ebx/8",
        "const auto lower=256-(ebx+upper)",
        "lower_four_plane_rows()",
        "lower_paired_plane_rows()",
    ):
        assert fragment in header,fragment
    for fragment in (
        "original_c738=127U",
        "51*8", "52*8",
        "0x800", "0x801",
        "original_c738",
    ):
        assert fragment in tests,fragment
    assert "NAME original_ground_ff50_source_span_tests" in cmake
    assert "original_ground_ff50_source_span_tests.cpp" in cmake
    # This deliberately is NOT connected to the game-loop renderer
    # because source 22220 acquisition and state C026/C027 are pending.
    assert "OriginalGroundFF50SourceModel" not in renderer
    print("PASS: bounded native FF50 projection, unhooked from host VGA")

def main()->None:
    cli=argparse.ArgumentParser(description=__doc__)
    cli.add_argument("--original-code2",type=Path)
    cli.add_argument("--original-sintab",type=Path)
    cli.add_argument("--binary-only",action="store_true")
    args=cli.parse_args()
    if args.binary_only and (args.original_code2 is None or args.original_sintab is None):
        cli.error("--binary-only requires both original CODE.2 and SINTAB")
    if not args.binary_only:verify_native()
    if args.original_code2:verify_original_code2(args.original_code2)
    if args.original_sintab:verify_original_sintab(args.original_sintab)

if __name__=="__main__":
    main()
