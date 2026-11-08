#!/usr/bin/env python3
"""Synthetic-only regression for compare_indexed_captures.py."""
from __future__ import annotations

import tempfile
import unittest
from pathlib import Path

import compare_indexed_captures as c


def fixture(
    directory: Path,
    name: str,
    *,
    kind: str,
    level: int = 22,
    x: int = 4,
    y: int = 4,
    facing: int = 261,
    changed_pixel: bool = False,
    changed_palette: bool = False,
) -> None:
    directory.mkdir(parents=True, exist_ok=True)
    base = directory / name
    label = c.DOS_LABEL if kind == "dos" else c.NATIVE_LABEL
    fmt = "rloft-dos-indexed-capture-v1" if kind == "dos" else (
        "rloft-native-indexed-capture-v2"
    )
    base.with_suffix(".tsv").write_text(
        f"format\t{fmt}\n"
        f"provenance\t{label}\n"
        f"dimensions\t320\t400\n"
        f"palette\t256x3\tVGA-6bit-raw\n"
        f"level\t{level}\n"
        f"cell\t{x}\t{y}\n"
        f"facing\t{facing}\n",
        encoding="utf-8",
    )
    image = bytearray([17] * c.PIXEL_COUNT)
    palette = bytearray([12] * c.PALETTE_COUNT)
    if changed_pixel:
        image[320 + 3] = 19
    if changed_palette:
        palette[5] = 13
    base.with_suffix(".idx").write_bytes(image)
    base.with_suffix(".pal").write_bytes(palette)


class IndexedCaptureTests(unittest.TestCase):
    def setUp(self) -> None:
        self.temporary = tempfile.TemporaryDirectory()
        self.addCleanup(self.temporary.cleanup)
        root = Path(self.temporary.name)
        self.native = root / "native"
        self.dos = root / "dos"

    def test_lossless_match_and_provenance(self) -> None:
        fixture(self.native, "frame_0001", kind="native")
        fixture(self.dos, "frame_0001", kind="dos")
        report = c.compare(self.native, self.dos)
        self.assertEqual(report["comparison"], "native-vs-dos")
        self.assertEqual(report["matched_count"], 1)
        self.assertEqual(report["exact_matches"], 1)
        self.assertEqual(report["frames"][0]["pixel_mismatches"], 0)
        self.assertEqual(report["frames"][0]["palette_byte_mismatches"], 0)
        self.assertIsNone(report["frames"][0]["first_mismatch_xy"])

    def test_exact_mismatch_position_and_palette(self) -> None:
        fixture(self.native, "frame_0001", kind="native")
        fixture(self.dos, "frame_0001", kind="dos",
                changed_pixel=True, changed_palette=True)
        report = c.compare(self.native, self.dos)
        self.assertEqual(report["exact_matches"], 0)
        self.assertEqual(report["frames"][0]["pixel_mismatches"], 1)
        self.assertEqual(report["frames"][0]["palette_byte_mismatches"], 1)
        self.assertEqual(report["frames"][0]["first_mismatch_xy"], [3, 1])

    def test_mismatched_poses_cannot_be_comparison(self) -> None:
        fixture(self.native, "frame_0001", kind="native")
        fixture(self.dos, "frame_0001", kind="dos", facing=262)
        report = c.compare(self.native, self.dos)
        self.assertEqual(report["matched_count"], 0)
        self.assertEqual(len(report["unmatched_native"]), 1)
        self.assertEqual(len(report["unmatched_reference"]), 1)

    def test_no_native_capture_claimed_as_dos(self) -> None:
        fixture(self.native, "frame_0001", kind="native")
        fixture(self.dos, "frame_0001", kind="native")
        with self.assertRaisesRegex(ValueError, "provenance"):
            c.compare(self.native, self.dos)
        result = c.compare(self.native, self.dos, "native")
        self.assertEqual(result["comparison"], "native-vs-native")
        self.assertEqual(result["exact_matches"], 1)

    def test_duplicate_camera_pose_rejected(self) -> None:
        fixture(self.native, "frame_0001", kind="native")
        fixture(self.native, "frame_0002", kind="native")
        fixture(self.dos, "frame_0001", kind="dos")
        with self.assertRaisesRegex(ValueError, "duplicate pose"):
            c.compare(self.native, self.dos)

    def test_malformed_capture_rejected(self) -> None:
        fixture(self.native, "frame_0001", kind="native")
        fixture(self.dos, "frame_0001", kind="dos")
        (self.dos / "frame_0001.idx").write_bytes(b"not-a-frame")
        with self.assertRaisesRegex(ValueError, "byte length"):
            c.compare(self.native, self.dos)
        fixture(self.dos, "frame_0001", kind="dos")
        (self.dos / "frame_0001.pal").write_bytes(bytes([255] * 768))
        with self.assertRaisesRegex(ValueError, "6-bit"):
            c.compare(self.native, self.dos)


if __name__ == "__main__":
    unittest.main()
