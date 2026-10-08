"""Static contract checks for the native WPF/Fluent frontend."""
from __future__ import annotations

import re
import unittest
from pathlib import Path
from xml.etree import ElementTree


BASE = Path(__file__).resolve().parents[1] / "WpfExtractor" / "RavenloftExtractor"
X = "{http://schemas.microsoft.com/winfx/2006/xaml/presentation}"
XN = "{http://schemas.microsoft.com/winfx/2006/xaml}"


class WpfLayoutTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.app = ElementTree.parse(BASE / "App.xaml").getroot()
        cls.window = ElementTree.parse(BASE / "MainWindow.xaml").getroot()
        cls.source = (BASE / "MainWindow.xaml.cs").read_text(encoding="utf-8")
        cls.named = {
            item.get(XN + "Name"): item
            for item in cls.window.iter()
            if item.get(XN + "Name")
        }

    def test_uses_builtin_windows11_fluent_theme(self):
        app = (BASE / "App.xaml").read_text(encoding="utf-8")
        self.assertIn("PresentationFramework.Fluent;component", app)
        self.assertIn("Fluent.Dark.xaml", app)

    def test_real_resize_border_not_custom_fake_window(self):
        self.assertEqual(self.window.get("WindowStyle"), "SingleBorderWindow")
        self.assertEqual(self.window.get("ResizeMode"), "CanResize")
        self.assertNotIn("AllowsTransparency", self.window.attrib)
        self.assertIn("DwmSetWindowAttribute", self.source)
        self.assertNotIn("WindowChrome", self.source)

    def test_scrollable_pages_with_fixed_controls(self):
        root = next(iter(self.window))
        self.assertEqual(root.tag, X + "Grid")
        self.assertIn("SettingsScroller", self.named)
        self.assertIn("RunButton", self.named)
        self.assertIn("CancelButton", self.named)
        self.assertIn("WorkProgress", self.named)
        scroll = self.named["SettingsScroller"]
        self.assertEqual(scroll.get("VerticalScrollBarVisibility"), "Auto")
        self.assertEqual(scroll.get("Grid.Row"), "1")

    def test_every_named_page_has_a_navigation_entry(self):
        for section in ("Source", "Graphics", "Audio", "Cinematics", "Support", "All"):
            self.assertIn("Page" + section, self.named)
            self.assertIn("Nav" + section, self.named)

    def test_all_xaml_event_handlers_have_methods(self):
        xml = (BASE / "MainWindow.xaml").read_text(encoding="utf-8")
        handlers = re.findall(r'\b(?:Click|Checked|Unchecked)="(\w+)"', xml)
        self.assertGreater(len(handlers), 4)
        for method in handlers:
            self.assertIn(method + "(", self.source)

    def test_preservation_and_group_controls(self):
        for key in (
            "GraphicsOrganization", "GroupNpcs", "GroupObjects",
            "GroupEffects", "GroupScenes", "GroupSheets", "GroupPreviews",
            "GroupAliases", "GraphicsIndices", "GraphicsNative",
            "CinematicFrames", "CinematicVideos", "AudioPreserve"
        ):
            self.assertIn(key, self.named)
        self.assertIn('"group_families"', self.source)
        self.assertIn('"graphics_organization"', self.source)

    def test_original_logo_is_part_of_application(self):
        self.assertTrue((BASE / "Assets" / "ravenloft_logo.png").is_file())
        self.assertIn("/Assets/ravenloft_logo.png", (BASE / "MainWindow.xaml").read_text(encoding="utf-8"))
        self.assertIn("Assets\\ravenloft_logo.png", (BASE / "RavenloftExtractor.csproj").read_text(encoding="utf-8"))


if __name__ == "__main__":
    unittest.main()
