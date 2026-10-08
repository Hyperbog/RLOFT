"""Ensure the WPF crest is a valid reconstructed original-game PNG."""
from __future__ import annotations

import tempfile
import unittest
from pathlib import Path

from PIL import Image

from render_wpf_logo import make_logo


class LogoTests(unittest.TestCase):
    def test_reconstruction_produces_valid_transparent_png(self):
        with tempfile.TemporaryDirectory() as temp:
            filename = Path(temp) / "logo.png"
            self.assertEqual(make_logo(filename), filename)
            with Image.open(filename) as image:
                self.assertEqual(image.format, "PNG")
                self.assertEqual(image.mode, "RGBA")
                self.assertEqual(image.size, (240, 114))
                self.assertEqual(image.getpixel((0, 0))[3], 0)
                self.assertTrue(image.getbbox())


if __name__ == "__main__":
    unittest.main()
