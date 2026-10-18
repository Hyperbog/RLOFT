"""Asset-free REA bridge contract tests; do not install or run REA in CI."""
from __future__ import annotations

import hashlib
import json
from pathlib import Path
import sys
import tempfile
import unittest
from unittest import mock

sys.path.insert(0, str(Path(__file__).resolve().parent))
import rea_bridge


class ReaBridgeTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.root = Path(self.temp.name) / "checkout"
        self.root.mkdir()
        (self.root / ".gitignore").write_text("/.rloft-private/\n", encoding="utf-8")
        self.private = self.root / ".rloft-private" / "rea"
        self.target = self.root / "game.exe"
        self.target.write_bytes(b"\x90\xc3\x00\x90")

    def test_plan_binds_digest_project_and_exact_raw_address(self):
        plan = rea_bridge.make_plan(
            self.target, "DosDis", "raw-binary",
            architecture="x86", entry_point=0x11234)
        self.assertEqual(plan["schema"], rea_bridge.SCHEMA)
        self.assertEqual(plan["source"]["sha256"], hashlib.sha256(self.target.read_bytes()).hexdigest())
        self.assertEqual(plan["mapped_file_start"], "0x11234")
        self.assertEqual(plan["command"][-6:],
                         ["--target-format", "raw-binary",
                          "--architecture", "x86", "--entry-point", "0x11234"])
        self.assertFalse(plan["engine_analysis_verified"])
        self.assertFalse(plan["dos_runtime_parity"])
        self.assertFalse(plan["native_gameplay_parity"])
        self.assertEqual(plan["rea_version_pin"], "6.4.0")

    def test_renamed_disassembler_uses_registered_project_identity(self):
        self.assertIn("DosDis", rea_bridge.projects())
        self.assertNotIn("disassembler-foundation", rea_bridge.projects())
        plan = rea_bridge.make_plan(self.target, "DosDis", "auto")
        self.assertEqual(plan["project"], "DosDis")

    def test_all_owner_workstreams_can_plan_without_owning_tool_source(self):
        for branch in sorted(rea_bridge.projects()) + ["external-owned-project"]:
            plan = rea_bridge.make_plan(self.target, branch, "auto")
            self.assertEqual(plan["project"], branch)
            self.assertEqual(plan["provider"], "ghidra")
            self.assertFalse(plan["original_byte_evidence"])

    def test_refuse_missing_raw_architecture_and_guessing_entry(self):
        for arch, entry in [(None, 0x10000), ("x86", None), ("mips", 0x10000),
                            ("x86", -1)]:
            with self.subTest(arch=arch, entry=entry):
                with self.assertRaises(ValueError):
                    rea_bridge.make_plan(self.target, "code1-decompilation",
                                         "raw-binary", architecture=arch,
                                         entry_point=entry)

    def test_refuse_nested_mz_le_lx_ne_as_dos(self):
        for signature in (b"LE", b"LX", b"NE"):
            data = bytearray(0x110)
            data[:2] = b"MZ"
            data[0x3c:0x40] = (0x100).to_bytes(4, "little")
            data[0x100:0x102] = signature
            self.target.write_bytes(data)
            with self.subTest(signature=signature):
                with self.assertRaisesRegex(ValueError, "does not support LE/LX/NE"):
                    rea_bridge.make_plan(self.target, "code1-decompilation", "auto")
        self.target.write_bytes(b"MZ" + b"\x00" * 100)
        self.assertEqual(rea_bridge.make_plan(
            self.target, "code1-decompilation", "auto")["format"], "auto")

    def test_private_output_guard_and_atomic_bytes(self):
        rea_bridge.private_directory(self.private, self.root)
        with self.assertRaisesRegex(ValueError, "private output"):
            rea_bridge.private_directory(self.root / "published", self.root)
        with self.assertRaisesRegex(ValueError, "private output"):
            rea_bridge.private_directory(self.root / ".rloft-private" / ".." / "leak",
                                         self.root)
        target = self.private / "case" / "bytes.bin"
        rea_bridge.private_write(target, b"\x90\xc3")
        self.assertEqual(target.read_bytes(), b"\x90\xc3")
        # Refuse traversal or symlink-based output outside ignored subtree.
        link = self.root / ".rloft-private" / "outside"
        link.symlink_to(self.root, target_is_directory=True)
        with self.assertRaisesRegex(ValueError, "private output"):
            rea_bridge.private_directory(link / "leak", self.root)

    def test_pinned_plan_rejects_mutation(self):
        plan = rea_bridge.make_plan(self.target, "native-engine", "auto")
        p = self.private / "plan.json"
        rea_bridge.private_write(p, (json.dumps(plan) + "\n").encode())
        self.assertEqual(rea_bridge.load_plan(p), plan)
        plan["provider"] = "unexpected"
        rea_bridge.private_write(p, json.dumps(plan).encode())
        with self.assertRaisesRegex(ValueError, "modified"):
            rea_bridge.load_plan(p)
        rea_bridge.private_write(p, json.dumps(rea_bridge.make_plan(
            self.target, "native-engine", "auto")).encode())
        self.target.write_bytes(b"edited-source")
        with self.assertRaisesRegex(ValueError, "changed"):
            rea_bridge.load_plan(p)

    def test_prepare_le_requires_full_sha_and_exports_exact_bounded_window(self):
        source = self.root / "CODE.1"
        source.write_bytes(b"auth-original-fixture")
        import original_cfg
        fake_code = bytes(range(256)) * 2
        fake_original = {
            "file": "CODE.1", "size": source.stat().st_size,
            "sha256": hashlib.sha256(source.read_bytes()).hexdigest(),
            "code_size": len(fake_code), "data_size": 100,
            "entry": 0x101,
        }
        args = type("Args", (), {
            "module": "code1", "original": source,
            "start": 0x80, "length": 0x40,
            "checkout": self.root, "output_dir": self.private,
            "project": "code1-decompilation",
        })()
        with mock.patch.dict(rea_bridge.ORIGINALS, {"code1": fake_original}):
            with mock.patch.object(rea_bridge, "original_object",
                                   return_value=(fake_code, {"entry": 0x101})):
                plan = rea_bridge.prepare_le(args)
                image = Path(plan["source"]["path"])
                self.assertEqual(image.read_bytes(), fake_code[0x80:0xc0])
                self.assertEqual(plan["mapped_file_start"], "0x10080")
                self.assertEqual(plan["provenance"]["code_object_offset"], "0x80")
                self.assertTrue(plan["original_byte_evidence"])
                self.assertEqual(json.loads(image.with_suffix(
                    ".plan.json").read_text())["source"]["sha256"],
                    hashlib.sha256(fake_code[0x80:0xc0]).hexdigest())
                args.length = len(fake_code)
                with self.assertRaisesRegex(ValueError, "code-object boundary"):
                    rea_bridge.prepare_le(args)
                args.length = 0x40
                source.write_bytes(b"changed-source-bytes")
                with self.assertRaisesRegex(ValueError, "full-file pinned"):
                    rea_bridge.prepare_le(args)

    def test_readonly_runner_explicit_cli_only_and_private_evidence(self):
        plan = rea_bridge.make_plan(self.target, "demo-v1", "auto")
        out = self.private / "result.json"
        with mock.patch("rea_bridge.shutil.which", return_value=None):
            with self.assertRaisesRegex(ValueError, "not installed"):
                rea_bridge.run_readonly(plan, out, "rea", 20, checkout=self.root)
        output = json.dumps({"evidence": "source-only", "original_parity": False}).encode()
        fake = subprocess_result(output)
        with mock.patch("rea_bridge.shutil.which", return_value="/usr/bin/rea"):
            with mock.patch("rea_bridge.subprocess.run", return_value=fake) as called:
                result = rea_bridge.run_readonly(plan, out, "rea", 30,
                                                 checkout=self.root)
        self.assertEqual(result["status"], "REA_STATIC_INSPECT_COMPLETED")
        self.assertFalse(result["original_dos_parity"])
        self.assertEqual(json.loads(out.read_text())["evidence"], "source-only")
        args = called.call_args.args[0]
        self.assertEqual(args[:2], ["/usr/bin/rea", "inspect"])
        self.assertNotIn("setup", args)
        self.assertNotIn("npx", args)


def subprocess_result(out: bytes):
    import subprocess
    return subprocess.CompletedProcess(args=["rea", "inspect"], returncode=0,
                                       stdout=out, stderr=b"")


if __name__ == "__main__":
    unittest.main()
