#!/usr/bin/env python3
"""Phase 1: test NPC two-subrecord hypothesis and six-bit cell table routing.

Original CODE.2, NPCS and LEVELS are read-only and SHA authenticated by
existing Phase 1 helpers. Branch paths are evidence, not gameplay labels.
"""
from __future__ import annotations
import argparse
import collections
import json
import re
import runpy
import struct
from pathlib import Path

CFG={
 "menzo":{
   "npc":"NPCS.DAT","level":"LEVELS.DAT",
   "npc_global":0x3a5cc,
   "wall_span":(0x2b32a,0x2b430),
   "render_span":(0x38f20,0x39020),
   "lookup_span":(0x291da,0x29270),
   "table_base":0x39da6},
 "stone":{
   "npc":"CD_Extracted/NPCS.DAT","level":"CD_Extracted/LEVELS.DAT",
   "npc_global":0x44790,
   "wall_span":(0x441e0,0x4430e),
   "render_span":(0x443b0,0x444a0),
   "lookup_span":(0x33d3c,0x33dce),
   "table_base":0x447d6}
}
INTERIOR=re.compile(r"\+0x([0-9a-f]+)\]")
LOADOPS={"mov","movzx","movsx","lea","cmp","test","push","add","and","or","sub"}
DUMMY_RAW_VALUES=()

def asm_windows(rows,config):
    regions={}
    for label in ("wall_span","render_span","lookup_span"):
        lo,hi=config[label]
        regions[label]=[{"pc":pc,"asm":op+" "+args}
                        for pc,op,args in rows if lo<=pc<hi][:150]
    return regions

def npc_candidate_sites(rows,conf):
    ptr=f"0x{conf['npc_global']:x}"
    all_hits=[]
    for i,(pc,op,args) in enumerate(rows):
        if op not in LOADOPS:continue
        offsets={int(m.group(1),16) for m in INTERIOR.finditer(args)}
        offsets={v for v in offsets if 0x1d<=v<=0x2f}
        if not offsets:continue
        window=rows[max(0,i-250):min(len(rows),i+25)]
        has_ptr=any(ptr in a for _,_,a in window)
        has_stride=any(mn=="imul" and ",0x58" in a
                       for _,mn,a in window)
        has_nine=any(mn=="imul" and ",0x9" in a
                     for _,mn,a in window)
        has_8plus=any(mn=="lea" and "*8" in a
                      for _,mn,a in window)
        score=4*int(has_ptr)+4*int(has_stride)+2*int(has_nine)+int(has_8plus)
        all_hits.append((score,pc,op,args,sorted(offsets),i))
    score_counts=collections.Counter(hit[0] for hit in all_hits)
    winners=sorted(all_hits,key=lambda x:(-x[0],x[1]))
    return {
      "all_syntactic_offsets":dict(sorted(collections.Counter(
          v for _,_,_,_,values,_ in all_hits for v in values).items())),
      "candidate_count":len(all_hits),
      "score_histogram":dict(sorted(score_counts.items())),
      "high_scoring_witnesses":[{
        "pc":pc,"asm":op+" "+args,"offsets":offsets,
        "unproven_score":score,"context":[{
          "pc":p,"asm":mn+" "+a}
          for p,mn,a in rows[max(0,i-10):min(len(rows),i+12)]]}
        for score,pc,op,args,offsets,i in winners[:55]],
      "all_provenance_status":"UNPROVED: a register-relative access can be to a non-NPC struct",
    }

def record_relation(npc):
    blob=npc.read_bytes()
    n=struct.unpack_from("<I",blob)[0]
    if len(blob)!=4+n*0x58:raise ValueError("bad NPC length")
    category=collections.defaultdict(lambda:collections.Counter())
    examples=[]
    for i in range(n):
        r=blob[4+i*88:4+(i+1)*88]
        count=r[0x1d]
        first=r[0x1e:0x27]
        second=r[0x27:0x30]
        if count>2:raise ValueError(f"unexpected 9-byte candidate descriptor selector {count}")
        first_pop=any(first)
        second_pop=any(second)
        category[count][(first_pop,second_pop)]+=1
        if (count==0 and (first_pop or second_pop)) or (count==1 and second_pop):
            if len(examples)<12:
                examples.append({"record_index":i,"selector":count,
                                 "first_nonzero":first_pop,
                                 "second_nonzero":second_pop})
    return {"observed_first_byte_domain":[0,1,2],
            "slot_presence_by_selector":{
                str(k):{"first_{}_second_{}".format(int(a),int(b)):v
                        for (a,b),v in sorted(count.items())}
                for k,count in sorted(category.items())},
            "unexpected_residual_presence_examples":examples,
            "confidence":"PHYSICAL DATA ONLY: descriptor meanings unknown"}

def level_cell_correlation(level):
    blob=level.read_bytes()
    count=struct.unpack_from("<I",blob)[0]
    offsets=struct.unpack_from(f"<{count}I",blob,4)
    total=collections.Counter()
    used_per_group=[0]*64
    populated_per_group=[0]*64
    example=[]
    for index,(a,b) in enumerate(zip(offsets,(*offsets[1:],len(blob)))):
        r=blob[a:b]; bounds=struct.unpack_from("<7I",r,1)
        hdr=r[29:29+0x824]
        w,h=struct.unpack_from("<hh",hdr,1)
        if bounds[1]!=29+0x824+2*w*h:raise ValueError("level grid extent wrong")
        table=struct.unpack_from("<1024H",hdr,10)
        grid=struct.unpack_from(f"<{w*h}H",r,29+0x824)
        cell_counts=collections.Counter(x&0x3f for x in grid)
        buckets=[]
        for g in range(64):
            used=cell_counts[g]>0
            populated=any(table[16*g:16*(g+1)])
            total[("used" if used else "not_used")+"_"+
                  ("table_populated" if populated else "table_zero")]+=1
            used_per_group[g]+=bool(used)
            populated_per_group[g]+=bool(populated)
            buckets.append((g,cell_counts[g],bool(populated)))
        if index<3:
            example.append({
                "level":index,"dimensions":[w,h],
                "distinct_six_bit_codes":len(cell_counts),
                "nonzero_table_groups":sum(pop for g,n,pop in buckets),
                "used_six_bit_codes_without_table_group":[g for g,n,pop in buckets
                   if n and not pop][:16],
                "unused_six_bit_codes_with_table_group":[g for g,n,pop in buckets
                   if not n and pop][:16],
            })
    return {"levels":count,
            "cell_code_hypothesis":"word & 0x3F; Strahd-proven, Menzo/Stone unproved",
            "contingency":dict(total),
            "group_appears_as_cell_code_in_levels":used_per_group,
            "group_populated_in_levels":populated_per_group,
            "sample_mismatch_groups":example,
            "conclusion":"correlation only, not proof that group index equals cell code"}

def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument("--root",type=Path,required=True)
    p.add_argument("--repo-root",type=Path,default=Path("."))
    p.add_argument("--json",type=Path,required=True)
    a=p.parse_args()
    codeutil=runpy.run_path(str(a.repo_root/"Tools/phase1_code2_consumers.py"))
    sem=runpy.run_path(str(a.repo_root/"Tools/phase1_npc_level_semantics.py"))
    out={}
    for title,cfg in CFG.items():
        root=a.root/codeutil["GAMES"][title]["dir"]
        npc=root/cfg["npc"]
        lev=root/cfg["level"]
        sem["assert_media_file"](title+"_npc",npc)
        sem["assert_media_file"](title+"_levels",lev)
        code,_,meta=codeutil["le_image"](root/codeutil["GAMES"][title]["exe"],
                                       codeutil["GAMES"][title]["expected"])
        rows=codeutil["disassembly"](code)
        out[title]={
            "code2_sha256":meta["sha256"],
            "npc_nine_byte_presence":record_relation(npc),
            "npc_byte_access_candidates":npc_candidate_sites(rows,cfg),
            "original_table_x86_context":asm_windows(rows,cfg),
            "level_table_vs_packed_cell_code":level_cell_correlation(lev),
        }
        print(title,"NPC syntactic leads",
              out[title]["npc_byte_access_candidates"]["candidate_count"],
              "grid association",
              out[title]["level_table_vs_packed_cell_code"]["contingency"])
    a.json.parent.mkdir(parents=True,exist_ok=True)
    a.json.write_text(json.dumps(out,indent=2)+"\n")
if __name__=="__main__":main()
