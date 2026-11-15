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
}
CALLS = (HELPER, 0x26CF0, 0x342F8, 0x431C6)
GLOBALS = (0x2757C, 0x16E86, 0x16ECC, 0x16EC8)

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
