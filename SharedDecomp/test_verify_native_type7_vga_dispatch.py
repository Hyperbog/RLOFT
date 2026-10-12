"""Original-free CODE.2 mask dispatch and SINTAB/EH7 multi-pass oracle tests."""
from __future__ import annotations
import copy
import json
from pathlib import Path
import struct
import subprocess
import sys
import tempfile
import unittest

from test_verify_native_sintab_raster import fixture as original_free_sources
from verify_native_type7_vga_dispatch import compare,source

def code2_fixture():
    obj=0x75F24
    b=bytearray(obj+0xEF4D+1)
    for phase in range(4):
        for count in range(1,8):
            for x in range(phase,phase+count):
                lane=(0xC668,0xC688,0xC6A8)[x//4]
                b[obj+lane+phase*8+count] |= 1<<(x&3)
    b[obj+0xC029]=1
    b[obj+0xEF4D]=0
    return bytes(b)

def body_literal_position(original_entry_start,decoded_position):
    return original_entry_start+7+9*(decoded_position//8)+1+(decoded_position%8)

def samples():
    sintab,res3=original_free_sources()
    original=bytearray(res3)
    first=struct.unpack_from("<I",original,4)[0]
    # Earlier EH7 fixture leaves the first list stop at 2 and
    # source adjustment=-4, yielding an empty or rejected sample.
    # Make exactly one legal original-format nonempty span using
    # original scale block's 0x204..0x21A generated instruction window.
    for offset,value in ((17,0),(18,1),(19,0),(20,0)):
        original[body_literal_position(first,offset)]=value
    return code2_fixture(),sintab,bytes(original)

def native_sample(c,s,r):
    return {
        "schema":"rloft-native-type7-vga-dispatch/v1",
        **source(c,s,r),
        "original_16bit_x86_executed":False,
        "original_camera_projection_verified":False,
        "original_screen_frame_parity_verified":False,
    }

class OriginalType7VgaDifferentialTests(unittest.TestCase):
    def setUp(self):
        self.code,self.sintab,self.res3=samples()
        self.native=native_sample(self.code,self.sintab,self.res3)
    def check(self,native=None,code=None,sintab=None,res3=None):
        return compare(self.code if code is None else code,
                       self.sintab if sintab is None else sintab,
                       self.res3 if res3 is None else res3,
                       self.native if native is None else native,pin=False)

    def test_synthetic_original_two_modes_and_all_28_mask_triplets(self):
        result=self.check()
        self.assertEqual(result["status"],"MATCH")
        self.assertEqual(result["mask_triplets_checked"],28)
        self.assertEqual(result["source_modes_compared"],2)
        self.assertEqual(result["source_descriptor_count"],1)
        self.assertEqual(self.native["grouped_mode"]["plane_passes"],3)
        self.assertEqual(self.native["individual_mode"]["plane_passes"],7)
        self.assertEqual(self.native["grouped_mode"]["masked_vga_writes"],6)
        self.assertEqual(self.native["individual_mode"]["masked_vga_writes"],14)
        self.assertFalse(result["original_screen_frame_parity_verified"])

    def test_mutated_native_individual_plane_sha_detected(self):
        bad=copy.deepcopy(self.native)
        bad["individual_mode"]["four_plane_sha256"]="0"*64
        result=self.check(native=bad)
        self.assertEqual(result["difference_count"],1)
        self.assertEqual(result["first_divergence"]["field"],
                         "individual_mode/four_plane_sha256")

    def test_mutated_native_mask_locates_exact_phase_slot(self):
        bad=copy.deepcopy(self.native)
        bad["original_vga_mask_triplets"][20][0]^=1
        result=self.check(native=bad)
        self.assertEqual(result["first_divergence"]["field"],
                         "original_vga_mask_triplets/20")

    def test_original_code2_mask_corruption_is_rejected(self):
        bad=bytearray(self.code)
        bad[0x75F24+0xC668+2*8+7]^=1
        with self.assertRaisesRegex(ValueError,"mask|coverage|table"):
            self.check(code=bytes(bad))

    def test_original_res3_source_offset_mutation_is_detected(self):
        bad=bytearray(self.res3)
        first=struct.unpack_from("<I",bad,4)[0]
        bad[body_literal_position(first,19)]=1
        result=self.check(res3=bytes(bad))
        self.assertEqual(result["status"],"DIFFERENT")
        self.assertTrue(any(d["field"]=="original_res3_sha256"
                            for d in result["differences"]))

    def test_native_lookup_mode_metadata_mutation_is_detected(self):
        bad=copy.deepcopy(self.native)
        bad["original_code2_initial_individual"]=False
        result=self.check(native=bad)
        self.assertEqual(result["first_divergence"]["field"],
                         "original_code2_initial_individual")

    def test_false_original_gameplay_parity_is_rejected(self):
        for name in ("original_16bit_x86_executed",
                     "original_camera_projection_verified",
                     "original_screen_frame_parity_verified"):
            bad=copy.deepcopy(self.native);bad[name]=True
            with self.assertRaisesRegex(ValueError,"unsupported"):
                self.check(native=bad)

    def test_retail_identity_rejects_synthetic(self):
        with self.assertRaisesRegex(ValueError,"pinned SHA256"):
            compare(self.code,self.sintab,self.res3,self.native,pin=True)

    def test_native_missing_grouped_draw_digest_detected(self):
        bad=copy.deepcopy(self.native)
        bad["grouped_mode"].pop("four_plane_sha256")
        self.assertEqual(self.check(native=bad)["first_divergence"]["field"],
                         "grouped_mode/four_plane_sha256")

    def test_cli_and_duplicate_native_json_field(self):
        with tempfile.TemporaryDirectory() as tmp:
            root=Path(tmp)
            files=(("CODE.2",self.code,"--code2"),
                   ("SINTAB",self.sintab,"--sintab"),
                   ("RES3",self.res3,"--res3"))
            cli=[sys.executable,
                 str(Path(__file__).with_name("verify_native_type7_vga_dispatch.py"))]
            for name,data,flag in files:
                path=root/name
                path.write_bytes(data)
                cli.extend((flag,str(path)))
            native=root/"native.json"
            out=root/"result.json"
            native.write_text(json.dumps(self.native))
            cli.extend(("--native-report",str(native),"--out",str(out),
                        "--allow-other-version"))
            run=subprocess.run(cli,capture_output=True,text=True)
            self.assertEqual(run.returncode,0,run.stderr)
            self.assertEqual(json.loads(out.read_text())["status"],"MATCH")
            self.assertNotIn(str(root),out.read_text())
            native.write_text('{"schema":"a","schema":"b"}')
            run=subprocess.run(cli,capture_output=True,text=True)
            self.assertEqual(run.returncode,2)
            self.assertIn("duplicate",run.stderr)

if __name__=="__main__":
    unittest.main()
