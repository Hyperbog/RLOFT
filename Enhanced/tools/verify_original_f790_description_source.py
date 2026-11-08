"""Offline verifier for CODE.2 F790 item-description arithmetic and resources.

Default CI run guards the standalone native source contract. Supply original
CODE.2 and OBJECTS.DAT to perform byte-for-byte source and fixture checks.
"""
from __future__ import annotations
import argparse
import hashlib
import struct
from pathlib import Path

ROOT=Path(__file__).resolve().parents[2]
CODE2_SHA="853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314"
OBJECTS_SHA="f23f0e7c1856904aa0156b0bf174cab794a41a327352b6ca00c3591ae5f2eed0"
BASE=0x3AF24
WINDOWS={
0xF7E4:"89dac1e2068b35d855020001dabf9c75000001d6578a0688073c0074108a4601",
0xF812:"a1d8550200807c0237007465837c242800745ebe10060000572bc949b0",
0xF883:"be14060000bf9c750000572bc949b000f2ae4f8a0688073c0074108a46",
0xF8AF:"89dfc1e706a1d855020001df8a44072f2c023c090f87740200000fb6c02eff248568f700",
0xF8D4:"89dfc1e70601df897c24088b3dd8550200037c24080fb66f3a0fb647388d1428895424040fb657390fafc283",
0xF9D4:"a1d8550200034424088d783b0fb66f020fb6078d1428895424040fb657010fafc2be2006",
0xFA01:"572bc949b000f2ae4f8a0688073c0074108a460183c60288470183c7023c0075e85fff742404",
0xFA85:"6a40bf9c75000029c94931c0f2aef7d14981c19c75000051e8c6c8000083c408be180600",
0xFADD:"0fbe440734f7d850e806ec010083c404bf9c75000089c657",
0xFB3D:"6a3dbf9c75000029c94931c0f2aef7d14981c19c75000089df51c1e706e809c800008d043b8b3dd855",
0xFB6D:"660fb647308944240c0fb6472f83f80175110fbe473150e86ffaffff83c4040144240c0fb754240c89d0c1",
0xFBD0:"8b44240c30e424010fb7c085c07427be24060000572bc949b000",
0xFC15:"0fb6472f83f8010f85a90000000fbe47316bc00a0fb6803273000083f87f0f841f010000be140600",
0xFCCB:"807f3100750983fb4a0f857e000000837c2428007477bf9c7500008b35986d0000",
0xFD0E:"89dfc1e706a1d855020001df0fbe4407310fb680ec67000050e8c4e9010083c4",
0x2E6F0:"6a0a684eb60200ff74240ce8bf8c0000b84eb6020083c40cc3",
0x2E70C:"6a0a684eb60200ff74240ce8678d0000b84eb6020083c40cc3",
}
def check_code2(path:Path)->None:
    raw=path.read_bytes()
    assert len(raw)==558689 and hashlib.sha256(raw).hexdigest()==CODE2_SHA, "not original CODE.2"
    obj=raw[BASE:BASE+0x3A852]
    for at,hexcode in WINDOWS.items():
        expected=bytes.fromhex(hexcode)
        assert obj[at:at+len(expected)]==expected,f"original F790 window {at:05X}"
    assert struct.unpack_from("<10I",obj,0xF768)==(
        (0xFA85,)*3+(0xFB3D,)*4+(0xF8D4,)*3)
    print("PASS: 17 original CODE.2 description instruction windows + ten-entry CS table")
def check_objects(path:Path)->None:
    raw=path.read_bytes()
    assert hashlib.sha256(raw).hexdigest()==OBJECTS_SHA
    assert len(raw)==21844 and struct.unpack_from("<I",raw)[0]==336
    records=[raw[4+i*0x41:4+(i+1)*0x41] for i in range(336)]
    assert all(len(r)==0x41 and 0 in r[:40] for r in records)
    categories={r[0x2f] for r in records}
    assert all(any(lo<=cat<=hi for cat in categories)
               for lo,hi in ((1,1),(2,4),(5,8),(9,11),(12,255)))
    print("PASS: all 336 original OBJECTS.DAT records and description branch classes")
def check_source()->None:
    h=(ROOT/"DemoV1/include/rloft/original_f790_item_description_source.hpp").read_text(encoding="utf-8")
    test=(ROOT/"DemoV1/tests/original_f790_item_description_source_tests.cpp").read_text(encoding="utf-8")
    cmake=(ROOT/"DemoV1/CMakeLists.txt").read_text(encoding="utf-8")
    native=(ROOT/"DemoV1/src/inventory_item_description.cpp").read_text(encoding="utf-8")
    host=(ROOT/"DemoV1/src/win32_main.cpp").read_text(encoding="utf-8")
    for fragment in ("record[0x37]","record[0x34]","record[0x38]",
                     "record[0x39]","record[0x3a]","record[0x3b]",
                     "record[0x3c]","record[0x3d]","record[0x2f]",
                     "record[0x30]","record[0x31]","nested_half_weight(subtype)",
                     "slot_7332(subtype)","slot_67ec(subtype)",
                     "static_cast<std::uint16_t>(halves)","t(0x3dU)",
                     "t(0x3eU)","t(0x40U)","t(0x41U)","ds(0x6d98U)"):
        assert fragment in h,fragment
    assert "CODE.2 F7E4..FD57" in h
    assert "Source::compose" in test
    assert "original_f790_item_description_source_tests" in cmake
    assert "original_f790_description_source_contract_tests" in cmake
    assert 'return name+": WEIGHT:"+weight_text(item.weight_half_units);' in native
    assert "OriginalF790ItemDescriptionSource" not in host, "premature live item text integration"
    print("PASS: original F790 byte fields/resolvers modeled, incomplete live text gated")
def main()->None:
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument("--original-code2",type=Path)
    p.add_argument("--original-objects",type=Path)
    p.add_argument("--binary-only",action="store_true")
    args=p.parse_args()
    if not args.binary_only:check_source()
    if args.original_code2:check_code2(args.original_code2)
    if args.original_objects:check_objects(args.original_objects)
    if args.binary_only and not(args.original_code2 or args.original_objects):
        p.error("binary-only needs an original data file")
if __name__=="__main__":main()
