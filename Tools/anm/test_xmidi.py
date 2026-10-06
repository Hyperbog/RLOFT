#!/usr/bin/env python3
"""Regression tests for Miles XMIDI -> Standard MIDI conversion."""
from __future__ import annotations

import unittest

import xmidi


def chunk(tag: bytes, payload: bytes) -> bytes:
    body = tag + len(payload).to_bytes(4, "big") + payload
    if len(payload) & 1:
        body += b"\x00"
    return body


def form_xmid(evnt: bytes) -> bytes:
    body = b"XMID" + chunk(b"EVNT", evnt)
    return b"FORM" + len(body).to_bytes(4, "big") + body


class XmidiTests(unittest.TestCase):
    def test_exact_120hz_smpte_division(self) -> None:
        # Program change at tick 1, EOT at tick 3.
        source = form_xmid(bytes([1, 0xC0, 5, 2, 0xFF, 0x2F, 0]))
        midi_files, infos = xmidi.convert_all(source)
        self.assertEqual(len(midi_files), 1)
        midi = midi_files[0]
        self.assertEqual(midi[:4], b"MThd")
        self.assertEqual(int.from_bytes(midi[12:14], "big"), 0xE204)
        self.assertEqual(infos[0].end_tick, 3)

    def test_note_duration_emits_note_off(self) -> None:
        # Note-on at tick 0, duration 2; EOT at tick 2.
        source = form_xmid(bytes([
            0x90, 60, 100, 2,
            2, 0xFF, 0x2F, 0,
        ]))
        midi, info = xmidi.convert_all(source)
        track = midi[0][22:]
        self.assertIn(bytes([0x80, 60, 0]), track)
        self.assertEqual(info[0].end_tick, 2)

    def test_zero_duration_still_lasts_one_interval(self) -> None:
        # Miles XMIDI.ASM stores duration-1 and expires once it becomes negative.
        source = form_xmid(bytes([
            0x90, 64, 90, 0,
            1, 0xFF, 0x2F, 0,
        ]))
        midi, _ = xmidi.convert_all(source)
        track = midi[0][22:]
        self.assertIn(bytes([1, 0x80, 64, 0]), track)

    def test_ail_loop_controls_are_retained_and_counted(self) -> None:
        source = form_xmid(bytes([
            0xB0, 116, 0,
            4, 0xB0, 117, 127,
            1, 0xFF, 0x2F, 0,
        ]))
        midi, info = xmidi.convert_all(source)
        track = midi[0][22:]
        self.assertIn(bytes([0xB0, 116, 0]), track)
        self.assertIn(bytes([0xB0, 117, 127]), track)
        self.assertEqual(info[0].loop_for_count, 1)
        self.assertEqual(info[0].loop_next_count, 1)


if __name__ == "__main__":
    unittest.main()
