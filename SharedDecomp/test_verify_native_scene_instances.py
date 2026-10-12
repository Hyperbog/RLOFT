"""Synthetic original-free tests for complete 32-level RES3 source-base oracle."""
from __future__ import annotations

import copy
import hashlib
import json
from pathlib import Path
import struct
import subprocess
import sys
import tempfile
import unittest

from verify_native_scene_instances import compare,derive


def put16(b,pos,v):
    struct.pack_into("<H",b,pos,v)


def put32(b,pos,v):
    struct.pack_into("<I",b,pos,v)


def source_fixture():
    init=bytearray(0x24+2+175*7+2+12*7)
    for i,v in enumerate((0,2169,3316,4158)):
        put16(init,6+i*2,v)
    put16(init,0x24,175)
    for j in range(175):
        p=0x26+j*7
        init[p+4]=2
        init[p+5]=1
    # Last descriptor explicitly closes the genuine 1147-entry
    # class-one bank; the others stay at an in-range base.
    put16(init,0x26+174*7,1146)
    init[0x26+174*7+4]=1
    second=0x26+175*7
    put16(init,second,12)
    for j in range(12):
        p=second+2+j*7
        init[p+4]=1
        init[p+5]=1

    put16(init,second+2+11*7,75)
    npcs=bytearray(4+83*0x45)
    put32(npcs,0,83)
    put16(npcs,4+82*0x45+0x14,2)
    objects=bytearray(4+336*0x41)
    put32(objects,0,336)
    put16(objects,4+0x58*0x41+0x28,1)
    objects[4+0x58*0x41+0x2a]=1
    put16(objects,4+335*0x41+0x28,841)
    objects[4+335*0x41+0x2a]=1

    b0=29
    b1=b0+0x2a4+2*2*2
    b2=b1+2*2
    b3=b2+11
    b4=b3+11
    b5=b4+11
    b6=b5+50*26
    rec=bytearray(b6+4+4+4)
    for j,p in enumerate((b0,b1,b2,b3,b4,b5,b6)):
        put32(rec,1+4*j,p)
    put16(rec,b0+1,2)
    put16(rec,b0+3,2)
    put16(rec,b0+0x28a,1)
    put16(rec,b0+0x28c,1)
    put16(rec,b0+0x28e,1)
    # one A source type 0, x=1, y=0, facing=5
    put16(rec,b2,0);put16(rec,b2+2,1);rec[b2+10]=5
    # one B source type 0, x=0, y=1
    put16(rec,b3,0);put16(rec,b3+6,1)
    # one C source type 0x54, x=1, y=1, alias -> 0x58
    put16(rec,b4,0x54);put16(rec,b4+2,1);put16(rec,b4+6,1)
    put16(rec,b6,2);put16(rec,b6+2,2)
    levels=bytearray(4+32*4)
    put32(levels,0,32)
    for i in range(32):
        put32(levels,4+4*i,len(levels))
        levels+=rec
    return {"LEVELS.DAT":bytes(levels),"INIT":bytes(init),
            "NPCS.DAT":bytes(npcs),"OBJECTS.DAT":bytes(objects)}


def synthetic_native(src):
    reference=derive(src["LEVELS.DAT"],src["INIT"],
                     src["NPCS.DAT"],src["OBJECTS.DAT"])
    return {
        "schema":"rloft-native-authored-scene-instances/v1",
        **reference,
        "original_res3_archive_present":False,
        "res3_archive_count_source":"known_retail_directory_cardinality",
        "original_view_frame_selection_verified":False,
        "original_dos_rasterizer_executed":False,
        "gameplay_parity_verified":False,
    }


class NativeSceneInstancesOracleTests(unittest.TestCase):
    def setUp(self):
        self.original=source_fixture()
        self.native=synthetic_native(self.original)

    def check(self,native=None,source=None):
        return compare(self.original if source is None else source,
                       self.native if native is None else native,
                       require_known=False)

    def test_full_32_map_synthetic_source_bases(self):
        result=self.check()
        self.assertEqual(result["status"],"ALL_ORIGINAL_SCENE_BASES_MATCH")
        self.assertEqual(result["verified_original_authored_instances"],96)
        self.assertEqual(result["per_level_resolution_hashes_checked"],32)
        self.assertFalse(result["original_res3_content_decoded"])
        self.assertFalse(result["original_gameplay_parity_verified"])

    def test_retail_sha_gate_rejects_synthetic(self):
        with self.assertRaisesRegex(ValueError,"SHA256"):
            compare(self.original,self.native,require_known=True)

    def test_source_family_byte_mutation_detected_in_map(self):
        modified=dict(self.original)
        src=bytearray(modified["LEVELS.DAT"])
        start=struct.unpack_from("<I",src,4)[0]
        family_a=struct.unpack_from("<I",src,start+1+2*4)[0]
        src[start+family_a+10]^=1
        modified["LEVELS.DAT"]=bytes(src)
        result=self.check(source=modified)
        self.assertEqual(result["status"],"DIFFERENT")
        self.assertTrue(any(change["field"]=="per_level_resolved_sha256/0"
                            for change in result["differences"]))

    def test_definition_source_change_detected_even_when_levels_same(self):
        source=dict(self.original)
        objects=bytearray(source["OBJECTS.DAT"])
        idx=4+0x58*0x41+0x28
        put16(objects,idx,0)
        source["OBJECTS.DAT"]=bytes(objects)
        # The unchanged final definition still closes class-two bank.
        source["OBJECTS.DAT"]=bytes(objects)
        result=self.check(source=source)
        self.assertTrue(any(d["field"]=="resolved_source_bases_sha256"
                            for d in result["differences"]))
        self.assertTrue(any(d["field"]=="per_level_resolved_sha256/22"
                            for d in result["differences"]))

    def test_native_per_level_hash_mutation_reports_level(self):
        report=copy.deepcopy(self.native)
        report["per_level_resolved_sha256"][22]="0"*64
        result=self.check(native=report)
        self.assertEqual(result["difference_count"],1)
        self.assertEqual(result["first_divergence"]["field"],
                         "per_level_resolved_sha256/22")

    def test_native_missing_resolved_hash_detected(self):
        report=copy.deepcopy(self.native)
        report.pop("resolved_source_bases_sha256")
        result=self.check(native=report)
        self.assertEqual(result["first_divergence"]["field"],
                         "resolved_source_bases_sha256")

    def test_bad_original_directory_rejected(self):
        src=dict(self.original)
        levels=bytearray(src["LEVELS.DAT"])
        put32(levels,8,132)
        src["LEVELS.DAT"]=bytes(levels)
        with self.assertRaisesRegex(ValueError,"directory"):
            self.check(source=src)

    def test_bad_init_allocation_rejected(self):
        src=dict(self.original)
        init=bytearray(src["INIT"])
        put16(init,0x26,1147) # base+stride exceeds 1147 entries
        src["INIT"]=bytes(init)
        with self.assertRaisesRegex(ValueError,"INIT shape"):
            self.check(source=src)

    def test_unsupported_runtime_parity_claim_rejected(self):
        for field in ("original_view_frame_selection_verified",
                      "original_dos_rasterizer_executed",
                      "gameplay_parity_verified"):
            report=copy.deepcopy(self.native)
            report[field]=True
            with self.assertRaisesRegex(ValueError,"unsupported"):
                self.check(native=report)

    def test_cli_on_synthetic_and_duplicate_json_keys(self):
        with tempfile.TemporaryDirectory() as td:
            root=Path(td)
            names={"LEVELS.DAT":"--levels","INIT":"--init",
                   "NPCS.DAT":"--npcs","OBJECTS.DAT":"--objects"}
            command=[sys.executable,str(
                Path(__file__).with_name("verify_native_scene_instances.py"))]
            for name,flag in names.items():
                f=root/name
                f.write_bytes(self.original[name])
                command.extend((flag,str(f)))
            native=root/"native.json"
            out=root/"result.json"
            native.write_text(json.dumps(self.native))
            command+=["--native-report",str(native),"--out",str(out),
                      "--allow-other-version"]
            completed=subprocess.run(command,capture_output=True,text=True)
            self.assertEqual(completed.returncode,0,completed.stderr)
            self.assertEqual(json.loads(out.read_text())["status"],
                             "ALL_ORIGINAL_SCENE_BASES_MATCH")
            self.assertNotIn(str(root),out.read_text())
            native.write_text('{"schema":"A","schema":"B"}')
            completed=subprocess.run(command,capture_output=True,text=True)
            self.assertEqual(completed.returncode,2)
            self.assertIn("duplicate",completed.stderr)


if __name__=="__main__":
    unittest.main()
