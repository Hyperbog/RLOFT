"""Fixture-based contracts against known NativeEngine/DOSDis/DemoV1 exports."""
from __future__ import annotations

import copy
import hashlib
import json
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path

from adapters import (adapt_bank, adapt_world, adapt_dosdis,
                      adapt_demo_events, compare)
from workflow import EvidenceError


def native_bank():
    return {"schema": "rloft-native-bank-audit/v1", "records": 2,
            "validated": 2, "failed": 0, "absent_banks": 0,
            "recognized_eh": 2, "issues": [], "banks": [
                {"name": "RES0", "bytes": 200, "records": 2,
                 "validated": 2, "failed": 0, "empty": 0, "non_eh": 0,
                 "raw_wordcode": 0, "image_eh1": 1, "inert_legacy_eh1": 0,
                 "font_eh2": 1, "xmidi_eh3": 0, "voc_eh4": 0,
                 "texture_eh6": 0, "shape_eh7": 0, "shipped_anomalies": 0}]}


def native_world():
    return {"schema": "rloft-native-world-system/v1", "level_count": 2,
            "level_cells": 8, "controls": 3, "event_slots": 1,
            "event_commands": 2, "level_archive_bytes": 100,
            "sintab_bytes": 200, "sintab_scales": 127,
            "sintab_scale_bytes": 120, "levels": [
                {"id": 0, "w": 2, "h": 2, "cells": 4, "controls": 1,
                 "a": 1, "b": 0, "c": 0, "events": 1, "commands": 2},
                {"id": 1, "w": 2, "h": 2, "cells": 4, "controls": 2,
                 "a": 0, "b": 0, "c": 0, "events": 0, "commands": 0}]}


def dosdis_mz():
    return {"format": "dosdis-mz-16-candidate-analysis",
            "schemaVersion": 1,
            "analysisStatus": "candidate-unverified;single-original-MZ-entry-segment;not-executed",
            "source": {"sha256": "a" * 64, "sourceBinaryIncluded": False,
                       "sourcePathIncluded": False, "instructionBytesIncluded": False},
            "graph": {"instructionCount": 1, "edgeCount": 0,
                      "diagnosticCount": 0, "UnknownFileBackedBytes": 10,
                      "CandidateInstructionBytes": 3, "Truncated": False},
            "instructions": [{"Address": {"Segment": 0, "Offset": 0}}],
            "edges": [], "diagnostics": []}


class AdapterTests(unittest.TestCase):
    def test_bank_native_schema_and_integrity(self):
        result = adapt_bank(native_bank(), "b" * 64)
        self.assertEqual(result["observations"]["bank/RES0/image_eh1"], 1)
        self.assertFalse(result["runtime_parity_verified"])
        self.assertFalse(result["original_identity_attested"])

    def test_bank_detects_counts_and_duplicate(self):
        case = native_bank(); case["records"] = 3
        with self.assertRaisesRegex(EvidenceError, "per-bank"):
            adapt_bank(case, "0" * 64)
        case = native_bank(); case["banks"].append(copy.deepcopy(case["banks"][0]))
        with self.assertRaisesRegex(EvidenceError, "duplicate"):
            adapt_bank(case, "0" * 64)

    def test_bank_reports_failure_instead_of_certifying(self):
        case = native_bank()
        case["banks"][0]["validated"] = 1
        case["banks"][0]["failed"] = 1
        case["validated"] = 1
        case["failed"] = 1
        result = adapt_bank(case, "a" * 64)
        self.assertEqual(result["status"], "STRUCTURAL_REPORT_FAIL")

    def test_world_full_level_conservation(self):
        r = adapt_world(native_world(), "f" * 64)
        self.assertEqual(r["observations"]["level/01/controls"], 2)
        self.assertEqual(r["observations"]["total/event_commands"], 2)

    def test_world_dimension_and_missing_record_rejected(self):
        j = native_world(); j["levels"][0]["cells"] = 3
        with self.assertRaisesRegex(EvidenceError, "dimensions"):
            adapt_world(j, "f" * 64)
        j = native_world(); j["levels"].pop()
        with self.assertRaisesRegex(EvidenceError, "enumeration"):
            adapt_world(j, "f" * 64)

    def test_dosdis_mz_never_execution_proof(self):
        r = adapt_dosdis(dosdis_mz(), "b" * 64)
        self.assertEqual(r["status"], "CANDIDATE_UNVERIFIED")
        self.assertEqual(r["observations"]["candidate/unknown_file_backed_bytes"], 10)
        self.assertFalse(r["original_code_verified"])

    def test_dosdis_le_never_execution_proof(self):
        j = dosdis_mz()
        j["format"] = "dosdis-analysis-report"
        j["analysisStatus"] = "candidate-unverified"
        j["counts"] = {"instructions": 1, "edges": 0, "diagnostics": 0,
                       "functions": 0, "references": 0, "jumpTables": 0,
                       "truncated": True}
        result = adapt_dosdis(j, "d" * 64)
        self.assertTrue(result["observations"]["candidate/truncated"])

    def test_dosdis_rejects_mixed_source_and_count_mismatch(self):
        j = dosdis_mz()
        j["source"]["sourceBinaryIncluded"] = True
        with self.assertRaisesRegex(EvidenceError, "private source"):
            adapt_dosdis(j, "0" * 64)
        j = dosdis_mz()
        j["graph"]["instructionCount"] = 2
        with self.assertRaisesRegex(EvidenceError, "cardinality"):
            adapt_dosdis(j, "0" * 64)

    def test_metadata_compare_first_difference_and_missing(self):
        a = adapt_bank(native_bank(), "a" * 64)
        b = copy.deepcopy(a)
        self.assertEqual(compare(a, b)["status"], "EQUAL_METADATA_UNATTESTED")
        b["observations"]["bank/RES0/bytes"] = 201
        diff = compare(a, b)
        self.assertEqual(diff["difference_count"], 1)
        self.assertEqual(diff["first_differences"][0]["key"], "bank/RES0/bytes")
        del b["observations"]["bank/RES0/bytes"]
        self.assertFalse(compare(a, b)["first_differences"][0]["candidate_present"])

    def test_metadata_cross_domain_fail_closed(self):
        a = adapt_bank(native_bank(), "a" * 64)
        b = adapt_world(native_world(), "a" * 64)
        with self.assertRaisesRegex(EvidenceError, "domains"):
            compare(a, b)

    def test_demo_native_event_normalization(self):
        with tempfile.TemporaryDirectory() as root:
            p = Path(root) / "events.jsonl"
            p.write_text('{"seq":1,"event":"session.start","a":0,"b":0}\n'
                         '{"seq":2,"event":"frame.capture","a":1,"b":22}\n')
            manifest = {"format": "rloft-level22-diagnostic-v1",
                        "provenance": "native-preview-not-original-DOS",
                        "source_revision": "demo-source-sha"}
            frames, summary = adapt_demo_events(p, manifest)
            self.assertEqual(frames[1]["step"], 1)
            self.assertEqual(frames[1]["b"], 22)
            self.assertFalse(summary["original_dos_capture"])
            self.assertIsNone(summary["truncated_by_original_logger"])

    def test_demo_gap_and_false_original_claim_rejected(self):
        with tempfile.TemporaryDirectory() as root:
            p = Path(root) / "events.jsonl"
            p.write_text('{"seq":2,"event":"frame.capture","a":1,"b":22}\n')
            manifest = {"format": "rloft-level22-diagnostic-v1",
                        "provenance": "native-preview-not-original-DOS",
                        "source_revision": "SHA"}
            with self.assertRaisesRegex(EvidenceError, "sequence"):
                adapt_demo_events(p, manifest)
            p.write_text('{"seq":1,"event":"session.start","a":0,"b":0}\n')
            manifest["provenance"] = "original-DOS"
            with self.assertRaisesRegex(EvidenceError, "manifest"):
                adapt_demo_events(p, manifest)

    def test_adapt_and_compare_cli_real_payload_shape(self):
        with tempfile.TemporaryDirectory() as root:
            base = Path(root)
            src = base / "native.json"
            out = base / "normalized.json"
            diff = base / "diff.json"
            src.write_text(json.dumps(native_bank()), encoding="utf-8")
            tool = Path(__file__).with_name("adapters.py")
            cmd = [sys.executable, str(tool), "adapt", "native-bank",
                   str(src), "--output", str(out)]
            proc = subprocess.run(cmd, capture_output=True, text=True)
            self.assertEqual(proc.returncode, 0, proc.stderr)
            cmd = [sys.executable, str(tool), "compare", str(out), str(out),
                   "--output", str(diff)]
            proc = subprocess.run(cmd, capture_output=True, text=True)
            self.assertEqual(proc.returncode, 0, proc.stderr)
            self.assertEqual(json.loads(diff.read_text())["status"],
                             "EQUAL_METADATA_UNATTESTED")


if __name__ == "__main__":
    unittest.main()
