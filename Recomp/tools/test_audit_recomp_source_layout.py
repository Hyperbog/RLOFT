"""Negative and positive source-inventory contract tests; synthetic only."""
import importlib.util
import tempfile
import unittest
from pathlib import Path

SCRIPT = Path(__file__).with_name("audit_recomp_source_layout.py")
spec = importlib.util.spec_from_file_location("recomp_source_audit", SCRIPT)
module = importlib.util.module_from_spec(spec)
spec.loader.exec_module(module)


class LayoutAuditTest(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.root = Path(self.temp.name)
        base = self.root / "Recomp"
        (base / "src").mkdir(parents=True)
        (base / "tests").mkdir()
        (base / "CMakeLists.txt").write_text(
            'add_executable(example src/example.cpp)\n'
            'add_test(NAME example_case COMMAND example)\n', encoding="utf-8"
        )
        (base / "src/example.cpp").write_text(
            '#include "part.inc"\nint main(){return 0;}\n', encoding="utf-8"
        )
        (base / "src/part.inc").write_text("// source fragment\n", encoding="utf-8")

    def scan(self):
        return module.audit(self.root)

    def test_valid_source_graph(self):
        output = self.scan()
        self.assertEqual([], output["errors"])
        self.assertEqual(1, output["cmake_cpp_inputs"])
        self.assertEqual(1, output["compiled_fragments"])

    def test_unreferenced_cpp_fails(self):
        (self.root / "Recomp/src/unused.cpp").write_text("// unused")
        self.assertTrue(any("Unreferenced" in e for e in self.scan()["errors"]))

    def test_missing_cmake_cpp_fails(self):
        (self.root / "Recomp/src/example.cpp").unlink()
        self.assertTrue(any("missing C++" in e for e in self.scan()["errors"]))

    def test_orphan_and_missing_fragments_fail(self):
        base = self.root / "Recomp/src"
        (base / "orphan.inc").write_text("// orphan")
        self.assertTrue(any("Orphan" in e for e in self.scan()["errors"]))
        (base / "part.inc").unlink()
        self.assertTrue(any("Missing or escaped" in e for e in self.scan()["errors"]))

    def test_duplicate_cmake_target_and_test_fail(self):
        cmake = self.root / "Recomp/CMakeLists.txt"
        cmake.write_text(cmake.read_text() +
            "add_executable(example src/example.cpp)\n" +
            "add_test(NAME example_case COMMAND example)\n")
        issues = self.scan()["errors"]
        self.assertTrue(any("Duplicate executable" in e for e in issues))
        self.assertTrue(any("Duplicate CTest" in e for e in issues))

    def test_included_cmake_module_owns_compiled_cpp(self):
        base = self.root / "Recomp"
        (base / "cmake").mkdir()
        (base / "src/module.cpp").write_text("int module(){return 1;}\\n", encoding="utf-8")
        (base / "cmake/GameSubsystems.cmake").write_text(
            "add_library(game_module STATIC src/module.cpp)\\n", encoding="utf-8"
        )
        with (base / "CMakeLists.txt").open("a", encoding="utf-8") as fp:
            fp.write("include(cmake/GameSubsystems.cmake)\\n")
        result = self.scan()
        self.assertEqual([], result["errors"])
        self.assertEqual(2, result["cmake_cpp_inputs"])

    def test_missing_cmake_module_fails_closed(self):
        base = self.root / "Recomp"
        with (base / "CMakeLists.txt").open("a", encoding="utf-8") as fp:
            fp.write("include(cmake/DoesNotExist.cmake)\\n")
        self.assertTrue(any("Missing or escaped CMake module" in item
                            for item in self.scan()["errors"]))

    def test_compiled_fragment_cycle_fails(self):
        (self.root / "Recomp/src/part.inc").write_text('#include "part.inc"\n')
        self.assertTrue(any("Cyclic" in e for e in self.scan()["errors"]))


if __name__ == "__main__":
    unittest.main()
