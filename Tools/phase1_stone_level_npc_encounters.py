#!/usr/bin/env python3
"""Phase 1: test Stone NPC encounter source in original 11-byte level family A.

Archive counts/column values are empirical. NPC-index semantics require
original CODE.2 loader cross-references before any encounter claim.
No original executable, level entries or NPC bytes are published.
"""
from __future__ import annotations
import argparse
import collections
import json
import runpy
import struct
from pathlib import Path

TARGET_NPCS = {4:"DEATH DOG",24:"SKELETON WARRIOR",27:"TOMB GUARDIAN"}
REGIONS={
    "original_level_load_and_creature_family":(0x21D80,0x22390),
    "original_creature_definition_construct":(0x13640,0x13940),
    "original_creature_add_path":(0x00000,0x00145),
    "original_level_family_deserialization_A":(0x220A0,0x22460),
    "original_level_family_deserialization_B":(0x22460,0x22840),
}
def level_family(path):
    data=path.read_bytes()
    n=struct.unpack_from("<I",data)[0]
    offsets=struct.unpack_from(f"<{n}I",data,4)
    counts=[collections.Counter() for _ in range(5)]
    per_byte=[collections.Counter() for _ in range(11)]
    candidate_locations=collections.defaultdict(list)
    total=0
    all_fields_valid=collections.Counter()
    fraction_quantization=collections.Counter()
    invalid_position_samples=[]
    for level,(a,b) in enumerate(zip(offsets,(*offsets[1:],len(data)))):
        rec=data[a:b];boundaries=struct.unpack_from("<7I",rec,1)
        width,height=struct.unpack_from("<hh",rec,0x1D+1)
        if width<=0 or height<=0:raise ValueError("invalid original level grid")
        family=rec[boundaries[2]:boundaries[3]]
        if len(family)%11:raise ValueError("corrupt original 11-byte family A")
        for nrecord in range(len(family)//11):
            row=family[nrecord*11:(nrecord+1)*11]
            total+=1
            type_id=struct.unpack_from("<H",row,0)[0]
            x_cell=struct.unpack_from("<h",row,2)[0]
            y_cell=struct.unpack_from("<h",row,6)[0]
            x_fraction=struct.unpack_from("<H",row,4)[0]
            y_fraction=struct.unpack_from("<H",row,8)[0]
            all_fields_valid["type_in_NPC_definition_range"]+=type_id<110
            all_fields_valid["x_cell_within_level_width"]+=0<=x_cell<width
            all_fields_valid["y_cell_within_level_height"]+=0<=y_cell<height
            fraction_quantization["x_fraction_divisible_256"]+=x_fraction%256==0
            fraction_quantization["y_fraction_divisible_256"]+=y_fraction%256==0
            if (not 0<=x_cell<width or not 0<=y_cell<height) and len(invalid_position_samples)<12:
                invalid_position_samples.append({"level":level,"record":nrecord,
                                                 "xy":[x_cell,y_cell],
                                                 "width_height":[width,height]})
            for col,off in enumerate((0,2,4,6,8)):
                word=struct.unpack_from("<H",row,off)[0]
                counts[col][word]+=1
                if word in TARGET_NPCS and len(candidate_locations[(off,word)])<12:
                    candidate_locations[(off,word)].append({
                      "level_index":level,"row_index":nrecord})
            for off,value in enumerate(row):
                per_byte[off][value]+=1
    return {
        "original_archive_level_count":n,"family_A_11byte_records":total,
        "provisional_Strahd_compatible_type_xy_field_equations":{
            "checks":dict(all_fields_valid),
            "fraction_divisibility_by_256":dict(fraction_quantization),
            "bad_coordinate_examples":invalid_position_samples,
            "meaning":"corpus consistency only; Stone CODE.2 type-index proof pending"},
        "target_ID_absence_if_plus_0_is_NPC_type":all(counts[0][t]==0 for t in TARGET_NPCS),
        "aligned_u16_column_profiles":[{
            "byte_offset":off,
            "minimum":min(counts[i]),"maximum":max(counts[i]),
            "distinct":len(counts[i]),
            "top_8_values":counts[i].most_common(8),
            "target_npc_ids":{str(t):counts[i][t] for t in TARGET_NPCS},
            "all_words_within_NPC_record_range_0_109":
                sum(num for word,num in counts[i].items() if 0<=word<110),
        } for i,off in enumerate((0,2,4,6,8))],
        "byte_column_value_domains":[{
            "offset":off,"minimum":min(c),"maximum":max(c),
            "distinct":len(c),"most_common":c.most_common(8),
            "target_npc_byte_values":{str(t):c[t] for t in TARGET_NPCS},
        } for off,c in enumerate(per_byte)],
        "candidate_locations":{f"u16_at_{off}_type_{id}":hits
                               for (off,id),hits in candidate_locations.items()},
        "semantics":"UNKNOWN until original CODE.2 family A type-id load confirmed",
    }

def bounded(rows,start,end,limit=280):
    return [{"pc":pc,"asm":op+" "+a}
            for pc,op,a in rows if start<=pc<end][:limit]

def loader_witnesses(rows):
    refer=[]
    for i,(pc,op,arg) in enumerate(rows):
        if not 0x21D80<=pc<0x22600:continue
        marker=(
            "0x58" in arg or "0x44790" in arg or "0x80a" in arg
            or "0x824" in arg or (op=="imul" and "0xb" in arg)
            or (op=="call" and arg in ("0x13779","0x0","0x13700"))
        )
        if not marker:continue
        refer.append({
            "pc":pc,"asm":op+" "+arg,
            "context":bounded(rows,max(0,pc-23),pc+43,30),
        })
    return {
        "original_loader_regions":{key:bounded(rows,*v,limit=240)
                                   for key,v in REGIONS.items()},
        "candidate_original_NPC_field_consumers":refer[:65],
        "record_schema_status":"UNKNOWN: code address/field provenance must be traced",
    }

def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument("--root",type=Path,required=True)
    p.add_argument("--repo-root",type=Path,default=Path("."))
    p.add_argument("--json",type=Path,required=True)
    a=p.parse_args()
    tool=runpy.run_path(str(a.repo_root/"Tools/phase1_code2_consumers.py"))
    hashutil=runpy.run_path(str(a.repo_root/"Tools/phase1_npc_level_semantics.py"))
    game=tool["GAMES"]["stone"];root=a.root/game["dir"]
    hashutil["assert_media_file"]("stone_levels",root/game["level"])
    hashutil["assert_media_file"]("stone_npc",root/game["npc"])
    code,_,meta=tool["le_image"](root/game["exe"],game["expected"])
    rows=tool["disassembly"](code)
    body={
       "code2_sha256":meta["sha256"],
       "original_11byte_family_A":level_family(root/game["level"]),
       "original_CODE2_readsite_probes":loader_witnesses(rows),
       "encounter_source_warning":"Raw field coincidences alone are not NPC spawns",
    }
    a.json.parent.mkdir(parents=True,exist_ok=True)
    a.json.write_text(json.dumps(body,indent=2)+"\n")
    print("stone: family-A records",body["original_11byte_family_A"][
          "family_A_11byte_records"],"candidate loader leads",
          len(body["original_CODE2_readsite_probes"][
              "candidate_original_NPC_field_consumers"]))
if __name__=="__main__":
    main()
