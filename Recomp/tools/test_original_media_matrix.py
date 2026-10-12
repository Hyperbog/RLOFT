#!/usr/bin/env python3
"""Adversarial offline tests of the real-media CODE.2 execution matrix."""
import hashlib
import json
from pathlib import Path
import tempfile
import unittest
from unittest import mock
import original_media_matrix as m

class MediaMatrixTests(unittest.TestCase):
    def test_full_path_coverage(self):
        self.assertEqual(len(m.SOURCE_MODES),12)
        self.assertEqual(len({c[0] for c in m.SOURCE_MODES}),12)
        self.assertEqual(len({c[1] for c in m.SOURCE_MODES}),12)
        self.assertEqual(sum(x[1].startswith("--to-mouse-reset") for x in m.SOURCE_MODES),4)
        self.assertTrue(all(v[2].startswith("STOP_BEFORE_") for v in m.SOURCE_MODES))
    def test_authentication_rejects_mutation(self):
        with tempfile.TemporaryDirectory() as d:
            p=Path(d)/"CODE.2"
            p.write_bytes(b"test media")
            known=hashlib.sha256(b"test media").hexdigest()
            self.assertEqual(m.digest(p,known,"test")["size"],10)
            p.write_bytes(b"test mediA")
            with self.assertRaisesRegex(ValueError,"wrong version/hash"):
                m.digest(p,known,"test")
    def test_real_media_is_not_assumed_from_fixture(self):
        with tempfile.TemporaryDirectory() as d:
            p=Path(d)/"exe.exe"; p.write_bytes(b"x")
            code=Path(d)/"CODE.2"; code.write_bytes(b"fixture")
            report=Path(d)/"report.json"
            calls=[]
            def fake_exec(exe,mode,source,sound=None,timeout=120):
                calls.append((mode,sound))
                bound=next(t for n,v,t in m.SOURCE_MODES if v==mode)
                return dict(mode=mode,returncode=0,reported_boundary=bound,
                            media="SYNTHETIC_FIXTURE",source_entry=True,
                            game_boot_claim=True,stderr="")
            with mock.patch.object(m,"CODE2_HASH",hashlib.sha256(b"fixture").hexdigest()),\
                 mock.patch.object(m,"execute",side_effect=fake_exec):
                self.assertEqual(m.main(["--exe",str(p),"--code2",str(code),
                                          "--report",str(report)]),0)
            payload=json.loads(report.read_text(encoding="utf-8"))
            self.assertTrue(payload["passed"])
            self.assertFalse(payload["game_booted"])
            self.assertFalse(payload["original_dos_runtime_parity"])
            self.assertEqual(len(calls),12)
            self.assertTrue(all(arg is None for _,arg in calls))
    def test_one_bad_boundary_fails_closed(self):
        with tempfile.TemporaryDirectory() as d:
            p=Path(d)/"exe.exe";p.write_bytes(b"x")
            code=Path(d)/"CODE.2";code.write_bytes(b"fixture")
            report=Path(d)/"audit.json"
            def fake_exec(exe,mode,source,sound=None,timeout=120):
                return dict(mode=mode,returncode=0,reported_boundary="WRONG",
                            media="SYNTHETIC_FIXTURE",source_entry=True,
                            game_boot_claim=True,stderr="")
            with mock.patch.object(m,"CODE2_HASH",hashlib.sha256(b"fixture").hexdigest()),\
                 mock.patch.object(m,"execute",side_effect=fake_exec):
                self.assertEqual(m.main(["--exe",str(p),"--code2",str(code),
                                         "--report",str(report)]),1)
            self.assertFalse(json.loads(report.read_text())["passed"])
    def test_original_sound_marking_is_required(self):
        with tempfile.TemporaryDirectory() as d:
            p=Path(d)/"exe.exe";p.write_bytes(b"x")
            code=Path(d)/"CODE.2";code.write_bytes(b"fixture")
            sound=Path(d)/"SOUND.CFG";sound.write_bytes(b"x"*59)
            report=Path(d)/"out.json"
            def fake_exec(exe,mode,source,sound_arg=None,timeout=120):
                bound=next(t for n,v,t in m.SOURCE_MODES if v==mode)
                return dict(mode=mode,returncode=0,reported_boundary=bound,
                            media="SYNTHETIC_FIXTURE",source_entry=True,
                            game_boot_claim=True,stderr="")
            with mock.patch.object(m,"CODE2_HASH",hashlib.sha256(b"fixture").hexdigest()),\
                 mock.patch.object(m,"SOUND_HASH",hashlib.sha256(b"x"*59).hexdigest()),\
                 mock.patch.object(m,"execute",side_effect=fake_exec):
                self.assertEqual(m.main(["--exe",str(p),"--code2",str(code),
                    "--sound-cfg",str(sound),"--report",str(report)]),1)
            result=json.loads(report.read_text())
            self.assertEqual(sum(not c["passed"] for c in result["cases"]),4)
if __name__=="__main__":
    unittest.main()
