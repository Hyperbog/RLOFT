#!/usr/bin/env python3
"""Private optional original CODE.2 oracle + public planar source regression."""
from __future__ import annotations
import argparse
import hashlib
from pathlib import Path

ROOT=Path(__file__).resolve().parents[2]
SHA="853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314"
ORIGINAL_FUNCTIONS=(
    (0x33DE0,70,"af26ff06080d1a9c321f187185a2c751c8ae1f982b26380607daf62a4c4f0dbe"),
    (0x33E28,67,"8e9622be88f5265cddb916d4cc41ff0ee1c93714952ce31d7a37863495844da0"),
)
FDBC_BYTES=bytes.fromhex("a150c00000c1e00f0500000a0083c404a3bcfd0000")

def source_verify(impl: str, test: str, header: str, cmake: str) -> None:
    requirements=(
        (impl,(
            "original_c050_page>1U",
            "VgaPlanarMemory::kPhysicalBase+",
            "original_c050_page)<<15U",
            "upper?0x148U:0x50F8U",
            "write_sequencer_raw_mask_02(0x0FU)",
            "base_color-original_fe94.subtract[kRows-row]",
            "video.write(edi+byte,0x0FU,color)",
            "edi=upper ? edi+kStride : edi-kStride",
        )),
        (header,("replay_flat_es(","kRows=0x80U","kBytesPerRow=0x40U","kStride=0x50U")),
        (test,("actual.pixels()==reference.pixels()",
                "actual.pixels()==page1_ref.pixels()",
                "0xA0148U","0xA50F8U","0xA8148U","0xA2928U","0xAA908U")),
        (cmake,("src/original_ground_planar_fill_replay.cpp",
                 "original_ground_planar_fill_replay_tests",
                 "original_ground_planar_fill_binary_contract_tests")),
    )
    for source,tokens in requirements:
        for token in tokens:
            if token not in source:
                raise ValueError("source replay lost original contract: "+token)

def original_verify(data: bytes) -> None:
    if len(data)!=558689 or hashlib.sha256(data).hexdigest()!=SHA:
        raise ValueError("CODE.2 full-file SHA or length differs")
    code=data[0x3AF24:]
    if code[0x1F1D6:0x1F1D6+len(FDBC_BYTES)]!=FDBC_BYTES:
        raise ValueError("CODE.2 1F1D6 FDBC address math differs")
    for addr,length,sha in ORIGINAL_FUNCTIONS:
        if hashlib.sha256(code[addr:addr+length]).hexdigest()!=sha:
            raise ValueError(f"CODE.2 full original fill body {addr:05X} differs")

def main() -> None:
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument("--original",type=Path)
    a=p.parse_args()
    try:
        paths=(
            "DemoV1/src/original_ground_planar_fill_replay.cpp",
            "DemoV1/tests/original_ground_planar_fill_replay_tests.cpp",
            "DemoV1/include/rloft/original_ground_planar_fill_replay.hpp",
            "DemoV1/CMakeLists.txt",
        )
        contents=tuple((ROOT/p).read_text(encoding="utf-8") for p in paths)
        source_verify(*contents)
        impl,test,header,cmake=contents
        mutants=(
            (impl.replace("original_c050_page)<<15U",
                          "original_c050_page)<<16U",1),test,header,cmake),
            (impl.replace("video.write(edi+byte,0x0FU,color)",
                          "video.write(edi+byte,0x03U,color)",1),test,header,cmake),
            (impl.replace("upper?0x148U:0x50F8U",
                          "upper?0x148U:0x5000U",1),test,header,cmake),
        )
        for i,m in enumerate(mutants):
            if m==contents:
                raise ValueError(f"mutation {i} failed to change replay")
            try:
                source_verify(*m)
            except ValueError:
                continue
            raise ValueError(f"mutation {i} escaped original replay contract")
        if a.original:
            original_verify(a.original.read_bytes())
    except (OSError,ValueError) as e:
        p.exit(1,f"GROUND_PLANAR_SOURCE_REJECTED: {e}\n")
    print("GROUND_PLANAR_FILL_SOURCE_PASS"+
          (" ORIGINAL_CODE2_PASS" if a.original else ""))

if __name__=="__main__":
    main()
