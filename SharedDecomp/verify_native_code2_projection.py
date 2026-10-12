#!/usr/bin/env python3
"""CODE.2 35680 / 35730 / 35798 original integer projection oracle.

Independent of NativeEngine C++; never executes original DOS machine code.
The original CODE.2 bytes are opened privately, SHA-checked and are NOT
included in this script, test fixtures, output or GitHub Actions artifacts.
"""
from __future__ import annotations
import argparse
import hashlib
import json
from pathlib import Path
import struct
import sys

SCHEMA="rloft-code2-projection-original-arithmetic/v1"
REPORT_SCHEMA="rloft-code2-projection-native-original-differential/v1"
RETAIL_SHA="853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314"
OBJECT_OFFSET=0x3AF24
OBJECT_LENGTH=0x3A852
SPANS={
    "original_helper_35680_sha256":(0x35680,0x356D8,
        "4a0de259caeeb0ea8a47809aefce47c363bf37e956578ad0bb402399cd353df6"),
    "original_height_35730_sha256":(0x35730,0x35797,
        "9c37f50641e5c02afc0d68f0c070ca2b38874be1a600d3eace267c9c3eeb9e60"),
    "original_point_35798_sha256":(0x35798,0x35869,
        "af99ef81b74f6d16cc3cc666717ea76fad4188393dc951ea68bfb01c523550b3"),
}
LABEL=b"RLOFT-CODE2-projection-35680-35730-35798-v1"
MASK=0xFFFFFFFF
def require(ok,why):
    if not ok:raise ValueError(why)
def sha(data):return hashlib.sha256(data).hexdigest()
def unsigned(n):return n&MASK
def signed(n):return n if n<0x80000000 else n-0x100000000
def word(n):return n&0xffff

def original_partial_product(x,coefficient):
    """Translate original low-16 MUL BX, second high-16 MUL BX, ADD/ADC."""
    is_negative_x=bool(x&0x80000000)
    is_negative_coeff=bool(coefficient&0x80000000)
    source_magnitude=unsigned(-x) if is_negative_x else x
    low_product=(source_magnitude&0xffff)*(coefficient&0xffff)
    high_product=((source_magnitude>>16)*(coefficient&0xffff)
                  +(low_product>>16))&MASK
    return unsigned(-high_product) if is_negative_x!=is_negative_coeff else high_product

def idiv_32(n,d):
    require(d!=0,"original signed CODE.2 IDIV denominator zero")
    abs_quotient=abs(n)//abs(d)
    quotient=-abs_quotient if (n<0)!=(d<0) else abs_quotient
    require(-0x80000000<=quotient<=0x7fffffff,
            "original CODE.2 IDIV would raise division overflow")
    return quotient

def wall(r):
    dx=unsigned(r["live_x"]-r["origin_x"])
    dy=unsigned(r["live_y"]-r["origin_y"])
    depth=unsigned(original_partial_product(dx,r["dir_x"])
                   -original_partial_product(dy,r["dir_y"]))
    clamped=signed(depth)<0x1680
    bounded=0x1680 if clamped else depth
    divisor=bounded>>8
    require(divisor>0,"original 35730 DIV denominator zero")
    return (depth,bounded,divisor,word(r["height_num"]//divisor),clamped)

def point(r,x,y,prior_x=1234):
    dx=unsigned(x-r["origin_x"]);dy=unsigned(y-r["origin_y"])
    depth=unsigned(original_partial_product(dx,r["dir_x"])
                   -original_partial_product(dy,r["dir_y"])-0x2000)
    transverse=unsigned(original_partial_product(dx,r["dir_y"])
                       +original_partial_product(dy,r["dir_x"]))
    if signed(depth)<0x5800:
        return (depth,transverse,prior_x,0,False)
    screen=word(r["horizontal_origin"]+
                idiv_32(signed(r["horizontal_scale"])*signed(transverse),
                        signed(depth)))
    projected_height=word(idiv_32(signed(r["height_num"]),signed(depth)>>8))
    return (depth,transverse,screen,projected_height,True)

def reference():
    seed=0xC2A35730
    def nxt():
        nonlocal seed
        seed=unsigned(seed*1664525+1013904223)
        return seed
    encoded=bytearray(LABEL)
    examples=[];accepted=rejected=clamped=0
    for n in range(4096):
        r={}
        r["origin_x"]=nxt()&0x1ffff
        r["origin_y"]=nxt()&0x1ffff
        r["live_x"]=unsigned(r["origin_x"]+(nxt()&0x7ffff))
        r["live_y"]=unsigned(r["origin_y"]+(nxt()&0x7ffff))
        r["dir_x"]=nxt()&0x7fff
        r["dir_y"]=nxt()&0x7fff
        r["height_num"]=0x10000+(nxt()&0x7ffff)
        r["horizontal_scale"]=256+(nxt()&0x3ff)
        r["horizontal_origin"]=word(nxt())
        w=wall(r)
        x=r["origin_x"]+(nxt()&0x7ffff)
        y=r["origin_y"]+(nxt()&0x7ffff)
        p=point(r,x,y)
        accepted+=int(p[4])
        rejected+=int(not p[4])
        clamped+=int(w[4])
        encoded.extend(struct.pack("<IIIHIIHHBB",
            w[0],w[1],w[2],w[3],
            p[0],p[1],p[2],p[3],int(p[4]),int(w[4])))
        if n<8:
            examples.append({
                "case":n,"wall_depth":w[0],"wall_height":w[3],
                "near_clamped":w[4],"point_depth":p[0],
                "point_transverse":p[1],"accepted":p[4],
                "screen_word":p[2],"height_word":p[3]})
    return {
        "deterministic_cases":4096,
        "input_seed":0xC2A35730,
        "first_samples":examples,
        "accepted_projected_points":accepted,
        "near_rejected_points":rejected,
        "wall_near_clamped_cases":clamped,
        "encoded_case_bytes":len(encoded)-len(LABEL),
        "original_algorithm_case_digest":sha(encoded),
        "original_fixed_point_arithmetic_implemented":True,
        "original_live_camera_state_verified":False,
        "original_ray_traversal_verified":False,
        "original_full_frame_parity_verified":False,
    }

def original_code2_signatures(data,require_retail=True):
    if require_retail:
        require(len(data)==558689 and sha(data)==RETAIL_SHA,
                "original CODE.2 retail SHA256 or length mismatch")
    require(len(data)>=OBJECT_OFFSET+OBJECT_LENGTH,
            "original CODE.2 object-1 source segment truncated")
    signatures={}
    for key,(a,b,pinned) in SPANS.items():
        digest=sha(data[OBJECT_OFFSET+a:OBJECT_OFFSET+b])
        require(not require_retail or digest==pinned,
                "original CODE.2 protected instruction span mismatch "+key)
        signatures[key]=digest
    return {"original_code2_sha256":sha(data),**signatures}

def compare(code2,native,require_retail=True):
    require(isinstance(native,dict) and native.get("schema")==SCHEMA,
            "CODE.2 native projection schema invalid")
    for tag in ("original_live_camera_state_verified",
                "original_ray_traversal_verified",
                "original_full_frame_parity_verified"):
        require(native.get(tag) is False,
                "unsupported original DOS runtime projection/frame parity claim")
    expected={**original_code2_signatures(code2,require_retail),**reference()}
    mismatches=[]
    for key,correct in expected.items():
        got=native.get(key)
        if key=="first_samples" and isinstance(got,list):
            if len(got)!=len(correct):
                mismatches.append({"field":"first_samples/count",
                                   "expected":len(correct),"actual":len(got)})
            for i,(a,b) in enumerate(zip(correct,got)):
                for field,value in a.items():
                    found=b.get(field) if isinstance(b,dict) else None
                    if type(found)!=type(value) or found!=value:
                        mismatches.append({"field":f"first_samples/{i}/{field}",
                                           "expected":value,"actual":found})
        elif type(got)!=type(correct) or got!=correct:
            mismatches.append({"field":key,"expected":correct,"actual":got})
    return {
        "schema":REPORT_SCHEMA,
        "status":"MATCH" if not mismatches else "DIFFERENT",
        "original_code2_sha256":sha(code2),
        "original_sha256_spans_verified":len(SPANS),
        "deterministic_case_count":4096,
        "independently_checked_case_bytes":reference()["encoded_case_bytes"],
        "difference_count":len(mismatches),
        "first_difference":mismatches[0] if mismatches else None,
        "differences":mismatches[:32],
        "original_dos_frame_parity_verified":False,
    }

def main(argv=None):
    p=argparse.ArgumentParser(description=__doc__)
    for flag in ("code2","native-report","out"):
        p.add_argument("--"+flag,required=True,type=Path)
    p.add_argument("--allow-other-version",action="store_true")
    args=p.parse_args(argv)
    try:
        def unique(pairs):
            result={}
            for k,v in pairs:
                require(k not in result,"duplicate native projection JSON key "+k)
                result[k]=v
            return result
        def nonfinite(v):raise ValueError("nonfinite JSON value "+v)
        code2=args.code2.read_bytes()
        require(0<len(code2)<1024*1024,"original CODE.2 file too large")
        native_bytes=args.native_report.read_bytes()
        require(0<len(native_bytes)<1024*1024,
                "native projection report size invalid")
        native=json.loads(native_bytes.decode("utf-8-sig"),
            object_pairs_hook=unique,parse_constant=nonfinite)
        result=compare(code2,native,not args.allow_other_version)
        args.out.parent.mkdir(parents=True,exist_ok=True)
        args.out.write_text(json.dumps(result,indent=2,sort_keys=True)+"\n")
        print(json.dumps({"status":result["status"],
                          "differences":result["difference_count"]}))
        return 0 if result["difference_count"]==0 else 1
    except (OSError,UnicodeError,ValueError,TypeError,KeyError,struct.error) as e:
        print("CODE.2 PROJECTION DIFFERENTIAL ERROR:",e,file=sys.stderr)
        return 2
if __name__=="__main__":
    raise SystemExit(main())
