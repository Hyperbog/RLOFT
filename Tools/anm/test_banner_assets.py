#!/usr/bin/env python3
"""Regression tests for baked Strahd banner source assets."""
from __future__ import annotations

import unittest
from pathlib import Path

from PIL import Image


ASSETS = Path(__file__).with_name("assets")


class BannerAssetTests(unittest.TestCase):
    def test_runtime_banner_sources_decode(self) -> None:
        expected = {
            "strahd_frame_main.png": (320, 200),
            "strahd_parchment.png": (96, 200),
            "strahd_banner.webp": (1120, 150),
        }
        for name, size in expected.items():
            with self.subTest(name=name):
                with Image.open(ASSETS / name) as image:
                    decoded = image.convert("RGB")
                    decoded.load()
                    self.assertEqual(decoded.size, size)

    def test_clean_banner_geometry(self) -> None:
        with Image.open(ASSETS / "strahd_frame_main.png") as image:
            frame = image.convert("RGB")
        with Image.open(ASSETS / "strahd_parchment.png") as image:
            parchment = image.convert("RGB")
        with Image.open(ASSETS / "strahd_banner.webp") as image:
            baked = image.convert("RGB")

        self.assertGreaterEqual(frame.width, 64)
        self.assertGreaterEqual(frame.height, 150)
        self.assertGreater(parchment.width, 0)
        self.assertEqual(baked.size, (1120, 150))

        logo_w, logo_h = 240, 114
        logo_x = (baked.width - logo_w) // 2
        logo_y = (baked.height - logo_h) // 2
        crest = baked.crop((logo_x, logo_y, logo_x + logo_w, logo_y + logo_h))
        self.assertEqual(crest.size, (240, 114))


if __name__ == "__main__":
    unittest.main()
