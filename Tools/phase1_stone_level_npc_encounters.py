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
    if n!=43 or total!=1535:
        raise ValueError(f"Stone level/FAMILY-A original corpus changed: {n}, {total}")
    if any(all_fields_valid[key]!=total for key in
           ("type_in_NPC_definition_range",
            "x_cell_within_level_width","y_cell_within_level_height")):
        raise ValueError("Stone 11-byte family-A type/coordinate geometry changed")
    if any(fraction_quantization[key]!=total for key in
           ("x_fraction_divisible_256","y_fraction_divisible_256")):
        raise ValueError("Stone 11-byte family-A fractional coordinate width changed")
    if any(counts[0][target] for target in TARGET_NPCS):
        raise ValueError("Stone affected zero-period NPC ID now appears in family-A column +0")
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


# The original CODE.2 stream reader below consumes **live** 16-byte
# position records and 50-byte actors. That is distinct from LEVELS.DAT's
# authored 11-byte family A and MUST NOT be used as a source-type proof.
LIVE_STREAM_WITNESSES = {
    0x220A9: ("movsx","WORD PTR ds:0x44fd6"),
    0x220B6: ("shl","edi,0x4"),
    0x220BB: ("push","0x35ad0"),
    0x220E1: ("imul","edi,edi,0x32"),
    0x220E6: ("push","0x44ff0"),
    0x220F3: ("cmp","eax,edi"),
    0x00059: ("imul","edi,DWORD PTR [ebp+0x14],0x58"),
    0x00063: ("mov","eax,ds:0x44790"),
    0x000F2: ("mov","WORD PTR [ebx+0x4],ax"),
    0x13779: ("imul","edx,DWORD PTR [ebp+0x18],0x58"),
    0x1378A: ("imul","ebx,DWORD PTR [ebp+0x14],0x32"),
    0x137CE: ("mov","WORD PTR [ebx+0x4],ax"),
}
def source_and_live_loader_provenance(rows):
    """Census source-vs-runtime layout and possible other spawn entry points.

    Instruction evidence, *not* an assumption that any particular caller
    receives an authored level record. Export only bounded machine metadata.
    """
    at={pc:(i,op,arg) for i,(pc,op,arg) in enumerate(rows)}
    fixed=[]
    for pc,(expected,operand) in LIVE_STREAM_WITNESSES.items():
        got=at.get(pc)
        if not got or got[1]!=expected or operand not in got[2]:
            raise ValueError(f"Stone live stream CODE.2 proof drift at {pc:#x}: {got}")
        fixed.append({"pc":pc,"asm":got[1]+" "+got[2]})
    def hits(predicate, limit=65, before=6, after=8):
        matches=[]
        for i,(pc,op,arg) in enumerate(rows):
            if not predicate(pc,op,arg):continue
            matches.append({"pc":pc,"asm":op+" "+arg,
              "before":[{"pc":p,"asm":m+" "+v} for p,m,v in rows[max(i-before,0):i]],
              "after":[{"pc":p,"asm":m+" "+v} for p,m,v in rows[i+1:i+1+after]]})
        return {"total_matches":len(matches),"examples":matches[:limit]}
    used_globals={
      "u16_live_actor_count_0x44FD6":"0x44fd6",
      "runtime_position_table_0x35AD0":"0x35ad",
      "runtime_actor_table_0x44FF0":"0x44ff",
      "catalog_pointer_global_0x44790":"0x44790",
    }
    # Only direct absolute references are automatically attributed to the
    # storage address. Displacements can be embedded in other arithmetic.
    glob_refs={}
    for label,needle in used_globals.items():
        glob_refs[label]=hits(lambda pc,op,arg,needle=needle:
          needle in arg and (("ds:"+needle) in arg or
                             ("+%s]"%needle) in arg or
                             ("push "+needle)==(op+" "+arg)),
          limit=28,before=3,after=4)
    stride11=hits(lambda pc,op,arg:op=="imul" and
                  arg.endswith(",0xb"),limit=70)
    spawner=hits(lambda pc,op,arg:op in ("call","jmp","push","mov","lea")
                 and "0x1364c" in arg,limit=55)
    direct_spawner=[pc for pc,op,arg in rows
                    if op=="call" and arg=="0x1364c"]
    import re
    # Earlier probes searched a literal base address and missed indexed
    # [register+0x35ADn]/[register+0x44FFn] operands. Match the full
    # runtime-table address range and keep the owning instruction.
    table_addresses={}
    for label,pat in {
        "position_16byte_runtime_struct":"0x35ad[0-9a-f]*",
        "live_50byte_actor_struct":"0x44ff[0-9a-f]*",
        "authoring_level_header_count":"0x44fd6",
    }.items():
        table_addresses[label]=hits(
           lambda pc,op,arg,rx=re.compile(pat):rx.search(arg) is not None,
           limit=110,before=4,after=4)
    count_read_sites = table_addresses["authoring_level_header_count"]
    candidate_region=[x for x in count_read_sites["examples"]
                      if x["pc"]<0x21D80]
    return {
       "verified_live_stream_instruction_witnesses":fixed,
       "runtime_table_and_count_xref_census":table_addresses,
       "level_count_reads_before_stream_reader":candidate_region,
       "eleven_stride_machine_context_0x16930":bounded(rows,0x16930,0x16990,limit=75),
       "verified_runtime_read_geometry":{
           "position_struct_stride_bytes":16,
           "live_actor_struct_stride_bytes":50,
           "npc_definition_catalog_stride_bytes":88,
           "runtime_position_base_code2_data":0x35AD0,
           "runtime_actor_base_code2_data":0x44FF0,
           "live_actor_type_word_offset":4,
           "read_window_code2":[0x220A9,0x220F5],
       },
       "authored_level_family_A_stride_bytes":11,
       "authored_to_live_converter":"UNKNOWN: no provenance proven by 16/50-byte read",
       "direct_call_to_0x1364C_count":len(direct_spawner),
       "direct_or_immediate_mentions_of_0x1364C":spawner,
       "machine_11_byte_imul_leads":stride11,
       "exact_storage_reference_windows":glob_refs,
       "accuracy_note":"Executable-wide instruction xrefs include false-positive data offsets. A bounded lead is not a verified loader or spawn.",
    }

CONSTRUCTOR_XREFS={
    0x0070D:None,  # LEVELS.DAT event opcode 0x0D: byte operands
    0x1AB03:77,
    0x1AF13:1,
    0x2C741:1,
    0x3018A:17,
    0x30286:17,
}
CONSTRUCTOR_WITNESSES={
    0x00010:("push","ebx"),
    0x00059:("imul","edi,DWORD PTR [ebp+0x14],0x58"),
    0x00063:("mov","eax,ds:0x44790"),
    0x000F2:("mov","WORD PTR [ebx+0x4],ax"),
    0x0070D:("call","0x10"),
    0x1AB03:("call","0x10"),
    0x1AF13:("call","0x10"),
    0x2C741:("call","0x10"),
    0x3018A:("call","0x10"),
    0x30286:("call","0x10"),
}

def direct_constructor_proof(rows):
    bypc={pc:(op,args) for pc,op,args in rows}
    checked=[]
    for pc,(op,fragment) in CONSTRUCTOR_WITNESSES.items():
        got=bypc.get(pc)
        if not got or got[0]!=op or fragment not in got[1]:
            raise ValueError(f"Stone original constructor @{pc:#x} drift: {got}")
        checked.append({"pc":pc,"asm":got[0]+" "+got[1]})
    all_calls=[(i,pc) for i,(pc,op,arg) in enumerate(rows)
               if op=="call" and arg=="0x10"]
    if sorted(pc for _,pc in all_calls)!=sorted(CONSTRUCTOR_XREFS):
        raise ValueError("Stone CODE.2 direct NPC constructor callsites changed")
    found=[]
    for idx,pc in all_calls:
        lines=rows[max(0,idx-12):idx]
        pushes=[(p,arg) for p,op,arg in lines if op=="push"]
        if len(pushes)<3:
            raise ValueError("original NPC constructor has fewer than three args")
        final=pushes[-1][1]
        expected=CONSTRUCTOR_XREFS[pc]
        if expected is not None and final!=f"0x{expected:x}":
            raise ValueError(f"NPC constructor {pc:#x} type arg changed: {final}")
        if expected is None and not final in ("eax","ebx","ecx","edx","esi","edi"):
            raise ValueError("event opcode13 type operand no longer dynamic")
        found.append({"call_at":pc,"template_id":expected,
                      "type_argument":final,
                      "preceding_type_input":[{"pc":p,"asm":op+" "+args}
                                              for p,op,args in lines][-9:]})
    return {
        "original_CODE2_direct_spawn_calls":len(all_calls),
        "immediate_template_ids":sorted({v for v in CONSTRUCTOR_XREFS.values() if v is not None}),
        "affected_zero_period_template_ids_in_direct_calls":
            sorted(set(CONSTRUCTOR_XREFS.values())&set(TARGET_NPCS)-{None}),
        "runtime_dynamic_constructor_callsites":[pc for pc,id in CONSTRUCTOR_XREFS.items() if id is None],
        "original_machine_witnesses":checked,
        "all_direct_spawn_calls":found,
        "boundary":"No proof that indirect CODE.2/CODE.1 calls or RES9 VM cannot construct these NPCs",
    }

def archived_event_spawn_opcode13(path):
    blob=path.read_bytes()
    n=struct.unpack_from("<I",blob)[0]
    offsets=struct.unpack_from(f"<{n}I",blob,4)
    counts=collections.Counter()
    for i,(a,b) in enumerate(zip(offsets,(*offsets[1:],len(blob)))):
        rec=blob[a:b]
        bounds=struct.unpack_from("<7I",rec,1)
        chunk=rec[bounds[5]:bounds[6]]
        if len(chunk)!=0x514:
            raise ValueError(f"Stone level {i} event block geometry changed")
        for j in range(50):
            count=chunk[j*26]
            if count>5:raise ValueError("invalid event slot command count")
            for k in range(count):
                code=chunk[j*26+1+k*5]
                counts[code]+=1
    if counts.get(13,0)!=0 or sum(counts.values())!=469:
        raise ValueError(f"Stone archived event opcode13 absence drift {counts}")
    return {
        "original_levels":n,"active_event_commands":sum(counts.values()),
        "command_opcode_13_spawn_calls_in_original_levels":counts.get(13,0),
        "meaning":"CODE.2 0x70D invokes dynamic NPC constructor for this event opcode",
        "limits":"Does not inspect generated event tables, saves, other script languages",
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
       "original_source_and_live_loader_provenance":source_and_live_loader_provenance(rows),
       "original_direct_constructor_callsite_audit":direct_constructor_proof(rows),
       "original_level_event_opcode_13_spawn_audit":archived_event_spawn_opcode13(root/game["level"]),
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
