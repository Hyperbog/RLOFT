"""Differential CODE.2 F790 caller audit: machine bytes vs candidate JSON.

Copyright-free CI source guard runs without original game files. Supplying
CODE.2 and Code2.json additionally checks all original machine CALLs and
quantifies candidate coverage gaps. Candidate reports never override E8 bytes.
"""
from __future__ import annotations
import argparse
import hashlib
import json
import struct
from pathlib import Path

ROOT=Path(__file__).resolve().parents[2]
ORIGINAL_SHA="853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314"
# Function associations are CANDIDATES only, not decompiler authority.
FUNCTION_CALLERS={
    0x112EC:(0x1134C,),
    0x11630:(0x1173B,0x1178E),
    0x117A0:(0x1184A,0x11882,0x118FF,0x119FC),
    0x11AD8:(0x11B63,),
    0x11C3C:(0x11F47,),
    0x12724:(0x127EC,0x128A3),
    0x12988:(0x12AFA,0x12BA4),
    0x12C20:(0x12D50,0x12DFD),
    0x13118:(0x131BB,),
    None:(0x130DE,),
}
EXPECTED={at:parent for parent,calls in FUNCTION_CALLERS.items() for at in calls}
assert len(EXPECTED)==17
# Original source-bounded island: after 1309E RET+NOP, before 13118.
MISSING_ISLAND_START=0x130A0
MISSING_ISLAND_END=0x13118  # exclusive; 120 bytes

def original_check(path:Path)->None:
    raw=path.read_bytes()
    assert len(raw)==558689 and hashlib.sha256(raw).hexdigest()==ORIGINAL_SHA
    b=raw[0x3AF24:0x3AF24+0x3A852]
    actual={i for i in range(len(b)-4)
            if b[i]==0xE8 and i+5+struct.unpack_from("<i",b,i+1)[0]==0xF790}
    assert actual==set(EXPECTED),"original E8 F790 inventory changed"
    assert b[0x1309E:0x130A5]==bytes.fromhex("c3905356beffff")
    assert b[0x13115:0x1311A]==bytes.fromhex("5e5bc36a00")
    assert b[0x130DE]==0xE8
    assert 0x130DE+5+struct.unpack_from("<i",b,0x130DF)[0]==0xF790

def report_check(path:Path)->tuple[int,int]:
    doc=json.loads(path.read_text(encoding="utf-8"))
    assert doc["analysisStatus"]=="candidate-unverified"
    assert doc["provenance"]["Module"]=="CODE.2"
    assert doc["provenance"]["SourceSha256"]==ORIGINAL_SHA
    assert doc["source"]["instructionBytesIncluded"] is False
    assert doc["counts"]["instructions"]==66321 and doc["counts"]["functions"]==833
    instruction_to_blocks={}
    for block in doc["blocks"]:
        if block["Start"]["Object"]!=1:continue
        block_address=block["Start"]["Offset"]
        for addr in block["InstructionAddresses"]:
            if addr["Object"]==1 and addr["Offset"] in EXPECTED:
                instruction_to_blocks.setdefault(addr["Offset"],set()).add(block_address)
    block_to_functions={}
    for f in doc["functionCandidates"]:
        if f["Entry"]["Object"]!=1:continue
        entry=f["Entry"]["Offset"]
        for block in f["Blocks"]:
            if block["Object"]==1:
                block_to_functions.setdefault(block["Offset"],set()).add(entry)
    matched=0
    missing=[]
    for at,parent in sorted(EXPECTED.items()):
        candidates=set()
        for block in instruction_to_blocks.get(at,()):
            candidates.update(block_to_functions.get(block,()))
        if parent is None:
            assert not candidates,(hex(at),candidates)
            missing.append(at)
        else:
            assert candidates=={parent},(hex(at),candidates,hex(parent))
            matched+=1
    assert missing==[0x130DE]
    # Confirm the analyzer omits the entire original 120-byte
    # selection routine, not merely a function parent at 130DE.
    assert not any(i["Address"]["Object"]==1 and
                   MISSING_ISLAND_START<=i["Address"]["Offset"]<MISSING_ISLAND_END
                   for i in doc["instructions"]), "source island now has instructions"
    assert not any(block["Start"]["Object"]==1 and
                   MISSING_ISLAND_START<=block["Start"]["Offset"]<MISSING_ISLAND_END
                   for block in doc["blocks"]), "source island now has candidate blocks"
    return matched,len(missing)

def source_check()->None:
    catalog=(ROOT/"DemoV1/include/rloft/original_f790_caller_catalog.hpp").read_text()
    model=(ROOT/"DemoV1/include/rloft/original_f790_feedback_edges.hpp").read_text()
    cmake=(ROOT/"DemoV1/CMakeLists.txt").read_text()
    host=(ROOT/"DemoV1/src/win32_main.cpp").read_text()
    for call in EXPECTED:
        assert f"0x{call:05x}U" in catalog,hex(call)
    for edge in (0x1173B,0x1178E,0x131BB):
        assert f"0x{edge:05x}U" in model,hex(edge)
    assert "original_f790_candidate_coverage_tests" in cmake
    assert "OriginalF790FeedbackEdges" not in host
    print("PASS: source F790 typed catalog, 17 callers, no guessed host hookup")

def main()->None:
    ap=argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--original-code2",type=Path)
    ap.add_argument("--code2-report",type=Path)
    ap.add_argument("--binary-only",action="store_true")
    ap.add_argument("--json",action="store_true")
    args=ap.parse_args()
    if not args.binary_only:source_check()
    if args.original_code2:original_check(args.original_code2)
    if args.code2_report:
        matched,gaps=report_check(args.code2_report)
        summary={"original_e8_f790_calls":17,"candidate_matched":matched,
                 "candidate_missing":["0x130de"],
                 "candidate_status":"candidate-unverified",
                 "uncovered_original_island":{
                     "entry":"0x130a0","end_exclusive":"0x13118",
                     "bytes":120,"f790_call":"0x130de",
                     "original_source_verified":bool(args.original_code2),
                     "candidate_instructions":0,"candidate_blocks":0,
                     "caller_into_island":"not-proven"}}
        if args.json:print(json.dumps(summary,sort_keys=True))
        else:print("PASS: 17 original F790 E8; 16 candidate parents; gap 130DE")
    if args.binary_only and not(args.original_code2 and args.code2_report):
        ap.error("--binary-only requires --original-code2 and --code2-report")

if __name__=="__main__":
    main()
