#!/usr/bin/env python3
"""Independent original-byte versus DOSDis candidate JSON reconciliation.

Reads DOSDis's actual 'dosdis-analysis-report' schema v1; refuses a wrong game,
wrong module hash, unsupported format or out-of-object offset. Both decoders
remain static candidates; a size/branch mismatch is a review item, not a fact
about DOS gameplay or a proven DOSDis defect.
"""
from __future__ import annotations

import argparse
import json
import sys
from collections import Counter
from pathlib import Path
import original_cfg

SCHEMA='rloft-dosdis-independent-compare-v1'
MAX_REPORT=128 * 1024 * 1024


def addr(value: dict | None, size: int, *, nullable=False) -> int | None:
    if value is None:
        if nullable:
            return None
        raise ValueError('missing DOSDis flow address')
    if not isinstance(value,dict) or type(value.get('Object')) is not int or type(value.get('Offset')) is not int:
        raise ValueError('unrecognized DOSDis FlowAddress layout')
    if value['Object'] != 1 or not (0<=value['Offset']<size):
        raise ValueError('DOSDis address is not in authenticated executable object 1')
    return value['Offset']


def parse_report(doc: dict, module: str, size: int) -> tuple[dict[int,dict], dict[int,set[tuple[str,int]]]]:
    if doc.get('format')!='dosdis-analysis-report' or doc.get('schemaVersion')!=1:
        raise ValueError('DOSDis analysis report schema/version unsupported')
    if doc.get('analysisStatus')!='candidate-unverified':
        raise ValueError('DOSDis claims an unexpected execution/verification status')
    orig=original_cfg.ORIGINALS[module]
    source=doc.get('source')
    if not isinstance(source,dict) or source.get('sha256','').lower()!=orig['sha256']:
        raise ValueError('DOSDis source SHA256 does not match authenticated original')
    if source.get('sourceBinaryIncluded') is not False:
        raise ValueError('DOSDis export unexpectedly embeds source binary')
    if not isinstance(doc.get('instructions'),list) or not isinstance(doc.get('edges'),list):
        raise ValueError('DOSDis instructions/edges collections missing')
    if len(doc['instructions'])>1_000_000 or len(doc['edges'])>2_000_000:
        raise ValueError('DOSDis report unreasonably large')
    inst={}
    for i in doc['instructions']:
        off=addr(i.get('Address'),size)
        length=i.get('Length')
        if type(length) is not int or not 1<=length<=15 or off+length>size:
            raise ValueError('DOSDis instruction exceeds authentic code-object extent')
        if off in inst:
            raise ValueError('duplicate DOSDis instruction address')
        inst[off]=i
    edges={}
    for e in doc['edges']:
        origin=addr(e.get('Source'),size)
        kind=e.get('Kind')
        if not isinstance(kind,str) or len(kind)>100:
            raise ValueError('DOSDis edge kind invalid')
        dest=e.get('Destination')
        if dest is not None:
            target=addr(dest,size)
            if kind in ('call','branch-taken','jump'):
                edges.setdefault(origin,set()).add((kind,target))
    return inst,edges


def compare(original: bytes, module: str, manifest: dict, doc: dict, *, executable='objdump') -> dict:
    if manifest.get('schema')!='rloft-original-witness-manifest-v1':
        raise ValueError('original witness manifest not recognized')
    if len(original)!=original_cfg.ORIGINALS[module]['size'] or original_cfg.sha(original)!=original_cfg.ORIGINALS[module]['sha256']:
        raise ValueError('independent original source is not authenticated')
    code,meta=original_cfg.original_object(original,module)
    observed,export_edges=parse_report(doc,module,len(code))
    found=set()
    matched=[];different=[];not_observed=[];call_equal=[];call_different=[]
    for w in manifest['witnesses']:
        if w['module']!=module:
            continue
        start,end=int(w['start'],16),int(w['end'],16)
        if not (0<=start<end<=len(code)):
            raise ValueError('original manifest witness out of authentic object')
        if original_cfg.sha(code[start:end])!=w['sha256']:
            raise ValueError('original manifest witness hash mismatches original bytes')
        decoded=original_cfg.decode(code[start:end],start,executable)
        for i in decoded:
            off=i['address'];found.add(off)
            candidate=observed.get(off)
            if candidate is None:
                not_observed.append(off)
                continue
            length=candidate['Length']
            if length!=i['size']:
                different.append({'address':off,'dosdis_length':length,'independent_length':i['size'],
                                  'reason':'candidate_instruction_length_disagreement'})
                continue
            # DOSDis opts out of including raw instruction bytes by default.
            # When included, they *must* agree with authenticated original.
            literal=candidate.get('originalBytes')
            if literal is not None:
                if not isinstance(literal,str):
                    raise ValueError('DOSDis originalBytes must be a hex string')
                try:
                    test_bytes=bytes.fromhex(literal)
                except ValueError as err:
                    raise ValueError('DOSDis originalBytes are not valid hex') from err
                if test_bytes!=i['bytes']:
                    different.append({'address':off,'reason':'DOSDis_included_original_bytes_disagree'})
                    continue
            matched.append(off)
            kind,target=original_cfg.classify(i)
            if kind in ('call','conditional','jump') and target is not None and target<len(code):
                expected_kind={'call':'call','conditional':'branch-taken','jump':'jump'}[kind]
                actual=export_edges.get(off,set())
                if (expected_kind,target) in actual:
                    call_equal.append(off)
                elif actual:
                    call_different.append({'address':off,'expected_kind':expected_kind,
                                           'expected_target':target,
                                           'observed':[{'kind':k,'target':t} for k,t in sorted(actual)]})
    summary={
        'witness_instruction_starts':len(found),
        'matching_candidate_starts_and_lengths':len(matched),
        'disagreeing_candidate_starts_or_bytes':len(different),
        'not_observed_by_DOSDis':len(not_observed),
        'matching_direct_branch_sites_when_both_present':len(call_equal),
        'disagreeing_direct_branch_sites_when_both_present':len(call_different),
        'dosdis_total_instruction_candidates':len(observed),
        'dosdis_total_direct_edge_sources':len(export_edges),
    }
    return {
        'schema':SCHEMA,'module':module,
        'original_file_sha256':meta['original_sha256'],
        'dosdis_report_status':'candidate-unverified',
        'independent_decoder':original_cfg.decoder_version(executable),
        'summary':summary,
        'review_disagreements':different,
        'review_edge_disagreements':call_different,
        'unobserved_original_witness_starts_sample':not_observed[:64],
        'unobserved_count_is_not_a_decoder_failure':True,
        'no_original_execution_or_compiler_equivalence_claimed':True,
    }


def main(argv=None) -> int:
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--dosdis',type=Path,required=True)
    p.add_argument('--module',required=True,choices=sorted(original_cfg.ORIGINALS))
    p.add_argument('--original-root',type=Path,required=True)
    p.add_argument('--manifest',type=Path,default=Path(__file__).with_name('original_witnesses.json'))
    p.add_argument('--out',type=Path,required=True)
    args=p.parse_args(argv)
    try:
        if args.dosdis.stat().st_size>MAX_REPORT:
            raise ValueError('DOSDis report exceeds 128 MiB')
        doc=json.loads(args.dosdis.read_text(encoding='utf-8'))
        manifest=json.loads(args.manifest.read_text(encoding='utf-8'))
        orig=(args.original_root/original_cfg.ORIGINALS[args.module]['file']).read_bytes()
        result=compare(orig,args.module,manifest,doc)
        args.out.parent.mkdir(parents=True,exist_ok=True)
        args.out.write_text(json.dumps(result,indent=2,sort_keys=True)+'\n',encoding='utf-8')
        print(json.dumps({'status':'CANDIDATE_REVIEW_ONLY','summary':result['summary'],
                          'original_dos_parity':False},sort_keys=True))
        return 0
    except (OSError,KeyError,ValueError,TypeError) as error:
        print(f'DOSDIS_CROSSCHECK_REJECTED: {error}',file=sys.stderr)
        return 1


if __name__=='__main__':
    raise SystemExit(main())
