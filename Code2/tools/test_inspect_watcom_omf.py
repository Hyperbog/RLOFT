"""Self-contained synthetic OMF32 fixture tests (no proprietary objects)."""
from __future__ import annotations

import io
import struct
import sys
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from inspect_watcom_omf import Cursor, OMFError, parse_omf, records


def rec(tag: int, payload: bytes, *, unchecked: bool = False) -> bytes:
    length = len(payload) + 1
    head = bytes((tag,)) + struct.pack('<H', length)
    check = 0 if unchecked else (-sum(head + payload)) & 0xff
    return head + payload + bytes((check,))


def text(value: str) -> bytes:
    v = value.encode('ascii')
    return bytes((len(v),)) + v


def example(*, bad_location: int = 1, thread: bool = False) -> bytes:
    # THEADR, LNAMES, one 32-bit CODE SEGDEF, one 32-bit PUBDEF,
    # one external, one LEDATA32 and one FIXUPP32 relocation.
    contents = [
        rec(0x80, text('fixture')),
        rec(0x96, text('CODE') + text('CLASS')),
        rec(0x99, b'\xa1' + struct.pack('<I', 16) + b'\x01\x02\x00'),
        rec(0x91, b'\x00\x01' + text('func') + struct.pack('<I', 0) + b'\x00'),
        rec(0x8C, text('callback') + b'\x00'),
        rec(0xA1, b'\x01' + struct.pack('<I', 0) + b'\xB8\0\0\0\0\xC3'),
    ]
    # LOCAT: 32-bit offset fixup location 9, segment-relative mode.
    loc = bytes((0xe4, bad_location))
    if thread:
        # Thread 0: explicit target method 2 (EXTDEF), index 1.
        # Target thread bit in FixDat; explicit frame method0 seg1.
        fix = b'\x08\x01' + loc + b'\x0c\x01'
    else:
        # FixDat=0x06 frame from SEGDEF1, explicit EXTDEF1, no target displacement.
        fix = loc + b'\x06\x01\x01'
    contents.extend((rec(0x9D, fix), rec(0x8B, b'\x00')))
    return b''.join(contents)


class OMF32Tests(unittest.TestCase):
    def test_genuine_omf32_record_structure_and_target(self):
        report = parse_omf(example())
        self.assertEqual(report['segments'][0]['name'], 'CODE')
        self.assertEqual(report['segments'][0]['size'], 16)
        self.assertEqual(report['public_symbols'][0]['name'], 'func')
        self.assertEqual(report['externals'], ['callback'])
        self.assertEqual(report['record_types']['9D'], 1)
        self.assertEqual(len(report['fixups']), 1)
        fixup = report['fixups'][0]
        self.assertEqual((fixup['offset'], fixup['width'], fixup['location_code']), (1, 4, 9))
        self.assertEqual(fixup['target_external'], 'callback')
        self.assertEqual(fixup['target_displacement'], 0)
        self.assertFalse(fixup['self_relative'])
        self.assertFalse(report['code_bytes_matched'])
        self.assertFalse(report['fixups_applied'])
        self.assertEqual(report['unparsed_code_features'], [])

    def test_thread_target_reuse_and_explicit_frame(self):
        fixup = parse_omf(example(thread=True))['fixups'][0]
        self.assertEqual(fixup['target_external'], 'callback')
        self.assertEqual(fixup['frame_index'], 1)

    def test_checksum_rejects_corruption(self):
        original = bytearray(example())
        original[6] ^= 1
        with self.assertRaisesRegex(OMFError, 'bad OMF checksum'):
            parse_omf(bytes(original))

    def test_explicit_unchecked_omf_checksum_reported(self):
        original = example()
        extra = rec(0x88, b'\x00\x00test', unchecked=True)
        at = len(rec(0x80, text('fixture')))
        report = parse_omf(original[:at] + extra + original[at:])
        self.assertEqual(report['unchecked_checksum_records'], 1)

    def test_cross_data_boundary_relocation_fails_closed(self):
        with self.assertRaisesRegex(OMFError, 'crosses source LEDATA boundary'):
            parse_omf(example(bad_location=3))

    def test_bad_name_index_and_segment_overrun(self):
        x = example()
        good_seg = rec(0x99, b'\xa1' + struct.pack('<I', 16) + b'\x01\x02\x00')
        bad_seg = rec(0x99, b'\xa1' + struct.pack('<I', 16) + b'\x09\x02\x00')
        with self.assertRaisesRegex(OMFError, 'unknown LNAMES'):
            parse_omf(x.replace(good_seg, bad_seg, 1))
        too_small = rec(0x99, b'\xa1' + struct.pack('<I', 4) + b'\x01\x02\x00')
        with self.assertRaisesRegex(OMFError, 'LEDATA exceeds'):
            parse_omf(x.replace(good_seg, too_small, 1))

    def test_missing_module_end_rejected(self):
        x = example()
        with self.assertRaisesRegex(OMFError, 'missing OMF MODEND'):
            parse_omf(x[:-5])

    def test_unsupported_comdat_never_accepted_as_matched_code(self):
        x = example()
        off = len(x) - len(rec(0x8B, b'\x00'))
        result = parse_omf(x[:off] + rec(0xC3, b'\0\0') + x[off:])
        self.assertIn('code/data record C3 (LIDATA/COMDAT)', result['unparsed_code_features'])
        self.assertFalse(result['code_bytes_matched'])

    def test_fixup_record_requires_immediately_previous_ledata(self):
        x = example()
        fix = rec(0x9D, b'\xe4\x01\x06\x01\x01')
        intermediate = rec(0x88, b'\x00\x00comment')
        with self.assertRaisesRegex(OMFError, 'without previous LEDATA'):
            parse_omf(x.replace(fix, intermediate + fix, 1))

    def test_undefined_target_thread_rejected(self):
        x = example()
        fix = rec(0x9D, b'\xe4\x01\x0c\x01')
        original = rec(0x9D, b'\xe4\x01\x06\x01\x01')
        with self.assertRaisesRegex(OMFError, 'undefined FIXUPP target thread'):
            parse_omf(x.replace(original, fix, 1))

    def test_overlapping_ledatas_and_invalid_length_rejected(self):
        x = example()
        second = rec(0xA1, b'\x01' + struct.pack('<I', 3) + b'ABC')
        tag = rec(0x9D, b'\xe4\x01\x06\x01\x01')
        with self.assertRaisesRegex(OMFError, 'overlapping LEDATA'):
            parse_omf(x.replace(tag, second + tag, 1))
        with self.assertRaisesRegex(OMFError, 'truncated OMF record'):
            list(records(rec(0x80, text('x'))[:-1]))

    def test_omf_index_decoding(self):
        c = Cursor(b'\x80\x80\x7f')
        self.assertEqual(c.index(), 128)
        self.assertEqual(c.index(), 127)
        c.done()


if __name__ == '__main__':
    unittest.main()
