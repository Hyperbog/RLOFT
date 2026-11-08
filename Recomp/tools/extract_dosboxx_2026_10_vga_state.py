#!/usr/bin/env python3
"""Extract VGA POD from DOSBox-X 2026.10.01 (SDL2) save states.

Strictly version-specific. Refuses unverified state layouts. Uses standard
Python only; does not modify the source .sav or require native-frame data.
Private game/video bytes MUST NOT be committed to Git.
"""
import argparse
import hashlib
import json
import zipfile
from pathlib import Path

VERSION=b"DOSBox-X 2026.10.01 (SDL2)"
PAYLOAD_LENGTH=2643024
CRTC_OFFSET=0x82F8C
DAC_OFFSET=0x82FB1
RGB_OFFSET=0x82FD1
VRAM_OFFSET=0x84A40
VRAM_BYTES=2097152
FRAME_PIXELS=320*400


def check(condition,message):
    if not condition:
        raise ValueError(message)


def digest(raw):
    return hashlib.sha256(raw).hexdigest()


def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument("state",type=Path)
    parser.add_argument("--output",required=True,type=Path)
    args=parser.parse_args()
    out=args.output
    out.mkdir(parents=True,exist_ok=True)
    with zipfile.ZipFile(args.state) as z:
        check(z.read("DOSBox-X_Version")==VERSION,"Unverified DOSBox-X save-state version")
        check(z.read("Machine_Type")==b"MCH_VGA","State is not VGA")
        vga=z.read("Vga")
        program=z.read("Program_Name").decode("ascii","replace")
        stamp=z.read("Time_Stamp").decode("ascii","replace")
    check(len(vga)==PAYLOAD_LENGTH,"Unexpected VGA serialization size")
    crtc=vga[CRTC_OFFSET:CRTC_OFFSET+27]
    check(len(crtc)==27 and list(crtc[:4])==[0x5f,0x4f,0x50,0x82],
          "CRTC layout mismatch")
    check(crtc[19]==0x28 and crtc[23]==0xe3,
          "Unrecognized CRTC stride or addressing mode")
    dac=vga[DAC_OFFSET:RGB_OFFSET+768]
    check(dac[0]==6 and dac[1]==255,"Unrecognized DAC precision/pixel mask")
    rgb6=vga[RGB_OFFSET:RGB_OFFSET+768]
    check(len(rgb6)==768 and all(channel<=63 for channel in rgb6),
          "Invalid RGB6 palette")
    start=(crtc[12]<<8)|crtc[13]
    line_compare=crtc[24]|((crtc[7]&16)<<4)|((crtc[9]&64)<<3)
    check(start in (0,0x8000) and line_compare>=399,
          "Special display start/split requires source-specific decoding")
    memory=vga[VRAM_OFFSET:VRAM_OFFSET+VRAM_BYTES]
    check(len(memory)==VRAM_BYTES,"Truncated video-memory array")
    frame=memory[start*4:start*4+FRAME_PIXELS]
    check(len(frame)==FRAME_PIXELS,"CRTC-visible page truncated")
    (out/"original_dosboxx_rgb6.pal").write_bytes(rgb6)
    (out/"original_saved_crtc_page.idx").write_bytes(frame)
    for page in range(4):
        # Four interleaved VGA planes, 64KiB each, covering the original
        # 0x0000 and 0x8000 draw/display pages.
        b=memory[page:0x40000:4]
        check(len(b)==65536,"VGA plane extraction incomplete")
        (out/f"original_vga_plane{page}.bin").write_bytes(b)
    report={
        "save_version":VERSION.decode(),
        "program_name":program,
        "time_stamp":stamp,
        "vga_pod_sha256":digest(vga),
        "vga_pod_length":len(vga),
        "source_offsets":{"crtc":hex(CRTC_OFFSET),"dac":hex(DAC_OFFSET),
                          "rgb6":hex(RGB_OFFSET),"vram":hex(VRAM_OFFSET)},
        "crtc_registers_00_18":list(crtc[:25]),
        "crtc_start_address":start,
        "crtc_offset_register":crtc[19],
        "crtc_line_compare":line_compare,
        "dac_bits":dac[0],
        "dac_pel_mask":dac[1],
        "original_rgb6_sha256":digest(rgb6),
        "saved_crtc_page_sha256":digest(frame),
        "caution":"A save-state memory snapshot is not guaranteed to coincide with the rendered frame in a separate screenshot. Verify display/animation timing independently.",
    }
    (out/"dosboxx_vga_state_report.json").write_text(json.dumps(report,indent=2)+"\n")
    print(json.dumps(report,indent=2))


if __name__=="__main__":
    main()
