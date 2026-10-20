#!/usr/bin/env python3
"""Strict original CODE.2 DOS/4GW LE fixup parser; no retail bytes emitted.

Audits loaded-reference targets, not indirect runtime reachability.
Supports only source 07h / internal 00h, no list, chain or additive,
with 16/32-bit targets: exactly the formats present in this retail CODE.2.
"""
import argparse
import hashlib
import json
import struct
from collections import Counter
from pathlib import Path

IDENTITY = '853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314'
MODULE = 0x25D24
HEADER = 0x27BDC
TARGETS = (0x29248, 0x323BC, 0x324F8)
PALETTE_BUFFERS = (0xC054, 0xC355, 0xF9D0)


def word(binary, offset):
    return struct.unpack_from('<H', binary, offset)[0]


def dword(binary, offset):
    return struct.unpack_from('<I', binary, offset)[0]


def require(ok, why):
    if not ok:
        raise ValueError(why)


def analyze(image):
    require(hashlib.sha256(image).hexdigest() == IDENTITY,
            'Unrecognized original CODE.2 SHA256')
    require(image[MODULE:MODULE+2] == b'MZ' and
            dword(image, MODULE + 0x3c) == HEADER - MODULE and
            image[HEADER:HEADER+4] == b'LE\x00\x00',
            'Invalid bound application LE header')
    pages = dword(image, HEADER+0x14)
    page_size = dword(image, HEADER+0x28)
    obj_count = dword(image, HEADER+0x44)
    obj_table = HEADER+dword(image, HEADER+0x40)
    require(pages == 78 and page_size == 4096 and obj_count == 2,
            'Unexpected LE object/page allocation')
    descriptors = [struct.unpack_from('<6I', image, obj_table+i*24)
                   for i in range(obj_count)]
    code, data = descriptors
    require((code[0], code[1], code[3], code[4]) == (0x3A852,0x10000,1,59),
            'Unexpected CODE.2 object-1 mapping')
    require((data[0], data[1], data[3], data[4]) == (0x2DD10,0x50000,60,19),
            'Unexpected CODE.2 object-2 mapping')
    page_table = HEADER+dword(image, HEADER+0x68)
    records = HEADER+dword(image, HEADER+0x6c)
    end = HEADER+dword(image, HEADER+0x70)
    require((page_table,records) == (HEADER+0x235,HEADER+0x371),
            'Unexpected LE fixup table locations')
    require(records+dword(image,page_table+pages*4) == end <= len(image),
            'Fixup table ending does not match LE header')
    counts = Counter()
    buffer_sites = {f'0x{n:05X}': [] for n in PALETTE_BUFFERS}
    target_sites = {f'0x{n:05X}': [] for n in TARGETS}
    for logical_page in range(pages):
        lo = records+dword(image,page_table+logical_page*4)
        hi = records+dword(image,page_table+(logical_page+1)*4)
        require(records<=lo<=hi<=end,'Corrupted fixup page offsets')
        cursor = lo
        source_object = 1 if logical_page<59 else 2
        source_page = logical_page if source_object==1 else logical_page-59
        while cursor<hi:
            require(cursor+7<=hi,'Truncated LE fixup header')
            kind, flags = image[cursor:cursor+2]
            require(kind==0x07 and flags in (0x00,0x10),
                    'Unsupported actual original LE fixup record form')
            source_offset = struct.unpack_from('<h',image,cursor+2)[0]
            target_object = image[cursor+4]
            require(target_object in (1,2), 'Invalid LE fixup target object')
            width = 4 if flags==0x10 else 2
            require(cursor+5+width<=hi,'Truncated LE target offset')
            destination = int.from_bytes(image[cursor+5:cursor+5+width], 'little')
            require(destination<descriptors[target_object-1][0],
                    'Relocation target exceeds declared LE object')
            source_position = source_page*page_size+source_offset
            require(-4<=source_offset<page_size and source_position>=0,
                    'Relocation source outside page mapping')
            counts[f'{kind:02X}_{flags:02X}_target{target_object}'] += 1
            if target_object==1 and destination in TARGETS:
                target_sites[f'0x{destination:05X}'].append(source_position)
            if target_object==2 and destination in PALETTE_BUFFERS:
                buffer_sites[f'0x{destination:05X}'].append(source_position)
            cursor += 5+width
        require(cursor==hi,'LE fixup page is not fully parsed')
    require(sum(counts.values())==9813,'Original fixup record count changed')
    require(sum(v for k,v in counts.items() if k.endswith('target1'))==794,
            'Original executable-object fixup count changed')
    require({k:len(v) for k,v in buffer_sites.items()} ==
            {'0x0C054':76,'0x0C355':5,'0x0F9D0':8},
            'Original palette buffer relocation inventory changed')
    require(all(not hits for hits in target_sites.values()),
            'An indirect palette helper acquired a relocated target')
    return {
        'evidence':'CODE2_AUTHENTICATED_FULL_LE_FIXUP_RECORDS',
        'original_code2_sha256': IDENTITY,
        'le_header_file_offset':hex(HEADER),
        'fixup_page_table_relative':hex(page_table-HEADER),
        'fixup_record_table_relative':hex(records-HEADER),
        'logical_pages':pages,
        'fixup_records':sum(counts.values()),
        'fixup_form_counts':dict(sorted(counts.items())),
        'targets_to_palette_helpers':{k:len(v) for k,v in target_sites.items()},
        'palette_buffer_fixup_sources':{k:{'count':len(v),
                                      'sample_source_object_relative':
                                      [hex(x) for x in v[:8]]}
                                      for k,v in buffer_sites.items()},
        'other_indirect_execution': 'NOT_EXCLUDED',
        'DOS_port_write_provenance': 'NOT_CAPTURED',
    }


def self_test():
    for bad in (b'',b'LE'+bytes(200)):
        try:
            analyze(bad)
        except ValueError as e:
            require('SHA256' in str(e),'Unexpected rejection cause')
        else:
            raise AssertionError('Unverified LE image accepted')
    print('SELF_TEST_PASS original authentication rejection')


def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--code2',type=Path)
    p.add_argument('--report',type=Path)
    p.add_argument('--self-test',action='store_true')
    a=p.parse_args()
    if a.self_test: self_test()
    if a.code2 is None:
        if not a.self_test:p.error('--code2 is required')
        return
    result=analyze(a.code2.read_bytes())
    if a.report:
        a.report.parent.mkdir(parents=True,exist_ok=True)
        a.report.write_text(json.dumps(result,indent=2)+'\n')
    print('ORIGINAL_LE_PALETTE_FIXUP_PASS records=9813 target_object1=794 '
          'candidate_palette_function_relocations=0 buffer_sources=89 '
          'dynamic_palette_provenance=NOT_VERIFIED')

if __name__=='__main__':main()
