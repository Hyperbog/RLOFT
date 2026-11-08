"""Contract for source provenance-labelled native 22220/FF50 F12 projections.

Optional original CODE.2 and SINTAB byte checks independently pin the
32F4C source sentinel writer, 335C8 nearest initialization and source
SINTAB C738. The projection is a read-only diagnostic, NOT DOS RAM.
"""
from __future__ import annotations

import argparse
import hashlib
import struct
from pathlib import Path

ROOT=Path(__file__).resolve().parents[2]
CODE2_SHA="853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314"
SINTAB_SHA="19b6bc15f1be616bb8ff49dc9029995a1a0413fae06addc9fd41ddef761d0e87"
CODE_BASE=0x3AF24
ORIGINAL_WINDOWS={
    0x335C8:"c7052022020000080000",
    0x32F4C:"506b05fcc600001c05185a0100c70000000000",
    0x32F5F:"c7400493000000c7400893000000",
    0x32F6D:"c7400c00000000c7401000000000",
    0x32F7B:"c7401400000000c740180000000058c3",
    0x330E7:"81be205a010093000000750a",
}
def check_original(code2:Path,sintab:Path)->None:
    b=code2.read_bytes()
    if len(b)!=558689 or hashlib.sha256(b).hexdigest()!=CODE2_SHA:
        raise AssertionError("not original immutable Strahd CODE.2")
    obj=b[CODE_BASE:]
    for at,hexbytes in ORIGINAL_WINDOWS.items():
        expected=bytes.fromhex(hexbytes)
        if obj[at:at+len(expected)]!=expected:
            raise AssertionError(f"CODE.2 ray source mismatch 0x{at:05X}")
    b=sintab.read_bytes()
    if len(b)!=333757 or hashlib.sha256(b).hexdigest()!=SINTAB_SHA:
        raise AssertionError("not original immutable SINTAB")
    if struct.unpack_from("<I",b,0x1B38)[0]!=127:
        raise AssertionError("original C738 scale count mismatch")
    print(f"PASS: {len(ORIGINAL_WINDOWS)} original 22220/32F4C windows, C738=127")

def check_native()->None:
    files={
        "projection":(ROOT/"DemoV1/include/rloft/original_ground_source_projection.hpp").read_text(),
        "tests":(ROOT/"DemoV1/tests/original_ground_source_projection_tests.cpp").read_text(),
        "capture":(ROOT/"DemoV1/src/exploration_frame_capture.cpp").read_text(),
        "runtime":(ROOT/"DemoV1/src/exploration_runtime.cpp").read_text(),
        "frame":(ROOT/"DemoV1/include/rloft/exploration_runtime.hpp").read_text(),
        "presenter":(ROOT/"DemoV1/src/exploration_presenter.cpp").read_text(),
        "cmake":(ROOT/"DemoV1/CMakeLists.txt").read_text(),
    }
    for phrase in (
        "OriginalGroundNearest22220SourceModel::replay(",
        "OriginalGroundFF50SourceModel::from_22220(",
        "record.original_materializer_executed",
        "record.original_sentinel_initializer_executed",
        "result.unknown_origins",
        "invalid_source_sentinel_fields",
        "record.resource_outer!=0x93U",
        "record.texture_column_offset!=0U",
    ):
        assert phrase in files["projection"],phrase
    for phrase in (
        "kOriginalC6D8=256U",
        "kOriginalC738=127U",
        "source_sentinel_writers==1U",
        "nearest_depth==408",
        "lower_ff50==1",
        "nearest_depth==-7",
        "unknown_origins==1U",
        "rays[3U].resource_outer=17U",
        "before_main==42U && before_depth==3000U",
    ):
        assert phrase in files["tests"],phrase
    for phrase in (
        "ground_projection_original_c6d8",
        "ground_projection_original_c738",
        "ground_projection_available",
        "ground_projection_22220",
        "ground_projection_zero_resets",
        "ground_projection_sentinel_fills",
        "ground_projection_upper_ff50",
        "ground_projection_lower_ff50",
        "ground_projection_lower_ff50_if_upper_unchanged",
        '"unverified"',
    ):
        assert phrase in files["capture"],phrase
    for phrase in (
        "out.original_c6d8_ray_count=renderer_tables_.c6d8()",
        "out.original_c738_scale_count=renderer_tables_.scale_block_count",
    ):
        assert phrase in files["runtime"],phrase
    for phrase in ("original_c6d8_ray_count","original_c738_scale_count"):
        assert phrase in files["frame"],phrase
    assert "OriginalGroundSourceProjectionModel" not in files["presenter"]
    for phrase in (
        "NAME original_ground_source_projection_tests",
        "NAME original_ground_source_projection_binary_contract_tests",
    ):
        assert phrase in files["cmake"],phrase
    print("PASS: read-only 22220/FF50 projection source and metadata wiring")

if __name__=="__main__":
    cli=argparse.ArgumentParser(description=__doc__)
    cli.add_argument("--original-code2",type=Path)
    cli.add_argument("--original-sintab",type=Path)
    a=cli.parse_args()
    if (a.original_code2 is None)!=(a.original_sintab is None):
        cli.error("both original CODE.2 and SINTAB must be supplied")
    check_native()
    if a.original_code2:
        check_original(a.original_code2,a.original_sintab)
