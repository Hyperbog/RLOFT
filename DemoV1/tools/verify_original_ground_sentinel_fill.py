"""Source-verify CODE.2 32F4C complete sentinel ray writer and two caller E8s.

Source-only CTest with optional immutable original binary input. Deliberately
distinguishes source-owned sentinel initialization from four materializer
tails that mutate nearest-wall 22220.
"""
from __future__ import annotations
import argparse,hashlib,struct
from pathlib import Path

ROOT=Path(__file__).resolve().parents[2]
ORIGINAL_SHA="853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314"
BASE=0x3AF24
# Complete routine: PUSH EAX, 28-byte stride, seven stores, POP, RET.
ROUTINE=bytes.fromhex(
    "506b05fcc600001c05185a0100"
    "c70000000000c7400493000000c7400893000000"
    "c7400c00000000c7401000000000c7401400000000"
    "c740180000000058c3")
CALLERS=(0x32C2F,0x32D19)

def check_binary(path:Path)->None:
    b=path.read_bytes()
    assert len(b)==558689 and hashlib.sha256(b).hexdigest()==ORIGINAL_SHA,"wrong original CODE.2"
    code=b[BASE:]
    assert code[0x32F4C:0x32F4C+len(ROUTINE)]==ROUTINE
    for address in CALLERS:
        assert code[address]==0xE8
        target=address+5+struct.unpack_from("<i",code,address+1)[0]
        assert target==0x32F4C,(address,target)
    # Original 32F4C zeroes slot+0 depth and +0C..+18,
    # writes resource 0x93 to +4/+8, and has NO global [22220] write.
    assert b"\x20\x22\x02\x00" not in ROUTINE
    print("PASS: original 32F4C 63-byte sentinel writer and two caller targets")

def check_native()->None:
    header=(ROOT/"DemoV1/include/rloft/view_sweep_record.hpp").read_text()
    traversal=(ROOT/"DemoV1/src/view_sweep_traversal.cpp").read_text()
    cap=(ROOT/"DemoV1/src/exploration_frame_capture.cpp").read_text()
    tests=(ROOT/"DemoV1/tests/view_sweep_traversal_tests.cpp").read_text()
    cm=(ROOT/"DemoV1/CMakeLists.txt").read_text()
    replay=(ROOT/"DemoV1/include/rloft/original_ground_nearest_22220.hpp").read_text()
    assert "bool original_sentinel_initializer_executed{}" in header
    assert "out.original_sentinel_initializer_executed=true" in traversal
    assert "out.original_materializer_executed=false" in traversal
    assert "ground_sweep_source_sentinel_initializer" in cap
    assert "hidden_records[0].original_sentinel_initializer_executed" in tests
    assert "if(!ray.original_materializer_executed)" in replay
    assert "NAME original_ground_sentinel_fill_binary_contract_tests" in cm
    print("PASS: separate original 32F4C and 22220 materializer provenance")

def main()->None:
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument("--original-code2",type=Path)
    p.add_argument("--binary-only",action="store_true")
    a=p.parse_args()
    if a.binary_only and a.original_code2 is None:
        p.error("--binary-only requires --original-code2")
    if not a.binary_only: check_native()
    if a.original_code2 is not None: check_binary(a.original_code2)
if __name__=="__main__":main()
