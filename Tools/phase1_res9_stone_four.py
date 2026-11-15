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
              "opcode_05_helper":(0x1CED0,0x1D045),
              "opcode_2E_handler":(0x1EA59,0x1EAA1),
              "opcode_2E_helper":(0x1D594,0x1D708)}
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
    if any(data["program_evidence"][str(i)]["error"] for i in FOUR):
        raise ValueError("Stone four previously undecoded records regressed")
    originals={i:words for i,words in programs}
    case_by_pc={}
    observations=[]
    for i in TARGET_999:
        words=originals[i]
        decoded=corr["decode"](words,corr["ARITY"]["stone"],"stone")
        case_by_pc={pc:(logical,args) for pc,logical,args in decoded}
        branch=case_by_pc.get(14)
        consumer=case_by_pc.get(17)
        if branch!=(6,(187,33)) and not (
            branch and branch[0]==6 and tuple(branch[1])==(187,33)
        ):
            raise ValueError(f"Stone record {i} 0x08 alignment changed: {branch}")
        if not consumer or consumer[0]!=44 or tuple(consumer[1])!=(999,21,27):
            raise ValueError(f"Stone record {i} 0x2E witness changed: {consumer}")
        if 27 not in case_by_pc:
            raise ValueError(f"Stone record {i} destination 27 not decoded")
        observations.append({"record":i,
                             "prior_opcode_at_word14":0x08,
                             "prior_serialized_args":[187,33],
                             "opcode_word17":0x2e,
                             "operand_0_word18":999,
                             "operand_1_word19":21,
                             "control_target_word20":27,
                             "target_27_is_instruction_start":True})
    by_address={pc:(op,args) for pc,op,args in rows}
    machine_witness={
        0x1ea5e:("movzx","WORD PTR [eax*2+0x2ec32]"),
        0x1ea67:("movzx","WORD PTR [eax*2+0x2ec30]"),
        0x1ea81:("movzx","WORD PTR [eax*2+0x2ec34]"),
        0x1ea89:("mov","ds:0x1599c,eax"),
        0x1ea94:("add","ds:0x1599c,0x3"),
    }
    for pc,(opcode,phrase) in machine_witness.items():
        ins=by_address.get(pc)
        if not ins or ins[0]!=opcode or phrase not in ins[1]:
            raise ValueError(f"Original opcode 0x2E handler changed @{pc:#x}: {ins}")
    data["target_999_resolution"]={
        "status":"verified_operand_not_control_target",
        "affected_records":list(TARGET_999),
        "observations":observations,
        "opcode_2e_case_at":0x1ea59,
        "opcode_2e_helper_at":0x1d594,
        "case_handler_instruction_witnesses":[hex(v) for v in machine_witness],
    }
    arg.json.parent.mkdir(parents=True,exist_ok=True)
    arg.json.write_text(json.dumps(data,indent=2)+"\n")
    print("Stone unresolved:",
        {k:(v["failure_at"],v["error"]) for k,v in data["program_evidence"].items() if v["error"]})
    print("Ten 999 operand witnesses: PASS; 999 is not a direct target")
    print("BITSET referenced",data["code_evidence"]["state_word_0x158A4_references"]["count"])
if __name__=="__main__":main()
