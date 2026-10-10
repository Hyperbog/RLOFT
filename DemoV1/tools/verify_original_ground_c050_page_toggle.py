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
    # Source-proven EBP preservation edges, NOT transitive ABI or callback
    # safety. 16400 enters a shared older-address epilogue (1602C).
    ebp_bounded_spans={
        (0x1608C,0x1624B):"06a38753b857225922bdcd64fc227240236235b49885277f98cd86c3fd876a59",
        (0x14218,0x14309):"26ac89581eed8a55c40a5a7e1f97c676b759a6132cb89fd7284c6dd70bd020ce",
        (0x16400,0x16524):"89ca8b9e8be1c00d726d68a0339e4ee4906782e9189292f2ea24d1e5694ffc71",
        (0x1A1F4,0x1A21D):"fc11e3acbaeb44a13b97555470d0c4fe9985889f86cd70e62d695f131182bbdd",
        (0x1A5B0,0x1A724):"2c1e3cf2a887bdcda658487698fe113b34d487dda4038050bd69f0cca14427c6",
    }
    for (start,end),sha in ebp_bounded_spans.items():
        assert hashlib.sha256(obj[start:end]).hexdigest()==sha,hex(start)
    ebp_windows={
        0x1608C:"53565755",0x160E1:"bd01000000",
        0x160E8:"e807410000",0x161F7:"e804020000",
        0x161FC:"5557e815e0ffff",0x1621C:"e88f430000",
        0x16221:"312d50c00000",
        0x14218:"53565755",0x1425A:"bd202c0100",
        0x14304:"5d5f5e5bc3",
        0x16400:"53565755",0x16411:"31ed",
        0x16431:"0f8d6cfbffff",0x164BF:"0f85defaffff",
        0x1651F:"e977faffff",
        0x15FA3:"834424085083c504",
        0x1602C:"83c40c5d5f5e5bc3",
        0x1A1F4:"e86f560100",
        0x2F868:"565783ec50",
        0x2F86D:"833dc8f0000000",
        0x2F876:"ff15c8f00000",
        0x1A5B0:"535683ec08",
        0x1A6E8:"83c4085e5bc3",
        0x1A71E:"83c4085e5bc3",
    }
    for at,hex_code in ebp_windows.items():
        expected=bytes.fromhex(hex_code)
        assert obj[at:at+len(expected)]==expected,hex(at)
    ebp_calls={
        0x160E8:0x1A1F4, 0x1A1F4:0x2F868,
        0x161F7:0x16400, 0x161FE:0x14218,
        0x1621C:0x1A5B0, 0x14224:0x291B0,
    }
    for at,dest in ebp_calls.items():
        assert obj[at]==0xE8,hex(at)
        actual=at+5+struct.unpack_from("<i",obj,at+1)[0]
        assert actual==dest,(hex(at),hex(actual),hex(dest))
    ebp_tail_branches={
        0x16431:(6,0x15FA3),
        0x164BF:(6,0x15FA3),
        0x1651F:(5,0x15F9B),
    }
    for at,(length,dest) in ebp_tail_branches.items():
        if length==6:
            assert obj[at:at+2] in (b"\x0f\x8d",b"\x0f\x85")
            displacement=struct.unpack_from("<i",obj,at+2)[0]
        else:
            assert obj[at]==0xE9
            displacement=struct.unpack_from("<i",obj,at+1)[0]
        assert at+length+displacement==dest,(hex(at),hex(dest))
    print("ORIGINAL_PASS: 5 EBP source spans, 24 opcodes, 6 E8 edges, 3 cross-tail branches")
    print("BOUND: 14218 and 16400 locally preserve EBP; 16400 uses 1602C shared epilogue")
    print("OPEN: intervening callback/callee effects, especially 160E8->1A1F4->2F868->F0C8")
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
    for term in ("14218","16400","1602C","1A1F4","2F868","F0C8"):
        assert term in doc,term
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
