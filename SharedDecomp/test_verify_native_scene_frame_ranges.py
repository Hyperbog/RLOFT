"""Original-free frame-range source/NativeEngine differential regression tests."""
from __future__ import annotations
import copy
import json
from pathlib import Path
import struct
import subprocess
import sys
import tempfile
import unittest

from test_verify_native_scene_instances import source_fixture
from verify_native_scene_frame_ranges import source,compare

def fixture_report(data):
    expected=source(data,with_shapes=False)
    return {
        "schema":"rloft-native-authored-scene-frame-ranges/v1",
        **expected,
        "original_view_subframe_selection_verified":False,
        "original_npc_animation_frame_verified":False,
        "original_dos_depth_order_verified":False,
        "original_full_frame_parity_verified":False,
        "original_live_npc_animation_verified":False,
        "original_perspective_frame_choice_verified":False,
        "original_dos_scene_sort_verified":False,
        "original_frame_pixels_verified":False,
    }

class AuthoredFrameRangeOriginalDifferentialTests(unittest.TestCase):
    def setUp(self):
        self.media=source_fixture()
        self.native=fixture_report(self.media)
    def check(self,media=None,native=None):
        return compare(self.media if media is None else media,
                       self.native if native is None else native,
                       pin=False,with_shapes=False)

    def test_all_32_synthetic_original_frame_ranges_match(self):
        result=self.check()
        self.assertEqual(result["status"],
                         "ALL_ORIGINAL_SCENE_FRAME_RANGES_MATCH")
        self.assertEqual(result["difference_count"],0)
        self.assertEqual(result["independent_original_instances"],96)
        self.assertEqual(result["original_bounded_frame_references"],128)
        self.assertEqual(result["per_level_digest_checks"],32)
        self.assertFalse(result["original_dos_pixel_frame_parity_verified"])

    def test_original_npc_animation_byte_mutation_detected(self):
        changed=dict(self.media)
        npc=bytearray(changed["NPCS.DAT"])
        npc[4+0x18]=8
        changed["NPCS.DAT"]=bytes(npc)
        result=self.check(media=changed)
        self.assertEqual(result["status"],"DIFFERENT")
        self.assertTrue(any(d["field"]=="per_level_frame_ranges_sha256/22"
                            for d in result["differences"]))

    def test_original_object_stride_mutation_detected(self):
        changed=dict(self.media)
        obj=bytearray(changed["OBJECTS.DAT"])
        obj[4+0x58*0x41+0x2a]=2
        changed["OBJECTS.DAT"]=bytes(obj)
        result=self.check(media=changed)
        self.assertEqual(result["status"],"DIFFERENT")
        # The report intentionally caps the first 32 discrepancies. Its
        # 32 per-level hashes precede the changed slot-total field.
        self.assertTrue(any(d["field"]=="per_level_frame_ranges_sha256/22"
                            for d in result["differences"]))

    def test_original_init_variant_range_overrun_rejected(self):
        changed=dict(self.media)
        init=bytearray(changed["INIT"])
        init[0x26+4]=255;init[0x26+5]=255
        changed["INIT"]=bytes(init)
        with self.assertRaisesRegex(ValueError,"INIT variants"):
            self.check(media=changed)

    def test_native_per_level_digest_mismatch_is_localized(self):
        bad=copy.deepcopy(self.native)
        bad["per_level_frame_ranges_sha256"][22]="f"*64
        result=self.check(native=bad)
        self.assertEqual(result["difference_count"],1)
        self.assertEqual(result["first_divergence"]["field"],
                         "per_level_frame_ranges_sha256/22")

    def test_native_aggregate_range_digest_mismatch(self):
        bad=copy.deepcopy(self.native)
        bad["frame_ranges_sha256"]="0"*64
        result=self.check(native=bad)
        self.assertEqual(result["first_divergence"]["field"],
                         "frame_ranges_sha256")

    def test_missing_npc_definition_metadata_is_detected(self):
        bad=copy.deepcopy(self.native)
        bad.pop("npc_metadata_instances")
        result=self.check(native=bad)
        self.assertEqual(result["first_divergence"]["field"],
                         "npc_metadata_instances")

    def test_unsupported_dos_fidelity_claims_rejected(self):
        for flag in ("original_view_subframe_selection_verified",
                     "original_npc_animation_frame_verified",
                     "original_dos_depth_order_verified",
                     "original_full_frame_parity_verified",
                     "original_live_npc_animation_verified",
                     "original_perspective_frame_choice_verified",
                     "original_dos_scene_sort_verified",
                     "original_frame_pixels_verified"):
            bad=copy.deepcopy(self.native)
            bad[flag]=True
            with self.assertRaisesRegex(ValueError,"unsupported"):
                self.check(native=bad)

    def test_pinned_original_sha_rejects_synthetic(self):
        with self.assertRaisesRegex(ValueError,"pinned retail"):
            compare(self.media,self.native,pin=True,with_shapes=False)

    def test_cli_and_duplicate_json_keys(self):
        with tempfile.TemporaryDirectory() as td:
            root=Path(td)
            for name,data in self.media.items():
                (root/name).write_bytes(data)
            native=root/"native.json"
            native.write_text(json.dumps(self.native))
            out=root/"comparison.json"
            command=[sys.executable,str(Path(__file__).with_name(
                "verify_native_scene_frame_ranges.py")),
                "--game-dir",str(root),"--native-report",str(native),
                "--out",str(out),"--allow-synthetic"]
            result=subprocess.run(command,capture_output=True,text=True)
            self.assertEqual(result.returncode,0,result.stderr)
            self.assertEqual(json.loads(out.read_text())["status"],
                             "ALL_ORIGINAL_SCENE_FRAME_RANGES_MATCH")
            self.assertNotIn(str(root),out.read_text())
            native.write_text('{"schema":"x","schema":"y"}')
            result=subprocess.run(command,capture_output=True,text=True)
            self.assertEqual(result.returncode,2)
            self.assertIn("duplicate",result.stderr)

if __name__=="__main__":
    unittest.main()
