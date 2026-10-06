#!/usr/bin/env python3
"""Regression tests for cinematic option outputs and diagnostics."""
from __future__ import annotations

import json
import struct
import tempfile
import unittest
from pathlib import Path
from unittest import mock

import master_dumper


def minimal_anm() -> bytes:
    # frame_count=1, default_wait=1, frame chunk_count=0
    return struct.pack("<HHH", 1, 1, 0)


class CinematicOptionTests(unittest.TestCase):
    def test_timeline_and_original_are_independent_outputs(self) -> None:
        with tempfile.TemporaryDirectory() as temp:
            root = Path(temp) / "game"
            out = Path(temp) / "out"
            root.mkdir()
            source = root / "CINE00.ANM"
            source.write_bytes(minimal_anm())

            options = master_dumper.DumpOptions(
                cinematic_frames=False,
                cinematic_videos=False,
                cinematic_timeline=True,
                preserve_native=True,
            )
            result = master_dumper.dump_cinematics(
                root,
                out,
                options,
                names=("CINE00.ANM",),
            )

            copied = out / "original_anm" / "CINE00.ANM"
            timeline = out / "timelines" / "CINE00.json"
            manifest = out / "cinematics_manifest.json"

            self.assertEqual(copied.read_bytes(), source.read_bytes())
            self.assertTrue(timeline.is_file())
            self.assertTrue(manifest.is_file())
            payload = json.loads(timeline.read_text(encoding="utf-8"))
            self.assertEqual(payload["source"], "CINE00.ANM")
            self.assertEqual(payload["frame_count"], 1)
            self.assertEqual(result["cinematics"][0]["original_anm"], str(copied))
            self.assertEqual(result["cinematics"][0]["timeline_json"], str(timeline))

    def test_disabled_timeline_and_original_create_neither(self) -> None:
        with tempfile.TemporaryDirectory() as temp:
            root = Path(temp) / "game"
            out = Path(temp) / "out"
            root.mkdir()
            (root / "CINE00.ANM").write_bytes(minimal_anm())

            options = master_dumper.DumpOptions(
                cinematic_frames=False,
                cinematic_videos=False,
                cinematic_timeline=False,
                preserve_native=False,
            )
            result = master_dumper.dump_cinematics(
                root,
                out,
                options,
                names=("CINE00.ANM",),
            )

            self.assertFalse((out / "original_anm").exists())
            self.assertFalse((out / "timelines" / "CINE00.json").exists())
            record = result["cinematics"][0]
            self.assertIsNone(record["original_anm"])
            self.assertIsNone(record["timeline_json"])

    def test_video_audio_sync_failure_identifies_stage(self) -> None:
        with tempfile.TemporaryDirectory() as temp:
            root = Path(temp) / "game"
            out = Path(temp) / "out"
            root.mkdir()
            (root / "CINE00.ANM").write_bytes(minimal_anm())

            options = master_dumper.DumpOptions(
                cinematic_frames=False,
                cinematic_videos=True,
                cinematic_audio=True,
                cinematic_timeline=False,
                preserve_native=False,
            )
            with mock.patch(
                "master_dumper.exporter.export_video",
                side_effect=RuntimeError("synthetic mux failure"),
            ):
                with self.assertRaisesRegex(
                    RuntimeError,
                    r"CINE00.ANM: video/SFX sync failed: synthetic mux failure",
                ):
                    master_dumper.dump_cinematics(
                        root,
                        out,
                        options,
                        names=("CINE00.ANM",),
                    )


if __name__ == "__main__":
    unittest.main()
