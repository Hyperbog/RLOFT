"""Original Strahd CODE.2 FDBC / FF48 direct reference census.

Independent original-byte oracle; does not prove DOS ES selector,
indirect aliases or all runtime C050 writers. Original CODE.2 is
supplied by the user, never committed.
"""
from __future__ import annotations

import argparse
import hashlib
from pathlib import Path

ROOT=Path(__file__).resolve().parents[2]
SOURCE_SHA="853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314"
SOURCE_LENGTH=558689
BASE=0x3AF24
CODE_LENGTH=0x3A852
# Every literal 32-bit absolute address operand in original
# CODE.2 object-1 (not an assertion about indirect memory).
DIRECT_OPERANDS={
    0xFDBC: {
        0x1F1E7:"a3",    # only direct FDBC store (page base)
        0x273FA:"8b3d",  # sprite/composite destination read
        0x33DF5:"8b3d",  # original lower flat-fill read
        0x33E3D:"8b3d",  # original upper flat-fill read
        0x3420D:"031d",  # add base to other raster destination
        0x34A9E:"0305",  # source texture adds active page base
    },
    0xFF48: {
        0x1F345:"c705", # upper raster offset = 0x148
        0x1F3B6:"c705", # lower raster offset = 0x50F8
        0x34A98:"a1",   # texture source reads offset
    },
}
CONTRACT_WINDOWS={
    0x1F1D6:"a150c00000c1e00f0500000a00", # PAGE 0xA0000+(DWORD C050<<15)
    0x1F1E6:"a3bcfd0000",
    0x1F343:"c70548ff000048010000",
    0x1F3B4:"c70548ff0000f8500000",
    0x34A97:"a148ff00000305bcfd0000a340ff0000",
}

def check_original(path:Path)->None:
    entire=path.read_bytes()
    assert len(entire)==SOURCE_LENGTH and hashlib.sha256(entire).hexdigest()==SOURCE_SHA,"incorrect original CODE.2"
    code=entire[BASE:BASE+CODE_LENGTH]
    assert len(code)==CODE_LENGTH
    for addr,required in DIRECT_OPERANDS.items():
        pattern=addr.to_bytes(4,"little")
        observed=[]
        cursor=0
        while True:
            cursor=code.find(pattern,cursor)
            if cursor<0:break
            observed.append(cursor)
            cursor+=1
        assert observed==sorted(required),f"unexpected original {addr:04X} operand positions {observed}"
        for offset,previous in required.items():
            opcode=bytes.fromhex(previous)
            assert code[offset-len(opcode):offset]==opcode,hex(offset)
    for offset,encoding in CONTRACT_WINDOWS.items():
        expected=bytes.fromhex(encoding)
        assert code[offset:offset+len(expected)]==expected,hex(offset)
    assert entire[0x75F24+0xC050:0x75F24+0xC054]==bytes([1,0,0,0])
    print("PASS: six exact FDBC direct references (one direct store), three FF48 (two stores)")
    print("PASS: source page/offset formula and shipped C050 DWORD initial value")
    print("OPEN: computed-pointer writes, C050 mutation, ES selector and DOS runtime state")

def check_repository()->None:
    cm=(ROOT/"DemoV1/CMakeLists.txt").read_text()
    doc=(ROOT/"DemoV1/LEVEL22_ORIGINAL_VGA_FDBC_FF48_ADDRESS_CENSUS_20261010.md").read_text()
    assert "NAME original_ground_vga_page_base_reference_tests" in cm
    for term in ("FDBC","FF48","C050","ES selector","indirect"):
        assert term in doc,term
    print("PASS: original page-base address audit registered")

if __name__=="__main__":
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument("--original-code2",type=Path)
    p.add_argument("--binary-only",action="store_true")
    args=p.parse_args()
    if args.binary_only and args.original_code2 is None:
        p.error("--binary-only requires --original-code2")
    if not args.binary_only:check_repository()
    if args.original_code2 is not None:check_original(args.original_code2)
