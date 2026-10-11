#!/usr/bin/env python3
"""Authenticate the original CODE.2 keyboard controller and PIC wrapper.

Source-only mode checks repository registration/integration boundaries.
Original-byte mode proves executable provenance but not DOS PIC timing,
installed ISR vectors, prefix translation or Windows keyboard parity.
"""
from __future__ import annotations

import argparse
import hashlib
from pathlib import Path
import struct

ROOT=Path(__file__).resolve().parents[2]
CODE2_SHA="853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314"
PRELUDE=(0x308F8,0x30950)
PRELUDE_SHA="bb2b244478a8d5f4e6fc4a8027d1c605b76a3d59cc851de8888886de18064a89"
WINDOWS={
    0x308F8:'601e060fa00fa889e5fc',
    0x30902:'e8654d0000',
    0x30907:'ba60000000',
    0x3090E:'ec',
    0x3090F:'ba61000000',
    0x30918:'ec',
    0x30919:'a270f50000',
    0x30925:'3d80000000',
    0x3092A:'750f',
    0x3092E:'ec',
    0x30934:'800d70f5000080',
    0x3093B:'a070f50000',
    0x30940:'ba61000000',
    0x30945:'0c80',
    0x30947:'ee',
    0x30948:'0fb60570f50000',
    0x3094F:'ee',
    0x30950:'fa',
    0x30A5C:'b020',
    0x30A63:'ee',
    0x30A64:'fb',
    0x30A6D:'cf',
    0x30A83:'b020',
    0x30A8A:'ee',
    0x30A8B:'fb',
    0x30A94:'cf',
    0x30AA8:'b020',
    0x30AAF:'ee',
    0x30AB0:'fb',
    0x30AB9:'cf',
}

def verify_original(path:Path)->None:
    data=path.read_bytes()
    assert len(data)==558689,"CODE.2 size differs from pinned original"
    assert hashlib.sha256(data).hexdigest()==CODE2_SHA,"incorrect original CODE.2"
    obj=data[0x3AF24:0x75F24]
    lo,hi=PRELUDE
    assert hashlib.sha256(obj[lo:hi]).hexdigest()==PRELUDE_SHA
    assert len(WINDOWS)==30
    for at,encoded in WINDOWS.items():
        wanted=bytes.fromhex(encoded)
        assert obj[at:at+len(wanted)]==wanted,hex(at)
    # Only one immediate near CALL is present in the 308F8 prelude.
    assert obj[0x30902]==0xE8
    assert 0x30902+5+struct.unpack_from('<i',obj,0x30903)[0]==0x3566C
    # A separate comparison of F570 (port 61h) to 80h is NOT an
    # E0/E1 keyboard prefix interpreter. The raw scan is in EBX.
    assert obj[0x30951:0x30957]==bytes.fromhex('81fb80000000')
    # The two special exits and the ordinary tail each end in IRET.
    assert [obj[at] for at in (0x30A6D,0x30A94,0x30AB9)]==[0xCF]*3
    print("ORIGINAL_PASS: keyboard hardware 88-byte wrapper SHA, 30 windows")
    print("ORIGINAL_PASS: call 3566C, controller ack, 3 PIC EOI/IRET exits")

def verify_repository()->None:
    cm=(ROOT/'DemoV1/CMakeLists.txt').read_text(encoding='utf-8')
    keyboard=(ROOT/'DemoV1/src/original_dos_keyboard_session.cpp').read_text(encoding='utf-8')
    audit=(ROOT/'DemoV1/ORIGINAL_DOS_KEYBOARD_SESSION_MPLAY01_20261011.md').read_text(encoding='utf-8')
    assert 'NAME original_dos_keyboard_session_tests' in cm
    assert 'NAME original_dos_keyboard_isr_hardware_source_tests' in cm
    for token in ('receive_port60','consume_key','peek_key','input_ready'):
        assert token in keyboard,token
    for token in ('308F8','30950','30925','0x60','0x61','0x20','E0/E1','Win32','not wired'):
        assert token in audit,token
    print("SOURCE_ONLY_PASS: original hardware wrapper audit and native seam")

if __name__=='__main__':
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--original-code2',type=Path)
    parser.add_argument('--binary-only',action='store_true')
    args=parser.parse_args()
    if args.binary_only and args.original_code2 is None:
        parser.error('--binary-only requires --original-code2')
    if not args.binary_only:verify_repository()
    if args.original_code2 is not None:verify_original(args.original_code2)
