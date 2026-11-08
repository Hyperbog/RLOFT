#!/usr/bin/env python3
"""Native ground C050-vs-EF4D provenance source guard.

Public CTest uses no proprietary media; --original accepts the privately
authenticated original CODE.2 for full-SHA and exact x86/data evidence.
"""
from __future__ import annotations
import argparse
import hashlib
from pathlib import Path

ROOT=Path(__file__).resolve().parents[2]
SHA="853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314"
FDBC=bytes.fromhex("a1 50 c0 00 00 c1 e0 0f 05 00 00 0a 00 83 c4 04 a3 bc fd 00 00")

def check_source(files:dict[str,str])->None:
    needs={
        "model":("kCode2Object2FileOffset+0xC050U",
                 "initial_ground_draw_page_c050_=",
                 "model.initial_video_page_ = code2_bytes[kVideoPageAt]"),
        "header":("initial_ground_draw_page_c050()",
                  "initial_ground_fdbc()","0xA0000U+"),
        "test":("#undef NDEBUG",
                "model.initial_ground_fdbc() == 0xA8000U",
                "altered_ground[kDataFileStart+0xC050U]=2U"),
        "runtime":("out.initial_code2_ground_draw_page_c050 =",
                   "scene_vga_masks_->initial_ground_draw_page_c050()"),
        "capture":("EF4D-native-selected-not-C050",
                   "original_initial_C050_ground_draw_page",
                   "original_live_C050_ground_draw_page"),
        "capturetest":("frame.initial_code2_ground_draw_page_c050=1U",
                       "original_live_C050_ground_draw_page\\tunverified"),
        "cmake":("original_ground_c050_ef4d_domain_contract_tests",),
    }
    for label,tokens in needs.items():
        for token in tokens:
            if token not in files[label]:
                raise ValueError(f"{label}: missing {token}")

def check_original(data:bytes)->None:
    if len(data)!=558689 or hashlib.sha256(data).hexdigest()!=SHA:
        raise ValueError("original CODE.2 identity differs")
    dgroup=0x75F24
    if data[dgroup+0xC050:dgroup+0xC054]!=b"\x01\x00\x00\x00":
        raise ValueError("original initial DWORD C050 is not one")
    if data[dgroup+0xEF4D]!=0:
        raise ValueError("original initial BYTE EF4D is not zero")
    at=0x3AF24+0x1F1D6
    if data[at:at+len(FDBC)]!=FDBC:
        raise ValueError("original 1F1D6 FDBC x86 window differs")

def main()->int:
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--root",type=Path,default=ROOT)
    parser.add_argument("--original",type=Path)
    opts=parser.parse_args()
    mapping={
        "model":"DemoV1/src/type7_vga_plane_masks.cpp",
        "header":"DemoV1/include/rloft/type7_vga_plane_masks.hpp",
        "test":"DemoV1/tests/type7_vga_plane_masks_tests.cpp",
        "runtime":"DemoV1/src/exploration_runtime.cpp",
        "capture":"DemoV1/src/exploration_frame_capture.cpp",
        "capturetest":"DemoV1/tests/exploration_frame_capture_tests.cpp",
        "cmake":"DemoV1/CMakeLists.txt",
    }
    try:
        files={k:(opts.root/p).read_text(encoding="utf-8")
               for k,p in mapping.items()}
        check_source(files)
        mutated=dict(files)
        mutated["model"]=mutated["model"].replace(
            "kCode2Object2FileOffset+0xC050U",
            "kCode2Object2FileOffset+0xEF4DU",1)
        if mutated["model"]==files["model"]:
            raise ValueError("mutation target missing")
        try:
            check_source(mutated)
        except ValueError:
            pass
        else:
            raise ValueError("C050/EF4D alias mutation escaped")
        if opts.original:
            check_original(opts.original.read_bytes())
    except (OSError,ValueError) as exc:
        parser.exit(1,"GROUND_PAGE_DOMAIN_REJECTED: "+str(exc)+"\n")
    print("GROUND_PAGE_DOMAIN_SOURCE_PASS"+
          (" ORIGINAL_CODE2_PASS" if opts.original else ""))
    return 0

if __name__=="__main__":
    raise SystemExit(main())
