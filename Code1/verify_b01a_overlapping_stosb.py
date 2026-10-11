#!/usr/bin/env python3
"""Authentic CODE.1 B01A..B054 overlapping x86 control-flow witness.

Branches into 0xB04C where byte AA is STOSB, despite the *linear* stream
decoding A8 AA as TEST AL,AA. Original bytes remain private/uncommitted.
"""
from __future__ import annotations
import argparse
import hashlib
from pathlib import Path
import sys
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'ResearchInfra'))
import original_cfg

SPAN='581ed03cd65d437d3601f3fd2511c1c82d444aba9d8de5dee5a365987a39ec09'
START=0xB01A
END=0xB055


def verify(original: bytes) -> dict:
    code,_=original_cfg.original_object(original,'code1')  # Full-file SHA and LE descriptors.
    fragment=code[START:END]
    if hashlib.sha256(fragment).hexdigest()!=SPAN:
        raise ValueError('original CODE.1 B01A..B054 witness hash differs')
    windows={
        0xB037:bytes.fromhex('74 13'),   # JE B04C
        0xB043:bytes.fromhex('74 07'),   # JE B04C
        0xB04B:bytes.fromhex('A8 AA'),   # Linear TEST AL,AA
        0xB04C:bytes.fromhex('AA'),      # Alternate entry STOSB AL at ES:EDI
        0xB04D:bytes.fromhex('5F 07 89 F8') # restore and return original dst
    }
    for offset,seq in windows.items():
        if code[offset:offset+len(seq)]!=seq:
            raise ValueError(f'original CODE.1 instruction witness {offset:05X} differs')
    ins=original_cfg.decode(code[0xB04C:END],0xB04C)
    if not ins or ins[0]['address']!=0xB04C or ins[0]['mnemonic']!='stos' or ins[0]['size']!=1:
        raise ValueError('alternate source control-flow target is not STOSB')
    return {'source_sha256':original_cfg.ORIGINALS['code1']['sha256'],
            'original_span_sha256':SPAN,'function_start':START,'function_end_exclusive':END,
            'direct_conditional_branches_to_STOSB':2,
            'encoded_overlap_at':0xB04C,
            'proven_effect':'zero byte written by STOSB when AL zero',
            'source_behavioral_model':'requires write8(dst_end,0) on low8(AX)==0'}


def main(argv=None) -> int:
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('code1',type=Path)
    a=p.parse_args(argv)
    try:
        result=verify(a.code1.read_bytes())
        import json
        print(json.dumps({'status':'ORIGINAL_OVERLAPPING_STOSB_WITNESS_PASS',**result},
                         sort_keys=True))
        return 0
    except (OSError,ValueError) as exc:
        print('B01A_ORIGINAL_REJECTED: '+str(exc),file=sys.stderr)
        return 1


if __name__=='__main__':
    raise SystemExit(main())
