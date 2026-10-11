#!/usr/bin/env python3
"""Bridge the existing CODE.1/CODE.2 source index into AI workflow v1 packets.

Never promotes reconstructed C++ source to authenticated DOS execution evidence.
Preserves source-hash freshness checking implemented by ResearchInfra.index.
"""
from __future__ import annotations
import argparse
import json
from pathlib import Path
import sys

THIS=Path(__file__).resolve()
ROOT=THIS.parents[1]
sys.path.insert(0,str(THIS.parent))
sys.path.insert(0,str(ROOT/'SharedDecomp'))
import index as source_index
from workflow import VERSION, EvidenceError, atomic_json


def export_packet(index_doc:dict, root:Path, key:str, max_chars:int=12000)->dict:
    source=source_index.packet(index_doc,root,key,max_chars=max_chars)
    f=source['function']
    target=f['id']
    return {
        'schema_version':VERSION,
        'target':target,
        'platform':'DOS/4GW LE x86 reconstructed C++',
        'source_authority':'STATIC_SOURCE_ONLY',
        'original_binary_match':False,
        'dos_runtime_parity':False,
        'native_windows_parity':False,
        'compiler_identity':'UNPROVED; Watcom toolchain hypotheses are separate',
        'source':{'file':f['source'],'source_sha256':source['source_sha256'],
                  'line_start':f['line_start'],'line_end':f['line_end'],
                  'excerpt':source['source_excerpt'],'truncated':source['truncated']},
        'static_calls':{'callers':source['callers'],'callees':source['callees'],
                         'unresolved':source['unresolved_callees'],
                         'ambiguous':source['ambiguous_callees']},
        'evidence_classes':{'reconstructed_cpp':'STRUCTURAL_CANDIDATE',
                            'source_structure_only':'STATIC_SOURCE_ONLY'},
        'next_verification':["authenticate original CODE file independently",
                             "import original DOS x86 basic blocks and fixups",
                             "capture original DOS execution", "compare to native Windows replay"],
    }


def main(argv=None)->int:
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--root',type=Path,default=ROOT)
    p.add_argument('--index',type=Path)
    p.add_argument('--key',required=True)
    p.add_argument('--out',type=Path,required=True)
    p.add_argument('--max-chars',type=int,default=12000)
    args=p.parse_args(argv)
    try:
        if args.max_chars<=0:raise ValueError('max-chars must be positive')
        doc=source_index.read_index(args.index) if args.index else source_index.build(args.root)
        payload=export_packet(doc,args.root,args.key,args.max_chars)
        atomic_json(args.out,payload)
        print(json.dumps({'status':'PASS','target':payload['target'],
                          'original_binary_match':False,'native_windows_parity':False},sort_keys=True))
        return 0
    except (ValueError,OSError,KeyError) as e:
        print(f'SOURCE CONTEXT ERROR: {e}',file=sys.stderr)
        return 2

if __name__=='__main__':raise SystemExit(main())
