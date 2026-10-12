"""Original-free EH7 source/NativeEngine differential gates."""
from __future__ import annotations

import copy
import json
from pathlib import Path
import struct
import subprocess
import sys
import tempfile
import unittest

from verify_native_res3_type7 import compare,source_fingerprints


def eh7_fixture():
    body=bytearray(27)
    struct.pack_into("<Hhhhh",body,0,64,0,2,0,127)
    struct.pack_into("<3H",body,10,17,17,25)
    body[16]=165
    struct.pack_into("<HhH",body,17,2,-4,4)
    stream=bytearray(b"EH\x07\x00\x00")
    stream+=struct.pack("<H",len(body))
    for at in range(0,len(body),8):
        stream.append(255)
        stream+=body[at:at+8]
    return bytes(stream)


def fixtures(n=8):
    entry=eh7_fixture()
    res=bytearray(struct.pack("<I",n))
    base=4+4*n
    offsets=[base+idx*len(entry) for idx in range(n)]
    res+=struct.pack("<"+str(n)+"I",*offsets)
    res+=entry*n
    init=bytearray(0x24)
    assert n>=4
    struct.pack_into("<4H",init,0x06,0,2,4,6)
    return bytes(res),bytes(init)


def native_shape_report(res,init):
    reference=source_fingerprints(res,init)
    return {
        "schema":"rloft-native-res3-type7-shapes/v1",
        **reference,
        "sintab_x86_code_executed":False,
        "pixel_color_parity_verified":False,
        "original_scene_frame_parity_verified":False,
        "event_vm_executed":False,
    }


class Res3Type7DifferentialTests(unittest.TestCase):
    def setUp(self):
        self.res,self.init=fixtures()
        self.native=native_shape_report(self.res,self.init)

    def check(self,report=None,source=None,init=None):
        return compare(self.res if source is None else source,
                       self.init if init is None else init,
                       self.native if report is None else report,
                       require_retail=False)

    def test_synthetic_eight_eh7_shares_odd_list_and_is_valid(self):
        result=self.check()
        self.assertEqual(result["status"],"ALL_ORIGINAL_EH7_SHAPES_MATCH")
        self.assertEqual(result["original_entries_compared"],8)
        self.assertEqual(result["original_unique_lists"],16)
        self.assertEqual(result["original_descriptor_references"],16)
        self.assertEqual(result["original_decoded_bytes"],27*8)
        self.assertFalse(result["original_pixel_parity_verified"])

    def test_mutated_native_body_digest_is_located(self):
        report=copy.deepcopy(self.native)
        report["shapes"][4]["eh7_body_sha256"]="f"*64
        changed=self.check(report=report)
        self.assertEqual(changed["difference_count"],1)
        self.assertEqual(changed["first_divergence"]["field"],
                         "shapes/4/eh7_body_sha256")

    def test_mutated_native_entry_digest_is_located(self):
        report=copy.deepcopy(self.native)
        report["shapes"][0]["eh7_entry_sha256"]="0"*64
        changed=self.check(report=report)
        self.assertEqual(changed["difference_count"],1)
        self.assertEqual(changed["first_divergence"]["field"],
                         "shapes/0/eh7_entry_sha256")

    def test_modified_source_header_detected(self):
        altered=bytearray(self.res)
        off=struct.unpack_from("<I",altered,4)[0]
        altered[off+7+16]=42
        result=self.check(source=bytes(altered))
        self.assertEqual(result["status"],"DIFFERENT")
        fields={d["field"] for d in result["differences"]}
        self.assertIn("shapes/0/eh7_body_sha256",fields)
        self.assertIn("original_res3_sha256",fields)

    def test_malformed_original_pointer_fails(self):
        altered=bytearray(self.res)
        first=struct.unpack_from("<I",altered,4)[0]
        # Literal at body byte 10 (first column pointer) is located
        # after flags for header bytes 0..7 + one control for bytes 8..15.
        stream=first+7
        data_positions=[]
        for group in range(0,27,8):
            data_positions.extend(range(stream+1+group//8*9,
                                        stream+1+group//8*9+min(8,27-group)))
        altered[data_positions[10]]=0x01
        with self.assertRaises(ValueError):
            self.check(source=bytes(altered))

    def test_wrong_original_retail_sha_rejected(self):
        with self.assertRaisesRegex(ValueError,"SHA256"):
            compare(self.res,self.init,self.native,require_retail=True)

    def test_unsupported_original_parity_claim_rejected(self):
        for field in ("sintab_x86_code_executed","pixel_color_parity_verified",
                      "original_scene_frame_parity_verified","event_vm_executed"):
            report=copy.deepcopy(self.native);report[field]=True
            with self.assertRaisesRegex(ValueError,"unsupported"):
                self.check(report=report)

    def test_wrong_source_class_directory_rejected(self):
        altered=bytearray(self.init)
        struct.pack_into("<H",altered,0x0a,2)
        with self.assertRaisesRegex(ValueError,"bank starts"):
            self.check(init=bytes(altered))

    def test_missing_native_shape_fingerprint_detected(self):
        report=copy.deepcopy(self.native)
        report["shapes"][2].pop("eh7_body_sha256")
        self.assertEqual(self.check(report=report)["first_divergence"]["field"],
                         "shapes/2/eh7_body_sha256")

    def test_cli_and_json_duplicate_protection(self):
        with tempfile.TemporaryDirectory() as tmp:
            root=Path(tmp)
            (root/"RES3").write_bytes(self.res)
            (root/"INIT").write_bytes(self.init)
            (root/"native.json").write_text(json.dumps(self.native))
            command=[
                sys.executable,
                str(Path(__file__).with_name("verify_native_res3_type7.py")),
                "--res3",str(root/"RES3"),"--init",str(root/"INIT"),
                "--native-report",str(root/"native.json"),
                "--report",str(root/"diff.json"),"--allow-other-version"]
            p=subprocess.run(command,capture_output=True,text=True)
            self.assertEqual(p.returncode,0,p.stderr)
            self.assertEqual(json.loads((root/"diff.json").read_text())["status"],
                             "ALL_ORIGINAL_EH7_SHAPES_MATCH")
            self.assertNotIn(str(root),(root/"diff.json").read_text())
            (root/"native.json").write_text('{"schema":"a","schema":"b"}')
            p=subprocess.run(command,capture_output=True,text=True)
            self.assertEqual(p.returncode,2)
            self.assertIn("duplicate",p.stderr)


if __name__=="__main__":
    unittest.main()
