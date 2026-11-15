#!/usr/bin/env python3
"""Phase 1: authenticated Menzo/Stone NPC and enlarged level-header evidence.

Read original commercial files only. Report aggregate geometry, value-domain
summaries and short disassembly witnesses, never raw files/record dumps.
Strahd field-name compatibility is NOT assumed.
"""
from __future__ import annotations
import argparse
import collections
import hashlib
import json
import runpy
import statistics
import struct
from pathlib import Path

CONF={
    "menzo":{"npc_loader":(0x1AF30,0x1AFF4),
             "level_loader":(0x1BD80,0x1BF40),
             "npc_ptr_global":0x3A5DC,"npc_count_global":0x3A600},
    "stone":{"npc_loader":(0x20EB0,0x20FBC),
             "level_loader":(0x21DB0,0x21F70),
             "npc_ptr_global":None,"npc_count_global":0x480FC},
}

def asm_range(rows,start,end,max_rows=100):
    return [{"pc":pc,"instruction":op+" "+arg}
            for pc,op,arg in rows if start<=pc<end][:max_rows]

def npc_audit(path):
    source=path.read_bytes()
    count=struct.unpack_from("<I",source)[0]
    if len(source)!=4+count*0x58:
        raise ValueError(f"{path}: NPC corpus not count*0x58")
    records=[source[4+i*0x58:4+(i+1)*0x58] for i in range(count)]
    payload=source[4:]
    sums=[]
    for pos in range(0x58):
        values=[r[pos] for r in records]
        freq=collections.Counter(values)
        sums.append({"offset":pos,"nonzero":sum(x!=0 for x in values),
                     "distinct":len(freq),"min":min(values),"max":max(values),
                     "top":freq.most_common(3)})
    pairs=[]
    for pos in range(0x14,0x57):
        nums=[struct.unpack_from("<H",r,pos)[0] for r in records]
        unique=set(nums)
        pairs.append({"offset":pos,"nonzero":sum(n!=0 for n in nums),
                      "distinct":len(unique),
                      "min":min(nums),"max":max(nums),
                      "signed_min":min((n-65536 if n>32767 else n) for n in nums),
                      "top":collections.Counter(nums).most_common(3)})
    word4=[]
    for pos in range(0x14,0x55):
        vals=[struct.unpack_from("<I",r,pos)[0] for r in records]
        if max(vals)>0 and len(set(vals))>1:
            word4.append({"offset":pos,"nonzero":sum(v!=0 for v in vals),
                          "distinct":len(set(vals)),"max":max(vals)})
    return {"count":count,"stride":88,"name_20_null_terminated":
            sum(b"\0" in r[:20] for r in records),
            "byte_columns":sums,"u16_windows":pairs,
            "candidate_u32_windows":word4,
            "additional_19_byte_nonzero":
            {str(pos):sum(r[pos]!=0 for r in records)
             for pos in range(0x45,0x58)},
            "sha256":hashlib.sha256(source).hexdigest()}

def level_audit(path):
    blob=path.read_bytes()
    count=struct.unpack_from("<I",blob)[0]
    offsets=struct.unpack_from(f"<{count}I",blob,4)
    if offsets[0]!=4+4*count or any(a>b for a,b in zip(offsets,offsets[1:])):
        raise ValueError("level directory invalid")
    headers=[]
    family=[]
    for n,(a,z) in enumerate(zip(offsets,(*offsets[1:],len(blob)))):
        entry=blob[a:z]
        if len(entry)<0x1D+0x824:
            raise ValueError(f"level {n} truncated")
        boundaries=struct.unpack_from("<7I",entry,1)
        head=entry[0x1D:0x1D+0x824]
        w,h=struct.unpack_from("<hh",head,1)
        if (boundaries[0]!=0x1D or
            boundaries[1]!=0x1D+0x824+2*w*h or
            not all(x<=y for x,y in zip(boundaries,boundaries[1:]))):
            raise ValueError(f"level {n}: invalid 0x824 header span")
        diffs=[boundaries[3]-boundaries[2],
               boundaries[4]-boundaries[3],
               boundaries[5]-boundaries[4]]
        family.append({"level":n,"width":w,"height":h,
                       "family_spans":diffs,
                       "divide_11":[v//11 if v%11==0 else None for v in diffs],
                       "divide_22":[v//22 if v%22==0 else None for v in diffs]})
        headers.append(head)
    extension=[{
        "start":j,"size":min(32,0x824-j),
        "nonzero_count":sum(1 for header in headers
                           for b in header[j:j+min(32,0x824-j)] if b!=0),
        "nonzero_position_count":sum(any(h[x] for h in headers)
                  for x in range(j,min(0x824,j+32)))
        } for j in range(0x2A4,0x824,32)]
    matches={}
    for family_index in range(3):
        for divisor in (11,22):
            if not all(v["family_spans"][family_index]%divisor==0 for v in family):
                continue
            targets=[v["family_spans"][family_index]//divisor for v in family]
            if max(targets)==0 or len(set(targets))<3:
                continue
            found=[]
            for off in range(0x824-1):
                vals=[struct.unpack_from("<H",hdr,off)[0] for hdr in headers]
                if vals==targets:
                    found.append(off)
            matches[f"family{family_index}_divide{divisor}"]={
                "target_count_min":min(targets),
                "target_count_max":max(targets),
                "target_distinct":len(set(targets)),
                "header_u16_equal_at":found}
    stats=[]
    for pos in range(0x824):
        vals=[r[pos] for r in headers]
        stats.append((sum(v!=0 for v in vals),len(set(vals))))
    prev=stats[:0x2A4]
    extra=stats[0x2A4:]
    populated=[p for p,s in enumerate(stats) if s[0]>0]
    return {"sha256":hashlib.sha256(blob).hexdigest(),
            "records":len(headers),"header_bytes":0x824,
            "fixed_width_height":[[v["width"],v["height"]] for v in family],
            "family_spans_divisible_11":{
                str(i):sum(v["family_spans"][i]%11==0 for v in family)
                for i in range(3)},
            "family_spans_divisible_22":{
                str(i):sum(v["family_spans"][i]%22==0 for v in family)
                for i in range(3)},
            "family_count_candidates":matches,
            "nonzero_header_positions":len(populated),
            "nonzero_old_0x2a4_positions":sum(s[0]>0 for s in prev),
            "nonzero_extension_positions":sum(s[0]>0 for s in extra),
            "extension_by_32_byte_block":extension,
            "nonzero_extension_ranges":[
                {"from":a,"to":b,"max_nonzero_records":max(stats[p][0]
                 for p in range(a,b+1))}
                for a,b in runs([p for p in populated if p>=0x2A4])
            ],
            "records_section_geometry":family}

def runs(xs):
    if not xs:return []
    start=last=xs[0];out=[]
    for p in xs[1:]:
        if p!=last+1:
            out.append((start,last));start=p
        last=p
    out.append((start,last))
    return out

def code_report(rows,params):
    witnesses={}
    for name in ("npc_loader","level_loader"):
        lo,hi=params[name]
        witnesses[name]=asm_range(rows,lo,hi,140)
    for key in ("npc_ptr_global","npc_count_global"):
        v=params.get(key)
        if v is None:continue
        term=f"0x{v:x}"
        occurrences=[]
        for i,(pc,op,arg) in enumerate(rows):
            if term not in arg:continue
            occurrences.append({"pc":pc,"op":op+" "+arg,
                                "context":asm_range(rows,max(0,pc-9),pc+33,22)})
        witnesses[key]={"reference_count":len(occurrences),
                        "readsite_windows":occurrences[:24]}
    return witnesses

def main():
    ap=argparse.ArgumentParser()
    ap.add_argument("--root",type=Path,required=True)
    ap.add_argument("--repo-root",type=Path,default=Path("."))
    ap.add_argument("--json",type=Path,required=True)
    args=ap.parse_args()
    repo=args.repo_root
    orig=runpy.run_path(str(repo/"Tools/phase1_code2_consumers.py"))
    result={}
    for game in CONF:
        cfg=orig["GAMES"][game]
        folder=args.root/cfg["dir"]
        code,data,meta=orig["le_image"](folder/cfg["exe"],cfg["expected"])
        rows=orig["disassembly"](code)
        npc=npc_audit(folder/cfg["npc"])
        level=level_audit(folder/cfg["level"])
        result[game]={"code2_sha256":meta["sha256"],
                      "npcs":npc,"levels":level,
                      "machine_readsite_candidates":code_report(rows,CONF[game])}
        print(f"{game}: NPCs {npc['count']}×88; levels {level['records']}×2084; "
              f"count candidates {level['family_count_candidates']}")
        print("  extension populated",level["nonzero_extension_positions"],
              "ranges",len(level["nonzero_extension_ranges"]))
    args.json.parent.mkdir(parents=True,exist_ok=True)
    args.json.write_text(json.dumps(result,indent=2)+"\n")
if __name__=="__main__":main()
