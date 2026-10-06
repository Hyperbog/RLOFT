#!/usr/bin/env python3
"""Validate Ravenloft's SINTAB renderer table / microcode package."""
from __future__ import annotations
import argparse, hashlib, math, struct
from pathlib import Path

PREFIX0=0x708
PREFIX1=0xE10
WORD_TABLE_BYTES=0x100
DWORD_TABLE_BYTES=0x400
SCALE_PREFIX=0x204
TRAILER_BYTES=0x200


def parse(data: bytes):
    if len(data) < 0x1F42:
        raise ValueError("short SINTAB")
    off=0
    sine=struct.unpack_from("<450I",data,off); off+=PREFIX0
    tangent=struct.unpack_from("<900I",data,off); off+=PREFIX1
    table_a=struct.unpack_from("<128H",data,off); off+=WORD_TABLE_BYTES
    table_b=struct.unpack_from("<128H",data,off); off+=WORD_TABLE_BYTES
    globals8=struct.unpack_from("<8I",data,off); off+=32
    angle=struct.unpack_from("<256i",data,off); off+=DWORD_TABLE_BYTES
    block_count, lookup_max, aux = struct.unpack_from("<III",data,off); off+=12
    lookup_words=lookup_max+1
    lookup=struct.unpack_from(f"<{lookup_words}H",data,off); off+=lookup_words*2
    code_total=struct.unpack_from("<I",data,off)[0]; off+=4

    blocks=[]
    code_sum=0
    for i in range(block_count):
        if off+2>len(data): raise ValueError(f"block {i} size EOF")
        size=struct.unpack_from("<H",data,off)[0]; off+=2
        if off+size>len(data): raise ValueError(f"block {i} EOF")
        raw=data[off:off+size]; off+=size; code_sum += size
        if size < SCALE_PREFIX: raise ValueError(f"block {i} < 0x204")
        patch=struct.unpack_from("<129H",raw,0)
        steps=struct.unpack_from("<129H",raw,0x102)
        if not all(SCALE_PREFIX <= x < size for x in patch):
            raise ValueError(f"block {i} patch offset out of range")
        if not all(a<=b for a,b in zip(patch,patch[1:])):
            raise ValueError(f"block {i} patch offsets decrease")
        if raw[-1] != 0xC3:
            raise ValueError(f"block {i} generated code does not end in RET")
        blocks.append(dict(index=i,size=size,patch=patch,steps=steps,
                           step_sum=sum(steps),code_bytes=size-SCALE_PREFIX))

    trailer=data[off:]
    if code_sum != code_total:
        raise ValueError(f"block size sum {code_sum} != stored {code_total}")
    if len(trailer)!=TRAILER_BYTES or any(trailer):
        raise ValueError(f"trailer is not 0x200 zero bytes: {len(trailer)}")

    sine_bad=[]
    for deg,x in enumerate(sine):
        mag=x&0x7fffffff
        expected=min(65535,int(abs(math.sin(math.radians(deg)))*65536))
        if abs(mag-expected)>1: sine_bad.append((deg,mag,expected))
    tangent_bad=[]
    for i,x in enumerate(tangent):
        deg=(i+1)/10 if i<450 else i/10
        expected=int(math.tan(math.radians(deg))*65536)
        if abs(x-expected)>2: tangent_bad.append((i,x,expected))

    return dict(
        bytes=len(data), sha256=hashlib.sha256(data).hexdigest(),
        sine_entries=len(sine), sine_formula_outliers=len(sine_bad),
        tangent_entries=len(tangent), tangent_formula_outliers=len(tangent_bad),
        projection_a_monotonic=all(a<=b for a,b in zip(table_a,table_a[1:])),
        projection_b_monotonic=all(a<=b for a,b in zip(table_b,table_b[1:])),
        projection_a_first_last=(table_a[0],table_a[-1]),
        projection_b_first_last=(table_b[0],table_b[-1]),
        renderer_constants=globals8,
        view_angle_first_center_last=(angle[0],angle[128],angle[-1]),
        scale_block_count=block_count, runtime_lookup_max=lookup_max,
        renderer_aux_value=aux, runtime_lookup_words=len(lookup),
        scale_block_payload_bytes=code_total, scale_block_size_sum=code_sum,
        scale_block_size_minmax=(min(b['size'] for b in blocks),max(b['size'] for b in blocks)),
        generated_code_size_minmax=(min(b['code_bytes'] for b in blocks),max(b['code_bytes'] for b in blocks)),
        scale_step_sum_first_last=(blocks[0]['step_sum'],blocks[-1]['step_sum']),
        all_patch_ranges_valid=True, all_patch_tables_monotonic=True,
        all_generated_code_ends_ret=True,
        trailer_bytes=len(trailer), trailer_all_zero=not any(trailer),
        parsed_end=off, file_end=len(data),
    )


def main():
    ap=argparse.ArgumentParser(); ap.add_argument("file",type=Path); args=ap.parse_args()
    r=parse(args.file.read_bytes())
    for k,v in r.items(): print(f"{k}: {v}")
    return 0

if __name__=="__main__": raise SystemExit(main())
