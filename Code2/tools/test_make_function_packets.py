"""No game assets needed for the source-only validation tests."""
from __future__ import annotations

import hashlib
from pathlib import Path
import tempfile
import unittest

from make_function_packets import build_packet, load_original, parse_targets, parse_offset, SPECS


class WorkPacketTests(unittest.TestCase):
    def test_offsets(self):
        self.assertEqual(parse_offset('0x2e74c'), 0x2e74c)
        self.assertEqual(parse_offset('12'), 12)
        with self.assertRaises(ValueError):
            parse_offset('-1')
        with self.assertRaises(ValueError):
            parse_offset('0xzz')
        with self.assertRaises(ValueError):
            parse_offset(True)

    def test_targets_reject_cross_boundary_and_duplicates(self):
        with tempfile.TemporaryDirectory() as d:
            p = Path(d) / 'targets.json'
            p.write_text('[{"start":"0x10","end":"0x20"}]')
            self.assertEqual(parse_targets(p, 32), [(16, 32)])
            p.write_text('[{"start":"0x10","end":"0x21"}]')
            with self.assertRaises(ValueError):
                parse_targets(p, 32)
            p.write_text('[{"start":"0x10","end":"0x20"},{"start":"0x10","end":"0x20"}]')
            with self.assertRaises(ValueError):
                parse_targets(p, 32)

    def test_refuse_wrong_binary_version_and_name(self):
        with tempfile.TemporaryDirectory() as d:
            p = Path(d) / 'CODE.2'
            p.write_bytes(b'nope')
            with self.assertRaisesRegex(ValueError, 'not the pinned'):
                load_original(p)
            q = Path(d) / 'malicious.exe'
            q.write_bytes(b'')
            with self.assertRaisesRegex(ValueError, 'exact filename'):
                load_original(q)

    def test_packet_hash_and_no_overwrite(self):
        with tempfile.TemporaryDirectory() as d:
            obj = bytes(range(64))
            spec = {'size': 100, 'sha256': 'synthetic-sha', 'object_offset': 16, 'object_length': 64}
            out = Path(d) / 'scratch'
            meta = build_packet('CODE.2', spec, obj, 4, 10, out)
            self.assertEqual(meta['physical_file_start'], '0x14')
            self.assertEqual(meta['selected_sha256'], hashlib.sha256(bytes(range(4,10))).hexdigest())
            self.assertEqual(meta['compiler_match'], 'not_tested')
            self.assertFalse((out / 'candidate_disassembly.txt').exists())
            self.assertNotIn('asm', (out/'packet.json').read_text().lower())
            with self.assertRaises(FileExistsError):
                build_packet('CODE.2', spec, obj, 4, 10, out)

    def test_exact_identity_specs(self):
        self.assertEqual(SPECS['CODE.2']['object_offset'], 0x3AF24)
        self.assertEqual(SPECS['CODE.2']['object_length'], 0x3A852)
        self.assertEqual(SPECS['CODE.1']['object_offset'], 0x2C724)
        self.assertEqual(SPECS['CODE.1']['object_length'], 0x11302)


if __name__ == '__main__':
    unittest.main()
