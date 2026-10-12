"""Synthetic original-file-free normal runtime evidence comparison tests."""
from __future__ import annotations

import copy
import json
from pathlib import Path
import subprocess
import sys
import tempfile
import unittest

from original_world_oracle import OracleError
from test_original_world_oracle import levels_fixture
from verify_native_world_runtime import compare, expected_original_runtime

class RuntimeOriginalTests(unittest.TestCase):
    def setUp(self):
        self.original = bytes(levels_fixture()[0])
        source = expected_original_runtime(self.original)
        self.native = {
            "schema": "rloft-native-world-runtime-evidence/v1",
            "original_world_sha256": source["retained_source_payload_sha256"],
            "levels": 32,
            "active_level": 31,
            **source["counters"],
            "scene_projection_verified": False,
            "event_vm_executed": False,
            "original_dos_execution_verified": False,
            "gameplay_parity_verified": False,
        }

    def verify(self, candidate=None, original=None):
        return compare(
            self.original if original is None else original,
            self.native if candidate is None else candidate,
            require_known=False)

    def test_generated_fixture_has_all_zero_values_explicit(self):
        expected = expected_original_runtime(self.original)
        self.assertEqual(expected["counters"]["total_cells"], 128)
        self.assertEqual(expected["counters"]["legacy_authoring_markers"], 32)
        self.assertEqual(expected["counters"]["kind1_dynamic_records"], 0)
        self.assertEqual(expected["counters"]["ordinary_blocked_cells"], 0)
        self.assertEqual(len(expected["per_level"]), 32)

    def test_all_original_metrics_and_source_fingerprint_equal(self):
        result = self.verify()
        self.assertEqual(result["status"], "ORIGINAL_NORMAL_WORLD_RUNTIME_MATCH")
        self.assertGreaterEqual(result["counter_fields_checked"], 12)
        self.assertFalse(result["original_dos_gameplay_parity_verified"])
        self.assertFalse(result["native_first_person_projection_verified"])

    def test_original_strict_retail_identity_rejects_fixture(self):
        with self.assertRaisesRegex(OracleError, "pinned"):
            compare(self.original, self.native, require_known=True)

    def test_mutated_original_detects_compiled_report_source_mismatch(self):
        data = bytearray(self.original)
        # First packed cell at file offset 132 + section 0 start + 0x2a4.
        data[132 + 29 + 0x2a4] ^= 1
        result = self.verify(original=bytes(data))
        self.assertEqual(result["status"], "DIFFERENT")
        self.assertEqual(result["first_divergence"]["field"],
                         "original_world_sha256")

    def test_mutated_runtime_category_detected(self):
        altered = copy.deepcopy(self.native)
        altered["special_blocked_cells"] += 1
        result = self.verify(candidate=altered)
        self.assertEqual(result["first_divergence"]["field"],
                         "special_blocked_cells")

    def test_missing_zero_counter_detected(self):
        altered = copy.deepcopy(self.native)
        del altered["animating_dynamic_records"]
        result = self.verify(candidate=altered)
        self.assertEqual(result["first_divergence"]["field"],
                         "animating_dynamic_records")

    def test_reject_bogus_parity_claims(self):
        for name in ("scene_projection_verified", "event_vm_executed",
                     "original_dos_execution_verified",
                     "gameplay_parity_verified"):
            altered = copy.deepcopy(self.native)
            altered[name] = True
            with self.assertRaisesRegex(OracleError, "unsupported"):
                self.verify(candidate=altered)

    def test_misidentified_level_count(self):
        altered = copy.deepcopy(self.native)
        altered["levels"] = 31
        result = self.verify(candidate=altered)
        self.assertEqual(result["first_divergence"]["field"], "levels")

    def test_invalid_source_directory_rejected(self):
        data = bytearray(self.original)
        data[8:12] = (132).to_bytes(4, "little")
        with self.assertRaises(OracleError):
            self.verify(original=bytes(data))

    def test_cli_and_duplicate_key_gate(self):
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary)
            source = root / "LEVELS.DAT"
            native = root / "report.json"
            output = root / "diff.json"
            source.write_bytes(self.original)
            native.write_text(json.dumps(self.native))
            base = [sys.executable,
                    str(Path(__file__).with_name("verify_native_world_runtime.py")),
                    "--levels", str(source), "--native-report", str(native),
                    "--output", str(output), "--allow-other-version"]
            result = subprocess.run(base, capture_output=True, text=True)
            self.assertEqual(result.returncode, 0, result.stderr)
            self.assertEqual(json.loads(output.read_text())["status"],
                             "ORIGINAL_NORMAL_WORLD_RUNTIME_MATCH")
            self.assertNotIn(str(root), output.read_text())
            native.write_text('{"schema":"a","schema":"b"}')
            result = subprocess.run(base, capture_output=True, text=True)
            self.assertEqual(result.returncode, 2)
            self.assertIn("duplicate", result.stderr)

if __name__ == "__main__":
    unittest.main()
