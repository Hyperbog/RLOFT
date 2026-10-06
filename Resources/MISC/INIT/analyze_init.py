#!/usr/bin/env python3
from __future__ import annotations
import argparse, hashlib, struct
from dataclasses import dataclass
from pathlib import Path

@dataclass
class SceneDesc:
    local_shape_base:int; spatial_extent:int; shape_stride:int; variant_count:int; scene_influence_step:int

@dataclass
class EnvPlacement:
    resource_selector:int; azimuth_deg:int; draw_width:int; screen_y:int

@dataclass
class EnvProfile:
    res8_base:int; stored_count:int; placements:list[EnvPlacement]

@dataclass
class InitFile:
    header_words:list[int]; class1:list[SceneDesc]; class3:list[SceneDesc]; env_profiles:list[EnvProfile]

def u16(b,o): return struct.unpack_from('<H',b,o)[0]
def i16(b,o): return struct.unpack_from('<h',b,o)[0]

def parse(data:bytes)->InitFile:
    if len(data)<0x24: raise ValueError('INIT shorter than fixed header')
    p=0
    header=list(struct.unpack_from('<18H',data,p)); p+=0x24
    def read_desc_table():
        nonlocal p
        if p+2>len(data): raise ValueError('truncated descriptor count')
        n=u16(data,p); p+=2
        out=[]
        for _ in range(n):
            if p+7>len(data): raise ValueError('truncated 7-byte descriptor')
            out.append(SceneDesc(u16(data,p),u16(data,p+2),data[p+4],data[p+5],data[p+6])); p+=7
        return out
    class1=read_desc_table()
    class3=read_desc_table()
    if p+2>len(data): raise ValueError('missing environment profile count')
    ng=u16(data,p); p+=2
    env=[]
    for gi in range(ng):
        if p+4>len(data): raise ValueError(f'truncated environment profile {gi} header')
        base=u16(data,p); n=u16(data,p+2); p+=4
        recs=[]
        for _ in range(n):
            if p+8>len(data): raise ValueError(f'truncated environment profile {gi} placement')
            recs.append(EnvPlacement(i16(data,p),i16(data,p+2),i16(data,p+4),i16(data,p+6))); p+=8
        env.append(EnvProfile(base,n,recs))
    if p!=len(data): raise ValueError(f'{len(data)-p} trailing bytes after parsed INIT structure')
    return InitFile(header,class1,class3,env)

def serialize(x:InitFile)->bytes:
    out=bytearray(struct.pack('<18H',*x.header_words))
    for table in (x.class1,x.class3):
        out += struct.pack('<H',len(table))
        for d in table:
            out += struct.pack('<HHBBB',d.local_shape_base,d.spatial_extent,d.shape_stride,d.variant_count,d.scene_influence_step)
    out += struct.pack('<H',len(x.env_profiles))
    for g in x.env_profiles:
        out += struct.pack('<HH',g.res8_base,g.stored_count)
        if g.stored_count != len(g.placements): raise ValueError('stored_count mismatch')
        for r in g.placements:
            out += struct.pack('<hhhh',r.resource_selector,r.azimuth_deg,r.draw_width,r.screen_y)
    return bytes(out)

def res_count(path:Path)->int:
    b=path.read_bytes()
    if len(b)<4:return -1
    return struct.unpack_from('<I',b,0)[0]

def main():
    ap=argparse.ArgumentParser()
    ap.add_argument('init',type=Path)
    ap.add_argument('--res8',type=Path)
    args=ap.parse_args()
    data=args.init.read_bytes(); x=parse(data)
    rebuilt=serialize(x)
    print('bytes',len(data))
    print('sha256',hashlib.sha256(data).hexdigest())
    print('round_trip_identical',rebuilt==data)
    print('header_words',x.header_words)
    print('class1_descriptors',len(x.class1))
    print('class3_descriptors',len(x.class3))
    print('environment_profiles',len(x.env_profiles))
    print('scene_descriptor_counts_expected',len(x.class1)==175 and len(x.class3)==12)
    rc=res_count(args.res8) if args.res8 else None
    for i,g in enumerate(x.env_profiles):
        active=[]
        for j,r in enumerate(g.placements):
            if r.resource_selector==0: continue
            rid=r.resource_selector + g.res8_base if r.resource_selector>=8 else r.resource_selector
            active.append((j,r,rid))
        zero_indices=[j for j,r in enumerate(g.placements) if r.resource_selector==0]
        print(f'profile {i}: res8_base={g.res8_base} stored={g.stored_count} active={len(active)} zero_indices={zero_indices}')
        print('  resolved_RES8_ids',sorted({rid for _,_,rid in active}))
        print('  azimuth_range',min((r.azimuth_deg for _,r,_ in active),default=None),max((r.azimuth_deg for _,r,_ in active),default=None))
        print('  width_range',min((r.draw_width for _,r,_ in active),default=None),max((r.draw_width for _,r,_ in active),default=None))
        print('  y_range',min((r.screen_y for _,r,_ in active),default=None),max((r.screen_y for _,r,_ in active),default=None))
        if rc is not None:
            bad=[rid for _,_,rid in active if not (0<=rid<rc)]
            print('  res8_count',rc,'all_resolved_ids_valid',not bad,'bad',bad)
        print('  terminal_zero_record',bool(g.placements and g.placements[-1]==EnvPlacement(0,0,0,0)))
        print('  premature_zero_record',any(r.resource_selector==0 for r in g.placements[:-1]))

if __name__=='__main__': main()
