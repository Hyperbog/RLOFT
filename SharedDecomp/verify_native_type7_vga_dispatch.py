#!/usr/bin/env python3
"""Original CODE.2 type-7 VGA dispatch vs independently compiled native audit.

Read original source bytes only. No original x86 execution or DOS frame
parity claims. Uses the independent RES archive LZSS implementation.
"""
from __future__ import annotations
import argparse
import hashlib
import json
from pathlib import Path
import struct
import sys

sys.path.insert(0,str(Path(__file__).resolve().parents[1]/"Resources"/"RES"))
from analyze_res import lzss,parse_res

SCHEMA="rloft-native-type7-vga-dispatch/v1"
REFERENCE_SCHEMA="rloft-original-native-code2-vga-dispatch-differential/v1"
RETAIL={
 "CODE.2":"853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314",
 "SINTAB":"19b6bc15f1be616bb8ff49dc9029995a1a0413fae06addc9fd41ddef761d0e87",
 "RES3":"44955ebbed0a8ed8d874fae07b01710317044882ab3c7c95d9be54e883df9a36",
}
sha=lambda data:hashlib.sha256(data).hexdigest()
def require(ok,message):
    if not ok:raise ValueError(message)
def w(b,pos):return struct.unpack_from("<H",b,pos)[0]
def d(b,pos):return struct.unpack_from("<I",b,pos)[0]

def source(code:bytes,sintab:bytes,res3:bytes)->dict:
    original=0x75F24
    require(len(code)>original+0xEF4D,
            "original CODE.2 bound data object truncated")
    mode=code[original+0xC029]
    page=code[original+0xEF4D]
    require(mode in (0,1) and page in (0,1),
            "original CODE.2 initial VGA mode/page invalid")
    tables=[code[original+k:original+k+32]
            for k in (0xC668,0xC688,0xC6A8)]
    require(all(len(t)==32 for t in tables),
            "original CODE.2 VGA tables truncated")
    masks=[]
    for phase in range(4):
        for n in range(1,8):
            actual=[tab[phase*8+n] for tab in tables]
            expected=[sum(1<<((phase+j)&3) for j in range(n)
                          if (phase+j)//4==pass_index)
                      for pass_index in range(3)]
            require(actual==expected,
                    f"original VGA table coverage inconsistent at {phase}/{n}")
            masks.append(actual)
    require(len(sintab)>=0x1F42 and
            (d(sintab,0x1B38),d(sintab,0x1B3C),d(sintab,0x1B40))
            ==(127,508,128),"original SINTAB cardinality invalid")
    pos=0x1F42
    block=None
    for index in range(127):
        require(pos+2<=len(sintab),"original SINTAB block size EOF")
        size=w(sintab,pos)
        pos+=2
        require(size>=0x205 and pos+size<=len(sintab),
                "original SINTAB scale truncated")
        if index==126:block=sintab[pos:pos+size]
        pos+=size
    require(pos+512==len(sintab) and not any(sintab[pos:]),
            "original SINTAB trailing reserve invalid")
    patch=struct.unpack_from("<129H",block)
    machine=block[0x204:]
    _,records=parse_res(res3)
    require(records and records[0][:5]==b"EH\x07\x00\x00",
            "original RES3 EH7 entry zero missing")
    record=records[0]
    body,consumed=lzss(record[7:],w(record,5))
    require(consumed==len(record)-7,
            "original RES3 source shape zero LZSS residue")
    require(len(body)>=12,"original RES3 source shape body truncated")
    col0=w(body,10)
    require(12<=col0<len(body),"original EH7 source column zero invalid")
    desc=[]
    while True:
        require(col0+2<=len(body),"original source list not terminated")
        stop=w(body,col0)
        if not stop:break
        require(col0+6<=len(body),"original six-byte span truncated")
        adjust,entry=struct.unpack_from("<hH",body,col0+2)
        require(0<=entry<stop<=256 and entry%2==stop%2==0,
                "original EH7 source selectors invalid")
        desc.append((entry,stop,adjust))
        col0+=6

    def raster(planes,entry,stop,adjust,destination,mask,ecx):
        pc=patch[entry//2]-0x204
        last=patch[stop//2]-0x204
        require(0<=pc<=last<len(machine),"original SINTAB patch range invalid")
        eax=0; reads=stores=plane_stores=0
        while pc<last:
            pair=machine[pc:pc+2]
            if pair==b"\x8a\x86":
                at=adjust+struct.unpack_from("<i",machine,pc+2)[0]
                require(0<=at<len(body),"original type-7 source sample out of bounds")
                eax=(eax&0xFFFFFF00)|body[at]
                reads+=1;pc+=6
            elif pair==b"\x88\xc4":
                eax=(eax&0xFFFF00FF)|((eax&255)<<8);pc+=2
            elif pair==b"\x29\xc8":
                eax=(eax-ecx)&0xFFFFFFFF;pc+=2
            elif pair in (b"\x88\x87",b"\x88\xa7"):
                at=destination+d(machine,pc+2)
                require(0<=at<65536,"original VGA write exceeds plane address")
                color=(eax&255) if pair==b"\x88\x87" else ((eax>>8)&255)
                for lane in range(4):
                    if mask&(1<<lane):
                        planes[lane][at]=color
                        plane_stores+=1
                stores+=1;pc+=6
            else:
                raise ValueError("original SINTAB raster opcode not supported")
        require(pc==last,"original SINTAB patch stops inside instruction")
        return reads,stores,plane_stores

    results={}
    for key in ("grouped_mode","individual_mode"):
        planes=[bytearray(65536) for _ in range(4)]
        reads=stores=plane_writes=invocations=descriptor_invocations=0
        calls=[]
        if key=="grouped_mode":
            triplet=masks[(6%4)*7+6]
            calls=[[(6//4+pass_i,mask,0)
                    for pass_i,mask in enumerate(triplet) if pass_i==0 or mask]]
        else:
            for x in range(6,13):
                triplet=masks[(x%4)*7]
                calls.append([(x//4+pass_i,mask,0)
                              for pass_i,mask in enumerate(triplet)
                              if pass_i==0 or mask])
        for group in calls:
            for entry,stop,adjust in desc:
                descriptor_invocations+=1
                for dest,mask,ecx in group:
                    a,b,c=raster(planes,entry,stop,adjust,dest,mask,ecx)
                    reads+=a;stores+=b;plane_writes+=c;invocations+=1
        results[key]={
            "individual_columns":key=="individual_mode",
            "original_page":page,
            "plane_passes":sum(len(group) for group in calls),
            "lookup_after_dispatch":0,
            "descriptor_invocations":descriptor_invocations,
            "raster_call_invocations":invocations,
            "indexed_source_reads":reads,
            "masked_vga_writes":stores,
            "individual_plane_writes":plane_writes,
            "four_plane_sha256":sha(b"".join(planes)),
            "original_frame_parity_verified":False,
        }
    return {
        "original_code2_sha256":sha(code),
        "original_sintab_sha256":sha(sintab),
        "original_res3_sha256":sha(res3),
        "original_code2_initial_individual":bool(mode),
        "original_code2_initial_page":page,
        "original_mask_table_runs_verified":28,
        "original_vga_mask_triplets":masks,
        "sample_shape_id":0,
        "sample_source_column_index":0,
        "sample_sintab_scale_index":126,
        "sample_start_viewport_column":6,
        "sample_viewport_run_width":7,
        "sample_renderer_row":0,
        "sample_renderer_row_byte_offset":0,
        "sample_source_base_offset":0,
        "sample_ecx_lookup_diagnostic":0,
        **results,
    }

def compare(code:bytes,sintab:bytes,res3:bytes,native:dict,pin=True)->dict:
    require(isinstance(native,dict) and native.get("schema")==SCHEMA,
            "native CODE.2 dispatch schema mismatch")
    for flag in ("original_16bit_x86_executed",
                 "original_camera_projection_verified",
                 "original_screen_frame_parity_verified"):
        require(native.get(flag) is False,
                "unsupported DOS/render/gameplay attestation "+flag)
    if pin:
        for name,body in (("CODE.2",code),("SINTAB",sintab),("RES3",res3)):
            require(sha(body)==RETAIL[name],
                    "original "+name+" does not match pinned SHA256")
    expected=source(code,sintab,res3)
    differences=[]
    for key,value in expected.items():
        seen=native.get(key)
        if type(value)!=type(seen) or value!=seen:
            if key=="original_vga_mask_triplets" and isinstance(seen,list):
                for index,(before,after) in enumerate(zip(value,seen)):
                    if before!=after:
                        differences.append({"field":f"{key}/{index}",
                                            "original":before,"native":after})
                if len(value)!=len(seen):
                    differences.append({"field":key+"/length",
                                        "original":len(value),"native":len(seen)})
            elif isinstance(value,dict) and isinstance(seen,dict):
                for field,correct in value.items():
                    if type(seen.get(field))!=type(correct) or seen.get(field)!=correct:
                        differences.append({"field":key+"/"+field,
                                            "original":correct,"native":seen.get(field)})
            else:
                differences.append({"field":key,"original":value,"native":seen})
    return {
        "schema":REFERENCE_SCHEMA,
        "status":"MATCH" if not differences else "DIFFERENT",
        "original_sha256":{n:sha(b) for n,b in
            (("CODE.2",code),("SINTAB",sintab),("RES3",res3))},
        "mask_triplets_checked":28,
        "source_modes_compared":2,
        "source_descriptor_count":expected["grouped_mode"]["descriptor_invocations"],
        "difference_count":len(differences),
        "first_divergence":differences[0] if differences else None,
        "differences":differences[:32],
        "original_16bit_x86_executed":False,
        "original_screen_frame_parity_verified":False,
    }

def main(argv=None):
    p=argparse.ArgumentParser(description=__doc__)
    for key in ("code2","sintab","res3","native-report","out"):
        p.add_argument("--"+key,required=True,type=Path)
    p.add_argument("--allow-other-version",action="store_true")
    args=p.parse_args(argv)
    try:
        def read(path,max_bytes):
            length=path.stat().st_size
            require(0<length<=max_bytes,"original source/report exceeds byte limit")
            return path.read_bytes()
        def unique(items):
            value={}
            for k,v in items:
                require(k not in value,"duplicate native report JSON key")
                value[k]=v
            return value
        def reject_nonfinite(v):raise ValueError("nonfinite JSON number")
        original_code=read(args.code2,8*1024*1024)
        original_sintab=read(args.sintab,2*1024*1024)
        original_res3=read(args.res3,32*1024*1024)
        native=json.loads(read(args.native_report,1024*1024).decode("utf-8-sig"),
                          object_pairs_hook=unique,parse_constant=reject_nonfinite)
        report=compare(original_code,original_sintab,original_res3,native,
                       pin=not args.allow_other_version)
        args.out.parent.mkdir(parents=True,exist_ok=True)
        args.out.write_text(json.dumps(report,indent=2,sort_keys=True)+"\n")
        print(json.dumps({"status":report["status"],
                          "differences":report["difference_count"]}))
        return 0 if not report["difference_count"] else 1
    except (OSError,ValueError,struct.error,TypeError,KeyError) as error:
        print("CODE.2 original VGA differential ERROR:",error,file=sys.stderr)
        return 2
if __name__=="__main__":
    raise SystemExit(main())
