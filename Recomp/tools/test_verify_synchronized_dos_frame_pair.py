"""Source-only negative tests for DOS/private indexed frame synchronization."""
from __future__ import annotations

import hashlib
import json
from pathlib import Path
import tempfile
import unittest

from compare_dos_indexed_frames import INDEX_BYTES, DAC_BYTES
from verify_synchronized_dos_frame_pair import check_pair, SCHEMA


def digest(blob: bytes) -> str:
    return hashlib.sha256(blob).hexdigest()


class SynchronizationTests(unittest.TestCase):
    def setUp(self):
        self.dir = tempfile.TemporaryDirectory()
        self.addCleanup(self.dir.cleanup)
        self.root = Path(self.dir.name)
        self.pix = bytes((i * 13) % 256 for i in range(INDEX_BYTES))
        self.pal = bytes(i % 64 for i in range(DAC_BYTES))
        self.write("dos.idx", self.pix)
        self.write("native.idx", self.pix)
        self.write("dos.pal", self.pal)
        self.write("native.pal", self.pal)
        self.obs = {
            "schema": SCHEMA,
            "level": 22,
            "frame_boundary": "1F8FC_1A5B0",
            "camera_x_16_16": 0x48000,
            "camera_y_16_16": 0x48000,
            "heading": 270,
            "crtc_start": 32768,
            "source_tick": 5,
            "keyboard_trace_sha256": digest(b"authenticated test keys"),
            "indices_sha256": digest(self.pix),
            "rgb6_sha256": digest(self.pal),
        }
        self.write_json("dos.json", dict(self.obs, role="original_dos"))
        self.write_json("native.json", dict(self.obs, role="compiled_recomp"))
        self.report = {
            "private_dos_raw_export": "EXACT_SCREENSHOT_AND_STATE_AGREEMENT",
            "state_vs_screenshot_matching_rgb_pixels": INDEX_BYTES,
            "state_vs_screenshot_different_rgb_pixels": 0,
            "synchronized_frame_evidence": "strong",
            "state_original_page_sha256": digest(self.pix),
            "original_dac_rgb6_sha256": digest(self.pal),
            "crtc_start": 32768,
            "crtc_line_compare": 399,
        }
        self.write_json("dosbox.json", self.report)

    def write(self, name, data):
        path = self.root / name
        path.write_bytes(data)
        return path

    def write_json(self, name, data):
        return self.write(name, (json.dumps(data)+"\n").encode("utf-8"))

    def run_check(self):
        return check_pair(
            self.root / "dos.idx", self.root / "native.idx",
            self.root / "dos.pal", self.root / "native.pal",
            self.root / "dos.json", self.root / "native.json",
            self.root / "dosbox.json")

    def test_equal_bytes_can_only_be_diagnostic_not_certified(self):
        found = self.run_check()
        self.assertTrue(found["pixel_and_palette_equal"])
        self.assertTrue(found["screenshot_save_state_exact"])
        self.assertFalse(found["original_dos_execution_independently_attested"])
        self.assertFalse(found["retail_dos_parity_certified"])

    def test_changed_native_index_is_detected(self):
        updated = bytearray(self.pix)
        updated[123456] ^= 1
        self.write("native.idx", updated)
        d = dict(self.obs, role="compiled_recomp",
                 indices_sha256=digest(updated))
        self.write_json("native.json", d)
        found = self.run_check()
        self.assertFalse(found["pixel_and_palette_equal"])
        self.assertFalse(found["indices"]["equal"])
        self.assertEqual(found["indices"]["mismatched_bytes"], 1)

    def test_changed_camera_is_rejected_even_with_equal_pixels(self):
        self.write_json("native.json", dict(self.obs, role="compiled_recomp",
                                           camera_x_16_16=0x58000))
        with self.assertRaisesRegex(ValueError, "not synchronized"):
            self.run_check()

    def test_different_source_boundary_is_rejected(self):
        self.write_json("native.json", dict(self.obs, role="compiled_recomp",
                                           frame_boundary="1FC14_1A510"))
        with self.assertRaisesRegex(ValueError, "not synchronized"):
            self.run_check()

    def test_tick_or_input_trace_mismatch_is_rejected(self):
        for changes in (
            {"source_tick": 6},
            {"keyboard_trace_sha256": digest(b"different host keys")},
        ):
            with self.subTest(changes=changes):
                self.write_json("native.json", dict(self.obs,
                                role="compiled_recomp", **changes))
                with self.assertRaisesRegex(ValueError, "not synchronized"):
                    self.run_check()

    def test_dos_export_not_synchronized_is_rejected(self):
        self.write_json("dosbox.json", dict(self.report,
                        state_vs_screenshot_matching_rgb_pixels=INDEX_BYTES-1,
                        state_vs_screenshot_different_rgb_pixels=1))
        with self.assertRaisesRegex(ValueError, "screenshot"):
            self.run_check()

    def test_missing_export_attestation_is_rejected(self):
        self.write_json("dosbox.json", dict(self.report,
                        private_dos_raw_export="NOT_REQUESTED"))
        with self.assertRaisesRegex(ValueError, "not exported"):
            self.run_check()

    def test_dos_report_hash_must_match_actual_bytes(self):
        self.write_json("dosbox.json", dict(self.report,
                        state_original_page_sha256=digest(b"unrelated frame")))
        with self.assertRaisesRegex(ValueError, "saved-state receipt"):
            self.run_check()

    def test_observation_hash_must_match_file(self):
        self.write_json("dos.json", dict(self.obs, role="original_dos",
                        indices_sha256=digest(b"fake")))
        with self.assertRaisesRegex(ValueError, "observation SHA256"):
            self.run_check()

    def test_different_display_page_is_rejected(self):
        self.write_json("native.json", dict(self.obs,
                        role="compiled_recomp", crtc_start=0))
        with self.assertRaisesRegex(ValueError, "not synchronized"):
            self.run_check()

    def test_different_level_is_rejected(self):
        self.write_json("native.json", dict(self.obs,
                        role="compiled_recomp", level=21))
        with self.assertRaisesRegex(ValueError, "Only authenticated Level 22"):
            self.run_check()

    def test_palette_mismatch_still_fails_even_with_matching_pixels(self):
        altered = bytearray(self.pal)
        altered[7] = 25 if altered[7] != 25 else 26
        self.write("native.pal", altered)
        self.write_json("native.json", dict(self.obs,
                        role="compiled_recomp", rgb6_sha256=digest(altered)))
        found = self.run_check()
        self.assertTrue(found["indices"]["equal"])
        self.assertFalse(found["palette_rgb6"]["equal"])
        self.assertFalse(found["pixel_and_palette_equal"])


if __name__ == "__main__":
    unittest.main()
