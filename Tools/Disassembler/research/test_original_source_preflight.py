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
    original_geometry, read_bounded_original, simulate_mz_relocations,
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

    def test_direct_description_cannot_claim_a_foreign_sha(self):
        source = b"\x90\xc3"
        actual = hashlib.sha256(source).hexdigest()
        verified, _ = describe(source, actual.upper(), "com", "fixture")
        self.assertEqual(verified["original_sha256"], actual)
        with self.assertRaises(SourceValidationError):
            describe(source, hashlib.sha256(b"\xc3\x90").hexdigest(),
                     "com", "forged-source")
        with self.assertRaises(SourceValidationError):
            describe(source, "not-a-sha", "com", "forged-source")

    def test_assumed_mz_fixup_matches_independent_word_arithmetic(self):
        source = synthetic_mz(relocations=1)
        original_snapshot = bytes(source)
        copied = bytearray(source[32:36])
        # The synthetic MZ storage word is 0x9090. A DOS-style fixup adds
        # the CALLER-ASSUMED segment modulo 2**16, not a guessed runtime CS.
        struct.pack_into("<H", copied, 0, (0x9090 + 0x1234) & 0xFFFF)
        expected = hashlib.sha256(copied).hexdigest()
        result = simulate_mz_relocations(source, 0x1234)
        self.assertEqual(result["relocation_write_count"], 1)
        self.assertFalse(result["overlapping_relocation_writes"])
        self.assertEqual(result["assumed_load_segment"], 0x1234)
        self.assertEqual(result["simulated_file_backed_image_sha256"], expected)
        self.assertEqual(source, original_snapshot,
                         "independent simulation must not modify the source")

        wrapped = bytearray(source[32:36])
        struct.pack_into("<H", wrapped, 0, (0x9090 + 0xFFFF) & 0xFFFF)
        result_wrap = simulate_mz_relocations(source, 0xFFFF)
        self.assertEqual(result_wrap["simulated_file_backed_image_sha256"],
                         hashlib.sha256(wrapped).hexdigest())

        with self.assertRaises(SourceValidationError):
            simulate_mz_relocations(source, 0x10000)
        with self.assertRaises(SourceValidationError):
            simulate_mz_relocations(source, -1)
        with self.assertRaises(SourceValidationError):
            simulate_mz_relocations(source, True)

    def test_no_relocations_preserves_original_stored_mz_image(self):
        source = synthetic_mz(cs=1, overlay=b"\x19")
        result = simulate_mz_relocations(source, 0x2345)
        self.assertEqual(result["relocation_write_count"], 0)
        self.assertEqual(result["original_file_backed_image_bytes"], 20)
        self.assertEqual(result["simulated_file_backed_image_sha256"],
                         hashlib.sha256(source[32:52]).hexdigest())

    def test_overlapping_mz_fixups_report_overlap_and_table_order(self):
        # A fresh 48-byte MZ header with two fixups at adjacent image bytes.
        # Their 16-bit word writes overlap. Both are in-range, but the
        # simulator must not hide overlap or reorder their additions.
        header = bytearray(48)
        header[:2] = b"MZ"
        struct.pack_into("<H", header, 2, 52)
        struct.pack_into("<H", header, 4, 1)
        struct.pack_into("<H", header, 6, 2)
        struct.pack_into("<H", header, 8, 3)
        struct.pack_into("<H", header, 0x18, 0x1C)
        struct.pack_into("<HH", header, 0x1C, 0, 0)
        struct.pack_into("<HH", header, 0x20, 1, 0)
        source = bytes(header) + b"\x00" * 4
        simulation = simulate_mz_relocations(source, 1)
        self.assertEqual(simulation["relocation_write_count"], 2)
        self.assertTrue(simulation["overlapping_relocation_writes"])
        self.assertEqual(simulation["simulated_file_backed_image_sha256"],
                         hashlib.sha256(b"\x01\x01\x00\x00").hexdigest())

    def test_full_resident_digest_reference_matches_independent_dosdis_fixture(self):
        # Deliberately mirrors only a simple PUBLISHED synthetic shape,
        # not a DOSDis output. C# tests independently construct the same
        # 480-byte expected resident image. All offsets are MZ file based.
        single = bytearray(512)
        single[0:2] = b"MZ"
        struct.pack_into("<HHHH", single, 2, 0, 1, 1, 2)
        struct.pack_into("<H", single, 0x18, 0x1C)
        struct.pack_into("<HH", single, 0x1C, 1, 0)
        single[32:36] = b"\xB8\x34\x12\xC3"

        expected_single = bytearray(480)
        expected_single[:4] = b"\xB8\x34\x32\xC3"
        single_result = simulate_mz_relocations(bytes(single), 0x2000)
        self.assertEqual(single_result["original_file_backed_image_bytes"], 480)
        self.assertEqual(single_result["relocation_write_count"], 1)
        self.assertFalse(single_result["overlapping_relocation_writes"])
        self.assertEqual(single_result["simulated_file_backed_image_sha256"],
                         hashlib.sha256(expected_single).hexdigest())

        # Same original word twice, in table order, in a larger MZ header:
        # 1234h + 2000h + 2000h = 5234h, preserving the original 464
        # resident bytes count. This overlaps intentionally and is not
        # generalized to runtime loader scheduling.
        repeated = bytearray(512)
        repeated[0:2] = b"MZ"
        struct.pack_into("<HHHH", repeated, 2, 0, 1, 2, 3)
        struct.pack_into("<H", repeated, 0x18, 0x1C)
        struct.pack_into("<HH", repeated, 0x1C, 1, 0)
        struct.pack_into("<HH", repeated, 0x20, 1, 0)
        repeated[48:52] = b"\xB8\x34\x12\xC3"

        expected_repeated = bytearray(464)
        expected_repeated[:4] = b"\xB8\x34\x52\xC3"
        repeated_result = simulate_mz_relocations(bytes(repeated), 0x2000)
        self.assertEqual(repeated_result["original_file_backed_image_bytes"], 464)
        self.assertEqual(repeated_result["relocation_write_count"], 2)
        self.assertTrue(repeated_result["overlapping_relocation_writes"])
        self.assertEqual(repeated_result["simulated_file_backed_image_sha256"],
                         hashlib.sha256(expected_repeated).hexdigest())

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
