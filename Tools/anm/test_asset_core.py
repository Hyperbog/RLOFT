#!/usr/bin/env python3
"""Focused tests for the shared Ravenloft asset extraction core."""
from __future__ import annotations

import struct
import tempfile
import unittest
from pathlib import Path

import media_inventory
import res_assets
import support_dumper


class AssetCoreTests(unittest.TestCase):
    def test_certified_inventory_shape(self) -> None:
        self.assertEqual(len(media_inventory.SOURCES), 33)
        self.assertEqual(len(media_inventory.ANM_NAMES), 13)
        self.assertEqual(len(media_inventory.RES_NAMES), 13)
        self.assertEqual(media_inventory.SOURCE_BY_NAME["RES3"].category, "resource")
        self.assertIn("ARIA", media_inventory.SOURCE_BY_NAME["GM1.BNK"].role)

    def test_lzss_literal_stream(self) -> None:
        # LSB-first flag bits: three literal bytes.
        self.assertEqual(res_assets.lzss_4k(bytes([0x07, 0x41, 0x42, 0x43]), 3), b"ABC")

    def test_type7_metadata(self) -> None:
        body = struct.pack("<HhhhhH", 64, 0, 0, 0, 1, 12) + b"\x00\x00"
        metadata = res_assets.type7_metadata(body)
        self.assertEqual(metadata["source_width"], 64)
        self.assertEqual(metadata["column_offsets"], [12])

    def test_sample_ad_single_timbre(self) -> None:
        # One six-byte directory entry, FF FF terminator, one 14-byte timbre.
        record = struct.pack("<H", 14) + bytes(12)
        data = bytes([0, 0]) + struct.pack("<I", 8) + b"\xFF\xFF" + record
        with tempfile.TemporaryDirectory() as temp:
            path = Path(temp) / "SAMPLE.AD"
            path.write_bytes(data)
            timbres = support_dumper.parse_sample_ad(path)
        self.assertEqual(len(timbres), 1)
        self.assertEqual(timbres[0]["bank"], 0)
        self.assertEqual(timbres[0]["patch"], 0)
        self.assertEqual(timbres[0]["record_length"], 14)


if __name__ == "__main__":
    unittest.main()
