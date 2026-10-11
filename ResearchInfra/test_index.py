"""Offline regression tests for ResearchInfra/index.py; no original game files."""
from __future__ import annotations
import json
import tempfile
import unittest
from pathlib import Path
from index import build, digest, extract, impact, lookup, packet, read_index, save, scrub, verify_orig


class ResearchIndexTests(unittest.TestCase):
    @staticmethod
    def fixture(root: Path) -> None:
        for name in ("Code1", "Code2"):
            (root / name).mkdir()
        (root / "Code1" / "Code1.cpp").write_text(
            'namespace code1 {\nvoid function_cc1c(){ function_1abc(); }\n'
            'void function_1abc(){ /* function_ffff() */ }\n}\n')
        (root / "Code2" / "Code2.cpp").write_text(
            'namespace code2 {\nvoid function_353a0(){ function_35418(); }\n'
            '[[nodiscard]] u32 function_35418() noexcept {'
            ' const char*x="function_dead()"; return function_395e2(); }\n'
            'u32 function_395e2(){ return 0; }\n'
            'void function_3a16e(){ function_abcd(); }\n}\n')

    def test_source_only_graph_and_unresolved(self):
        with tempfile.TemporaryDirectory() as p:
            root = Path(p)
            self.fixture(root)
            a = build(root)
            self.assertEqual(len(a["functions"]), 6)
            self.assertEqual(len(a["edges"]), 3)
            self.assertEqual(len(a["unresolved_calls"]), 1)
            self.assertEqual(lookup(a, "code2:35418")["callees"], ["code2:395e2"])
            self.assertEqual(impact(a, "code2:395e2")["potentially_affected_callers"],
                             ["code2:353a0", "code2:35418"])
            self.assertNotIn("dead", json.dumps(a))
            self.assertEqual(a["authority"], "source_structure_only_not_original_binary_or_DOS_acceptance")

    def test_determinism_and_provenance_rejection(self):
        with tempfile.TemporaryDirectory() as p:
            root = Path(p); self.fixture(root)
            a = build(root); b = build(root)
            self.assertEqual(json.dumps(a, sort_keys=True), json.dumps(b, sort_keys=True))
            path = root / "index.json"; save(a, path)
            self.assertEqual(read_index(path), a)
            self.assertFalse(packet(a, root, "code1:0cc1c")["truncated"])
            (root / "Code1" / "Code1.cpp").write_text("modified")
            with self.assertRaises(ValueError):
                packet(a, root, "code1:0cc1c")

    def test_unsigned_original_identity_fail_closed(self):
        with tempfile.TemporaryDirectory() as p:
            root = Path(p); (root / "CODE.2").write_bytes(b"not CODE.2")
            with self.assertRaises(ValueError):
                verify_orig(root, "code2")

    def test_overloaded_address_variants_are_preserved(self):
        defs = extract("void function_abcd(){} void function_abcd(){}", "code2", "x.cpp")
        self.assertEqual([f["id"] for f in defs],
                         ["code2:0abcd#variant1", "code2:0abcd#variant2"])
        self.assertTrue(all(f["address_alias_ambiguous"] for f in defs))

    def test_comments_strings_not_false_calls(self):
        s = 'void function_ab12(){// function_beef()\n"function_dead()"; function_ab13();}'
        a = extract(s, "code2", "t.cpp")
        self.assertEqual(a[0]["calls"], ["code2:0ab13"])

    def test_scrub_preserves_byte_positions_and_newlines(self):
        s = 'a/* b\n c */" q " //x\nz'
        m = scrub(s)
        self.assertEqual(len(s), len(m))
        self.assertEqual([i for i, c in enumerate(s) if c == "\n"],
                         [i for i, c in enumerate(m) if c == "\n"])


if __name__ == "__main__":
    unittest.main()
