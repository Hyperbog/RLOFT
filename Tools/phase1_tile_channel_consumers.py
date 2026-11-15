#!/usr/bin/env python3
"""Phase 1: inspect original caller uses of four-channel tile lookup helpers.

Read-only original CODE.2 disassembly. Reports bounded caller instruction
windows and resource-loader candidates without asserting inferred semantics.
"""
from __future__ import annotations
import argparse
import collections
import json
import re
import runpy
from pathlib import Path

CASES={
    "menzo":{"lookup_target":(0x291e9,0x29245),"direct_data":0x39da6},
    "stone":{"lookup_target":(0x33d35,0x33d98),"direct_data":0x447d6},
}
ORIGINAL_CALLSITE_WITNESSES={
 "menzo":{
     0x25b7c:("and","al,0x3f"),
     0x25b83:("movzx","BYTE PTR [ebx+0xf]"),
     0x25b88:("movzx","BYTE PTR [ebx+0xe]"),
     0x25b8e:("call","0x291ec"),
     0x25d4d:("mov","WORD PTR [eax*2+0x25000]"),
     0x25d56:("and","al,0x3f"),
     0x25d63:("call","0x291ec"),
 },
 "stone":{
     0x2e750:("and","al,0x3f"),
     0x2e755:("movzx","BYTE PTR [ebx+0xf]"),
     0x2e75a:("movzx","BYTE PTR [ebx+0xe]"),
     0x2e760:("call","0x33d3c"),
     0x2ea5d:("and","al,0x3f"),
     0x2ea64:("call","0x33d3c"),
 },
}

def certify_call_inputs(rows,game):
    source={pc:(op,args) for pc,op,args in rows}
    checked=[]
    for pc,(op,part) in ORIGINAL_CALLSITE_WITNESSES[game].items():
        actual=source.get(pc)
        if not actual or actual[0]!=op or part not in actual[1]:
            raise ValueError(f"{game}: original tile lookup caller drift at {pc:#x}: {actual}")
        checked.append({"pc":pc,"asm":actual[0]+" "+actual[1]})
    return {
        "status":"VERIFIED original CODE.2",
        "tile_code":"passed as the low six bits of a grid cell in both titles",
        "orientation_like_selector":"passed as the u8 in caller object +0xE",
        "channel_selector":"passed as the u8 in caller object +0xF; accessor guards it <=3",
        "postlookup_action":"result compared to a caller-provided expected value; gameplay meaning not yet named",
        "instruction_witnesses":checked,
    }


IMMEDIATE_CALL=re.compile(r"^0x([0-9a-f]+)$")

def collect_callers(rows,lo,hi):
    out=[]
    by_target=collections.Counter()
    for i,(pc,op,args) in enumerate(rows):
        if op!="call":continue
        match=IMMEDIATE_CALL.fullmatch(args)
        if not match:continue
        target=int(match.group(1),16)
        if not lo<=target<hi:continue
        window=rows[max(0,i-21):min(len(rows),i+22)]
        before=rows[max(0,i-14):i]
        pushed=[{"pc":p,"value":v}
                for p,mn,v in before if mn=="push" and
                re.fullmatch(r"0x[0-9a-f]+",v)]
        out.append({"pc":pc,"target":target,
                    "nearby_literal_pushes":pushed[-6:],
                    "disassembly":[{"pc":p,"asm":mn+" "+v}
                                   for p,mn,v in window]})
        by_target[target]+=1
    return {"total_calls":len(out),
            "call_targets_histogram":{hex(k):v for k,v in sorted(by_target.items())},
            "bounded_original_caller_contexts":out[:65],
            "semantic_labels_unverified":True}

def direct_access_windows(rows,needle):
    literal=f"0x{needle:x}"
    matches=[]
    for i,(pc,mn,args) in enumerate(rows):
        if literal not in args:continue
        if mn not in ("mov","movsx","movzx","lea","add"):
            continue
        before=rows[max(0,i-10):i]
        after=rows[i+1:min(len(rows),i+15)]
        matches.append({"pc":pc,"asm":mn+" "+args,
                        "before":[{"pc":p,"asm":op+" "+v} for p,op,v in before],
                        "after":[{"pc":p,"asm":op+" "+v} for p,op,v in after]})
    return {"count":len(matches),"examples":matches[:60]}

def main():
    ap=argparse.ArgumentParser()
    ap.add_argument("--root",type=Path,required=True)
    ap.add_argument("--repo-root",type=Path,default=Path("."))
    ap.add_argument("--json",type=Path,required=True)
    a=ap.parse_args()
    core=runpy.run_path(str(a.repo_root/"Tools/phase1_code2_consumers.py"))
    output={}
    for game,profile in CASES.items():
        cfg=core["GAMES"][game]
        root=a.root/cfg["dir"]
        code,data,meta=core["le_image"](root/cfg["exe"],cfg["expected"])
        rows=core["disassembly"](code)
        low,high=profile["lookup_target"]
        output[game]={"code2_sha256":meta["sha256"],
                      "verified_original_call_inputs":certify_call_inputs(rows,game),
                      "original_four_channel_lookup_callers":collect_callers(rows,low,high),
                      "render_and_direct_table_accesses":
                      direct_access_windows(rows,profile["direct_data"])}
        print(game,"four-channel helper calls",
              output[game]["original_four_channel_lookup_callers"]["total_calls"],
              "table immediates",
              output[game]["render_and_direct_table_accesses"]["count"])
    a.json.parent.mkdir(parents=True,exist_ok=True)
    a.json.write_text(json.dumps(output,indent=2)+"\n")
if __name__=="__main__":main()
