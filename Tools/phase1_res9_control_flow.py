#!/usr/bin/env python3
"""Phase 1: auditable RES9 control-flow destination checks.

Uses authenticated CODE.2 dispatcher and physically decoded original RES9
programs. Current opcode arities are explicitly provisional: the analyzer
reports coverage, not VM completion, and never writes game-data payloads.
"""
from __future__ import annotations
import argparse
import collections
import json
import runpy
from pathlib import Path

# Direct target fields proved by original CODE.2 cursor assignments.
# Keys are ENCODED opcode words, values are offsets into parsed operands.
# Deferred/UI choice and helper-resolved branches are intentionally excluded.
TARGET_WORDS = {
    "menzo": {
        0x08:(1,), 0x09:(1,), 0x0D:(1,), 0x0F:(1,),
        0x12:(0,), 0x18:(0,), 0x21:(1,), 0x22:(1,),
    },
    "stone": {
        0x09:(1,), 0x0D:(1,), 0x0F:(1,),
        0x12:(0,), 0x18:(0,), 0x21:(1,), 0x22:(1,),
        0x2F:(2,), 0x33:(0,), 0x34:(0,), 0x35:(0,),
        0x36:(1,),
    },
}
# Opcode 0x0F may use 0xFFFF as the no-timer sentinel in Strahd;
# do not assume it is supported in newer games without code proof.
OPTIONAL_SENTINEL = {0x0F:0xFFFF}


def examine_program(index, words, title, correlate):
    try:
        decoded=correlate["decode"](words, correlate["ARITY"][title], title)
    except ValueError as ex:
        return {"index":index,"decoded":False,"reason":str(ex)}
    starts={ip for ip,op,args in decoded}
    checked=[]
    malformed=[]
    for ip,logical,args in decoded:
        encoded=logical+2
        offsets=TARGET_WORDS[title].get(encoded,())
        for operand in offsets:
            if operand>=len(args):
                malformed.append({"source_word":ip,"encoded_opcode":encoded,
                                  "expected_target_argument":operand,
                                  "available_args":len(args)})
                continue
            target=int(args[operand])
            if encoded in OPTIONAL_SENTINEL and target==OPTIONAL_SENTINEL[encoded]:
                status="unknown_sentinel_semantics"
            elif target in starts:
                status="decoded_start"
            elif target==len(words):
                status="one_past_program"
            else:
                status="not_decoded_start"
            checked.append({"source_word":ip,"encoded_opcode":encoded,
                            "arg_index":operand,"target_word":target,
                            "result":status})
    terminal=[ip for ip,opcode,args in decoded if opcode+2==0x1B]
    return {
        "index":index,"decoded":True,"instruction_count":len(decoded),
        "terminal_opcode_offsets":terminal[:20],
        "checked_branch_targets":checked,"missing_branch_operands":malformed,
    }


def audit(root,repo):
    correlate=runpy.run_path(str(repo/"Tools/phase1_res9_arity_correlate.py"))
    handler=runpy.run_path(str(repo/"Tools/phase1_res9_handler_map.py"))
    reader=runpy.run_path(str(repo/"Tools/phase1_code2_consumers.py"))
    res=runpy.run_path(str(repo/"Resources/RES/analyze_legacy_archives.py"))
    report={}
    for title in ("menzo","stone"):
        folder=root/reader["GAMES"][title]["dir"]
        spec=handler["VM"][title]
        specimens=list(correlate["read_programs"](folder/spec["archive"],res))
        results=[examine_program(i,words,title,correlate)
                 for i,words in specimens]
        checked=[p for p in results if p["decoded"]]
        unresolved=[p for p in results if not p["decoded"]]
        counts=collections.Counter()
        opstats=collections.Counter()
        samples=collections.defaultdict(list)
        for p in checked:
            for row in p["checked_branch_targets"]:
                counts[row["result"]]+=1
                opstats[row["encoded_opcode"]]+=1
                if len(samples[row["result"]])<18:
                    samples[row["result"]].append({
                        "record":p["index"],**row})
        # Structural coverage regression is explicit; unresolved Stone
        # branch targets remain visible rather than counted as successes.
        if title == "menzo" and (len(checked)!=31 or counts["not_decoded_start"]):
            raise ValueError("Menzo RES9 full-corpus/branch-target regression")
        if title == "stone" and len(checked)<44:
            raise ValueError("Stone RES9 candidate coverage regressed")
        report[title]={
            "programs_present":len(results),
            "completely_linearly_consumed":len(checked),
            "unresolved_programs":len(unresolved),
            "target_source_rule":"CODE.2 direct cursor and timer operand candidates",
            "encoded_opcodes_with_direct_targets":list(TARGET_WORDS[title]),
            "source_operations_count":sum(opstats.values()),
            "per_opcode_target_observations":dict(sorted(opstats.items())),
            "target_classification":dict(sorted(counts.items())),
            "target_examples":dict(samples),
            "missing_target_argument_count":sum(
                len(p["missing_branch_operands"]) for p in checked),
            "candidate_instruction_boundaries_only":True,
            "unresolved_program_ids":[p["index"] for p in unresolved],
        }
        print(title,"programs",len(results),"linear",len(checked),
              "target checks",dict(sorted(counts.items())),
              "missing operand",report[title]["missing_target_argument_count"])
    return report


def main():
    ap=argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--root",type=Path,required=True)
    ap.add_argument("--repo-root",type=Path,default=Path("."))
    ap.add_argument("--json",type=Path,required=True)
    a=ap.parse_args()
    output=audit(a.root.resolve(strict=True),a.repo_root.resolve(strict=True))
    a.json.parent.mkdir(parents=True,exist_ok=True)
    a.json.write_text(json.dumps(output,indent=2)+"\n",encoding="utf-8")


if __name__=="__main__":
    main()
