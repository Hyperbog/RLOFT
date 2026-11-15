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
  "flags": [0x21,0x2A,0x23,0x26],
 },
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
    if len(set(cases))!=len(cases):
        raise ValueError("case handlers share a target; investigate before assuming order")
    result=[]
    for i,addr in enumerate(cases):
        # Labels and semantics need parent/jump predecessor tracing.
        lines=insns(rows,addr,addr+0x120,60)
        result.append({"selector_value":i,"handler_offset":addr,
                       "instructions_first_60":lines})
    return {"table_offset":base,"cases":n,"dispatch_branch":conf["dispatch"],
            "handler_addresses":list(cases),
            "cases_with_bounded_provenance":result}

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
        result[game]={
           "source_code2_sha256":meta["sha256"],
           "tile_action_dispatch":table,
           "attack_dynamic_flags":candidate_flags(rows,cfg),
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
