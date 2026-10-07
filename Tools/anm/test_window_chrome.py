#!/usr/bin/env python3
"""Regression tests for the custom frameless window resize geometry."""
from __future__ import annotations

import unittest

import window_chrome


class WindowChromeResizeTests(unittest.TestCase):
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
