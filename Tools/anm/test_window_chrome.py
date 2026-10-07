#!/usr/bin/env python3
"""Regression tests for the custom frameless window resize geometry."""
from __future__ import annotations

import unittest

import window_chrome


class WindowChromeResizeTests(unittest.TestCase):
    def test_edge_hit_testing_without_overlay_widgets(self) -> None:
        hit = window_chrome.FramelessWindow._edge_hit_test
        self.assertEqual(hit(0, 350, 1000, 700), window_chrome.HTLEFT)
        self.assertEqual(hit(999, 350, 1000, 700), window_chrome.HTRIGHT)
        self.assertEqual(hit(500, 0, 1000, 700), window_chrome.HTTOP)
        self.assertEqual(hit(500, 699, 1000, 700), window_chrome.HTBOTTOM)
        self.assertEqual(hit(3, 3, 1000, 700), window_chrome.HTTOPLEFT)
        self.assertEqual(hit(997, 3, 1000, 700), window_chrome.HTTOPRIGHT)
        self.assertEqual(hit(3, 697, 1000, 700), window_chrome.HTBOTTOMLEFT)
        self.assertEqual(hit(997, 697, 1000, 700), window_chrome.HTBOTTOMRIGHT)
        self.assertIsNone(hit(500, 350, 1000, 700))

    def test_right_resize(self) -> None:
        self.assertEqual(
            window_chrome.FramelessWindow._resized_geometry(
                window_chrome.HTRIGHT, 120, 40, 100, 80, 900, 700, 860, 650
            ),
            (100, 80, 1020, 700),
        )

    def test_left_resize_moves_origin(self) -> None:
        self.assertEqual(
            window_chrome.FramelessWindow._resized_geometry(
                window_chrome.HTLEFT, 100, 0, 100, 80, 1000, 700, 860, 650
            ),
            (200, 80, 900, 700),
        )

    def test_top_left_corner_resize(self) -> None:
        self.assertEqual(
            window_chrome.FramelessWindow._resized_geometry(
                window_chrome.HTTOPLEFT, -40, -30, 100, 80, 900, 700, 860, 650
            ),
            (60, 50, 940, 730),
        )

    def test_minimum_width_clamps_left_edge_without_drifting_right_edge(self) -> None:
        self.assertEqual(
            window_chrome.FramelessWindow._resized_geometry(
                window_chrome.HTLEFT, 500, 0, 100, 80, 900, 700, 860, 650
            ),
            (140, 80, 860, 700),
        )

    def test_minimum_height_clamps_top_edge_without_drifting_bottom_edge(self) -> None:
        self.assertEqual(
            window_chrome.FramelessWindow._resized_geometry(
                window_chrome.HTTOP, 0, 500, 100, 80, 900, 700, 860, 650
            ),
            (100, 130, 900, 650),
        )


if __name__ == "__main__":
    unittest.main()
