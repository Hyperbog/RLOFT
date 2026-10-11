"""Synthetic trace-certificate regressions: never require licensed game data."""
from __future__ import annotations

import copy
import hashlib
import json
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path

from certify_trace import SCHEMA, certify
from workflow import EvidenceError


class CaptureCertificateTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        root = Path(self.temp.name)
        self.ref = root / "original.jsonl"
        self.host = root / "native.jsonl"
        self.rows = [
            {"step": 0, "pc": "object1:353a0", "registers": {"EAX": 1},
             "memory_writes": [], "hardware_events": []},
            {"step": 1, "pc": "object1:35418", "registers": {"EAX": 2},
             "memory_writes": [{"addr": "DGROUP:1000", "value": 7}],
             "hardware_events": []},
        ]
        self.write(self.ref, self.rows)
        self.write(self.host, self.rows)
        self.rm = self.manifest(self.ref, "reference", "original_dos", "DOS-session-A")
        self.cm = self.manifest(self.host, "candidate", "native_windows", "WIN-session-B")

    @staticmethod
    def write(path, rows):
        path.write_text("".join(json.dumps(r, sort_keys=True) + "\n" for r in rows),
                        encoding="utf-8")

    @staticmethod
    def manifest(path, role, source_kind, capture_id):
        return {"schema": SCHEMA, "role": role,
                "scenario_id": "startup-original-01",
                "capture_id": capture_id, "source_kind": source_kind,
                "trace_sha256": hashlib.sha256(path.read_bytes()).hexdigest(),
                "binary_sha256": "a" * 64 if role == "reference" else "b" * 64,
                "adapter_sha256": "c" * 64 if role == "reference" else "d" * 64}

    def test_equal_but_never_claim_original_or_native_parity(self):
        result = certify(self.ref, self.host, self.rm, self.cm)
        self.assertEqual(result["status"], "EQUAL_UNATTESTED")
        self.assertTrue(result["trace_equal"])
        self.assertEqual(result["comparison"]["matched_steps"], 2)
        self.assertFalse(result["capture_origin_attested"])
        self.assertFalse(result["dos_runtime_parity_verified"])
        self.assertFalse(result["original_binary_authenticated"])
        self.assertFalse(result["native_gameplay_parity_verified"])

    def test_first_real_register_divergence(self):
        mutated = copy.deepcopy(self.rows)
        mutated[1]["registers"]["EAX"] = 99
        self.write(self.host, mutated)
        self.cm["trace_sha256"] = hashlib.sha256(self.host.read_bytes()).hexdigest()
        result = certify(self.ref, self.host, self.rm, self.cm)
        self.assertEqual(result["status"], "DIFFERENT")
        self.assertEqual(result["comparison"]["field"], "registers")
        self.assertEqual(result["comparison"]["first_divergence_step"], 1)

    def test_missing_field_is_serializable(self):
        mutated = copy.deepcopy(self.rows)
        del mutated[0]["registers"]
        self.write(self.host, mutated)
        self.cm["trace_sha256"] = hashlib.sha256(self.host.read_bytes()).hexdigest()
        result = certify(self.ref, self.host, self.rm, self.cm)
        self.assertEqual(result["status"], "DIFFERENT")
        json.dumps(result)
        self.assertEqual(result["comparison"]["candidate"], "<field absent>")

    def test_manifest_hash_tampering_rejected(self):
        self.cm["trace_sha256"] = "0" * 64
        with self.assertRaisesRegex(EvidenceError, "differs"):
            certify(self.ref, self.host, self.rm, self.cm)

    def test_mismatched_scenarios_rejected(self):
        self.cm["scenario_id"] = "unrelated-scenario"
        with self.assertRaisesRegex(EvidenceError, "scenario"):
            certify(self.ref, self.host, self.rm, self.cm)

    def test_nonindependent_session_and_source_rejected(self):
        self.cm["capture_id"] = self.rm["capture_id"]
        with self.assertRaisesRegex(EvidenceError, "same declared capture"):
            certify(self.ref, self.host, self.rm, self.cm)
        self.cm["capture_id"] = "distinct"
        self.cm["source_kind"] = self.rm["source_kind"]
        with self.assertRaisesRegex(EvidenceError, "source kinds"):
            certify(self.ref, self.host, self.rm, self.cm)

    def test_same_trace_file_rejected(self):
        with self.assertRaisesRegex(EvidenceError, "separate trace"):
            certify(self.ref, self.ref, self.rm, self.cm)

    def test_duplicate_missing_or_unordered_steps_rejected(self):
        for steps in ([0, 0], [0, 2], [1, 2], [0, -1]):
            rows = copy.deepcopy(self.rows)
            for i, step in enumerate(steps):
                rows[i]["step"] = step
            self.write(self.host, rows)
            self.cm["trace_sha256"] = hashlib.sha256(self.host.read_bytes()).hexdigest()
            with self.assertRaisesRegex(EvidenceError, "step"):
                certify(self.ref, self.host, self.rm, self.cm)

    def test_missing_event_identity_rejected(self):
        rows = copy.deepcopy(self.rows)
        rows[1].pop("pc")
        self.write(self.host, rows)
        self.cm["trace_sha256"] = hashlib.sha256(self.host.read_bytes()).hexdigest()
        with self.assertRaisesRegex(EvidenceError, "event/PC identity"):
            certify(self.ref, self.host, self.rm, self.cm)

    def test_cli_writes_report_without_false_parity(self):
        base = Path(self.temp.name)
        rm = base / "reference.manifest.json"
        cm = base / "candidate.manifest.json"
        out = base / "certificate.json"
        rm.write_text(json.dumps(self.rm), encoding="utf-8")
        cm.write_text(json.dumps(self.cm), encoding="utf-8")
        cmd = [sys.executable, str(Path(__file__).with_name("certify_trace.py")),
               "--reference", str(self.ref), "--candidate", str(self.host),
               "--reference-manifest", str(rm),
               "--candidate-manifest", str(cm), "--report", str(out)]
        process = subprocess.run(cmd, capture_output=True, text=True)
        self.assertEqual(process.returncode, 0, process.stderr)
        actual = json.loads(out.read_text(encoding="utf-8"))
        self.assertEqual(actual["status"], "EQUAL_UNATTESTED")
        self.assertFalse(actual["dos_runtime_parity_verified"])


if __name__ == "__main__":
    unittest.main()
