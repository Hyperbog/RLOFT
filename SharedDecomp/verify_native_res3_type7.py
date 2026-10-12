#!/usr/bin/env python3
"""Independent original RES3 EH7 vs compiled NativeEngine 4234-shape oracle.

Uses the canonical original CODE.2 Python decoder in Resources/RES, not
NativeEngine C++ algorithms or exported bytes. Original-free CI fixtures.
"""
from __future__ import annotations

import argparse
from collections import Counter
import hashlib
import json
from pathlib import Path
import struct
import sys

# Different algorithm/codebase from NativeEngine. Existing authoritative
# code1-decompilation LZSS and EH7 parser are the original-side witnesses.
sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "Resources" / "RES"))
from analyze_res import lzss, parse_res
from analyze_type7 import validate_body

SCHEMA = "rloft-native-res3-type7-shapes/v1"
RESULT_SCHEMA = "rloft-original-res3-eh7-native-differential/v1"
RETAIL = {
    "INIT": "cc48df4eb4c0710d45e7f5557e440129df44c5efb1af3ca3426ea9f9e3798276",
    "RES3": "44955ebbed0a8ed8d874fae07b01710317044882ab3c7c95d9be54e883df9a36",
}
MAX_RES3 = 32 * 1024 * 1024
MAX_REPORT = 8 * 1024 * 1024


def require(ok, reason):
    if not ok:
        raise ValueError(reason)


def sha(data):
    return hashlib.sha256(data).hexdigest()


def source_fingerprints(original_res3: bytes, init: bytes) -> dict:
    require(len(init) >= 0x24, "original INIT too short for RES3 source banks")
    bases = list(struct.unpack_from("<4H", init, 0x06))
    _, entries = parse_res(original_res3)
    count = len(entries)
    require(bases[0] == 0 and all(a < b for a,b in zip(bases,bases[1:]))
            and bases[-1] < count,
            "original INIT source-bank starts invalid for RES3 archive")
    lengths = [b-a for a,b in zip(bases,bases[1:]+[count])]
    widths=Counter()
    referenced=0
    unique=0
    total_bytes=0
    shapes=[]
    for index,entry in enumerate(entries):
        require(len(entry)>=8 and entry[:5]==b"EH\x07\x00\x00",
                f"original RES3 entry {index} not expected EH7")
        size=struct.unpack_from("<H",entry,5)[0]
        body,used=lzss(entry[7:],size)
        require(used==len(entry)-7,
                f"original RES3 entry {index} LZSS trailing bytes")
        geometry=validate_body(body)
        columns=geometry["column_count"]
        ptrs=struct.unpack_from(f"<{columns}H",body,10)
        descriptor_count_by_pointer={}
        for ptr in set(ptrs):
            n=0
            cursor=ptr
            while True:
                selector=struct.unpack_from("<H",body,cursor)[0]
                if selector==0:break
                cursor+=6
                n+=1
            descriptor_count_by_pointer[ptr]=n
        local_references=sum(descriptor_count_by_pointer[p] for p in ptrs)
        local_unique=sum(descriptor_count_by_pointer.values())
        widths[geometry["source_width"]]+=1
        referenced+=local_references
        unique+=len(descriptor_count_by_pointer)
        total_bytes+=len(body)
        shapes.append({
            "id":index,
            "eh7_entry_sha256":sha(entry),
            "eh7_body_sha256":sha(body),
            "decoded_body_bytes":len(body),
            "source_width":geometry["source_width"],
            "source_columns":columns,
            "unique_column_lists":len(descriptor_count_by_pointer),
            "raster_descriptors_with_column_references":local_references,
            "raster_descriptors_unique_lists":local_unique,
        })
    return {
        "res3_entries":count,
        "decoded_eh7_shapes":count,
        "unique_column_lists":unique,
        "raster_descriptors":referenced,
        "total_decoded_body_bytes":total_bytes,
        "width_64":widths[64],
        "width_96":widths[96],
        "width_128":widths[128],
        "source_class_lengths":lengths,
        "shapes":shapes,
        "original_init_sha256":sha(init),
        "original_res3_sha256":sha(original_res3),
    }


def compare(res3: bytes, init: bytes, native: dict, require_retail=True) -> dict:
    require(isinstance(native,dict) and native.get("schema")==SCHEMA,
            "compiled native RES3 schema mismatch")
    for field in ("sintab_x86_code_executed", "pixel_color_parity_verified",
                  "original_scene_frame_parity_verified", "event_vm_executed"):
        require(native.get(field) is False,
                "unsupported original rendering/gameplay parity claim "+field)
    if require_retail:
        for name,content in (("INIT",init),("RES3",res3)):
            require(sha(content)==RETAIL[name],
                    "original "+name+" SHA256 not the pinned retail source")
    expected=source_fingerprints(res3,init)
    differences=[]
    for key,correct in expected.items():
        observed=native.get(key)
        if key=="shapes":
            if not isinstance(observed,list) or len(observed)!=len(correct):
                differences.append({"field":"shapes/length",
                                    "original":len(correct),
                                    "native":len(observed) if isinstance(observed,list) else None})
                continue
            for index,(source_row,native_row) in enumerate(zip(correct,observed)):
                for subkey,source_value in source_row.items():
                    value=native_row.get(subkey) if isinstance(native_row,dict) else None
                    if type(value) is not type(source_value) or value != source_value:
                        differences.append({"field":f"shapes/{index}/{subkey}",
                                            "original":source_value,"native":value})
        elif type(observed) is not type(correct) or observed != correct:
            differences.append({"field":key,"original":correct,
                                "native":observed})
    return {
        "schema":RESULT_SCHEMA,
        "status":"ALL_ORIGINAL_EH7_SHAPES_MATCH" if not differences else "DIFFERENT",
        "original_res3_sha256":sha(res3),
        "original_init_sha256":sha(init),
        "original_retail_sha256_attested":require_retail,
        "original_entries_compared":expected["res3_entries"],
        "original_decoded_body_digests_compared":expected["decoded_eh7_shapes"],
        "original_entry_digests_compared":expected["res3_entries"],
        "original_per_shape_metric_fields_compared":len(expected["shapes"])*9,
        "original_unique_lists":expected["unique_column_lists"],
        "original_descriptor_references":expected["raster_descriptors"],
        "original_decoded_bytes":expected["total_decoded_body_bytes"],
        "first_divergence":differences[0] if differences else None,
        "difference_count":len(differences),
        "differences":differences[:32],
        "original_x86_code_executed":False,
        "original_pixel_parity_verified":False,
    }


def main(argv=None):
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument("--res3",required=True,type=Path)
    p.add_argument("--init",required=True,type=Path)
    p.add_argument("--native-report",required=True,type=Path)
    p.add_argument("--report",required=True,type=Path)
    p.add_argument("--allow-other-version",action="store_true")
    args=p.parse_args(argv)
    try:
        def limited(path,maximum):
            size=path.stat().st_size
            require(0<size<=maximum,f"source report {path.name} exceeds byte budget")
            return path.read_bytes()
        def unique(pairs):
            obj={}
            for k,v in pairs:
                require(k not in obj,"duplicate native JSON field")
                obj[k]=v
            return obj
        def no_nonfinite(v):
            raise ValueError("non-finite JSON number")
        source_res3=limited(args.res3,MAX_RES3)
        original_init=limited(args.init,65536)
        native=json.loads(limited(args.native_report,MAX_REPORT).decode("utf-8-sig"),
                          object_pairs_hook=unique,parse_constant=no_nonfinite)
        report=compare(source_res3,original_init,native,
                       require_retail=not args.allow_other_version)
        args.report.parent.mkdir(parents=True,exist_ok=True)
        args.report.write_text(json.dumps(report,indent=2,sort_keys=True)+"\n",
                               encoding="utf-8")
        print(json.dumps({"status":report["status"],
                          "shapes":report["original_entries_compared"],
                          "mismatches":report["difference_count"]}))
        return 0 if report["status"]=="ALL_ORIGINAL_EH7_SHAPES_MATCH" else 1
    except (OSError,ValueError,TypeError,KeyError,struct.error,
            UnicodeError) as exc:
        print("RES3 ORIGINAL DIFFERENTIAL ERROR:",exc,file=sys.stderr)
        return 2


if __name__=="__main__":
    raise SystemExit(main())
