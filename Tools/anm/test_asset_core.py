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

    def test_empty_type1_record_is_valid(self) -> None:
        # Certified RES12:74/153/154/156/157 use this exact empty-raster form.
        resource = res_assets.EhResource(
            archive=12,
            index=74,
            resource_type=1,
            header=bytes.fromhex("0000010002fe0300"),
            payload=b"",
            raw=b"",
        )
        raster, widths = res_assets.decode_type1(resource)
        self.assertIsNone(widths)
        self.assertEqual(raster.count, 0)
        self.assertEqual(raster.width, 16)
        self.assertEqual(raster.element_height, 1)
        self.assertEqual(raster.pixels, b"")

    def test_type7_native_raster_reuses_duplicate_column_source(self) -> None:
        # width=4, x=0..2. x0/x1 share one descriptor pointer and therefore
        # intentionally share the same packed source texels.
        header = struct.pack("<Hhhhh", 4, 0, 2, 0, 2)
        table_end = 10 + 3 * 2
        source = bytes([10, 11, 20])
        p1 = table_end + len(source)
        list1 = struct.pack("<HhHH", 4, 0, 0, 0)
        p2 = p1 + len(list1)
        list2 = struct.pack("<HhHH", 4, 0, 2, 0)
        body = header + struct.pack("<HHH", p1, p1, p2) + source + list1 + list2

        raster = res_assets.decode_type7_raster(body)
        self.assertEqual((raster.width, raster.height), (4, 128))
        self.assertEqual(raster.pixels[0 * 4 + 0], 10)
        self.assertEqual(raster.pixels[1 * 4 + 0], 11)
        self.assertEqual(raster.pixels[0 * 4 + 1], 10)
        self.assertEqual(raster.pixels[1 * 4 + 1], 11)
        self.assertEqual(raster.pixels[1 * 4 + 2], 20)
        self.assertEqual(raster.mask[0 * 4 + 0], 255)
        self.assertEqual(raster.mask[1 * 4 + 2], 255)
        self.assertEqual(raster.mask[0 * 4 + 3], 0)

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
