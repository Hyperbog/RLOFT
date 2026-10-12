"""Original-free CODE.2 338C8 source-column and VGA-plane differential gates."""
from __future__ import annotations

import copy
import json
from pathlib import Path
import struct
import subprocess
import sys
import tempfile
import unittest

from verify_native_type6_columns import compare,independent_source

def synthetic_eh6(width):
    pixels=bytes((x*3+y*7)&255 for x in range(width) for y in range(128))
    header=b"EH\x06\x00\x00"+struct.pack("<H",len(pixels))
    codes=bytearray()
    for at in range(0,len(pixels),8):
        codes.append(255)
        codes.extend(pixels[at:at+8])
    return header+codes

def synth():
    entries=[synthetic_eh6(64),synthetic_eh6(32)]
    at=4+4*len(entries)
    points=[]
    for e in entries:
        points.append(at)
        at+=len(e)
    res=struct.pack("<I2I",2,*points)+b"".join(entries)
    init=bytearray(0x24)
    struct.pack_into("<H",init,14,1)
    colors=bytes([2])+bytes((i%64) for i in range(1536))
    return bytes(res),bytes(init),colors

class OriginalType6ColumnsTests(unittest.TestCase):
    def setUp(self):
        self.res,self.init,self.colors=synth()
        self.native=independent_source(self.res,self.init,self.colors)
    def check(self,report=None,res=None,init=None,colors=None):
        return compare(self.res if res is None else res,
                       self.init if init is None else init,
                       self.colors if colors is None else colors,
                       self.native if report is None else report,
                       retail=False)
    def test_full_32_and_64_128_source_fixture(self):
        r=self.check()
        self.assertEqual(r["status"],"MATCH")
        self.assertEqual(r["original_res4_entries"],2)
        self.assertEqual(r["per_entry_typed_fields_compared"],12)
        self.assertEqual(r["original_texture_texel_reads"],960)
        self.assertFalse(r["original_frame_pixel_parity_verified"])
        self.assertNotEqual(self.native["textures"][0]["diagnostic_four_plane_sha256"],
                            self.native["textures"][1]["diagnostic_four_plane_sha256"])
    def test_mutated_native_sha_locates_entry_and_field(self):
        report=copy.deepcopy(self.native)
        report["textures"][1]["diagnostic_four_plane_sha256"]="0"*64
        r=self.check(report=report)
        self.assertEqual(r["difference_count"],1)
        self.assertEqual(r["first_difference"]["field"],
                         "textures/1/diagnostic_four_plane_sha256")
    def test_original_texel_mutation_changes_native_output(self):
        bad=bytearray(self.res)
        original_start=struct.unpack_from("<I",bad,4)[0]
        # First literal in the first resource at source column 0, row 0.
        bad[original_start+8]^=1
        r=self.check(res=bytes(bad))
        self.assertEqual(r["status"],"DIFFERENT")
        self.assertTrue(any(d["field"]=="textures/0/diagnostic_four_plane_sha256"
                            for d in r["differences"]))
    def test_mutated_native_decoded_texture_digest_detected(self):
        bad=copy.deepcopy(self.native)
        bad["textures"][0]["decoded_row_major_sha256"]="f"*64
        r=self.check(report=bad)
        self.assertEqual(r["first_difference"]["field"],
                         "textures/0/decoded_row_major_sha256")
    def test_original_init_narrow_class_boundary_rejected(self):
        bad=bytearray(self.init)
        struct.pack_into("<H",bad,14,2)
        with self.assertRaisesRegex(ValueError,"narrow source bank"):
            self.check(init=bytes(bad))
    def test_original_palette_component_range_rejected(self):
        bad=bytearray(self.colors)
        bad[5]=64
        with self.assertRaisesRegex(ValueError,"COLORS"):
            self.check(colors=bytes(bad))
    def test_corrupted_eh6_header_rejected(self):
        bad=bytearray(self.res)
        pos=struct.unpack_from("<I",bad,4)[0]
        bad[pos+2]=7
        with self.assertRaisesRegex(ValueError,"not EH6"):
            self.check(res=bytes(bad))
    def test_retail_pin_rejects_synthetic(self):
        with self.assertRaisesRegex(ValueError,"pinned retail"):
            compare(self.res,self.init,self.colors,self.native,retail=True)
    def test_fidelity_lies_rejected(self):
        for flag in ("original_x86_executed",
                     "original_camera_projection_verified",
                     "original_wall_order_verified",
                     "original_dos_frame_parity_verified"):
            bad=copy.deepcopy(self.native)
            bad[flag]=True
            with self.assertRaisesRegex(ValueError,"unsupported"):
                self.check(report=bad)
    def test_cli_and_duplicate_native_json_keys(self):
        with tempfile.TemporaryDirectory() as tmp:
            root=Path(tmp)
            for name,data in (("RES4",self.res),("INIT",self.init),
                              ("COLORS",self.colors)):
                (root/name).write_bytes(data)
            native=root/"native.json"
            native.write_text(json.dumps(self.native))
            out=root/"result.json"
            args=[sys.executable,str(Path(__file__).with_name(
                  "verify_native_type6_columns.py")),
                  "--res4",str(root/"RES4"),"--init",str(root/"INIT"),
                  "--colors",str(root/"COLORS"),
                  "--native-report",str(native),"--out",str(out),
                  "--allow-other-version"]
            p=subprocess.run(args,capture_output=True,text=True)
            self.assertEqual(p.returncode,0,p.stderr)
            self.assertEqual(json.loads(out.read_text())["status"],"MATCH")
            self.assertNotIn(str(root),out.read_text())
            native.write_text('{"schema":"x","schema":"y"}')
            p=subprocess.run(args,capture_output=True,text=True)
            self.assertEqual(p.returncode,2)
            self.assertIn("duplicate",p.stderr)
if __name__=="__main__":
    unittest.main()
