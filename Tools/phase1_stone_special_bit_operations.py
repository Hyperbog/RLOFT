#!/usr/bin/env python3
"""Phase 1: exact original special-attack bit operations, no guessed spell names.

Verifies SHA-pinned original Menzo/Stone CODE.2 and NPCS.DAT.
Exports only machine instruction witnesses, derived effect IDs and
record-level flag counts. It does not execute the original game.
"""
from __future__ import annotations
import argparse
import collections
import json
import runpy
import struct
from pathlib import Path

BITS={
 "menzo":{
  0x04:{"call":0xF9A5,"helper":0x285EC,"effect_id":1,
        "id_push":0xF9A3,"test":0xF967,"alternative":(0xF989,0xF98B,1)},
  0x08:{"call":0xF9E9,"helper":0x285EC,"effect_id":0,
        "id_push":0xF9E7,"test":0xF9AF,"alternative":(0xF9CD,0xF9CF,0)},
 },
 "stone":{
  0x04:{"call":0x12575,"helper":0x32F70,"effect_id":1,
        "id_push":0x1256C,"test":0x1253E},
  0x80:{"call":0x125B2,"helper":0x32F70,"effect_id":5,
        "id_push":0x125B0,"test":0x12582},
  0x01:{"call":0x1260E,"helper":0x32F70,"effect_id":24,
        "id_push":0x12605,"test":0x125CF},
  0x08:{"call":0x12865,"helper":0x32F70,"effect_id":0,
        "id_push":0x12863,"test":0x12835},
 }
}
EXACT_STONE={
    "bit_02_prior_character_check":(0x12699,"call","0x3395c"),
    "bit_02_stored_status_byte":(0x126C6,"or","BYTE PTR [eax+0x15e1d],0x10"),
    "bit_20_d20_threshold":(0x12713,"call","0x11b94"),
    "bit_20_stored_status_byte":(0x12737,"or","BYTE PTR [eax+0x15e1d],0x8"),
    "bit_10_d20_threshold":(0x1275F,"call","0x11b94"),
    "bit_10_stored_status_byte":(0x12783,"or","BYTE PTR [eax+0x15e1c],0x10"),
    "bit_40_d20_threshold":(0x127DB,"call","0x11b94"),
    "bit_40_identity_call":(0x12807,"call","0x343d8"),
}

def original_cases(rows,title):
    lookup={pc:(op,args) for pc,op,args in rows}
    result={}
    for bit,case in sorted(BITS[title].items()):
        test=lookup.get(case["test"])
        push=lookup.get(case["id_push"])
        call=lookup.get(case["call"])
        if (not test or test[0] not in ("test","and") or
            not test[1].endswith(f"0x{bit:x}")):
            raise ValueError(f"{title}: flag {bit:#x} check drift: {test}")
        if push!=("push",f"0x{case['effect_id']:x}"):
            raise ValueError(f"{title}: flag {bit:#x} effect ID drift: {push}")
        if call!=("call",f"0x{case['helper']:x}"):
            raise ValueError(f"{title}: flag {bit:#x} helper drift: {call}")
        info={"encoded_flag_bit":bit,
              "effect_parameter_value":case["effect_id"],
              "callee_code2_offset":case["helper"],
              "x86_witness_pcs":[case["test"],case["id_push"],case["call"]],
              "meaning":"VERIFIED passed numeric selector; named effect UNKNOWN"}
        if "alternative" in case:
            pc,p2,selector=case["alternative"]
            if lookup.get(pc)!=("push",f"0x{selector:x}") or lookup.get(p2)!=("call","0xf850"):
                raise ValueError(f"{title} flag {bit:#x} alternate dice path drift")
            info["alternate_dice_path"]={
                "condition":"live state byte +0x0E has bit 0x02",
                "call":0xF850,"selector":selector}
        result[hex(bit)]=info
    if title=="stone":
        ops={}
        for label,(pc,op,snippet) in EXACT_STONE.items():
            got=lookup.get(pc)
            if not got or got[0]!=op or snippet not in got[1]:
                raise ValueError(f"Stone original {label} machine mismatch: {got}")
            ops[label]={"pc":pc,"asm":got[0]+" "+got[1]}
        result["additional_status_paths"]={
            "exact_machine_witnesses":ops,
            "bit_02":"tests two 16-bit character words against 55, then can OR char status +0x15E1D with 0x10",
            "bit_20":"d20 threshold + random check, then can OR char status +0x15E1D with 0x08",
            "bit_10":"d20 threshold + random check, then can OR char status +0x15E1C with 0x10",
            "bit_40":"d20 threshold + random check, then calls identity 0x343D8; later writes/effects need tracing",
            "runtime_effect_names":"UNKNOWN",
            "bit_40_instructions_after_identity":[
                {"pc":pc,"asm":op+" "+args}
                for pc,op,args in rows if 0x12807<=pc<0x12835][:32],
        }
    return result

def observed_attack_flags(path):
    buf=path.read_bytes()
    n=struct.unpack_from("<I",buf)[0]
    if len(buf)!=4+n*88:raise ValueError("NPC archive size drift")
    word_counts=collections.Counter()
    bit_counts=collections.Counter()
    active=0
    for i in range(n):
        record=buf[4+i*88:4+(i+1)*88]
        for slot in range(record[0x1D]):
            offset=0x1E+slot*9
            flag=struct.unpack_from("<H",record,offset+3)[0]
            word_counts[flag]+=1
            for bit in (1,2,4,8,16,32,64,128):
                if flag&bit:bit_counts[bit]+=1
            active+=1
    if active!=({"menzo":47,"stone":45}["menzo" if "MENZO" in str(path).upper() else "stone"]):
        # Strict title-inference isn't needed for correctness; expected count
        # is also pinned by phase1_npc_attack_descriptors.py.
        pass
    return {"active_attack_records":active,
            "u16le_flags_histogram":dict(sorted(word_counts.items())),
            "bit_counts":dict(sorted(bit_counts.items()))}

def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument("--root",type=Path,required=True)
    p.add_argument("--repo-root",type=Path,default=Path("."))
    p.add_argument("--json",type=Path,required=True)
    args=p.parse_args()
    base=runpy.run_path(str(args.repo_root/"Tools/phase1_code2_consumers.py"))
    hashcheck=runpy.run_path(str(args.repo_root/"Tools/phase1_npc_level_semantics.py"))
    output={}
    for title in ("menzo","stone"):
        cfg=base["GAMES"][title]
        directory=args.root/cfg["dir"]
        npc=directory/cfg["npc"]
        hashcheck["assert_media_file"](title+"_npc",npc)
        code,_,meta=base["le_image"](directory/cfg["exe"],cfg["expected"])
        cases=original_cases(base["disassembly"](code),title)
        corpus=observed_attack_flags(npc)
        if corpus["active_attack_records"]!={"menzo":47,"stone":45}[title]:
            raise ValueError(f"{title} attack count drift")
        output[title]={"code2_sha256":meta["sha256"],
                       "original_attack_effect_case_witnesses":cases,
                       "original_npc_aggregate_flags":corpus}
        print(title,"attack effects checked",len(BITS[title]),
              "active attack descriptors",corpus["active_attack_records"])
    args.json.parent.mkdir(parents=True,exist_ok=True)
    args.json.write_text(json.dumps(output,indent=2)+"\n",encoding="utf-8")

if __name__=="__main__":
    main()
