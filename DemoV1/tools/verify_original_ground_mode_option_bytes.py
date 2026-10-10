"""Original CODE.2 C026/C027 mutable options / ground dispatch contract.

Checks exact shipped game binary only when --original-code2 is given.
No shipped game bytes are committed. A successful default CTest proves
source/test registration, not original DOS live menu state.
"""
from __future__ import annotations

import argparse
import hashlib
import struct
from pathlib import Path

ROOT=Path(__file__).resolve().parents[2]
CODE2_SHA="853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314"
BASE=0x3AF24
DGROUP=0x75F24
WINDOWS={
    0x05F59:"0fb60526c00000",  # GAME.OPT display Floors
    0x05F6A:"0fb60527c00000",  # GAME.OPT display Ceiling
    0x065CF:"803526c0000001", # mutate C026 ^= 1
    0x065E7:"803527c0000001", # mutate C027 ^= 1
    0x1F227:"803d26c0000000", # 34998 conditional first byte
    0x1F230:"803d27c0000000",
    0x1F305:"803d27c0000000", # upper 34A74 vs 33E28
    0x1F369:"803d26c0000000", # lower 34A74 vs 33DE0
    0x34D46:"803d26c0000000", # surface record low-flag branch
    0x34D95:"803d27c0000000", # source flag=0x20 branch
}
REFERENCES={
    0xC026:(0x05F5C,0x065D1,0x1F229,0x1F36B,0x34D48),
    0xC027:(0x05F6D,0x065E9,0x1F232,0x1F307,0x34D97),
}

def check_original(path:Path)->None:
    b=path.read_bytes()
    assert len(b)==558689 and hashlib.sha256(b).hexdigest()==CODE2_SHA, "wrong original Strahd CODE.2"
    code=b[BASE:BASE+0x3A852]
    assert b[DGROUP+0xC026:DGROUP+0xC028]==bytes([1,1]),"shipped C026/C027 defaults"
    for address,expected_hex in WINDOWS.items():
        expected=bytes.fromhex(expected_hex)
        assert code[address:address+len(expected)]==expected, f"source opcode {address:05X}"
    for address,expected in REFERENCES.items():
        data=struct.pack("<I",address)
        actual=[]
        start=0
        while True:
            start=code.find(data,start)
            if start<0:break
            actual.append(start)
            start+=1
        assert tuple(actual)==expected,(hex(address),actual)
    assert struct.unpack_from("<I",code,0x6520+4)[0]==0x65C3 # Floors menu arm
    assert struct.unpack_from("<I",code,0x6520+8)[0]==0x65DB # Ceiling menu arm
    assert code[0x65C3:0x65C9]==bytes.fromhex("81f9a8000000")
    assert code[0x65DB:0x65E1]==bytes.fromhex("81f9a8000000")
    print("PASS: 10 C026/C027 original machine windows, 10 unique operand references")
    print("PASS: original mutable GAME.OPT default 1/1, menu arms 1/2, XOR guards")
    print("BOUND: direct addressing only; live game option ownership not yet integrated")

def check_native()->None:
    options=(ROOT/"DemoV1/include/rloft/options_menu.hpp").read_text()
    impl=(ROOT/"DemoV1/src/options_menu.cpp").read_text()
    tests=(ROOT/"DemoV1/tests/options_menu_tests.cpp").read_text()
    dispatch=(ROOT/"DemoV1/include/rloft/original_ground_surface_dispatch.hpp").read_text()
    cm=(ROOT/"DemoV1/CMakeLists.txt").read_text()
    doc=(ROOT/"DemoV1/LEVEL22_ORIGINAL_C026_C027_OPTIONS_GROUND_DISPATCH_20261010.md").read_text()
    for token in ("Floors = 1","Ceiling = 2","raw[1] != 0","raw[2] != 0","kToggleMaxRowExclusive = 0xA8"):
        assert token in options,token
    assert "state.raw[index] ^ 1U" in impl
    for token in ("OriginalGroundSurfaceDispatch::plan_upper","Fill33DE0","Textured34A74",
                  "!stage->recompute_original_34998","modes.raw==before_plan"):
        assert token in tests,token
    for token in ("input.original_c026!=0U || input.original_c027!=0U",
                  "input.original_c027==0U","original_c026==0U"):
        assert token in dispatch,token
    assert "NAME original_ground_mode_option_bytes_tests" in cm
    assert "original DOS" in doc
    print("PASS: native original option and isolated staged terrain source contract")

def main()->None:
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument("--original-code2",type=Path)
    p.add_argument("--binary-only",action="store_true")
    a=p.parse_args()
    if a.binary_only and a.original_code2 is None:
        p.error("--binary-only requires --original-code2")
    if not a.binary_only:check_native()
    if a.original_code2 is not None:check_original(a.original_code2)
if __name__=="__main__":main()
