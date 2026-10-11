#!/usr/bin/env python3
"""Original CODE.2 keyboard held-state and raw-scancode IRQ evidence.

Not proof of Windows WM_KEY*, installed ISR, DOS PIC/IRQ timing or parity.
"""
from __future__ import annotations
import argparse
import hashlib
from pathlib import Path
import struct

ROOT=Path(__file__).resolve().parents[2]
ORIGINAL_SHA='853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314'
BOUNDS={
 (0x30748,0x307C8):'5c02396e39d1f70b88b50ac04972c25ce1bc3064fb836a291b60bde43ca04cfd',
 (0x307C8,0x307F0):'2b850e58e8998dec00467f93084fb33291739c5e200ec0cc49895c5e149c2059',
 (0x307F0,0x308B9):'f44711e3a4376548b98e8c4bf72fc99e9982de685e5cf751f196164009dae716',
 (0x308BC,0x308F5):'d477d5952d4a8495767276a8e2b0b56a0f1823cc89d15724ea8543a32dd7adf4',
 (0x30950,0x30ABA):'509b4f6b6abc6b0292e6adccfc0bbf98dddf6bf5079acb8ea7ae54678438783b',
}
KEYPAD_TARGETS=(0x30857,0x30843,0x3086B,0x308A7,0x30876,
                0x308A7,0x3088A,0x308A7,0x308A7,0x30895)
WINDOWS={
 0x3075C:'0fb61588f50000',0x30777:'e858050000',
 0x30784:'fe0d88f50000',0x307A1:'c60572f5000000',
 0x307B1:'8a80c3b60200',0x307B7:'a272f50000',
 0x307C1:'a271f50000',0x307C6:'5bc3',
 0x30805:'e83effffff',0x30822:'8898c4b60200',
 0x3082D:'fe0588f50000',0x30833:'8d43b9',
 0x30836:'83f809',0x3083B:'2eff2485c8070300',
 0x308A7:'881d72f50000',0x308B2:'a271f50000',
 0x308BD:'8b5c2408',0x308D2:'0fb690c4b60200',
 0x30951:'81fb80000000',0x30957:'0f8e6b000000',
 0x3095D:'83e37f',0x30965:'802573f50000fe',
 0x309CD:'800d73f5000001',0x309DB:'800d73f5000002',
 0x309E9:'800d73f5000004',0x309F7:'800d73f5000008',
 0x309AF:'e894fdffff',0x30A0B:'e8e0fdffff',
 0x309C1:'a272f50000',0x30A13:'881d71f50000',
 0x30A19:'881d72f50000',0x30A26:'83f810',
 0x30A30:'2404',0x30A39:'800db1f0000001',
 0x30A40:'c60572f5000000',0x30A50:'83f843',
 0x30A55:'c60572f5000000',0x30A75:'83f819',
 0x30A7A:'66c7057c4501000100',
 0x30A9C:'83f839',0x30AA1:'c60574f5000001',
 0x30ABA:'8bc0',0x30ABC:'0fb60573f50000',
 0x30AC8:'0fb60574f50000',0x30ACF:'c60574f5000000',
}
E8_EDGES={0x30777:0x30CD4,0x30805:0x30748,0x309AF:0x30748,
          0x30A0B:0x307F0}

def audit_original(path:Path):
 b=path.read_bytes()
 assert len(b)==558689 and hashlib.sha256(b).hexdigest()==ORIGINAL_SHA,'wrong original CODE.2'
 obj=b[0x3AF24:0x75F24]
 for (lo,hi),sha in BOUNDS.items():
  assert hashlib.sha256(obj[lo:hi]).hexdigest()==sha,hex(lo)
 targets=struct.unpack_from('<10I',obj,0x307C8)
 assert targets==KEYPAD_TARGETS,targets
 for at,code in WINDOWS.items():
  expected=bytes.fromhex(code)
  assert obj[at:at+len(expected)]==expected,hex(at)
 for at,target in E8_EDGES.items():
  assert obj[at]==0xE8 and at+5+struct.unpack_from('<i',obj,at+1)[0]==target,hex(at)
 # F588/F571..574 initial zero; mutable runtime stack is not fixed-sized.
 data=b[0x75F24:]
 assert data[0xF588]==0 and data[0xF571:0xF575]==bytes(4)
 print(f'ORIGINAL_PASS: CODE.2 keyboard 5 spans, {len(WINDOWS)} op windows, 4 E8 and 10 dispatch entries')


def audit_repo():
 cm=(ROOT/'DemoV1/CMakeLists.txt').read_text()
 src=(ROOT/'DemoV1/src/original_dos_keyboard_irq.cpp').read_text()
 hdr=(ROOT/'DemoV1/include/rloft/original_dos_keyboard_irq.hpp').read_text()
 tst=(ROOT/'DemoV1/tests/original_dos_keyboard_irq_tests.cpp').read_text()
 doc=(ROOT/'DemoV1/ORIGINAL_DOS_KEYBOARD_IRQ_20261011.md').read_text()
 for name in ('release_30748','press_307f0','held_308bc','dispatch_30950'):
  assert all(name in s for s in (src,hdr,tst)),name
 assert 'NAME original_dos_keyboard_irq_tests' in cm
 assert 'NAME original_dos_keyboard_irq_binary_contract_tests' in cm
 for term in ('F588','F571','F572','F573','2B6C4','307C8','30A1F','Win32'):
  assert term in doc,term
 print('SOURCE_ONLY_PASS: keyboard model source/test/original audit registration')


if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__)
 p.add_argument('--original-code2',type=Path)
 p.add_argument('--binary-only',action='store_true')
 a=p.parse_args()
 if a.binary_only and a.original_code2 is None:p.error('--binary-only requires --original-code2')
 if not a.binary_only:audit_repo()
 if a.original_code2 is not None:audit_original(a.original_code2)
