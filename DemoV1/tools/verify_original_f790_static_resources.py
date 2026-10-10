"""Verify shipped CODE.2 F790 static DS, serialized containers and T text.

Original binaries are optional in copyright-free CI. JSON disassembly files
are candidate-unverified cross-references, not a substitute for original
instruction bytes, page maps, object tables or resource contents.
"""
from __future__ import annotations
import argparse
import hashlib
import json
import struct
from pathlib import Path

C2SHA="853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314"
C1SHA="fd9b709ceee190e6a70870f9012ebea3bc437cae6bc74e0150d6f922fa750fed"
OSHA="f23f0e7c1856904aa0156b0bf174cab794a41a327352b6ca00c3591ae5f2eed0"
T_GIT_BLOB="68cde745e55de1def1b6ce828f1455d38c6f483b"
BASE=0x75F24
LAST=0x1273D
LITERALS={0x610:b" +",0x614:b"; ",0x618:b" ",0x61C:b"-",
          0x620:b"/",0x624:b".5",0x598:b"; CHARGES: "}
EXPECTED_T={0x3d:"WEIGHT:",0x3e:"DAMAGE:",0x3f:"D",
            0x40:"A.C. BONUS:",0x41:"CAPACITY:"}

def u32(data:bytes,offset:int)->int:
    return struct.unpack_from("<I",data,offset)[0]

def check_code2(path:Path)->bytes:
    code2=path.read_bytes()
    assert len(code2)==558689 and hashlib.sha256(code2).hexdigest()==C2SHA
    assert code2[0x27BDC:0x27BDE]==b"LE"
    assert struct.unpack_from("<6I",code2,0x27BDC+0xC4)[3:5]==(1,59)
    assert struct.unpack_from("<6I",code2,0x27BDC+0xDC)[3:5]==(60,19)
    assert BASE==0x3AF24+59*0x1000
    # Original F5F8 explicitly tests subtype 0x14 then reads
    # object ID 0x50's +0x30 byte at 0x1480 (not its slot array).
    instruction_windows={
        0xF5FE:"837c2414147452",
        0xF614:"0fbf0683f8ff742a6bc0418b15d8550200",
        0xF637:"0fbe403150e8b7ffffff83c40401c3",
        0xF657:"a1d8550200660fb698801400000fb7c3",
    }
    for offset,encoded in instruction_windows.items():
        expected=bytes.fromhex(encoded)
        actual=code2[0x3AF24+offset:0x3AF24+offset+len(expected)]
        assert actual==expected,f"original F5F8 instruction 0x{offset:05X}"
    data=code2[BASE:]
    assert len(data)==LAST
    for off,literal in LITERALS.items():
        assert data[off:off+len(literal)+1]==literal+b"\0",hex(off)
    assert u32(data,0x6d98)==0x598
    assert data[0x67ec:0x67ec+17]==b"\x0c"*15+b"\xfd\xfe"
    for idx in (0,1,2,3,20,59,60,61):
        p=0x7330+10*idx
        count=data[p]
        loc=u32(data,p+5)
        assert 0<=count<=8 and (not count or loc+2*count<=LAST)
    assert data[0x7332]==8 and data[0x7332+20*10]==255
    print("PASS: CODE.2 LE object 2, seven DS strings, signed variant and container descriptors")
    return data

def check_objects(path:Path,data:bytes)->None:
    raw=path.read_bytes()
    assert hashlib.sha256(raw).hexdigest()==OSHA and len(raw)==21844
    assert u32(raw,0)==336
    records=[raw[4+i*0x41:4+(i+1)*0x41] for i in range(336)]
    def nested(subtype:int,seen:tuple[int,...]=())->int:
        if subtype==20:return records[80][0x30]
        assert 0<=subtype<63 and subtype not in seen
        off=0x7330+subtype*10
        count=data[off]
        pos=u32(data,off+5)
        total=0
        for slot in range(count):
            oid=struct.unpack_from("<h",data,pos+2*slot)[0]
            if oid==-1:continue
            assert 0<=oid<336
            record=records[oid]
            total+=record[0x30]
            if record[0x2f]==1:
                total+=nested(struct.unpack("<b",record[0x31:0x32])[0],
                              seen+(subtype,))
        return total&0xffffffff
    assert {i:nested(i) for i in (0,1,2,9,20)}=={
        0:0,1:5,2:8,9:4,20:0}
    print("PASS: OBJECTS.DAT all 336 records, initial F5F8 nested weights")

def check_t(path:Path)->None:
    raw=path.read_bytes()
    sha1=hashlib.sha1(f"blob {len(raw)}\0".encode()+raw).hexdigest()
    assert sha1==T_GIT_BLOB, "wrong original T Git blob"
    assert len(raw)==133584 and u32(raw,0)==1073
    for ix,label in EXPECTED_T.items():
        off=u32(raw,4+ix*4)
        length=struct.unpack_from("<H",raw,off)[0]
        text=bytes(b^0xDC for b in raw[off+2:off+2+length])
        assert text.decode("ascii")==label
    print("PASS: original T blob, 1073 slots, F790 records 3D/3E/3F/40/41")

def check_json(path:Path,sha:str,module:str,required:dict[int,str])->None:
    report=json.loads(path.read_text(encoding="utf-8"))
    assert report["analysisStatus"]=="candidate-unverified"
    assert report["provenance"]["Module"]==module
    assert report["provenance"]["SourceSha256"]==sha
    assert report["source"]["instructionBytesIncluded"] is False
    indexed={i["Address"]["Offset"]:i for i in report["instructions"]
             if i["Address"]["Object"]==1}
    for address,part in required.items():
        assert part in indexed[address]["Assembly"],hex(address)
    print(f"PASS: {module} candidate-only JSON provenance and {len(required)} cross-references")

def main()->None:
    ap=argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--code2",type=Path)
    ap.add_argument("--objects",type=Path)
    ap.add_argument("--t",type=Path)
    ap.add_argument("--code2-json",type=Path)
    ap.add_argument("--code1-json",type=Path)
    args=ap.parse_args()
    if args.code2:
        data=check_code2(args.code2)
        if args.objects:check_objects(args.objects,data)
    elif args.objects:ap.error("--objects requires --code2")
    if args.t:check_t(args.t)
    if args.code2_json:
        check_json(args.code2_json,C2SHA,"CODE.2",{
            0xF5F8:"push ebx",0xF603:"je short",0xF60C:"7335h",
            0xF637:"31h",0xF790:"push ebx",0xF825:"mov esi, 610h",
            0xFB84:"call 0000F5F8h",0xFC29:"7332h",
            0xFD1F:"67ECh",0x1C3F8:"0DCh"})
    if args.code1_json:
        check_json(args.code1_json,C1SHA,"CODE.1",{0x10:"push ebp"})
    if not any([args.code2,args.t,args.code2_json,args.code1_json]):
        root=Path(__file__).resolve().parents[2]
        static=(root/"DemoV1/include/rloft/original_f790_static_resources.hpp").read_text()
        decoded=(root/"DemoV1/include/rloft/original_t_text_archive.hpp").read_text()
        cmake=(root/"DemoV1/CMakeLists.txt").read_text()
        assert "0x75F24" in static and "0x7330" in static
        assert "0xDCU" in decoded
        assert "original_f790_static_resource_contract_tests" in cmake
        inventory=(root/"DemoV1/src/inventory_runtime.cpp").read_text()
        assert "if(index==0x14U)" in inventory
        assert "return objects.at(0x50U).weight_half_units;" in inventory
        assert "original_f5f8_special_weight_tests" in cmake
        print("PASS: copyright-free F790 resources and original F5F8 native short-circuit")
if __name__=="__main__":main()
