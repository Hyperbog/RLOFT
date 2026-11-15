#!/usr/bin/env python3
"""Phase 1: NPC attack flags/auxiliary bytes and tile-channel filter consumers.

Original CODE.2 and DATs are read-only, with digests verified by Phase 1
helpers. Export bounded instruction witnesses and aggregate distributions,
not original scripts, executable bodies or unproved gameplay field names.
"""
from __future__ import annotations

import argparse
import collections
import json
import re
import runpy
import struct
from pathlib import Path

CONFIG = {
    "menzo": {
        "npc": "NPCS.DAT", "level": "LEVELS.DAT",
        "attack_flag_sites": [0x1FF30, 0x1FF41, 0x2004E, 0x20059],
        "attack_selection": (0x1FA1C, 0x1FC30),
        "tile_wrappers": [0x25B5C, 0x25D34],
        "tile_gates": {
            0x25B6A: ("test", "BYTE PTR [ebx+0x1],0x4"),
            0x25B96: ("cmp", "eax,DWORD PTR [ebp+0x18]"),
            0x25BAC: ("movzx", "BYTE PTR [ebx+0xc]"),
            0x25BC8: ("movzx", "BYTE PTR [ebx+0xd]"),
            0x25D3C: ("test", "BYTE PTR [ebx+0x1],0x4"),
            0x25D6B: ("cmp", "eax,DWORD PTR [ebp+0x18]"),
        },
    },
    "stone": {
        "npc": "CD_Extracted/NPCS.DAT", "level": "CD_Extracted/LEVELS.DAT",
        "attack_flag_sites": [0x275F9, 0x275FF, 0x27608, 0x275B2],
        "attack_selection": (0x26CF0, 0x26F20),
        "tile_wrappers": [0x2E72C, 0x2EA34],
        "tile_gates": {
            0x2E73A: ("test", "BYTE PTR [ebx+0x1],0x4"),
            0x2E768: ("cmp", "eax,DWORD PTR [ebp+0x18]"),
            0x2E77E: ("movzx", "BYTE PTR [ebx+0xc]"),
            0x2E79A: ("movzx", "BYTE PTR [ebx+0xd]"),
            0x2EA3B: ("test", "BYTE PTR [ebx+0x1],0x4"),
            0x2EA6C: ("cmp", "eax,DWORD PTR [ebp+0x14]"),
        },
    },
}


def bounded(rows, i, left=7, right=15):
    return [{"pc":pc, "asm":op+" "+arg}
            for pc,op,arg in rows[max(0,i-left):min(len(rows),i+right)]]


def exact_gates(rows, gates):
    table={pc:(op,arg) for pc,op,arg in rows}
    result=[]
    for pc,(op,piece) in gates.items():
        entry=table.get(pc)
        if not entry or entry[0]!=op or piece not in entry[1]:
            raise ValueError(f"CODE.2 tile filter witness @{pc:#x} "
                             f"expected {op} {piece}; found {entry}")
        result.append({"pc":pc,"instruction":entry[0]+" "+entry[1]})
    return result


def combat_flags(rows, profile):
    bypc={pc:(i,op,args) for i,(pc,op,args) in enumerate(rows)}
    found=[]
    for pc in profile["attack_flag_sites"]:
        if pc not in bypc:
            raise ValueError(f"missing original attack-flag site {pc:#x}")
        i,op,args=bypc[pc]
        found.append({"pc":pc, "instruction":op+" "+args,
                      "surroundings":bounded(rows,i,9,24)})
    a,b=profile["attack_selection"]
    # Keep short dispatcher windows at each target; do not export entire code.
    return {"flag_bit_readsites":found,
            "two_attack_selector_entry":a,
            "selector_prefix":[{"pc":pc,"asm":op+" "+args}
                               for pc,op,args in rows if a<=pc<min(a+0xA0,b)][:50]}


def attack_auxiliary_values(path):
    data=path.read_bytes()
    num=struct.unpack_from("<I",data)[0]
    if len(data)!=4+num*88: raise ValueError("NPC definition extent mismatch")
    all_slots=[collections.Counter() for _ in range(9)]
    active_slots=[collections.Counter() for _ in range(9)]
    by_slot=[[collections.Counter() for _ in range(9)] for _ in range(2)]
    flag_bits=collections.Counter()
    counts=collections.Counter()
    residual=[]
    for i in range(num):
        r=data[4+88*i:4+88*(i+1)]
        count=r[0x1D]
        if count not in (0,1,2): raise ValueError("invalid NPC attack count")
        counts[count]+=1
        for slot in range(2):
            d=r[0x1E+9*slot:0x27+9*slot]
            active=slot<count
            for at,v in enumerate(d):
                all_slots[at][v]+=1
                by_slot[slot][at][v]+=1
                if active:active_slots[at][v]+=1
            if active:
                f=d[3]
                for bit in range(8):
                    flag_bits[bit]+=bool(f&(1<<bit))
            elif any(d) and len(residual)<15:
                residual.append({"index":i,"slot":slot,
                                 "present":True, "attack_count":count})
    return {
        "records":num, "attack_counts":dict(sorted(counts.items())),
        "active_attack_descriptors":sum(k*v for k,v in counts.items()),
        "active_aux_byte_histograms":{
            str(pos):dict(sorted(active_slots[pos].items()))
            for pos in (3,4,5,6,7,8)},
        "active_control_bits_set":{
            str(bit):flag_bits[bit] for bit in range(8)},
        "per_slot_auxiliary_unique_values":{
            str(slot):{str(at):len(by_slot[slot][at])
                       for at in (3,4,5,6,7,8)}
            for slot in range(2)},
        "inactive_nonzero_slot_examples":residual,
        "nonzero_reserved_bytes_active":{
            str(pos):sum(n for v,n in active_slots[pos].items() if v)
            for pos in (4,6,7)},
    }


def auxiliary_readsite_leads(rows):
    # Candidate indexed accesses: descriptor base + 9*attackIndex + field.
    # Unanchored register accesses may point to unrelated structures.
    fields=[0x21,0x22,0x23,0x24,0x25,0x26,0x2A,0x2B,0x2C,0x2D,0x2E,0x2F]
    groups={}
    for field in fields:
        needle=f"+0x{field:x}]"
        found=[]
        for i,(pc,op,args) in enumerate(rows):
            if needle not in args or "PTR [" not in args:
                continue
            if not any(op==mn for mn in
                       ("mov","movzx","movsx","cmp","test","lea","add","and","or")):
                continue
            window=rows[max(0,i-17):i]
            index_hint=any((op0=="lea" and "*8" in arg0)
                           or (op0=="imul" and ",0x9" in arg0)
                           for _,op0,arg0 in window)
            found.append({"pc":pc,"asm":op+" "+args,
                          "nine_byte_index_in_previous_17":index_hint})
        groups[f"0x{field:x}"]={
            "all_syntactic_hits":len(found),
            "nine_byte_index_hint_hits":sum(v["nine_byte_index_in_previous_17"]
                                            for v in found),
            "interesting":([v for v in found if v["nine_byte_index_in_previous_17"]][:25]
                           or found[:8]),
        }
    return {"original_code_candidate_offset_hits":groups,
            "provenance_warning":"Only exact derived case witnesses certify NPC field base"}


def tile_filter_callers(rows, entries):
    wanted=set(entries)
    result=[]
    for i,(pc,op,args) in enumerate(rows):
        if op!="call" or not args.startswith("0x"):continue
        try:target=int(args,16)
        except ValueError:continue
        if target not in wanted:continue
        result.append({"callsite":pc,"target":target,
                       "instructions":bounded(rows,i,20,16)})
    return {"wrapper_entrypoints":entries,
            "direct_callsite_count":len(result),
            "callsite_windows":result[:45]}


def channel_distributions(path):
    data=path.read_bytes()
    levels=struct.unpack_from("<I",data)[0]
    offsets=struct.unpack_from(f"<{levels}I",data,4)
    by_ch=[collections.Counter() for _ in range(4)]
    by_orientation=[collections.Counter() for _ in range(4)]
    channel_distinct=collections.Counter()
    orientation_distinct=collections.Counter()
    cell_zero=collections.Counter()
    exceptional=0
    for a,b in zip(offsets,(*offsets[1:],len(data))):
        rec=data[a:b]
        table=struct.unpack_from("<1024H",rec,0x1D+0x0A)
        for cell in range(64):
            for ori in range(4):
                vals=table[16*cell+4*ori:16*cell+4*ori+4]
                channel_distinct[len(set(vals))]+=1
                for ch,v in enumerate(vals):by_ch[ch][v]+=1
                if cell==0 and any(vals):exceptional+=1
            for ch in range(4):
                vals=tuple(table[16*cell+4*ori+ch] for ori in range(4))
                orientation_distinct[len(set(vals))]+=1
                for ori,v in enumerate(vals):by_orientation[ori][v]+=1
    return {
        "levels":levels,"checked_code_orientation_combinations":levels*64*4,
        "nonzero_entries_by_channel":[sum(n for v,n in co.items() if v)
                                      for co in by_ch],
        "distinct_values_by_channel":[len(co) for co in by_ch],
        "channel_top_values":[co.most_common(7) for co in by_ch],
        "distinct_channels_per_cell_orientation":
            dict(sorted(channel_distinct.items())),
        "distinct_orientations_per_cell_channel":
            dict(sorted(orientation_distinct.items())),
        "nonzero_cell_zero_groups":exceptional,
        "interpretation":"Values compared as equality keys in a flagged, rectangular filter; "
                         "specific downstream effect not yet proven",
    }


EXPECTED_CHANNEL_NONZERO={
    "menzo":[6041,6319,4658,4318],
    "stone":[3771,3835,1419,1466],
}
EXPECTED_AUXILIARY={
    "menzo":{"5":{"3":47},"7":{"128":47},"8":{"1":47}},
    "stone":{"5":{"0":5,"3":37,"4":2,"6":1},
             "7":{"0":5,"128":40},"8":{"0":5,"1":16,"2":23,"3":1}},
}

def assert_corpus(title, observed):
    channel=observed["level_channel_data"]
    if channel["nonzero_entries_by_channel"]!=EXPECTED_CHANNEL_NONZERO[title]:
        raise ValueError(f"{title} tile-channel distribution changed")
    if channel["nonzero_cell_zero_groups"]!=0:
        raise ValueError(f"{title} level table code-0 group unexpectedly populated")
    if observed["original_tile_match_filter"]["callers"]["direct_callsite_count"]!=2:
        raise ValueError(f"{title} original tile filter dispatch count changed")
    h=observed["npc_auxiliary_data"]["active_aux_byte_histograms"]
    for field,values in EXPECTED_AUXILIARY[title].items():
        actual={str(k):n for k,n in h[field].items()}
        if actual!=values:
            raise ValueError(f"{title} attack auxiliary +{field} values changed: {actual}")
    for pos in ("4","6"):
        actual={str(k):n for k,n in h[pos].items()}
        if actual!={"0":sum(h["5"].values())}:
            raise ValueError(f"{title} attack auxiliary +{pos} not zero: {actual}")


def main():
    ap=argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--root",type=Path,required=True)
    ap.add_argument("--repo-root",type=Path,default=Path("."))
    ap.add_argument("--json",type=Path,required=True)
    args=ap.parse_args()
    util=runpy.run_path(str(args.repo_root/"Tools/phase1_code2_consumers.py"))
    secure=runpy.run_path(str(args.repo_root/"Tools/phase1_npc_level_semantics.py"))
    report={}
    for title,conf in CONFIG.items():
        cfg=util["GAMES"][title]
        folder=args.root/cfg["dir"]
        npcs=folder/conf["npc"]
        levels=folder/conf["level"]
        secure["assert_media_file"](title+"_npc",npcs)
        secure["assert_media_file"](title+"_levels",levels)
        code,_,meta=util["le_image"](folder/cfg["exe"],cfg["expected"])
        rows=util["disassembly"](code)
        report[title]={
            "code2_sha256":meta["sha256"],
            "npc_auxiliary_data":attack_auxiliary_values(npcs),
            "original_attack_control_flow":combat_flags(rows,conf),
            "npc_auxiliary_offset_leads":auxiliary_readsite_leads(rows),
            "original_tile_match_filter":{
                "exact_witnesses":exact_gates(rows,conf["tile_gates"]),
                "callers":tile_filter_callers(rows,conf["tile_wrappers"]),
            },
            "level_channel_data":channel_distributions(levels),
        }
        assert_corpus(title,report[title])
        print(title,"flags",
              report[title]["npc_auxiliary_data"]["active_control_bits_set"],
              "tile lookup callers",
              report[title]["original_tile_match_filter"]["callers"]["direct_callsite_count"])
    args.json.parent.mkdir(parents=True,exist_ok=True)
    args.json.write_text(json.dumps(report,indent=2)+"\n",encoding="utf-8")


if __name__=="__main__":
    main()
