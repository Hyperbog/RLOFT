"""Source-authoritative original CODE.2 34A74 planar micro-raster proof.
This is a source-only test unless --original-code2 is supplied.
Does not prove ES segmentation or matched original DOS framebuffer.
"""
from __future__ import annotations
import argparse
import hashlib
import struct
from pathlib import Path

ROOT=Path(__file__).resolve().parents[2]
SHA="853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314"
BASE=0x3AF24
SAMPLER=bytes.fromhex("33f603d303c50fa4d6060fa4c606033554ff0000ac2ac4aa")
CALLS=(0x34302,0x34319,0x34330,0x34972,0x34989)
FIRST_MASKS=(0x0802,0x0402,0x0202,0x0102)
SECOND_MASKS=(0x0C02,0x0302)

def audit_original(path:Path)->None:
    b=path.read_bytes()
    assert len(b)==558689 and hashlib.sha256(b).hexdigest()==SHA,"wrong original CODE.2"
    obj=b[BASE:0x75F24]
    assert len(SAMPLER)==24
    assert obj[0x34350:0x34950]==SAMPLER*64,"34350 must unroll exactly 64 times"
    assert obj[0x34950:0x34954]==bytes.fromhex("c387db90")
    first=obj[0x342E4:0x34350]
    second=obj[0x34954:0x34995]
    assert len(first)==108 and len(second)==65
    assert hashlib.sha256(first).hexdigest()=="7719fdfb3607f4ef5b0e771ac9c3ca98917139085d95969a7ae88a8b36c21f65"
    assert hashlib.sha256(second).hexdigest()=="2bb4519117e6ba14357e2f1496cfcc837775d05910c821716b85cecb0dd00470"
    for body,masks,call_count in ((first,FIRST_MASKS,3),(second,SECOND_MASKS,2)):
        words=[struct.unpack_from("<H",body,i+2)[0]
               for i in range(len(body)-3)
               if body[i:i+2]==bytes.fromhex("66b8")]
        assert tuple(words)==masks
        assert body.count(bytes.fromhex("66ef"))==len(masks)
        assert body.count(bytes.fromhex("66bac403"))==len(masks)
        assert body.count(bytes.fromhex("8b3d40ff0000"))==len(masks)
        assert body.count(bytes.fromhex("8f0558020100"))==1
        assert body.count(bytes.fromhex("ff3558020100"))==1
        assert body.count(b"\xe8")==call_count
        assert bytes.fromhex("50ff0000") not in body
    for at in CALLS:
        assert obj[at]==0xE8,hex(at)
        assert at+5+struct.unpack_from("<i",obj,at+1)[0]==0x34350,hex(at)
    # First helper uses three near calls and reaches fourth plane
    # through 3434D XCHG/3434F NOP fall-through into 34350.
    assert obj[0x34347:0x34350]==bytes.fromhex("ff355802010087db90")
    assert obj[0x3498e:0x34995]==bytes.fromhex("ff3558020100c3")
    assert obj[0x34a97:0x34aa7]==bytes.fromhex("a148ff00000305bcfd0000a340ff0000")
    assert obj[0x34aa7:0x34ab4]==bytes.fromhex("a140c0000083c002a354ff0000")
    assert obj[0x34ab9:0x34abf]==bytes.fromhex("8b0d50ff0000")
    assert obj[0x34b47:0x34b4d]==bytes.fromhex("8b0d50ff0000")
    print("PASS: original 34350 has exactly 64 identical 24-byte texel samplers")
    print("PASS: 4 single-plane masks and 2 paired masks; 5 E8s plus fourth-plane fall-through")
    print("PASS: original FF40/FF54 address setup, FF50 reads, no direct helper FF50 writes")
    print("OPEN: ES:[EDI] alias, mutable FF50 and independent DOS pixel parity")

def audit_native()->None:
    cm=(ROOT/"DemoV1/CMakeLists.txt").read_text()
    src=(ROOT/"DemoV1/include/rloft/original_ground_texture_raster.hpp").read_text()
    doc=(ROOT/"DemoV1/LEVEL22_ORIGINAL_34A74_UNROLLED_PLANAR_RASTER_AUDIT_20261010.md").read_text()
    assert "NAME original_ground_raster_unrolled_plane_tests" in cm
    for token in ("kPlaneBytesPerRow = 64U","kFirstFullRows = 80U","kMaxRows = 128U"):
        assert token in src,token
    for token in ("fall-through","34350","34A74","FF50","ES","03C4"):
        assert token in doc,token
    print("PASS: native source unrolled planar sampler metadata contract")

if __name__=="__main__":
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument("--original-code2",type=Path)
    p.add_argument("--binary-only",action="store_true")
    args=p.parse_args()
    if args.binary_only and args.original_code2 is None:
        p.error("--binary-only requires --original-code2")
    if not args.binary_only:audit_native()
    if args.original_code2 is not None:audit_original(args.original_code2)
