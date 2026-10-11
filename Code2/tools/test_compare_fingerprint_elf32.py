"""Regression tests for source-only parser and opt-in original-file controls."""
from __future__ import annotations

import os
from pathlib import Path
import shutil
import struct
import tempfile
import unittest

from compare_fingerprint_elf32 import (
    PROBES, compare_probe, extract_elf_function, _check_slice,
)
from run_elf32_controls import run

ROOT = Path(__file__).parent
EXAMPLE = ROOT/'probes'/'elf32_negative_controls.c'


class Elf32Controls(unittest.TestCase):
    def test_no_unverified_target_ranges(self):
        self.assertEqual(PROBES['set_f0c8'][0:2], (0x2E74C, 0x2E756))
        self.assertEqual(PROBES['call_f0c8'][0:2], (0x2E758, 0x2E768))
        self.assertEqual(PROBES['get_2b680'][2], {'g2b680': 0x2B680})

    def test_bounds(self):
        self.assertEqual(_check_slice(b'abcdef', 2, 3), b'cde')
        for off, size in [(-1, 2), (6, 1), (0, -1)]:
            with self.subTest(off=off, size=size):
                with self.assertRaises(ValueError):
                    _check_slice(b'abcdef', off, size)

    def test_reject_non_elf_and_corrupt_header(self):
        with tempfile.TemporaryDirectory() as d:
            f = Path(d)/'fake.o'
            f.write_bytes(b'OMF file')
            with self.assertRaisesRegex(ValueError, 'requires little-endian ELF32'):
                extract_elf_function(f, 'set_f0c8')
            f.write_bytes(b'\x7fELF\x02\x01\x01' + b'\x00'*100)
            with self.assertRaisesRegex(ValueError, 'requires little-endian ELF32'):
                extract_elf_function(f, 'set_f0c8')

    @unittest.skipUnless(os.environ.get('RLOFT_ORIGINAL_CODE2'),
                         'set RLOFT_ORIGINAL_CODE2 for optional authenticated binary tests')
    def test_wrong_game_version_rejected(self):
        with tempfile.TemporaryDirectory() as d:
            bad = Path(d)/'CODE.2'
            bad.write_bytes(b'tampered')
            with self.assertRaisesRegex(ValueError, 'not the pinned CODE.2 original'):
                run(bad, EXAMPLE, ['gcc'])

    @unittest.skipUnless(os.environ.get('RLOFT_ORIGINAL_CODE2') and shutil.which('gcc'),
                         'requires original CODE.2 and 32-bit GCC')
    def test_original_rel32_resolution_and_indirect_mismatch(self):
        original = Path(os.environ['RLOFT_ORIGINAL_CODE2'])
        with tempfile.TemporaryDirectory() as d:
            output = Path(d)/'control.o'
            import subprocess
            cmd = ['gcc','-m32','-O2','-fno-pic','-fno-pie','-ffreestanding',
                   '-fno-asynchronous-unwind-tables','-fno-unwind-tables',
                   '-fno-stack-protector','-fomit-frame-pointer','-c',str(EXAMPLE),'-o',str(output)]
            subprocess.run(cmd, check=True, capture_output=True)
            data, relocs = extract_elf_function(output, 'set_f0c8')
            self.assertEqual(len(data), 10)
            self.assertEqual(relocs, [(5, 1, 'f0c8', 0)])
            self.assertTrue(compare_probe(original, output, 'set_f0c8')['relocation_resolved_exact_bytes'])
            self.assertFalse(compare_probe(original, output, 'call_f0c8')['relocation_resolved_exact_bytes'])

    @unittest.skipUnless(os.environ.get('RLOFT_ORIGINAL_CODE2') and shutil.which('gcc'),
                         'requires original CODE.2 and GCC')
    def test_reject_wrong_relocation_target_type(self):
        original = Path(os.environ['RLOFT_ORIGINAL_CODE2'])
        with tempfile.TemporaryDirectory() as d:
            import subprocess
            obj = Path(d)/'candidate.o'
            subprocess.run(['gcc','-m32','-O2','-fno-pic','-fno-pie','-ffreestanding',
                            '-fno-asynchronous-unwind-tables','-fno-unwind-tables',
                            '-fno-stack-protector','-fomit-frame-pointer','-c',str(EXAMPLE),'-o',str(obj)],
                           check=True,capture_output=True)
            raw=bytearray(obj.read_bytes())
            shoff=struct.unpack_from('<I',raw,32)[0]
            count=struct.unpack_from('<H',raw,48)[0]
            for i in range(count):
                at=shoff+i*40
                if struct.unpack_from('<I',raw,at+4)[0]==9: # SHT_REL
                    reloff=struct.unpack_from('<I',raw,at+16)[0]
                    rinfo=struct.unpack_from('<I',raw,reloff+4)[0]
                    # Change only the relocation kind, preserve the symbol ID.
                    struct.pack_into('<I',raw,reloff+4,(rinfo & ~0xFF)|2)
                    break
            obj.write_bytes(raw)
            with self.assertRaisesRegex(ValueError, 'unsupported relocation'):
                compare_probe(original,obj,'set_f0cc')


if __name__ == '__main__':
    unittest.main()
