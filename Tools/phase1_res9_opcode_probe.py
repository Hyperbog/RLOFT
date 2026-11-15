#!/usr/bin/env python3
"""Read-only Phase 1 RES9 VM dispatcher and grammar lead finder.

Locates all indirect x86 branch tables in authenticated original CODE.2 LE
code objects, ranks likely wordcode dispatchers and reports bounded machine
instruction evidence. It does not invent opcode arities or emit game assets.
"""
from __future__ import annotations

import argparse
import collections
import json
import re
import runpy
import struct
from pathlib import Path

SELECTOR = re.compile(r"\[([^]]+\*4[^]]*)\]")
HEX = re.compile(r"0x([0-9a-f]+)")
TITLE_TARGETS = {"menzo": (0x29, 0x30), "stone": (0x29, 0x30)}


def classify_branch(rows, i, code, data):
    pc, op, operand = rows[i]
    if op not in ("jmp", "call") or "DWORD PTR" not in operand or "*4" not in operand:
        return None
    group = SELECTOR.search(operand)
    if not group:
        return None
    terms = group.group(1)
    table_candidates = [int(x, 16) for x in HEX.findall(terms)]
    if not table_candidates:
        return None
    table = table_candidates[-1]
    # LE object pages may contain code and data. Candidate table could be in
    # either; never assume an address maps to one object without checking.
    scan = []
    for obj_name, obj in (("code", code), ("data", data)):
        if table + 4 * 8 > len(obj):
            continue
        targets = []
        for j in range(min(128, (len(obj) - table) // 4)):
            target = struct.unpack_from("<I", obj, table + 4 * j)[0]
            if target >= len(code):
                break
            targets.append(target)
        if len(targets) >= 8:
            scan.append({"object": obj_name, "targets": targets[:80],
                         "candidate_length": len(targets)})
    window = rows[max(0, i - 55):i]
    cmp_checks = []
    for offset, opcode, args in window:
        if opcode in ("cmp", "sub", "add") and "0x" in args:
            nums = [int(v,16) for v in HEX.findall(args)]
            if nums and any(12 <= v <= 160 for v in nums):
                cmp_checks.append({"pc": offset, "asm": opcode + " " + args})
    word_fetch = [
        {"pc":p, "asm":mn+" "+a}
        for p,mn,a in window
        if mn in ("mov", "movzx", "movsx") and "WORD PTR" in a
    ]
    score = 0
    if op == "jmp":
        score += 2
    if len(scan):
        score += 4
    if cmp_checks:
        score += 3
    if word_fetch:
        score += 3
    if any(x["candidate_length"] >= 40 for x in scan):
        score += 9
    if any("0x29" in v["asm"] or "0x2a" in v["asm"] or
           "0x30" in v["asm"] or "0x37" in v["asm"] for v in cmp_checks):
        score += 7
    return {"branch":pc,"instruction":op+" "+operand, "table":table,
            "score":score, "table_candidates":scan,
            "checks":cmp_checks[-15:],
            "word_fetch":word_fetch[-10:],
            "nearby": [
                {"pc":p, "asm":mn+" "+a}
                for p,mn,a in rows[max(0,i-16):min(len(rows),i+9)]
            ]}


def near_immediate(rows, start, end, max_results=30):
    out = []
    for i,(pc,op,args) in enumerate(rows):
        if pc < start or pc > end:
            continue
        out.append({"pc":pc,"asm":op+" "+args})
        if len(out) >= max_results:
            break
    return out


def raw_programs(path, parse_res):
    entries = parse_res(path)
    raw = []
    for i,entry in enumerate(entries):
        if not entry or entry[:2] == b"EH":
            continue
        if len(entry) < 2:
            continue
        wc = struct.unpack_from("<H",entry)[0]
        kind = ("two_bank" if len(entry)==2+4*wc else
                "one_bank" if len(entry)==2+2*wc else "other")
        raw.append({"archive_index":i,"word_count":wc,
                    "container_shape":kind})
    return raw


def audit(root, repo):
    consumers=runpy.run_path(str(repo/"Tools/phase1_code2_consumers.py"))
    archive=runpy.run_path(str(repo/"Resources/RES/analyze_legacy_archives.py"))
    out={}
    for title in ("menzo","stone"):
        cfg=consumers["GAMES"][title]
        folder=root/cfg["dir"]
        code,data,info=consumers["le_image"](folder/cfg["exe"],cfg["expected"])
        rows=consumers["disassembly"](code)
        leads=[]
        for i in range(len(rows)):
            candidate=classify_branch(rows,i,code,data)
            if candidate:
                leads.append(candidate)
        leads.sort(key=lambda a:(-a["score"],-a["table"],a["branch"]))
        res9 = (folder/"CD_Extracted/RES9" if title=="menzo"
                else folder/"PROPHET/RES9")
        prog=raw_programs(res9,archive["parse_res"])
        out[title]={
            "original_sha256":info["sha256"],
            "indirect_table_candidate_count":len(leads),
            "highest_ranked_candidates":leads[:35],
            "all_candidates_index":[{"pc":v["branch"],"table":v["table"],
                                     "score":v["score"],
                                     "possible_lengths":[t["candidate_length"] for t in v["table_candidates"]]}
                                    for v in leads],
            "res9_raw_programs":prog,
        }
        print(f"{title}: {len(leads)} indirect branch candidates")
        for item in leads[:22]:
            print(f"  {item['branch']:#07x}: table={item['table']:#x} "
                  f"score={item['score']} "
                  f"lengths={[x['candidate_length'] for x in item['table_candidates']]} "
                  f"cmp={[v['asm'] for v in item['checks'][-3:]]}")
    return out


def main():
    parser=argparse.ArgumentParser()
    parser.add_argument("--root",type=Path,required=True)
    parser.add_argument("--repo-root",type=Path,default=Path("."))
    parser.add_argument("--json",type=Path,required=True)
    args=parser.parse_args()
    result=audit(args.root.resolve(strict=True),
                 args.repo_root.resolve(strict=True))
    args.json.parent.mkdir(parents=True,exist_ok=True)
    args.json.write_text(json.dumps(result,indent=2)+"\n",encoding="utf-8")


if __name__=="__main__":
    main()
