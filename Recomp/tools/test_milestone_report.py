"""Synthetic-only report validation: build evidence never self-certifies original game."""
import json
import tempfile
import unittest
from pathlib import Path
from unittest.mock import patch
import sys

sys.path.insert(0, str(Path(__file__).resolve().parent))
from milestone_report import report_for


class MilestoneReportTests(unittest.TestCase):
    def test_default_targets_next_unaccepted_gate(self):
        d = report_for("next", "a" * 40)
        self.assertEqual(d["milestone_id"], "R04")
        self.assertFalse(d["milestone_accepted_by_source_evidence"])
        self.assertFalse(d["player_usable_game_certified"])
        self.assertEqual(d["ctest"]["status"], "not_executed")
        self.assertFalse(d["original_media_bundled"])

    def test_accepted_gate_still_not_playable(self):
        d = report_for("R03", "B" * 40, build_result="success")
        self.assertTrue(d["milestone_accepted_by_source_evidence"])
        self.assertFalse(d["dos_runtime_parity_certified"])
        self.assertFalse(d["player_usable_game_certified"])
        self.assertEqual(d["source_sha"], "b" * 40)

    def test_counts_junit_without_conflating_acceptance(self):
        with tempfile.TemporaryDirectory() as d:
            p = Path(d) / "junit.xml"
            p.write_text('<testsuite tests="10" failures="2" errors="1" skipped="3"/>', encoding="utf-8")
            x = report_for("R04", "c" * 40, p, "failure")
            self.assertEqual(x["ctest"]["total"], 10)
            self.assertEqual(x["ctest"]["failures"], 2)
            self.assertEqual(x["ctest"]["errors"], 1)
            self.assertEqual(x["ctest"]["skipped"], 3)
            self.assertFalse(x["milestone_accepted_by_source_evidence"])

    def test_invalid_milestone_and_sha(self):
        with self.assertRaisesRegex(ValueError, "Unknown milestone"):
            report_for("R13", "a" * 40)
        with self.assertRaisesRegex(ValueError, "source commit"):
            report_for("R04", "latest")


if __name__ == "__main__":
    unittest.main()
