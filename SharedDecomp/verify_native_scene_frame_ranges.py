#!/usr/bin/env python3
"""Source-only original-vs-native authored RES3 frame range differential.

The accepted source files are user-owned originals. Original bytes are never
written to the resulting metadata report or included in GitHub Actions.
"""
from __future__ import annotations
import argparse
from collections import Counter
import hashlib
import json
from pathlib import Path
import struct
import sys

sys.path.insert(0,str(Path(__file__).resolve().parents[1]/"Resources"/"RES"))
from analyze_res import lzss,parse_res
from analyze_type7 import validate_body
from verify_native_scene_instances import derive as derive_instances

PINNED={
 "LEVELS.DAT":"50d0b1d13f81733a1ac55296d00fcba9aef002202bc00341f1062c37fd017d91",
 "INIT":"cc48df4eb4c0710d45e7f5557e440129df44c5efb1af3ca3426ea9f9e3798276",
 "NPCS.DAT":"f2dd1175bf42e8291b95952282dfc0cfb84455aaa532987cdd7380094c8eb30b",
 "OBJECTS.DAT":"f23f0e7c1856904aa0156b0bf174cab794a41a327352b6ca00c3591ae5f2eed0",
 "RES3":"44955ebbed0a8ed8d874fae07b01710317044882ab3c7c95d9be54e883df9a36",
}
SCHEMA="rloft-native-authored-scene-frame-ranges/v1"
REPORT="rloft-original-authored-frame-ranges-differential/v1"

def require(ok,msg):
    if not ok:raise ValueError(msg)
def sha(data):return hashlib.sha256(data).hexdigest()
def u16(data,pos):return struct.unpack_from("<H",data,pos)[0]
def u32(data,pos):return struct.unpack_from("<I",data,pos)[0]

def source(files,with_shapes=True):
    levels=files["LEVELS.DAT"];init=files["INIT"]
    npcs=files["NPCS.DAT"];obj=files["OBJECTS.DAT"]
    require(u32(levels,0)==32 and u32(npcs,0)==83 and
            u32(obj,0)==336,"original level/NPC/object counts invalid")
    require(len(npcs)==4+83*0x45 and len(obj)==4+336*0x41,
            "original source definition record stride invalid")
    bases=struct.unpack_from("<4H",init,6)
    require(bases==(0,2169,3316,4158),
            "original INIT source-class RES3 boundaries changed")
    ends=list(bases[1:])+[4234]
    at=0x24;tables=[]
    for cls,count in ((1,175),(3,12)):
        require(u16(init,at)==count,"original INIT descriptor count invalid")
        at+=2
        table=[struct.unpack_from("<HHBBB",init,at+i*7) for i in range(count)]
        at+=count*7
        require(all(variant>0 and base+stride*variant<=ends[cls]-bases[cls]
                    for base,extent,stride,variant,influence in table),
                "original INIT variants exceed source-class bank")
        tables.append(table)
    npc_fields=[(u16(npcs,4+i*0x45+0x14),
                 *npcs[4+i*0x45+0x18:4+i*0x45+0x1b]) for i in range(83)]
    item_fields=[(u16(obj,4+i*0x41+0x28),
                  obj[4+i*0x41+0x2a]) for i in range(336)]
    require(all(b<ends[0] for b,*_ in npc_fields),
            "original NPC shape base exceeds class zero")
    require(all(b+stride<=ends[2]-bases[2] for b,stride in item_fields),
            "original item shape allocation exceeds class two")
    offsets=[u32(levels,4+i*4) for i in range(32)]+[len(levels)]
    require(offsets[0]==132 and
            all(a<b for a,b in zip(offsets,offsets[1:])),
            "original LEVELS directory offsets invalid")
    label=b"RLOFT-native-authored-frame-ranges-v1"
    encoded=bytearray(label+struct.pack("<I",32))
    hashes=[];metrics=Counter();used=set()
    for level in range(32):
        rec=levels[offsets[level]:offsets[level+1]]
        cuts=struct.unpack_from("<7I",rec,1)
        require(all(a<=b for a,b in zip(cuts,cuts[1:])),
                "original LEVELS source family intervals invalid")
        rows=[]
        for cls in range(3):
            family=rec[cuts[cls+2]:cuts[cls+3]]
            require(len(family)%11==0,
                    "original source family not 11-byte aligned")
            for ordinal in range(len(family)//11):
                archive_type=u16(family,ordinal*11)
                runtime_type=0x58 if cls==2 and archive_type==0x54 else archive_type
                if cls==0:
                    require(runtime_type<len(npc_fields),"NPC type index invalid")
                    base,animation,cycle,span=npc_fields[runtime_type]
                    absolute=bases[0]+base
                    slots=1;variants=1;stride=animation
                    metrics["npc_metadata_instances"]+=1
                elif cls==1:
                    require(runtime_type<len(tables[0]),"INIT type index invalid")
                    base,extent,stride,variants,influence=tables[0][runtime_type]
                    absolute=bases[1]+base
                    slots=stride*variants
                    animation=cycle=span=0
                    metrics["init_state_subranges"]+=1
                else:
                    require(runtime_type<len(item_fields),"OBJECTS type index invalid")
                    base,stride=item_fields[runtime_type]
                    absolute=bases[2]+base
                    slots=max(1,stride);variants=1
                    animation=cycle=span=0
                    metrics["loose_item_subranges"]+=1
                require(slots>0 and bases[cls]<=absolute and
                        absolute+slots<=ends[cls],
                        "source-authored subrange crosses original RES3 bank")
                rows.append(struct.pack("<BIHHIIBBBBB",cls,ordinal,
                    archive_type,runtime_type,absolute,slots,variants,
                    stride,animation,cycle,span))
                metrics["bounded_resource_slots"]+=slots
                used.update(range(absolute,absolute+slots))
        data=struct.pack("<HI",level,len(rows))+b"".join(rows)
        hashes.append(sha(data))
        encoded.extend(data)
        metrics["authored_instance_count"]+=len(rows)
    require(metrics["authored_instance_count"]==6204 or
            not with_shapes,"retail scene-instance count changed")
    result={
        "frame_ranges_sha256":sha(encoded),
        "per_level_frame_ranges_sha256":hashes,
        "original_instance_source_sha256":derive_instances(
            levels,init,npcs,obj)["resolved_source_bases_sha256"],
        **dict(metrics),
    }
    if with_shapes:
        require("RES3" in files,"authentic RES3 required for full gate")
        _, entries=parse_res(files["RES3"])
        require(len(entries)==4234,"original RES3 archive size changed")
        widths=Counter()
        for index in sorted(used):
            entry=entries[index]
            require(entry[:5]==b"EH\x07\x00\x00",
                    "referenced original RES3 is not EH7")
            body,consumed=lzss(entry[7:],u16(entry,5))
            require(consumed==len(entry)-7,
                    "original EH7 compressed bytes not exhausted")
            geom=validate_body(body)
            widths[geom["source_width"]]+=1
        result.update(
            unique_referenced_eh7_shapes_decoded=len(used),
            all_source_legal_frame_references_checked=metrics["bounded_resource_slots"],
            unique_width64=widths[64],
            unique_width96=widths[96],
            unique_width128=widths[128])
    return result

def compare(files,native,pin=True,with_shapes=True):
    require(isinstance(native,dict) and native.get("schema")==SCHEMA,
            "native authored source frame-range schema mismatch")
    for flag in ("original_view_subframe_selection_verified",
                 "original_npc_animation_frame_verified",
                 "original_dos_depth_order_verified",
                 "original_full_frame_parity_verified",
                 "original_live_npc_animation_verified",
                 "original_perspective_frame_choice_verified",
                 "original_dos_scene_sort_verified",
                 "original_frame_pixels_verified"):
        require(native.get(flag) is False,
                "unsupported original scene frame/gameplay parity claim "+flag)
    if pin:
        for name,s in PINNED.items():
            require(name in files and sha(files[name])==s,
                    "original "+name+" does not match pinned retail SHA256")
    expected=source(files,with_shapes)
    differences=[]
    for field,correct in expected.items():
        got=native.get(field)
        if field=="per_level_frame_ranges_sha256" and isinstance(got,list):
            if len(got)!=len(correct):
                differences.append({"field":field+"/length",
                                    "original":len(correct),"native":len(got)})
            for i,(x,y) in enumerate(zip(correct,got)):
                if x!=y: differences.append({"field":f"{field}/{i}",
                                              "original":x,"native":y})
        elif type(got)!=type(correct) or got!=correct:
            differences.append({"field":field,"original":correct,"native":got})
    for name,body in files.items():
        key="original_"+name.lower().replace(".dat","")+"_sha256"
        if key in native and native[key]!=sha(body):
            differences.append({"field":key,"original":sha(body),
                                "native":native[key]})
    return {
        "schema":REPORT,
        "status":"ALL_ORIGINAL_SCENE_FRAME_RANGES_MATCH" if not differences else "DIFFERENT",
        "source_sha256":{k:sha(v) for k,v in files.items()},
        "independent_original_instances":expected["authored_instance_count"],
        "original_bounded_frame_references":expected["bounded_resource_slots"],
        "per_level_digest_checks":len(expected["per_level_frame_ranges_sha256"]),
        "original_unique_eh7_shapes_decoded":
            expected.get("unique_referenced_eh7_shapes_decoded"),
        "difference_count":len(differences),
        "first_divergence":differences[0] if differences else None,
        "differences":differences[:32],
        "original_dos_pixel_frame_parity_verified":False,
    }

def main(argv=None):
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument("--game-dir",type=Path,required=True)
    p.add_argument("--native-report",type=Path,required=True)
    p.add_argument("--out",type=Path,required=True)
    p.add_argument("--allow-synthetic",action="store_true")
    args=p.parse_args(argv)
    try:
        def unique(pairs):
            d={}
            for k,v in pairs:
                require(k not in d,"duplicate native JSON key "+k)
                d[k]=v
            return d
        def nonfinite(v):raise ValueError("nonfinite native JSON "+v)
        native_bytes=args.native_report.read_bytes()
        require(len(native_bytes)<1024*1024,
                "native original frame range report exceeds bound")
        native=json.loads(native_bytes.decode("utf-8"),
                          object_pairs_hook=unique,parse_constant=nonfinite)
        files={}
        for name in PINNED:
            file=args.game_dir/name
            if name=="RES3" and args.allow_synthetic and not file.exists():
                continue
            size=file.stat().st_size
            require(0<size<16*1024*1024,"original "+name+" outside size bound")
            files[name]=file.read_bytes()
        report=compare(files,native,pin=not args.allow_synthetic,
                       with_shapes="RES3" in files)
        args.out.parent.mkdir(parents=True,exist_ok=True)
        args.out.write_text(json.dumps(report,indent=2,sort_keys=True)+"\n")
        print(json.dumps({"status":report["status"],
                          "instances":report["independent_original_instances"],
                          "differences":report["difference_count"]}))
        return 0 if report["difference_count"]==0 else 1
    except (OSError,KeyError,ValueError,TypeError,struct.error) as e:
        print("ORIGINAL SCENE FRAME RANGES ERROR:",e,file=sys.stderr)
        return 2
if __name__=="__main__":
    raise SystemExit(main())
