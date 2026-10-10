"""Offline regression: fail-closed local native-research preflight."""
from pathlib import Path
import subprocess
import shutil
import tempfile
import unittest

from research_preflight import REQUIRED_DOCS, declarations, preflight


class NativeResearchPreflightTests(unittest.TestCase):
    def test_multiline_and_comment_count(self):
        src = "# add_test(NAME fake)\nadd_test(\n NAME actual COMMAND actual)\n" \
              "add_test(NAME other COMMAND other)\n"
        data = declarations(src)
        self.assertEqual(data["source_declarations"], 2)
        self.assertEqual(data["unique_names"], 2)
        self.assertFalse(data["duplicate_names"])

    def test_duplicate_test_names_fail(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            self.make_checkout(root, "add_test(NAME same COMMAND same)\n"
                               "add_test(NAME same COMMAND another)\n")
            data, status = preflight(root, {})
            self.assertEqual(status, 2)
            self.assertIn("duplicate CMake test names", data["checks"])

    def test_missing_original_fails(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            self.make_checkout(root, "add_test(NAME okay COMMAND okay)\n")
            data, status = preflight(root, {"CODE.2": root / "absent"})
            self.assertEqual(status, 2)
            self.assertIn("original file could not be read: CODE.2", data["checks"])

    def test_wrong_original_hash_fails(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            self.make_checkout(root, "add_test(NAME okay COMMAND okay)\n")
            original = root / "CODE.2"
            original.write_bytes(b"wrong version")
            data, status = preflight(root, {"CODE.2": original})
            self.assertEqual(status, 2)
            self.assertFalse(data["originals"]["CODE.2"]["matches_known_uploaded_version"])

    def test_clean_git_checkout_passes_without_claiming_windows_acceptance(self):
        if shutil.which("git") is None:
            self.skipTest("git not installed")
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            self.make_checkout(root, "add_test(NAME okay COMMAND okay)\n")
            subprocess.run(["git", "-C", str(root), "init", "-q"], check=True)
            subprocess.run(["git", "-C", str(root), "add", "."], check=True)
            subprocess.run(["git", "-C", str(root), "-c", "user.name=Test",
                            "-c", "user.email=test@example.org", "commit", "-qm", "fixture"],
                           check=True)
            current = subprocess.check_output(["git", "-C", str(root),
                                              "rev-parse", "HEAD"], text=True).strip()
            data, status = preflight(root, {}, expected_head=current, require_clean=True)
            self.assertEqual(status, 0, data)
            self.assertTrue(data["git"]["working_tree_clean"])
            self.assertEqual(data["acceptance"]["current_Windows_Full"], "NOT CHECKED")

    def test_stale_head_fails(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            self.make_checkout(root, "add_test(NAME okay COMMAND okay)\n")
            data, status = preflight(root, {}, expected_head="deadbeef")
            self.assertEqual(status, 2)
            self.assertIn("expected Git HEAD mismatch or unavailable", data["checks"])

    @staticmethod
    def make_checkout(root: Path, cmake: str):
        for name in REQUIRED_DOCS:
            path = root / name
            path.parent.mkdir(parents=True, exist_ok=True)
            path.write_text("preserved continuity\n", encoding="utf-8")
        (root / "DemoV1/CMakeLists.txt").write_text(cmake, encoding="utf-8")


if __name__ == "__main__":
    unittest.main()
