#!/usr/bin/env python3
"""Phase 1: control-flow witnesses for attack flags, zero-divisor guards,
and tile-filter action cases, from SHA-pinned original CODE.2.

Conservative: machine instruction addresses and tiny bounded windows are
evidence, not proof of global path reachability or source-level meanings.
Never writes original media or includes game code/bytes in the repo.
"""
from __future__ import annotations
import argparse
import collections
import json
import re
import runpy
import struct
from pathlib import Path

GAMES = {
 "menzo": {
  "table_base":0x27300, "case_count":10,
  "dispatch":0x27586,
  "switch_context":(0x274D0,0x277A0),
  "state_context":(0x1FE80,0x20118),
  "damage_context":(0xF7E0,0xFAD0),
  "attack_indexed_flags":(0xF900,0xFA40),
  "period_path":(0x20320,0x20450),
  "period_entry":0x203B2,
  "state_table":0x202EC,"state_count":5,"state_dispatch":0x20353,
  "range_read":0xF911,"range_double":0xF91B,
  "flag_word_read":0xF916,
  "flags": [0x21,0x2A,0x23,0x26],
 },
 "stone": {
  "table_base":0x3064C, "case_count":11,
  "dispatch":0x308CB,
  "switch_context":(0x30810,0x30A50),
  "state_context":(0x27570,0x27850),
  "damage_context":(0x12000,0x12580),
  "attack_indexed_flags":(0x12080,0x12440),
  "period_path":(0x27BA0,0x27E20),
  "period_entry":0x27C89,
  "state_table":0x27B90,"state_count":8,"state_dispatch":0x27BFB,
  "range_read":0x12301,"range_double":0x12309,
  "flag_word_read":0x1230B,
  "flags": [0x21,0x2A,0x23,0x26],
 },
}

PINNED_CASE_DISPATCH={
    "menzo":[0x2758e,0x275aa,0x2749c,0x275cc,0x2749c,0x2749c,
             0x2749c,0x2749c,0x2749c,0x275ee],
    "stone":[0x308d3,0x308ef,0x307e1,0x30911,0x307e1,0x307e1,
             0x307e1,0x307e1,0x307e1,0x307e1,0x30933],
}
PINNED_ACTOR_STATE_DISPATCH={
    "menzo":[0x2035b,0x203b2,0x203b2,0x2041d,0x20445],
    "stone":[0x27c03,0x27c89,0x27c89,0x27d7f,0x27dda,
             0x27dac,0x27dc7,0x27de2],
}


def insns(rows,a,b,cap=150):
    return [{"pc":pc,"asm":op+" "+arg}
            for pc,op,arg in rows if a<=pc<b][:cap]

def address_uses(rows, targets):
    result={}
    for target in targets:
        s=f"0x{target:x}"
        found=[]
        for idx,(pc,op,arg) in enumerate(rows):
            if op not in ("je","jne","jz","jnz","ja","jae","jb","jbe",
                          "jg","jge","jl","jle","jmp","call","loop","jecxz"):
                continue
            if arg!=s:continue
            before=rows[max(idx-4,0):idx]
            found.append({"pc":pc,"instruction":op+" "+arg,
                          "prior":[{"pc":p,"asm":mn+" "+val}
                                   for p,mn,val in before]})
        result[hex(target)]={"references":len(found),"examples":found[:45]}
    return result

def case_targets(code,rows,conf):
    n=conf["case_count"];base=conf["table_base"]
    if base+n*4>len(code):raise ValueError("switch table beyond code")
    cases=struct.unpack_from(f"<{n}I",code,base)
    starts={pc for pc,_,_ in rows}
    if any(addr not in starts for addr in cases):
        raise ValueError(f"unaligned original opcode table {base:#x}: {cases}")
    # Multiple selector values can share an actual code handler.
    # Preserve those aliases in the data instead of fabricating unique cases.
    alias_counts=collections.Counter(cases)
    result=[]
    for i,addr in enumerate(cases):
        # Labels and semantics need parent/jump predecessor tracing.
        lines=insns(rows,addr,addr+0x120,60)
        result.append({"selector_value":i,"handler_offset":addr,
                       "instructions_first_60":lines})
    return {"table_offset":base,"cases":n,"dispatch_branch":conf["dispatch"],
            "handler_addresses":list(cases),
            "aliased_handler_targets":{hex(pc):cnt for pc,cnt in alias_counts.items()
                                       if cnt>1},
            "distinct_case_handlers":len(alias_counts),
            "cases_with_bounded_provenance":result}

def original_state_table(code,rows,conf):
    base=conf["state_table"]
    count=conf["state_count"]
    handlers=struct.unpack_from(f"<{count}I",code,base)
    starts={pc for pc,_,_ in rows}
    if any(h not in starts for h in handlers):
        raise ValueError("original NPC live-state dispatch target unaligned")
    return {
        "state_bytecode_table":base,
        "case_count":count,
        "dispatch":conf["state_dispatch"],
        "cases":[{"state":i,"handler_pc":pc,
                  "first_24_insns":insns(rows,pc,pc+0xA0,24)}
                 for i,pc in enumerate(handlers)],
        "period_read_branch_entry":conf["period_entry"],
        "case_indices_directly_entering_period_loop":[i for i,pc in enumerate(handlers)
                                                   if pc==conf["period_entry"]],
        "unknown_runtime_state_reachability":True,
    }

def attack_range_field_reads(rows,conf):
    pcs={
        "u16_distance_parameter":conf["range_read"],
        "multiply_by_two":conf["range_double"],
        "u16_control_flags":conf["flag_word_read"],
    }
    by_pc={pc:(op,arg) for pc,op,arg in rows}
    reads=[]
    for name,pc in pcs.items():
        instruction=by_pc.get(pc)
        if instruction is None:
            raise ValueError(f"missing original attack range machine read @{pc:#x}")
        reads.append({"field":name,"pc":pc,"instruction":instruction[0]+" "+instruction[1]})
    if "WORD PTR" not in by_pc[conf["range_read"]][1]:
        raise ValueError("attack distance parameter must be u16")
    if "WORD PTR" not in by_pc[conf["flag_word_read"]][1]:
        raise ValueError("attack control field must be u16")
    if by_pc[conf["range_double"]][0]!="add" or not (
        by_pc[conf["range_double"]][1] in ("ecx,ecx","edx,edx")):
        raise ValueError("attack distance twice-scale machine operation changed")
    return {"descriptor_offset_relative":6,
            "width_bytes":2,
            "endian":"little",
            "role":"VERIFIED: twice-scaled attack distance/range input",
            "flag_word_relative_offset":3,
            "flag_width_bytes":2,
            "machine_witnesses":reads,
            "known_data_range_for_active_attack":"Menzo 0x8000, Stone 0x0000/0x8000",
            "note":"zero-valued fixed-point range in 5 Stone descriptors needs conditional path proof"}


def candidate_flags(rows,conf):
    a,b=conf["attack_indexed_flags"]
    selected=insns(rows,a,b,200)
    flagmem=re.compile(r"\[(?:edi|esi)\+eax\*1\+0x(?:21|2a|24|25|23|26)\]")
    matched=[]
    for i,(pc,mn,arg) in enumerate(rows):
        if flagmem.search(arg):
            matched.append({"pc":pc,"instruction":mn+" "+arg,
                            "surroundings":insns(rows,pc-26,pc+65,32)})
    return {"original_attack_window":selected,
            "cross_object_readsite_matches":matched[:55]}

STATE_WRITERS = {
    "menzo":{"field_offset":0x24,"source_ranges":[(0x1FA1C,0x20120),(0x20320,0x20450)]},
    "stone":{"field_offset":0x28,"source_ranges":[(0x26CF0,0x27040),
                                                   (0x27570,0x27890),
                                                   (0x27BB0,0x27E20)]},
}

def state_writes(rows,title):
    conf=STATE_WRITERS[title]
    suffix=f"+0x{conf['field_offset']:x}]"
    findings=[]
    for i,(pc,op,arg) in enumerate(rows):
        if op not in ("mov","inc","dec","add","sub","and","or","xor"):
            continue
        operand=arg.split(",",1)[0].strip()
        if not operand.endswith(suffix):
            continue
        if "WORD PTR" not in operand:
            continue
        nearby=insns(rows,max(0,pc-22),pc+45,26)
        relevant=any(a<=pc<b for a,b in conf["source_ranges"])
        findings.append({"pc":pc,"asm":op+" "+arg,
                         "within_known_attack_runtime":relevant,
                         "context":nearby})
    return {"state_word_offset":conf["field_offset"],
            "total_register_relative_writes":len(findings),
            "within_known_attack_runtime":sum(x["within_known_attack_runtime"]
                                              for x in findings),
            "sample_writers":[x for x in findings
                              if x["within_known_attack_runtime"]][:65],
            "other_candidates":[x for x in findings
                                if not x["within_known_attack_runtime"]][:35],
            "confidence":"register-base def-use not globally certified"}

def flag_behavior_windows(rows,title):
    ranges=([(0xF900,0xFA20),(0x1FF20,0x200A0)] if title=="menzo"
            else [(0x12300,0x12590),(0x27590,0x27665)])
    return [dict(start=a,end=b,instructions=insns(rows,a,b,220))
            for a,b in ranges]


def original_trace(rows,conf):
    targets=[conf["period_entry"],conf["dispatch"]]
    for i in range(conf["case_count"]):
        pass
    return {
        "period_state_window":insns(rows,*conf["period_path"],cap=240),
        "period_entry_inbound_edges":address_uses(rows,[conf["period_entry"]]),
        "original_action_dispatch_window":insns(rows,*conf["switch_context"],cap=240),
        "original_attack_control_window":insns(rows,*conf["state_context"],cap=200),
        "damage_helper_window":insns(rows,*conf["damage_context"],cap=150),
    }

def main():
    ap=argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--root",type=Path,required=True)
    ap.add_argument("--repo-root",type=Path,default=Path("."))
    ap.add_argument("--json",type=Path,required=True)
    args=ap.parse_args()
    codeutil=runpy.run_path(str(args.repo_root/"Tools/phase1_code2_consumers.py"))
    result={}
    for game,cfg in GAMES.items():
        media=codeutil["GAMES"][game]
        folder=args.root/media["dir"]
        code,_,meta=codeutil["le_image"](folder/media["exe"],media["expected"])
        rows=codeutil["disassembly"](code)
        table=case_targets(code,rows,cfg)
        if table["handler_addresses"] != PINNED_CASE_DISPATCH[game]:
            raise ValueError(f"{game}: original action handler regression")
        state=original_state_table(code,rows,cfg)
        if [entry["handler_pc"] for entry in state["cases"]] != PINNED_ACTOR_STATE_DISPATCH[game]:
            raise ValueError(f"{game}: actor state handler regression")
        result[game]={
           "source_code2_sha256":meta["sha256"],
           "tile_action_dispatch":table,
           "live_npc_action_state_dispatch":state,
           "attack_range_and_control_u16_fields":attack_range_field_reads(rows,cfg),
           "attack_dynamic_flags":candidate_flags(rows,cfg),
           "attack_flag_behavior_windows":flag_behavior_windows(rows,game),
           "live_attack_state_writer_leads":state_writes(rows,game),
           "control_flow":original_trace(rows,cfg),
           "table_handler_inbound_edges":
               address_uses(rows,[row["handler_offset"] for row in table[
                   "cases_with_bounded_provenance"]]),
           "confidence":"CASE target bytes and opcodes verified; semantics pending path trace",
        }
        print(game,"verified switch targets",len(table["handler_addresses"]),
              "attack period entry inbound",result[game]["control_flow"][
              "period_entry_inbound_edges"][hex(cfg["period_entry"])]["references"])
    args.json.parent.mkdir(parents=True,exist_ok=True)
    args.json.write_text(json.dumps(result,indent=2)+"\n",encoding="utf-8")

if __name__=="__main__":
    main()
