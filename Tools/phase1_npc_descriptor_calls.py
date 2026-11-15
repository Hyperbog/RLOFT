#!/usr/bin/env python3
"""Trace executable-backed nine-byte NPC descriptor consumers.

All original binaries and records remain read-only. Emits bounded instructions,
field offsets and aggregated data; register provenance is marked independently.
"""
from __future__ import annotations
import argparse
import collections
import json
import re
import runpy
import struct
from pathlib import Path

GAMES={
 "menzo":{"npc":"NPCS.DAT","first_trace":(0x202a0,0x20490),
          "npc_pointer":0x3a5cc},
 "stone":{"npc":"CD_Extracted/NPCS.DAT","first_trace":(0x27a50,0x27c50),
          "npc_pointer":0x44790},
}
MEM_OFFSET=re.compile(r"\+0x([0-9a-f]+)\]")
READS=("mov","movzx","movsx","test","cmp","lea","push","add",
       "sub","and","or")

def snippet(rows,lo,hi,limit=120):
    return [{"pc":pc,"asm":op+" "+args}
            for pc,op,args in rows if lo<=pc<hi][:limit]

def seek_nine_stride(rows):
    candidates=[]
    for i,(pc,op,arg) in enumerate(rows):
        if not (
            (op=="imul" and re.search(r",0x9(?:\b|$)",arg))
            or (op=="lea" and "*8" in arg)
        ):
            continue
        nexts=rows[i+1:min(len(rows),i+20)]
        if op=="lea":
            dst=arg.split(",",1)[0].strip()
            if not any(mn=="add" and
                       (a.startswith(dst+",") or a.endswith(","+dst))
                       for _,mn,a in nexts[:5]):
                continue
        nearby=[
            {"pc":p,"asm":mn+" "+a}
            for p,mn,a in nexts
            if any(f"+0x{v:x}]" in a for v in
                   (0x1d,0x1e,0x1f,0x23,0x26,0x27,0x2c,0x2f))
        ]
        candidates.append({
            "pc":pc,"stride_instruction":op+" "+arg,
            "near_offset_reads":nearby[:14],
            "followed_by_inserted_region_access":bool(nearby),
            "window":snippet(rows,max(0,pc-22),pc+100,70) if nearby else [],
        })
    candidates.sort(key=lambda x:(not x["followed_by_inserted_region_access"],x["pc"]))
    return {"count_total":len(candidates),
            "with_near_inserted_region_reads":sum(x["followed_by_inserted_region_access"]
                                                   for x in candidates),
            "first_candidate_witnesses":candidates[:26]}

def field_readsites(rows):
    groups=collections.defaultdict(list)
    for pc,op,arg in rows:
        if op not in READS:continue
        if "PTR [" not in arg:continue
        for mo in MEM_OFFSET.finditer(arg):
            offset=int(mo.group(1),16)
            if 0x1d<=offset<=0x2f:
                groups[offset].append((pc,op,arg))
    targets=(0x1d,0x1e,0x1f,0x23,0x26,0x27,0x2c,0x2f)
    return {"counts_by_syntactic_offset":{str(k):len(v)
                  for k,v in sorted(groups.items())},
            "selected_offset_readsite_examples":{
                str(off):[{"pc":pc,"asm":op+" "+arg}
                          for pc,op,arg in groups[off][:65]]
                for off in targets},
            "source_pointer_provenance":"UNKNOWN for unanchored register-relative reads"}

def sampled_descriptors(path):
    raw=path.read_bytes()
    n=struct.unpack_from("<I",raw)[0]
    rows=[raw[4+88*i:4+88*(i+1)] for i in range(n)]
    groups={0:[],1:[],2:[]}
    overlap=collections.defaultdict(collections.Counter)
    for i,rec in enumerate(rows):
        count=rec[0x1d]
        if count not in groups:
            raise ValueError(f"new descriptor selector {count}")
        first=rec[0x1e:0x27]
        second=rec[0x27:0x30]
        hd=rec[0x30]
        overlap[count][str(int(hd>0))]+=1
        if len(groups[count])<5:
            nm=rec[:20].partition(b"\0")[0].decode("latin1",errors="replace")
            groups[count].append({
                "record":i,"npc_name":nm,"hit_dice_byte":hd,
                "first_descriptor_present":bool(any(first)),
                "second_descriptor_present":bool(any(second)),
                "first_descriptor_damage_like_prefix_u8":list(first[:2]),
                "second_descriptor_damage_like_prefix_u8":list(second[:2]),
            })
    return {"examples_by_possible_descriptor_count":groups,
            "hit_dice_presence_by_selector":{
                str(k):dict(v) for k,v in sorted(overlap.items())},
            "naming_status":"nine-byte record interpretation not CODE.2 certified"}

def main():
    ap=argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--root",type=Path,required=True)
    ap.add_argument("--repo-root",type=Path,default=Path("."))
    ap.add_argument("--json",type=Path,required=True)
    a=ap.parse_args()
    core=runpy.run_path(str(a.repo_root/"Tools/phase1_code2_consumers.py"))
    pinned=runpy.run_path(str(a.repo_root/"Tools/phase1_npc_level_semantics.py"))
    data={}
    for game,cfg in GAMES.items():
        original=core["GAMES"][game]
        folder=a.root/original["dir"]
        npc=folder/cfg["npc"]
        pinned["assert_media_file"](game+"_npc",npc)
        code,_,meta=core["le_image"](folder/original["exe"],original["expected"])
        rows=core["disassembly"](code)
        data[game]={
            "code2_sha256":meta["sha256"],
            "nine_stride_instruction_witnesses":seek_nine_stride(rows),
            "candidate_definition_field_readsites":field_readsites(rows),
            "relevant_function_context":snippet(rows,*cfg["first_trace"],limit=220),
            "sample_descriptor_appearance":sampled_descriptors(npc),
        }
        print(f"{game}: nine-byte arithmetic leads "
              f"{data[game]['nine_stride_instruction_witnesses']['with_near_inserted_region_reads']}, "
              f"original CODE2 field offset candidates "
              f"{len(data[game]['candidate_definition_field_readsites']['counts_by_syntactic_offset'])}")
    a.json.parent.mkdir(parents=True,exist_ok=True)
    a.json.write_text(json.dumps(data,indent=2)+"\n")
if __name__=="__main__":main()
