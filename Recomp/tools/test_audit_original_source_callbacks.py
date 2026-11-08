#!/usr/bin/env python3
"""Unit regression of static source dependency census (no game media)."""
from __future__ import annotations
import tempfile
import unittest
from pathlib import Path
from audit_original_source_callbacks import GUEST_PATHS, catalog

class CallbackInventoryTests(unittest.TestCase):
    def setUp(self):
        self.tmp=tempfile.TemporaryDirectory()
        self.root=Path(self.tmp.name)
        (self.root/"Code2").mkdir()
        (self.root/"Code2/Code2.cpp").write_text(
            "void function_181b4() {} void function_1bdac() { function_181b4(); }\n",
            encoding="utf-8")
        for rel in GUEST_PATHS:
            p=self.root/rel
            p.parent.mkdir(parents=True,exist_ok=True)
            p.write_text("// source guest path\n",encoding="utf-8")
    def tearDown(self):
        self.tmp.cleanup()
    def test_reports_nullable_and_fail_closed_edges_without_pretending_completion(self):
        p=self.root/GUEST_PATHS[1]
        p.write_text(
            'void callbacks() {\n'
            '  scene.call_1bdac = ready ? &function_1bdac : nullptr;\n'
            '  scene.call_29738_stack4 = &source_save_clip;\n'
            '  stop("ORIGINAL_2C294_29738_CLIP_SAVE_PENDING");\n'
            '}\n',encoding="utf-8")
        d=catalog(self.root)
        self.assertEqual(d["canonical_function_address_tokens"],2)
        self.assertEqual(d["canonical_function_bodies"],2)
        self.assertEqual(d["canonical_direct_call_edges"],1)
        edges={entry["source_address"]:entry
               for entry in d["canonical_direct_call_graph"]}
        self.assertEqual(edges["1bdac"]["direct_callees"],["181b4"])
        self.assertEqual(d["live_guest_source_function_references"],1)
        self.assertEqual(d["live_guest_callback_wirings"],2)
        self.assertEqual(d["conditional_or_explicit_null_wirings"],1)
        self.assertEqual(d["unresolved_unique_guard_names"],1)
        self.assertEqual(d["fail_closed_guard_sites"][0]["line"],4)
        self.assertIn("dynamic",d["basis"])
    def test_missing_source_owner_rejected(self):
        (self.root/GUEST_PATHS[0]).unlink()
        with self.assertRaisesRegex(ValueError,"missing retained source"):
            catalog(self.root)
    def test_missing_canonical_source_rejected(self):
        (self.root/"Code2/Code2.cpp").unlink()
        with self.assertRaisesRegex(ValueError,"canonical reconstructed"):
            catalog(self.root)

if __name__=="__main__":
    unittest.main()
