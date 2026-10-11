#!/usr/bin/env python3
"""Bounded original LE code-object survey, seeded by canonical C++ labels.

This is NOT function-boundary verification: a reconstructed source label is only
a candidate start, and a bounded sequential x86 decode can contain original
tables/data. Counts are candidate survey coverage, not executed instructions.
"""
from __future__ import annotations

import argparse
import json
import sys
from collections import defaultdict
from pathlib import Path

import compare_call_evidence
import index as source_index
import original_cfg

SCHEMA = 'rloft-original-source-seeded-cfg-survey-v1'
MAX_LABELS = 2500
MAX_SPAN = 1024
MAX_TOTAL = 500000


def ranges_from_index(doc: dict, objects: dict[str, bytes], *, span_limit: int,
                      total_limit: int, max_labels: int) -> tuple[list[dict], list[dict]]:
    if doc.get('schema') != source_index.SCHEMA:
        raise ValueError('unrecognized canonical source-index schema')
    if span_limit < 1 or span_limit > MAX_SPAN or total_limit < 1 or total_limit > MAX_TOTAL:
        raise ValueError('span or total byte budget exceeds published bounds')
    if max_labels < 1 or max_labels > MAX_LABELS:
        raise ValueError('source-label count outside bounded range')
    labels: dict[str, dict[int, list[str]]] = defaultdict(lambda: defaultdict(list))
    rejected = []
    for row in doc['functions']:
        module = row['module']
        off = row['offset']
        if module not in objects or type(off) is not int:
            raise ValueError('source label refers to unrecognized object')
        if off < 0 or off >= len(objects[module]):
            rejected.append({'module': module, 'offset': off,
                             'reason': 'source_label_outside_authentic_code_object'})
        else:
            labels[module][off].append(row['id'])
    if sum(len(v) for v in labels.values()) > max_labels:
        raise ValueError('source label budget exceeded; no silent truncation')
    candidates = []
    spent = 0
    for module in sorted(labels):
        positions = sorted(labels[module])
        size = len(objects[module])
        for index, start in enumerate(positions):
            next_start = positions[index + 1] if index + 1 < len(positions) else size
            end = min(start + span_limit, next_start, size)
            # Labels sharing one original address are retained as an ambiguity,
            # not represented as multiple independently decoded functions.
            if end <= start:
                raise ValueError('unsound source-label partition')
            spent += end - start
            if spent > total_limit:
                raise ValueError('aggregate original survey byte budget exceeded')
            candidates.append({'module': module, 'start': start,
                               'end_exclusive': end,
                               'source_ids': sorted(labels[module][start]),
                               'next_source_label': next_start if next_start < size else None,
                               'truncated_by_cap': start + span_limit < next_start,
                               'source_symbol_is_proven_function_boundary': False})
    return candidates, rejected


def inspect(original_root: Path, doc: dict, *, span_limit=160, max_labels=MAX_LABELS,
            total_limit=MAX_TOTAL, executable='objdump') -> dict:
    version = original_cfg.decoder_version(executable)
    objs = {}
    origins = {}
    for module, info in original_cfg.ORIGINALS.items():
        data = (original_root / info['file']).read_bytes()
        code, identity = original_cfg.original_object(data, module)
        objs[module] = code
        origins[module] = {'file_sha256': identity['original_sha256'],
                           'object_size': len(code), 'entry': identity['entry']}
    survey, rejected = ranges_from_index(doc, objs, span_limit=span_limit,
                                          max_labels=max_labels, total_limit=total_limit)
    rows = []
    totals = defaultdict(int)
    for spec in survey:
        start, end, module = spec['start'], spec['end_exclusive'], spec['module']
        witness = objs[module][start:end]
        row = {**spec, 'original_span_sha256': original_cfg.sha(witness)}
        try:
            linear = original_cfg.decode(witness, start, executable)
            flow = original_cfg.graph(linear, start, end)
            row.update({'status': 'candidate_decoded_original_bytes',
                        'linear_instructions': flow['linear_decoded_instruction_count'],
                        'statically_traversed_candidates': flow['reachable_instruction_count'],
                        'basic_blocks': len(flow['basic_blocks']),
                        'local_edges': len(flow['edges']),
                        'external_edges': len(flow['external_targets']),
                        'indirect_edges': len(flow['indirect_edges']),
                        'conflicts': flow['conflicts'],
                        'indirect_sites': flow['indirect_edges'],
                        'external_sites': flow['external_targets'][:40],
                        'interrupt_sites': flow['service_boundaries']})
            totals['decoded_windows'] += 1
            totals['linear_instructions'] += row['linear_instructions']
            totals['traversed_candidate_instructions'] += row['statically_traversed_candidates']
            totals['candidate_blocks'] += row['basic_blocks']
            totals['local_edges'] += row['local_edges']
            totals['indirect_edges'] += row['indirect_edges']
            totals['conflict_sites'] += len(row['conflicts'])
        except ValueError as error:
            row.update({'status': 'candidate_decoder_rejected',
                        'error_kind': 'decode_failure_without_promotion'})
            totals['failed_windows'] += 1
        totals['selected_original_bytes'] += end - start
        rows.append(row)
    totals['distinct_source_seed_addresses'] = len(rows)
    totals['source_labels_outside_original_code'] = len(rejected)
    return {
        'schema': SCHEMA,
        'authority': 'authenticated_original_bytes_and_source_seeds_cannot_prove_function_boundaries',
        'decoder': version, 'originals': origins, 'source_hashes': doc['sources'],
        'limits': {'span_limit': span_limit, 'total_limit': total_limit,
                   'max_source_labels': max_labels},
        'summary': dict(sorted(totals.items())),
        'out_of_object_source_labels': rejected,
        'candidates': rows,
        'original_dos_execution_proven': False,
        'watcom_matching_proven': False,
        'native_gameplay_proven': False,
    }


def main(argv=None) -> int:
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('--original-root', type=Path, required=True)
    p.add_argument('--source-root', type=Path, default=Path(__file__).resolve().parent.parent)
    p.add_argument('--index', type=Path)
    p.add_argument('--out', type=Path, required=True)
    p.add_argument('--span-limit', type=int, default=160)
    p.add_argument('--total-limit', type=int, default=MAX_TOTAL)
    p.add_argument('--max-labels', type=int, default=MAX_LABELS)
    args = p.parse_args(argv)
    try:
        doc = source_index.read_index(args.index) if args.index else source_index.build(args.source_root)
        compare_call_evidence.verify_source_index_freshness(doc, args.source_root)
        report = inspect(args.original_root, doc, span_limit=args.span_limit,
                         max_labels=args.max_labels, total_limit=args.total_limit)
        args.out.parent.mkdir(parents=True, exist_ok=True)
        args.out.write_text(json.dumps(report, sort_keys=True, indent=2) + '\n')
        print(json.dumps({'schema': report['schema'], 'summary': report['summary'],
                          'original_dos_execution_proven': False}, sort_keys=True))
        return 0
    except (OSError, KeyError, ValueError) as error:
        print(f'BULK_ORIGINAL_SURVEY_REJECTED: {error}', file=sys.stderr)
        return 1


if __name__ == '__main__':
    raise SystemExit(main())
