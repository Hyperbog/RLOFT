#!/usr/bin/env python3
"""Original CODE.2 F790 SS:ESP-local T[0x3F] pre-read regression.

Source-only mode (no commercial binaries):
  python Code2/verify_f790_stack_local_preread.py

Original-byte mode (read local immutable files; never write them):
  python Code2/verify_f790_stack_local_preread.py --code2 /path/CODE.2 --t /path/T

This verifies a previously omitted executable call, not DOS/port parity.
"""
from __future__ import annotations

import argparse
import hashlib
from pathlib import Path
import struct

ROOT=Path(__file__).resolve().parents[1]
CODE2_SHA256="853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314"
ORIGINAL_OBJECT1=0x3AF24
F790_READ_BYTES=bytes.fromhex("6a3f8d44240450e890cb000083c408")

def source_contract(source:Path)->None:
    text=source.read_text(encoding="utf-8")
    before="struct Application21Boundary {"
    function="void function_0f790(Machine& m,Application21Boundary& b,s32 item,u32 detailed) noexcept {"
    assert text.count(before)==1, "source boundary structure changed"
    assert text.count(function)==1, "F790 entry changed"
    declaration=text.split(before,1)[1].split("};",1)[0]
    body=text.split(function,1)[1].split("static_assert(0x0FD84u",1)[0]
    assert "void (*call_2c368_local)(u8*,u32)=nullptr;" in declaration
    assert "u8 text_3f_stack_local[0x10u];" in body
    assert "if(b.call_2c368_local)b.call_2c368_local(text_3f_stack_local,0x3Fu);" in body
    pending=body.index("if(*dg(m,0x6DACu)!=0u)")
    fetch=body.index("if(b.call_2c368_local)")
    empty=body.index("if(item==-1)return;")
    assert pending<fetch<empty, "original pending -> local T[3F] -> empty order lost"
    assert body.count("call_2c368_local")==2, "unexpected local read sites"
    print("PASS canonical Code2.cpp: F790 scratch T[3F] order and stack-local reader")

def source_binary(path:Path)->None:
    full=path.read_bytes()
    assert len(full)==558689 and hashlib.sha256(full).hexdigest()==CODE2_SHA256
    object1=full[ORIGINAL_OBJECT1:ORIGINAL_OBJECT1+0x3A852]
    assert object1[0xF794:0xF797]==bytes.fromhex("83ec10")
    assert object1[0xF7B4:0xF7CC]==bytes.fromhex(
        "803dac6d000000740f689c750000e8a5feffffe9ad050000")
    assert object1[0xF7CC:0xF7DB]==F790_READ_BYTES
    assert object1[0xF7D3]==0xE8
    displacement=struct.unpack_from("<i",object1,0xF7D4)[0]
    assert 0xF7D3+5+displacement==0x1C368
    assert object1[0xF7DB:0xF7E4]==bytes.fromhex("83fbff0f8498050000")
    # The called 1C368 reads through original DS:22224 stream handle,
    # terminates the result, and XOR-decodes its bytes with 0xDC.
    assert object1[0x1C36C:0x1C374]==bytes.fromhex("8b5c24108b442414")
    assert object1[0x1C3E2:0x1C3F1]==bytes.fromhex(
        "0fb7442404c604030066837c240400")
    print("PASS original CODE.2: F790 16-byte local and T[3F] E8 before empty ID")
    
def t_archive(path:Path)->None:
    raw=path.read_bytes()
    assert len(raw)==133584 and struct.unpack_from("<I",raw,0)[0]==1073
    ix=0x3F
    at=struct.unpack_from("<I",raw,4+4*ix)[0]
    length=struct.unpack_from("<H",raw,at)[0]
    assert length==1 and raw[at+2]^0xDC==ord("D")
    print("PASS original T: index 0x3F decodes one byte 'D', 16-byte scratch fits")

def main()->None:
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument("--source",type=Path,default=ROOT/"Code2/Code2.cpp")
    p.add_argument("--code2",type=Path)
    p.add_argument("--t",type=Path)
    args=p.parse_args()
    source_contract(args.source)
    if args.code2:source_binary(args.code2)
    if args.t:t_archive(args.t)

if __name__=="__main__":
    main()
