#!/usr/bin/env python3
"""Executable-backed Phase 1 attack-selector and tile table mutation checks.

Verifies exact original CODE.2 readsites and aggregate original data.
Unresolved effects remain labeled UNKNOWN. Does not alter original assets.
"""
from __future__ import annotations

import argparse
import collections
import json
import runpy
import struct
from pathlib import Path

CODE_WITNESSES={
    "menzo":{
        "attack_control":{
            0x1FF30:("test","BYTE PTR [edi+0x21],0xc"),
            0x1FF41:("test","BYTE PTR [edi+0x2a],0xc"),
            0x2004E:("test","BYTE PTR [edi+0x21],0xc"),
            0x20054:("mov","ecx,0xb0000"),
            0x20059:("test","BYTE PTR [edi+0x2a],0xc"),
            0x2005F:("mov","DWORD PTR [ebp-0xc],0xb0000"),
        },
        "attack_bit_effects":{
            0xF967:("test","BYTE PTR [ebp-0x4],0x4"),
            0xF9AF:("test","BYTE PTR [ebp-0x4],0x8"),
            0xF98B:("call","0xf850"),
            0xF9A5:("call","0x285ec"),
            0xF9CF:("call","0xf850"),
            0xF9E9:("call","0x285ec"),
        },
        "post_filter_dispatch":{
            0x27576:("mov","BYTE PTR [eax]"),
            0x2757A:("cmp","dl,0x9"),
            0x27586:("jmp","DWORD PTR cs:[edx*4+0x27300]"),
            0x275B7:("call","0x25d34"),
            0x275BF:("test","eax,eax"),
            0x275D9:("call","0x25b5c"),
            0x275E1:("test","eax,eax"),
            0x275EE:("cmp","BYTE PTR [eax+0x9],0x14"),
            0x275F6:("call","0x1ab28"),
        },
        "mutable_tile_words":{
            0x2B352:("mov","WORD PTR [edx*2+0x25000]"),
            0x2B35A:("and","dl,0x3f"),
            0x2B362:("cmp","edx,0x33"),
            0x2B36B:("cmp","edx,0x3e"),
            0x2B384:("shl","edx,0x5"),
            0x2B398:("cmp","WORD PTR [ebx],0x5a"),
            0x2B3A1:("mov","DWORD PTR [ebp-0x4],0x5"),
            0x2B3AF:("idiv","DWORD PTR [ebp-0x4]"),
            0x2B3B2:("sub","ecx,edx"),
            0x2B3B4:("lea","[ecx+0x4]"),
            0x2B3B7:("mov","WORD PTR [ebx],dx"),
            0x2B3BB:("cmp","edi,0x10"),
        },
    },
    "stone":{
        "attack_control":{
            0x275F4:("cmp","eax,0x2"),
            0x275F9:("mov","WORD PTR [edi+0x2a]"),
            0x275FF:("and","al,0x8d"),
            0x27604:("test","eax,eax"),
            0x27608:("test","BYTE PTR [ebx+0xe],0x10"),
            0x2760E:("test","BYTE PTR [ebx+0xe],0x1"),
            0x27625:("call","0x431c6"),
            0x27630:("cmp","eax,0x14"),
            0x27638:("call","0x431c6"),
            0x27643:("cmp","eax,0x46"),
        },
        "attack_bit_effects":{
            0x1253E:("and","al,0x4"),
            0x12582:("and","al,0x80"),
            0x125CF:("and","al,0x1"),
            0x12658:("and","al,0x2"),
            0x126FF:("and","al,0x20"),
            0x1274D:("and","al,0x10"),
            0x127CD:("and","al,0x40"),
            0x12835:("and","al,0x8"),
            0x12575:("call","0x32f70"),
            0x125B2:("call","0x32f70"),
            0x1260E:("call","0x32f70"),
            0x12699:("call","0x3395c"),
            0x12713:("call","0x11b94"),
            0x1275F:("call","0x11b94"),
            0x127DB:("call","0x11b94"),
            0x12807:("call","0x343d8"),
            0x12865:("call","0x32f70"),
        },
        "post_filter_dispatch":{
            0x308BB:("mov","BYTE PTR [eax]"),
            0x308BF:("cmp","dl,0xa"),
            0x308CB:("jmp","DWORD PTR cs:[edx*4+0x3064c]"),
            0x308FC:("call","0x2ea34"),
            0x30904:("test","eax,eax"),
            0x3091E:("call","0x2e72c"),
            0x30926:("test","eax,eax"),
            0x30933:("movzx","BYTE PTR [eax+0x9]"),
            0x3093C:("sub","eax,0x1e"),
        },
    },
}
SOURCES={
    "menzo":{"npc":"NPCS.DAT","level":"LEVELS.DAT"},
    "stone":{"npc":"CD_Extracted/NPCS.DAT",
             "level":"CD_Extracted/LEVELS.DAT"},
}

def machine_check(rows,game):
    pcmap={pc:(op,arg) for pc,op,arg in rows}
    findings={}
    for label,matches in CODE_WITNESSES[game].items():
        proof=[]
        for pc,(op,snippet) in matches.items():
            instr=pcmap.get(pc)
            if not instr or instr[0]!=op or snippet not in instr[1]:
                raise ValueError(f"{game}: {label} @{pc:#x} "
                                 f"expected {op} {snippet}; got {instr}")
            proof.append({"pc":pc,"asm":instr[0]+" "+instr[1]})
        findings[label]=proof
    if game=="stone":
        # Both live states 1 and 2 enter 0x27C89. No instruction
        # branches around the period-loaded IDIV at 0x27CA8.
        near=[(pc,op,args) for pc,op,args in rows
              if 0x27c9e<=pc<=0x27ca8]
        if (not near or near[0][0]!=0x27c9e
            or near[-1][0]!=0x27ca8
            or any(op.startswith("j") for _,op,args in near)):
            raise ValueError("Stone divisor read/IDIV straight-line path changed")
        findings["zero_division_local_guard_check"]={
            "period_load_pc":0x27c9e,
            "divide_pc":0x27ca8,
            "local_conditional_jump_between":False,
            "runtime_reachability":"UNKNOWN; nonzero period invariant requires state producer proof"}
    return findings


def attack_gate_consistency(npc_path):
    data=npc_path.read_bytes()
    count=struct.unpack_from("<I",data)[0]
    if len(data)!=4+count*88:
        raise ValueError("NPCS media count/extent mismatched")
    per_slot=collections.Counter()
    descriptor_flags=collections.Counter()
    nonzero_attacks=0
    invalid_divisors=[]
    for record in range(count):
        body=data[4+record*88:4+(record+1)*88]
        n=body[0x1D]
        if n>2:raise ValueError(f"invalid attack count record {record}")
        for slot in range(n):
            d=body[0x1E+9*slot:0x27+9*slot]
            dice,sides,bonus,flags,pad,period,pad2,raw7,phase=d
            descriptor_flags[flags]+=1
            is_zero=not any(d)
            key=("zero_record" if is_zero else "populated",
                 "period_zero" if period==0 else "period_positive",
                 "dice_zero" if dice==0 else "dice_positive")
            per_slot[key]+=1
            nonzero_attacks+=not is_zero
            if period==0:
                name=body[:20].split(b"\0",1)[0].decode("latin1",errors="replace")
                invalid_divisors.append({"npc_index":record,
                                         "npc_name":name,
                                         "attack_slot":slot,
                                         "record_is_all_zero":is_zero,
                                         "control_word":struct.unpack_from("<H",d,3)[0],
                                         "distance_word":struct.unpack_from("<H",d,6)[0],
                                         "trigger_phase":d[8]})
    return {
        "active_slots_in_records":sum(per_slot.values()),
        "nonzero_attack_descriptors":nonzero_attacks,
        "active_descriptors_by_structure":{
            "|".join(k):v for k,v in sorted(per_slot.items())},
        "active_attack_control_byte_frequency":
            dict(sorted(descriptor_flags.items())),
        "zero_divisor_declared_slot_examples":invalid_divisors,
        "zero_divisor_semantics":"UNKNOWN: no zero-divide execution inferred",
    }


def menzo_archive_mutation_scope(path):
    data=path.read_bytes()
    count=struct.unpack_from("<I",data)[0]
    start=struct.unpack_from(f"<{count}I",data,4)
    candidate=0
    changed_level_count=0
    per_level=[]
    for level,(a,b) in enumerate(zip(start,(*start[1:],len(data)))):
        entry=data[a:b]
        table=struct.unpack_from("<1024H",entry,0x1D+0x0A)
        affected=[]
        # Original loop 0x2B362..0x2B3BB covers code groups 0x33..0x3E
        # and all 16 words, gated on mutable runtime conditions.
        for group in range(0x33,0x3F):
            for word in range(16):
                old=table[group*16+word]
                if old>=0x5A:
                    transformed=old-(old%5)+4
                    affected.append((group,word,transformed!=old))
        n=sum(is_changed for g,w,is_changed in affected)
        candidate+=n
        changed_level_count+=bool(n)
        if level<4:
            per_level.append({"level":level,
                              "candidate_words_changed_under_gate":n,
                              "candidate_groups":len(set(g for g,w,c in affected))})
    return {
        "source_level_count":count,
        "candidate_archive_words_that_would_change_if_runtime_gate_true":candidate,
        "levels_with_potentially_changed_words":changed_level_count,
        "samples":per_level,
        "original_code_gating":["tile_code>=0x33 and <=0x3E",
                                "additional runtime-state thresholds",
                                "table_value>=0x5A"],
        "transformation":"new=u16(old - (old modulo 5) + 4)",
        "interpretation":"VERIFIED original instruction math, HYPOTHETICAL execution on pristine archive",
    }


def main():
    parser=argparse.ArgumentParser()
    parser.add_argument("--root",type=Path,required=True)
    parser.add_argument("--repo-root",type=Path,default=Path("."))
    parser.add_argument("--json",type=Path,required=True)
    a=parser.parse_args()
    parser_file=runpy.run_path(str(a.repo_root/"Tools/phase1_code2_consumers.py"))
    hashes=runpy.run_path(str(a.repo_root/"Tools/phase1_npc_level_semantics.py"))
    result={}
    for game,paths in SOURCES.items():
        original=parser_file["GAMES"][game]
        base=a.root/original["dir"]
        npc=base/paths["npc"]
        level=base/paths["level"]
        hashes["assert_media_file"](game+"_npc",npc)
        hashes["assert_media_file"](game+"_levels",level)
        code,_,meta=parser_file["le_image"](base/original["exe"],
                                            original["expected"])
        rows=parser_file["disassembly"](code)
        result[game]={
            "code2_sha256":meta["sha256"],
            "attack_control_and_dispatch_witnesses":machine_check(rows,game),
            "attack_descriptor_zero_divisor_check":attack_gate_consistency(npc),
        }
        expected={
            "menzo":{"active":47,"zero_divisors":0},
            "stone":{"active":45,"zero_divisors":5},
        }[game]
        summary=result[game]["attack_descriptor_zero_divisor_check"]
        if (summary["active_slots_in_records"]!=expected["active"]
            or len(summary["zero_divisor_declared_slot_examples"])!=expected["zero_divisors"]):
            raise ValueError(f"{game}: attack slot or divisor regression: {summary}")
        if game=="menzo":
            result[game]["original_level_table_value_mutation"] = (
                menzo_archive_mutation_scope(level))
            scope=result[game]["original_level_table_value_mutation"]
            if (scope["candidate_archive_words_that_would_change_if_runtime_gate_true"]!=184
                or scope["levels_with_potentially_changed_words"]!=6):
                raise ValueError("Menzo original mutable resource-table scope drift")

        print(game,"attack active slots",result[game][
            "attack_descriptor_zero_divisor_check"]["active_slots_in_records"])
    a.json.parent.mkdir(parents=True,exist_ok=True)
    a.json.write_text(json.dumps(result,indent=2)+"\n")

if __name__=="__main__":
    main()
