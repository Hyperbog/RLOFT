#!/usr/bin/env python3
"""Phase 1 Stone RES9 targeted original-code/record witnesses, metadata only.

Read-only: emits small operand windows, x86 instruction windows, and data
flow around the shared runtime state. Does not ship original RES9 records.
"""
import argparse, collections, json, re, runpy, struct
from pathlib import Path

FOUR=(568,575,585,595)
TARGET_999=tuple(range(615,625))
CODE_WINDOWS={"opcode_08_case":(0x1E3BF,0x1E43C),
              "opcode_21_case":(0x1E50C,0x1E56B),
              "opcode_22_case":(0x1E560,0x1E5AA),
              "ip_fetch_and_control":(0x1E1D5,0x1E258),
              "choice_helper":(0x1D118,0x1D28C),
              "opcode_02_helper":(0x1C4E4,0x1C630),
              "opcode_05_helper":(0x1CED0,0x1D045)}
IP_GLOBAL="0x1599c"
BITSET_GLOBAL="0x158a4"

def parse_report(original, correlation):
    valid_ids={*FOUR,*TARGET_999}
    result={}
    for index,words in original:
        if index not in valid_ids:continue
        decoded=[]
        cursor=0
        error=None
        try:
            decoded=correlation["decode"](words,correlation["ARITY"]["stone"],"stone")
        except ValueError as e:
            error=str(e)
            m=re.search(r"at IP (\d+)",error)
            cursor=int(m.group(1)) if m else 0
        annotated=[{"ip":pc,"opcode":op+2,"arity":len(args)}
                    for pc,op,args in decoded] if decoded else []
        window_start=max(0,cursor-18)
        window_end=min(len(words),cursor+15)
        # Limited witness: no full original record or content is exported.
        around=[{"word_index":i,"value_u16":words[i]}
                for i in range(window_start,window_end)]
        # Always provide only the candidate 0x21 conditional argument pair
        # for record 615..624, not complete game script contents.
        if index in TARGET_999:
            around=[{"word_index":i,"value_u16":words[i]}
                    for i in range(14,min(21,len(words)))]
        result[str(index)]={"word_count":len(words),
                            "error":error,
                            "failure_at":cursor if error else None,
                            "nearest_words":around,
                            "last_ten_candidate_instructions":annotated[-10:],
                            "first_fourteen_candidate_instructions":annotated[:14]}
    return result

def asm_inspect(rows):
    out={}
    for label,(lo,hi) in CODE_WINDOWS.items():
        out[label]=[
            {"pc":pc,"op":op,"args":args}
            for pc,op,args in rows if lo<=pc<hi
        ][:120]
    hits=[]
    for i,(pc,op,args) in enumerate(rows):
        if BITSET_GLOBAL not in args:
            continue
        context=[
            {"pc":p,"op":name,"args":params}
            for p,name,params in rows[max(0,i-5):i+7]
        ]
        # Ignore false positives where immediate value appears in code/data.
        hits.append({"at":pc,"instruction":op+" "+args,"context":context})
    out["state_word_0x158A4_references"]={"count":len(hits),"windows":hits[:48]}
    return out

def main():
    a=argparse.ArgumentParser(description=__doc__)
    a.add_argument("--root",type=Path,required=True)
    a.add_argument("--repo-root",type=Path,default=Path("."))
    a.add_argument("--json",type=Path,required=True)
    arg=a.parse_args()
    codeutil=runpy.run_path(str(arg.repo_root/"Tools/phase1_code2_consumers.py"))
    corr=runpy.run_path(str(arg.repo_root/"Tools/phase1_res9_arity_correlate.py"))
    original=runpy.run_path(str(arg.repo_root/"Resources/RES/analyze_legacy_archives.py"))
    cfg=codeutil["GAMES"]["stone"]
    root=arg.root/cfg["dir"]
    code,_,info=codeutil["le_image"](root/cfg["exe"],cfg["expected"])
    rows=codeutil["disassembly"](code)
    programs=list(corr["read_programs"](root/"PROPHET/RES9",original))
    data={"code2_sha256":info["sha256"],"program_evidence":parse_report(programs,corr),
          "code_evidence":asm_inspect(rows)}
    if len([v for v in data["program_evidence"].values() if v["error"]])!=4:
        raise ValueError("four known unresolved program witnesses changed")
    arg.json.parent.mkdir(parents=True,exist_ok=True)
    arg.json.write_text(json.dumps(data,indent=2)+"\n")
    print("Stone unresolved:",
        {k:(v["failure_at"],v["error"]) for k,v in data["program_evidence"].items() if v["error"]})
    print("BITSET referenced",data["code_evidence"]["state_word_0x158A4_references"]["count"])
if __name__=="__main__":main()
