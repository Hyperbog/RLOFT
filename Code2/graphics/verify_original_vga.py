#!/usr/bin/env python3
"""Verify original CODE.2 VGA/resource instruction SHA-256 without publishing bytes."""
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
 "res_image_layout_2a20c": (0x2A20C,0x2A256,"3c469e26818f3c532e07ef15f5edf6f9895812a6c0df951392f19484357c1539"),
 "res_image_loader_2fd9c": (0x2FD9C,0x30040,"bd24fd53a88d3fd58bb34d2bcaf8c68f650d87f2099e8dc0d046bebf7a5297a2"),
 "res_cached_lookup_2f29c": (0x2F29C,0x2F2E0,"2662ebb816f7d7e2ce1b541b7b416835a297c067662840bcb6843c70ec32547e"),
 "res_lookup_draw_2975c": (0x2975C,0x2978A,"57133ad4a6f17943d795e39c4fbefe47e8d2f8260ddd5d42099c2dfba74e6c7d"),
 "res_layout_permutation_37810": (0x37810,0x37958,"3b4abcfdc9dad165e0aba16bc070a7863508c9a4d1ac70eeb12ae98b1e4ee156"),
 "res_original_lzss_37700": (0x37700,0x3780D,"61c42dd14fd8db7a22e439ff433ed851343e160d38b1b0d88ac5548ecc7f9915"),
 "res_typed_header_2eb60": (0x2EB60,0x2EC73,"5c98f9e862c7cda25a0a050947ba9ef1dd486c461665aaaec170529892fe4ec5"),
 "res_unloaded_dispatch_2ecb4": (0x2ECB4,0x2EFB3,"787b20bda33e47803ed2f13cb90e203835e9f55fdcd0416e428a87745ba17e9b"),
 "res_stream_align_2f96c": (0x2F96C,0x2F9AE,"4b40b4aa87a161b4a568e12206f9f23eca59211f1663d50b6d9a26e41f525eb8"),
 "res_stream_byte_2f9e4": (0x2F9E4,0x2FA2F,"6b63596dc23c7949dc6c201c6025bb92fac290dd50b43a0524b71b1277d75586"),
 "res_lzss_dispatch_2fb04": (0x2FB04,0x2FB66,"8685f1bcf40a6f3adf36a45cf60f76dc768ed1a7bf25e6dcd64b52323e83c7dc"),
 "res_lzss_pack_2fa30": (0x2FA30,0x2FB01,"d7746363c0a980d7c84cc27ffd57febdae1b4b41b973f2b32689bb7ba75abf76"),
 "res_startup_slots_2e7f0": (0x2E7F0,0x2E9CB,"b762134b4bc2d7e4d1c3da2ea9db97f303235212e4c7c1dc55e096baee7bb7c7"),
 "res_type6_loader_37590": (0x37590,0x375F0,"47c69a86484cfb139763bc18bee1fe72cfba523ddf58a9abd7fbbff3464f1441"),
 "res4_column_select_33647": (0x33647,0x33665,"d1e0b75e4cf429c81f6a4294f6266c4ee508d609231b28355296438fe518d226"),
 "res4_fixed_point_sampler_338c8": (0x338C8,0x3395A,"80e61b5265ca7c9512332895079f07b02ab7e8dee610ffc6b000173cc76946d7"),
 "res4_vga_plane_selector_3395c": (0x3395C,0x339A2,"7bccc9fd1f104e0d2cbae0fb041c2df398e1f8708919aeab0159ad582576c6dd"),
 "init_source_header_16d04": (0x16D04,0x17097,"41af04fcfe969f2dbccbcd830eded9c35ce6e758c4b203ac49379e34ea62fee7"),
 "info_dat_archive_flags_2e768": (0x2E768,0x2E7F0,"7ab9c6d1ce7c170e33bd913d6a27cf85fe020fa13c969ad125f1ffa91ee8a5e0"),
 "world_column_compositor_3263c": (0x3263C,0x32952,"f13246466f26e83b31e3f6800a4a3a76a940ccc71d598ae11dfa3ce74d6782ee"),
 "world_camera_sweep_32954": (0x32954,0x32F4A,"c8f30081beebcd0bcc31d057d4a72f94abb5730a74a9d7d967720b816eb07416"),
 "world_prepare_334c4": (0x334C4,0x335DC,"66a8a82aeffae8ff115ae4e314d766582c159ec629d6dce222dd319d03a2586d"),
 "world_two_pass_wall_sweep_335dc": (0x335DC,0x337C7,"f43d0ef565a18eb347dabdf4ec30d2a35afaa02fcbfe5dbe2e7747922be07f59"),
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
    if len(WINDOWS)!=32: raise AssertionError("Unexpected source manifest count")
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
