#!/usr/bin/env python3
"""CODE.2 input poll / optional graphics callback and DOS text output proof.

Original binary oracle, not proof of installed callback targets,
interrupt side effects, valid key indices, or DOS runtime parity.
"""
from __future__ import annotations

import argparse
import hashlib
from pathlib import Path
import struct

ROOT=Path(__file__).resolve().parents[2]
CODE2_SHA="853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314"
START=0x3AF24
LENGTH=0x3A852

# Ret-bounded helpers and two explicitly truncated code slices.
BOUNDS={
    (0x30AEC,0x30B19):"98e6449a10488f637fbf79124fd5abd22bf95d69d9b0e0a14b71491539724a4c",
    (0x311E8,0x31218):"5c362c2024540017c67ba0a24f78c8a148539299ffc9fde08f9d1315ba1f9a95",
    (0x3058C,0x30592):"f7b4ce2cb8758811b4ff02ca828fd9b05642a2e3956ebbbb73444a82cf89a580",
    (0x30594,0x3059A):"c4c0f5bc3083b3d0b06c46fd1e80febf6b4f9da50c8537cb13ee361fce983ea3",
    (0x2F868,0x2F87C):"66d4063312dee76fc87a5787e4066001827ce0b891f77074d4bb450e1cc7dea6",
    (0x2F77A,0x2F7AF):"6ee60f9fe675c777b0bc4e3ce4cf0a628df67485bb75ecdd345b95c6ea42fe2f",
}
CALLS={0x30AF5:0x2F868,0x2F77C:0x30AEC,
       0x2F79E:0x30AEC,0x2F827:0x311E8}
BRANCHES={
    0x30AF3:0x30B03,0x30B01:0x30AF5,
    0x2F77A:0x2F79E,0x2F790:0x2F77C,
    0x2F7AD:0x2F79E,0x2F874:0x2F87C,
}
WINDOWS={
    0x30AEC:"803d72f5000000",0x30AF5:"e86eedffff",
    0x30AFA:"803d72f5000000",0x30B03:"0fb60572f50000",
    0x30B0A:"8b048590f50000",0x30B11:"c60572f5000000",
    0x30B18:"c3",0x2F868:"565783ec50",
    0x2F86D:"833dc8f0000000",0x2F874:"7406",
    0x2F876:"ff15c8f00000",0x2F77C:"e86b130000",
    0x2F783:"83f84e",0x2F788:"83f859",
    0x2F78D:"83f853",0x2F792:"83fe53",
    0x2F79E:"e849130000",0x2F7A5:"83f820",
    0x2F7AA:"83f854",0x2F827:"e8bc190000",
    0x311E8:"535589e5",0x311F4:"0fbe03",
    0x311FC:"c6032a",0x31205:"c60324",
    0x3120E:"b409",0x31210:"cd21",
    0x31212:"c60300",0x31215:"c9",
    0x31216:"5bc3",0x3058C:"a184b60200c3",
    0x30594:"a180b60200c3",
}

def audit_original(path:Path)->None:
    original=path.read_bytes()
    assert len(original)==558689
    assert hashlib.sha256(original).hexdigest()==CODE2_SHA,"wrong CODE.2"
    obj=original[START:START+LENGTH]
    assert len(obj)==LENGTH
    for (lo,hi),digest in BOUNDS.items():
        assert hashlib.sha256(obj[lo:hi]).hexdigest()==digest,hex(lo)
    for lo,hi in ((0x30AEC,0x30B19),(0x311E8,0x31218),
                  (0x3058C,0x30592),(0x30594,0x3059A)):
        assert obj[hi-1]==0xC3,hex(hi)
        for operand in (0xC050,0xF0C8,0xF0CC):
            assert struct.pack("<I",operand) not in obj[lo:hi]
    for at,target in CALLS.items():
        assert obj[at]==0xE8,hex(at)
        assert at+5+struct.unpack_from("<i",obj,at+1)[0]==target
    for at,target in BRANCHES.items():
        assert at+2+struct.unpack_from("<b",obj,at+1)[0]==target
    assert len(WINDOWS)==31
    for at,code in WINDOWS.items():
        signature=bytes.fromhex(code)
        assert obj[at:at+len(signature)]==signature,hex(at)
    print("ORIGINAL_PASS: 6 source slices, 4 E8, 6 branches, 31 windows")
    print("OPEN: F0C8 installer, DOS IRQ/register effects, ES and key bounds")

def audit_repository()->None:
    cm=(ROOT/"DemoV1/CMakeLists.txt").read_text(encoding="utf-8")
    manifest=(ROOT/"DemoV1/tools/run_original_ground_proof_batch.py").read_text(encoding="utf-8")
    doc=(ROOT/"DemoV1/LEVEL22_ORIGINAL_VGA_C050_PAGE_TOGGLE_AUDIT_20261010.md").read_text(encoding="utf-8")
    assert "NAME original_ground_input_callback_poll_tests" in cm
    assert '"input_f0c8", "verify_original_ground_input_callback_poll.py"' in manifest
    for word in ("30AEC","F572","F590","2F868","311E8","AH=09","INT 21h","N/Y/S"):
        assert word in doc,word
    print("SOURCE_ONLY_PASS: input poll source contract and registration")

if __name__=="__main__":
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--original-code2",type=Path)
    parser.add_argument("--binary-only",action="store_true")
    args=parser.parse_args()
    if args.binary_only and args.original_code2 is None:
        parser.error("--binary-only requires --original-code2")
    if not args.binary_only:
        audit_repository()
    if args.original_code2 is not None:
        audit_original(args.original_code2)
