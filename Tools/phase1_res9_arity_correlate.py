#!/usr/bin/env python3
"""Phase 1: test CODE.2-derived fixed RES9 opcode operand footprints.

All lengths in this evidence-driven candidate map are word counts, not bytes.
A passing corpus test is structural corroboration; behavioral labels remain
UNKNOWN unless the original consumers have been independently reconstructed.
"""
from __future__ import annotations
import argparse
import collections
import json
import runpy
import struct
from pathlib import Path

# Indexed by decoded dispatcher case (encoded opcode - 2).
# Provisional until complete control-flow and helper readsites are certified.
ARITY = {
 "menzo": (
    2,2,0,0,1,0,2,2,1,1,1,2,0,3,3,0,1,0,0,0,0,1,1,1,0,0,
    1,1,1,1,1,1,2,4,1,3,3,1,1,1,0,3,0
 ),
 "stone": (
    1,2,0,0,1,0,2,2,1,1,1,2,0,3,3,0,1,0,0,0,0,1,1,1,0,0,
    1,1,1,1,1,2,2,4,1,3,3,1,1,1,0,3,0,0,3,3,1,1,1,1,1,1,2,0
 ),
}

def read_programs(path, parser):
    entries=parser["parse_res"](path)
    for i,record in enumerate(entries):
        if len(record)<2 or record[:2]==b"EH":
            continue
        count=struct.unpack_from("<H",record)[0]
        if len(record) in (2+2*count,2+4*count):
            yield i,struct.unpack_from(f"<{count}H",record,2)

def decode(words, widths):
    decoded=[]
    ip=0
    while ip < len(words):
        pc=ip
        encoded=words[ip]
        logical=encoded-2
        ip+=1
        if logical < 0 or logical >=len(widths):
            raise ValueError(f"bad encoded opcode {encoded:#x} at IP {pc}; previous decoded instruction starts and arities={[(p, op, len(a)) for p,op,a in decoded[-5:]]}")
        n=widths[logical]
        if ip+n>len(words):
            raise ValueError(f"opcode {encoded:#x} operands overrun at IP {pc}")
        args=words[ip:ip+n]
        decoded.append((pc,logical,args))
        ip+=n
    return decoded

def audit(root, repo):
    code=runpy.run_path(str(repo/"Tools/phase1_res9_handler_map.py"))
    read=runpy.run_path(str(repo/"Tools/phase1_code2_consumers.py"))
    archive=runpy.run_path(str(repo/"Resources/RES/analyze_legacy_archives.py"))
    out={}
    for title,widths in ARITY.items():
        cfg=read["GAMES"][title]
        folder=root/cfg["dir"]
        spec=code["VM"][title]
        if len(widths)!=spec["count"]:
            raise ValueError(f"{title} arity count != actual original dispatch table")
        programs=list(read_programs(folder/spec["archive"],archive))
        failed=[]
        successful=[]
        usage=collections.Counter()
        examples=collections.defaultdict(list)
        for record,words in programs:
            try:
                parsed=decode(words,widths)
                successful.append({"record":record,"words":len(words),
                                   "instructions":len(parsed)})
                for ip,opcode,args in parsed:
                    usage[opcode]+=1
                    if len(examples[opcode])<3:
                        examples[opcode].append([record,ip])
            except ValueError as ex:
                failed.append({"record":record,"reason":str(ex)})
        out[title]={
            "candidate_not_executable_certified":True,
            "declared_opcodes":len(widths),
            "opcode_word_arity":dict(enumerate(widths)),
            "programs_tested":len(programs),
            "completely_parsed":len(successful),
            "decoded_programs":successful,
            "first_failures":failed[:60],
            "usage":dict(sorted(usage.items())),
            "instruction_start_samples":dict(sorted(examples.items())),
        }
        print(f"{title}: candidate arities consume {len(successful)}/"
              f"{len(programs)} programs; first differences {failed[:8]}")
        print(f"  observed cases: {sorted(usage)}")
    return out

def main():
    p=argparse.ArgumentParser()
    p.add_argument("--root",type=Path,required=True)
    p.add_argument("--repo-root",type=Path,default=Path("."))
    p.add_argument("--json",type=Path,required=True)
    a=p.parse_args()
    result=audit(a.root.resolve(strict=True),a.repo_root.resolve(strict=True))
    a.json.parent.mkdir(parents=True,exist_ok=True)
    a.json.write_text(json.dumps(result,indent=2)+"\n")

if __name__=="__main__":
    main()
