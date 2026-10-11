#!/usr/bin/env python3
"""Cross-project original-binary + source-index work packets, never gameplay proof.

The command rebuilds the machine witness from SHA-authenticated original media;
it NEVER trusts a preexisting JSON CFG as proof of an original-file comparison.
"""
from __future__ import annotations

import argparse
import json
import sys
from pathlib import Path

import index as source_index
import original_cfg
import compare_call_evidence

SCHEMA = 'rloft-original-crossproject-packet-v1'
ROOT = Path(__file__).resolve().parent.parent


def packet(machine: dict, sources: dict, root: Path, registry: dict, key: str,
           max_chars: int=12000, authenticated_original: bool=False) -> dict:
    if machine.get('schema') != original_cfg.SCHEMA or sources.get('schema') != source_index.SCHEMA:
        raise ValueError('incompatible original machine/source graph schemas')
    if registry.get('schema') != 'rloft-project-registry-v1':
        raise ValueError('unrecognized owning project registry')
    found = [w for w in machine['witnesses'] if w['id'] == key]
    if len(found) != 1:
        raise ValueError('original witness not uniquely indexed for '+key)
    witness = found[0]
    # The source packet verifier checks that the actual C++ SHA agrees with the
    # index file; a historical/stale source index cannot silently be reused.
    sources_for_key = witness['source_index_matches']
    selected = None
    if len(sources_for_key) == 1:
        selected = source_index.packet(sources,root,sources_for_key[0],max_chars=max_chars)
    elif len(sources_for_key)>1:
        # Explicit overload ambiguity, rather than an invented source match.
        selected={'candidate_ids':sources_for_key,'source_status':'ambiguous'}
    owner=[]
    for project in registry.get('primary',[]):
        branch=project['branch']
        if branch=='DnD-Classics':
            role='original_read_only_preservation_no_changes'
        elif branch=='code1-decompilation':
            role='source_owner_consider_validation'
        elif branch=='disassembler-foundation':
            role='independent_decoder_comparison_candidate'
        else:
            role='manual_triage_only_no_proven_consumer_dependency'
        owner.append({'branch':branch, 'role':role, 'confirmed_direct_dependency':False})
    return {
        'schema':SCHEMA, 'target':key,
        'machine_authority':'authenticated_original_LE_bytes_independent_static_decode',
        'original_identity_verified_at_packet_generation':authenticated_original,
        'original_span_sha256':witness['original_span_sha256'],
        'original_file_sha256':witness['original_file_sha256'],
        'decoder':machine['decoder'],
        'source_authority':'STATIC_RECONSTRUCTED_CPP_ONLY',
        'source_index_matches':sources_for_key,
        'source_context':selected,
        'static_machine_cfg':witness['flow'],
        'original_vs_cpp_call_observations':compare_call_evidence.compare_witness(witness,sources),
        'cross_project_review_lanes':owner,
        'verification_gaps':[
            'original CFG is bounded linear x86-32 candidate, not traced DOS reachability',
            'static source-name alignment does not show matching machine-code generation',
            'indirect branches require independent target analysis',
            'original DOS and native captures require separate verified provenance',
            'project review lanes are NOT confirmed code or asset dependencies'],
        'compiler_output_matching':False,
        'original_dos_execution_parity':False,
        'native_engine_gameplay_parity':False,
    }


def main(argv=None) -> int:
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--root',type=Path,default=ROOT)
    parser.add_argument('--original-root',type=Path,required=True)
    parser.add_argument('--key',required=True,help='e.g. code2:35418')
    parser.add_argument('--manifest',type=Path,default=Path(__file__).with_name('original_witnesses.json'))
    parser.add_argument('--registry',type=Path,default=Path(__file__).with_name('project_registry.json'))
    parser.add_argument('--source-index',type=Path)
    parser.add_argument('--output',type=Path,required=True)
    parser.add_argument('--max-chars',type=int,default=12000)
    args=parser.parse_args(argv)
    try:
        if args.max_chars<1 or args.max_chars>100000:
            raise ValueError('max-chars outside 1..100000')
        m=json.loads(args.manifest.read_text())
        registry=json.loads(args.registry.read_text())
        index=(source_index.read_index(args.source_index) if args.source_index else
               source_index.build(args.root))
        compare_call_evidence.verify_source_index_freshness(index,args.root)
        cfg=original_cfg.build(m,args.original_root,source_index=index)
        p=packet(cfg,index,args.root,registry,args.key,args.max_chars,authenticated_original=True)
        args.output.parent.mkdir(parents=True,exist_ok=True)
        args.output.write_text(json.dumps(p,sort_keys=True,indent=2)+'\n',encoding='utf-8')
        print(json.dumps({'result':'ORIGINAL_VERIFIED_STATIC_WORK_PACKET',
                          'target':args.key,'original_dos_execution_parity':False,
                          'machine_cfg_blocks':len(p['static_machine_cfg']['basic_blocks']),
                          'source_matches':len(p['source_index_matches'])},sort_keys=True))
        return 0
    except (ValueError,OSError,KeyError) as exc:
        print('ORIGINAL_WORK_PACKET_REJECTED: '+str(exc),file=sys.stderr)
        return 1

if __name__=='__main__':raise SystemExit(main())
