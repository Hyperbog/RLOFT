#!/usr/bin/env python3
"""SHA-pinned original CODE.1/CODE.2 LE witness decoder and bounded x86 CFG.

This is static, candidate-only disassembly. Original bytes and executable media
are never put into reports. Independent DOS runtime reachability is not implied.
GNU binutils objdump is intentionally an external independent decoder.
"""
from __future__ import annotations

import argparse
import hashlib
import json
import re
import struct
import subprocess
import sys
import tempfile
from collections import deque
from pathlib import Path

SCHEMA = 'rloft-original-le-cfg-v1'
HEADER = 0x27BDC
MODULE = 0x25D24
ORIGINALS = {
    'code1': {'file': 'CODE.1', 'size': 277787,
              'sha256': 'fd9b709ceee190e6a70870f9012ebea3bc437cae6bc74e0150d6f922fa750fed',
              'code_size': 0x11302, 'data_size': 0x9E90, 'entry': 0xCC1C},
    'code2': {'file': 'CODE.2', 'size': 558689,
              'sha256': '853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314',
              'code_size': 0x3A852, 'data_size': 0x2DD10, 'entry': 0x353A0},
}
INSN = re.compile(r'^\s*([0-9a-f]+):\s*((?:[0-9a-f]{2}\s+)+)\s*(\S.*?)\s*$', re.I)
DEST = re.compile(r'^\s*(?:0x)?([0-9a-f]{3,8})\s*(?:<[^>]*>)?\s*$', re.I)
JCC = re.compile(r'^(?:j(?!mp)[a-z0-9]+|loop(?:e|ne|nz|z)?|jecxz|jcxz)$')
RET = {'ret', 'retf', 'retfq', 'lret', 'iret', 'iretd', 'iretq', 'sysret', 'hlt', 'ud2'}


def sha(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()


def get32(buf: bytes, off: int) -> int:
    if off < 0 or off + 4 > len(buf):
        raise ValueError('LE header read out of range')
    return struct.unpack_from('<I', buf, off)[0]


def original_object(file: bytes, module: str) -> tuple[bytes, dict]:
    expected = ORIGINALS[module]
    if len(file) != expected['size'] or sha(file) != expected['sha256']:
        raise ValueError('not authenticated original ' + expected['file'])
    if file[MODULE:MODULE + 2] != b'MZ' or get32(file, MODULE + 0x3C) != HEADER - MODULE:
        raise ValueError('nested MZ locator invalid')
    if file[HEADER:HEADER + 2] != b'LE' or file[HEADER + 2:HEADER + 4] != b'\x00\x00':
        raise ValueError('unsupported LE signature/byte order')
    def h(n: int) -> int:
        return get32(file, HEADER + n)
    pages = h(0x14)
    if h(0x44) != 2 or h(0x28) != 4096 or h(0x18) != 1 or h(0x1C) != expected['entry']:
        raise ValueError('unrecognized original LE layout')
    table, mapping, data_start = h(0x40), h(0x48), MODULE + h(0x80)
    if HEADER + table + 48 > len(file) or HEADER + mapping + pages * 4 > len(file):
        raise ValueError('LE header tables truncated')
    objects = [struct.unpack_from('<6I', file, HEADER + table + k * 24) for k in range(2)]
    a, b = objects
    if (a[0], b[0], a[1], b[1], a[3], a[4], b[3], b[4]) != (
        expected['code_size'], expected['data_size'], 0x10000,
        0x30000 if module == 'code1' else 0x50000, 1, (expected['code_size']+4095)//4096,
        (expected['code_size']+4095)//4096+1, 6 if module == 'code1' else 19):
        raise ValueError('unexpected LE code/data object descriptors')
    if a[3] + a[4] - 1 > pages or data_start + pages * 4096 - (4096 - h(0x2C)) > len(file):
        raise ValueError('LE page allocation outside file')
    code = bytearray(a[0])
    physical_offsets = []
    for i in range(a[4]):
        page = a[3] + i - 1
        record = file[HEADER + mapping + 4 * page:HEADER + mapping + 4 * (page+1)]
        number = int.from_bytes(record[:3], 'big')
        if len(record) != 4 or record[3] != 0 or number < 1 or number > pages:
            raise ValueError('unsupported LE physical page map entry')
        start = data_start + (number-1)*4096
        remaining = a[0] - i*4096
        size = min(remaining, h(0x2C) if number == pages else 4096)
        if size <= 0 or start + size > len(file):
            raise ValueError('LE code page exceeds physical file')
        code[i*4096:i*4096+size] = file[start:start+size]
        physical_offsets.append({'logical_page': page+1, 'physical_page': number, 'file_offset': start})
    return bytes(code), {'original_sha256': expected['sha256'],
                         'code_object_size': len(code), 'entry': h(0x1C),
                         'mapped_code_pages': len(physical_offsets),
                         'physical_page_mapping': physical_offsets}


def decoder_version(executable='objdump') -> str:
    try:
        r = subprocess.run([executable, '--version'], capture_output=True, text=True,
                           check=True, timeout=10)
    except (OSError, subprocess.SubprocessError) as exc:
        raise ValueError('GNU objdump unavailable: ' + str(exc)) from exc
    if not r.stdout.startswith('GNU objdump (GNU Binutils'):
        raise ValueError('independent decoder is not identified GNU binutils objdump')
    return r.stdout.splitlines()[0]


def decode(window: bytes, logical_start: int, executable='objdump') -> list[dict]:
    if not 0 < len(window) <= 16384:
        raise ValueError('bounded witness length must be 1..16384 bytes')
    with tempfile.TemporaryDirectory(prefix='rloft-cfg-') as tmp:
        p = Path(tmp) / 'code.bin'
        p.write_bytes(window)
        args = [executable, '-D', '-b', 'binary', '-m', 'i386', '-M', 'intel', '-w',
                '--disassemble-zeroes', f'--adjust-vma=0x{logical_start:x}', str(p)]
        try:
            r = subprocess.run(args, capture_output=True, text=True, check=True, timeout=15)
        except (OSError, subprocess.SubprocessError) as exc:
            raise ValueError('x86 decoder failed: ' + str(exc)) from exc
    result = []
    for line in r.stdout.splitlines():
        m = INSN.match(line)
        if not m:
            continue
        address = int(m.group(1),16)
        raw = bytes.fromhex(m.group(2))
        tail = m.group(3).strip()
        operation = tail.split(None, 1)
        mnemonic = operation[0].lower()
        operand = operation[1].strip() if len(operation) > 1 else ''
        result.append({'address': address, 'size':len(raw),
                       'mnemonic': mnemonic, 'operand': operand,
                       'bytes': raw})
    # Output must be a faithful partition of the exact original witness bytes,
    # not merely a plausible textual disassembly.
    at = logical_start
    for ins in result:
        if ins['address'] != at:
            raise ValueError(f'objdump emitted gap or overlap at {at:#x}')
        start = at-logical_start
        if ins['bytes'] != window[start:start+ins['size']]:
            raise ValueError(f'decoder byte stream differs at {at:#x}')
        at += ins['size']
    if at != logical_start+len(window):
        raise ValueError('decoder omitted original bytes')
    return result


def classify(ins: dict) -> tuple[str, int | None]:
    mn, op = ins['mnemonic'], ins['operand']
    if mn in RET or mn.startswith('ret '):
        return 'terminate', None
    if mn in ('int', 'int3', 'into', 'syscall', 'sysenter'):
        return 'interrupt', None
    if mn in ('jmp', 'ljmp') or mn.startswith('jmp '):
        match = DEST.fullmatch(op)
        return ('jump', int(match.group(1), 16)) if match else ('indirect_jump',None)
    if mn in ('call', 'lcall'):
        match = DEST.fullmatch(op)
        return ('call', int(match.group(1),16)) if match else ('indirect_call',None)
    if JCC.fullmatch(mn):
        match = DEST.fullmatch(op)
        return ('conditional', int(match.group(1),16)) if match else ('indirect_conditional',None)
    if mn in ('(bad)', '.byte', '.word', '.long'):
        return 'undecodable',None
    return 'ordinary', None


def graph(insns: list[dict], start: int, end: int) -> dict:
    instructions={x['address']:x for x in insns}
    if start not in instructions:
        raise ValueError('entry not an instruction boundary')
    pending=deque([start]); visited=set(); edges=[]; external=[]; unresolved=[]; services=[]
    conflicts=[]
    while pending:
        address=pending.popleft()
        if address in visited:
            continue
        if address < start or address >= end:
            external.append({'source':None,'target':address,'kind':'entry_outside_window'});continue
        ins=instructions.get(address)
        if not ins:
            conflicts.append({'address':address,'reason':'target_not_linear_instruction_boundary'});continue
        visited.add(address)
        kind, target=classify(ins)
        next_at=address+ins['size']
        if kind=='undecodable':
            conflicts.append({'address':address,'reason':'undecodable_reachable_instruction'})
            continue
        if kind=='interrupt':
            services.append({'address':address,'mnemonic':ins['mnemonic'],
                             'operand':ins['operand']})
        if kind in ('indirect_jump','indirect_call','indirect_conditional'):
            unresolved.append({'address':address,'kind':kind,'operand':ins['operand']})
        if kind in ('jump','call','conditional') and target is not None:
            edge={'source':address,'target':target,'kind':kind}
            if start <= target < end:
                edges.append(edge);pending.append(target)
            else:
                external.append(edge)
        if kind not in ('terminate','jump','indirect_jump'):
            if next_at < end:
                edges.append({'source':address,'target':next_at,'kind':'fallthrough'})
                pending.append(next_at)
            elif next_at > end:
                conflicts.append({'address':address,'reason':'instruction_crosses_witness_boundary'})
            elif kind in ('ordinary','conditional','indirect_conditional'):
                external.append({'source':address,'target':end,'kind':'witness_fallthrough'})
    # Form basic blocks from actual reachable leaders, not all disassembled bytes.
    leaders={start}
    for e in edges:
        if e['kind'] != 'fallthrough':
            leaders.add(e['target'])
        if e['source'] in visited and classify(instructions[e['source']])[0] != 'ordinary':
            if e['target'] in visited:leaders.add(e['target'])
    predecessors={x:[] for x in visited}
    successors={x:[] for x in visited}
    for e in edges:
        if e['source'] in visited and e['target'] in visited:
            predecessors[e['target']].append(e['source'])
            successors[e['source']].append(e['target'])
    for v in visited:
        if len(predecessors[v])>1:leaders.add(v)
        elif predecessors[v]:
            p=predecessors[v][0];typ,_=classify(instructions[p]);
            if typ!='ordinary':leaders.add(v)
    blocks=[];assigned=set()
    for lead in sorted(leaders & visited):
        if lead in assigned:continue
        address=lead; block=[]
        while address in visited and address not in assigned:
            assigned.add(address);block.append(address)
            choices=successors[address]
            if len(choices)!=1:break
            nxt=choices[0]
            if nxt!=address+instructions[address]['size'] or (nxt in leaders and nxt!=lead):break
            address=nxt
        last=block[-1]
        blocks.append({'start':lead, 'end_exclusive':last+instructions[last]['size'],
                       'instruction_count':len(block),'last_operation':instructions[last]['mnemonic']})
    if len(assigned) != len(visited):
        conflicts.append({'reason':'basic_block_partition_incomplete','missing':sorted(visited-assigned)})
    return {'reachable_instruction_count':len(visited),
            'linear_decoded_instruction_count':len(insns),
            'linear_decoded_but_not_reached':len(insns)-len(visited),
            'basic_blocks':blocks, 'edges':sorted(edges,key=lambda x:(x['source'],x['target'],x['kind'])),
            'external_targets':sorted(external,key=lambda x:((x['source'] or 0),x['target'])),
            'indirect_edges':sorted(unresolved,key=lambda x:x['address']),
            'service_boundaries':sorted(services,key=lambda x:x['address']),
            'conflicts':conflicts,
            'semantic_status':'candidate_static_cfg_not_runtime_reachability'}


def build(manifest: dict, original_root: Path, executable='objdump', source_index: dict | None=None) -> dict:
    if manifest.get('schema') != 'rloft-original-witness-manifest-v1':
        raise ValueError('unsupported manifest schema')
    version=decoder_version(executable)
    spans=manifest.get('witnesses',[])
    if not spans or len(spans)>200:
        raise ValueError('witness inventory must contain 1..200 entries')
    images={};objects={};seen=set();observed=[]
    for item in spans:
        module=item['module'];start=int(item['start'],16);end=int(item['end'],16)
        key=(module,start,end)
        if key in seen:raise ValueError('duplicate original witness')
        seen.add(key)
        if module not in ORIGINALS or start<0 or end<=start or end-start>16384:
            raise ValueError('unrecognized or unbounded witness')
        if module not in objects:
            binary=(original_root/ORIGINALS[module]['file']).read_bytes()
            code,meta=original_object(binary,module)
            objects[module]=code;images[module]=meta
        code=objects[module]
        if end>len(code):raise ValueError('witness exceeds authenticated code object')
        chunk=code[start:end]
        if sha(chunk)!=item['sha256']:
            raise ValueError(f'original witness {module}:{start:05X} span SHA mismatch')
        original=decode(chunk,start,executable)
        flow=graph(original,start,end)
        logical=f'{module}:{start:05x}'
        linked=[]
        if source_index is not None:
            linked=[f['id'] for f in source_index['functions']
                    if f['module']==module and f['offset']==start]
        observed.append({'id':logical,'start':start,'end_exclusive':end,
                         'length':len(chunk),'original_span_sha256':sha(chunk),
                         'original_file_sha256':ORIGINALS[module]['sha256'],
                         'source_index_matches':linked,
                         'source_status':'matched_symbol_name_only' if len(linked)==1 else
                                        'ambiguous_source_name' if linked else 'not_indexed_in_source',
                         'flow':flow})
    report={'schema':SCHEMA, 'authority':'authenticated_original_LE_bytes_independent_static_decode',
            'decoder':version,'originals':images,
            'witnesses':sorted(observed,key=lambda x:(x['id'],x['start'])),
            'dos_runtime_parity':False,'compiler_byte_match':False,
            'native_gameplay_parity':False}
    return report


def main(argv=None) -> int:
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--manifest',type=Path,default=Path(__file__).with_name('original_witnesses.json'))
    p.add_argument('--original-root',type=Path,required=True)
    p.add_argument('--source-index',type=Path,help='optional deterministic source graph JSON')
    p.add_argument('--output',type=Path,required=True)
    p.add_argument('--objdump',default='objdump')
    a=p.parse_args(argv)
    try:
        manifest=json.loads(a.manifest.read_text(encoding='utf-8'))
        index=json.loads(a.source_index.read_text(encoding='utf-8')) if a.source_index else None
        out=build(manifest,a.original_root,a.objdump,index)
        a.output.parent.mkdir(parents=True,exist_ok=True)
        a.output.write_text(json.dumps(out,sort_keys=True,indent=2)+'\n',encoding='utf-8')
        print(json.dumps({'result':'ORIGINAL_STATIC_CFG_CANDIDATE',
                          'witnesses':len(out['witnesses']),
                          'reachable_instructions':sum(w['flow']['reachable_instruction_count'] for w in out['witnesses']),
                          'conflicts':sum(len(w['flow']['conflicts']) for w in out['witnesses']),
                          'unresolved_indirect':sum(len(w['flow']['indirect_edges']) for w in out['witnesses']),
                          'runtime_parity':False},sort_keys=True))
        return 0
    except (OSError,ValueError,KeyError,subprocess.SubprocessError) as exc:
        print('ORIGINAL_CFG_REJECTED: '+str(exc),file=sys.stderr)
        return 1

if __name__=='__main__':raise SystemExit(main())
