#!/usr/bin/env python3
from __future__ import annotations
import argparse, hashlib, struct
from pathlib import Path

SHA = {
 'INIT':'cc48df4eb4c0710d45e7f5557e440129df44c5efb1af3ca3426ea9f9e3798276',
 'RES0':'5e30584947c9627e31b5dd2573ad75c2d3cc9fe796ac669e0ca5d02d6db7e61d',
 'RES1':'c13b8ae42f7d3b65c06d8413627f40a2a155a7404da44abfc158b38a4e5efd19',
 'RES4':'d8989c50fa58e317ff33465e7baec16e970f40586417d790fd8a1b9005c60f46',
 'RES5':'f943c933cfe593bb2912bc25e10b72f1ecafe10f684526f38489616cedbe91b6',
 'RES6':'697aa0803b2219dcb094bfae769ffb0c60cd7d89f7a7ba06c12ed6a4553f736d',
 'RES9':'c3021a8bb16bf1d8b9234ac47a670e4a41b8a1c078cba34be34f0f89a5135543',
 'RES10':'dc0c6b25a521f54d82bc564168859c74aa4b464413338d4b58d519b166e8e6f9',
 'OBJECTS':'f23f0e7c1856904aa0156b0bf174cab794a41a327352b6ca00c3591ae5f2eed0',
 'T':'a3f1e19e40f226c8cf150a43aacd9c378ee35a1294d2c9dd72870bfdf1e16315',
 'SAVE':'f898cb92fca6447954f5e0b2bf6f8ad2435e4a64ae3b9de74f8bf75e0ed10d5d',
}

def read_checked(p: Path, key: str) -> bytes:
 b=p.read_bytes(); got=hashlib.sha256(b).hexdigest()
 if got != SHA[key]: raise SystemExit(f'{key}: SHA mismatch {got}')
 return b

def res_entries(b: bytes):
 n=struct.unpack_from('<I',b,0)[0]; offs=struct.unpack_from(f'<{n}I',b,4)
 return [b[o:(offs[i+1] if i+1<n else len(b))] for i,o in enumerate(offs)]

def typ(e: bytes):
 return None if not e else (e[2] if len(e)>=3 and e[:2]==b'EH' else 'raw')

def decoded_size_type67(e: bytes) -> int:
 assert typ(e) in (6,7)
 return struct.unpack_from('<H',e,5)[0]

def t_text(b: bytes, idx: int) -> str:
 n=struct.unpack_from('<I',b,0)[0]; assert idx<n
 off=struct.unpack_from('<I',b,4+4*idx)[0]; ln=struct.unpack_from('<H',b,off)[0]
 return bytes(x^0xDC for x in b[off+2:off+2+ln]).decode('cp437')

def portrait(e: bytes) -> bool:
 if len(e)<13 or e[:3]!=b'EH\x01' or struct.unpack_from('<H',e,3)[0] != 8: return False
 return struct.unpack_from('<HHBBBB',e,5)==(80,1,6,255,3,1)

def main():
 ap=argparse.ArgumentParser()
 ap.add_argument('root',type=Path,help='directory containing INIT, RES0/1/4/5/6/9/10, OBJECTS.DAT, T, SAVE.DAT')
 a=ap.parse_args(); r=a.root
 init=read_checked(r/'INIT','INIT'); words=struct.unpack_from('<18H',init,0)
 expected=(15,168,336,0,2169,3316,4158,600,49,49,69,166,192,266,32,103,48,414)
 assert words==expected, (words, expected)
 e0=res_entries(read_checked(r/'RES0','RES0')); assert len(e0)==141
 assert all(typ(e)==2 for e in e0[:15]) and all(typ(e)==1 for e in e0[15:])
 e1=res_entries(read_checked(r/'RES1','RES1')); assert len(e1)==428
 obj=read_checked(r/'OBJECTS.DAT','OBJECTS'); n=struct.unpack_from('<I',obj,0)[0]; assert n==336
 ui=[struct.unpack_from('<H',obj,4+i*0x41+0x2D)[0] for i in range(n)]
 assert min(ui)==0 and max(ui)==167 and all(0 <= x < 168 for x in ui)
 assert all(portrait(e1[i]) for i in range(336,380))
 assert e1[380]==b'' and e1[381]==b'\x60\x00\x00\x00'
 assert all(portrait(e1[i]) for i in range(382,426))
 assert e1[426]==b'' and e1[427]==b'\x60\x00\x00\x00'
 e4=res_entries(read_checked(r/'RES4','RES4')); assert len(e4)==667 and all(typ(e)==6 for e in e4)
 sizes4=[decoded_size_type67(e) for e in e4]
 assert sizes4[:600]==[8192]*600 and sizes4[600:]==[4096]*67
 e5=res_entries(read_checked(r/'RES5','RES5')); assert len(e5)==98 and all(typ(e)==3 for e in e5)
 e6=res_entries(read_checked(r/'RES6','RES6')); assert len(e6)==266 and all(typ(e)==4 for e in e6)
 e9=res_entries(read_checked(r/'RES9','RES9')); assert len(e9)==555
 assert all(typ(e)==1 for e in e9[:517]) and all(typ(e)=='raw' for e in e9[517:])
 e10=res_entries(read_checked(r/'RES10','RES10')); assert len(e10)==219 and all(typ(e)==4 for e in e10)
 tb=read_checked(r/'T','T')
 assert [t_text(tb,i) for i in range(15,21)] == ['HUMAN','HALF-ELF','HALFLING','GNOME','DWARF','ELF']
 assert [t_text(tb,i) for i in (27,28)] == ['MALE','FEMALE']
 sb=read_checked(r/'SAVE.DAT','SAVE'); starts=[0x77+i*0x248 for i in range(4)]+[0xD68+i*0x248 for i in range(10)]
 checked=0
 for s in starts:
  rec=sb[s:s+0x248]
  if rec[:2]==b'\xff\xff': continue
  face=struct.unpack_from('<b',rec,0x12)[0]; sex=rec[0x13]; race=rec[0x14]
  assert 0<=face<23 and sex in (0,1) and 0<=race<6
  rid=336+23*sex+face
  assert portrait(e1[rid]) and portrait(e1[rid+46])
  checked+=1
 print('INIT fixed words:', ','.join(map(str,words)))
 print('RES0: 15 fonts then 126 type-1 graphics')
 print('RES1: OBJECTS UI local 0..167 -> absolute 168..335')
 print('RES1 portraits: 336..381 and 382..427; typed 44 + empty + raw marker in each 46-slot bank')
 print('RES4: 600 x 8192-byte decoded textures then 67 x 4096-byte decoded textures')
 print('RES5: 98 XMIDI/XDIR resources; INIT split=49')
 print('RES6: 266 VOC resources; INIT SFX boundaries=49,69,166,192,266')
 print('RES9: 517 typed graphics then 38 raw wordcode records; bases 32/103 and program displacement 414')
 print('RES10: 219 VOC resources; interaction voice base=48')
 print(f'SAVE/T identity cross-check: {checked} populated records; T[27:29]=MALE/FEMALE; T[15:21]=six races')
 return 0
if __name__=='__main__': raise SystemExit(main())
