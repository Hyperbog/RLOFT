#!/usr/bin/env python3
"""Regression tests for automatic Ravenloft palette routing."""
from __future__ import annotations

import unittest

import palette_router


class PaletteRouterTests(unittest.TestCase):
    def test_res0_world_mode_triplets_follow_palette_mode(self) -> None:
        self.assertEqual(palette_router.resolve_palette(0, 16, 1).bank, 0)
        self.assertEqual(palette_router.resolve_palette(0, 19, 1).bank, 1)
        self.assertEqual(palette_router.resolve_palette(0, 22, 1).bank, 2)
        self.assertEqual(palette_router.resolve_palette(0, 25, 1).bank, 3)
        self.assertEqual(palette_router.resolve_palette(0, 28, 1).bank, 4)

    def test_res0_world_mode_single_resource_families(self) -> None:
        for start in (31, 36, 41, 46, 51):
            for mode in range(5):
                self.assertEqual(
                    palette_router.resolve_palette(0, start + mode, 1).bank,
                    mode,
                )

    def test_fixed_modal_palette_mappings(self) -> None:
        self.assertEqual(palette_router.resolve_palette(0, 63, 1).bank, 10)
        self.assertEqual(palette_router.resolve_palette(0, 65, 1).bank, 11)
        self.assertEqual(palette_router.resolve_palette(0, 100, 1).bank, 12)
        self.assertEqual(palette_router.resolve_palette(0, 77, 1).bank, 13)
        self.assertEqual(palette_router.resolve_palette(0, 116, 1).bank, 13)

    def test_environment_profile_palette_context(self) -> None:
        p0 = palette_router.resolve_palette(8, 8, 1)
        self.assertEqual(p0.bank, 4)
        self.assertEqual(p0.alternatives, (3,))
        p1 = palette_router.resolve_palette(8, 45, 1)
        self.assertEqual(p1.bank, 4)
        self.assertEqual(p1.confidence, "exact")

    def test_world_asset_records_valid_runtime_alternatives(self) -> None:
        decision = palette_router.resolve_palette(3, 100, 7)
        self.assertEqual(decision.bank, 4)
        self.assertEqual(set(decision.alternatives), {0, 2, 3})
        self.assertEqual(palette_router.WORLD_RUNTIME_BANKS, (0, 2, 3, 4))


if __name__ == "__main__":
    unittest.main()
