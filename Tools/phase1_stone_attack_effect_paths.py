#!/usr/bin/env python3
"""Phase 1: Stone zero-period state reachability conditions and special-effect helpers.

Original CODE.2 and NPCS.DAT are immutable and SHA-pinned by Phase 1.
Outputs only bounded instruction-derived metadata and aggregate record facts.
Runtime reachability and untraced helper semantics remain explicitly UNKNOWN.
"""
from __future__ import annotations
import argparse
import collections
import json
import runpy
import struct
from pathlib import Path

HELPERS = {
    "menzo": {
        "attack_effect_selector_0": (0x0F850, 0x0F9A0),
        "attack_effect_selector_1": (0x285EC, 0x28730),
    },
    "stone": {
        "multi_effect_apply": (0x32F70, 0x33100),
        "two_character_word_comparison": (0x3395C, 0x33990),
        "d20_character_threshold_predicate": (0x11B94, 0x11C10),
        "identity_passthrough_helper": (0x343D8, 0x343E0),
        "attack_perform": (0x1229C, 0x1231D),
    },
}
EXCERPTS = {
    "stone": {
        "attack_state_producer": (0x26CC0, 0x26DD3),
        "alternative_state_producer": (0x276B0, 0x27872),
        "state_zero_and_two": (0x27BC0, 0x27CCF),
        "second_attack_select": (0x275D5, 0x27680),
        "special_bit_selection": (0x12510, 0x12890),
    },
    "menzo": {
        "attack_state_producer": (0x1FCD0, 0x1FF10),
        "special_bit_selection": (0xF950, 0xFA05),
    },
}
PINNED = {
    "stone": {
        0x26D48: ("cmp", "eax,edi"),
        0x26D53: ("cmp", "eax,edi"),
        0x26D57: ("mov", "WORD PTR [ebx+0x28],0x1"),
        0x26D69: ("cmp", "eax,DWORD PTR [ebp+0x18]"),
        0x26D75: ("cmp", "eax,DWORD PTR [ebp+0x18]"),
        0x26D7A: ("mov", "WORD PTR [ebx+0x28],0x2"),
        0x26D89: ("cmp", "eax,DWORD PTR [ebp+0x18]"),
        0x26D95: ("cmp", "eax,DWORD PTR [ebp+0x18]"),
        0x26D9A: ("mov", "WORD PTR [ebx+0x28],0x2"),
        0x26DB2: ("cmp", "eax,edi"),
        0x26DBD: ("cmp", "eax,edi"),
        0x26DC1: ("mov", "WORD PTR [ebx+0x28],0x1"),
        0x27C9E: ("movzx", "BYTE PTR [esi+eax*1+0x23]"),
        0x27CA8: ("idiv", "ecx"),
    },
}
TARGET_CALLS = {
    "menzo": [0xF850, 0x285EC],
    "stone": [0x32F70, 0x3395C, 0x11B94, 0x343D8,
              0x26D10, 0x26F74, 0x1229C],
}
SEMANTIC_INSTRUCTIONS = {
 "menzo": {
     0x0F87A: ("call","0x20818"),  # verified four-parameter dice helper
     0x0F892: ("call","0x10534"),  # downstream damage/interaction: unidentified
     0x0F89A: ("and","BYTE PTR [ebx+0xe],0xfd"),
 },
 "stone": {
     # title-specific u16LE character record predicate: either field equals arg
     0x3395F: ("imul","eax,DWORD PTR [ebp+0x8],0x41c"),
     0x33966: ("movsx","WORD PTR [eax+0x15e64]"),
     0x3396D: ("cmp","edx,DWORD PTR [ebp+0xc]"),
     0x33979: ("movsx","WORD PTR [eax+0x15e66]"),
     0x33980: ("cmp","eax,DWORD PTR [ebp+0xc]"),
     0x3398C: ("xor","eax,eax"),
     # helper 0x11B94: d20 roll, modifiers, compare against class table
     0x11B9A: ("imul","ebx,DWORD PTR [ebp+0x14],0x41c"),
     0x11BDA: ("call","0x29ed0"),
     0x11BFF: ("cmp","eax,edi"),
     0x11C01: ("setge","al"),
     # 0x343D8 is a passthrough, not an effect applicator
     0x343DB: ("mov","eax,DWORD PTR [ebp+0x8]"),
     0x343DE: ("pop","ebp"),
     0x343DF: ("ret",""),
     # bounded 150-entry x 44-byte effect/result catalog
     0x32FBC: ("cmp","DWORD PTR ds:0x1a578,0x96"),
     0x32FCC: ("imul","ebx,DWORD PTR ds:0x1a578,0x2c"),
 },
}

def semantic_witnesses(rows,title):
    lookup={pc:(op,args) for pc,op,args in rows}
    lines=[]
    for pc,(op,needle) in SEMANTIC_INSTRUCTIONS[title].items():
        observed=lookup.get(pc)
        if not observed or observed[0]!=op or needle not in observed[1]:
            raise ValueError(f"{title} original semantic helper witness {pc:#x}: expected "
                             f"{op} {needle}, got {observed}")
        lines.append({"pc":pc,"asm":observed[0]+" "+observed[1]})
    return {
        "instructions":lines,
        "structured_conclusions":{
           "stone_character_word_match":"two signed 16-bit character fields compared to caller's expected value; returns boolean",
           "stone_d20_threshold":"rolls 1d20 plus derived bonuses and checks >= indexed character threshold",
           "stone_identity":"CODE.2 0x343D8 returns its first argument unchanged, is NOT itself a status applicator",
           "stone_bounded_dispatch":"0x32F70 bounds a 150-entry 44-byte table; individual effects remain UNKNOWN",
           "menzo_dice_side_effect":"0xF850 calls verified dice helper and an unidentified downstream helper, then clears live flag bit 0x02",
        },
        "certainty":"Original machine instructions VERIFIED; higher-level effect names UNKNOWN",
    }


MEDIA_EXPECTED = {
    "stone": {4: (2, (0,0)), 24:(1,(0,)), 27:(2,(0,0))},
}

def window(rows, low, high, max_ops=130):
    return [{"pc":pc, "asm":op+" "+arg}
            for pc,op,arg in rows if low<=pc<high][:max_ops]

def inspect_executable(rows, title):
    lookup={pc:(op,arg) for pc,op,arg in rows}
    for pc,(op,piece) in PINNED.get(title,{}).items():
        witness=lookup.get(pc)
        if not witness or witness[0]!=op or piece not in witness[1]:
            raise ValueError(f"{title} original CODE.2 witness drift @{pc:#x}: {witness}")
    effects={}
    for name,(lo,hi) in HELPERS[title].items():
        ins=window(rows,lo,hi,100)
        effects[name]={
            "code2_entry":lo,
            "bounded_instruction_prefix":ins[:36],
            "direct_calls_in_window":[{
                 "pc":pc,"target":arg
             } for pc,op,arg in rows if lo<=pc<hi
               and op=="call" and arg.startswith("0x")][:18],
            "global_data_references":[{
                 "pc":pc,"asm":op+" "+arg
             } for pc,op,arg in rows if lo<=pc<hi
               and "ds:0x" in arg][:16],
            "meaning":"UNVERIFIED, requires callee/argument side-effect proof",
        }
    xrefs={}
    wanted=set(TARGET_CALLS[title])
    for addr in wanted:
        calls=[(i,pc) for i,(pc,op,arg) in enumerate(rows)
               if op=="call" and arg==f"0x{addr:x}"]
        xrefs[hex(addr)]={
            "callsites":len(calls),
            "bounded_caller_windows":[{
                "call_at":pc,
                "before": [{"pc":p,"asm":op+" "+arg}
                           for p,op,arg in rows[max(0,i-9):i]],
            } for i,pc in calls[:15]],
        }
    return {
        "machine_witnesses":[{"pc":pc,"asm":lookup[pc][0]+" "+lookup[pc][1]}
                              for pc in PINNED.get(title,{})],
        "state_producer_instruction_windows": {
            name:window(rows,*span,max_ops=210)
            for name,span in EXCERPTS[title].items()},
        "effect_helpers":effects,
        "callee_callsite_xrefs":xrefs,
    }

def pinned_zero_periods(path):
    raw=path.read_bytes()
    total=struct.unpack_from("<I",raw)[0]
    if total!=110 or len(raw)!=4+total*88:
        raise ValueError("Stone original 110×88 NPC catalog changed")
    data=[]
    for i in range(total):
        rec=raw[4+88*i:4+88*(i+1)]
        n=rec[0x1D]
        for s in range(n):
            desc=rec[0x1E+9*s:0x27+9*s]
            if desc[5]:
                continue
            data.append({
                "npc_index":i,
                "name":rec[:20].split(b"\0")[0].decode("latin1"),
                "slot":s,
                "dice_count":desc[0],"die_sides":desc[1],
                "flags_u16le":struct.unpack_from("<H",desc,3)[0],
                "cycle_period":desc[5],
                "range_u16le":struct.unpack_from("<H",desc,6)[0],
                "phase":desc[8],
                "all_zero":not any(desc),
            })
    actual=collections.defaultdict(list)
    for x in data:
        actual[x["npc_index"]].append(x["cycle_period"])
    if {i:(len(v),tuple(v)) for i,v in actual.items()} != MEDIA_EXPECTED["stone"]:
        raise ValueError(f"Stone zero-period NPC list changed: {actual}")
    if any(x["range_u16le"]!=0 for x in data):
        raise ValueError("Stone zero-period attack acquired nonzero range")
    return {
        "affected_attack_count":len(data),
        "affected_definition_count":len(actual),
        "per_record_evidence":data,
        "candidate_necessary_condition":"if 0 is supplied as distance threshold, both compared absolute coordinate deltas must equal 0",
        "runtime_reachability":"UNKNOWN: full register provenance and global state/overlap invariant not certified",
    }

def main():
    ap=argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--root",type=Path,required=True)
    ap.add_argument("--repo-root",type=Path,default=Path("."))
    ap.add_argument("--json",type=Path,required=True)
    args=ap.parse_args()
    source=runpy.run_path(str(args.repo_root/"Tools/phase1_code2_consumers.py"))
    hashes=runpy.run_path(str(args.repo_root/"Tools/phase1_npc_level_semantics.py"))
    report={}
    for title in ("menzo","stone"):
        cfg=source["GAMES"][title]
        folder=args.root/cfg["dir"]
        code,_,meta=source["le_image"](folder/cfg["exe"],cfg["expected"])
        rows=source["disassembly"](code)
        report[title]={
            "code2_sha256":meta["sha256"],
            "original_executable":inspect_executable(rows,title),
            "machine_verified_helper_semantics":semantic_witnesses(rows,title),
        }
        if title=="stone":
            npc=folder/cfg["npc"]
            hashes["assert_media_file"]("stone_npc",npc)
            report[title]["zero_period_original_records"]=pinned_zero_periods(npc)
        print(title,"helper profiles",len(HELPERS[title]),
              "state windows",len(EXCERPTS[title]))
    args.json.parent.mkdir(parents=True,exist_ok=True)
    args.json.write_text(json.dumps(report,indent=2)+"\n",encoding="utf-8")

if __name__=="__main__":main()
