#!/usr/bin/env python3
"""Reproducible source and optional immutable CODE.2 check for DOS feedback.

Source-only is not proof of original binary identity, DOS INT/IRQ behavior,
actual F0C8 installation, or integration into the Windows launch path.
"""
from __future__ import annotations
import argparse
import hashlib
from pathlib import Path
import struct

ROOT = Path(__file__).resolve().parents[2]
ORIGINAL_SHA = "853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314"
RANGES = {
    (0x30AEC, 0x30B19): "98e6449a10488f637fbf79124fd5abd22bf95d69d9b0e0a14b71491539724a4c",
    (0x311E8, 0x31218): "5c362c2024540017c67ba0a24f78c8a148539299ffc9fde08f9d1315ba1f9a95",
    (0x2F868, 0x2F87C): "66d4063312dee76fc87a5787e4066001827ce0b891f77074d4bb450e1cc7dea6",
}
WINDOWS = {
    0x30AEC: "803d72f5000000",  # test live input byte
    0x30AF5: "e86eedffff",      # call graphics/frame callback
    0x30B03: "0fb60572f50000",  # BYTE->DWORD index
    0x30B0A: "8b048590f50000",  # F590[index] DWORD
    0x30B11: "c60572f5000000",# reset pending BYTE
    0x311E8: "535589e5",        # local frame
    0x311F4: "0fbe03",          # sign-extended byte char
    0x311F7: "83f824",          # compare '$'
    0x311FC: "c6032a",          # write '*'
    0x31205: "c60324",          # temporary DOS '$' terminator
    0x3120E: "b409",            # AH=09
    0x31210: "cd21",            # DOS interrupt
    0x31212: "c60300",          # restore NUL terminator
    0x31215: "c95bc3",          # restore stack frame
    0x2F86D: "833dc8f0000000",# dynamic F0C8 test
    0x2F876: "ff15c8f00000",    # dynamic F0C8 call
}
CALLS = {0x30AF5:0x2F868, 0x2F827:0x311E8}

def audit_original(path:Path)->None:
    b=path.read_bytes()
    assert len(b)==558689 and hashlib.sha256(b).hexdigest()==ORIGINAL_SHA, "wrong original CODE.2"
    obj=b[0x3AF24:0x75F24]
    assert len(obj)==0x3B000
    for (lo,hi),expected in RANGES.items():
        assert hashlib.sha256(obj[lo:hi]).hexdigest()==expected, hex(lo)
    for at,sig in WINDOWS.items():
        wanted=bytes.fromhex(sig)
        assert obj[at:at+len(wanted)]==wanted,hex(at)
    for at,target in CALLS.items():
        assert obj[at]==0xE8 and at+5+struct.unpack_from('<i',obj,at+1)[0]==target,hex(at)
    assert obj[0x30B18]==obj[0x31217]==0xC3
    print('ORIGINAL_PASS: CODE.2 identity, 3 source spans, 16 windows, 2 E8 edges')

def audit_repo()->None:
    cm=(ROOT/'DemoV1/CMakeLists.txt').read_text(encoding='utf-8')
    h=(ROOT/'DemoV1/include/rloft/original_dos_feedback_runtime.hpp').read_text(encoding='utf-8')
    src=(ROOT/'DemoV1/src/original_dos_feedback_runtime.cpp').read_text(encoding='utf-8')
    test=(ROOT/'DemoV1/tests/original_dos_feedback_runtime_tests.cpp').read_text(encoding='utf-8')
    audit=(ROOT/'DemoV1/ORIGINAL_DOS_FEEDBACK_RUNTIME_20261010.md').read_text(encoding='utf-8')
    assert 'NAME original_dos_feedback_runtime_tests' in cm
    assert 'NAME original_dos_feedback_binary_contract_tests' in cm
    assert 'src/original_dos_feedback_runtime.cpp' in cm
    for term in ('poll_30aec','print_311e8'):
        assert term in h and term in src and term in test
    for term in ('F572','F590','F0C8','311E8','AH=09','source-only','not wired'):
        assert term in audit,term
    print('SOURCE_ONLY_PASS: native runtime/original binary contracts registered')

if __name__=='__main__':
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--original-code2',type=Path)
    parser.add_argument('--binary-only',action='store_true')
    args=parser.parse_args()
    if args.binary_only and args.original_code2 is None:
        parser.error('--binary-only requires --original-code2')
    if not args.binary_only:
        audit_repo()
    if args.original_code2:
        audit_original(args.original_code2)
