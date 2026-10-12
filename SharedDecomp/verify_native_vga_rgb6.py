#!/usr/bin/env python3
"""Independent original CODE.2 + COLORS DAC6 four-plane scanout differential.

This source-only oracle never imports NativeEngine, never executes DOS x86,
never guesses DOS camera or dynamic palette/fade state, and writes only
original-free hash summaries.
"""
from __future__ import annotations
import argparse
import hashlib
import json
from pathlib import Path
import struct
import sys

sys.path.insert(0,str(Path(__file__).resolve().parents[1]/"Resources"/"RES"))
from analyze_res import parse_res,lzss

SCHEMA="rloft-original-code2-colors-vga-rgb6/v1"
COMPARE_SCHEMA="rloft-code2-colors-vga-rgb6-original-differential/v1"
PINNED={
    "CODE.2":"853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314",
    "COLORS":"e366986bf701ba832aa769bf99f0db90a76af658300ce3d02e7039a970a9baf5",
    "RES4":"d8989c50fa58e317ff33465e7baec16e970f40586417d790fd8a1b9005c60f46",
}
sha=lambda b:hashlib.sha256(b).hexdigest()
def require(ok,msg):
    if not ok:raise ValueError(msg)

def source(original:dict,retail:bool=True):
    code=original["CODE.2"];colors=original["COLORS"];res4=original["RES4"]
    if retail:
        for name,hashvalue in PINNED.items():
            require(sha(original[name])==hashvalue,
                    "original "+name+" differs from retail SHA256")
    offset=0x75F24+0x81AC
    require(len(code)>=offset+96,"original CODE.2 display tuple table truncated")
    table=code[offset:offset+96]
    triples=[list(table[i*3:i*3+3]) for i in range(32)]
    require(len(colors)>=1 and colors[0]==15 and len(colors)==11521,
            "COLORS original 15-bank DAC6 layout invalid")
    require(all(v<=63 for v in colors[1:]),
            "COLORS DAC6 component exceeds original range")
    require(all(0<=t[0]<15 for t in triples),
            "CODE.2 world palette index outside COLORS bank domain")
    _,entries=parse_res(res4)
    require(len(entries)>62,"original RES4 diagnostic material ID absent")
    if retail: require(len(entries)==667,"original RES4 entry count invalid")
    entry=entries[62]
    require(entry[:5]==b"EH\x06\x00\x00" and len(entry)>=8 and
            struct.unpack_from("<H",entry,5)[0]==8192,
            "original RES4 diagnostic EH6 source layout invalid")
    decoded,used=lzss(entry[7:],8192)
    require(used==len(entry)-7 and len(decoded)==8192,
            "original RES4 source LZSS bytes not fully consumed")
    planes=[bytearray(65536) for _ in range(4)]
    for column,x,height,shade,page,top in (
        (3,9,128,5,0,0),(42,10,64,7,0,0),(0,11,32,3,1,144)):
        step=0x800000//height
        start=top+(256-height)//2
        for y in range(height):
            source_index=(y*step)>>16
            color=(decoded[column*128+source_index]-shade)&255
            planes[x&3][page*32768+(start+y)*80+x//4]=color
    # Explicit diagnostic only. These are the eight original palette
    # entries distinguishing all four level-selected world banks.
    for x in range(8):
        planes[x&3][80+x//4]=246+x
    def frame_hash(bank,page):
        pal=colors[1+bank*768:1+(bank+1)*768]
        table_rgb=[pal[i*3:i*3+3] for i in range(256)]
        h=hashlib.sha256()
        for y in range(400):
            at=page*32768+y*80
            row=bytearray(960)
            for x in range(320):
                index=planes[x&3][at+(x>>2)]
                row[3*x:3*x+3]=table_rgb[index]
            h.update(row)
        return h.hexdigest()
    cache={bank:frame_hash(bank,0) for bank in sorted({t[0] for t in triples})}
    count=[0]*15
    for t in triples:count[t[0]]+=1
    return {
        "schema":SCHEMA,
        "original_code2_sha256":sha(code),
        "original_colors_sha256":sha(colors),
        "original_res4_sha256":sha(res4),
        "original_level_table_sha256":sha(table),
        "original_level_count":32,
        "original_palette_bank_count":15,
        "original_level_display_triplets":triples,
        "level_palette_histogram":count,
        "diagnostic_res4_id":62,
        "diagnostic_source_reads":224,
        "diagnostic_palette_probe_indexes":list(range(246,254)),
        "diagnostic_probe_pixel_writes":8,
        "diagnostic_four_plane_sha256":sha(b"".join(planes)),
        "original_rgb6_frame_dimensions":[320,400],
        "original_rgb6_frame_level_page0_sha256":[cache[t[0]] for t in triples],
        "original_rgb6_level0_page1_sha256":frame_hash(triples[0][0],1),
        "original_dac6_palette_values_preserved":True,
        "original_vga_plane_addressing_implemented":True,
        "original_video_page0_and_page1_supported":True,
        "original_live_dac_updates_verified":False,
        "original_dos_frame_pixel_parity_verified":False,
        "original_raycaster_camera_verified":False,
    }

def compare(original,native,retail=True):
    require(isinstance(native,dict) and native.get("schema")==SCHEMA,
            "compiled native COLORS RGB6 schema mismatch")
    for flag in ("original_live_dac_updates_verified",
                 "original_dos_frame_pixel_parity_verified",
                 "original_raycaster_camera_verified"):
        require(native.get(flag) is False,
                "unsupported DOS/gameplay frame fidelity claim "+flag)
    expected=source(original,retail=retail)
    differences=[]
    for field,value in expected.items():
        got=native.get(field)
        if field in ("original_level_display_triplets",
                     "original_rgb6_frame_level_page0_sha256") and isinstance(got,list):
            if len(got)!=len(value):
                differences.append({"field":field+"/count",
                    "original":len(value),"native":len(got)})
            for index,(a,b) in enumerate(zip(value,got)):
                if type(a)!=type(b) or a!=b:
                    differences.append({"field":f"{field}/{index}",
                        "original":a,"native":b})
        elif type(got)!=type(value) or got!=value:
            differences.append({"field":field,"original":value,"native":got})
    return {
        "schema":COMPARE_SCHEMA,
        "status":"ORIGINAL_CODE2_DAC6_SCANOUT_MATCH" if not differences else "DIFFERENT",
        "original_inputs_sha256":{name:sha(value) for name,value in original.items()},
        "source_original_level_palette_maps_checked":32,
        "original_colors_palette_banks_checked":15,
        "world_palette_variants_checked":len(set(expected["original_rgb6_frame_level_page0_sha256"])),
        "original_diagnostic_source_reads_checked":224,
        "original_diagnostic_palette_probe_writes_checked":8,
        "original_full_rgb6_page_hashes_checked":33,
        "different_field_count":len(differences),
        "first_difference":differences[0] if differences else None,
        "differences":differences[:32],
        "original_dos_frame_parity_verified":False,
    }

def main(argv=None):
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument("--game-dir",type=Path,required=True)
    p.add_argument("--native-report",type=Path,required=True)
    p.add_argument("--out",type=Path,required=True)
    p.add_argument("--allow-synthetic",action="store_true")
    a=p.parse_args(argv)
    try:
        def unique(pairs):
            obj={}
            for key,value in pairs:
                require(key not in obj,"duplicate native RGB6 JSON key")
                obj[key]=value
            return obj
        def nonfinite(value):
            raise ValueError("nonfinite RGB6 JSON "+value)
        original={}
        for name,maxsize in (("CODE.2",1024*1024),
                             ("COLORS",16384),("RES4",16*1024*1024)):
            file=a.game_dir/name
            require(file.stat().st_size<=maxsize,"original RGB6 source too large")
            original[name]=file.read_bytes()
        report=a.native_report.read_bytes()
        require(len(report)<1024*1024,"native scanout report exceeds byte bound")
        native=json.loads(report.decode("utf-8-sig"),object_pairs_hook=unique,
                          parse_constant=nonfinite)
        result=compare(original,native,retail=not a.allow_synthetic)
        a.out.parent.mkdir(parents=True,exist_ok=True)
        a.out.write_text(json.dumps(result,indent=2,sort_keys=True)+"\n")
        print(json.dumps({"status":result["status"],
                          "differences":result["different_field_count"]}))
        return 0 if not result["different_field_count"] else 1
    except (OSError,ValueError,TypeError,IndexError,struct.error) as err:
        print("ORIGINAL CODE2 DAC6 RGB6 differential ERROR:",err,file=sys.stderr)
        return 2

if __name__=="__main__":
    raise SystemExit(main())
