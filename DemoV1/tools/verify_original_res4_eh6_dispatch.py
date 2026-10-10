"""Fail-closed original CODE.2 EH6 runtime dispatch contract.

Source-only mode proves the native path explicitly carries EH6 to the
same runtime type1 loader used by original CODE.2 2F29C. Optional
--original-code2 requires the immutable original binary checksum and
exact byte instruction windows/near call targets, independent of C++.
No copyrighted game resource content or byte buffers are checked in.
"""
from __future__ import annotations

import argparse
import hashlib
import struct
from pathlib import Path

ROOT=Path(__file__).resolve().parents[2]
CODE2_SHA="853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314"
OBJECT1_BASE=0x3AF24

WINDOWS={
    0x1F388:"0fbf051f5602006a040305f858020050e8fffe0000",
    0x2F2CE:"0fb6500283fa0175058b4005",
    0x2EDD9:"ffa072b60200fec883c4103c060f8723010000",
    0x2EEED: "ff34036870b60200", # type-7 loader's matching callsite, separately inspected
    0x2EEC5:"a1b8f00000ff34036870b60200e8b9860000",
    0x2EF38:"c644030201",
    0x37590:"5383ec04ff7424106a028d5c240853e8489dffff",
    0x375A7:"0fb71c2483c30253e8989affff",
    0x375C8:"83c404e89c83ffff6a00",
    0x37700:"c806000060fcbb00000000bf10060100b0feb9ee0f0000f3aa",
}
CALLS={
    0x1F398:0x2F29C,
    0x2EED2:0x37590, # source type-6 loader switch
    0x3759F:0x312EC,
    0x375AF:0x3104C,
    0x375C3:0x2F960,
    0x375CB:0x2F96C,
    0x375E0:0x2FB04,
}


def check_original(path:Path)->None:
    data=path.read_bytes()
    if len(data)!=558689 or hashlib.sha256(data).hexdigest()!=CODE2_SHA:
        raise AssertionError("not immutable Strahd original CODE.2")
    binary=data[OBJECT1_BASE:]
    for offset,hex_bytes in WINDOWS.items():
        actual=binary[offset:offset+len(bytes.fromhex(hex_bytes))]
        if actual!=bytes.fromhex(hex_bytes):
            raise AssertionError(f"original CODE.2 source window 0x{offset:05x}")
    for at,target in CALLS.items():
        if binary[at]!=0xE8 or (
            at+5+struct.unpack_from("<i",binary,at+1)[0]!=target):
            raise AssertionError(f"CODE.2 E8 rel32 target 0x{at:05x}")
    # Switch table: original EH6 branch is index (type-1)=5 and
    # enters 2EEC5, which calls 37590. Runtime type becomes 1.
    assert struct.unpack_from("<I",binary,0x2EC98+5*4)[0]==0x2EEC5
    print(f"PASS: {len(WINDOWS)} immutable EH6 source windows, "
          f"{len(CALLS)} rel32 targets and original type-6 dispatch slot")


def check_native()->None:
    archive_h=(ROOT/"DemoV1/include/rloft/resource_assets.hpp").read_text()
    archive=(ROOT/"DemoV1/src/resource_assets.cpp").read_text()
    res4=(ROOT/"DemoV1/src/original_ground_res4_audit.cpp").read_text()
    tests=(ROOT/"DemoV1/tests/original_ground_res4_audit_tests.cpp").read_text()
    oracle=(ROOT/"DemoV1/tools/probe_original_res4_selection.py").read_text()
    cmake=(ROOT/"DemoV1/CMakeLists.txt").read_text()
    for fragment in (
        "decode_type6_block(int index) const",
        "Original CODE.2 2ECB4",
    ):
        assert fragment in archive_h,fragment
    for fragment in (
        "ResArchive::decode_type6_block(",
        "raw[2]!=6U",
        "const auto extended=s16(raw.data()+3)",
        "const auto expected=static_cast<std::size_t>(u16(raw.data()+at))",
        "lzss_4k(",
        "result.push_back(raw[at+1U])",
    ):
        assert fragment in archive,fragment
    for fragment in (
        "raw[2]==6U","decode_type6_block(i)",
        "decode_type6_block(original_absolute_index)",
        "fnv1a(pixels)","4098U",
    ):
        assert fragment in res4,fragment
    for fragment in (
        "eh6_fixture()", "source_eh6[1]==0x10U",
        "eh6_report.type6_entries==4",
        "probe_eh6.verified",
    ):
        assert fragment in tests,fragment
    for fragment in (
        "original_37700_lzss", "history[:0xFEE]",
        "selected_pixels_sha256","selected_decode_verified",
    ):
        assert fragment in oracle,fragment
    assert "NAME original_res4_eh6_runtime_binary_contract_tests" in cmake
    print("PASS: native EH6 decoder, source ground and independent LZSS oracle")


if __name__=="__main__":
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument("--original-code2",type=Path)
    p.add_argument("--binary-only",action="store_true")
    args=p.parse_args()
    if args.binary_only and not args.original_code2:
        p.error("--binary-only requires original CODE.2")
    if not args.binary_only:
        check_native()
    if args.original_code2:
        check_original(args.original_code2)
