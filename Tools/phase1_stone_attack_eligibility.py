#!/usr/bin/env python3
"""Phase 1: inspect Stone attack eligibility helper and zero-period paths.

Read-only SHA-verified original CODE.2, NPCS.DAT and LEVELS.DAT. Export
bounded original x86 witnesses and aggregated field evidence, not game media.
Never infer successful runtime encounters from a statically feasible path.
"""
from __future__ import annotations
import argparse
import collections
import json
import runpy
import struct
from pathlib import Path

HELPER = 0x342F8
CALLER = 0x276F5
MODE_BYTE = 0x16E86
TARGETS = (4, 24, 27)
BASELINE = {
    0x276E0: ("movzx", "WORD PTR [edi+0x16]"),
    0x276E5: ("movsx", "WORD PTR [ebx+0x12]"),
    0x276EA: ("push", "0xac"),
    0x276EF: ("push", "DWORD PTR ds:0x2757c"),
    CALLER: ("call", "0x342f8"),
    0x276FD: ("test", "eax,eax"),
    0x276FF: ("jne", "0x2786d"),
    0x27705: ("movzx", "BYTE PTR ds:0x16e86"),
    0x27718: ("cmp", "edx,0x1"),
    0x2771D: ("mov", "ecx,0x10000"),
    0x27722: ("mov", "eax,ecx"),
    0x277A5: ("movzx", "BYTE PTR [edi+0x1d]"),
    0x277B7: ("call", "0x26cf0"),
    0x277E4: ("mov", "WORD PTR [ebx+0x28],0x1"),
    0x27C9E: ("movzx", "BYTE PTR [esi+eax*1+0x23]"),
    0x27CA8: ("idiv", "ecx"),
}
RANGES = {
    "eligibility_helper": (0x342F8, 0x343D8),
    "eligibility_callpath": (0x27630, 0x277F5),
    "attack_selection": (0x26CF0, 0x26DD2),
    "timing_handler": (0x27C89, 0x27CD0),
    "runtime_mode_initializer": (0x0D8EC, 0x0D975),
    "runtime_mode_toggle": (0x2E115, 0x2E1B0),
    "render_view_global_value_producer": (0x43822, 0x438D8),
    "actor_position_bound_enforcer": (0x26FB0, 0x27065),
}
CALLS = (HELPER, 0x26CF0, 0x342F8, 0x431C6)
GLOBALS = (0x2757C, 0x16E86, 0x16ECC, 0x16EC8)

GEOMETRY_WITNESSES = {
    0x342FC: ("cmp","DWORD PTR [ebp+0x18],0x180"),
    0x34303: ("jl","0x3430a"),
    0x34305: ("xor","eax,eax"),
    0x3430A: ("mov","edx,DWORD PTR [ebp+0xc]"),
    0x3430D: ("mov","eax,0xd4"),
    0x34312: ("sub","edx,0xd4"),
    0x34318: ("sub","eax,DWORD PTR [ebp+0x14]"),
    0x3431F: ("sub","ebx,DWORD PTR [ebp+0x10]"),
    0x34322: ("sub","ecx,DWORD PTR [ebp+0x18]"),
    0x34325: ("cmp","edx,eax"),
    0x34329: ("cmp","edx,ecx"),
    0x34332: ("cmp","ebx,eax"),
    0x34336: ("cmp","ebx,ecx"),
    0x3433F: ("cmp","eax,edx"),
    0x34343: ("cmp","eax,ebx"),
    0x3434C: ("cmp","ecx,edx"),
    0x34350: ("cmp","ecx,ebx"),
    0x34359: ("mov","eax,0x1"),
    0x3F2D8: ("mov","DWORD PTR ds:0x2757c,0xd3"),
    0x3FC9E: ("mov","DWORD PTR ds:0x2757c,0xd3"),
    0x27653: ("cmp","eax,0x1"),
    0x27676: ("cmp","eax,0x1"),
    0x27695: ("test","eax,eax"),
    0x276B8: ("cmp","eax,0x1"),
    0x276CF: ("test","eax,eax"),
    0x276D3: ("mov","ecx,0x1"),
    0x276D8: ("test","ecx,ecx"),
    0x27023: ("sub","WORD PTR [ebx+0x12],0xa"),
    0x27031: ("mov","WORD PTR [ebx+0x12],0x1"),
    0x27039: ("add","WORD PTR [ebx+0x12],0xa"),
    0x27042: ("cmp","eax,0xd4"),
    0x27049: ("mov","WORD PTR [ebx+0x12],0xd4"),
    0x438AE: ("mov","ds:0x2757c,eax"),
}
def geometric_machine_witnesses(rows):
    bypc={pc:(op,args) for pc,op,args in rows}
    checked=[]
    for pc,(op,piece) in GEOMETRY_WITNESSES.items():
        actual=bypc.get(pc)
        if not actual or actual[0]!=op or piece not in actual[1]:
            raise ValueError(f"Stone 0x342F8 geometry witness drift @{pc:#x}: {actual}")
        checked.append({"pc":pc,"asm":actual[0]+" "+actual[1]})
    return checked

def executable_interval_helper(view,span,npc_screen_word,npc_dimension):
    """Literal arithmetic and signed endpoint tests transcribed from CODE.2."""
    if npc_dimension>=0x180:
        return 0
    a_hi=view-212
    a_lo=a_hi-span
    b_hi=212-npc_screen_word
    b_lo=b_hi-npc_dimension
    if b_lo<a_hi<b_hi:return 0
    if b_lo<a_lo<b_hi:return 0
    if a_lo<b_hi<a_hi:return 0
    if a_lo<b_lo<a_hi:return 0
    return 1

def zero_width_equivalence(rows):
    verified=geometric_machine_witnesses(rows)
    # When npc_dimension is zero, 0x342F8 compares a point with one
    # interval. No world grid read, distance roll or runtime exception.
    for view in (0,0xD3,0xD4,0x134,384,512):
        for actor_word in range(-512,1025):
            observed=executable_interval_helper(view,0xAC,actor_word,0)
            simplified=0 if view-384<212-actor_word<view-212 else 1
            if observed!=simplified:
                raise ValueError(f"CODE.2 zero-dimension geometry mismatch: "
                                 f"{view},{actor_word}")
    accepted=[x for x in range(0,512)
              if executable_interval_helper(0xD3,0xAC,x,0)==0]
    if accepted!=list(range(214,385)):
        raise ValueError("original helper view 0xD3 eligibility range drift")
    if (executable_interval_helper(0xD3,0xAC,256,0)!=0
        or executable_interval_helper(0xD3,0xAC,212,0)!=1
        or executable_interval_helper(0xD3,0xAC,385,0)!=1):
        raise ValueError("geometric machine bounds regression")
    return {
        "verified_machine_instruction_witnesses":verified,
        "original_helper_role":"signed interval-endpoint exclusion (zero=overlap), not grid collision",
        "original_function_return_0":"one interval endpoint strictly inside the other, or dimension >= 384",
        "zero_dimension_predicate":"view-384 < 212 - live_actor_word_0x12 < view-212",
        "initialized_view":0xD3,
        "integer_screen_word_values_passing_at_view_0xD3":[214,384],
        "example_machine_args":{"view":211,"span":172,
                                "actor_word_0x12":256,"npc_dimension_0x16":0,
                                "helper_return":0},
        "alternative_if_view_0x134_and_actor_word_0xD4":{
            "view":0x134,"actor_word":0xD4,"helper_return":
            executable_interval_helper(0x134,0xAC,0xD4,0),
            "certainty":"local mathematical witness only, not observed runtime globals"},
        "actor_local_field_update":"CODE.2 0x27023..0x27049 bounds live +0x12 to 1..0xD4 along one state-update path",
        "view_dynamic_assignment":"CODE.2 0x438AE assigns view global from eax; achievable range not established",
        "test_total":6*(1025+512),
        "reality_check":"The live +0x12 value 256 is a SAT witness, NOT an observed live actor position",
        "conditional_attack_gates":"Mode 1 and actor within cardinal tile difference 1 can enable the helper caller",
    }

def candidate_screen_field_writes(rows):
    # These are leads only: without exact base-register provenance some
    # register-relative +0x12 stores may refer to unrelated records.
    register_bases=("ebx","esi","edi","eax","edx","ecx")
    found=[]
    for i,(pc,op,arg) in enumerate(rows):
        if op not in ("mov","add","sub","inc","dec","and","or","xor"):continue
        dest=arg.split(",",1)[0]
        if not any(f"[{reg}+0x12]" in dest for reg in register_bases):continue
        if "WORD PTR" not in dest:continue
        found.append({"pc":pc,"asm":op+" "+arg,
                      "nearby":[{"pc":p,"asm":mn+" "+args}
                                for p,mn,args in rows[max(0,i-7):i+5]]})
    return {"original_candidate_u16_writes_at_object_plus_0x12":len(found),
            "bounded_leads":found[:55],
            "register_provenance":"UNVERIFIED across callers; no runtime actor value proven"}

def check_known(rows):
    lookup={pc:(op,args) for pc,op,args in rows}
    result=[]
    for pc,(op,fragment) in BASELINE.items():
        found=lookup.get(pc)
        if not found or found[0]!=op or fragment not in found[1]:
            raise ValueError(f"original Stone CODE.2 {pc:#x}: "
                             f"expected {op} {fragment}, got {found}")
        result.append({"pc":pc,"asm":found[0]+" "+found[1]})
    return result

def instructions(rows,lo,hi,max_count=150):
    return [{"pc":pc,"asm":op+" "+args}
            for pc,op,args in rows if lo<=pc<hi][:max_count]

def references(rows, target):
    token=f"0x{target:x}"
    result=[]
    for i,(pc,op,args) in enumerate(rows):
        if op not in ("call","jmp","je","jne","ja","jae","jb","jbe",
                      "jl","jle","jg","jge","jz","jnz"):
            continue
        if args!=token:continue
        before=rows[max(0,i-10):i]
        after=rows[i+1:i+8]
        result.append({
            "pc":pc, "branch":op, "target":target,
            "preceding":[{"pc":p,"asm":mn+" "+a} for p,mn,a in before],
            "following":[{"pc":p,"asm":mn+" "+a} for p,mn,a in after],
        })
    return {"count":len(result),"bounded_calls":result[:32]}

def global_sites(rows, global_offset):
    needle=f"ds:0x{global_offset:x}"
    result=[]
    for i,(pc,op,args) in enumerate(rows):
        if needle not in args:continue
        before=rows[max(0,i-4):i]
        after=rows[i+1:i+5]
        dest=args.split(",",1)[0]
        result.append({
            "pc":pc,"asm":op+" "+args,
            "kind":"definite_direct_write" if needle in dest
              and op in ("mov","inc","dec","or","and","xor","add","sub") else
              "read_or_nonassigning",
            "preceding":[{"pc":p,"asm":mn+" "+a} for p,mn,a in before],
            "following":[{"pc":p,"asm":mn+" "+a} for p,mn,a in after],
        })
    return {"count":len(result),"uses":result[:100]}

def helper_summary(rows,lo,hi):
    window=[(pc,op,args) for pc,op,args in rows if lo<=pc<hi]
    return {
      "entry":lo,
      "first_instructions":instructions(rows,lo,hi,175),
      "call_targets":[{"pc":pc,"target":int(args,16)}
                      for pc,op,args in window if op=="call"
                      and args.startswith("0x")],
      "return_instructions":[{"pc":pc,"asm":op+" "+args}
                             for pc,op,args in window if op.startswith("ret")],
      "branch_targets":[{"pc":pc,"asm":op+" "+args}
                        for pc,op,args in window if op.startswith("j")][:48],
    }

def original_npc_case(path):
    data=path.read_bytes()
    n=struct.unpack_from("<I",data)[0]
    if n!=110 or len(data)!=4+n*88:
        raise ValueError("NPCS size/count changed")
    samples=[]
    for index in TARGETS:
        record=data[4+index*88:4+(index+1)*88]
        attacks=record[0x1D]
        entries=[]
        for slot in range(attacks):
            at=record[0x1E+slot*9:0x27+slot*9]
            entries.append({
                "slot":slot,"period":at[5],
                "range":struct.unpack_from("<H",at,6)[0],
                "dice_count":at[0],"dice_sides":at[1],
                "flags":struct.unpack_from("<H",at,3)[0],
                "phase":at[8],
            })
        if len(entries)!=({4:2,24:1,27:2}[index]):
            raise ValueError("target attack count changed")
        if any(x["period"]!=0 or x["range"]!=0 for x in entries):
            raise ValueError("target zero-period record changed")
        samples.append({
            "index":index,
            "npc_name":record[:20].split(b"\0")[0].decode("latin1"),
            "attack_count":attacks,
            "helper_source_u16_at_definition_0x16":struct.unpack_from("<H",record,0x16)[0],
            "attack_entries":entries,
            "other_context_bytes":{
                "byte_0x14":record[0x14],"byte_0x15":record[0x15],
                "byte_0x4c":record[0x4c],"byte_0x4d":record[0x4d],
            },
        })
    return {"record_count":n,"sampled_zero_period_definitions":samples}

def level_code_candidate(path):
    # A coarse cross-reference only: NPC type ID field in 11-byte map
    # actor records is NOT yet established by original executable.
    blob=path.read_bytes()
    n=struct.unpack_from("<I",blob)[0]
    starts=struct.unpack_from(f"<{n}I",blob,4)
    columns=[collections.Counter() for _ in range(11)]
    rows_count=0
    sampled=[]
    for level,(a,b) in enumerate(zip(starts,(*starts[1:],len(blob)))):
        record=blob[a:b]
        off=struct.unpack_from("<7I",record,1)
        segment=record[off[2]:off[3]]
        if len(segment)%11:
            raise ValueError("11-byte level-family-A span drift")
        for k in range(0,len(segment),11):
            part=segment[k:k+11]
            rows_count+=1
            for col,value in enumerate(part):
                columns[col][value]+=1
                if (value in TARGETS and len(sampled)<12):
                    sampled.append({"level":level,"row_in_family_A":k//11,
                                    "field_offset":col,
                                    "value":value})
    return {
        "level_count":n,"section_A_records":rows_count,
        "raw_byte_column_occurrences_of_target_index":{
            str(i):{str(t):columns[i][t] for t in TARGETS}
            for i in range(11)},
        "samples_of_unverified_candidate_references":sampled,
        "certainty":"HYPOTHESIS only; no proven NPC index column",
    }

def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--root",type=Path,required=True)
    parser.add_argument("--repo-root",type=Path,default=Path("."))
    parser.add_argument("--json",type=Path,required=True)
    args=parser.parse_args()
    consumers=runpy.run_path(str(args.repo_root/"Tools/phase1_code2_consumers.py"))
    stable=runpy.run_path(str(args.repo_root/"Tools/phase1_npc_level_semantics.py"))
    cfg=consumers["GAMES"]["stone"]
    base=args.root/cfg["dir"]
    npc=base/cfg["npc"];level=base/cfg["level"]
    stable["assert_media_file"]("stone_npc",npc)
    stable["assert_media_file"]("stone_levels",level)
    code,_,meta=consumers["le_image"](base/cfg["exe"],cfg["expected"])
    rows=consumers["disassembly"](code)
    report={
        "code2_sha256":meta["sha256"],
        "original_known_machine_witnesses":check_known(rows),
        "geometric_helper_zero_dimension_equivalence":zero_width_equivalence(rows),
        "candidate_live_actor_screen_word_writes":candidate_screen_field_writes(rows),
        "helper_and_attack_windows":{
            name:helper_summary(rows,*extent)
            for name,extent in RANGES.items()},
        "direct_branch_and_call_xrefs":{hex(x):references(rows,x)
                                        for x in sorted(set(CALLS))},
        "original_data_global_reads_writes":{hex(x):global_sites(rows,x)
                                            for x in GLOBALS},
        "original_zero_period_definitions":original_npc_case(npc),
        "unproven_level_family_A_index_field":level_code_candidate(level),
        "runtime_reachability":"UNKNOWN: this probe does not execute the game",
    }
    args.json.parent.mkdir(parents=True,exist_ok=True)
    args.json.write_text(json.dumps(report,indent=2)+"\n",encoding="utf-8")
    print("stone: original helper 0x342F8 checked;",
          len(report["helper_and_attack_windows"]["eligibility_helper"]["call_targets"]),
          "direct callees,",
          report["direct_branch_and_call_xrefs"][hex(HELPER)]["count"],
          "direct callers,",
          report["unproven_level_family_A_index_field"]["section_A_records"],
          "family-A rows")

if __name__=="__main__":
    main()
