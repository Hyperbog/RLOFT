"""Bounded original CODE.2 literal-address FF50 operand audit.

Direct references only; does not eliminate indirect writes or
nested callback side effects. Original files are never committed.
"""
from __future__ import annotations
import argparse
import hashlib
from pathlib import Path

ROOT=Path(__file__).resolve().parents[2]
SHA="853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314"
BASE=0x3AF24
END=0x75F24
PATTERN=bytes.fromhex("50ff0000")
OPERANDS={
    0x1F29F:bytes.fromhex("890d"),  # initial upper store
    0x1F2A7:bytes.fromhex("c705"),  # open-field upper store
    0x1F310:bytes.fromhex("833d"),  # compare before upper texture
    0x1F379:bytes.fromhex("031d"),  # reload after upper callbacks
    0x1F380:bytes.fromhex("a3"),    # computed lower store
    0x34ABB:bytes.fromhex("8b0d"),  # 34A74 first phase read
    0x34B49:bytes.fromhex("8b0d"),  # 34A74 paired phase read
}

def original(path:Path)->None:
    b=path.read_bytes()
    assert len(b)==558689 and hashlib.sha256(b).hexdigest()==SHA,"wrong Strahd CODE.2"
    obj=b[BASE:END]
    offsets=[]
    pos=0
    while True:
        pos=obj.find(PATTERN,pos)
        if pos<0:break
        offsets.append(pos)
        pos+=1
    assert sorted(offsets)==sorted(OPERANDS),offsets
    for at,opcode in OPERANDS.items():
        assert obj[at-len(opcode):at]==opcode,hex(at)
    print("PASS: seven original FF50 operand references, 3 writes, 4 read/compare")
    print("BOUND: indirect alias/callback writes remain unresolved")

def native()->None:
    cm=(ROOT/"DemoV1/CMakeLists.txt").read_text()
    doc=(ROOT/"DemoV1/LEVEL22_ORIGINAL_FF50_DIRECT_REFERENCE_AUDIT_20261010.md").read_text()
    assert "NAME original_ground_ff50_direct_reference_tests" in cm
    assert "1F377" in doc and "34A74" in doc
    assert "indirect" in doc.lower()
    print("PASS: direct-reference audit and bounded documentation registered")

def main()->None:
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument("--original-code2",type=Path)
    p.add_argument("--binary-only",action="store_true")
    a=p.parse_args()
    if a.binary_only and a.original_code2 is None:
        p.error("--binary-only requires --original-code2")
    if not a.binary_only:native()
    if a.original_code2 is not None:original(a.original_code2)
if __name__=="__main__":main()
