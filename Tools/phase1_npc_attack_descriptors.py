#!/usr/bin/env python3
"""Authenticated Menzo/Stone NPC attack-descriptor executable witnesses.

This tool verifies original CODE.2 instruction operands and original 88-byte
NPC definition geometry. It does not execute the combat VM or modify assets.
"""
from __future__ import annotations
import argparse
import collections
import json
import runpy
import struct
from pathlib import Path

ATTACKS={
 "menzo":{
   "npc":"NPCS.DAT","count_reads":{
       0x1ff3b:("cmp","BYTE PTR [edi+0x1d],0x2"),
       0x20077:("cmp","BYTE PTR [edi+0x1d],0x2"),
       0x1ff30:("test","BYTE PTR [edi+0x21],0xc"),
       0x1ff41:("test","BYTE PTR [edi+0x2a],0xc")},
   "roll":{
       0xfb0c:("movsx","WORD PTR [esi+0x24]"),
       0xfb13:("lea","[edx*8+0x0]"),
       0xfb1a:("add","eax,edx"),
       0xfb1e:("movzx","BYTE PTR [edi+eax*1+0x20]"),
       0xfb24:("movzx","BYTE PTR [edi+eax*1+0x1f]"),
       0xfb2a:("movzx","BYTE PTR [edi+eax*1+0x1e]"),
       0xfb30:("call","0x20818")},
   "cycle":{
       0x203b2:("movsx","WORD PTR [ebx+0x24]"),
       0x203c5:("movzx","BYTE PTR [esi+eax*1+0x23]"),
       0x203cf:("idiv","ecx"),
       0x203e3:("movzx","BYTE PTR [esi+eax*1+0x26]")},
   "contexts":{"count_A":(0x1ff10,0x1ffb0),
               "count_B":(0x20035,0x200a0),
               "attack":(0xfa70,0xfb60),
               "cycle":(0x20370,0x20450)}},
 "stone":{
   "npc":"CD_Extracted/NPCS.DAT","count_reads":{
       0x275f0:("movzx","BYTE PTR [edi+0x1d]"),
       0x277a5:("movzx","BYTE PTR [edi+0x1d]"),
       0x275f4:("cmp","eax,0x2"),
       0x275f9:("mov","WORD PTR [edi+0x2a]"),
       0x275ff:("and","al,0x8d")},
   "roll":{
       0x12b67:("movsx","WORD PTR [esi+0x28]"),
       0x12b6c:("lea","[edx*8+0x0]"),
       0x12b73:("add","eax,edx"),
       0x12b75:("push","ds:0x1a4bc"),
       0x12b7b:("movzx","BYTE PTR [edi+eax*1+0x20]"),
       0x12b81:("movzx","BYTE PTR [edi+eax*1+0x1f]"),
       0x12b87:("movzx","BYTE PTR [edi+eax*1+0x1e]"),
       0x12b8d:("call","0x29ed0")},
   "cycle":{
       0x27c9e:("movzx","BYTE PTR [esi+eax*1+0x23]"),
       0x27cbe:("movzx","BYTE PTR [esi+eax*1+0x26]"),
       0x27ca8:("idiv","ecx")},
   "contexts":{"count_A":(0x275b0,0x27680),
               "count_B":(0x27770,0x27830),
               "attack":(0x12af0,0x12bd0),
               "cycle":(0x27c50,0x27d60)}},
}

EXPECTED_ATTACK_COUNTS={"menzo":{0:67,1:15,2:16},"stone":{0:80,1:15,2:15}}


def verify_machine(rows,profile):
    bypc={pc:(op,args) for pc,op,args in rows}
    out={}
    for role in ("count_reads","roll","cycle"):
        evidence=[]
        for pc,(op,frag) in profile[role].items():
            actual=bypc.get(pc)
            if not actual or actual[0]!=op or frag not in actual[1]:
                raise ValueError(f"{role} CODE.2 @{pc:#x}: expected {op} {frag}, found {actual}")
            evidence.append({"pc":pc,"instruction":op+" "+actual[1]})
        out[role]=evidence
    out["bounded_original_instruction_contexts"]={
        name:[{"pc":pc,"asm":op+" "+args}
              for pc,op,args in rows if a<=pc<b][:150]
        for name,(a,b) in profile["contexts"].items()}
    return out

def records(path):
    buf=path.read_bytes()
    count=struct.unpack_from("<I",buf)[0]
    if len(buf)!=4+count*0x58:raise ValueError("bad NPC extent")
    tally=collections.Counter()
    descriptors=collections.Counter()
    nonzero_by_field=[0]*9
    for i in range(count):
        rec=buf[4+0x58*i:4+0x58*(i+1)]
        attack_count=rec[0x1d]
        if attack_count not in (0,1,2):
            raise ValueError(f"NPC record {i}: unrecognized attack count {attack_count}")
        tally[attack_count]+=1
        for slot in range(2):
            desc=rec[0x1e+slot*9:0x27+slot*9]
            if len(desc)!=9:raise ValueError("truncated attack entry")
            active=slot<attack_count
            dice,sides,bonus=desc[:3]
            descriptors[(slot,int(active),int(any(desc)))]+=1
            if active:
                for x in range(9):
                    nonzero_by_field[x]+=bool(desc[x])
    return {"npc_count":count,"attack_count_field_offset":0x1d,
            "attack_count_histogram":dict(sorted(tally.items())),
            "attack_descriptor_0_offset":0x1e,
            "attack_descriptor_1_offset":0x27,
            "each_descriptor_bytes":9,
            "attack_damage_dice_count_offset_in_descriptor":0,
            "attack_damage_die_sides_offset_in_descriptor":1,
            "attack_damage_bonus_offset_in_descriptor":2,
            "candidate_animation_modulus_byte_offset_in_descriptor":5,
            "candidate_animation_limit_byte_offset_in_descriptor":8,
            "active_and_residual_slot_histogram":{
                "slot%d_active%d_has_bytes%d"%k:v
                for k,v in sorted(descriptors.items())},
            "nonzero_active_fields":nonzero_by_field,
            "descriptor_auxiliary_fields":{
                "byte_3":"VERIFIED attack-control bit tests in CODE.2; exact bit semantics partly unknown",
                "byte_4":"raw; zero in all sampled definitions",
                "byte_5":"VERIFIED divisor in modulo of attack/action phase counter",
                "byte_6":"raw; zero in all sampled definitions",
                "byte_7":"raw; preservation required",
                "byte_8":"VERIFIED compared to modulo attack/action phase counter",
            }}

def main():
    ap=argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--root",type=Path,required=True)
    ap.add_argument("--repo-root",type=Path,default=Path("."))
    ap.add_argument("--json",type=Path,required=True)
    a=ap.parse_args()
    codeutil=runpy.run_path(str(a.repo_root/"Tools/phase1_code2_consumers.py"))
    secure=runpy.run_path(str(a.repo_root/"Tools/phase1_npc_level_semantics.py"))
    output={}
    for game,conf in ATTACKS.items():
        cfg=codeutil["GAMES"][game]
        root=a.root/cfg["dir"]
        npc=root/conf["npc"]
        secure["assert_media_file"](game+"_npc",npc)
        code,_,meta=codeutil["le_image"](root/cfg["exe"],cfg["expected"])
        rows=codeutil["disassembly"](code)
        output[game]={"source_sha256":meta["sha256"],
                      "original_executable_witnesses":verify_machine(rows,conf),
                      "record_corpus":records(npc)}
        histogram=output[game]["record_corpus"]["attack_count_histogram"]
        if histogram!=EXPECTED_ATTACK_COUNTS[game]:
            raise ValueError(f"{game} original NPC attack count regression: {histogram}")
        print(game,"verified attack-count and dice-rolling machine accesses",
              "NPC record count",output[game]["record_corpus"]["npc_count"])
    a.json.parent.mkdir(parents=True,exist_ok=True)
    a.json.write_text(json.dumps(output,indent=2)+"\n")
if __name__=="__main__":main()
