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
        # The logical resize zone is intentionally wider than the painted
        # rail so high-DPI users can actually grab the border.
        self.assertEqual(hit(7, 350, 1000, 700), window_chrome.HTLEFT)
        self.assertEqual(hit(992, 350, 1000, 700), window_chrome.HTRIGHT)
        self.assertEqual(hit(500, 7, 1000, 700), window_chrome.HTTOP)
        self.assertEqual(hit(500, 692, 1000, 700), window_chrome.HTBOTTOM)
        self.assertIsNone(hit(20, 350, 1000, 700))
        self.assertIsNone(hit(500, 350, 1000, 700))

    def test_maximized_titlebar_click_does_not_restore(self) -> None:
        class Root:
            def winfo_x(self): return 0
            def winfo_y(self): return 0

        class Event:
            x_root = 500
            y_root = 10
            x = 500
            y = 10

        chrome = object.__new__(window_chrome.FramelessWindow)
        chrome.root = Root()
        chrome._resize_hit = None
        chrome._resize_origin = None
        chrome._move_origin = None
        chrome._maximized_drag_origin = None
        chrome._edge_hit_from_event = lambda _event: None
        chrome.is_maximized = lambda: True

        toggled = []
        chrome.toggle_maximize = lambda: toggled.append(True)

        self.assertEqual(chrome._begin_move(Event()), "break")
        self.assertEqual(toggled, [])
        self.assertEqual(chrome._maximized_drag_origin, (500, 10, 500, 10))
        chrome._end_move(Event())
        self.assertIsNone(chrome._maximized_drag_origin)

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
