#!/usr/bin/env python3
"""Phase 1: corroborate fixed field-shifts with original CPU readsites.

Separates VERIFIED machine reads from hypotheses about Strahd-equivalent
field meaning. Original media are immutable; metadata output only.
"""
from __future__ import annotations
import argparse
import collections
import hashlib
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

NPC_FIELD_OFFSETS=(0x30,0x31,0x32,0x33,0x34,0x35,
                   0x38,0x3B,0x44,0x48,0x4C,0x4D,
                   0x50,0x52,0x54,0x56)

def npc_offset_code_leads(rows):
    # Unproven bare register-relative addressing is explicitly a lead.
    # Call-path-specific facts must still be tied to the 0x58 stride or
    # pointer previously authenticated in machine().
    out={}
    for offset in NPC_FIELD_OFFSETS:
        hits=[]
        needle=f"+0x{offset:x}]"
        for i,(pc,op,args) in enumerate(rows):
            if needle not in args:continue
            if "PTR [" not in args:continue
            hits.append({"pc":pc,"asm":op+" "+args,
                         "context":[{"pc":p,"asm":mn+" "+ar}
                                    for p,mn,ar in rows[max(0,i-7):i+9]]})
        out[str(offset)]={"unfiltered_count":len(hits),
                          "candidate_instances":hits[:12],
                          "register_provenance_verified":False}
    return out


PINNED_MEDIA={
    "menzo_npc":"ad365e5da571e95306cf20f226399088563a7cc5d371d53db971e8c1e8449e39",
    "menzo_levels":"d3509c9288cc5b1e14b6a9e6c4e2d496e336b3dcaca05555a65e5ed4e564d8aa",
    "stone_npc":"646e6936e496a66bfa2768cb2288d9b5a959cd0042ab8530f1ecc0b3f24c3a02",
    "stone_levels":"212b56deb635af954a257744a15c26bc48d801b6e09b0ef7fc2b614a425513b2",
    "strahd_levels":"50d0b1d13f81733a1ac55296d00fcba9aef002202bc00341f1062c37fd017d91",
}

def assert_media_file(key,path):
    digest=hashlib.sha256(path.read_bytes()).hexdigest()
    if digest!=PINNED_MEDIA[key]:
        raise ValueError(f"Phase 1 preserved original changed {key}: {digest}")
    return digest


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
    helper_ranges={
        "menzo":{
            "npc_hd_die_roll_helper":(0x20818,0x20960),
            "npc_flags_test":(0xFBA0,0xFD05),
            "npc_sound0_reader":(0x200A0,0x20195),
            "npc_sound1_reader":(0xFD50,0xFE20),
            "npc_sound2_reader":(0x10640,0x106AC),
        },
        "stone":{
            "npc_hd_or_hp_helper":(0x29F08,0x2A090),
            "npc_flags_test":(0x12620,0x1276A),
            "npc_sound0_reader":(0x10D18,0x10DB0),
            "npc_sound1_reader":(0x12A05,0x12B58),
            "npc_flag_init":(0x23930,0x23A00),
        },
    }
    helper_windows={
        k:[{"pc":pc,"asm":op+" "+args}
           for pc,op,args in rows if lo<=pc<hi][:150]
        for k,(lo,hi) in helper_ranges[game].items()}
    # Enforce known original code reads at the original object offsets.
    return {"verified_original_machine_witnesses":found,
            "original_helper_instruction_windows":helper_windows,
            "candidate_register_field_reads":npc_offset_code_leads(rows),
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

def level_sample(path, table_count=1024):
    buf=path.read_bytes()
    count=struct.unpack_from("<I",buf)[0]
    offs=struct.unpack_from(f"<{count}I",buf,4)
    arrays=[]
    samples=[]
    tail_records=[]
    for i,(start,end) in enumerate(zip(offs,(*offs[1:],len(buf)))):
        rec=buf[start:end]
        header_bytes=0x0A+table_count*2+0x1A
        h=rec[0x1D:0x1D+header_bytes]
        if len(h)!=header_bytes:
            raise ValueError("short fixed header")
        table=struct.unpack_from(f"<{table_count}H",h,0x0A)
        tail=0x0A+2*table_count
        arr_counts=struct.unpack_from("<3H",h,tail)
        bounds=struct.unpack_from("<7I",rec,1)
        calculated=((bounds[3]-bounds[2])//11,
                    (bounds[5]-bounds[4])//11,
                    (bounds[4]-bounds[3])//11)
        if any((bounds[e+1]-bounds[e])%11!=0 for e in (2,3,4)) or arr_counts!=calculated:
            raise ValueError(f"record {i}: fixed header family counts differ")
        arrays.append(table)
        tail_records.append(h[tail+6:tail+0x1A])
        if i<3:
            samples.append({"index":i,"size":len(rec),
                            "fixed_header_counts_ACB":list(arr_counts),
                            "section_span_counts_ACB":list(calculated),
                            "table_nonzero_u16_entries":sum(x!=0 for x in table)})
    odd_high=sum(v>0xff for arr in arrays for v in arr)
    observed={v for arr in arrays for v in arr}
    suffix_profile={
        str(0x0A+2*table_count+6+index):{
            "width_bytes":1,
            "nonzero_records":sum(t[index]!=0 for t in tail_records),
            "distinct_values":len({t[index] for t in tail_records}),
            "observed_min":min(t[index] for t in tail_records),
            "observed_max":max(t[index] for t in tail_records),
        }
        for index in range(0x1A-6)
    }
    return {"records":count,
            "verified_level_header_field_widths":{
                "table_prefix_offset":0x0A,"table_prefix_entry_count":table_count,
                "table_prefix_entry_width":2,"tail_A":0x0A+2*table_count,
                "tail_C":0x0C+2*table_count,
                "tail_B":0x0E+2*table_count,
                "semantic_equivalence_unproved":True},
            "u16_table_value_statistics":{
                "nonzero_entries_all_levels":sum(v!=0 for arr in arrays for v in arr),
                "above_255_entries":odd_high,
                "distinct_values":len(observed),
                "range":[min(observed),max(observed)]},
            "minimal_samples":samples,
            "tail_after_three_counts_byte_profiles":suffix_profile,
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
        npc_sha=assert_media_file(title+"_npc",folder/profile["npc"])
        level_sha=assert_media_file(title+"_levels",folder/profile["level"])
        result[title]={"original_code2_sha256":meta["sha256"],
                       "original_npc_sha256":npc_sha,
                       "original_level_sha256":level_sha,
                       "npcs":npc_sample(folder/profile["npc"]),
                       "levels":level_sample(folder/profile["level"]),
                       "original_machine":machine(rows,title)}
        print(title,"verified code",len(WITNESSES[title]),
              "candidate u16 table hits",result[title]["original_machine"][
              "level_header_u16_table_base_pointer"]["instruction_reference_count"],
              "NPC +0x30 sample",result[title]["npcs"][
              "candidate_definition_hit_dice_byte_0x30"]["sample_value"],
              "header count proofs",result[title]["levels"]["count_assertions_passed"])
    strahd_path=a.root/"Ravenloft - Strahd's Possession"/"RLOFT"/"LEVELS.DAT"
    assert_media_file("strahd_levels",strahd_path)
    result["strahd_reference"]={
        "levels":level_sample(strahd_path,table_count=320),
        "evidence":"Original Strahd LEVELS.DAT read-only source; fixed header 0x2A4",
    }
    if result["strahd_reference"]["levels"]["count_assertions_passed"]!=32:
        raise ValueError("Strahd 32-level reference geometry changed")
    print("strahd-reference:",result["strahd_reference"]["levels"]["records"],
          "levels, 320-word header table, all count equations checked")
    a.json.parent.mkdir(parents=True,exist_ok=True)
    a.json.write_text(json.dumps(result,indent=2)+"\n")
if __name__=="__main__":main()
