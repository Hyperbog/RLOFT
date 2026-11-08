"""Original-free tests for strict DOS/native indexed-frame comparison."""
from __future__ import annotations

import tempfile
from pathlib import Path
import unittest

from compare_dos_indexed_frames import (
    DAC_BYTES, INDEX_BYTES, compare, validate,
)


class IndexedFrameOracleTests(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.addCleanup(self.tmp.cleanup)
        self.root = Path(self.tmp.name)

    def save(self, name: str, content: bytes) -> Path:
        path = self.root / name
        path.write_bytes(content)
        return path

    def test_exact_indices_and_palette(self):
        pixels = bytes((i * 7) % 256 for i in range(INDEX_BYTES))
        palette = bytes(i % 64 for i in range(DAC_BYTES))
        d = self.save("dos.idx", pixels)
        n = self.save("native.idx", pixels)
        dp = self.save("dos.pal", palette)
        np = self.save("native.pal", palette)
        receipt = validate(d, n, dp, np)
        self.assertTrue(receipt["indices"]["equal"])
        self.assertTrue(receipt["palette_rgb6"]["equal"])
        self.assertTrue(receipt["pixel_and_palette_equal"])
        self.assertFalse(receipt["reference_provenance_verified"])

    def test_exact_pixel_mismatch_and_coordinates(self):
        original = bytearray(INDEX_BYTES)
        native = bytearray(original)
        native[320 * 27 + 19] = 199
        result = compare(original, native, 320)
        self.assertFalse(result["equal"])
        self.assertEqual(result["mismatched_bytes"], 1)
        self.assertEqual((result["first_mismatch"]["x"],
                          result["first_mismatch"]["y"]), (19, 27))

    def test_palette_mismatch_fails_full_frame(self):
        pixels = bytes(INDEX_BYTES)
        original = bytearray(DAC_BYTES)
        native = bytearray(original)
        native[3] = 18
        receipt = validate(self.save("d.idx", pixels),
                           self.save("n.idx", pixels),
                           self.save("d.pal", original),
                           self.save("n.pal", native))
        self.assertTrue(receipt["indices"]["equal"])
        self.assertFalse(receipt["palette_rgb6"]["equal"])
        self.assertFalse(receipt["pixel_and_palette_equal"])

    def test_bad_frame_length_is_rejected(self):
        with self.assertRaisesRegex(ValueError, "byte length"):
            validate(self.save("d.idx", b"bad"),
                     self.save("n.idx", bytes(INDEX_BYTES)))

    def test_invalid_rgb6_is_rejected(self):
        pixels = bytes(INDEX_BYTES)
        invalid = bytearray(DAC_BYTES)
        invalid[0] = 64
        with self.assertRaisesRegex(ValueError, "above 63"):
            validate(self.save("d.idx", pixels),
                     self.save("n.idx", pixels),
                     self.save("d.pal", invalid),
                     self.save("n.pal", bytes(DAC_BYTES)))

    def test_palette_pair_is_required(self):
        pixels = bytes(INDEX_BYTES)
        with self.assertRaisesRegex(ValueError, "together"):
            validate(self.save("d.idx", pixels),
                     self.save("n.idx", pixels),
                     self.save("d.pal", bytes(DAC_BYTES)), None)


if __name__ == "__main__":
    unittest.main()
