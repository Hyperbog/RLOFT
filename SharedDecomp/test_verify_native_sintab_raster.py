"""Source-only mutation tests for original SINTAB raster-plan differential."""
from __future__ import annotations
import copy
import json
from pathlib import Path
import struct
import subprocess
import sys
import tempfile
import unittest

from test_verify_native_res3_type7 import fixtures as res3_fixtures
from verify_native_sintab_raster import source,compare

def p16(data,pos,value):
    struct.pack_into("<H",data,pos,value)
def p32(data,pos,value):
    struct.pack_into("<I",data,pos,value)

def fixture():
    # Same six x86 opcode bytes observed in all shipped SINTAB blocks;
    # never execute them in the tests.
    code=bytes([
        0x8A,0x86,0,0,0,0,
        0x88,0xC4,
        0x29,0xC8,
        0x88,0x87,0,0,0,0,
        0x88,0xA7,80,0,0,0,
        0xC3])
    length=0x204+len(code)
    s=bytearray(0x1F42)
    p32(s,0x1B38,127)
    p32(s,0x1B3C,508)
    p32(s,0x1B40,128)
    p32(s,0x1F3E,length*127)
    for _ in range(127):
        blk=bytearray(length)
        for i in range(129):
            p16(blk,i*2,0x204+(22 if i==128 else 0))
        p16(blk,0x102+128*2,4)
        blk[0x204:]=code
        s+=struct.pack("<H",length)+blk
    s+=bytes(512)
    res3,_=res3_fixtures(8)
    # The pre-existing EH7 compression smoke fixture deliberately uses
    # stop=2, entry=4; correct its *literal* entry to 0 because original
    # CODE.2 draw plans require entry < stop in the source-domain gate.
    res3=bytearray(res3)
    for index in range(8):
        start=struct.unpack_from("<I",res3,4+4*index)[0]
        res3[start+7+2*9+1+5]=0  # EH7 decoded body[21], entry selector
    return bytes(s),bytes(res3)

def native_shape(s,r):
    return {
        "schema":"rloft-native-sintab-eh7-raster-plans/v1",
        **source(s,r),
        "original_x86_code_executed":False,
        "safe_opcode_interpreter_implemented":True,
        "all_127_original_opcode_streams_structurally_validated":True,
        "original_view_raycast_verified":False,
        "original_pixel_frame_parity_verified":False,
    }

class SintabNativeRasterDifferentialTests(unittest.TestCase):
    def setUp(self):
        self.s,self.r=fixture()
        self.native=native_shape(self.s,self.r)
    def cmp(self,native=None,sintab=None,res3=None):
        return compare(
            self.s if sintab is None else sintab,
            self.r if res3 is None else res3,
            self.native if native is None else native,
            require_retail=False)
    def test_complete_synthetic_127_programs_and_eight_eh7_shapes(self):
        result=self.cmp()
        self.assertEqual(result["status"],"ORIGINAL_SINTAB_RASTER_PLAN_MATCH")
        self.assertEqual(result["scale_blocks_compared"],127)
        self.assertEqual(result["selector_scale_mappings_checked"],48)
        self.assertFalse(result["original_dos_x86_executed"])
        self.assertFalse(result["original_pixel_frame_parity_verified"])
    def test_mutated_native_code_digest_finds_scale(self):
        candidate=copy.deepcopy(self.native)
        candidate["scale_blocks"][63]["generated_code_sha256"]="0"*64
        result=self.cmp(native=candidate)
        self.assertEqual(result["difference_count"],1)
        self.assertEqual(result["first_divergence"]["field"],
                         "scale_blocks/63/generated_code_sha256")
    def test_mutated_original_generated_opcode_rejected(self):
        s=bytearray(self.s)
        s[0x1F42+2+0x204]=0x90
        with self.assertRaisesRegex(ValueError,"unknown original SINTAB"):
            self.cmp(sintab=bytes(s))
    def test_mutated_original_scale_steps_fingerprint_detected(self):
        s=bytearray(self.s)
        pos=0x1F42+2+0x102+128*2
        p16(s,pos,5)
        result=self.cmp(sintab=bytes(s))
        self.assertEqual(result["first_divergence"]["field"],
                         "original_sintab_sha256")
        self.assertTrue(any(x["field"]=="scale_blocks/0/summed_scale_steps"
                            for x in result["differences"]))
    def test_modified_original_res3_entry_digest_detected(self):
        r=bytearray(self.r)
        pos=struct.unpack_from("<I",r,4)[0]
        # The EH7 literal byte at body offset 16 is padding, not
        # a pointer. Change it without altering geometry.
        r[pos+7+1+2*9]=43
        result=self.cmp(res3=bytes(r))
        self.assertEqual(result["first_divergence"]["field"],
                         "original_res3_sha256")
    def test_missing_scale_report_field_detected(self):
        candidate=copy.deepcopy(self.native)
        del candidate["scale_blocks"][126]["instructions"]
        self.assertEqual(self.cmp(native=candidate)["first_divergence"]["field"],
                         "scale_blocks/126/instructions")
    def test_unsupported_original_frame_or_x86_claim_rejected(self):
        for name in ("original_x86_code_executed",
                     "original_view_raycast_verified",
                     "original_pixel_frame_parity_verified"):
            candidate=copy.deepcopy(self.native);candidate[name]=True
            with self.assertRaisesRegex(ValueError,"unsupported"):
                self.cmp(native=candidate)
    def test_retail_source_identity_gate(self):
        with self.assertRaisesRegex(ValueError,"pinned retail"):
            compare(self.s,self.r,self.native,require_retail=True)
    def test_original_trailer_mutation_rejected(self):
        s=bytearray(self.s);s[-1]=1
        with self.assertRaisesRegex(ValueError,"trailer"):
            self.cmp(sintab=bytes(s))
    def test_cli_and_duplicate_json_key_gate(self):
        with tempfile.TemporaryDirectory() as td:
            root=Path(td)
            for name,data in (("SINTAB",self.s),("RES3",self.r)):
                (root/name).write_bytes(data)
            report=root/"native.json"
            report.write_text(json.dumps(self.native))
            out=root/"diff.json"
            cmd=[sys.executable,
                 str(Path(__file__).with_name("verify_native_sintab_raster.py")),
                 "--sintab",str(root/"SINTAB"),"--res3",str(root/"RES3"),
                 "--native-report",str(report),"--out",str(out),
                 "--allow-other-version"]
            result=subprocess.run(cmd,capture_output=True,text=True)
            self.assertEqual(result.returncode,0,result.stderr)
            self.assertEqual(json.loads(out.read_text())["status"],
                             "ORIGINAL_SINTAB_RASTER_PLAN_MATCH")
            self.assertNotIn(str(root),out.read_text())
            report.write_text('{"schema":"a","schema":"b"}')
            result=subprocess.run(cmd,capture_output=True,text=True)
            self.assertEqual(result.returncode,2)
            self.assertIn("duplicate",result.stderr)
if __name__=="__main__":
    unittest.main()
