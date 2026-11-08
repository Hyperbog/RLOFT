#!/usr/bin/env python3
"""Pure source-only unit tests; never needs original game media."""
from __future__ import annotations

import sys
from pathlib import Path
import tempfile
import unittest

sys.path.insert(0, str(Path(__file__).resolve().parent))
from summarize_win32_scan_trace import summarize  # noqa: E402


class ScanSummaryTests(unittest.TestCase):
    def test_aggregate_only_and_first_unresolved(self):
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "events.jsonl"
            path.write_text(
                '{"seq":1,"event":"key.down","a":65,"b":0}\n'
                '{"seq":2,"event":"key.scan_observation","a":72,"b":256}\n'
                '{"seq":3,"event":"key.scan_observation","a":72,"b":16777473}\n'
                '{"seq":4,"event":"key.scan_observation","a":30,"b":66050}\n'
                '{"seq":5,"event":"key.scan_observation","a":30,"b":256}\n',
                encoding="utf-8")
            result = summarize(path)
        self.assertEqual(result["observation_count"], 4)
        self.assertEqual(result["single_unextended_candidates"], 2)
        self.assertEqual(result["unresolved_count"], 2)
        self.assertEqual(result["first_unresolved_event_sequence"], 3)
        self.assertEqual(result["by_status"]["extended_sequence_unresolved"], 1)
        self.assertEqual(result["by_status"]["repeat_timing_unresolved"], 1)
        self.assertNotIn("scan", result)
        self.assertNotIn("72", str(result))

    def test_empty_event_trace_is_not_a_pass(self):
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "events.jsonl"
            path.write_text('{"seq":1,"event":"f12.marker.request","a":0,"b":0}\n')
            result = summarize(path)
        self.assertEqual(result["observation_count"], 0)
        self.assertEqual(result["assessment"], "diagnostic_metadata_only_not_original_DOS_parity")

    def test_rejects_invalid_status(self):
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "events.jsonl"
            path.write_text('{"seq":1,"event":"key.scan_observation","a":10,"b":255}\n')
            with self.assertRaises(ValueError):
                summarize(path)

    def test_rejects_out_of_order_observations(self):
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "events.jsonl"
            path.write_text(
                '{"seq":5,"event":"key.scan_observation","a":12,"b":256}\n'
                '{"seq":3,"event":"key.scan_observation","a":12,"b":256}\n')
            with self.assertRaises(ValueError):
                summarize(path)


    def test_finished_capture_not_a_dos_parity_verdict(self):
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "events.jsonl"
            path.write_text(
                '{"seq":1,"event":"session.start","a":0,"b":0}\n'
                '{"seq":2,"event":"key.scan_observation","a":72,"b":256}\n'
                '{"seq":3,"event":"session.finish","a":0,"b":0}\n',
                encoding="utf-8")
            result = summarize(path)
        self.assertEqual(result["capture_state"], "finished")
        self.assertEqual(result["trace_record_count"], 3)
        self.assertEqual(result["single_unextended_candidates"], 1)
        self.assertIn("not_original_DOS_parity", result["assessment"])

    def test_missing_finish_not_misreported_as_complete(self):
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "events.jsonl"
            path.write_text(
                '{"seq":1,"event":"session.start","a":0,"b":0}\n'
                '{"seq":2,"event":"key.scan_observation","a":30,"b":256}\n',
                encoding="utf-8")
            result = summarize(path)
        self.assertEqual(result["capture_state"], "missing_finish")
        self.assertEqual(result["trace_record_count"], 2)

    def test_event_limit_without_finish(self):
        from summarize_win32_scan_trace import TRACE_EVENT_LIMIT
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "events.jsonl"
            with path.open("w", encoding="utf-8") as handle:
                for i in range(1, TRACE_EVENT_LIMIT + 1):
                    handle.write(f'{{"seq":{i},"event":"noop"}}\n')
            result = summarize(path)
        self.assertEqual(result["capture_state"], "event_limit_reached")
        self.assertEqual(result["trace_record_count"], TRACE_EVENT_LIMIT)
        self.assertEqual(result["observation_count"], 0)

    def test_refuse_event_after_finish(self):
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "events.jsonl"
            path.write_text(
                '{"seq":1,"event":"session.finish"}\n'
                '{"seq":2,"event":"key.scan_observation","a":48,"b":256}\n',
                encoding="utf-8")
            with self.assertRaisesRegex(ValueError, "after session.finish"):
                summarize(path)

    def test_rejects_corrupted_candidate_and_status_flags(self):
        # A purported single nonextended event cannot also be E0-extended.
        corrupt = ((1 << 24) | 256, 0), (257, 1), (258, 2), (261, 5)
        for encoded, status in corrupt:
            with self.subTest(status=status):
                with tempfile.TemporaryDirectory() as directory:
                    path = Path(directory) / "events.jsonl"
                    path.write_text(
                        f'{{"seq":1,"event":"key.scan_observation","a":30,"b":{encoded}}}\n',
                        encoding="utf-8")
                    with self.assertRaises(ValueError):
                        summarize(path)

    def test_rejects_out_of_order_non_keyboard_event(self):
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "events.jsonl"
            path.write_text(
                '{"seq":4,"event":"key.scan_observation","a":72,"b":256}\n'
                '{"seq":3,"event":"f12.marker.request"}\n',
                encoding="utf-8")
            with self.assertRaises(ValueError):
                summarize(path)


if __name__ == "__main__":
    unittest.main()
