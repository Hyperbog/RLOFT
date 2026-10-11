"""Offline unit tests for the Windows-only historical compiler pilot."""
from __future__ import annotations
import json
from pathlib import Path
import tempfile
import types
import unittest
from unittest.mock import patch

from run_watcom10_windows_probe import run_probe, verify_toolchain, VARIANTS


class WatcomWindowsProbeTests(unittest.TestCase):
    def test_non_windows_refuses_to_execute(self):
        with tempfile.TemporaryDirectory() as d:
            with self.assertRaisesRegex(RuntimeError, 'real Windows'):
                run_probe(Path(d), Path(d)/'out', windows=False)

    def test_missing_toolchain_rejected(self):
        with tempfile.TemporaryDirectory() as d:
            with self.assertRaisesRegex(ValueError, 'missing Watcom'):
                verify_toolchain(Path(d))

    def test_bad_binary_checksum_rejected(self):
        with tempfile.TemporaryDirectory() as d:
            root = Path(d)
            (root/'binnt').mkdir()
            (root/'binnt'/'wcc386.exe').write_bytes(b'evil')
            with self.assertRaisesRegex(ValueError, 'binary mismatch'):
                verify_toolchain(root)

    def test_validated_toolchain_layout_and_failed_compile_report(self):
        # Simulated compiler failure without attempting to execute a PE.
        with tempfile.TemporaryDirectory() as d:
            root = Path(d)/'watcom'
            for path in ('binnt','binb','h'):
                (root/path).mkdir(parents=True)
            for path in ('binnt/wcc386.exe','binb/wcc386.exe'):
                (root/path).write_bytes(b'synthetic')
            out = Path(d)/'out'
            def fake_run(args, **kwargs):
                self.assertIn('-bt=dos',args)
                self.assertIn('-s',args)
                self.assertTrue(kwargs['env']['WCC386'] == '')
                return types.SimpleNamespace(returncode=1,stdout='',stderr='syntax intentionally failed')
            with patch('run_watcom10_windows_probe.verify_toolchain',return_value={'synthetic':'only'}):
                result = run_probe(root,out,windows=True,executor=fake_run)
            self.assertEqual(len(result['profiles']),len(VARIANTS))
            self.assertTrue(all(not x['object_present'] for x in result['profiles']))
            self.assertEqual(json.loads((out/'report.json').read_text())['compiler'], 'Watcom C 10.0a')
            with self.assertRaises(FileExistsError):
                with patch('run_watcom10_windows_probe.verify_toolchain',return_value={'synthetic':'only'}):
                    run_probe(root,out,windows=True,executor=fake_run)


if __name__ == '__main__':
    unittest.main()
