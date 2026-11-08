#!/usr/bin/env python3
"""Asset-free regression checks for verify_renderer_offline.py.

Creates synthetic copies of the *layout*, never redistributes game data.
Exercises the complete 28-case plane-mask matrix and identity failures.
"""
from __future__ import annotations

import hashlib
import unittest
from unittest.mock import patch

import verify_renderer_offline as runner


def fixture() -> bytes:
    # Include both the C029 mode byte and the later EF4D page byte.
    base = runner.OBJECT2_FILE_START
    raw = bytearray(base + 0xEF4D + 1)
    raw[base + 0xC029] = 1
    raw[base + 0xEF4D] = 0
    for phase in range(4):
        for count in range(1, 8):
            for pixel in range(count):
                x = phase + pixel
                at = base + runner.MASK_TABLE_OFFSETS[x // 4]
                raw[at + phase * 8 + count] |= 1 << (x & 3)
    return bytes(raw)


class Code2MaskTests(unittest.TestCase):
    def setUp(self) -> None:
        self.original = fixture()

    def verify(self, raw: bytes) -> int:
        # Certified identities are checked in production. For fixture
        # regressions patch identity fields, NOT the actual mask checker.
        with patch.object(runner, "CODE2_BYTES", len(self.original)), patch.object(
            runner, "CODE2_SHA256", hashlib.sha256(raw).hexdigest()
        ):
            return runner.verify_code2_masks(raw)

    def test_all_28_phase_width_combinations(self) -> None:
        self.assertEqual(self.verify(self.original), 28)

    def test_single_mask_corruption_fails(self) -> None:
        raw = bytearray(self.original)
        raw[runner.OBJECT2_FILE_START + 0xC668 + 1] ^= 1
        with self.assertRaisesRegex(ValueError, "mask mismatch"):
            self.verify(bytes(raw))

    def test_initial_mode_and_page_fail_closed(self) -> None:
        for offset, expected in ((0xC029, "dispatcher"), (0xEF4D, "page")):
            with self.subTest(offset=offset):
                raw = bytearray(self.original)
                raw[runner.OBJECT2_FILE_START + offset] ^= 1
                with self.assertRaisesRegex(ValueError, expected):
                    self.verify(bytes(raw))

    def test_truncation_and_identity_mismatch_fail_closed(self) -> None:
        with patch.object(runner, "CODE2_BYTES", len(self.original)), patch.object(
            runner, "CODE2_SHA256", hashlib.sha256(self.original).hexdigest()
        ):
            with self.assertRaisesRegex(ValueError, "certified"):
                runner.verify_code2_masks(self.original[:-1])
            raw = bytearray(self.original)
            raw[30] ^= 1
            with self.assertRaisesRegex(ValueError, "certified"):
                runner.verify_code2_masks(bytes(raw))


if __name__ == "__main__":
    unittest.main()
