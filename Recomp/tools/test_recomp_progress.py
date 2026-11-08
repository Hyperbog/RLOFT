#!/usr/bin/env python3
"""Offline contracts for canonical Recomp acceptance-gate percentage."""
from copy import deepcopy
import json
from pathlib import Path
import subprocess
import sys
import unittest
import recomp_progress as progress

class RecompProgressContracts(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.manifest=json.loads(progress.ACCEPTANCE_FILE.read_text(encoding="utf-8"))
    def test_canonical_accepted_count_and_percentage(self):
        report=progress.calculate(self.manifest)
        self.assertEqual(report["accepted"],3)
        self.assertEqual(report["total"],12)
        self.assertEqual(report["percent"],25.0)
        self.assertEqual(report["accepted_ids"],["R01","R02","R03"])
        self.assertEqual(len(report["pending"]),9)
        self.assertFalse(report["game_booted"])
        self.assertFalse(report["original_dos_parity"])
    def test_report_cli_is_reproducible(self):
        p=subprocess.run([sys.executable,str(Path(progress.__file__)),"--json"],
                         capture_output=True,text=True,check=True)
        self.assertEqual(json.loads(p.stdout),progress.calculate(self.manifest))
    def test_cannot_silently_change_denominator(self):
        m=deepcopy(self.manifest);m["gates"].pop()
        with self.assertRaisesRegex(ValueError,"denominator"):
            progress.calculate(m)
    def test_cannot_inflate_gates_with_duplicate_ids(self):
        m=deepcopy(self.manifest);m["gates"][8]["id"]="R01"
        with self.assertRaisesRegex(ValueError,"identity"):
            progress.calculate(m)
    def test_accepted_gate_must_have_canonical_evidence(self):
        m=deepcopy(self.manifest);m["gates"][3]["accepted"]=True
        with self.assertRaisesRegex(ValueError,"evidence"):
            progress.calculate(m)
    def test_reject_false_truthy_text(self):
        m=deepcopy(self.manifest);m["gates"][4]["accepted"]="yes"
        with self.assertRaisesRegex(ValueError,"true or false"):
            progress.calculate(m)
    def test_complete_game_is_not_reported_from_three_gates(self):
        report=progress.calculate(self.manifest)
        self.assertNotIn("100",str(report["percent"]))
        self.assertEqual(report["pending"][-1],"R12")
    def test_every_pending_scope_has_a_reason(self):
        for g in self.manifest["gates"]:
            self.assertTrue(g["scope"])
            self.assertTrue(g["limitation"])
            if not g["accepted"]:
                self.assertFalse(g["evidence"])
if __name__=="__main__":
    unittest.main()
