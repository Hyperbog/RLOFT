"""Synthetic-only fixtures for the independent DOS MZ/COM source preflight.

Do not import, download, or execute any proprietary game binary here.
This suite is source-authored and must not be claimed as run until a
separately authorized test gate records the actual interpreter/results.
"""
import hashlib
import struct
import tempfile
import unittest
from pathlib import Path

from original_source_preflight import (
    SourceValidationError, compare_entries, describe,
    original_geometry, read_bounded_original,
)


def synthetic_mz(*, cs=0, relocations=0, overlay=b""):
    # Exact 32-byte header, source-backed entry at image-relative CS:0000.
    header = bytearray(32)
    header[0:2] = b"MZ"
    code_at = cs * 16
    image = bytearray(code_at + 4)
    image[code_at:code_at + 4] = b"\x90\x90\xeb\xfe"
    declared = len(header) + len(image)
    struct.pack_into("<H", header, 2, declared)
    struct.pack_into("<H", header, 4, 1)
    struct.pack_into("<H", header, 6, relocations)
    struct.pack_into("<H", header, 8, 2)
    struct.pack_into("<H", header, 0x16, cs)
    if relocations:
        struct.pack_into("<H", header, 0x18, 0x1C)
        struct.pack_into("<HH", header, 0x1C, code_at, 0)
    return bytes(header + image + overlay)


class OriginalSourcePreflightSmoke(unittest.TestCase):
    def test_different_mz_containers_same_original_mapped_bytes(self):
        first = synthetic_mz(cs=0, overlay=b"\x11")
        second = synthetic_mz(cs=1, overlay=b"\x22")
        a, ga = describe(first, hashlib.sha256(first).hexdigest(), "mz", "mz-a")
        b, gb = describe(second, hashlib.sha256(second).hexdigest(), "mz", "mz-b")
        self.assertNotEqual(a["original_sha256"], b["original_sha256"])
        self.assertNotEqual(ga["source_entry_offset"], gb["source_entry_offset"])
        self.assertNotEqual(ga["initial_relative_cs"], gb["initial_relative_cs"])
        self.assertEqual(a["mapped_region_sha256"], b["mapped_region_sha256"])
        self.assertEqual(
            compare_entries(first, ga, second, gb, 16)
            ["matching_original_entry_prefix_bytes"], 4
        )
        self.assertEqual(ga["trailing_bytes"], 1)
        self.assertEqual(gb["trailing_bytes"], 1)

    def test_relocations_are_storage_only_and_bounded(self):
        source = synthetic_mz(relocations=1)
        geometry = original_geometry(source, "mz")
        self.assertEqual(geometry["relocation_count"], 1)
        self.assertEqual(geometry["relocation_unique_sites"], 1)
        self.assertEqual(geometry["relocation_overlaps"], 0)
        self.assertEqual(geometry["relocation_directory_start"], 0x1C)
        self.assertEqual(geometry["relocation_directory_end_exclusive"], 0x20)
        bad = bytearray(source)
        struct.pack_into("<HH", bad, 0x1C, 0xFFFF, 0)
        with self.assertRaises(SourceValidationError):
            original_geometry(bytes(bad), "mz")

    def test_com_mode_requires_explicit_source_kind(self):
        source = b"\xe9\x00\x00\xc3"
        geometry = original_geometry(source, "com")
        self.assertEqual(geometry["origin"], 0x100)
        self.assertEqual(geometry["source_entry_offset"], 0)
        with self.assertRaises(SourceValidationError):
            original_geometry(source, "mz")
        with self.assertRaises(SourceValidationError):
            original_geometry(b"MZ" + source, "com")

    def test_sha_mismatch_never_falls_back_to_name_or_size(self):
        with tempfile.TemporaryDirectory(prefix="dosdis-source-preflight-synthetic-") as d:
            path = Path(d) / "synthetic.com"
            path.write_bytes(b"\x90\xc3")
            source = read_bounded_original(
                path, hashlib.sha256(b"\x90\xc3").hexdigest()
            )
            self.assertEqual(source, b"\x90\xc3")
            with self.assertRaises(SourceValidationError):
                read_bounded_original(
                    path, hashlib.sha256(b"\x90\x90").hexdigest()
                )

    def test_prefix_divergence_is_not_classified_as_instructions(self):
        first = b"\xe9\x00\x00" + b"\x90" * 280 + b"\x01" + b"\x90"
        second = b"\xe9\x00\x00" + b"\x90" * 280 + b"\x02" + b"\x90"
        a, b = (original_geometry(blob, "com") for blob in (first, second))
        comparison = compare_entries(first, a, second, b, 512)
        self.assertEqual(comparison["matching_original_entry_prefix_bytes"], 283)
        self.assertTrue(comparison["first_different_byte_observed"])
        self.assertEqual(
            comparison["sample_semantics"],
            "unclassified-source-bytes-not-instructions",
        )


if __name__ == "__main__":
    unittest.main()
