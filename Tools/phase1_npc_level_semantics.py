#!/usr/bin/env python3
"""Phase 1: corroborate fixed field-shifts with original CPU readsites.

Separates VERIFIED machine reads from hypotheses about Strahd-equivalent
field meaning. Original media are immutable; metadata output only.
"""
from __future__ import annotations
import argparse
import collections
import json
import runpy
import struct
from pathlib import Path

WITNESSES={
"menzo":{
  0x1047C:("imul","0x58"),
  0x1048D:("imul","0x2e"),
  0x104D5:("mov","DWORD PTR [esi+0x48]"),
  0x104EA:("mov","DWORD PTR [ebx+0xe],eax"),
  0x104F6:("push","0x8"),
  0x104F8:("movzx","BYTE PTR [esi+0x30]"),
  0x104FD:("call","0x20818"),
  0x165CA:("movzx","BYTE PTR [edi+0x30]"),
  0x165ED:("mov","DWORD PTR [edi+0x48]"),
  0x165FD:("mov","DWORD PTR [ebx+0xe],eax"),
  0x1BE46:("push","0x39d9c")
},
"stone":{
  0x13779:("imul","0x58"),
  0x1378A:("imul","0x32"),
  0x137D2:("mov","DWORD PTR [edi+0x48]"),
  0x137E4:("mov","DWORD PTR [ebx+0xe],eax"),
  0x23968:("mov","DWORD PTR [edi+0x48]"),
  0x2396B:("mov","DWORD PTR [esi+0xe],eax"),
  0x23995:("test","BYTE PTR [edi+0x4c],0x80"),
  0x2399F:("test","BYTE PTR [edi+0x4d],0x8"),
  0x21E76:("push","0x447cc")
},
}
TITLE={
"menzo":{"npc":"NPCS.DAT","level":"LEVELS.DAT",
         "header_table_anchor":0x39DA6},
"stone":{"npc":"CD_Extracted/NPCS.DAT",
         "level":"CD_Extracted/LEVELS.DAT",
         "header_table_anchor":0x447D6},
}

def machine(rows,game):
    bypc={pc:(op,args) for pc,op,args in rows}
    found=[]
    for pc,(op,needle) in WITNESSES[game].items():
        actual=bypc.get(pc)
        if not actual or actual[0]!=op or needle not in actual[1]:
            raise ValueError(f"{game} CODE.2 witness {pc:#x} "
                             f"expected {op} {needle}; received {actual}")
        found.append({"pc":pc,"asm":actual[0]+" "+actual[1]})
    anchor=TITLE[game]["header_table_anchor"]
    indexes=[]
    for i,(pc,op,args) in enumerate(rows):
        if f"0x{anchor:x}" not in args:
            continue
        indexes.append({"pc":pc,"asm":op+" "+args,
                        "context":[{"pc":p,"asm":mn+" "+params}
                                   for p,mn,params in rows[max(0,i-4):i+7]]})
    # Enforce known original code reads at the original object offsets.
    return {"verified_original_machine_witnesses":found,
            "npc_definition_runtime_copy":{
                "source_u32_offset":0x48,
                "live_u32_offset":0x0E,
                "confidence":"VERIFIED",
                "npc_stride":0x58,
                "runtime_record_stride":0x2E if game=="menzo" else 0x32,
                "record_runtime_type":"live family-A NPC candidate; callsites not exhaustive"},
            "level_header_u16_table_base_pointer":{
                "address":anchor,"instruction_reference_count":len(indexes),
                "code2_candidates":indexes[:25]}}

def npc_sample(path):
    raw=path.read_bytes()
    n=struct.unpack_from("<I",raw)[0]
    assert len(raw)==4+88*n
    rows=[raw[4+i*88:4+(i+1)*88] for i in range(n)]
    hpd=[rec[0x30] for rec in rows]
    copy=[struct.unpack_from("<I",rec,0x48)[0] for rec in rows]
    sounds=[[struct.unpack_from("<H",rec,x)[0]
             for x in (0x50,0x52,0x54,0x56)] for rec in rows]
    candidate=next((i for i,v in enumerate(hpd) if v),0)
    return {"count":n,
            "candidate_definition_hit_dice_byte_0x30":{
                "populated":sum(x>0 for x in hpd),
                "range":[min(hpd),max(hpd)],
                "values_distribution":dict(sorted(collections.Counter(hpd).items())),
                "sample_record":candidate,"sample_value":hpd[candidate],
                "meaning":"LIKELY d8 hit dice; original caller passes value plus literal 8"},
            "copied_u32_0x48":{
                "nonzero_records":sum(v!=0 for v in copy),
                "distinct":len(set(copy)),
                "sample_record":candidate,"sample_value_hex":hex(copy[candidate]),
                "meaning":"VERIFIED: initialized into live runtime dword +0x0E"},
            "post_shift_sound_id_candidates_0x50_0x52_0x54_0x56":{
                "nonzero_per_column":[sum(row[i]!=0 for row in sounds)
                                       for i in range(4)],
                "sample_record":candidate,
                "sample_values":sounds[candidate],
                "meaning":"HYPOTHESIS: analogous to Strahd +0x3D/+0x3F/+0x41/+0x43, not executable-certified"},
            "pre_shift_insertion_0x1D_to_0x2F":{
                "nonzero_positions":[p for p in range(0x1D,0x30)
                                     if any(rec[p] for rec in rows)],
                "meaning":"extension candidate, per-offset functions unresolved"},
            }

def level_sample(path):
    buf=path.read_bytes()
    count=struct.unpack_from("<I",buf)[0]
    offs=struct.unpack_from(f"<{count}I",buf,4)
    arrays=[]
    samples=[]
    for i,(start,end) in enumerate(zip(offs,(*offs[1:],len(buf)))):
        rec=buf[start:end]
        h=rec[0x1D:0x1D+0x824]
        if len(h)!=0x824:
            raise ValueError("short 0x824 fixed header")
        table=struct.unpack_from("<1024H",h,0x0A)
        arr_counts=struct.unpack_from("<3H",h,0x80A)
        bounds=struct.unpack_from("<7I",rec,1)
        calculated=((bounds[3]-bounds[2])//11,
                    (bounds[5]-bounds[4])//11,
                    (bounds[4]-bounds[3])//11)
        if any((bounds[e+1]-bounds[e])%11!=0 for e in (2,3,4)) or arr_counts!=calculated:
            raise ValueError(f"record {i}: fixed header family counts differ")
        arrays.append(table)
        if i<3:
            samples.append({"index":i,"size":len(rec),
                            "fixed_header_counts_ACB":list(arr_counts),
                            "section_span_counts_ACB":list(calculated),
                            "table_nonzero_u16_entries":sum(x!=0 for x in table)})
    odd_high=sum(v>0xff for arr in arrays for v in arr)
    observed={v for arr in arrays for v in arr}
    return {"records":count,
            "verified_level_header_field_widths":{
                "table_prefix_offset":0x0A,"table_prefix_entry_count":1024,
                "table_prefix_entry_width":2,"tail_A":0x80A,
                "tail_C":0x80C,"tail_B":0x80E,
                "stradh_table_320_entry_analogy_is_hypothesis":True},
            "u16_table_value_statistics":{
                "nonzero_entries_all_levels":sum(v!=0 for arr in arrays for v in arr),
                "above_255_entries":odd_high,
                "distinct_values":len(observed),
                "range":[min(observed),max(observed)]},
            "minimal_samples":samples,
            "count_assertions_passed":count}

def main():
    p=argparse.ArgumentParser()
    p.add_argument("--root",type=Path,required=True)
    p.add_argument("--repo-root",type=Path,default=Path("."))
    p.add_argument("--json",type=Path,required=True)
    a=p.parse_args()
    previous=runpy.run_path(str(a.repo_root/"Tools/phase1_code2_consumers.py"))
    result={}
    for title,profile in TITLE.items():
        cfg=previous["GAMES"][title]
        folder=a.root/cfg["dir"]
        code,data,meta=previous["le_image"](folder/cfg["exe"],cfg["expected"])
        rows=previous["disassembly"](code)
        result[title]={"original_code2_sha256":meta["sha256"],
                       "npcs":npc_sample(folder/profile["npc"]),
                       "levels":level_sample(folder/profile["level"]),
                       "original_machine":machine(rows,title)}
        print(title,"verified code",len(WITNESSES[title]),
              "candidate u16 table hits",result[title]["original_machine"][
              "level_header_u16_table_base_pointer"]["instruction_reference_count"],
              "NPC +0x30 sample",result[title]["npcs"][
              "candidate_definition_hit_dice_byte_0x30"]["sample_value"],
              "header count proofs",result[title]["levels"]["count_assertions_passed"])
    a.json.parent.mkdir(parents=True,exist_ok=True)
    a.json.write_text(json.dumps(result,indent=2)+"\n")
if __name__=="__main__":main()
