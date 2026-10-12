#!/usr/bin/env python3
"""Offline regression for original-media-free diagnostic archive integrity."""
from __future__ import annotations

import json
from pathlib import Path
import sys
import tempfile
import unittest
from zipfile import ZipFile, ZIP_DEFLATED

sys.path.insert(0, str(Path(__file__).resolve().parent))
from audit_mplay01_trace import audit, fnv64  # noqa: E402

SHA = "a" * 40


def write_session(directory: Path, *, marker: bool = True) -> None:
    manifest = {
        "format": "rloft-level22-diagnostic-v1",
        "provenance": "native-preview-not-original-DOS",
        "source_revision": SHA,
        "media_kind": "installed-original",
        "framebuffer": "320x400-indexed",
        "palette": "256x3-raw-6bit",
        "cursor": "host-overlay-not-in-indexed-frame",
        "events_max": 16384,
        "snapshots_max": 96,
        "markers_max": 16,
    }
    (directory / "manifest.json").write_text(json.dumps(manifest))
    event_names = [
        {"seq": 1, "event": "session.start", "a": 0, "b": 0},
        {"seq": 2, "event": "frame.capture", "a": 1, "b": 22},
    ]
    if marker:
        event_names.extend([
            {"seq": 3, "event": "f12.marker.request", "a": 0, "b": 0},
            {"seq": 4, "event": "frame.marker", "a": 1, "b": 22},
        ])
    event_names.append({"seq": len(event_names) + 1,
                        "event": "session.finish", "a": 1, "b": int(marker)})
    (directory / "events.jsonl").write_text(
        "\n".join(json.dumps(v) for v in event_names) + "\n")
    for kind in ("frame", "marker") if marker else ("frame",):
        stem = f"{kind}_0001"
        indexed = bytes((i % 256 for i in range(128000)))
        palette = bytes((i % 64 for i in range(768)))
        (directory / f"{stem}.idx").write_bytes(indexed)
        (directory / f"{stem}.pal").write_bytes(palette)
        metadata = {"format": "rloft-trace-frame-v1",
                    "screen": "viewport", "irq": 20,
                    "level": 22, "hash_pixels": fnv64(indexed),
                    "hash_palette": fnv64(palette)}
        (directory / f"{stem}.json").write_text(json.dumps(metadata))
        (directory / f"{stem}.draws.tsv").write_text(
            "order\top\tframe\tx\ty\twidth\theight\tparameter_a\tparameter_b\n"
            "omitted\t0\n")
        if kind == "marker":
            (directory / f"{stem}_recent.jsonl").write_text(
                json.dumps(event_names[2]) + "\n")


class TraceAuditTests(unittest.TestCase):
    def setUp(self) -> None:
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.root = Path(self.temp.name)
        self.session = self.root / "session"
        self.session.mkdir()
        write_session(self.session)

    def test_complete_frame_and_marker(self) -> None:
        result = audit(self.session, SHA)
        self.assertEqual(result["capture_state"], "finished")
        self.assertEqual(result["frame_count"], 1)
        self.assertEqual(result["marker_count"], 1)
        self.assertEqual(result["last_capture"]["level"], 22)
        self.assertTrue(result["expected_source_verified"])
        self.assertIn("not_normal_gameplay", result["assessment"])

    def test_zipped_session_with_folder_prefix(self) -> None:
        destination = self.root / "trace.zip"
        with ZipFile(destination, "w", ZIP_DEFLATED) as zip_file:
            for p in self.session.iterdir():
                zip_file.write(p, "private-session/" + p.name)
        result = audit(destination)
        self.assertEqual((result["frame_count"], result["marker_count"]), (1, 1))
        self.assertFalse(result["expected_source_verified"])

    def test_fails_closed_when_pixels_change(self) -> None:
        p = self.session / "frame_0001.idx"
        bytes_ = bytearray(p.read_bytes())
        bytes_[777] ^= 1
        p.write_bytes(bytes_)
        with self.assertRaisesRegex(ValueError, "hash mismatch"):
            audit(self.session)

    def test_fails_closed_when_palette_changes(self) -> None:
        p = self.session / "marker_0001.pal"
        data = bytearray(p.read_bytes())
        data[4] ^= 1
        p.write_bytes(data)
        with self.assertRaisesRegex(ValueError, "hash mismatch"):
            audit(self.session)

    def test_fails_closed_on_dangling_marker(self) -> None:
        (self.session / "marker_0001_recent.jsonl").unlink()
        with self.assertRaisesRegex(ValueError, "recent-event"):
            audit(self.session)

    def test_fails_closed_on_event_reordering(self) -> None:
        path = self.session / "events.jsonl"
        text = path.read_text().replace('"seq": 4', '"seq": 1')
        path.write_text(text)
        with self.assertRaisesRegex(ValueError, "out-of-order"):
            audit(self.session)

    def test_reports_interrupted_session_without_fake_pass(self) -> None:
        path = self.session / "events.jsonl"
        records = path.read_text().splitlines()
        path.write_text("\n".join(records[:-1]) + "\n")
        result = audit(self.session)
        self.assertEqual(result["capture_state"], "missing_finish")
        self.assertIn("not_normal_gameplay", result["assessment"])

    def test_fails_closed_on_unmatched_sha(self) -> None:
        with self.assertRaisesRegex(ValueError, "differs"):
            audit(self.session, "f" * 40)

    def test_rejects_archive_traversal(self) -> None:
        destination = self.root / "hostile.zip"
        with ZipFile(destination, "w", ZIP_DEFLATED) as archive:
            for p in self.session.iterdir():
                archive.write(p, "session/" + p.name)
            archive.writestr("session/../escape", "bad")
        with self.assertRaisesRegex(ValueError, "unsafe"):
            audit(destination)

    def test_rejects_wrong_manifest_format(self) -> None:
        path = self.session / "manifest.json"
        obj = json.loads(path.read_text())
        obj["format"] = "not-RLOFT"
        path.write_text(json.dumps(obj))
        with self.assertRaisesRegex(ValueError, "unsupported"):
            audit(self.session)

    def test_rejects_more_than_six_bit_palette(self) -> None:
        path = self.session / "frame_0001.pal"
        data = bytearray(path.read_bytes())
        data[0] = 255
        path.write_bytes(data)
        with self.assertRaisesRegex(ValueError, "6-bit"):
            audit(self.session)

    def test_rejects_event_orphan_frame(self) -> None:
        path = self.session / "frame_0001.json"
        obj = json.loads(path.read_text())
        obj["level"] = 23
        path.write_text(json.dumps(obj))
        with self.assertRaisesRegex(ValueError, "disagrees"):
            audit(self.session)


if __name__ == "__main__":
    unittest.main()
