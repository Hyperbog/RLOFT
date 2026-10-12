#!/usr/bin/env python3
"""Authenticated CODE.2 initializer and termination indirect-target candidates.

The original 0x12738 six-byte termination record has five file-backed bytes;
the last high byte is LE virtual-object zero fill. Missing file backing is
NOT evidence of a missing callback. Static candidates are not observed DOS
runtime indirect call targets.
"""
from __future__ import annotations
import argparse
import hashlib
import json
from pathlib import Path
import sys
import original_cfg

SCHEMA='rloft-code2-original-callback-table-candidates-v1'
DATA_START=0x75F24
PHYSICAL_DATA_SIZE=0x1273D
VIRTUAL_DATA_SIZE=0x2DD10
INITIALIZER_START=0x12720
TERMINATION_START=0x12738
INIT_TARGETS=(0x3968E,0x3A1B3,0x3A2AD,0x3A16E)
TERMINATION_TARGET=0x3A228
WINDOWS={
    (0x395E2,0x3962D):'4372327a1ecb72c421622e01ebc4cab18320202d60e5afc3a8aef6c39268e410',
    (0x3962D,0x3967C):'ddbb043ce125bac62303954e01d5fa2f1d74667c9c588acecf089c18dfc2bbfe',
}


def reconstruct_data(image: bytes) -> tuple[bytes,bytes]:
    code,_=original_cfg.original_object(image,'code2')
    if len(image)-DATA_START!=PHYSICAL_DATA_SIZE:
        raise ValueError('CODE.2 original data file backing differs')
    data=bytearray(VIRTUAL_DATA_SIZE)
    data[:PHYSICAL_DATA_SIZE]=image[DATA_START:]
    return code,bytes(data)


def table_records(data: bytes, count: int, start: int, physical_limit: int) -> list[dict]:
    if len(data)!=VIRTUAL_DATA_SIZE or count<1 or count>16:
        raise ValueError('invalid CODE.2 virtual data object or table count')
    output=[]
    for i in range(count):
        off=start+i*6
        if off+6>len(data):
            raise ValueError('callback table exceeds virtual data')
        raw=data[off:off+6]
        output.append({
            'record_offset':off, 'original_flag':raw[0],
            'priority':raw[1], 'target':int.from_bytes(raw[2:6],'little'),
            'file_backed_bytes':sum(1 for x in range(off,off+6) if x<physical_limit),
            'zero_filled_bytes':sum(1 for x in range(off,off+6) if x>=physical_limit),
        })
    return output


def resolve(image: bytes) -> dict:
    code,data=reconstruct_data(image)
    for (a,z),identity in WINDOWS.items():
        if hashlib.sha256(code[a:z]).hexdigest()!=identity:
            raise ValueError(f'original callback scheduler {a:05X} span differs')
    if code[0x3961F:0x39621]!=bytes.fromhex('ff d0') or \
       code[0x3966F:0x39671]!=bytes.fromhex('ff d0'):
        raise ValueError('original indirect CALL EAX locations changed')
    init=table_records(data,4,INITIALIZER_START,PHYSICAL_DATA_SIZE)
    fini=table_records(data,1,TERMINATION_START,PHYSICAL_DATA_SIZE)
    if tuple(x['target'] for x in init)!=INIT_TARGETS or \
       any(x['original_flag']!=0 or x['priority']!=0x20 for x in init):
        raise ValueError('initialization callback table differs from owner-original')
    if fini[0]['target']!=TERMINATION_TARGET or \
       (fini[0]['original_flag'],fini[0]['priority'],
        fini[0]['file_backed_bytes'],fini[0]['zero_filled_bytes'])!=(0,0x20,5,1):
        raise ValueError('original zero-filled termination record differs')
    # The binary uses an unsigned JA to retain the *later* equal-priority
    # candidate in initializer scans, not the first record by list order.
    execution_order=[x['target'] for x in reversed(init)]
    return {
        'schema':SCHEMA,
        'original_code2_sha256':original_cfg.ORIGINALS['code2']['sha256'],
        'original_file_backed_data_bytes':PHYSICAL_DATA_SIZE,
        'original_virtual_data_bytes':VIRTUAL_DATA_SIZE,
        'init_call':{
            'indirect_call_address':0x3961F,
            'table_range':[INITIALIZER_START,TERMINATION_START],
            'source_table':init,'candidate_targets':list(INIT_TARGETS),
            'static_equal_priority_execution_order':execution_order,
            'runtime_target_proven':False,
        },
        'termination_call':{
            'indirect_call_address':0x3966F,
            'table_range':[TERMINATION_START,TERMINATION_START+6],
            'source_table':fini,
            'candidate_targets':[TERMINATION_TARGET],
            'synthesized_zero_filled_high_byte':True,
            'runtime_target_proven':False,
        },
        'evidence':'authenticated_original_LE_loaded_bytes_static_table_candidates',
        'source_named_target_3a228':'stream_close_then_exit',
        'runtime_DOS_calls_observed':False,
        'compiler_byte_match_proven':False,
    }


def main(argv=None) -> int:
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--code2',type=Path,required=True)
    p.add_argument('--out',type=Path,required=True)
    args=p.parse_args(argv)
    try:
        data=resolve(args.code2.read_bytes())
        args.out.parent.mkdir(parents=True,exist_ok=True)
        args.out.write_text(json.dumps(data,sort_keys=True,indent=2)+'\n',encoding='utf-8')
        print(json.dumps({'status':'AUTHENTICATED_STATIC_CALLBACK_TABLES',
                          'initializer_candidates':len(data['init_call']['candidate_targets']),
                          'termination_candidates':len(data['termination_call']['candidate_targets']),
                          'original_dos_runtime_observed':False},sort_keys=True))
        return 0
    except (OSError,ValueError) as exc:
        print('CODE2_CALLBACK_ORACLE_REJECTED: '+str(exc),file=sys.stderr)
        return 1


if __name__=='__main__':
    raise SystemExit(main())
