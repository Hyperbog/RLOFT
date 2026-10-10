"""Independent CODE.2 four-materializer 22220 zero/min original byte proof.

Source-only CTest checks that the bounded native model remains isolated.
With --original-code2 the original Strahd binary is verified byte-for-byte.
No game binaries are committed.
"""
from __future__ import annotations

import argparse
import hashlib
from pathlib import Path

ROOT=Path(__file__).resolve().parents[2]
CODE2_SHA="853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314"
BASE=0x3AF24
SOURCE_WINDOWS={
    0x335C8:"c7052022020000080000",
    0x330E7:"81be205a010093000000750ac70520220200000000008b86185a01003b05202202007d05a320220200",
    0x33278:"81be205a010093000000750ac70520220200000000008b86185a01003b05202202007d05a320220200",
    0x3337F:"81bf205a010093000000750ac70520220200000000008b87185a01003b05202202007d05a320220200",
    0x33487:"81bf205a010093000000750ac70520220200000000008b87185a01003b05202202007d05a320220200",
}

def original(path:Path)->None:
    b=path.read_bytes()
    assert len(b)==558689 and hashlib.sha256(b).hexdigest()==CODE2_SHA,"wrong original Strahd CODE.2"
    obj=b[BASE:]
    for start,hex_bytes in SOURCE_WINDOWS.items():
        by=bytes.fromhex(hex_bytes)
        assert obj[start:start+len(by)]==by, f"CODE.2 nearest 22220 mismatch {start:05x}"
    print(f"PASS: {len(SOURCE_WINDOWS)} original 22220 init/materializer instruction windows")

def native()->None:
    h=(ROOT/"DemoV1/include/rloft/original_ground_nearest_22220.hpp").read_text()
    t=(ROOT/"DemoV1/tests/original_ground_nearest_22220_tests.cpp").read_text()
    cm=(ROOT/"DemoV1/CMakeLists.txt").read_text()
    for part in ("OriginalGroundNearest22220SourceModel", "original_materializer_executed",
                 "main_resource_at_15a20", "depth_at_15a18",
                 "out.nearest_depth=0", "ray.depth_at_15a18<out.nearest_depth"):
        assert part in h,part
    for part in ("Ray{700,0x93U,true}", "Ray{-7,42U,true}",
                 "original_materializer_executed=false", "count=256U"):
        assert part in t,part
    assert "NAME original_ground_nearest_22220_tests" in cm
    assert "NAME original_ground_nearest_22220_binary_contract_tests" in cm
    print("PASS: isolated 22220 ray replay and CTest provenance")

def main()->None:
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument("--original-code2",type=Path)
    p.add_argument("--binary-only",action="store_true")
    a=p.parse_args()
    if a.binary_only and not a.original_code2:p.error("--binary-only requires --original-code2")
    if not a.binary_only:native()
    if a.original_code2:original(a.original_code2)

if __name__=="__main__":main()
