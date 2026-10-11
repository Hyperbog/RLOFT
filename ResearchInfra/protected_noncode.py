#!/usr/bin/env python3
"""Import source-classified CODE.1 residual ranges; never rewrite source media.

An original CODE.1 file hash and the legacy residual ledger are separate
evidence sources. The ledger protects candidate decoding from known non-code;
it does NOT establish DOS reachability or classify unknown CODE.2 ranges.
"""
from __future__ import annotations
import hashlib
import re
from pathlib import Path

START='<!-- RESIDUAL-RANGES-BEGIN -->'
END='<!-- RESIDUAL-RANGES-END -->'
ROW=re.compile(r'([0-9A-F]{5})\.\.([0-9A-F]{5}) \| (alignment|table|noncode) \| ([0-9]+)')
ORIGINAL_CODE1_SIZE=0x11302
EXPECTED_CLASSIFIED=1041


def parse_ledger(raw: bytes, *, code_size: int=ORIGINAL_CODE1_SIZE, expected_total: int | None=EXPECTED_CLASSIFIED) -> dict:
    text=raw.decode('utf-8')
    if text.count(START)!=1 or text.count(END)!=1:
        raise ValueError('CODE.1 residual ledger markers must be unique')
    section=text.split(START,1)[1].split(END,1)[0].strip()
    spans=[];prev_end=0;counts={'alignment':0,'table':0,'noncode':0}
    for line in section.splitlines():
        m=ROW.fullmatch(line.strip())
        if not m:
            raise ValueError('invalid CODE.1 source residual range syntax')
        begin=int(m.group(1),16)
        end=int(m.group(2),16)+1
        declared=int(m.group(4))
        cls=m.group(3)
        if begin<prev_end or end<=begin or end>code_size or end-begin!=declared:
            raise ValueError('CODE.1 residual range overlaps, is invalid, or exceeds object')
        spans.append({'start':begin,'end_exclusive':end,
                      'class':cls,'authority':'source_ledger_declared_noncode'})
        counts[cls]+=declared
        prev_end=end
    if not spans or (expected_total is not None and sum(counts.values())!=expected_total):
        raise ValueError('CODE.1 residual ledger total differs from audited accounting')
    return {'source_sha256':hashlib.sha256(raw).hexdigest(),
            'ranges':spans,'counts':counts,'declared_noncode_bytes':sum(counts.values()),
            'classification':'original_research_ledger_not_runtime_execution'}


def load(root: Path) -> dict:
    return parse_ledger((root/'Code1'/'RESIDUAL_LEDGER.md').read_bytes())
