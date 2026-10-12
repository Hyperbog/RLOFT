"""Synthetic source-only CODE.2/COLORS RGB6 framebuffer differential tests."""
from __future__ import annotations
import copy
import json
from pathlib import Path
import struct
import subprocess
import sys
import tempfile
import unittest

from verify_native_vga_rgb6 import compare,source

WORLD_BANKS=(
    4,4,4,2,2,3,3,2,4,2,3,3,2,2,2,4,
    4,4,3,4,3,3,4,3,3,4,4,3,2,0,3,4,
)
def fixture():
    at=0x75F24+0x81AC
    code=bytearray(at+96)
    for level,bank in enumerate(WORLD_BANKS):
        code[at+level*3:at+level*3+3]=bytes((bank,level,31-level))
    colors=bytearray([15])
    for bank in range(15):
        for index in range(256):
            colors.extend(((index+bank)&63,
                           (index//4+bank*2)&63,
                           (index//16+bank*3)&63))
    pixels=bytes((row+col*5)&255 for col in range(64) for row in range(128))
    payload=bytearray()
    for offset in range(0,len(pixels),8):
        payload.append(255)
        payload.extend(pixels[offset:offset+8])
    entry=b"EH\x06\x00\x00"+struct.pack("<H",8192)+bytes(payload)
    offsets=struct.pack("<"+str(63)+"I",*[4+63*4 for _ in range(63)])
    archive=struct.pack("<I",63)+offsets+entry
    return {"CODE.2":bytes(code),"COLORS":bytes(colors),"RES4":archive}

class OriginalRgb6DifferentialTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.original=fixture()
        cls.native=source(cls.original,retail=False)
    def check(self,original=None,native=None):
        return compare(self.original if original is None else original,
                       self.native if native is None else native,
                       retail=False)
    def test_all_32_palette_tuples_and_two_pages(self):
        r=self.check()
        self.assertEqual(r["status"],"ORIGINAL_CODE2_DAC6_SCANOUT_MATCH")
        self.assertEqual(r["different_field_count"],0)
        self.assertEqual(r["source_original_level_palette_maps_checked"],32)
        self.assertEqual(r["original_colors_palette_banks_checked"],15)
        self.assertEqual(r["original_full_rgb6_page_hashes_checked"],33)
        self.assertEqual(r["world_palette_variants_checked"],4)
        self.assertFalse(r["original_dos_frame_parity_verified"])
    def test_changed_native_level_29_frame_digest_detected(self):
        bad=copy.deepcopy(self.native)
        bad["original_rgb6_frame_level_page0_sha256"][29]="0"*64
        r=self.check(native=bad)
        self.assertEqual(r["different_field_count"],1)
        self.assertEqual(r["first_difference"]["field"],
                         "original_rgb6_frame_level_page0_sha256/29")
    def test_original_colormap_value_change_changes_rgb6(self):
        changed=dict(self.original)
        colors=bytearray(changed["COLORS"])
        colors[1+0*768+246*3]^=1
        changed["COLORS"]=bytes(colors)
        r=self.check(original=changed)
        self.assertEqual(r["status"],"DIFFERENT")
        self.assertTrue(any(d["field"]=="original_rgb6_frame_level_page0_sha256/29"
                            for d in r["differences"]))
    def test_original_level_palette_selector_change_detected(self):
        changed=dict(self.original)
        code=bytearray(changed["CODE.2"])
        code[0x75F24+0x81AC+29*3]=4
        changed["CODE.2"]=bytes(code)
        r=self.check(original=changed)
        self.assertTrue(any(d["field"]=="original_level_display_triplets/29"
                            for d in r["differences"]))
    def test_original_texture_source_mutation_changes_page1_hash(self):
        changed=dict(self.original)
        res4=bytearray(changed["RES4"])
        first=struct.unpack_from("<I",res4,4)[0]
        res4[first+8]^=1  # Original texture column0/source row0, page1 case.
        changed["RES4"]=bytes(res4)
        r=self.check(original=changed)
        self.assertTrue(any(d["field"]=="original_rgb6_level0_page1_sha256"
                            for d in r["differences"]))
    def test_original_invalid_dac6_value_rejected(self):
        changed=dict(self.original)
        colors=bytearray(changed["COLORS"])
        colors[1]=64
        changed["COLORS"]=bytes(colors)
        with self.assertRaisesRegex(ValueError,"DAC6"):
            self.check(original=changed)
    def test_original_missing_63rd_entry_rejected(self):
        changed=dict(self.original)
        changed["RES4"]=struct.pack("<II",0,0)
        with self.assertRaisesRegex(ValueError,"diagnostic material"):
            self.check(original=changed)
    def test_native_unsupported_dos_parity_rejected(self):
        for field in ("original_live_dac_updates_verified",
                      "original_dos_frame_pixel_parity_verified",
                      "original_raycaster_camera_verified"):
            bad=copy.deepcopy(self.native)
            bad[field]=True
            with self.assertRaisesRegex(ValueError,"unsupported"):
                self.check(native=bad)
    def test_retail_hash_pin_rejects_synthetic(self):
        with self.assertRaisesRegex(ValueError,"retail SHA256"):
            compare(self.original,self.native,retail=True)
    def test_cli_and_duplicate_native_json_field(self):
        with tempfile.TemporaryDirectory() as tmp:
            root=Path(tmp)
            for name,data in self.original.items():
                (root/name).write_bytes(data)
            native=root/"native.json"
            out=root/"comparison.json"
            native.write_text(json.dumps(self.native))
            cmd=[sys.executable,
                 str(Path(__file__).with_name("verify_native_vga_rgb6.py")),
                 "--game-dir",str(root),"--native-report",str(native),
                 "--out",str(out),"--allow-synthetic"]
            p=subprocess.run(cmd,capture_output=True,text=True)
            self.assertEqual(p.returncode,0,p.stderr)
            self.assertEqual(json.loads(out.read_text())["different_field_count"],0)
            self.assertNotIn(str(root),out.read_text())
            native.write_text('{"schema":"a","schema":"b"}')
            p=subprocess.run(cmd,capture_output=True,text=True)
            self.assertEqual(p.returncode,2)
            self.assertIn("duplicate",p.stderr)
if __name__=="__main__":
    unittest.main()
