"""Original-free CODE.2 35680/35730/35798 integer projection tests."""
from __future__ import annotations
import copy
import json
from pathlib import Path
import subprocess
import sys
import tempfile
import unittest

from verify_native_code2_projection import (
    SCHEMA,compare,original_code2_signatures,original_partial_product,
    reference,point,wall,
)

class Code2ProjectionDifferentialTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.code2=bytes(558689) # never contains original copyrighted code
        cls.baseline={
            "schema":SCHEMA,
            **original_code2_signatures(cls.code2,require_retail=False),
            **reference(),
        }
    def cmp(self,native=None,code=None):
        return compare(
            self.code2 if code is None else code,
            self.baseline if native is None else native,
            require_retail=False)

    def test_all_4096_diagnostic_projection_cases(self):
        result=self.cmp()
        self.assertEqual(result["status"],"MATCH")
        self.assertEqual(result["difference_count"],0)
        self.assertEqual(result["deterministic_case_count"],4096)
        self.assertEqual(result["independently_checked_case_bytes"],114688)
        self.assertEqual(self.baseline["accepted_projected_points"],1339)
        self.assertEqual(self.baseline["near_rejected_points"],2757)
        self.assertEqual(self.baseline["wall_near_clamped_cases"],2177)

    def test_original_16bit_partial_product_sign_and_shift(self):
        self.assertEqual(original_partial_product(0x20000,0x8000),0x10000)
        self.assertEqual(original_partial_product(0x20000,0x80008000),0xffff0000)
        self.assertEqual(original_partial_product(0xffff0000,0x8000),0xffff8000)
        self.assertEqual(original_partial_product(0xffff0000,0x80008000),0x8000)
        self.assertEqual(original_partial_product(0x7fffffff,0xffff),0x7fff7fff)

    def test_recovered_35730_unsigned_height_and_near_gate(self):
        r=dict(origin_x=0,origin_y=0,live_x=0x20000,live_y=0,
               dir_x=0x8000,dir_y=0,height_num=0x10000,
               horizontal_scale=0x100,horizontal_origin=100)
        self.assertEqual(wall(r),(0x10000,0x10000,256,256,False))
        r["live_x"]=0
        self.assertEqual(wall(r),(0,0x1680,22,65536//22,True))
        r["height_num"]=0xffffffff
        self.assertEqual(wall(r)[3],(0xffffffff//22)&0xffff)

    def test_recovered_35798_accepted_and_rejected_geometry(self):
        r=dict(origin_x=0,origin_y=0,live_x=0x20000,live_y=0,
               dir_x=0x8000,dir_y=0,height_num=0x10000,
               horizontal_scale=0x100,horizontal_origin=100)
        self.assertEqual(point(r,0x20000,0x8000,777),
                         (0xe000,0x4000,173,292,True))
        self.assertEqual(point(r,0xefff,0,777),
                         (0x57ff,0,777,0,False))
        self.assertEqual(point(r,0xf000,0,777),
                         (0x5800,0,100,744,True))

    def test_corrupt_native_projection_digest_detected(self):
        altered=copy.deepcopy(self.baseline)
        altered["original_algorithm_case_digest"]="f"*64
        result=self.cmp(native=altered)
        self.assertEqual(result["first_difference"]["field"],
                         "original_algorithm_case_digest")
        self.assertEqual(result["difference_count"],1)

    def test_corrupt_original_function_byte_detected(self):
        altered=bytearray(self.code2)
        altered[0x3AF24+0x35730]=0x90
        result=self.cmp(code=bytes(altered))
        self.assertEqual(result["status"],"DIFFERENT")
        self.assertTrue(any(a["field"]=="original_height_35730_sha256"
                            for a in result["differences"]))

    def test_native_first_case_original_screen_mismatch_localized(self):
        altered=copy.deepcopy(self.baseline)
        altered["first_samples"][0]["screen_word"]+=1
        result=self.cmp(native=altered)
        self.assertEqual(result["first_difference"]["field"],
                         "first_samples/0/screen_word")

    def test_missing_native_signed_rejection_count_detected(self):
        altered=copy.deepcopy(self.baseline)
        altered.pop("near_rejected_points")
        result=self.cmp(native=altered)
        self.assertEqual(result["first_difference"]["field"],
                         "near_rejected_points")

    def test_disallow_unverified_original_ray_or_frame_claims(self):
        for flag in ("original_live_camera_state_verified",
                     "original_ray_traversal_verified",
                     "original_full_frame_parity_verified"):
            altered=copy.deepcopy(self.baseline)
            altered[flag]=True
            with self.assertRaisesRegex(ValueError,"unsupported"):
                self.cmp(native=altered)

    def test_retail_SHA_gate_rejects_synthetic(self):
        with self.assertRaisesRegex(ValueError,"retail SHA256"):
            compare(self.code2,self.baseline,require_retail=True)

    def test_CLI_and_duplicate_key_rejection(self):
        with tempfile.TemporaryDirectory() as td:
            directory=Path(td)
            (directory/"CODE.2").write_bytes(self.code2)
            native=directory/"native.json"
            native.write_text(json.dumps(self.baseline))
            out=directory/"verification.json"
            command=[sys.executable,str(Path(__file__).with_name(
                "verify_native_code2_projection.py")),
                "--code2",str(directory/"CODE.2"),
                "--native-report",str(native),
                "--out",str(out),"--allow-other-version"]
            p=subprocess.run(command,text=True,capture_output=True)
            self.assertEqual(p.returncode,0,p.stderr)
            self.assertEqual(json.loads(out.read_text())["status"],"MATCH")
            self.assertNotIn(str(directory),out.read_text())
            native.write_text('{"schema":"a","schema":"b"}')
            p=subprocess.run(command,text=True,capture_output=True)
            self.assertEqual(p.returncode,2)
            self.assertIn("duplicate",p.stderr)
if __name__=="__main__":
    unittest.main()
