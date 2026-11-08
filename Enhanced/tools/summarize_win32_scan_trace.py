#!/usr/bin/env python3
"""Summarize opt-in native Win32 scan observations without key-text output.

Reads a DevSessionTrace events.jsonl file. Diagnostic host metadata is not
original DOS port-60 evidence, a user keyboard capture, or a parity verdict.
"""
from __future__ import annotations

import argparse
from collections import Counter
import json
from pathlib import Path

TRACE_EVENT_LIMIT = 16384  # DevSessionTrace::kMaxEvents, not a DOS IRQ limit

STATUSES = (
    "single_unextended_candidate",
    "extended_sequence_unresolved",
    "repeat_timing_unresolved",
    "invalid_scan",
    "invalid_transition",
    "not_keyboard_message",
)


def summarize(path: Path) -> dict:
    counts: Counter[str] = Counter()
    count = 0
    first_unresolved = None
    last_sequence = -1
    logged_records = 0
    saw_finish = False
    with path.open(encoding="utf-8") as handle:
        for lineno, line in enumerate(handle, 1):
            if not line.strip():
                continue
            event = json.loads(line)
            if not isinstance(event, dict):
                raise ValueError(f"record {lineno}: expected JSON object")
            if saw_finish:
                raise ValueError(f"record {lineno}: event after session.finish")
            logged_records += 1
            seq = event.get("seq")
            if not isinstance(seq, int) or isinstance(seq, bool) or seq <= last_sequence:
                raise ValueError(f"record {lineno}: invalid observation sequence")
            last_sequence = seq
            if event.get("event") == "session.finish":
                saw_finish = True
            if event.get("event") != "key.scan_observation":
                continue
            a, packed = event.get("a"), event.get("b")
            if (not isinstance(a, int) or isinstance(a, bool) or not 0 <= a <= 255 or
                not isinstance(packed, int) or isinstance(packed, bool) or not 0 <= packed <= 0x1FFFFFFF):
                raise ValueError(f"record {lineno}: invalid scan metadata")
            status = packed & 0xFF
            if status >= len(STATUSES):
                raise ValueError(f"record {lineno}: unknown observation status")
            repeat = (packed >> 8) & 0xFFFF
            extended = bool(packed & (1 << 24))
            previous = bool(packed & (1 << 26))
            key_up = bool(packed & (1 << 27))
            if status == 5:
                raise ValueError(f"record {lineno}: not-keyboard status in keyboard-only log")
            if status == 0 and (
                not 1 <= a <= 127 or repeat != 1 or extended or
                (previous and not key_up)
            ):
                raise ValueError(f"record {lineno}: invalid candidate metadata")
            if status == 1 and not extended:
                raise ValueError(f"record {lineno}: extended status without marker")
            if status == 2 and (repeat == 1 and (not previous or key_up)):
                raise ValueError(f"record {lineno}: repeat status without repeat evidence")
            count += 1
            label = STATUSES[status]
            counts[label] += 1
            # Do not emit the per-key scan code, sequence of keys or virtual-key IDs.
            if status != 0 and first_unresolved is None:
                first_unresolved = seq
    capture_state = (
        "finished" if saw_finish else
        "event_limit_reached" if logged_records >= TRACE_EVENT_LIMIT else
        "missing_finish"
    )
    return {
        "capture_state": capture_state,
        "trace_record_count": logged_records,
        "observation_count": count,
        "single_unextended_candidates": counts[STATUSES[0]],
        "unresolved_count": count - counts[STATUSES[0]],
        "first_unresolved_event_sequence": first_unresolved,
        "by_status": {label: counts[label] for label in STATUSES},
        "assessment": "diagnostic_metadata_only_not_original_DOS_parity",
    }


def main() -> None:
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument("events_jsonl", type=Path, help="Opt-in DevSessionTrace/events.jsonl")
    args = p.parse_args()
    print(json.dumps(summarize(args.events_jsonl), sort_keys=True, indent=2))


if __name__ == "__main__":
    main()
