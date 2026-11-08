#!/usr/bin/env python3
"""Independent DOSDis JSON feedback backed by supplied original CODE.1/.2 bytes.

Use only authorized local files:
 python DemoV1/tools/audit_disassembler_json_bytes.py \
   --code1 CODE.1 --json1 Code1.json --code2 CODE.2 --json2 Code2.json --json

This is an analyst-only tool; it never edits binaries, dispatches CI or
reclassifies conservative candidate code as proved runtime execution.
"""
from __future__ import annotations

import argparse
import hashlib
import json
import re
import struct
from pathlib import Path

SOURCES={
 "CODE.1":(277787,"fd9b709ceee190e6a70870f9012ebea3bc437cae6bc74e0150d6f922fa750fed",0x2C724,0x11302),
 "CODE.2":(558689,"853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314",0x3AF24,0x3A852),
}
CALL=re.compile(r"call ([0-9a-f]+)h",re.I)
F790_CALLS={0x1134C,0x1173B,0x1178E,0x1184A,0x11882,0x118FF,
            0x119FC,0x11B63,0x11F47,0x127EC,0x128A3,0x12AFA,
            0x12BA4,0x12D50,0x12DFD,0x130DE,0x131BB}
F790_TARGETS=(0xFA85,)*3+(0xFB3D,)*4+(0xF8D4,)*3

def verify_pair(module:str,source:Path,report:Path)->dict:
    length,sha,base,live=SOURCES[module]
    binary=source.read_bytes()
    if len(binary)!=length or hashlib.sha256(binary).hexdigest()!=sha:
        raise ValueError(f"wrong original binary: {module}")
    obj=binary[base:base+live]
    doc=json.loads(report.read_text(encoding="utf-8"))
    if doc["analysisStatus"]!="candidate-unverified" or \
       doc["provenance"]["SourceSha256"]!=sha or \
       doc["provenance"]["Module"]!=module or \
       doc["source"]["instructionBytesIncluded"] is not False:
        raise ValueError(f"report does not match verified original {module}")
    instructions=sorted(doc["instructions"],
         key=lambda item:(item["Address"]["Object"],item["Address"]["Offset"]))
    starts=set()
    previous=None
    direct=0
    for item in instructions:
        address=item["Address"]
        key=(address["Object"],address["Offset"])
        n=item["Length"]
        if key in starts or not 1<=n<=15:
            raise ValueError(f"duplicate/bad instruction {key}")
        starts.add(key)
        if previous and previous[0]==key[0] and previous[1]+previous[2]>key[1]:
            raise ValueError(f"overlap at {key}")
        previous=(key[0],key[1],n)
        if item["FlowControl"]=="Call":
            at=key[1]
            if key[0]!=1 or n!=5 or obj[at]!=0xE8:
                raise ValueError(f"non-E8 rel32 near call {key}")
            target=at+5+struct.unpack_from("<i",obj,at+1)[0]
            match=CALL.fullmatch(item["Assembly"])
            if not match or int(match.group(1),16)!=target:
                raise ValueError(f"near-call original bytes mismatch at {key}")
            direct+=1
    if doc["counts"]["instructions"]!=len(instructions):
        raise ValueError("report instruction count mismatch")
    unknown=[entry for entry in doc["classifiedRanges"]
             if entry["Object"]==1 and
                entry["Classification"]=="unknown-file-backed"]
    unknown_lengths=[entry["EndExclusive"]-entry["Begin"] for entry in unknown]
    ledger=doc["gapAnalysis"]
    if len(unknown)!=ledger["TotalUnknownSpans"] or \
       sum(unknown_lengths)!=ledger["UnknownFileBackedBytes"]:
        raise ValueError("source gap ledger totals mismatch")
    result={
      "module":module,
      "snapshot_generated_utc":doc["generatedUtc"],
      "near_e8_calls_verified":direct,
      "duplicate_or_overlapping_starts":0,
      "unknown_bytes":sum(unknown_lengths),
      "unknown_spans":len(unknown),
      "short_unknown_spans_64_to_255":sum(64<=x<256 for x in unknown_lengths),
      "ranked_gap_minimum":ledger["MinimumGapBytes"],
      "ranked_gap_count":len(ledger["Gaps"]),
    }
    if module=="CODE.2":
        calls={at for at in range(len(obj)-4) if obj[at]==0xE8 and
               at+5+struct.unpack_from("<i",obj,at+1)[0]==0xF790}
        if calls!=F790_CALLS:
            raise ValueError("original CODE.2 F790 callsite set mismatch")
        missing=next((entry for entry in unknown
                     if entry["Begin"]<=0x130A0<entry["EndExclusive"]),None)
        if (1,0x130DE) not in starts and missing is None:
            raise ValueError("unexplained CODE.2 130DE candidate classification")
        if obj[0x130DE:0x130E3]!=bytes.fromhex("e8adc6ffff"):
            raise ValueError("original selected poll E8 bytes changed")
        if obj[0xF8BF:0xF8CC]!=bytes.fromhex("2c023c090f87740200000fb6c0"):
            raise ValueError("source eight-bit bounded F790 dispatch changed")
        if struct.unpack_from("<10I",obj,0xF768)!=F790_TARGETS:
            raise ValueError("source F790 ten-entry jump table changed")
        table=next((t for t in doc["jumpTableCandidates"]
                    if t["Source"]=={"Object":1,"Offset":0xF8CC} and
                    t["Table"]=={"Object":1,"Offset":0xF768}),None)
        if table is None or tuple(x["Offset"] for x in table["CandidateTargets"])!=F790_TARGETS:
            raise ValueError("candidate F790 table targets mismatch source")
        inbound=sum(1 for at in range(len(obj)-4) if obj[at]==0xE8 and
               at+5+struct.unpack_from("<i",obj,at+1)[0]==0x130A0)
        result["f790"]={
          "original_direct_call_count":17,
          "selected_call_decoded":(1,0x130DE) in starts,
          "unknown_range_containing_130a0":(
             [f"0x{missing['Begin']:x}",f"0x{missing['EndExclusive']:x}"]
             if missing else None),
          "direct_e8_inbound_to_130a0":inbound,
          "source_table": "1:0xf768",
          "source_table_guard":"byte SUB 2; CMP 9; unsigned JA; MOVZX",
          "report_index_evidence":table["Evidence"],
          "source_routine_execution_proven":False,
        }
    return result

def main()->None:
    ap=argparse.ArgumentParser(description=__doc__)
    for n in ("1","2"):
        ap.add_argument(f"--code{n}",type=Path)
        ap.add_argument(f"--json{n}",type=Path)
    ap.add_argument("--json",action="store_true")
    ap.add_argument("--out",type=Path)
    args=ap.parse_args()
    results=[]
    for n in ("1","2"):
        source=getattr(args,f"code{n}")
        report=getattr(args,f"json{n}")
        if (source is None)!=(report is None):
            ap.error(f"--code{n} and --json{n} must be paired")
        if source is not None:
            results.append(verify_pair(f"CODE.{n}",source,report))
    if not results: ap.error("supply at least one original CODE/report pair")
    payload={"schema":"rloft-own-disassembler-json-feedback-v1",
             "reports":results,
             "original_source_bytes_included":False,
             "original_gameplay_reachability_proven":False,
             "manual_windows_build_started":False}
    message=json.dumps(payload,indent=2,sort_keys=True)
    if args.out:args.out.write_text(message+"\n",encoding="utf-8")
    if args.json:print(message)
    else:
        for x in results:
            print(f"PASS {x['module']}: {x['near_e8_calls_verified']} original near E8 "
                  f"targets, {x['unknown_spans']} unknown spans")
        print("Candidate-gap feedback only; no build, no original DOS acceptance.")

if __name__=="__main__":
    main()
