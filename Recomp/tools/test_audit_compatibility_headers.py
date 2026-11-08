"""Media-free compatibility-forward census and reference safety tests."""
import importlib.util
import tempfile
import unittest
from pathlib import Path

PATH = Path(__file__).with_name("audit_compatibility_headers.py")
spec = importlib.util.spec_from_file_location("compat_header_audit", PATH)
module = importlib.util.module_from_spec(spec)
spec.loader.exec_module(module)


class CompatibilityHeadersTest(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.root = Path(self.temp.name)
        self.src = self.root / "Recomp" / "src"
        self.src.mkdir(parents=True)
        (self.src / "game" / "audio").mkdir(parents=True)
        (self.src / "game" / "audio" / "Audio.hpp").write_text(
            "#pragma once\n", encoding="utf-8")
        self.forward = self.src / "old_audio.hpp"
        self.forward.write_text(
            '#pragma once\n// backward compatibility\n'
            '#include "game/audio/Audio.hpp"\n', encoding="utf-8")

    def scan(self):
        return module.inventory(self.root)

    def test_unreferenced_forward_is_candidate(self):
        result = self.scan()
        self.assertEqual(["Recomp/src/old_audio.hpp"],
                         result["proposed_deletion_candidates"])
        self.assertEqual(1, len(result["forwarders"]))

    def test_source_include_blocks_deletion(self):
        (self.src / "reader.cpp").write_text(
            '#include "old_audio.hpp"\n', encoding="utf-8")
        result = self.scan()
        self.assertEqual([], result["proposed_deletion_candidates"])
        self.assertEqual(["Recomp/src/reader.cpp"],
                         result["forwarders"][0]["source_includes"])

    def test_build_script_mentions_block_deletion(self):
        (self.root / "CMakeLists.txt").write_text(
            'set(DEPRECATED_HEADER old_audio.hpp)\n', encoding="utf-8")
        result = self.scan()
        self.assertEqual([], result["proposed_deletion_candidates"])
        self.assertIn("CMakeLists.txt",
                      result["forwarders"][0]["other_mentions"])

    def test_comment_is_not_cpp_include(self):
        (self.src / "reader.cpp").write_text(
            '// old_audio.hpp may be deleted\n', encoding="utf-8")
        self.assertEqual(["Recomp/src/old_audio.hpp"],
                         self.scan()["proposed_deletion_candidates"])

    def test_not_an_owned_forward_is_not_candidate(self):
        self.forward.write_text(
            "#pragma once\nint decoder();\n", encoding="utf-8")
        self.assertEqual([], self.scan()["forwarders"])

    def test_nested_source_file_counts_as_reference(self):
        nested = self.src / "game" / "core"
        nested.mkdir()
        (nested / "engine.inc").write_text(
            '#include "../old_audio.hpp"\n', encoding="utf-8")
        result = self.scan()
        self.assertEqual([], result["proposed_deletion_candidates"])


if __name__ == "__main__":
    unittest.main()
