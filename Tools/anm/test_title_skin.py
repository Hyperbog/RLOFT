#!/usr/bin/env python3
"""Regression coverage for the baked Strahd title-screen GUI skin."""
from __future__ import annotations

import unittest

import title_skin


class TitleSkinTests(unittest.TestCase):
    def test_baked_source_assets_decode(self) -> None:
        panel = title_skin.menu_panel()
        texture = title_skin.red_texture()
        self.assertEqual(panel.size, (208, 191))
        self.assertEqual(texture.size, (96, 200))
        self.assertEqual(panel.mode, "RGBA")
        self.assertEqual(texture.mode, "RGB")

    def test_full_window_skin_renders_at_common_sizes(self) -> None:
        for width, height, scale in (
            (900, 680, 0.8),
            (1280, 900, 1.0),
            (1600, 1000, 1.2),
        ):
            image = title_skin.render_window(width, height, scale)
            self.assertEqual(image.size, (width, height))
            self.assertEqual(image.mode, "RGB")
            # Frame/title rendering must make the skin non-uniform.
            self.assertNotEqual(image.getpixel((2, 2)), image.getpixel((width // 2, height // 2)))

    def test_chrome_insets_are_positive(self) -> None:
        side, top, bottom = title_skin.chrome_insets(1.0)
        self.assertGreaterEqual(side, 38)
        self.assertGreaterEqual(top, 66)
        self.assertGreaterEqual(bottom, 36)


if __name__ == "__main__":
    unittest.main()
