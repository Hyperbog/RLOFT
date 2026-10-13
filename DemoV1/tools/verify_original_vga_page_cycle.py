#!/usr/bin/env python3
"""Original VGA page choreography source-only CTest and optional x86 oracle.

Original files are NEVER committed to GitHub or read by default CTests.
With --original, verify the user's pinned CODE.2 before comparing windows.
"""
from __future__ import annotations
import argparse
import hashlib
from pathlib import Path

ROOT=Path(__file__).resolve().parents[2]
SHA="853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314"
ORIGINAL_WINDOWS={
    0x1EA78:"ff 35 50 c0 00 00 e8 8d ba ff ff 80 35 50 c0 00 00 01 83 c4 04 c3",
    0x291B0:"0f b6 44 24 04 50 e8 31 0e 00 00 83 c4 04 8a 44 24 04 a2 4d ef 00 00 c3",
    0x1F1D6:"a1 50 c0 00 00 c1 e0 0f 05 00 00 0a 00 83 c4 04 a3 bc fd 00 00",
}

def contract(header:str,test:str,cmake:str)->None:
    for part,keys in (
        (header,(
            "original.draw_page_c050 << 15U",
            "const auto pushed=original.draw_page_c050",
            "callbacks.present_1a510(pushed);",
            "original.draw_page_c050 ^= 1U;",
            "const auto page=static_cast<std::uint8_t>(argument);",
            "callbacks.select_29fec(static_cast<std::uint32_t>(page));",
            "original.selected_page_ef4d=page;",
            "if(!callbacks.present_1a510)",
            "if(!callbacks.select_29fec)",
        )),
        (test,(
            "#undef NDEBUG",
            "s.ground_base_fdbc==0xA8000U",
            "s.ground_base_fdbc==0xA0000U",
            "s.draw_page_c050==0xF0000100U",
            "s.selected_page_ef4d==0xABU",
            "Model::present_then_flip_1ea78(s,callbacks)",
            "Model::select_then_store_291b0(s,0x101U,callbacks)",
        )),
        (cmake,(
            "original_vga_page_cycle_tests",
            "original_vga_page_cycle_binary_contract_tests",
        )),
    ):
        for key in keys:
            if key not in part:
                raise ValueError("missing source/order assertion: "+key)
    bad=header.replace("original.draw_page_c050 ^= 1U;",
                       "original.selected_page_ef4d ^= 1U;",1)
    if bad==header:
        raise ValueError("unable to apply regression mutation")
    try:
        contract_inner(bad)
    except ValueError:
        pass
    else:
        raise ValueError("EF4D alias defect was not rejected")

def contract_inner(header:str)->None:
    for key in (
        "original.draw_page_c050 ^= 1U;",
        "callbacks.present_1a510(pushed);",
        "original.selected_page_ef4d=page;",
    ):
        if key not in header:
            raise ValueError("original page source regression: "+key)

def original_oracle(data:bytes)->None:
    if len(data)!=558689 or hashlib.sha256(data).hexdigest()!=SHA:
        raise ValueError("private CODE.2 full-file identity mismatch")
    for addr,code in ORIGINAL_WINDOWS.items():
        want=bytes.fromhex(code)
        at=0x3AF24+addr
        if data[at:at+len(want)]!=want:
            raise ValueError(f"CODE.2 original x86 differs at {addr:05X}")
    if data[0x75F24+0xC050:0x75F24+0xC054]!=b"\x01\x00\x00\x00":
        raise ValueError("CODE.2 original initial C050 DWORD differs")
    if data[0x75F24+0xEF4D]!=0:
        raise ValueError("CODE.2 original initial EF4D byte differs")

def main()->int:
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument("--root",type=Path,default=ROOT)
    p.add_argument("--original",type=Path,help="private, uncommitted CODE.2")
    a=p.parse_args()
    try:
        header=(a.root/"DemoV1/include/rloft/original_vga_page_cycle.hpp").read_text()
        test=(a.root/"DemoV1/tests/original_vga_page_cycle_tests.cpp").read_text()
        cmake=(a.root/"DemoV1/CMakeLists.txt").read_text()
        contract(header,test,cmake)
        if a.original:
            original_oracle(a.original.read_bytes())
    except (ValueError,OSError) as e:
        p.exit(1,"VGA_PAGE_CYCLE_REJECTED: "+str(e)+"\n")
    print("VGA_PAGE_CYCLE_SOURCE_PASS"+
          (" ORIGINAL_CODE2_PASS" if a.original else ""))
    return 0

if __name__=="__main__":
    raise SystemExit(main())
