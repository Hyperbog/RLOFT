#!/usr/bin/env python3
"""Regression tests for reconstructed ANM timing and sampled-SFX mapping."""
from __future__ import annotations

import json
import tempfile
import unittest
from pathlib import Path

import exporter
import master_dumper
import rloft_anm as anm


def frame(index: int, *chunks: anm.Chunk) -> anm.Frame:
    return anm.Frame(index, tuple(chunks))


class CinematicTimingTests(unittest.TestCase):
    def test_native_wait_uses_default_for_non_image_frame(self) -> None:
        movie = anm.AnmFile(
            2, 1,
            (
                frame(0, anm.Chunk(7, b"\x00\x00\x00\x00")),
                frame(1, anm.Chunk(8, bytes(anm.RAW_PLANAR_SIZE))),
            ),
        )
        self.assertEqual(exporter.native_wait_ticks(movie, "CINE00.ANM"), [1, 6])

    def test_cine12_image_path_uses_two_ticks(self) -> None:
        movie = anm.AnmFile(
            1, 1,
            (frame(0, anm.Chunk(3, b"\x00\x00")),),
        )
        self.assertEqual(exporter.native_wait_ticks(movie, "CINE12.ANM"), [2])

    def test_native_timing_is_irq0_based(self) -> None:
        movie = anm.AnmFile(
            1, 1,
            (frame(0, anm.Chunk(7, b"\x00\x00\x00\x00")),),
        )
        starts, durations = exporter.frame_start_times(movie, "CINE00.ANM", None)
        self.assertEqual(starts, [0.0])
        self.assertAlmostEqual(durations[0], 1.0 / exporter.PIT_HZ, places=9)

    def test_queued_sfx_uses_init_base_49(self) -> None:
        movie = anm.AnmFile(
            1, 1,
            (frame(0, anm.Chunk(6, (4).to_bytes(2, "little"))),),
        )
        events = exporter.cinematic_sfx_events(movie, "CINE00.ANM", None)
        self.assertEqual(len(events), 1)
        self.assertEqual(events[0].archive, 6)
        self.assertEqual(events[0].resource_index, 53)
        self.assertEqual(events[0].command, 6)

    def test_ignored_queued_sfx_ids_are_not_emitted(self) -> None:
        chunks = tuple(
            anm.Chunk(6, value.to_bytes(2, "little"))
            for value in (3, 15, 0x88)
        )
        movie = anm.AnmFile(1, 1, (frame(0, *chunks),))
        self.assertEqual(
            exporter.cinematic_sfx_events(movie, "CINE00.ANM", None),
            [],
        )

    def test_direct_sfx_uses_res10_index(self) -> None:
        movie = anm.AnmFile(
            1, 1,
            (frame(0, anm.Chunk(9, (42).to_bytes(2, "little"))),),
        )
        events = exporter.cinematic_sfx_events(movie, "CINE00.ANM", None)
        self.assertEqual(events[0].archive, 10)
        self.assertEqual(events[0].resource_index, 42)
        self.assertEqual(events[0].command, 9)

    def test_timeline_and_original_anm_can_export_together(self) -> None:
        # Minimal valid ANM: one logical frame with zero chunks.
        source_bytes = b"\x01\x00\x01\x00\x00\x00"
        with tempfile.TemporaryDirectory() as temp_name:
            root = Path(temp_name)
            source = root / "CINE00.ANM"
            source.write_bytes(source_bytes)
            destination = root / "out"
            options = master_dumper.DumpOptions(
                cinematic_frames=False,
                cinematic_videos=False,
                cinematic_timeline=True,
                preserve_native=True,
            )
            result = master_dumper.dump_cinematics(
                root,
                destination,
                options,
                names=("CINE00.ANM",),
            )
            copied = destination / "original_anm" / "CINE00.ANM"
            timeline = destination / "timelines" / "CINE00.json"
            self.assertEqual(copied.read_bytes(), source_bytes)
            self.assertTrue(timeline.is_file())
            manifest = json.loads(timeline.read_text(encoding="utf-8"))
            self.assertEqual(manifest["source"], "CINE00.ANM")
            self.assertEqual(result["cinematics"][0]["source"], "CINE00.ANM")


if __name__ == "__main__":
    unittest.main()
