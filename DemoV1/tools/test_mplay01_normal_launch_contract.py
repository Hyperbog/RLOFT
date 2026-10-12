#!/usr/bin/env python3
"""Fail-closed source-only gate: M-PLAY-01 normal launcher cannot warp."""
from pathlib import Path
import re
import unittest

ROOT = Path(__file__).resolve().parents[2]
WIN = ROOT / "DemoV1/Windows/Trace-Normal-Launch.cmd"
WORKFLOW = ROOT / ".github/workflows/demo-v1-build.yml"
HOST = ROOT / "DemoV1/src/win32_main.cpp"


class NormalLaunchContract(unittest.TestCase):
    def test_only_normal_trace_launch(self):
        text = WIN.read_text(encoding="utf-8")
        lines = [s.strip() for s in text.splitlines()]
        commands = [s for s in lines if s.lower().startswith('"%~dp0ravenloft_demo_v1.exe"')]
        self.assertEqual(commands, ['"%~dp0ravenloft_demo_v1.exe" --dev-trace'])
        # Forbid forwarding arbitrary flags, injected parties or dev gameplay.
        self.assertNotIn('%*', text)
        for flag in ('--explore-level22', '--dev-inspect', '--dev-interact',
                     '--capture-render', '--explore '):
            self.assertNotIn(flag, commands[0])
        self.assertIn('latest-session.txt', text)
        self.assertIn('Compress-Archive', text)
        self.assertIn('CHARS and SAVE.DAT', text)
        self.assertNotRegex(text.lower(), r'del\s+[^\n]*\b(chars|save\.dat)\b')

    def test_packaging_is_explicit_and_reproducible(self):
        workflow = WORKFLOW.read_text(encoding="utf-8")
        name = "Trace-Normal-Launch.cmd"
        self.assertIn(f"Copy-Item DemoV1/Windows/{name} ", workflow)
        self.assertIn(f"build/demo-v1/Release/{name}", workflow)
        self.assertIn('build-source-sha.txt', workflow)
        self.assertIn('workflow_dispatch:', workflow)
        # The host already accepts --dev-trace without --explore-level22.
        source = HOST.read_text(encoding="utf-8")
        self.assertIn('arg == L"--dev-trace"', source)
        self.assertIn('result.dev_trace = true;', source)
        self.assertGreaterEqual(source.count('options.dev_trace'), 3)
        self.assertTrue(re.search(r'\*startup_chars,false,false,16U,false,false,options\.dev_trace', source))


if __name__ == '__main__':
    unittest.main()
