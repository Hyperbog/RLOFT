"""Original CODE.2 selection loop 130A0->130DE F790 event parity.

The immutable executable is authoritative; the candidate function graph is
explicitly non-authoritative, including its missing 130DE function assignment.
"""
from __future__ import annotations
import argparse
import hashlib
import struct
from pathlib import Path

ROOT=Path(__file__).resolve().parents[2]
ORIGINAL_SHA="853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314"
OBJECT1=0x3AF24
WINDOWS={
    0x1309B:"5f5e5bc3905356beffffffff31",
    0x130A0:"5356beffffffff31dbe84671000085c075510fbf1d339e0000",
    0x130B2:"0fbf1d339e000039f374ec3b1de85802007e068b1de8580200",
    0x130BD:"3b1de85802007e068b1de8580200",
    0x130CB:"0fb60550c0000050e8d860010083c4046a0053e8adc6ffff83",
    0x130DB:"6a0053e8adc6ffff83c40868400100006a106a",
    0x130E6:"68400100006a106a006a62ff3550c0000089dee8ca77000083c414",
    0x130F7:"89dee8ca77000083c414eba6a801740e6a00",
    0x13103:"a801740e6a000fbfc350e83272000083c4085e5bc3",
}

def verify_binary(path:Path)->None:
    original=path.read_bytes()
    assert len(original)==558689
    assert hashlib.sha256(original).hexdigest()==ORIGINAL_SHA
    object1=original[OBJECT1:OBJECT1+0x3A852]
    for location,expected in WINDOWS.items():
        signature=bytes.fromhex(expected)
        assert object1[location:location+len(signature)]==signature,hex(location)
    for location,target in ((0x130A9,0x1A1F4),(0x130D3,0x291B0),
                            (0x130DE,0xF790),(0x130F9,0x1A8C8),
                            (0x1310D,0x1A344)):
        assert object1[location]==0xE8,hex(location)
        assert location+5+struct.unpack_from('<i',object1,location+1)[0]==target,hex(location)
    # Isolated 130A0 source boundary: follows previous RET+NOP;
    # 13117 is RET before adjacent modal routine at 13118.
    assert object1[0x1309e:0x130a1]==b'\xc3\x90\x53'
    assert object1[0x13117]==0xc3
    print("PASS: 9 original selected-loop signatures, 5 rel32 targets and two boundaries")

def verify_source()->None:
    h=(ROOT/'DemoV1/include/rloft/original_f790_selected_poll.hpp').read_text()
    t=(ROOT/'DemoV1/tests/original_f790_selected_poll_tests.cpp').read_text()
    cmake=(ROOT/'DemoV1/CMakeLists.txt').read_text()
    candidate=(ROOT/'DemoV1/tools/analyze_original_f790_candidate_coverage.py').read_text()
    host=(ROOT/'DemoV1/src/win32_main.cpp').read_text()
    for requirement in ('0x130deU','previous_selected_=-1',
                        'raw==previous_selected_','raw>maximum_258e8',
                        'previous_selected_=selected','SourceEvent{kSourceE8,selected,false}'):
        assert requirement in h,requirement
    assert 'i<1000' in t and 'item_id==12' in t
    assert 'original_f790_selected_poll_tests' in cmake
    assert 'NAME original_f790_selected_poll_binary_contract_tests' in cmake
    assert '0x130DE' in candidate
    assert 'OriginalF790SelectedPoll' not in host
    print('PASS: pure selected-item source event, redraw-on-clamped-input invariant')

def main()->None:
    ap=argparse.ArgumentParser(description=__doc__)
    ap.add_argument('--original-code2',type=Path)
    ap.add_argument('--binary-only',action='store_true')
    args=ap.parse_args()
    if not args.binary_only: verify_source()
    if args.original_code2: verify_binary(args.original_code2)
    elif args.binary_only: ap.error('--binary-only requires --original-code2')

if __name__=='__main__':main()
