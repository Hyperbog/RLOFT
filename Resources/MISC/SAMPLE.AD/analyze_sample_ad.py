#!/usr/bin/env python3
from __future__ import annotations
import argparse, hashlib, json, struct
from collections import Counter, defaultdict
from pathlib import Path

EXPECTED_SHA256='79ee9f4ad3134be906ce493072ce0c3b7169c9013d3c25947ac385aaf3f9633b'
FIELD_NAMES=['transpose','mod_avekm','mod_ksl_tl','mod_ad','mod_sr','mod_ws','fb_c','car_avekm','car_ksl_tl','car_ad','car_sr','car_ws']

def parse(path:Path):
 d=path.read_bytes(); entries=[]; p=0
 while True:
  if p+2>len(d): raise ValueError('unterminated GTL header')
  patch,bank=d[p],d[p+1]
  if patch==0xff and bank==0xff:
   term=p; p+=2; break
  if p+6>len(d): raise ValueError('truncated GTL header entry')
  off=struct.unpack_from('<I',d,p+2)[0]
  entries.append((patch,bank,off)); p+=6
 if not entries: raise ValueError('no timbres')
 if entries[0][2]!=p: raise ValueError(f'first timbre offset {entries[0][2]} != header end {p}')
 banks=defaultdict(list); timbres=[]
 for i,(patch,bank,off) in enumerate(entries):
  if off+2>len(d): raise ValueError(f'timbre {i} offset beyond EOF')
  n=struct.unpack_from('<H',d,off)[0]
  if n<2 or off+n>len(d): raise ValueError(f'timbre {i} bad length {n}')
  end=off+n
  nextoff=entries[i+1][2] if i+1<len(entries) else len(d)
  if end!=nextoff: raise ValueError(f'timbre {i} end {end} != next offset/EOF {nextoff}')
  payload=d[off+2:end]
  if n!=14 or len(payload)!=12: raise ValueError(f'timbre {i} is not 14-byte OPL2 record')
  vals=dict(zip(FIELD_NAMES,[struct.unpack('b',payload[:1])[0],*payload[1:]]))
  timbres.append({'index':i,'patch':patch,'bank':bank,'offset':off,'length':n,'payload_sha256':hashlib.sha256(payload).hexdigest(),**vals})
  banks[bank].append(patch)
 summary={str(b):{'count':len(ps),'patch_min':min(ps),'patch_max':max(ps),'contiguous':ps==list(range(min(ps),max(ps)+1))} for b,ps in sorted(banks.items())}
 sha=hashlib.sha256(d).hexdigest()
 return {'bytes':len(d),'sha256':sha,'source_sha256_matches':sha==EXPECTED_SHA256,'header_entries':len(entries),'header_entry_bytes':len(entries)*6,'terminator_offset':term,'header_end_first_timbre':p,'terminator':'FFFF','bank_summary':summary,'length_counts':dict(Counter(t['length'] for t in timbres)),'transpose_counts':dict(sorted(Counter(t['transpose'] for t in timbres).items())),'final_timbre_end':timbres[-1]['offset']+timbres[-1]['length'],'ends_exactly_at_eof':timbres[-1]['offset']+timbres[-1]['length']==len(d),'timbres':timbres}

def main():
 ap=argparse.ArgumentParser(); ap.add_argument('file',type=Path); ap.add_argument('--json',action='store_true'); a=ap.parse_args(); r=parse(a.file)
 if a.json: print(json.dumps(r,indent=2))
 else:
  print(f"bytes={r['bytes']} sha256={r['sha256']}")
  print(f"entries={r['header_entries']} terminator=0x{r['terminator_offset']:X} first_timbre=0x{r['header_end_first_timbre']:X}")
  print(f"banks={r['bank_summary']}")
  print(f"lengths={r['length_counts']} eof_exact={r['ends_exactly_at_eof']}")
 return 0
if __name__=='__main__': raise SystemExit(main())
