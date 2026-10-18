#!/usr/bin/env python3
"""Verify original CODE.2 VGA instruction-range SHA256 without publishing bytes."""
from __future__ import annotations
import argparse
import hashlib
from pathlib import Path

ORIGINAL_SHA256 = "853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314"
ORIGINAL_LENGTH = 558689
OBJECT1_OFFSET = 0x3AF24
WINDOWS = {
 "video_init": (0x29953,0x29A84,"b33f63404acdb74fc53f88adddc841612bd792fe2627a765bda8bec108ef78f0"),
 "planar_address": (0x2AB38,0x2AB6C,"51b0f554019e0dcca8236ff9894358b7cea837c0f21defb69f5eeef067e623dd"),
 "framebuffer_base": (0x2AB6C,0x2AB94,"c4073ea577aef5d2957d1e0864ee096eb7116e8a1e2b13d5db01c4816507186a"),
 "indexed_pixel": (0x29AA4,0x29AF0,"20fb8d840fa06b2ace3cac28eb856c5d3dec9e079351cfc2fd6cf19b6dab0ec2"),
 "palette_dac": (0x29CA0,0x29CE6,"54243cf9b97b5dbe2b9e75b30b93a3d53ee3ff9c9242fb49c9d042f05074020f"),
 "crtc_start": (0x2ABF4,0x2AC10,"ace61f68b80924da7c40f774c3b95f311b9ed16fd00bdaf7771441099617c7a5"),
 "planar_image_draw_2a380": (0x2A380,0x2A478,"f5d3f72361f32920058ab1527607be381717c93abeac8b0d2e1ee72d0c0fea91"),
 "palette_rescaler_32150": (0x32150,0x3221F,"298469caa887ee56b8f119c7d38f923f41d1f6c6fec26e475cd73a68303f515f"),
 "colors_bank_loader_325b0": (0x325B0,0x3261C,"176ec8713ca3d022e11e6380294f56d8f1fe77e54864a562f3fbf05f19a375c8"),
}

def verify(data: bytes) -> dict[str,bool]:
    if len(data)!=ORIGINAL_LENGTH or hashlib.sha256(data).hexdigest()!=ORIGINAL_SHA256:
        raise ValueError("Not the authenticated owner-supplied original CODE.2")
    return {name: hashlib.sha256(data[OBJECT1_OFFSET+start:OBJECT1_OFFSET+stop]).hexdigest()==sha
            for name,(start,stop,sha) in WINDOWS.items()}

def self_test() -> None:
    for name,(start,stop,sha) in WINDOWS.items():
        if not (0<=start<stop<ORIGINAL_LENGTH-OBJECT1_OFFSET and len(sha)==64 and
                all(c in "0123456789abcdef" for c in sha)):
            raise AssertionError("Invalid source span record: "+name)
    if len(WINDOWS)!=9: raise AssertionError("Unexpected source manifest count")
    try: verify(b"bad")
    except ValueError: pass
    else: raise AssertionError("Invalid original accepted")
    print("VGA_SOURCE_MANIFEST_SELF_TEST_PASS")

def main() -> int:
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--code2", type=Path, help="Original CODE.2 file; read-only")
    parser.add_argument("--self-test", action="store_true")
    args=parser.parse_args()
    if args.self_test: self_test()
    if args.code2:
        result=verify(args.code2.read_bytes())
        for name,passed in result.items():
            print(f"{name}: {'PASS' if passed else 'FAIL'}")
        if not all(result.values()): return 1
    elif not args.self_test: parser.error("Pass --code2 /path/to/CODE.2 or --self-test")
    return 0

if __name__=="__main__": raise SystemExit(main())
