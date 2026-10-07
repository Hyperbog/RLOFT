#!/usr/bin/env python3
"""Regression tests for the baked original title-screen window skin."""
from __future__ import annotations

import unittest

import title_skin


class TitleSkinTests(unittest.TestCase):
    def test_original_menu_panel_decodes_exact_geometry(self) -> None:
        panel = title_skin.menu_panel()
        self.assertEqual(panel.size, (208, 191))
        self.assertEqual(panel.mode, "RGBA")

    def test_red_texture_decodes(self) -> None:
        texture = title_skin.red_texture()
        self.assertGreater(texture.width, 0)
        self.assertGreater(texture.height, 0)
        self.assertEqual(texture.mode, "RGB")

    def test_window_skin_matches_requested_canvas(self) -> None:
        image = title_skin.render_window(1280, 900, 1.0)
        self.assertEqual(image.size, (1280, 900))
        self.assertEqual(image.mode, "RGB")

        # Decorative corners/rails must differ from the tiled interior field.
        self.assertNotEqual(image.getpixel((12, 12)), image.getpixel((640, 450)))
        self.assertNotEqual(image.getpixel((640, 15)), image.getpixel((640, 450)))

    def test_ravenloft_header_renders(self) -> None:
        header = title_skin.render_header(1280, 104, 1.0)
        self.assertEqual(header.size, (1280, 104))
        self.assertEqual(header.mode, "RGB")
        self.assertNotEqual(header.getpixel((640, 52)), header.getpixel((10, 52)))

        # The full-bleed header must redraw the outer top rail because the
        # header canvas otherwise covers the frame rendered behind it.
        self.assertEqual(header.getpixel((100, 16)), (40, 29, 31))

    def test_chrome_insets_are_positive(self) -> None:
        side, top, bottom = title_skin.chrome_insets(1.0)
        self.assertGreater(side, 0)
        self.assertEqual(top, 0)
        self.assertGreaterEqual(bottom, 66)


if __name__ == "__main__":
    unittest.main()
