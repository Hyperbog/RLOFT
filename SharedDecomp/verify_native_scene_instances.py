#!/usr/bin/env python3
"""Independent raw-original oracle for native RES3 source-base scene instances.

No original x86 code is executed; no raw records or shape bytes are exported.
Renderer frame choice, RES3 rasterization and gameplay parity remain unproved.
"""
from __future__ import annotations

import argparse
import hashlib
import json
from pathlib import Path
import struct
import sys

SCHEMA = "rloft-native-authored-scene-instances/v1"
RESULT_SCHEMA = "rloft-original-scene-bases-crosscheck/v1"
KNOWN = {
    "LEVELS.DAT": "50d0b1d13f81733a1ac55296d00fcba9aef002202bc00341f1062c37fd017d91",
    "INIT": "cc48df4eb4c0710d45e7f5557e440129df44c5efb1af3ca3426ea9f9e3798276",
    "NPCS.DAT": "f2dd1175bf42e8291b95952282dfc0cfb84455aaa532987cdd7380094c8eb30b",
    "OBJECTS.DAT": "f23f0e7c1856904aa0156b0bf174cab794a41a327352b6ca00c3591ae5f2eed0",
}
MAX_SOURCE = 8*1024*1024
MAX_NATIVE = 1024*1024


def require(ok: bool, reason: str) -> None:
    if not ok:
        raise ValueError(reason)


def sha(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()


def u16(data: bytes, pos: int) -> int:
    require(0 <= pos <= len(data)-2, "source uint16 outside bytes")
    return struct.unpack_from("<H", data, pos)[0]


def u32(data: bytes, pos: int) -> int:
    require(0 <= pos <= len(data)-4, "source uint32 outside bytes")
    return struct.unpack_from("<I", data, pos)[0]


def derive(levels: bytes, init: bytes, npcs: bytes, objects: bytes) -> dict:
    require(len(init) >= 0x24, "original INIT truncated")
    require(len(levels) >= 132 and u32(levels, 0) == 32,
            "original LEVELS must have 32 source maps")
    require(len(npcs) == 4 + 83*0x45 and u32(npcs, 0) == 83,
            "original NPCS definition cardinality/stride")
    require(len(objects) == 4 + 336*0x41 and u32(objects, 0) == 336,
            "original OBJECTS definition cardinality/stride")

    starts = [u16(init, 6+2*i) for i in range(4)]
    require(starts[0] == 0 and all(a < b for a,b in zip(starts,starts[1:]))
            and starts[3] < 4234, "original INIT RES3 source-class starts")
    lengths = [starts[1], starts[2]-starts[1], starts[3]-starts[2], 4234-starts[3]]
    pos=0x24
    init_tables=[]
    for wanted, length in ((175,lengths[1]), (12,lengths[3])):
        require(u16(init,pos)==wanted, "original INIT descriptor count")
        pos+=2
        require(pos+wanted*7<=len(init), "INIT descriptor array truncated")
        rows=[]
        for j in range(wanted):
            base, extent, stride, variants, influence = struct.unpack_from(
                "<HHBBB",init,pos+j*7)
            require(variants>0 and base+stride*variants<=length,
                    "original INIT shape variant exceeds source bank")
            rows.append((base,extent,stride,variants,influence))
        require(max(r[0]+r[2]*r[3] for r in rows)==length,
                "INIT descriptor allocation does not close source class")
        pos+=wanted*7
        init_tables.append(rows)

    npc_bases=[u16(npcs,4+i*0x45+0x14) for i in range(83)]
    object_bases=[(u16(objects,4+i*0x41+0x28),
                  objects[4+i*0x41+0x2A]) for i in range(336)]
    require(all(v < lengths[0] for v in npc_bases),
            "NPC source base outside class 0")
    require(all(base<lengths[2] and base+stride<=lengths[2]
                for base,stride in object_bases),
            "OBJECTS source base/stride outside class 2")
    require(max(base+stride for base,stride in object_bases)==lengths[2],
            "OBJECTS definitions do not close class 2")

    offsets=[u32(levels,4+4*i) for i in range(32)]+[len(levels)]
    require(offsets[0]==132 and all(a<b for a,b in zip(offsets,offsets[1:])),
            "original LEVELS directory offsets invalid")
    raw_serial=bytearray(b"RLOFT-compact-world-scene-families-v1")
    raw_serial+=struct.pack("<I",32)
    resolved=bytearray(b"RLOFT-native-scene-source-bases-v1")
    resolved+=struct.pack("<I",32)
    per_level=[]
    totals=[0,0,0]
    aliases=0
    for level_id in range(32):
        rec=levels[offsets[level_id]:offsets[level_id+1]]
        require(len(rec)>=29, "level directory record truncated")
        cuts=[u32(rec,1+4*i) for i in range(7)]
        require(cuts[0]>=29 and cuts[0]+0x2A4<=cuts[1] and
                all(a<=b for a,b in zip(cuts,cuts[1:])) and cuts[6]<=len(rec),
                "original scene record boundaries invalid")
        width,height=struct.unpack_from("<hh",rec,cuts[0]+1)
        require(0<width<=1024 and 0<height<=1024, "original map shape invalid")
        raw_serial+=struct.pack("<HHH",level_id,width,height)
        entries=[]
        for cls in range(3):
            first,last=cuts[cls+2],cuts[cls+3]
            require((last-first)%11==0,"original scene record stride")
            source=rec[first:last]
            raw_serial+=struct.pack("<I",len(source)//11)+source
            for ordinal in range(len(source)//11):
                oid,x,xf,y,yf,flags=struct.unpack_from(
                    "<HhHhHB",source,ordinal*11)
                require(0<=x<width and 0<=y<height,
                        "authored scene instance coordinate outside map")
                runtime_id=oid
                if cls==0:
                    require(runtime_id<83,"NPC source type out of range")
                    base=npc_bases[runtime_id]
                    facing=flags&7
                elif cls==1:
                    require(runtime_id<175,"INIT class 1 type out of range")
                    base=starts[1]+init_tables[0][runtime_id][0]
                    facing=0
                else:
                    if runtime_id==0x54:
                        runtime_id=0x58
                        aliases+=1
                    require(runtime_id<336,"OBJECTS scene type out of range")
                    base=starts[2]+object_bases[runtime_id][0]
                    facing=0
                require(starts[cls]<=base<starts[cls]+lengths[cls],
                        "scene candidate base outside source class")
                row=struct.pack("<BIHHIIHHBBI",
                    cls,ordinal,oid,runtime_id,
                    (x*65536+xf)&0xffffffff,(y*65536+yf)&0xffffffff,
                    x,y,flags,facing,base)
                require(len(row)==27,"original resolved record encoding mismatch")
                entries.append(row)
                totals[cls]+=1
        per_level_bytes=struct.pack("<HI",level_id,len(entries))+b"".join(entries)
        per_level.append(sha(per_level_bytes))
        resolved+=per_level_bytes
    return {
        "input_levels_sha256":sha(levels),
        "input_init_sha256":sha(init),
        "input_npcs_sha256":sha(npcs),
        "input_objects_sha256":sha(objects),
        "map_count":32,
        "res3_entries":4234,
        "authored_npc_instances":totals[0],
        "authored_placed_instances":totals[1],
        "authored_loose_instances":totals[2],
        "legacy_object_type_aliases":aliases,
        "compact_scene_instance_source_sha256":sha(raw_serial),
        "resolved_source_bases_sha256":sha(resolved),
        "per_level_resolved_sha256":per_level,
    }


def compare(original: dict[str,bytes], native: dict,
            require_known=True) -> dict:
    require(isinstance(native,dict) and native.get("schema")==SCHEMA,
            "native scene-source report schema invalid")
    for name in ("original_view_frame_selection_verified",
                 "original_dos_rasterizer_executed","gameplay_parity_verified"):
        require(native.get(name) is False,
                "unsupported original gameplay/render parity claim: "+name)
    if require_known:
        for name,known in KNOWN.items():
            require(sha(original[name])==known,
                    name+" original SHA256 does not match pinned retail source")
    expected=derive(original["LEVELS.DAT"],original["INIT"],
                    original["NPCS.DAT"],original["OBJECTS.DAT"])
    differences=[]
    for key,value in expected.items():
        got=native.get(key)
        if type(got) is not type(value) or got!=value:
            if isinstance(value,list) and isinstance(got,list):
                for i,(a,b) in enumerate(zip(value,got)):
                    if a!=b:
                        differences.append({"field":f"{key}/{i}",
                                            "expected":a,"native":b})
                if len(value)!=len(got):
                    differences.append({"field":key+"/length",
                                        "expected":len(value),
                                        "native":len(got)})
            else:
                differences.append({"field":key,"expected":value,"native":got})
    return {
        "schema":RESULT_SCHEMA,
        "status":"ALL_ORIGINAL_SCENE_BASES_MATCH" if not differences else "DIFFERENT",
        "source_identity_pinned":require_known,
        "verified_original_level_count":32,
        "verified_original_authored_instances":sum(
            expected[k] for k in ("authored_npc_instances",
                                  "authored_placed_instances",
                                  "authored_loose_instances")),
        "original_source_hashes":{k:sha(v) for k,v in original.items()},
        "per_level_resolution_hashes_checked":32,
        "source_definition_fields_checked":len(expected),
        "first_divergence":differences[0] if differences else None,
        "difference_count":len(differences),
        "differences":differences[:32],
        "original_res3_content_decoded":False,
        "original_rasterization_verified":False,
        "original_gameplay_parity_verified":False,
    }


def main(argv=None) -> int:
    p=argparse.ArgumentParser(description=__doc__)
    for name,option in (("levels","LEVELS.DAT"),("init","INIT"),
                        ("npcs","NPCS.DAT"),("objects","OBJECTS.DAT")):
        p.add_argument("--"+name,required=True,type=Path)
    p.add_argument("--native-report",required=True,type=Path)
    p.add_argument("--out",required=True,type=Path)
    p.add_argument("--allow-other-version",action="store_true")
    args=p.parse_args(argv)
    try:
        def read(path,limit):
            size=path.stat().st_size
            require(0<size<=limit,"evidence input outside size bound")
            return path.read_bytes()
        def unique(pairs):
            out={}
            for key,value in pairs:
                require(key not in out,"duplicate native JSON field")
                out[key]=value
            return out
        def reject_nonfinite(value):
            raise ValueError("nonfinite native JSON number")
        native=json.loads(read(args.native_report,MAX_NATIVE).decode("utf-8-sig"),
                          object_pairs_hook=unique,parse_constant=reject_nonfinite)
        source={name:read(getattr(args,option),MAX_SOURCE) for
                option,name in (("levels","LEVELS.DAT"),("init","INIT"),
                                ("npcs","NPCS.DAT"),("objects","OBJECTS.DAT"))}
        result=compare(source,native,require_known=not args.allow_other_version)
        args.out.parent.mkdir(parents=True,exist_ok=True)
        args.out.write_text(json.dumps(result,indent=2,sort_keys=True)+"\n")
        print(json.dumps({"status":result["status"],"differences":result["difference_count"],
                          "instances":result["verified_original_authored_instances"]}))
        return 0 if result["status"]=="ALL_ORIGINAL_SCENE_BASES_MATCH" else 1
    except (OSError,ValueError,KeyError,TypeError,struct.error) as e:
        print("ORIGINAL SCENE BASES EVIDENCE ERROR:",e,file=sys.stderr)
        return 2


if __name__=="__main__":
    raise SystemExit(main())
