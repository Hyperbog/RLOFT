#!/usr/bin/env python3
"""Compare original x86 direct CALL targets with reconstructed C++ direct calls.

Observations are not equivalence verdicts. Callbacks, ABI adapters, indirect
calls and inlined code routinely produce differences without implying defects.
"""
from __future__ import annotations

import argparse
import json
from pathlib import Path
import sys

import index as sources
import original_cfg

SCHEMA = 'rloft-original-vs-cpp-direct-call-evidence-v1'


def compare_witness(witness: dict, src_index: dict) -> dict:
    if src_index.get('schema') != sources.SCHEMA:
        raise ValueError('unknown source index schema')
    key=witness['id']
    module=key.split(':',1)[0]
    matching=[f for f in src_index['functions'] if f['id']==key]
    all_edges=witness['flow']['edges']+witness['flow']['external_targets']
    machine=sorted({f'{module}:{e["target"]:05x}' for e in all_edges
                    if e['kind']=='call' and e.get('target') is not None})
    machine_sites=sorted([{'offset':e['source'],'callee':f'{module}:{e["target"]:05x}'}
                          for e in all_edges if e['kind']=='call' and e.get('target') is not None],
                         key=lambda e:(e['offset'],e['callee']))
    ambiguous=witness.get('source_status')=='ambiguous_source_name' or len(matching)>1
    if len(matching)==1 and not ambiguous:
        source_calls=sorted(set(matching[0]['calls']))
        common=sorted(set(machine)&set(source_calls))
        machine_only=sorted(set(machine)-set(source_calls))
        source_only=sorted(set(source_calls)-set(machine))
        status='SOURCE_AND_ORIGINAL_DIRECT_CALL_OBSERVATIONS'
    else:
        source_calls=[];common=[];machine_only=[];source_only=[]
        status='SOURCE_SYMBOL_NOT_UNIQUELY_INDEXED'
    return {
        'id':key, 'schema':SCHEMA,'status':status,
        'machine_direct_calls':machine,'machine_call_sites':machine_sites,
        'cpp_direct_calls':source_calls,'shared_target_names':common,
        'original_only_target_names':machine_only,
        'cpp_only_target_names':source_only,
        'indirect_original_calls':[e for e in witness['flow']['indirect_edges']
                                   if 'call' in e['kind']],
        'source_match_is_abi_only':True,
        'differences_are_defects':False,
        'notes':'Missing C++ direct calls may be intentionally represented by a runtime callback/ABI boundary.',
    }


def verify_source_index_freshness(src_index: dict, source_root: Path) -> None:
    """Refuse source/assembly comparison with stale C++ SHA provenance."""
    if src_index.get('schema') != sources.SCHEMA:
        raise ValueError('source schema mismatch')
    for meta in src_index.get('sources', []):
        relative = Path(meta['path'])
        if relative.is_absolute() or '..' in relative.parts:
            raise ValueError('unsafe indexed source path')
        current = (source_root / relative).read_bytes()
        if sources.digest(current) != meta['sha256']:
            raise ValueError('reconstructed source changed after index generation: '+str(relative))


def compare_all(machine_report: dict, src_index: dict) -> dict:
    if machine_report.get('schema')!=original_cfg.SCHEMA or \
       machine_report.get('authority')!='authenticated_original_LE_bytes_independent_static_decode':
        raise ValueError('original CFG identity/evidence classification mismatch')
    outputs=[compare_witness(w,src_index) for w in machine_report['witnesses']]
    return {'schema':SCHEMA,'items':outputs,
            'original_witnesses':len(outputs),
            'original_direct_call_sites':sum(len(x['machine_call_sites']) for x in outputs),
            'review_only':True, 'original_dos_parity':False,
            'compiler_byte_match':False}


def main(argv=None) -> int:
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--original-root',type=Path,required=True)
    p.add_argument('--source-root',type=Path,default=Path(__file__).resolve().parent.parent)
    p.add_argument('--manifest',type=Path,default=Path(__file__).with_name('original_witnesses.json'))
    p.add_argument('--index',type=Path)
    p.add_argument('--output',type=Path,required=True)
    args=p.parse_args(argv)
    try:
        doc=json.loads(args.manifest.read_text())
        src=sources.read_index(args.index) if args.index else sources.build(args.source_root)
        verify_source_index_freshness(src,args.source_root)
        machine=original_cfg.build(doc,args.original_root,source_index=src)
        report=compare_all(machine,src)
        args.output.parent.mkdir(parents=True,exist_ok=True)
        args.output.write_text(json.dumps(report,sort_keys=True,indent=2)+'\n')
        print(json.dumps({'original_direct_call_sites':report['original_direct_call_sites'],
                          'review_only':True,'compiler_byte_match':False},sort_keys=True))
        return 0
    except (ValueError,OSError,KeyError) as exc:
        print('CALL_EVIDENCE_REJECTED: '+str(exc),file=sys.stderr)
        return 1


if __name__=='__main__':raise SystemExit(main())
