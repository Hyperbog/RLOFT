#!/usr/bin/env python3
"""Independent original RES4 EH6 fixed-point column scaler differential.

Original CODE.2 338C8: step=0x800000/height, source column stride=128,
shade subtraction modulo 256, 80 bytes per VGA plane row. Diagnostic
heights/positions are explicit, NOT a recovered game camera or scene.
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

SCHEMA="rloft-native-type6-original-texture-strips/v1"
COMPARISON_SCHEMA="rloft-native-original-res4-texture-sampler-differential/v1"
RETAIL={
 "RES4":"d8989c50fa58e317ff33465e7baec16e970f40586417d790fd8a1b9005c60f46",
 "INIT":"cc48df4eb4c0710d45e7f5557e440129df44c5efb1af3ca3426ea9f9e3798276",
 "COLORS":"e366986bf701ba832aa769bf99f0db90a76af658300ce3d02e7039a970a9baf5",
}

def require(ok,why):
    if not ok:raise ValueError(why)
def sha(data):return hashlib.sha256(data).hexdigest()

def frame_digest(original_column_major,width):
    planes=[bytearray(65536) for _ in range(4)]
    cases=((0,0,128,0,0,0),
           (width//2,1,64,7,0,0),
           (width-1,2,256,255,0,0),
           (width//2,3,32,3,1,144))
    for column,x,height,shade,page,origin in cases:
        step=0x800000//height
        first=origin+(256-height)//2
        for y in range(height):
            texel=original_column_major[column*128+((y*step)>>16)]
            planes[x&3][page*32768+(first+y)*80+x//4]=(texel-shade)&255
    return sha(b"".join(planes))

def independent_source(res4,init,colors,retail=False):
    require(len(init)>=16 and len(colors)>=1 and
            (len(colors)-1)%768==0 and
            colors[0]==(len(colors)-1)//768 and
            colors[0]>=1 and all(x<=63 for x in colors[1:]),
            "original INIT / 6-bit COLORS bank validation failed")
    narrow=struct.unpack_from("<H",init,14)[0]
    _,entries=parse_res(res4)
    require(0<narrow<len(entries),"original RES4 narrow source bank invalid")
    if retail:
        require(len(entries)==667 and narrow==600 and colors[0]==15,
                "retail original RES4 cardinality or palette banks incorrect")
    rows=[]
    identity=bytearray(b"RLOFT-native-RES4-strips-v1")
    wide=narrow_count=0
    for index,entry in enumerate(entries):
        require(len(entry)>=8 and entry[:5]==b"EH\x06\x00\x00",
                f"original RES4 entry {index} not EH6")
        size=struct.unpack_from("<H",entry,5)[0]
        require(size in (4096,8192),
                f"original RES4 entry {index} body shape unsupported")
        width=size//128
        require(width==(64 if index<narrow else 32),
                f"original RES4 entry {index} source family width mismatch")
        decoded,used=lzss(entry[7:],size)
        require(used==len(entry)-7 and len(decoded)==size,
                f"original RES4 entry {index} LZSS consumption mismatch")
        row_major=bytes(decoded[x*128+y] for y in range(128)
                        for x in range(width))
        fingerprints=(sha(entry),sha(row_major),frame_digest(decoded,width))
        rows.append({
            "id":index,
            "width":width,
            "height":128,
            "original_entry_sha256":fingerprints[0],
            "decoded_row_major_sha256":fingerprints[1],
            "diagnostic_four_plane_sha256":fingerprints[2]})
        wide+=width==64
        narrow_count+=width==32
        identity+=struct.pack("<HH",index,width)
        for d in fingerprints:identity+=d.encode("ascii")
    return {
        "schema":SCHEMA,
        "original_res4_sha256":sha(res4),
        "original_init_sha256":sha(init),
        "original_colors_sha256":sha(colors),
        "original_res4_entries":len(entries),
        "original_init_narrow_bank_start":narrow,
        "validated_palette_banks":colors[0],
        "textures":rows,
        "wide_64x128_entries":wide,
        "narrow_32x128_entries":narrow_count,
        "source_texel_reads":len(entries)*480,
        "plane_writes":len(entries)*480,
        "original_wide_source_base":0,
        "original_narrow_source_base":narrow,
        "combined_diagnostic_sha256":sha(identity),
        "original_x86_executed":False,
        "original_camera_projection_verified":False,
        "original_wall_order_verified":False,
        "original_dos_frame_parity_verified":False,
    }

def compare(res4,init,colors,native,retail=True):
    require(isinstance(native,dict) and native.get("schema")==SCHEMA,
            "original RES4 native sampler schema incorrect")
    for f in ("original_x86_executed",
              "original_camera_projection_verified",
              "original_wall_order_verified",
              "original_dos_frame_parity_verified"):
        require(native.get(f) is False,
                "unsupported original DOS frame/renderer execution claim "+f)
    for name,b in (("RES4",res4),("INIT",init),("COLORS",colors)):
        if retail:require(sha(b)==RETAIL[name],
                          "original "+name+" not pinned retail SHA256")
    expected=independent_source(res4,init,colors,retail=retail)
    mismatches=[]
    for k,value in expected.items():
        observed=native.get(k)
        if k=="textures":
            if not isinstance(observed,list) or len(observed)!=len(value):
                mismatches.append({"field":"textures/count","original":len(value),
                                   "native":len(observed) if isinstance(observed,list) else None})
                continue
            for i,(a,b) in enumerate(zip(value,observed)):
                for field,correct in a.items():
                    got=b.get(field) if isinstance(b,dict) else None
                    if type(got)!=type(correct) or got!=correct:
                        mismatches.append({"field":f"textures/{i}/{field}",
                                           "original":correct,"native":got})
        elif type(observed)!=type(value) or observed!=value:
            mismatches.append({"field":k,"original":value,"native":observed})
    return {
        "schema":COMPARISON_SCHEMA,
        "status":"MATCH" if not mismatches else "DIFFERENT",
        "original_res4_sha256":sha(res4),
        "original_init_sha256":sha(init),
        "original_colors_sha256":sha(colors),
        "original_res4_entries":len(expected["textures"]),
        "per_entry_typed_fields_compared":len(expected["textures"])*6,
        "original_texture_texel_reads":expected["source_texel_reads"],
        "native_plane_writes":expected["plane_writes"],
        "aggregate_fingerprint_sha256":expected["combined_diagnostic_sha256"],
        "difference_count":len(mismatches),
        "first_difference":mismatches[0] if mismatches else None,
        "differences":mismatches[:32],
        "original_camera_projection_verified":False,
        "original_frame_pixel_parity_verified":False,
    }

def main(argv=None):
    p=argparse.ArgumentParser(description=__doc__)
    for key in ("res4","init","colors","native-report","out"):
        p.add_argument("--"+key,required=True,type=Path)
    p.add_argument("--allow-other-version",action="store_true")
    a=p.parse_args(argv)
    try:
        def limited(path,maximum):
            size=path.stat().st_size
            require(0<size<=maximum,"original source/native report exceeds byte limit")
            return path.read_bytes()
        def unique(items):
            result={}
            for k,v in items:
                require(k not in result,"duplicate native JSON key "+k)
                result[k]=v
            return result
        def finite(value):raise ValueError("nonfinite native JSON "+value)
        native=json.loads(limited(a.native_report,4*1024*1024).decode("utf-8-sig"),
                          object_pairs_hook=unique,parse_constant=finite)
        result=compare(limited(a.res4,16*1024*1024),
                       limited(a.init,65536),
                       limited(a.colors,100*1024),
                       native,retail=not a.allow_other_version)
        a.out.parent.mkdir(parents=True,exist_ok=True)
        a.out.write_text(json.dumps(result,indent=2,sort_keys=True)+"\n")
        print(json.dumps({"status":result["status"],
                          "res4":result["original_res4_entries"],
                          "mismatches":result["difference_count"]}))
        return 0 if not result["difference_count"] else 1
    except (OSError,ValueError,KeyError,TypeError,struct.error,UnicodeError) as e:
        print("ORIGINAL RES4 FIXED-POINT DIFFERENTIAL ERROR:",e,file=sys.stderr)
        return 2
if __name__=="__main__":
    raise SystemExit(main())
