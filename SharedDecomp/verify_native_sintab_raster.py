#!/usr/bin/env python3
"""Independent original-file SINTAB+RES3 raster-span plans vs compiled C++.

This module parses raw original files; it never imports NativeEngine code,
never executes original x86, and never claims rasterized DOS-frame parity.
"""
from __future__ import annotations
import argparse
from collections import Counter
import hashlib
import json
from pathlib import Path
import struct
import sys

sys.path.insert(0, str(Path(__file__).resolve().parents[1]/"Resources"/"RES"))
from analyze_res import lzss,parse_res

SCHEMA="rloft-native-sintab-eh7-raster-plans/v1"
RESULT_SCHEMA="rloft-sintab-original-versus-native-plan-diff/v1"
PINNED={
    "SINTAB":"19b6bc15f1be616bb8ff49dc9029995a1a0413fae06addc9fd41ddef761d0e87",
    "RES3":"44955ebbed0a8ed8d874fae07b01710317044882ab3c7c95d9be54e883df9a36",
}

def require(cond,msg):
    if not cond:raise ValueError(msg)
def sha(b):return hashlib.sha256(b).hexdigest()
def w(b,n):return struct.unpack_from("<H",b,n)[0]
def u32(b,n):return struct.unpack_from("<I",b,n)[0]

def program_metrics(code:bytes):
    at=0
    bounds={0}
    op=Counter()
    while at<len(code):
        pair=code[at:at+2]
        if pair in (b"\x8a\x86",b"\x88\x87",b"\x88\xa7"):
            require(at+6<=len(code),"SINTAB truncated six-byte opcode")
            disp=struct.unpack_from("<i",code,at+2)[0]
            if pair==b"\x8a\x86":
                require(0<=disp<128,"SINTAB source read outside source column")
                op["read"]+=1
            else:
                require(0<=disp<=20400 and disp%80==0,
                        "SINTAB store outside aligned original VGA row")
                op["write"]+=1
            at+=6
        elif pair in (b"\x88\xc4",b"\x29\xc8"):
            at+=2
        elif code[at]==0xc3:
            at+=1
            require(at==len(code),"SINTAB interior original RET")
            op["return"]+=1
        else:
            raise ValueError("unknown original SINTAB generated opcode")
        bounds.add(at)
        op["instructions"]+=1
    require(op["return"]==1,"SINTAB missing natural return")
    return bounds,op

def source(sintab:bytes,res3:bytes):
    require(len(sintab)>=0x1F42+512,"original SINTAB prefix/trailer truncated")
    require((u32(sintab,0x1B38),u32(sintab,0x1B3C),u32(sintab,0x1B40))==
            (127,508,128),"original SINTAB scale/lookup count invalid")
    declared=u32(sintab,0x1F3E)
    rows=[]
    patches=[]
    at=0x1F42
    total_payload=total_code=reads=writes=0
    for scale in range(127):
        require(at+2<=len(sintab),"truncated SINTAB size word")
        size=w(sintab,at)
        at+=2
        require(size>=0x205 and at+size<=len(sintab),
                "truncated original SINTAB scale block")
        block=sintab[at:at+size]
        at+=size
        patch=struct.unpack_from("<129H",block)
        step=struct.unpack_from("<129H",block,0x102)
        code=block[0x204:]
        boundaries,ops=program_metrics(code)
        require(patch[0]==0x204 and all(0x204<=p<size and
                    p-0x204 in boundaries for p in patch) and
                all(a<=b for a,b in zip(patch,patch[1:])),
                "original SINTAB patch instruction boundary invalid")
        patches.append(patch)
        total_payload+=size
        total_code+=len(code)
        reads+=ops["read"]
        writes+=ops["write"]
        rows.append({
            "index":scale,
            "original_block_sha256":sha(block),
            "generated_code_sha256":sha(code),
            "code_bytes":len(code),
            "first_patch":patch[0],
            "last_patch":patch[-1],
            "summed_scale_steps":sum(step),
            "instructions":ops["instructions"],
            "source_reads":ops["read"],
            "destination_writes":ops["write"],
        })
    require(total_payload==declared and
            len(sintab)-at==512 and
            not any(sintab[at:]),"original SINTAB payload/trailer mismatch")
    _, entries=parse_res(res3)
    assert len(patches)==127
    # Count each original column's actual six-byte raster descriptors.
    pairs=Counter()
    for id,item in enumerate(entries):
        require(item[:5]==b"EH\x07\x00\x00" and len(item)>=8,
                f"RES3 original entry {id} not expected EH7")
        decoded,used=lzss(item[7:],w(item,5))
        require(used==len(item)-7 and len(decoded)>=10,
                "original EH7 LZSS decoded stream invalid")
        width,xmin,xmax,ymin,ymax=struct.unpack_from("<Hhhhh",decoded)
        require(width in (64,96,128) and
                0<=xmin<=xmax<width and 0<=ymin<=ymax<=128,
                "original EH7 geometry outside bounds")
        count=xmax-xmin+1
        tableend=10+2*count
        require(tableend<=len(decoded),"original EH7 pointer table truncated")
        for ptr in struct.unpack_from("<"+str(count)+"H",decoded,10):
            require(tableend<=ptr<len(decoded),"original EH7 list pointer invalid")
            cursor=ptr
            while True:
                require(cursor+2<=len(decoded),"original EH7 list missing stop")
                stop=w(decoded,cursor)
                if stop==0:break
                require(cursor+6<=len(decoded),"original EH7 descriptor truncated")
                entry=w(decoded,cursor+4)
                require(entry<stop<=0x100 and
                        entry%2==0 and stop%2==0,
                        "original EH7 descriptor selector invalid")
                pairs[(entry//2,stop//2)]+=1
                cursor+=6
    n_descriptors=sum(pairs.values())
    nonempty=0
    for scale in (0,63,126):
        patch=patches[scale]
        for (entry,stop),count in pairs.items():
            require(patch[entry]<=patch[stop],
                    "original EH7 entry/stop plan reversed")
            nonempty+=(patch[entry]<patch[stop])*count
    return {
        "original_sintab_sha256":sha(sintab),
        "original_res3_sha256":sha(res3),
        "scale_count":127,
        "total_scale_payload_bytes":total_payload,
        "total_generated_code_bytes":total_code,
        "original_source_read_opcodes":reads,
        "original_destination_store_opcodes":writes,
        "eh7_original_entries":len(entries),
        "eh7_descriptors_referenced":n_descriptors,
        "eh7_sample_scale_count":3,
        "eh7_descriptor_scale_plans":n_descriptors*3,
        "eh7_nonempty_span_plans":nonempty,
        "scale_blocks":rows,
    }

def compare(sintab:bytes,res3:bytes,native:dict,require_retail=True):
    require(isinstance(native,dict) and native.get("schema")==SCHEMA,
            "compiled native SINTAB raster report schema mismatch")
    for flag in ("original_x86_code_executed",
                 "original_view_raycast_verified",
                 "original_pixel_frame_parity_verified"):
        require(native.get(flag) is False,
                "unsupported original execution/frame parity claim: "+flag)
    require(native.get("safe_opcode_interpreter_implemented") is True,
            "missing portable source interpreter statement")
    if require_retail:
        for filename,content in (("SINTAB",sintab),("RES3",res3)):
            require(sha(content)==PINNED[filename],
                    filename+" original file differs from pinned retail SHA256")
    expected=source(sintab,res3)
    diffs=[]
    for key,value in expected.items():
        got=native.get(key)
        if key=="scale_blocks" and isinstance(got,list) and len(got)==127:
            for i,(a,b) in enumerate(zip(value,got)):
                for field,correct in a.items():
                    observed=b.get(field) if isinstance(b,dict) else None
                    if type(observed) is not type(correct) or observed!=correct:
                        diffs.append({"field":f"scale_blocks/{i}/{field}",
                                      "original":correct,"native":observed})
        elif type(got) is not type(value) or got!=value:
            diffs.append({"field":key,
                          "original":len(value) if isinstance(value,list) else value,
                          "native":len(got) if isinstance(got,list) else got})
    return {
        "schema":RESULT_SCHEMA,
        "status":"ORIGINAL_SINTAB_RASTER_PLAN_MATCH" if not diffs else "DIFFERENT",
        "original_sintab_sha256":sha(sintab),
        "original_res3_sha256":sha(res3),
        "scale_blocks_compared":127,
        "original_eh7_descriptors_compared":expected["eh7_descriptors_referenced"],
        "selector_scale_mappings_checked":expected["eh7_descriptor_scale_plans"],
        "per_scale_typed_fields_compared":127*len(expected["scale_blocks"][0]),
        "difference_count":len(diffs),
        "first_divergence":diffs[0] if diffs else None,
        "differences":diffs[:32],
        "original_dos_x86_executed":False,
        "original_pixel_frame_parity_verified":False,
    }

def main(argv=None):
    p=argparse.ArgumentParser(description=__doc__)
    for flag in ("sintab","res3","native-report","out"):
        p.add_argument("--"+flag,required=True,type=Path)
    p.add_argument("--allow-other-version",action="store_true")
    args=p.parse_args(argv)
    try:
        def limited(file,size):
            n=file.stat().st_size
            require(0<n<=size,"source/native report exceeds expected size")
            return file.read_bytes()
        def pairs(values):
            d={}
            for k,v in values:
                require(k not in d,"duplicate native JSON report key")
                d[k]=v
            return d
        def nonfinite(v):raise ValueError("nonfinite JSON number")
        sintab=limited(args.sintab,2*1024*1024)
        res3=limited(args.res3,32*1024*1024)
        native=json.loads(limited(args.native_report,1024*1024).decode("utf-8-sig"),
                          object_pairs_hook=pairs,parse_constant=nonfinite)
        result=compare(sintab,res3,native,not args.allow_other_version)
        args.out.parent.mkdir(parents=True,exist_ok=True)
        args.out.write_text(json.dumps(result,indent=2,sort_keys=True)+"\n")
        print(json.dumps({"status":result["status"],
                          "differences":result["difference_count"]}))
        return 0 if result["difference_count"]==0 else 1
    except (OSError,ValueError,TypeError,KeyError,struct.error) as e:
        print("ORIGINAL SINTAB DIFFERENTIAL ERROR:",e,file=sys.stderr)
        return 2
if __name__=="__main__":
    raise SystemExit(main())
