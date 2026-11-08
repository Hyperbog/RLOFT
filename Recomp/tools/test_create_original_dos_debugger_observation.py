"""Synthetic-only tests: original retail game data are never distributed."""
import hashlib
import struct
import unittest
from unittest.mock import patch

from create_original_dos_debugger_observation import (
    candidate, digest, KNOWN_CODE2_SHA256, MIN_DGROUP, BYTES_320x400
)


class OriginalDosDebuggerTests(unittest.TestCase):
    def setUp(self):
        self.code2=b"synthetic executable fixture NOT RLOFT"
        self.memory=bytearray(MIN_DGROUP)
        struct.pack_into("<I",self.memory,0xC6C8,22)
        struct.pack_into("<I",self.memory,0x255D0,0x48000)
        struct.pack_into("<I",self.memory,0x255D4,0x58000)
        struct.pack_into("<I",self.memory,0xC658,270)
        self.index=bytes([i%256 for i in range(BYTES_320x400)])
        self.palette=bytes([i%64 for i in range(768)])
        self.keys=b"\x00\x00\x48\x01"
        self.report={
            "private_dos_raw_export": "EXACT_SCREENSHOT_AND_STATE_AGREEMENT",
            "state_vs_screenshot_matching_rgb_pixels": BYTES_320x400,
            "state_vs_screenshot_different_rgb_pixels": 0,
            "synchronized_frame_evidence": "strong",
            "state_original_page_sha256": digest(self.index),
            "original_dac_rgb6_sha256": digest(self.palette),
            "crtc_start": 32768,
        }

    def check(self, **changes):
        data=dict(original_code2=self.code2,debugger_dgroup=bytes(self.memory),
                  original_indices=self.index,original_rgb6=self.palette,
                  screenshot_state_report=self.report,
                  source_keys=self.keys,tick=2)
        data.update(changes)
        # Unit tests substitute only a *test* identity anchor; the
        # shipped CLI retains the immutable authentic retail hash.
        with patch("create_original_dos_debugger_observation.KNOWN_CODE2_SHA256",
                   digest(self.code2)):
            return candidate(**data)

    def test_level22_camera_comes_from_debugger_memory(self):
        data=self.check()
        self.assertEqual(data["camera_x_16_16"],0x48000)
        self.assertEqual(data["camera_y_16_16"],0x58000)
        self.assertEqual(data["heading"],270)
        self.assertEqual(data["crtc_start"],32768)
        self.assertEqual(data["keyboard_trace_sha256"],digest(self.keys))
        self.assertFalse(data["original_dos_execution_independently_attested"])

    def test_bad_dgroup_rejected(self):
        with self.assertRaisesRegex(ValueError,"too small"):
            self.check(debugger_dgroup=bytes(self.memory[:0x10000]))

    def test_incorrect_retail_code2_rejected(self):
        with self.assertRaisesRegex(ValueError,"identity"):
            self.check(original_code2=b"changed")

    def test_invalid_camera_heading_rejected(self):
        struct.pack_into("<I",self.memory,0xC658,0xFFFFFFFF)
        with self.assertRaisesRegex(ValueError,"heading"):
            self.check()

    def test_wrong_level_rejected(self):
        struct.pack_into("<I",self.memory,0xC6C8,21)
        with self.assertRaisesRegex(ValueError,"Level 22"):
            self.check()

    def test_different_dos_export_sha_rejected(self):
        with self.assertRaisesRegex(ValueError,"hashes"):
            self.check(original_indices=bytes(BYTES_320x400))

    def test_mismatched_screenshot_rejected(self):
        r=dict(self.report,state_vs_screenshot_different_rgb_pixels=3)
        with self.assertRaisesRegex(ValueError,"do not agree"):
            self.check(screenshot_state_report=r)

    def test_non_rgb6_palette_rejected(self):
        pal=bytearray(self.palette);pal[0]=64
        with self.assertRaisesRegex(ValueError,"RGB6"):
            self.check(original_rgb6=bytes(pal))

    def test_ascii_keyboard_trace_rejected(self):
        with self.assertRaisesRegex(ValueError,"LE16"):
            self.check(source_keys=b"W")

    def test_boundary_not_manually_guessed(self):
        with self.assertRaisesRegex(ValueError,"Unsupported"):
            self.check(boundary="UNKNOWN")

    def test_report_never_claims_dos_attestation(self):
        data=self.check()
        self.assertEqual(data["source_tick_origin"],
                         "INDEPENDENTLY_COUNTED_EXTERNAL_DOS_EVENTS_REQUIRED")
        self.assertFalse(data["original_dos_execution_independently_attested"])


if __name__=="__main__":
    unittest.main()
