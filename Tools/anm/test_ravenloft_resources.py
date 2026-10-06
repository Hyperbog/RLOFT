import struct
import tempfile
import unittest
from pathlib import Path

import ravenloft_resources as rr


class ResourceDecoderTests(unittest.TestCase):
    def test_literal_lzss(self):
        self.assertEqual(rr._lzss_4k(b"\xffABCD", 4), b"ABCD")

    def test_colors_palette_expansion(self):
        with tempfile.TemporaryDirectory() as td:
            path = Path(td) / "COLORS"
            record = bytes([0, 31, 63]) * 256
            path.write_bytes(bytes([15]) + record * 15)
            palette = rr.ColorsFile(path).palette(0)
            self.assertEqual(palette[:3], bytes([0, 125, 255]))

    def test_raw_type1_raster(self):
        # One 8x2 raw type-1 raster wrapped in a one-entry RES archive.
        header = struct.pack("<HhBBBB", 1, 2, 1, 0, 0, 0)
        payload = bytes(range(16))
        entry = b"EH\x01" + struct.pack("<H", 8) + header + payload
        archive = struct.pack("<ii", 1, 8) + entry
        with tempfile.TemporaryDirectory() as td:
            path = Path(td) / "RES0"
            path.write_bytes(archive)
            raster = rr.ResArchive(path).raster(0)
            self.assertEqual((raster.width, raster.height), (8, 2))
            self.assertEqual(raster.pixels, payload)


if __name__ == "__main__":
    unittest.main()
