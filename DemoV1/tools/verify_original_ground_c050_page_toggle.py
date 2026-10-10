#!/usr/bin/env python3
"""Original Strahd CODE.2 C050 direct-operand page-state census.

This is a binary literal-address audit, NOT proof of indirect aliases,
callee-saved EBP, original DOS runtime page bounds, or segment mapping.
"""
from __future__ import annotations
import argparse
from collections import Counter
import hashlib
from pathlib import Path
import struct

ROOT=Path(__file__).resolve().parents[2]
SIZE=558689
SHA="853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314"
OBJ_START=0x3AF24
OBJ_SIZE=0x3A852
DATA_START=0x75F24
C050=0xC050
# Literal DWORD occurrence -> exact immediately preceding opcode.
# Reconcile every occurrence, not a selected subset.
PATTERNS={
    "push_dword":("ff35",115),
    "movzx_byte":("0fb605",111),
    "xor_byte":("8035",56),
    "mov_eax":("a1",25),
    "mov_al":("a0",13),
    "imul_edx":("6915",6),
    "mov_edx":("8b15",5),
    "mov_ecx":("8b0d",4),
    "imul_eax":("6905",2),
    "mov_esi":("8b35",1),
    "xor_dword_ebp":("312d",1),
}

def audit_original(path:Path)->None:
    b=path.read_bytes()
    assert len(b)==SIZE and hashlib.sha256(b).hexdigest()==SHA,"wrong original CODE.2"
    obj=b[OBJ_START:OBJ_START+OBJ_SIZE]
    assert len(obj)==OBJ_SIZE
    assert b[DATA_START+C050:DATA_START+C050+4]==b"\x01\x00\x00\x00"
    encoded=struct.pack("<I",C050)
    positions=[]
    at=0
    while True:
        at=obj.find(encoded,at)
        if at<0:break
        positions.append(at)
        at+=4
    assert len(positions)==339,len(positions)
    found=Counter()
    byte_sites=[]
    wide_sites=[]
    for at in positions:
        matches=[name for name,(hex_op,_) in PATTERNS.items()
                 if obj[at-len(bytes.fromhex(hex_op)):at]==bytes.fromhex(hex_op)]
        assert len(matches)==1,(hex(at),matches)
        name=matches[0]
        found[name]+=1
        if name=="xor_byte":
            assert obj[at+4]==1,hex(at)
            byte_sites.append(at-2)
        if name=="xor_dword_ebp":
            wide_sites.append(at-2)
    for name,(_,expected) in PATTERNS.items():
        assert found[name]==expected,(name,found[name],expected)
    assert wide_sites==[0x16221] and 0x160C0 in byte_sites
    # The caller locally initializes EBP=1, but intervening calls
    # and original protected-mode callback effects remain unproved.
    source_windows={
        0x160C0:"803550c0000001",
        0x160E1:"bd0100000089f7",
        0x160E8:"e807410000",
        0x160F6:"ebf0",
        0x161FC:"5557e815e0ffff",
        0x16216:"ff3550c00000",
        0x1621C:"e88f430000",
        0x16221:"312d50c00000",
        0x1622A:"0fb60550c00000",
        0x16234:"e8772f0100",
        0x1623C:"e9a7feffff",
        0x1F1C8:"530fb60550c00000",
        0x1F1D6:"a150c00000c1e00f",
    }
    for at,hex_op in source_windows.items():
        expected=bytes.fromhex(hex_op)
        assert obj[at:at+len(expected)]==expected,hex(at)
    print("ORIGINAL_PASS: original CODE.2 SHA and C050=1 initializer")
    print("ORIGINAL_PASS: 339 C050 operands: 282 reads, 56 byte toggles, 1 DWORD EBP toggle")
    print("BOUND: EBP=1 at 160E1; original call preservation and aliases remain open")

def audit_repository()->None:
    cm=(ROOT/"DemoV1/CMakeLists.txt").read_text(encoding="utf-8")
    doc=(ROOT/"DemoV1/LEVEL22_ORIGINAL_VGA_C050_PAGE_TOGGLE_AUDIT_20261010.md").read_text(encoding="utf-8")
    runner=(ROOT/"DemoV1/tools/run_original_ground_proof_batch.py").read_text(encoding="utf-8")
    assert "NAME original_ground_c050_page_toggle_tests" in cm
    assert '"c050_page", "verify_original_ground_c050_page_toggle.py"' in runner
    for word in ("339","56","16221","160E1","FDBC","indirect","EBP"):
        assert word in doc,word
    print("SOURCE_ONLY_PASS: registered CTest, original ground batch and audit notes")

if __name__=="__main__":
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument("--original-code2",type=Path)
    p.add_argument("--binary-only",action="store_true")
    a=p.parse_args()
    if a.binary_only and a.original_code2 is None:
        p.error("--binary-only requires --original-code2")
    if not a.binary_only:audit_repository()
    if a.original_code2 is not None:audit_original(a.original_code2)
