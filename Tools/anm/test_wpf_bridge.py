"""The WPF backend must delegate to existing asset decoders verbatim."""
from __future__ import annotations

import io
import json
import tempfile
import unittest
from contextlib import redirect_stdout
from pathlib import Path
from unittest.mock import patch

import wpf_bridge


class BridgeTests(unittest.TestCase):
    def test_protocol_self_test(self):
        stream = io.StringIO()
        with redirect_stdout(stream), patch("sys.argv", ["wpf_bridge.py", "--self-test"]):
            self.assertEqual(wpf_bridge.main(), 0)
        result = json.loads(stream.getvalue().strip())
        self.assertEqual(result["type"], "done")
        self.assertEqual(result["result"]["protocol"], 1)

    def test_grouped_export_keeps_parameters(self):
        with patch.object(wpf_bridge.asset_dumper, "dump_graphics", return_value={}) as called:
            wpf_bridge.graphics(Path("source"), Path("out"), {
                "graphics_format": "PNG (lossless indexed)",
                "graphics_organization": "Grouped only",
                "palette_index": 4,
                "graphics_archives": [3],
                "group_families": ["npcs", "effects"]
            })
        args = called.call_args
        self.assertEqual(args.kwargs["palette_index"], 4)
        self.assertEqual(args.kwargs["archives"], {3})
        self.assertEqual(args.kwargs["group_families"], {"npcs", "effects"})
        self.assertEqual(args.kwargs["organization"], "Grouped only")

    def test_support_subset_skips_missing_files(self):
        with tempfile.TemporaryDirectory() as temp:
            source = Path(temp)
            (source / "COLORS").write_bytes(b"stub")
            with patch.object(
                wpf_bridge.support_dumper, "dump_colors",
                return_value={"palette_count": 15}
            ) as extract:
                result = wpf_bridge.support(source, source / "out", {
                    "support_colors": True,
                    "support_text": False,
                    "support_sample": False,
                    "support_banks": False,
                    "support_init": False,
                    "support_sintab": False
                })
            self.assertEqual(result["palettes"]["palette_count"], 15)
            extract.assert_called_once()
            self.assertEqual(len(result), 1)

    def test_cinematic_originals_option_is_independent_of_audio(self):
        with patch.object(wpf_bridge.master_dumper, "dump_cinematics",
                          return_value={"cinematics": []}) as called:
            wpf_bridge.cinematics(Path("source"), Path("out"), {
                "preserve_native": True,
                "cinematic_preserve_native": False,
            })
        self.assertFalse(called.call_args.args[2].preserve_native)

    def test_duplicate_small_progress_is_throttled(self):
        wpf_bridge._last_progress.clear()
        with patch.object(wpf_bridge, "emit") as emit:
            with patch.object(
                wpf_bridge.time, "monotonic", side_effect=(1.0, 1.01, 1.02, 1.03)
            ):
                wpf_bridge.progress("graphics", 0, 200, "Start")
                wpf_bridge.progress("graphics", 2, 200, "First")
                wpf_bridge.progress("graphics", 2, 200, "Duplicate")
                wpf_bridge.progress("graphics", 200, 200, "Done")
            self.assertEqual(emit.call_count, 3)


if __name__ == "__main__":
    unittest.main()
