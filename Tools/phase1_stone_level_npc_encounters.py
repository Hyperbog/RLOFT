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
    # Restoring saved live actors re-links type (+0x04) to 88-byte catalog.
    0x23AE7: ("imul","edi,ebx,0x32"),
    0x23AF1: ("movsx","WORD PTR [edi+0x44ff4]"),
    0x23AF8: ("imul","edi,edi,0x58"),
    0x23B00: ("mov","DWORD PTR [esi+0x35ad8],eax"),
    0x23B06: ("mov","eax,ds:0x44790"),
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
        "authoring_level_header_count":"0x44fd6(?![0-9a-f])",
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
           "live_type_field_u16LE_offset":4,
           "relink_live_type_to_NPCS_definition":
               "CODE.2 0x23AE7/0x23AF1/0x23AF8/0x23B06 uses live +4 signed word, multiplies by 88, reuses catalog base",
           "authored_11byte_type_source_proven":False,
       },
       "authored_level_family_A_stride_bytes":11,
       "authored_to_live_converter":"UNKNOWN: no provenance proven by 16/50-byte read",
       "direct_call_to_0x1364C_count":len(direct_spawner),
       "direct_or_immediate_mentions_of_0x1364C":spawner,
       "machine_11_byte_imul_leads":stride11,
       "exact_storage_reference_windows":glob_refs,
       "accuracy_note":"Executable-wide instruction xrefs include false-positive data offsets. A bounded lead is not a verified loader or spawn.",
    }


# Prove the **filename actually used by this loader**, instead of inferring
# the file identity from matching record sizes in an extracted CD archive.
# Stone's DOS guest data-object string is constant except three digit slots.
WORK_LEVEL_NAME_AT = 0x1F630
WORK_LEVEL_WITNESSES = {
    0x21DB9: ("mov","esi,DWORD PTR [ebp+0x14]"),
    0x21DD2: ("mov","ebx,0x64"),
    0x21DE0: ("mov","edi,0xa"),
    0x21DEF: ("mov","ds:0x1f638,al"),
    0x21DFB: ("mov","BYTE PTR ds:0x1f63a,dl"),
    0x21E12: ("push","0x1f630"),
    0x21E17: ("mov","ds:0x1f639,al"),
    0x21E1C: ("call","0x434b0"),
    0x21E24: ("push","0x1f630"),
    0x21E29: ("call","0x433e8"),
    0x21E3A: ("push","0x1f630"),
    0x21E3F: ("call","0x433d4"),
    0x21E5F: ("push","0x1d"),
    0x21E71: ("push","0x824"),
    0x21E76: ("push","0x447cc"),
    0x21E7B: ("call","0x43424"),
    0x220B6: ("shl","edi,0x4"),
    0x220BB: ("push","0x35ad0"),
    0x220C0: ("call","0x43424"),
    0x220E1: ("imul","edi,edi,0x32"),
    0x220E6: ("push","0x44ff0"),
    0x220EB: ("call","0x43424"),
}
def work_level_source_proof(rows, data):
    prefix=b"work\\lev???.dat\x00"
    if data[WORK_LEVEL_NAME_AT:WORK_LEVEL_NAME_AT+len(prefix)]!=prefix:
        raise ValueError("original CODE.2 work-level filename template changed")
    # The data object also contains an error message mentioning levels.dat.
    # That is NOT, by itself, evidence of a file-open argument or converter.
    if b"Cannot find file levels.dat" not in data:
        raise ValueError("original CODE.2 missing level-archive diagnostic string")
    at={pc:(op,arg) for pc,op,arg in rows}
    ins=[]
    for pc,(op,fragment) in WORK_LEVEL_WITNESSES.items():
        observed=at.get(pc)
        if not observed or observed[0]!=op or fragment not in observed[1]:
            raise ValueError(f"Stone numbered work-level loader witness drift {pc:#x}: {observed}")
        ins.append({"pc":pc,"asm":observed[0]+" "+observed[1]})
    calls=[pc for pc,op,arg in rows if op=="call" and arg=="0x21db0"]
    # Caller context supports source-identity classification, but a branch
    # near the call alone never proves a real new-game or restore sequence.
    call_windows=[]
    for i,(pc,op,arg) in enumerate(rows):
        if op=="call" and arg=="0x21db0":
            call_windows.append({"call_pc":pc,
                "preceding":[{"pc":p,"asm":m+" "+v}
                             for p,m,v in rows[max(0,i-22):i]],
                "following":[{"pc":p,"asm":m+" "+v}
                             for p,m,v in rows[i+1:i+17]]})
    if calls!=[0x8C30,0x9AD5,0x2445E]:
        raise ValueError(f"Stone work-level reader caller census drift: {calls}")
    # Archive-level source requires an independent importer or launch/runtime
    # generation: the numbered file reader does not identify an authored
    # family-A 11-byte input conversion.
    return {
      "file_name_template":"work\\lev???.dat",
      "file_name_storage":"CODE.2 LE data object 0x1F630, NUL-terminated char[16]",
      "decimal_digit_write_offsets":["0x1F638","0x1F639","0x1F63A"],
      "level_id_examples":[{"level_index":n,"constructed_name":
         f"work\\lev{n:03d}.dat"} for n in (0,22,123)],
      "filename_initializer_calls":[0x21E1C,0x21E29,0x21E3F],
      "per_level_file_read_region":[0x21DB0,0x22439],
      "exact_original_instruction_witnesses":ins,
      "direct_callers_of_numbered_level_reader":calls,
      "work_level_reader_caller_instruction_windows":call_windows,
      "CODE2_data_contains_levels_dat_diagnostic_text":True,
      "diagnostic_is_file_open_proof":False,
      "conclusion":"VERIFIED distinct numbered work-level path; 0x824 header and live 16/50-byte reads are from this opened stream, not shown to be raw CD_Extracted/LEVELS.DAT",
      "raw_archive_to_work_file_producer":"UNKNOWN; could be installer, CODE.1, another routine, or saved state",
    }


# Focused one-pass search for the producer of the numbered work-level file.
# The 0x2445E caller is immediately followed by a second formatter of the
# same filename; probe both subroutines and the actual file-helper calls.
# Do NOT label these as save or archive producer until evidence supports it.
def work_level_producer_and_indirect_dispatch(rows):
    bypc={pc:(op,arg) for pc,op,arg in rows}
    for pc,op,arg in (
      (0x2445E,"call","0x21db0"),
      (0x24477,"mov","eax,DWORD PTR [ebp+0x14]"),
      (0x2448B,"mov","ds:0x1f638,al"),
    ):
        got=bypc.get(pc)
        if got is None or got[0]!=op or arg not in got[1]:
            raise ValueError(f"Stone work-level producer lead witness drift {pc:#x}: {got}")
    windows={
       "level_change_post_reader":(0x2445D,0x24760),
       "work_file_candidate_helper":(0x25084,0x25340),
       "work_reader_open_header":(0x21DB0,0x21ED0),
       "work_reader_tail":(0x223B0,0x22470),
    }
    # Bounded xrefs: include the instruction before/after for disambiguating
    # an address constant used as a buffer vs an actual stream-argument.
    def xref(needle,max_hits=55):
        matches=[]
        for i,(pc,op,arg) in enumerate(rows):
            if needle not in arg:continue
            matches.append({"pc":pc,"asm":op+" "+arg,
               "before":[{"pc":p,"asm":m+" "+a} for p,m,a in rows[max(0,i-5):i]],
               "after":[{"pc":p,"asm":m+" "+a} for p,m,a in rows[i+1:i+7]]})
        return {"total_matches":len(matches),"examples":matches[:max_hits]}
    indirect=[]
    for i,(pc,op,arg) in enumerate(rows):
        if op!="call" or arg.startswith("0x"):continue
        indirect.append({"pc":pc,"asm":op+" "+arg,
           "before":[{"pc":p,"asm":m+" "+a} for p,m,a in rows[max(0,i-6):i]]})

    if indirect and min(x["pc"] for x in indirect)<0x40000:
        raise ValueError("Stone gameplay-region indirect call census changed")
    # Indirect jumps are distinct from indirect calls: switch/case dispatch
    # can still transfer to an unknown destination, and no CODE.1 or save
    # path has been ruled out.
    indirect_jumps=[
       {"pc":pc,"asm":op+" "+arg}
       for pc,op,arg in rows
       if op=="jmp" and not arg.startswith("0x") and pc<0x40000
    ]
    # Pin original source-backed stream accesses before using the names
    # open/read/write/seek/close in the format report.
    LEVEL_WRITER_WITNESSES={
       0x313A:("call","0x433c0"),0x317E:("call","0x4343c"),
       0x318D:("call","0x4340c"),0x31AB:("call","0x4340c"),
       0x31F5:("call","0x4340c"),0x3291:("call","0x433fc"),
       0x33C1:("call","0x433c0"),0x33D9:("call","0x4343c"),
       0x33EC:("call","0x4340c"),0x3450:("call","0x4340c"),
       0x2445E:("call","0x21db0"),0x244CE:("call","0x433c0"),
       0x244E9:("call","0x40584"),0x24558:("call","0x4340c"),
       0x2462E:("call","0x4340c"),
    }
    pinned=[]
    for pc,(want_op,want_arg) in LEVEL_WRITER_WITNESSES.items():
       got=bypc.get(pc)
       if got!=(want_op,want_arg):
           raise ValueError(f"Stone numbered work-file stream witness drift @{pc:#x}: {got}")
       pinned.append({"pc":pc,"asm":got[0]+" "+got[1]})
    # Code object disassembly is whole-object, so accidental instruction
    # decodes are leads until original CFG/call ownership is established.
    return {
       "original_file_io_helper_excerpts":{
          "numbered_file_handle_open":bounded(rows,0x43390,0x43400,limit=85),
          "stream_io_and_offset_helpers":bounded(rows,0x43400,0x43495,limit=100),
          "missing_work_file_fallback":bounded(rows,0x40550,0x40650,limit=110),
       },
       "verified_writer_machine_witnesses":pinned,
       "indirect_gameplay_call_count_below_0x40000":len([
          rec for rec in indirect if rec["pc"]<0x40000]),
       "indirect_gameplay_jumps":indirect_jumps[:65],
       "indirect_gameplay_jumps_total":len(indirect_jumps),
       "verified_leads":[{"pc":pc,"asm":bypc[pc][0]+" "+bypc[pc][1]}
         for pc in (0x2445E,0x24477,0x2448B)],
       "producer_candidate_machine_windows":{
           k:bounded(rows,lo,hi,limit=280) for k,(lo,hi) in windows.items()},
       "early_file_creation_candidates":{
           "formatter_open_at_0x313A":bounded(rows,0x3010,0x3350,limit=520),
           "formatter_open_at_0x33C1":bounded(rows,0x3350,0x3550,limit=280),
           "early_creature_setup":bounded(rows,0x3500,0x3980,limit=350),
       },
       "candidate_file_io_calls":{
           k:xref(f"0x{pc:x}",max_hits=45)
           for k,pc in {
               "open_read_existing_0x433C0":0x433c0,
               "stream_write_0x4340C":0x4340c,
               "stream_seek_0x4343C":0x4343c,
               "stream_close_0x433FC":0x433fc,
           }.items()
       },
       "filename_buffer_and_decimal_writes":xref("0x1f63",55),
       "actor_live_type_field_write_leads":xref("0x44ff4",50),
       "file_helper_write_read_leads":{
           k:xref(f"0x{p:x}",max_hits=45) for k,p in {
            "open_0x433D4":0x433D4,"open_0x433E8":0x433E8,
            "stream_read_0x43424":0x43424,
            "path_helper_0x434B0":0x434B0,
           }.items()
       },
       "indirect_call_count":len(indirect),
       "indirect_callsite_examples":indirect[:95],
       "scope":"Executable offsets and instruction windows only. Do not assert work-file production, a spawn path, or stock encounter reachability from coincidence.",
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
    code,data,meta=tool["le_image"](root/game["exe"],game["expected"])
    rows=tool["disassembly"](code)
    source_path_proof=work_level_source_proof(rows,data)
    body={
       "code2_sha256":meta["sha256"],
       "numbered_work_level_loader_source_proof":source_path_proof,
       "work_level_producer_and_indirect_dispatch_probe":work_level_producer_and_indirect_dispatch(rows),
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
