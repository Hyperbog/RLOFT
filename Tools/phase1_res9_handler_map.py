#!/usr/bin/env python3
"""Read-only CODE.2 RES9 opcode table/operand-boundary comparison."""
import argparse
import collections
import json
import runpy
import struct
from pathlib import Path

VM = {
 "menzo":dict(jump=0x19b4d, table=0x19a6c, count=43,
              guard=0x19b40, bound=0x2a, subtract=0x19b37,
              stream=0x1d950, ip=0x8c0c, archive="CD_Extracted/RES9"),
 "stone":dict(jump=0x1e220, table=0x1e10c, count=54,
              guard=0x1e20c, bound=0x35, subtract=0x1e203,
              stream=0x2ec30, ip=0x1599c, archive="PROPHET/RES9"),
}

def dispatch(rows, code, title):
    spec=VM[title]
    instructions={pc:(op,operand) for pc,op,operand in rows}
    checks=[
        (spec["jump"],"jmp",f"+0x{spec['table']:x}]"),
        (spec["guard"],"cmp",f"ax,0x{spec['bound']:x}"),
        (spec["subtract"],"sub","eax,0x2"),
    ]
    for pc,op,piece in checks:
        actual=instructions.get(pc)
        if actual is None or actual[0]!=op or piece not in actual[1]:
            raise ValueError(f"{title} @{pc:#x}: {actual!r} versus {op} {piece}")
    table=list(struct.unpack_from(f"<{spec['count']}I",code,spec["table"]))
    if any(t not in instructions for t in table):
        raise ValueError("case target not aligned to disassembly")
    starts=sorted(set(table))
    result=[]
    for opcode,start in enumerate(table):
        next_addr=next((p for p in starts if p>start),start+0x9c)
        end=min(start+0x9c,next_addr)
        asm=[{"at":pc,"instruction":op+" "+operand}
             for pc,op,operand in rows if start<=pc<end][:80]
        referenced=[
            entry for entry in asm
            if f"0x{spec['ip']:x}" in entry["instruction"]
            or f"0x{spec['stream']:x}" in entry["instruction"]
            or "movzx ax,WORD PTR" in entry["instruction"]
            or "mov ax,WORD PTR" in entry["instruction"]
        ]
        result.append(dict(logical_opcode=opcode,encoded_opcode=opcode+2,
                           handler=start,context=asm,operand_fetch_leads=referenced))
    return {"case_count":len(result),"code_object_relative_jump":spec["jump"],
            "table":spec["table"],"encoded_min":2,
            "encoded_max":1+spec["count"],"cases":result}

HELPERS = {"menzo":{"opcode_02":0x18850,"opcode_05":0x192a0,"opcode_2A":0x19534},
           "stone":{"opcode_02":0x1c4e4,"opcode_05":0x1ced0,"opcode_37":0x1d77c}}


def helper_windows(rows,title):
    out={}
    for name,start in HELPERS[title].items():
        lines=[{"pc":pc,"asm":op+" "+args}
               for pc,op,args in rows if start<=pc<start+0x1b0]
        out[name]={"entry":start,"instructions":lines[:125]}
    return out


def programs(folder, title, original):
    items=original["parse_res"](folder/VM[title]["archive"])
    for i,rec in enumerate(items):
        if len(rec)<2 or rec[:2]==b"EH":continue
        n=struct.unpack_from("<H",rec)[0]
        if len(rec) in (2+2*n,2+4*n):
            yield i,list(struct.unpack_from(f"<{n}H",rec,2))

def parse_shifted(words, maximum, arity):
    """Hypothesis: original -2 opcode, but Strahd operand grammar.
    Not asserting the hypothesis as runtime behavior."""
    ip=0
    parsed=[]
    while ip<len(words):
        at=ip
        op=words[ip]-2
        ip+=1
        if op<0 or op>maximum:
            raise ValueError(f"opcode {op+2:#x} outside original range at IP {at}")
        if op in (0,1):
            if ip>=len(words):raise ValueError(f"missing list length at {at}")
            count=words[ip]
            ip+=1
            if count>len(words)-ip:raise ValueError(f"list overrun at {at}")
            args=[count]+words[ip:ip+count]
            ip+=count
        elif op==2:
            begin=ip
            if ip>=len(words):raise ValueError(f"missing dialog selector at {at}")
            ip+=1
            while True:
                if ip+1>=len(words):raise ValueError(f"unterminated dialog at {at}")
                cond=words[ip]
                ip+=2
                if cond==0x21:break
            args=words[begin:ip]
        elif op in arity:
            count=arity[op]
            if count>len(words)-ip:raise ValueError(f"operand overrun at {at}")
            args=words[ip:ip+count]
            ip+=count
        else:
            raise ValueError(f"unknown operand count for logical opcode {op:#x} at {at}")
        parsed.append({"ip":at,"opcode":op,"args":args})
    return parsed

def analyze(root, repo):
    codeutil=runpy.run_path(str(repo/"Tools/phase1_code2_consumers.py"))
    strahd=runpy.run_path(str(repo/"Resources/RES/analyze_legacy_archives.py"))
    report={}
    for title in ("menzo","stone"):
        cfg=codeutil["GAMES"][title]
        folder=root/cfg["dir"]
        code,_,meta=codeutil["le_image"](folder/cfg["exe"],cfg["expected"])
        asm=codeutil["disassembly"](code)
        handlers=dispatch(asm,code,title)
        successes=[]
        failures=[]
        counts=collections.Counter()
        bad_targets=[]
        for index,words in programs(folder,title,strahd):
            try:
                parsed=parse_shifted(words,handlers["case_count"]-1,strahd["ACTIVE_ARITY"])
                valid={x["ip"] for x in parsed}
                bad=[]
                for ins in parsed:
                    counts[ins["opcode"]]+=1
                    for target in strahd["targets"](ins):
                        if target not in valid:bad.append((ins["ip"],target))
                successes.append({"record":index,"instruction_count":len(parsed),
                                  "bad_Strahd_branch_targets":len(bad)})
                if bad:bad_targets.append({"record":index,"examples":bad[:6]})
            except (ValueError,KeyError,IndexError) as e:
                failures.append({"record":index,"first_mismatch":str(e)})
        print(f"{title}: authenticated {handlers['case_count']} cases, "
              f"Strahd-arity-hypothesis: {len(successes)}/{len(successes)+len(failures)} "
              f"programs, first failures {failures[:5]}")
        report[title]={"original_sha256":meta["sha256"],
                       "operand_helper_windows":helper_windows(asm,title),
                       "dispatch":handlers,
                       "strahd_operand_grammar_hypothesis":{
                           "parsed":successes,"failed":failures,
                           "opcode_counts":dict(sorted(counts.items())),
                           "branch_target_mismatch":bad_targets}}
    return report

def main():
    ap=argparse.ArgumentParser()
    ap.add_argument("--root",type=Path,required=True)
    ap.add_argument("--repo-root",type=Path,default=Path("."))
    ap.add_argument("--json",type=Path,required=True)
    args=ap.parse_args()
    data=analyze(args.root.resolve(strict=True),args.repo_root.resolve(strict=True))
    args.json.parent.mkdir(parents=True,exist_ok=True)
    args.json.write_text(json.dumps(data,indent=2)+"\n")
if __name__=="__main__":main()
