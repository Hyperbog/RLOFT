"""Pure synthetic OMF32 relocation/hash tests. No original game bytes."""
from __future__ import annotations
import hashlib
import io
import json
import struct
import unittest
from unittest.mock import patch
from inspect_watcom_omf import OMFError
from match_watcom_omf import PROBES, compare_object
from test_inspect_watcom_omf import rec, text


def example(*, target='f0cc', payload=None, fix_type=0xE4, with_fixup=True):
    # Unrelated synthetic bytes: five NOPs, four unresolved operand bytes, RET.
    code = payload if payload is not None else b'\x90'*5 + bytes(4) + b'\xC3'
    records = [
        rec(0x80,text('synthetic')),
        rec(0x96,text('_TEXT')+text('CODE')),
        rec(0x99,b'\xa1'+struct.pack('<I',len(code))+b'\x01\x02\x00'),
        rec(0x91,b'\x00\x01'+text('_set_f0cc')+struct.pack('<I',0)+b'\x00'),
        rec(0x8C,text(target)+b'\x00'),
        rec(0xA1,b'\x01'+struct.pack('<I',0)+code),
    ]
    if with_fixup:
        records.append(rec(0x9D,bytes((fix_type,5,0x16,1,1))))
    records.append(rec(0x8B,b'\x00'))
    return b''.join(records)


class WatcomOMFMatcherTests(unittest.TestCase):
    def test_synthetic_exact_relocation_resolved_match(self):
        expected=b'\x90'*5 + struct.pack('<I',0xF0CC) + b'\xc3'
        with patch.dict(PROBES,{'set_f0cc':(10,hashlib.sha256(expected).hexdigest(),{'f0cc':0xF0CC})}):
            r=compare_object(example())
        self.assertEqual(r['matches'],1)
        x=r['probes']['set_f0cc']
        self.assertTrue(x['match'])
        self.assertEqual(x['applied_fixups'][0]['target'],'f0cc')
        self.assertEqual(x['candidate_size'],10)
        self.assertFalse(r['compiler_identified'])

    def test_corruption_fails_hash_after_relocation(self):
        corrupted=b'\x91'*5+bytes(4)+b'\xc3'
        expected=b'\x90'*5 + struct.pack('<I',0xF0CC) + b'\xc3'
        with patch.dict(PROBES,{'set_f0cc':(10,hashlib.sha256(expected).hexdigest(),{'f0cc':0xF0CC})}):
            r=compare_object(example(payload=corrupted))
        self.assertFalse(r['probes']['set_f0cc']['match'])

    def test_no_fixup_never_claims_match(self):
        expected=b'\x90'*5+struct.pack('<I',0xF0CC)+b'\xc3'
        with patch.dict(PROBES,{'set_f0cc':(10,hashlib.sha256(expected).hexdigest(),{'f0cc':0xF0CC})}):
            r=compare_object(example(payload=expected,with_fixup=False))
        self.assertFalse(r['probes']['set_f0cc']['match'])

    def test_relative_fixup_never_resolves_as_absolute(self):
        expected=b'\x90'*5+struct.pack('<I',0xF0CC)+b'\xc3'
        # locat with M=0 and 32-bit offset location 9; self-relative.
        with patch.dict(PROBES,{'set_f0cc':(10,hashlib.sha256(expected).hexdigest(),{'f0cc':0xF0CC})}):
            r=compare_object(example(fix_type=0xA4))
        self.assertFalse(r['probes']['set_f0cc']['match'])
        self.assertEqual(len(r['probes']['set_f0cc']['unresolved_fixups']),1)

    def test_unknown_external_is_not_accepted(self):
        expected=b'\x90'*5+struct.pack('<I',0xF0CC)+b'\xc3'
        with patch.dict(PROBES,{'set_f0cc':(10,hashlib.sha256(expected).hexdigest(),{'f0cc':0xF0CC})}):
            r=compare_object(example(target='unknown_global'))
        self.assertFalse(r['probes']['set_f0cc']['match'])

    def test_real_watcom_fixture_if_supplied(self):
        import os,zipfile
        zpath=os.environ.get('RLOFT_REAL_WATCOM_ARTIFACT')
        if not zpath:
            self.skipTest('real artifact optional; no proprietary object committed')
        with zipfile.ZipFile(zpath) as z:
            r=compare_object(z.read('386_stack_time/watcom_candidates.obj'))
        self.assertEqual(r['matches'],0)  # pre -s compiler has __CHK checks
        self.assertEqual(len(r['probes']),5)

if __name__=='__main__':
    unittest.main()
