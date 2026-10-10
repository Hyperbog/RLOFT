"""Source-bounded CODE.2 upper callback callgraph and FF50 literal footprint.

Validates original code, but DOES NOT establish transitive pointer alias
safety, callback side effects, or original DOS runtime parity.
"""
from __future__ import annotations
import argparse,hashlib,struct
from pathlib import Path

ROOT=Path(__file__).resolve().parents[2]
ORIGINAL_SHA="853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314"
BASE=0x3AF24
BOUNDS={
    "29B40 rectangle overlay":(0x29B40,0x29C9E,0),
    "314B0 animated overlay":(0x314B0,0x318B1,0),
    "33E28 default upper fill":(0x33E28,0x33E6B,0),
    "34A74 source raster":(0x34A74,0x34BAD,2),
    "2AB38 destination helper":(0x2AB38,0x2AB6C,0),
}
CALLS={
    0x29B68:0x2AB38,
    0x314EF:0x296C0,0x314FD:0x30CC4,
    0x31532:0x30CA2,0x31553:0x29838,
    0x3158D:0x30CA2,0x315B3:0x29838,
    0x315EA:0x30CA2,0x31610:0x29838,
    0x3161A:0x30700,0x31620:0x30CC4,
    0x31766:0x2986C,0x3180B:0x2986C,
    0x31889:0x2986C,0x318A4:0x296C0,
    0x34B1C:0x342E4,0x34B84:0x34954,
}

def audit_original(path:Path)->None:
    b=path.read_bytes()
    assert len(b)==558689 and hashlib.sha256(b).hexdigest()==ORIGINAL_SHA,"wrong CODE.2"
    obj=b[BASE:0x75F24]
    ff50=bytes.fromhex("50ff0000")
    for name,(start,end,refs) in BOUNDS.items():
        body=obj[start:end]
        assert len(body)==end-start,(name,"truncated")
        assert body.count(ff50)==refs,(name,"direct FF50 footprint")
        assert obj[end-1]==0xC3,(name,"RET")
    for call,dest in CALLS.items():
        assert obj[call]==0xE8,hex(call)
        actual=call+5+struct.unpack_from("<i",obj,call+1)[0]
        assert actual==dest,(hex(call),hex(actual),hex(dest))
    assert obj[0x3161A:0x3161F]==bytes.fromhex("e8e1f0ffff")
    assert obj[0x1F377:0x1F37D]==bytes.fromhex("031d50ff0000")
    for at,target in ((0x34A7E,0x34AF9),(0x34A83,0x34B69),
                      (0x34A8D,0x34AF6),(0x34A92,0x34B66)):
        assert obj[at]==0xA2
        assert struct.unpack_from("<I",obj,at+1)[0]==target
    # Animated overlay 314B0 seeds an actual shared original LCG.
    # 30700 is a READ of F58C (the interrupt entry 30708 is
    # responsible for incrementing it); 30C9C returns fixed F6F4.
    # 30CA2 and 30CC4 indirect [EAX] writes therefore have a
    # statically resolved F6F4 destination, NOT a FF50 alias.
    windows={
        0x30700:"a18cf50000c3",
        0x3071C:"ff058cf50000",
        0x30C9C:"b8f4f60000c3",
        0x30CA2:"e8f5ffffff",
        0x30CAB:"69106d4ec641",
        0x30CB1:"81c239300000",
        0x30CB7:"8910",
        0x30CC4:"e8d3ffffff",
        0x30CCD:"8b5424048910",
        0x314F7:"ff3558c60000",
        0x3161A:"e8e1f0ffff",
        0x31620:"e89ff6ffff",
    }
    for at,sig in windows.items():
        want=bytes.fromhex(sig)
        assert obj[at:at+len(want)]==want,hex(at)
    # Reaffirm reseed/advance order using actual E8 destinations.
    for call,dest in ((0x314FD,0x30CC4),
                      (0x31532,0x30CA2),
                      (0x3158D,0x30CA2),
                      (0x315EA,0x30CA2),
                      (0x3161A,0x30700),
                      (0x31620,0x30CC4)):
        assert obj[call]==0xE8
        assert call+5+struct.unpack_from("<i",obj,call+1)[0]==dest
    # CODE.2 296C0 is the 314B0 geometry helper. Its fully
    # disassembled 117 bytes write ONLY these four EFxx setup
    # globals (plus a caller-owned stack-slot clamp), not FF50.
    # 2AB38 is the 29B40 geometry helper: reads EFED/EFF1/F001
    # to calculate EBX destination and ECX plane, with NO memory
    # write in its 52-byte body. The *caller* may still write to
    # an aliased framebuffer: this audit does NOT close ES aliases.
    leafs=(
        (0x296C0,0x29735,
         "dc43d298f510365164f0b68c82bb954b83d8be8882a4c6fb0b659aaeffb0b9f7",0),
        (0x2AB38,0x2AB6C,
         "51b0f554019e0dcca8236ff9894358b7cea837c0f21defb69f5eeef067e623dd",0),
    )
    for begin,end,expected_sha,direct_ff50_count in leafs:
        body=obj[begin:end]
        assert hashlib.sha256(body).hexdigest()==expected_sha,hex(begin)
        assert body.count(ff50)==direct_ff50_count,hex(begin)
        assert body[-1]==0xC3,hex(end)
    stores={
        0x29715:"890d59ef0000",
        0x2971E:"891d5def0000",
        0x29728:"891561ef0000",
        0x2972E:"a355ef0000",
    }
    for address,hexbytes in stores.items():
        wanted=bytes.fromhex(hexbytes)
        assert obj[address:address+len(wanted)]==wanted,hex(address)
    address_reads={
        0x2AB43:"8b0dedef0000",
        0x2AB4D:"8b0df1ef0000",
        0x2AB63:"031d01f00000",
    }
    for address,hexbytes in address_reads.items():
        wanted=bytes.fromhex(hexbytes)
        assert obj[address:address+len(wanted)]==wanted,hex(address)
    # Original graphics callback slot F0C8 is mutable DGROUP
    # data, shipped as NULL. 2E74C stores a caller-provided pointer;
    # 2E758 invokes it when nonnull. 2F868 performs the same check
    # in the frame tail. The callback target's FF50 write footprint
    # CANNOT be inferred from the initial null value.
    f0c8=bytes.fromhex("c8f00000")
    locations=[]
    scan_at=0
    while True:
        scan_at=obj.find(f0c8,scan_at)
        if scan_at<0:break
        locations.append(scan_at)
        scan_at+=1
    assert locations==[0x2E751,0x2E75A,0x2E763,0x2F86F,0x2F878]
    assert b[0x75F24+0xF0C8:0x75F24+0xF0CC]==bytes(4)
    windows={
        0x2E74C:"8b442404a3c8f00000c3",  # user pointer setter
        0x2E758:"833dc8f00000007406ff15c8f00000c3",
        0x2F86D:"833dc8f00000007406ff15c8f00000",
        0x29867:"e9ec4e0000",  # 29838 wrapper jumps to 2E758
        0x29573:"e8e0510000",  # 29564 enters 2E758
    }
    for address,expected_hex in windows.items():
        expected=bytes.fromhex(expected_hex)
        assert obj[address:address+len(expected)]==expected,hex(address)
    print("PASS: F0C8 default NULL, setter, two indirect callsites (5 source references)")
    print("OPEN: runtime-installed pointer and callback FF50/VGA effects")
    print("PASS: complete 296C0 geometry and 2AB38 destination helper bodies")
    print("BOUND: 296C0 direct globals EF55/EF59/EF5D/EF61; 2AB38 reads F001")
    print("PASS: overlay facing/RNG/IRQ reseed and fixed F6F4 alias (12 windows)")
    print("PASS: 17 original callback E8 edges; 5 function boundaries")
    print("PASS: direct FF50 occurrences 0/0/0/2/0; 1F377 reload")
    print("UNRESOLVED: indirect aliases, transitive helpers and IRQ effects")

def audit_repository()->None:
    cm=(ROOT/"DemoV1/CMakeLists.txt").read_text()
    doc=(ROOT/"DemoV1/LEVEL22_FF50_UPPER_CALLBACK_BOUNDARY_AUDIT_20261010.md").read_text()
    assert "NAME original_ground_upper_callback_boundary_tests" in cm
    for term in ("29B40","314B0","33E28","34A74","30700","1F377","indirect"):
        assert term in doc,term
    native_rng=(ROOT/"DemoV1/src/original_random.cpp").read_text()
    assert "0x41C64E6DU" in native_rng
    assert "0x00003039U" in native_rng
    assert "state_ *" in native_rng
    for term in ("296C0","2AB38","EF55","F001","indirect"):
        assert term in doc,term
    for term in ("F0C8","2E74C","2E758","2F868","indirect"):
        assert term in doc,term
    print("PASS: source callback evidence and CTest registration")

if __name__=="__main__":
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument("--original-code2",type=Path)
    p.add_argument("--binary-only",action="store_true")
    args=p.parse_args()
    if args.binary_only and args.original_code2 is None:
        p.error("--binary-only requires --original-code2")
    if not args.binary_only:audit_repository()
    if args.original_code2 is not None:audit_original(args.original_code2)
