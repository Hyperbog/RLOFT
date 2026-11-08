#!/usr/bin/env python3
"""Source-only: preserve original intro flow and retire player Level22 preview."""
from pathlib import Path
import re
import unittest

ROOT=Path(__file__).resolve().parents[2]
HOST=ROOT/"DemoV1/src/win32_main.cpp"
CMD=ROOT/"DemoV1/Windows/Play-Original-Intro.cmd"
WF=ROOT/".github/workflows/demo-v1-build.yml"

class FrontEndContract(unittest.TestCase):
    def test_launcher_forces_intro_not_preview(self):
        text=CMD.read_text(encoding="utf-8")
        lines=[s.strip() for s in text.splitlines()
               if s.strip().startswith('"%~dp0ravenloft_demo_v1.exe"')]
        self.assertEqual(lines,
            ['"%~dp0ravenloft_demo_v1.exe" --front-end-only --choose-install'])
        self.assertNotIn('%*',text)
        self.assertNotRegex(text.lower(),
            r'(?m)^\s*(?:del|erase|move|ren|copy)\s+.*(?:chars|save\.dat)')
        self.assertIn("never changes CHARS or SAVE.DAT",text)

    def test_showcase_preserves_original_normal_startup(self):
        host=HOST.read_text(encoding="utf-8")
        self.assertIn('arg == L"--front-end-only"',host)
        self.assertIn("if(result.front_end_only && result.explore_level22)",host)
        self.assertIn("if (options.choose_install)",host)
        self.assertRegex(host,re.compile(
            r'auto startup_chars = options\.front_end_only\s*\?\s*'
            r'std::optional<std::vector<std::uint8_t>>\{\}\s*:\s*'
            r'source\.read_runtime\("CHARS"\);'))
        begin=host.index('const auto image =\n'
                         '                                    rloft::build_character_transfer(')
        guard=host.index('if (options.front_end_only)',begin)
        disk_write=host.index('std::filesystem::remove(path, ec);',begin)
        self.assertLess(guard,disk_write)
        self.assertIn('return 0;',host[guard:disk_write])
        for proof in ('assets.source().read("CINE00.ANM")',
                      'assets.source().read("CINE12.ANM")',
                      'presenter.first_logo()',
                      'presenter.second_logo()',
                      'presenter.third_logo()',
                      'presenter.menu_animation_frame(step)',
                      'rloft::CharacterCreationPresenter character_presenter(assets)',
                      'if (startup_chars) {',
                      'return run_exploration('):
            self.assertIn(proof,host)

    def test_release_contains_no_level22_preview_launchers(self):
        wf=WF.read_text(encoding="utf-8")
        self.assertIn("Ravenloft-Original-Intro-Windows-x64",wf)
        self.assertIn('Copy-Item DemoV1/Windows/Play-Original-Intro.cmd ',wf)
        self.assertIn("github.event_name == 'push' || inputs.build_mode == 'full'",wf)
        for name in ('Explore-Level22.cmd','Capture-Level22.cmd',
                     'Diagnose-Level22.cmd','Probe-Level22-Ground.cmd',
                     'Trace-Normal-Launch.cmd'):
            self.assertNotIn('Copy-Item DemoV1/Windows/'+name+' ',wf)
            self.assertNotIn('build/demo-v1/Release/'+name,wf)

if __name__=="__main__":
    unittest.main()
