#!/usr/bin/env python3
"""Phase 1: investigate 19 NPC bytes and grouped level-table semantics.

All inputs are SHA-256-authenticated original media. Outputs are aggregate
derived metadata and bounded x86 instruction witnesses, never original files.
Unverified gameplay field names must not be inferred from aligned bytes.
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
    "menzo":{"npc":"NPCS.DAT","levels":"LEVELS.DAT",
             "npc_ptr":0x3a5cc,"table_base":0x39da6,
             "block_witnesses":{
                 0x2b37f:("mov","0x39da6"),
                 0x2b384:("shl","edx,0x5"),
                 0x2b387:("add","ebx,edx")},
             "expect_npcs":98,"expect_levels":56,
             "expect_nonzero_table":21336},
    "stone":{"npc":"CD_Extracted/NPCS.DAT",
             "levels":"CD_Extracted/LEVELS.DAT",
             "npc_ptr":0x44790,"table_base":0x447d6,
             "block_witnesses":{
                 0x4423a:("shl","ebx,0x5"),
                 0x44244:("add","ebx,0x447d6")},
             "expect_npcs":110,"expect_levels":43,
             "expect_nonzero_table":10491},
}

def read_npcs(path, count):
    blob=path.read_bytes()
    if len(blob)!=4+count*0x58 or struct.unpack_from("<I",blob)[0]!=count:
        raise ValueError("original 0x58 NPC corpus drift")
    return [blob[4+i*88:4+(i+1)*88] for i in range(count)]

def npc_region(records):
    # Candidate 1+9+9 shape, not a claim that the first byte is a count.
    region=[r[0x1d:0x30] for r in records]
    if any(len(r)!=19 for r in region):
        raise ValueError("NPC region should contain 19 bytes")
    discriminant=collections.Counter(r[0] for r in region)
    first=[r[1:10] for r in region]
    second=[r[10:19] for r in region]
    cells=[]
    for slot,rows in enumerate((first,second)):
        cells.append({
            "candidate_slot_index":slot,
            "physical_offset_start":0x1e+9*slot,
            "physical_width_bytes":9,
            "any_nonzero":sum(any(x) for x in rows),
            "per_byte_nonzero":[sum(x[j]!=0 for x in rows) for j in range(9)],
            "per_byte_distinct":[len({x[j] for x in rows}) for j in range(9)],
            "nonzero_by_leading_byte":{
                str(v):sum(any(row) for k,row in zip(region,rows) if k[0]==v)
                for v in sorted(discriminant)},
        })
    pair_equality={
        str(j):sum(a[j]==b[j] for a,b in zip(first,second))
        for j in range(9)}
    return {
        "candidate_1_plus_9_plus_9_storage_shape":True,
        "leading_byte_at_0x1d_histogram":dict(sorted(discriminant.items())),
        "candidate_nine_byte_regions":cells,
        "matching_byte_positions_across_slots_count":pair_equality,
        "distinct_19_byte_patterns":len(set(region)),
        "meaning":"HYPOTHESIS: one descriptor byte and two 9-byte subrecords; no code-proven semantics",
    }

def npc_code_leads(rows,pointer):
    # The two games use a 0x58 NPC definition stride. Require a nearby
    # multiplication and base-global reference for a strong readsite lead.
    ptr=f"0x{pointer:x}"
    relevant=[]
    for i,(pc,op,arg) in enumerate(rows):
        if op not in ("mov","movzx","movsx","cmp","test","lea",
                      "add","sub","inc","dec","and","or","xor","push"):
            continue
        matches=list(re.finditer(r"\+0x([0-9a-f]+)\]",arg))
        offsets=[int(m.group(1),16) for m in matches]
        extensions=[p for p in offsets if 0x1d<=p<=0x2f]
        if not extensions:
            continue
        near=rows[max(0,i-95):i+20]
        pointers=[(p,mn,a) for p,mn,a in near if ptr in a]
        multiplies=[(p,mn,a) for p,mn,a in near
                    if mn=="imul" and re.search(r",0x58(?:\b|$)",a)]
        if not pointers or not multiplies:
            continue
        contexts=rows[max(0,i-10):min(len(rows),i+14)]
        relevant.append({
            "pc":pc,"opcode":op,"operand":arg,
            "candidate_definition_offsets":extensions,
            "nearest_pointer_pc":min((p for p,mn,a in pointers),
                                     key=lambda p:abs(pc-p)),
            "nearest_stride_pc":min((p for p,mn,a in multiplies),
                                    key=lambda p:abs(pc-p)),
            "context":[{"pc":p,"instruction":mn+" "+a}
                       for p,mn,a in contexts],
            "confidence":"LEAD: register provenance not verified",
        })
    return {"npc_extension_anchored_readsite_leads":len(relevant),
            "field_offsets_seen":dict(sorted(collections.Counter(
                p for r in relevant for p in r["candidate_definition_offsets"]).items())),
            "candidate_windows":relevant[:70]}

def authentic_block_witnesses(rows,conf):
    bypc={pc:(mn,a) for pc,mn,a in rows}
    verified=[]
    for pc,(mn,phrase) in conf["block_witnesses"].items():
        got=bypc.get(pc)
        if not got or got[0]!=mn or phrase not in got[1]:
            raise ValueError(f"block-index CODE.2 instruction drift {pc:#x}: {got}")
        verified.append({"pc":pc,"asm":got[0]+" "+got[1]})
    return verified

def level_table(path,count,expect_nonzero):
    blob=path.read_bytes()
    n=struct.unpack_from("<I",blob)[0]
    if n!=count:
        raise ValueError("level directory count drift")
    starts=struct.unpack_from(f"<{n}I",blob,4)
    if starts[0]!=4+4*n or any(a>=b for a,b in zip(starts,starts[1:])):
        raise ValueError("level offset directory drift")
    lanes=[collections.Counter() for _ in range(16)]
    group_nonzero=[0]*64
    group_first_nonzero=[0]*64
    group_count_hist=collections.Counter()
    total=0
    all_lanes=[0]*16
    samples=[]
    for i,(a,b) in enumerate(zip(starts,(*starts[1:],len(blob)))):
        record=blob[a:b]
        boundaries=struct.unpack_from("<7I",record,1)
        header=record[29:29+0x824]
        width,height=struct.unpack_from("<hh",header,1)
        if (len(header)!=0x824 or boundaries[0]!=29 or
            boundaries[1]!=29+0x824+2*width*height):
            raise ValueError(f"level {i}: fixed-header span changed")
        table=struct.unpack_from("<1024H",header,10)
        groups=[]
        for g in range(64):
            words=table[g*16:(g+1)*16]
            cnt=sum(x!=0 for x in words)
            group_nonzero[g]+=cnt
            group_first_nonzero[g]+=bool(cnt)
            groups.append(cnt)
            group_count_hist[cnt]+=1
            for lane,val in enumerate(words):
                lanes[lane][val]+=1
                all_lanes[lane]+=bool(val)
        total+=sum(groups)
        if i<3:
            samples.append({"level_index":i,"width":width,"height":height,
                            "active_groups":sum(x>0 for x in groups),
                            "group_nonzero_min_max":[min(groups),max(groups)],
                            "first_12_group_nonzero_counts":groups[:12]})
    if total!=expect_nonzero:
        raise ValueError(f"table corpus tally changed: {total} != {expect_nonzero}")
    return {
        "records":count,"table_offset":10,
        "size_words":1024,"inferred_32_byte_groups":64,
        "u16_words_per_group":16,"table_nonzero":total,
        "per_lane_nonzero_total":all_lanes,
        "per_lane_value_domains":[{
            "lane":i,"distinct":len(lanes[i]),
            "min":min(lanes[i]),"max":max(lanes[i]),
            "top":lanes[i].most_common(5),
        } for i in range(16)],
        "per_group_nonzero_total":group_nonzero,
        "per_group_active_level_count":group_first_nonzero,
        "per_group_number_of_nonzero_words_histogram":dict(sorted(group_count_hist.items())),
        "levels_first_three_summary":samples,
        "physical_grouping":"VERIFIED x86 index shift left five; interpretation UNKNOWN",
    }

def main():
    p=argparse.ArgumentParser()
    p.add_argument("--root",type=Path,required=True)
    p.add_argument("--repo-root",type=Path,default=Path("."))
    p.add_argument("--json",type=Path,required=True)
    a=p.parse_args()
    codeutil=runpy.run_path(str(a.repo_root/"Tools/phase1_code2_consumers.py"))
    semantic=runpy.run_path(str(a.repo_root/"Tools/phase1_npc_level_semantics.py"))
    output={}
    for name,conf in GAMES.items():
        meta=codeutil["GAMES"][name]
        root=a.root/meta["dir"]
        code,_,orig=codeutil["le_image"](root/meta["exe"],meta["expected"])
        rows=codeutil["disassembly"](code)
        npc_path=root/conf["npc"]
        level_path=root/conf["levels"]
        semantic["assert_media_file"](name+"_npc",npc_path)
        semantic["assert_media_file"](name+"_levels",level_path)
        npcs=read_npcs(npc_path,conf["expect_npcs"])
        output[name]={
            "code2_sha256":orig["sha256"],
            "original_32byte_table_block_witnesses":authentic_block_witnesses(rows,conf),
            "npc_region":npc_region(npcs),
            "npc_19byte_candidate_code_readsites":npc_code_leads(rows,conf["npc_ptr"]),
            "level_table":level_table(level_path,conf["expect_levels"],
                                      conf["expect_nonzero_table"]),
        }
        print(f"{name}: NPC candidate leading byte",
              output[name]["npc_region"]["leading_byte_at_0x1d_histogram"],
              "code leads",
              output[name]["npc_19byte_candidate_code_readsites"][
                  "npc_extension_anchored_readsite_leads"])
        print(f"  level table 64 groups × 16 words: "
              f"{output[name]['level_table']['table_nonzero']} nonzero words")
    a.json.parent.mkdir(parents=True,exist_ok=True)
    a.json.write_text(json.dumps(output,indent=2)+"\n")
if __name__=="__main__":
    main()
