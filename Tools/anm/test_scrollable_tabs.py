#!/usr/bin/env python3
"""Non-display tests for scrollable tab behavior."""
import unittest

from scrollable_tabs import ScrollableNotebookPage


class ScrollTests(unittest.TestCase):
    def test_wheel_directions_and_small_deltas(self) -> None:
        wheel = ScrollableNotebookPage.wheel_units
        self.assertEqual(wheel(120), -1)
        self.assertEqual(wheel(-120), 1)
        self.assertEqual(wheel(240), -2)
        self.assertEqual(wheel(-240), 2)
        self.assertEqual(wheel(1), -1)
        self.assertEqual(wheel(-1), 1)
        self.assertEqual(wheel(0), 0)


if __name__ == "__main__":
    unittest.main()
