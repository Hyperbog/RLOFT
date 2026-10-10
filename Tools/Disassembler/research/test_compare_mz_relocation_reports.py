"""Synthetic-only comparison-contract checks; SOURCE ONLY until approved.

These reports are authored examples, not files emitted by DOSDis or a
second decoder. The fixtures contain no game bytes or runtime trace.
"""
import copy
import json
import tempfile
import unittest
from pathlib import Path

from compare_mz_relocation_reports import InvalidReport, compare, read_report


SOURCE = "a" * 64
RELOCATED = "b" * 64


def reports():
    independent = {
        "evidence": "read-only-original-source-preflight",
        "g2_accuracy_status": "not-certified",
        "independent_decoder_or_execution": False,
        "originals": [{
            "original_sha256": SOURCE,
            "geometry": {
                "format": "outer-DOS-MZ-stored-image-only",
                "file_backed_load_bytes": 480,
                "relocation_count": 1,
                "relocation_overlaps": 0,
            },
        }],
        "assumed_mz_relocation_simulation": {
            "status": "simulated-relocations;assumed-segment;not-runtime-proof",
            "assumed_load_segment": 0x2000,
            "original_file_backed_image_bytes": 480,
            "relocation_write_count": 1,
            "overlapping_relocation_writes": False,
            "simulated_file_backed_image_sha256": RELOCATED,
        },
    }
    dosdis = {
        "SourceSha256": SOURCE,
        "OriginalFileBackedBytes": 480,
        "AssumedLoadSegment": 0x2000,
        "RelocationWriteCount": 1,
        "OverlappingRelocationWrites": False,
        "SimulatedFileBackedImageSha256": RELOCATED,
        "AnalysisStatus": "simulated-MZ-relocations;runtime-base-assumed;not-verified",
        "Writes": [],  # deliberately not copied to comparison output
    }
    return independent, dosdis


class MzMetadataComparisonSmoke(unittest.TestCase):
    def test_agreement_is_not_marked_certified(self):
        independent, dosdis = reports()
        result = compare(independent, dosdis, SOURCE)
        self.assertEqual(result["status"], "metadata-agreement-not-certification")
        self.assertEqual(result["matching_fields"], 5)
        self.assertEqual(result["compared_fields"], 5)
        self.assertEqual(result["discrepancy_fields"], [])
        self.assertEqual(result["g2_accuracy_gate"], "pending")
        self.assertNotIn("Writes", result)
        self.assertNotIn("RelocationModel", result)

    def test_every_discrepancy_is_named_not_discarded(self):
        cases = [
            ("AssumedLoadSegment", 0x3000, "assumed_load_segment"),
            ("OriginalFileBackedBytes", 478, "original_file_backed_image_bytes"),
            ("RelocationWriteCount", 2, "relocation_write_count"),
            ("OverlappingRelocationWrites", True, "overlapping_relocation_writes"),
            ("SimulatedFileBackedImageSha256", "c" * 64,
             "simulated_file_backed_image_sha256"),
        ]
        for key, value, name in cases:
            with self.subTest(key=key):
                independent, dosdis = reports()
                dosdis[key] = value
                result = compare(independent, dosdis, SOURCE)
                self.assertEqual(result["status"], "metadata-disagreement")
                self.assertEqual(result["discrepancy_fields"], [name])
                self.assertEqual(result["matching_fields"], 4)

    def test_source_identity_must_be_exactly_pinned(self):
        independent, dosdis = reports()
        for invalid in ("not-a-digest", "0" * 64):
            with self.assertRaises(InvalidReport):
                compare(independent, dosdis, invalid)
        independent["originals"][0]["original_sha256"] = "d" * 64
        with self.assertRaises(InvalidReport):
            compare(independent, dosdis, SOURCE)
        independent, dosdis = reports()
        dosdis["SourceSha256"] = "d" * 64
        with self.assertRaises(InvalidReport):
            compare(independent, dosdis, SOURCE)

    def test_missing_or_ambiguous_source_evidence_fails_closed(self):
        independent, dosdis = reports()
        independent["originals"].append(copy.deepcopy(independent["originals"][0]))
        with self.assertRaises(InvalidReport):
            compare(independent, dosdis, SOURCE)
        independent, dosdis = reports()
        independent["originals"][0]["geometry"]["relocation_count"] = 2
        with self.assertRaises(InvalidReport):
            compare(independent, dosdis, SOURCE)
        independent, dosdis = reports()
        independent["independent_decoder_or_execution"] = True
        with self.assertRaises(InvalidReport):
            compare(independent, dosdis, SOURCE)
        independent, dosdis = reports()
        independent["originals"][0]["geometry"]["format"] = "explicit-headerless-COM"
        with self.assertRaises(InvalidReport):
            compare(independent, dosdis, SOURCE)

    def test_invalid_types_and_status_do_not_silently_compare(self):
        independent, dosdis = reports()
        for name, value in [
            ("AssumedLoadSegment", True),
            ("RelocationWriteCount", -1),
            ("OverlappingRelocationWrites", 0),
            ("SimulatedFileBackedImageSha256", ""),
            ("OriginalFileBackedBytes", None),
            ("AnalysisStatus", "source-analyzed;runtime-proof"),
        ]:
            with self.subTest(name=name):
                changed = dict(dosdis)
                changed[name] = value
                with self.assertRaises(InvalidReport):
                    compare(independent, changed, SOURCE)

    def test_json_import_rejects_duplicate_or_nonfinite_fields(self):
        with tempfile.TemporaryDirectory(prefix="dosdis-mz-meta-synthetic-") as d:
            p = Path(d) / "report.json"
            p.write_text('{"SourceSha256":"a","sourcesha256":"b"}', encoding="utf-8")
            with self.assertRaises(InvalidReport):
                read_report(p)
            p.write_text('{"valid":NaN}', encoding="utf-8")
            with self.assertRaises(InvalidReport):
                read_report(p)
            p.write_text(json.dumps({"valid": True}), encoding="utf-8")
            self.assertEqual(read_report(p), {"valid": True})


if __name__ == "__main__":
    unittest.main()
