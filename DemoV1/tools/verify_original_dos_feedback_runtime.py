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
    # Immutable source: 89 DWORD entries F590..F6F3, then RNG F6F4.
    # Validate the ENTIRE table and literal original-key witnesses.
    mapping=b[0x75F24+0xF590:0x75F24+0xF6F4]
    assert len(mapping)==356
    assert hashlib.sha256(mapping).hexdigest()==(
        'fb179b786f755ff0efb0fbdc639ca738fcb6f74a05719103cdd08490b1fe024f')
    values=struct.unpack('<89I',mapping)
    assert values[0]==0
    for scan,expected in {1:0x1b,0xe:8,0x1c:13,0x39:32,
                          0x47:0x147,0x48:0x148,0x58:0x158}.items():
        assert values[scan]==expected,hex(scan)
    assert b[0x75F24+0xF6F4:0x75F24+0xF6F4+4]==bytes((1,0,0,0))
    assert b[0x75F24+0xF572]==0
    # Sibling input services use the same original F572/F590 state
    # but have different polling/consumption semantics.
    siblings={
        (0x30AD8,0x30AEB):
            '12f0d299d6c3adfca40aaec0b8acd901eff26e65ef537659482760cf34e96e0a',
        (0x30B3E,0x30B54):
            '36295f21f86851b1668c46e0f5718a3a349fc8d15faac5a333cdb16b8164af7c',
    }
    for (lo,hi),expected in siblings.items():
        assert hashlib.sha256(obj[lo:hi]).hexdigest()==expected,hex(lo)
        assert obj[hi-1]==0xC3
    sibling_windows={
        0x30AD8:'e88bedffff',0x30ADD:'803d72f5000000',
        0x30AE4:'0f95c0',0x30AE7:'0fb6c0',0x30AEA:'c3',
        0x30B3E:'8bc0',0x30B40:'e823edffff',
        0x30B45:'0fb60572f50000',
        0x30B4C:'8b048590f50000',0x30B53:'c3',
    }
    for at,encoding in sibling_windows.items():
        s=bytes.fromhex(encoding)
        assert obj[at:at+len(s)]==s,hex(at)
    for call in (0x30AD8,0x30B40):
        assert obj[call]==0xE8
        assert call+5+struct.unpack_from('<i',obj,call+1)[0]==0x2F868
    print('ORIGINAL_PASS: 30AD8/30B40 2 spans, 10 windows, 2 E8')
    print('ORIGINAL_PASS: F590 89-entry original keymap, boundary and seed')

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
    for term in ('poll_30aec','print_311e8','capture_initial_f590_from_code2',
                 'input_ready_30ad8','peek_key_30b40'):
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
