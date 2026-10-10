"""Independent original CODE.2 1F1C8 upper/lower surface-mode oracle.

The native plan is deliberately staged because original upper callbacks
(29B40,314B0,33E28,34A74) run BEFORE lower 1F377 reloads FF50.
Optional original CODE.2 checks verify instruction windows and all
related source near-call targets, not names from reconstructed C++.
"""
from __future__ import annotations

import argparse
import hashlib
import struct
from pathlib import Path

ROOT=Path(__file__).resolve().parents[2]
SOURCE_SHA="853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314"
CODE_BASE=0x3AF24

# All windows begin on disassembled instruction boundaries.
WINDOWS={
    0x1F227:"803d26c00000007509803d27c0000000741c",
    0x1F239:"a1f0bf00003b0558c60000740f",
    0x1F2B1:"0fb615a858020081faff0000007445",
    0x1F2C0:"0fb605b058020083f82c7528",
    0x1F2F4:"803d28c00000007463",
    0x1F305:"803d27c0000000744b",
    0x1F30E:"833d50ff0000007e52",
    0x1F317:"0fbf051d5602006a040305f8580200",
    0x1F369:"803d26c00000007458",
    0x1F372:"b800010000031d50ff000029d8a350ff0000",
    0x1F384:"85c07e52",
    0x1F3CA:"0fb605b158020050e8094a010083c404",
    0x1F3DA:"e8095a0100e8c8c9ffffe8abce0000",
}
CALLS={
    0x1F250:0x34998,  # conditional perspective regenerate
    0x1F2E7:0x29B40,  # selected overlay kind 2C
    0x1F2FE:0x314B0,  # selected active dynamic overlay
    0x1F327:0x2F29C,  # upper RES4 source selected by 2561D
    0x1F352:0x34A74,  # upper original texture path
    0x1F361:0x33E28,  # other upper fill
    0x1F398:0x2F29C,  # lower RES4 source selected by 2561F
    0x1F3C3:0x34A74,  # lower original texture path
    0x1F3D2:0x33DE0,  # C026=0 flat lower
    0x1F3DA:0x34DE8,  # compositing tail, always after lower
    0x1F3DF:0x1BDAC,
    0x1F3E4:0x2C294,
    0x1F3E9:0x1EA04,
    0x1F3EE:0x274A4,
    0x1F3F3:0x2F868,
}

def check_original(path:Path)->None:
    data=path.read_bytes()
    if len(data)!=558689 or hashlib.sha256(data).hexdigest()!=SOURCE_SHA:
        raise AssertionError("not the immutable original Strahd CODE.2")
    obj=data[CODE_BASE:]
    for at,source in WINDOWS.items():
        actual=obj[at:at+len(bytes.fromhex(source))]
        if actual!=bytes.fromhex(source):
            raise AssertionError(f"original 1F1C8 instruction mismatch 0x{at:05x}")
    for at,expected in CALLS.items():
        if obj[at]!=0xE8 or at+5+struct.unpack_from("<i",obj,at+1)[0]!=expected:
            raise AssertionError(f"original 1F1C8 call mismatch 0x{at:05x}")
    print(f"PASS: {len(WINDOWS)} original upper/lower mode windows, "
          f"{len(CALLS)} original near-call targets")
    print("PASS: 1F369 C026 second-stage branch occurs after original upper calls")

def check_native()->None:
    header=(ROOT/"DemoV1/include/rloft/original_ground_surface_dispatch.hpp").read_text()
    tests=(ROOT/"DemoV1/tests/original_ground_surface_dispatch_tests.cpp").read_text()
    renderer=(ROOT/"DemoV1/src/exploration_presenter.cpp").read_text()
    cmake=(ROOT/"DemoV1/CMakeLists.txt").read_text()
    for part in (
        "OriginalUpperSurfaceAction",
        "OriginalLowerSurfaceAction",
        "Overlay29B40", "Overlay314B0", "Fill33E28",
        "Textured34A74", "Fill33DE0",
        "plan_upper(", "plan_lower(",
        "original_ff50_after_upper_callbacks",
        "upper.original_ebx_15618+",
        "input.original_selected_258a8!=0xFFU",
        "input.original_c027==0U",
        "input.original_c026!=0U || input.original_c027!=0U",
        "original_c026==0U",
        "OriginalGroundFF50SourceModel::from_22220",
    ):
        assert part in header,part
    for part in (
        "UA::Overlay29B40","UA::Overlay314B0","UA::Fill33E28",
        "UA::Textured34A74","UA::SkipNonpositiveFF50",
        "LA::Fill33DE0","LA::SkipNonpositiveFF50",
        "140,1U","248,1U",
        "numeric_limits<std::int32_t>::min()",
    ):
        assert part in tests,part
    assert "NAME original_ground_surface_dispatch_tests" in cmake
    assert "NAME original_ground_surface_dispatch_binary_contract_tests" in cmake
    # No premature live drawing. Source mode and 22220 producer integration
    # remain separate gates; stage objects cannot have Win32 callbacks.
    assert "OriginalGroundSurfaceDispatch" not in renderer
    print("PASS: native original surface mode stages remain isolated from host")

def main()->None:
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument("--original-code2",type=Path)
    p.add_argument("--binary-only",action="store_true")
    args=p.parse_args()
    if args.binary_only and not args.original_code2:
        p.error("--binary-only needs --original-code2")
    if not args.binary_only:check_native()
    if args.original_code2:check_original(args.original_code2)

if __name__=="__main__":
    main()
