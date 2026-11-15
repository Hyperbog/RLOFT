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

# CODE.2 evidence: 0x13 and 0x1A both call the same helper (Menzo
# 0x19610, Stone 0x1D118) which advances IP across 2 operand words.
# 0x14 and 0x16 call related choice helpers; one-word consumption is a
# provisional candidate requiring further callee-level confirmation.
# Opcode 0x05 calls an input reader (Menzo 0x18AC0, Stone 0x1C6A4),
# which advances one program-word IP on every active party-slot route.
# Menzo 0x21 branches with a flag word and target word (2 total).
# Stone 0x08 handler 0x1E3CB reads condition, then EITHER writes the\n# second word as branch target (0x1E3F4), OR skips it (0x1EB7A).
# It is ALWAYS a two-word SERIALIZED instruction.
# Stone opcode 0x02 helper 0x1C4E4 advances IP twice: the second
# fetch is at [edx*2+0x2EC2E], where edx=initial IP+2.
CORRECTIONS = {
    "menzo": {3: 1, 17: 2, 18: 1, 20: 1, 24: 2, 31: 2},
    "stone": {0: 2, 3: 1, 6: 2, 17: 2, 18: 1, 20: 1, 24: 2},
}
ARITY = {title: tuple(CORRECTIONS[title].get(i, n)
                      for i,n in enumerate(widths))
         for title,widths in ARITY.items()}
EXPECTED_MINIMUM = {"menzo": (31, 31), "stone": (48, 48)}


def read_programs(path, parser):
    entries=parser["parse_res"](path)
    for i,record in enumerate(entries):
        if len(record)<2 or record[:2]==b"EH":
            continue
        count=struct.unpack_from("<H",record)[0]
        if len(record) in (2+2*count,2+4*count):
            yield i,struct.unpack_from(f"<{count}H",record,2)

def decode(words, widths, title):
    decoded=[]
    ip=0
    while ip < len(words):
        pc=ip
        encoded=words[ip]
        logical=encoded-2
        ip+=1
        if logical < 0 or logical >=len(widths):
            raise ValueError(f"bad encoded opcode {encoded:#x} at IP {pc}; previous decoded instruction starts and arities={[(p, op, len(a)) for p,op,a in decoded[-5:]]}")
        if logical in (17,24):
            # Menzo CODE.2 0x19610 and Stone 0x1D118: both read
            # two header words, then consume at most four condition/value
            # pairs. The stop-condition value is game-specific (0x21/0x26).
            if ip+2>len(words):
                raise ValueError(f"choice helper initial pair overrun at IP {pc}")
            start=ip
            ip+=2
            sentinel=0x21 if title=="menzo" else 0x26
            for pair in range(4):
                if ip+2>len(words):
                    raise ValueError(f"choice helper pair {pair} overrun at IP {pc}")
                condition=words[ip]
                ip+=2
                if condition==sentinel:
                    break
            args=words[start:ip]
        elif title=="stone" and logical==53:
            # Stone CODE.2 0x1D77C: count word; count argument words.
            if ip>=len(words):
                raise ValueError(f"Stone 0x37 count absent at IP {pc}")
            count=words[ip]
            ip+=1
            if count>len(words)-ip:
                raise ValueError(f"Stone 0x37 count overrun at IP {pc}")
            args=words[ip-1:ip+count]
            ip+=count
        elif title=="menzo" and logical==40:
            # Menzo CODE.2 0x19534: selector then up to four
            # condition/value pairs; condition 0x21 terminates the loop.
            start=ip
            if ip>=len(words):
                raise ValueError(f"Menzo 0x2A selector absent at IP {pc}")
            ip+=1
            for pair in range(4):
                if ip+1>=len(words):
                    raise ValueError(f"Menzo 0x2A pair overrun at IP {pc}")
                condition=words[ip]
                ip+=2
                if condition==0x21:
                    break
            args=words[start:ip]
        else:
            n=widths[logical]
            if ip+n>len(words):
                raise ValueError(f"opcode {encoded:#x} operands overrun at IP {pc}")
            args=words[ip:ip+n]
            ip+=n
        decoded.append((pc,logical,args))
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
                parsed=decode(words,widths,title)
                successful.append({"record":record,"words":len(words),
                                   "instructions":len(parsed)})
                for ip,opcode,args in parsed:
                    usage[opcode]+=1
                    if len(examples[opcode])<3:
                        examples[opcode].append([record,ip])
            except ValueError as ex:
                failed.append({"record":record,"reason":str(ex)})
        if len(programs) != EXPECTED_MINIMUM[title][0] or len(successful) < EXPECTED_MINIMUM[title][1]:
            raise ValueError(
                f"{title}: wordcode parser regression "
                f"{(len(programs),len(successful))} minimum {EXPECTED_MINIMUM[title]}"
            )
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
