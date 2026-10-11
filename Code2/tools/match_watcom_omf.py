#!/usr/bin/env python3
"""Compare five approved Watcom OMF probes to pinned original CODE.2 hashes.

No original game bytes are stored. Only absolute 32-bit external DGROUP
fixups with zero addends are accepted; no LE linker parity is asserted.
"""
from __future__ import annotations
import argparse
import hashlib
import json
from pathlib import Path
import struct
from inspect_watcom_omf import Cursor, OMFError, parse_omf, records

# Authenticated original CODE.2 object-1 spans, never raw original bytes.
PROBES = {
    'set_f0cc': (12, '4f04d3978215298d3dfb158f4d8b8476c9b1df1f01505cbf21153d0fddf54a3e', {'f0cc': 0xF0CC}),
    'set_f0c8': (12, 'ee71c3d92af7ab0a952c3dd6243542249d199a9ed3320d78a449711c06ba21e0', {'f0c8': 0xF0C8}),
    'get_2b684': (8, 'dcf75714bd9f236c0923f4cae20b5377105cb8b5c24ed371d8cae18c29295acc', {'g2b684': 0x2B684}),
    'get_2b680': (8, '576ae5640d8d4b10a3d95da6ccf9098113db564171c0936d476a8f8fa5bd00fd', {'g2b680': 0x2B680}),
    'call_f0c8': (16, 'dab1ea08314669c9073f77d62e5ef9cbeec0045753bf87746def521896a22858', {'f0c8': 0xF0C8}),
}

def compare_object(data: bytes) -> dict:
    inv = parse_omf(data)
    # Reject any unparsed code-bearing record; never silently omit bytes.
    if inv['unparsed_code_features'] or inv['unknown_record_types']:
        raise OMFError('unsupported OMF code features prevent byte comparisons')
    code_segments = [s for s in inv['segments'] if s['class'] == 'CODE' and s['size']]
    if len(code_segments) != 1 or not code_segments[0]['use32']:
        raise OMFError('requires exactly one 32-bit CODE segment')
    seg = code_segments[0]
    buf = bytearray(seg['size'])
    covered = bytearray(seg['size'])
    for tag, payload, _, _ in records(data):
        if tag not in (0xA0, 0xA1):
            continue
        cur = Cursor(payload)
        sid = cur.index()
        offset = cur.uint(4 if tag == 0xA1 else 2)
        if sid != seg['index']:
            continue
        length = len(payload) - cur.p
        if offset + length > len(buf) or any(covered[offset:offset+length]):
            raise OMFError('invalid or overlapping CODE LEDATA')
        buf[offset:offset+length] = payload[cur.p:]
        covered[offset:offset+length] = bytes([1])*length
    pubs = sorted((p for p in inv['public_symbols'] if p['segment'] == seg['index']),
                  key=lambda p: p['offset'])
    names = [p['name'].lstrip('_') for p in pubs]
    if len(names) != len(set(names)) or any(p['offset'] >= len(buf) for p in pubs):
        raise OMFError('duplicate or out-of-range public function')
    results = {}
    for i, pub in enumerate(pubs):
        name = names[i]
        if name not in PROBES:
            continue
        start = pub['offset']
        end = pubs[i+1]['offset'] if i+1 < len(pubs) else len(buf)
        if end <= start or not all(covered[start:end]):
            raise OMFError('candidate function has missing CODE bytes')
        target_len, expected_sha, reloc_map = PROBES[name]
        candidate = bytearray(buf[start:end])
        fixes = [f for f in inv['fixups'] if f['segment'] == seg['index'] and
                 start <= f['offset'] < end]
        unsupported = []
        applied = []
        for f in fixes:
            sym = (f['target_external'] or '').lstrip('_')
            at = f['offset']-start
            if (f['location_code'] != 9 or f['width'] != 4 or
                f['self_relative'] or f['target_method'] != 2 or
                f['frame_method'] != 1 or f['target_displacement'] != 0 or
                sym not in reloc_map or at+4 > len(candidate) or
                candidate[at:at+4] != bytes(4)):
                unsupported.append({'offset': at, 'target': sym, 'location_code': f['location_code']})
                continue
            candidate[at:at+4] = struct.pack('<I', reloc_map[sym])
            applied.append({'offset': at, 'target': sym, 'resolved': hex(reloc_map[sym])})
        required = set(reloc_map)
        exact = (not unsupported and len(candidate) == target_len and
                 {f['target'] for f in applied} == required and
                 hashlib.sha256(candidate).hexdigest() == expected_sha)
        results[name] = {'match': exact, 'candidate_size': len(candidate),
                         'original_size': target_len, 'target_sha256': expected_sha,
                         'resolved_candidate_sha256': hashlib.sha256(candidate).hexdigest(),
                         'applied_fixups': applied, 'unresolved_fixups': unsupported,
                         'scope': 'relocation-resolved CODE bytes only; historical compiler provenance not proven'}
    return {'schema': 'rloft-watcom-omf-hashmatch-v1',
            'object_sha256': inv['object_sha256'], 'probes': results,
            'matches': sum(x['match'] for x in results.values()),
            'expected_probes': len(PROBES), 'compiler_identified': False,
            'original_le_relocations_verified': False, 'dos_runtime_parity': 'not_tested'}

def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('compiled_omf_obj', type=Path)
    p.add_argument('--output', type=Path)
    a = p.parse_args()
    result = json.dumps(compare_object(a.compiled_omf_obj.read_bytes()), indent=2, sort_keys=True)+'\n'
    if a.output:
        a.output.write_text(result, encoding='utf-8')
    else:
        print(result, end='')

if __name__ == '__main__':
    main()
