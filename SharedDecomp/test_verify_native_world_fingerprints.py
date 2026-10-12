"""Synthetic original-to-native payload section SHA compare regressions."""
from __future__ import annotations

import copy
import hashlib
import json
from pathlib import Path
import subprocess
import sys
import tempfile
import unittest

from original_world_oracle import OracleError, audit
from test_original_world_oracle import levels_fixture, sintab_fixture
from verify_native_world_fingerprints import (compare_payload, expected_hashes,
                                               read_report)


def source():
    return bytes(levels_fixture()[0]), bytes(sintab_fixture())


def synthetic_native(original_levels, original_sintab):
    original=audit(original_levels,original_sintab)
    records=[]
    for id,item in enumerate(original["levels"]):
        record={"id":id, "cells":item["cells"],
                "controls":item["controls"], "commands":item["commands"]}
        record.update(expected_hashes(original_levels,id,item))
        records.append(record)
    return {
        "schema":"rloft-native-world-payload-fingerprints/v1",
        "input_levels_sha256":hashlib.sha256(original_levels).hexdigest(),
        "input_levels_bytes":len(original_levels),
        "decoded_section_roundtrip_equal":True,
        "original_file_identity_independently_attested":False,
        "original_dos_execution_observed":False,
        "native_gameplay_parity_verified":False,
        "total_cells":original["summary"]["level_cells"],
        "total_controls":original["summary"]["controls"],
        "total_commands":original["summary"]["event_commands"],
        "levels":records,
    }


class NativePayloadFingerprintTests(unittest.TestCase):
    def setUp(self):
        self.levels,self.sintab=source()
        # Synthetic reference constructed independently; not a real C++ run.
        self.native=synthetic_native(self.levels,self.sintab)

    def check(self,report=None,levels=None,requires_known=False):
        return compare_payload(levels if levels is not None else self.levels,
                               self.sintab,
                               report if report is not None else self.native,
                               require_known=requires_known)

    def test_all_288_field_digests_with_synthetic_native_shape(self):
        result=self.check()
        self.assertEqual(result["status"],"ALL_32_ORIGINAL_SECTIONS_MATCH")
        self.assertEqual(result["section_hashes_checked"],288)
        self.assertFalse(result["compiled_native_binary_verified"])
        self.assertFalse(result["native_gameplay_parity_verified"])
        self.assertFalse(result["original_identity_pinned"])

    def test_known_original_gate_rejects_fixture(self):
        with self.assertRaisesRegex(OracleError,"SHA256"):
            self.check(requires_known=True)

    def test_one_cell_byte_mutation_finds_exact_section(self):
        altered=bytearray(self.levels)
        start=int.from_bytes(altered[4:8],"little")
        at=start+29+0x2a4
        altered[at]^=0x40
        result=self.check(levels=bytes(altered))
        self.assertEqual(result["status"],"DIFFERENT")
        self.assertEqual(result["first_divergence"]["field"],"input_levels_sha256")
        self.assertTrue(any(x["field"]=="levels/0/cells_sha256"
                            for x in result["differences"]))

    def test_source_header_mutation_is_detected(self):
        altered=bytearray(self.levels)
        first=int.from_bytes(altered[4:8],"little")
        # Header +0x290 is profile metadata; changing a byte here does
        # not change the directory/controls/events structural boundaries.
        altered[first+29+0x290]^=1
        result=self.check(levels=bytes(altered))
        self.assertEqual(result["status"],"DIFFERENT")
        self.assertTrue(any(change["field"]=="levels/0/header_sha256"
                            for change in result["differences"]))

    def test_missing_native_header_digest_is_rejected(self):
        candidate=copy.deepcopy(self.native)
        candidate["levels"][22].pop("header_sha256")
        with self.assertRaisesRegex(OracleError,"header_sha256"):
            self.check(report=candidate)

    def test_native_header_digest_mutation_finds_exact_level(self):
        candidate=copy.deepcopy(self.native)
        candidate["levels"][22]["header_sha256"]="f"*64
        result=self.check(report=candidate)
        self.assertEqual(result["first_divergence"]["field"],
                         "levels/22/header_sha256")

    def test_one_native_control_hash_mutation_finds_level(self):
        candidate=copy.deepcopy(self.native)
        candidate["levels"][22]["controls_sha256"]="0"*64
        result=self.check(report=candidate)
        self.assertEqual(result["difference_count"],1)
        self.assertEqual(result["first_divergence"]["field"],
                         "levels/22/controls_sha256")

    def test_native_events_automap_annotations_families_all_compared(self):
        for field in ("events_sha256","automap_sha256","annotations_sha256",
                      "family_a_sha256","family_b_sha256","family_c_sha256"):
            candidate=copy.deepcopy(self.native)
            candidate["levels"][5][field]="f"*64
            result=self.check(report=candidate)
            self.assertEqual(result["first_divergence"]["field"],
                             "levels/5/"+field)

    def test_original_source_sha_spoof_cannot_hide_change(self):
        candidate=copy.deepcopy(self.native)
        candidate["input_levels_sha256"]="a"*64
        result=self.check(report=candidate)
        self.assertFalse(result["native_report_source_sha256_matches_original"])
        self.assertEqual(result["first_divergence"]["field"],"input_levels_sha256")

    def test_missing_level_rejected(self):
        candidate=copy.deepcopy(self.native)
        candidate["levels"].pop()
        with self.assertRaisesRegex(OracleError,"exactly 32"):
            self.check(report=candidate)

    def test_bad_sha_rejected(self):
        candidate=copy.deepcopy(self.native)
        candidate["levels"][1]["cells_sha256"]="bad"
        with self.assertRaisesRegex(OracleError,"missing SHA256"):
            self.check(report=candidate)

    def test_false_runtime_or_identity_attestation_rejected(self):
        for field in ("original_file_identity_independently_attested",
                      "original_dos_execution_observed",
                      "native_gameplay_parity_verified"):
            candidate=copy.deepcopy(self.native)
            candidate[field]=True
            with self.assertRaisesRegex(OracleError,"unsupported"):
                self.check(report=candidate)

    def test_decoded_roundtrip_false_rejected(self):
        candidate=copy.deepcopy(self.native)
        candidate["decoded_section_roundtrip_equal"]=False
        with self.assertRaisesRegex(OracleError,"roundtrip"):
            self.check(report=candidate)

    def test_duplicate_json_fields_rejected(self):
        with tempfile.TemporaryDirectory() as temp:
            p=Path(temp)/"native.json"
            p.write_text('{"schema":"a","schema":"b"}')
            with self.assertRaisesRegex(OracleError,"duplicate JSON"):
                read_report(p)

    def test_cli_synthetic_only(self):
        with tempfile.TemporaryDirectory() as temp:
            p=Path(temp)
            orig=p/"LEVELS.DAT";sin=p/"SINTAB"
            cand=p/"native.json";out=p/"report.json"
            orig.write_bytes(self.levels);sin.write_bytes(self.sintab)
            cand.write_text(json.dumps(self.native))
            cmd=[sys.executable,
                 str(Path(__file__).with_name("verify_native_world_fingerprints.py")),
                 "--levels",str(orig),"--sintab",str(sin),
                 "--native-report",str(cand),"--out",str(out),
                 "--allow-other-version"]
            result=subprocess.run(cmd,capture_output=True,text=True)
            self.assertEqual(result.returncode,0,result.stderr)
            report=json.loads(out.read_text())
            self.assertEqual(report["status"],"ALL_32_ORIGINAL_SECTIONS_MATCH")
            self.assertNotIn(str(p),out.read_text())
            self.assertFalse(report["native_gameplay_parity_verified"])


if __name__=="__main__":
    unittest.main()
