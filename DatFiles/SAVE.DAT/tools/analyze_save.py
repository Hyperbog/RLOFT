#!/usr/bin/env python3
"""Analyze the serializer-certified Ravenloft: Strahd's Possession SAVE.DAT layout."""
from __future__ import annotations
import argparse
import struct
from pathlib import Path

CHARACTER_SIZE=0x248; PREFIX_SIZE=0x77; PARTY_COUNT=4; PARTY_OFFSET=PREFIX_SIZE
MIDDLE_OFFSET=PARTY_OFFSET+CHARACTER_SIZE*PARTY_COUNT
WORLD_OBJECT_SIZE=0x16; WORLD_OBJECT_LIMIT=0x96
# CODE.2 descriptor table 0x7330, recovered from initialized image at physical 0x7D254.
CONTAINER_SLOT_COUNTS=(4,8,8,8,6,6,8,6,8,8,4,8,8,6,8,4,8,4,8,8,8,8,8,8,6,8,8,4,6,8,8,8,8,8,8,8,8,8,4,8,8,8,4,8,8,6,6,8,8,8,4,8,8,8,6,8,4,6,8,8,6,8)
CONTAINER_STATE_COUNT=len(CONTAINER_SLOT_COUNTS)
CONTAINER_MUTABLE_PAYLOAD_SIZE=sum(2*n+1 for n in CONTAINER_SLOT_COUNTS)
POST_CONTAINER_GLOBALS_SIZE=8; CHARGES_SIZE=15; ROSTER_COUNT=10
ROSTER_SIZE=CHARACTER_SIZE*ROSTER_COUNT; TAIL_SIZE=0x10; REFERENCE_SIZE=0x2448

def u16(d,o): return struct.unpack_from('<H',d,o)[0]
def s16(d,o): return struct.unpack_from('<h',d,o)[0]
def u32(d,o): return struct.unpack_from('<I',d,o)[0]
def c_string(d,o,limit=50): return d[o:o+limit].split(b'\0',1)[0].decode('cp437',errors='replace')
def require(d,end,label):
    if end>len(d): raise SystemExit(f'short SAVE while reading {label}: need 0x{end:X}, file is 0x{len(d):X}')
def hex_preview(d,o,n,limit=32): return d[o:o+min(n,limit)].hex(' ')+(' ...' if n>limit else '')

def main():
    ap=argparse.ArgumentParser(); ap.add_argument('save',type=Path); a=ap.parse_args(); d=a.save.read_bytes()
    require(d,MIDDLE_OFFSET+4,'fixed prefix/party/world-object count')
    print(f'file: {a.save}\nsize: {len(d)} bytes (0x{len(d):X})')
    print(f'reference quick-start size: 0x{REFERENCE_SIZE:X} (not universal)')
    print('\nFixed global prefix')
    print(f"  description : {c_string(d,0,50)!r}")
    print(f"  event flags : {d[0x32:0x52].hex(' ')}")
    print(f'  level/map   : {u32(d,0x52)}')
    print(f'  coordinate A: 0x{u32(d,0x56):08X}\n  coordinate B: 0x{u32(d,0x5A):08X}')
    print(f'  facing      : {u32(d,0x5E)}\n  game clock  : {u32(d,0x6C)}')
    print('\nActive party')
    for i in range(PARTY_COUNT):
        o=PARTY_OFFSET+i*CHARACTER_SIZE; print(f'  party[{i}] @ 0x{o:04X}: {c_string(d,o,16)!r}')
    count=u32(d,MIDDLE_OFFSET)
    if count>WORLD_OBJECT_LIMIT: raise SystemExit(f'invalid live-world-object count {count}')
    world_start=MIDDLE_OFFSET+4; world_end=world_start+count*WORLD_OBJECT_SIZE; require(d,world_end,'world objects')
    links_off=world_end; counter_off=links_off+8; bank_off=counter_off+4
    print('\nDynamic middle state')
    print(f'  live-world-object count : {count}')
    print(f'  D8A8 dwords             : [{u32(d,links_off)}, {u32(d,links_off+4)}]')
    print(f'  A7E4 state              : {u32(d,counter_off)}')
    print(f'  container descriptor slots sum: {sum(CONTAINER_SLOT_COUNTS)}')
    print(f'  container serialized bytes    : 0x{CONTAINER_MUTABLE_PAYLOAD_SIZE:X}')
    p=bank_off; nonempty=0
    for i,n in enumerate(CONTAINER_SLOT_COUNTS):
        end=p+2*n+1; require(d,end,f'container state {i}')
        ids=[s16(d,p+2*j) for j in range(n)]; bits=d[p+2*n]
        if any(x!=-1 for x in ids) or bits: nonempty+=1
        print(f'    state[{i:02d}] @ 0x{p:04X} slots={n} ids={ids} bits=0x{bits:02X}')
        p=end
    post_container_off=p
    if post_container_off-bank_off != CONTAINER_MUTABLE_PAYLOAD_SIZE: raise SystemExit('container width mismatch')
    print(f'  nonempty container states     : {nonempty}/{CONTAINER_STATE_COUNT}')
    print(f'  F920                          : 0x{post_container_off:04X} = {u32(d,post_container_off)}')
    print(f'  F91C                          : 0x{post_container_off+4:04X} = {u32(d,post_container_off+4)}')
    charges_off=post_container_off+POST_CONTAINER_GLOBALS_SIZE; roster_off=charges_off+CHARGES_SIZE
    tail_off=roster_off+ROSTER_SIZE; expected_end=tail_off+TAIL_SIZE; require(d,expected_end,'roster/tail')
    print('\nLate serializer blocks')
    print(f'  charges : 0x{charges_off:04X}-0x{charges_off+14:04X} {hex_preview(d,charges_off,15)}')
    print(f'  roster  : 0x{roster_off:04X}-0x{tail_off-1:04X}')
    for i in range(ROSTER_COUNT):
        o=roster_off+i*CHARACTER_SIZE; print(f'    roster[{i:02d}] @ 0x{o:04X}: {c_string(d,o,16)!r}')
    print(f'  trailing: 0x{tail_off:04X}-0x{expected_end-1:04X} {hex_preview(d,tail_off,TAIL_SIZE)}')
    if expected_end!=len(d): print(f'WARNING: derived end 0x{expected_end:X}, file 0x{len(d):X}'); return 1
    predicted=REFERENCE_SIZE+count*WORLD_OBJECT_SIZE
    print(f'\nserializer-derived size: 0x{expected_end:X}\nreference-size formula : 0x{predicted:X}')
    return 0 if predicted==expected_end else 1

if __name__=='__main__': raise SystemExit(main())
