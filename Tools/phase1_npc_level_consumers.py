#!/usr/bin/env python3
"""Phase 1: cross-title CODE.2 readsites for NPC and 0x824-level fields.

Original LE code is SHA pinned. Address hits are exact object-relative
disassembly operands. Register-derived field candidates remain marked
LIKELY until def-use provenance has been independently certified.
"""
from __future__ import annotations
import argparse
import collections
import json
import re
import runpy
from pathlib import Path

FAMILIES={
 "menzo":{"header_base":0x39D9C,"header_len":0x824,
          "npc_definition_base_ptr":0x3A5CC,
          "npc_count":0x3A600,"npc_stride":0x58},
 "stone":{"header_base":0x447CC,"header_len":0x824,
          "npc_definition_base_ptr":0x44790,
          "npc_count":0x480FC,"npc_stride":0x58},
}
GLOBAL=re.compile(r"ds:0x([0-9a-f]+)")
MEM=re.compile(r"\[([^\]]+)\]")
DISP=re.compile(r"\+0x([0-9a-f]+)")
MEM_IMMEDIATE=re.compile(r"^(?:BYTE|WORD|DWORD) PTR ")


def lines(rows,a,b,limit=55):
    return [{"pc":pc,"opcode":op,"operand":arg}
            for pc,op,arg in rows if a<=pc<b][:limit]


def classify_memory(op,arg,addr):
    # Intel syntax with memory as first operand is a write for mov/lea,
    # but RMW patterns may read and write. Preserve instruction unchanged.
    dst=arg.split(",",1)[0].strip()
    if op in ("mov","movzx","movsx","cmp","test","push"):
        if op=="mov" and addr in dst:
            return "write"
        return "read"
    if op in ("inc","dec","add","sub","and","or","xor","shl","shr"):
        return "read_modify_write"
    return "unknown"


def direct_header_accesses(rows,head,size):
    by_field=collections.defaultdict(list)
    for i,(pc,op,arg) in enumerate(rows):
        vals=[int(x,16) for x in GLOBAL.findall(arg)]
        for value in vals:
            if head<=value<head+size:
                off=value-head
                item={"pc":pc,"op":op+" "+arg,
                      "kind":classify_memory(op,arg,f"ds:0x{value:x}")}
                by_field[off].append(item)
    fields=[]
    for off,hits in sorted(by_field.items()):
        fields.append({
            "header_offset":off,
            "hits":len(hits),
            "read":sum(v["kind"]=="read" for v in hits),
            "write":sum(v["kind"]=="write" for v in hits),
            "examples":hits[:9],
        })
    return {"header_base":head,"span":size,
            "distinct_direct_absolute_offsets":len(fields),
            "direct_absolute_fields":fields,
            "level_counts_endian_le16_offsets":[0x80A,0x80C,0x80E],
            "count_field_references":{
               str(pos):by_field.get(pos,[])[:30]
               for pos in (0x80A,0x80C,0x80E)}}


def pointer_starts(rows,address):
    token=f"ds:0x{address:x}"
    out=[]
    for i,(pc,op,args) in enumerate(rows):
        if token not in args: continue
        out.append({"pc":pc,"instruction":op+" "+args,
                    "window":lines(rows,max(0,pc-8),pc+90,45)})
    return {"global_addr":address,
            "reference_count":len(out),
            "windows":out[:35]}


def npc_indexed_candidates(rows,global_ptr):
    # Require both scale 0x58 and the known runtime pointer/global in
    # a bounded instruction window; this is intentionally NOT a proof
    # of an individual NPC field meaning.
    token=f"0x{global_ptr:x}"
    findings=[]
    for i,(pc,op,arg) in enumerate(rows):
        if op!="imul" or "0x58" not in arg:
            continue
        lo=max(0,i-38);hi=min(len(rows),i+55)
        window=rows[lo:hi]
        relevant=[(j,p,mn,a) for j,(p,mn,a) in enumerate(window)
                  if token in a]
        if not relevant:
            continue
        distances=[abs(i-(lo+j)) for j,p,mn,a in relevant]
        if min(distances)>35:
            continue
        field_hits=[]
        for p,mn,a in rows[max(0,i-18):min(len(rows),i+50)]:
            for mem in MEM.findall(a):
                if "ds:0x" in mem:continue
                disps=[int(x,16) for x in DISP.findall(mem)]
                for offset in disps:
                    if 0x14<=offset<0x58:
                        field_hits.append({"pc":p,"candidate_offset":offset,
                                           "asm":mn+" "+a})
        findings.append({
            "stride_multiply_at":pc,
            "multiply":op+" "+arg,
            "near_pointer_global":[{"pc":p,"asm":mn+" "+a}
                                   for j,p,mn,a in relevant[:9]],
            "near_register_relative_field_leads":field_hits[:22],
            "instruction_window":lines(rows,pc-28,pc+90,64),
        })
    return {"stride_0x58_within_35_instructions_of_base_pointer":
            len(findings),"windows":findings[:50]}


def shared_direct_fields(a,b):
    x={r["header_offset"]:r for r in a["direct_absolute_fields"]}
    y={r["header_offset"]:r for r in b["direct_absolute_fields"]}
    found=sorted(set(x)&set(y))
    return {"shared_offset_count":len(found),
            "shared": [{"off":p,
                        "menzo_reads":x[p]["read"],
                        "stone_reads":y[p]["read"],
                        "menzo_writes":x[p]["write"],
                        "stone_writes":y[p]["write"],
                        "menzo_sample":x[p]["examples"][0],
                        "stone_sample":y[p]["examples"][0]}
                       for p in found]}


def main():
    ap=argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--root",type=Path,required=True)
    ap.add_argument("--repo-root",type=Path,default=Path("."))
    ap.add_argument("--json",type=Path,required=True)
    args=ap.parse_args()
    consumers=runpy.run_path(str(args.repo_root/"Tools/phase1_code2_consumers.py"))
    result={}
    for game,profile in FAMILIES.items():
        cfg=consumers["GAMES"][game]
        folder=args.root/cfg["dir"]
        code,data,meta=consumers["le_image"](folder/cfg["exe"],cfg["expected"])
        rows=consumers["disassembly"](code)
        headers=direct_header_accesses(rows,profile["header_base"],
                                        profile["header_len"])
        ptr=pointer_starts(rows,profile["npc_definition_base_ptr"])
        indexed=npc_indexed_candidates(rows,profile["npc_definition_base_ptr"])
        result[game]={
            "code2_sha256":meta["sha256"],
            "original_code_instruction_count":len(rows),
            "level_header_direct_addresses":headers,
            "npc_loaded_definition_pointer":ptr,
            "npc_indexed_field_candidates_not_yet_verified":indexed,
        }
        print(f"{game}: header fields with direct absolute access "
              f"{headers['distinct_direct_absolute_offsets']}; "
              f"NPC global refs {ptr['reference_count']}; "
              f"NPC stride-linked leads {indexed['stride_0x58_within_35_instructions_of_base_pointer']}")
        print("  Count readsites:",{p:len(v)
              for p,v in headers["count_field_references"].items()})
    result["cross_title"]={
        "common_direct_header_offsets":
        shared_direct_fields(result["menzo"]["level_header_direct_addresses"],
                             result["stone"]["level_header_direct_addresses"])
    }
    args.json.parent.mkdir(parents=True,exist_ok=True)
    args.json.write_text(json.dumps(result,indent=2)+"\n")
if __name__=="__main__":main()
