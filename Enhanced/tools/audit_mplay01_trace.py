#!/usr/bin/env python3
"""Validate a native M-PLAY-01 diagnostic ZIP/folder without extracting it.

This is host-side evidence validation. A valid recording is NOT evidence of
ordinary playthrough or original DOS presentation/input equivalence.
"""
from __future__ import annotations

import argparse
from collections import Counter
import json
from pathlib import Path, PurePosixPath
import re
import zipfile

MAX_FILES = 20000
MAX_ARCHIVE_BYTES = 256 * 1024 * 1024
MAX_EVENTS_BYTES = 8 * 1024 * 1024
EXPECTED_FORMAT = "rloft-level22-diagnostic-v1"
EXPECTED_FRAME_FORMAT = "rloft-trace-frame-v1"
FRAME_RE = re.compile(r"(frame|marker)_(\d{4})\.json\Z")
SHA_RE = re.compile(r"[a-fA-F0-9]{40}\Z")


def fnv64(data: bytes) -> int:
    value = 14695981039346656037
    for byte in data:
        value = ((value ^ byte) * 1099511628211) & ((1 << 64) - 1)
    return value


class TraceFiles:
    def __init__(self, source: Path):
        self.archive = None
        self.directory = None
        self.prefix = ""
        if source.is_dir():
            self.directory = source
            return
        self.archive = zipfile.ZipFile(source, "r")
        members = self.archive.infolist()
        if len(members) > MAX_FILES:
            raise ValueError("too many diagnostic archive entries")
        if sum(m.file_size for m in members) > MAX_ARCHIVE_BYTES:
            raise ValueError("diagnostic archive exceeds uncompressed budget")
        names = []
        for m in members:
            name = m.filename
            parts = PurePosixPath(name).parts
            if (name.startswith("/") or "\\" in name or not parts or
                    any(p in (".", "..") for p in parts) or
                    (m.external_attr >> 16) & 0o170000 == 0o120000):
                raise ValueError("unsafe diagnostic archive member")
            if not m.is_dir():
                names.append(name)
        if len(names) != len(set(names)):
            raise ValueError("duplicate diagnostic archive members")
        if "manifest.json" in names:
            self.prefix = ""
        else:
            candidates = [n[:-len("manifest.json")] for n in names
                          if n.endswith("/manifest.json")]
            if len(candidates) != 1:
                raise ValueError("expected exactly one diagnostic manifest")
            self.prefix = candidates[0]
        if any(not name.startswith(self.prefix) for name in names):
            raise ValueError("mixed diagnostic archive roots")

    def list_names(self) -> list[str]:
        if self.archive is not None:
            return [m.filename[len(self.prefix):]
                    for m in self.archive.infolist()
                    if not m.is_dir()]
        assert self.directory is not None
        return [p.name for p in self.directory.iterdir() if p.is_file()]

    def read(self, name: str, maximum: int) -> bytes:
        if not name or "/" in name or "\\" in name or name in (".", ".."):
            raise ValueError("invalid diagnostic filename")
        if self.archive is not None:
            info = self.archive.getinfo(self.prefix + name)
            if info.file_size > maximum:
                raise ValueError(f"oversized diagnostic record: {name}")
            with self.archive.open(info, "r") as handle:
                data = handle.read(maximum + 1)
        else:
            assert self.directory is not None
            path = self.directory / name
            if path.is_symlink() or not path.is_file() or path.stat().st_size > maximum:
                raise ValueError(f"missing or unsafe diagnostic file: {name}")
            data = path.read_bytes()
        if len(data) > maximum:
            raise ValueError(f"oversized diagnostic record: {name}")
        return data

    def close(self) -> None:
        if self.archive is not None:
            self.archive.close()


def integer(record: dict, name: str) -> int:
    v = record.get(name)
    if type(v) is not int or v < 0:
        raise ValueError(f"invalid {name} metadata")
    return v


def audit(source: Path, expected_sha: str | None = None) -> dict:
    if expected_sha is not None and not SHA_RE.fullmatch(expected_sha):
        raise ValueError("expected SHA must be a complete Git commit identifier")
    files = TraceFiles(source)
    try:
        names = set(files.list_names())
        manifest = json.loads(files.read("manifest.json", 16384))
        if (manifest.get("format") != EXPECTED_FORMAT or
                manifest.get("provenance") != "native-preview-not-original-DOS" or
                manifest.get("framebuffer") != "320x400-indexed" or
                manifest.get("palette") != "256x3-raw-6bit"):
            raise ValueError("unsupported native diagnostic manifest")
        revision = manifest.get("source_revision")
        if not isinstance(revision, str) or not revision:
            raise ValueError("missing native source revision")
        if expected_sha is not None and revision.lower() != expected_sha.lower():
            raise ValueError("diagnostic checkout differs from expected source SHA")
        limits = {"events_max": 16384, "snapshots_max": 96, "markers_max": 16}
        for name, value in limits.items():
            if integer(manifest, name) != value:
                raise ValueError(f"unsupported {name} recorder limit")

        events = []
        seq = 0
        finished = False
        started = False
        finished_counts = None
        errors = Counter()
        captures = Counter()
        for lineno, raw in enumerate(files.read("events.jsonl", MAX_EVENTS_BYTES).splitlines(), 1):
            if not raw.strip():
                continue
            event = json.loads(raw)
            if not isinstance(event, dict) or not isinstance(event.get("event"), str):
                raise ValueError(f"invalid event record at line {lineno}")
            n = integer(event, "seq")
            if n != seq + 1 or finished:
                raise ValueError("out-of-order, missing, or post-finish event")
            seq = n
            tag = event["event"]
            if n == 1:
                if tag != "session.start":
                    raise ValueError("missing session.start")
                started = True
            elif tag == "session.start":
                raise ValueError("duplicate session.start")
            if tag == "session.finish":
                finished_counts = (integer(event, "a"), integer(event, "b"))
                finished = True
            if tag == "error":
                stage = event.get("stage")
                if not isinstance(stage, str):
                    raise ValueError("invalid error stage")
                # Keep user paths and arbitrary runtime exception details private.
                errors["redacted" if "/" in stage or "\\" in stage else stage[:64]] += 1
            if tag in ("frame.capture", "frame.marker"):
                number = integer(event, "a")
                level = integer(event, "b")
                kind = "marker" if tag == "frame.marker" else "frame"
                if number < 1 or number > (16 if kind == "marker" else 96):
                    raise ValueError("invalid frame event index")
                captures[(kind, number)] += 1
                if captures[(kind, number)] != 1:
                    raise ValueError("duplicate frame event")
                events.append((kind, number, level))
        if len(events) > 112 or not started or seq > 16384:
            raise ValueError("invalid diagnostic event count")
        actual_counts = (sum(k == "frame" for k, _, _ in events),
                         sum(k == "marker" for k, _, _ in events))
        if finished and finished_counts != actual_counts:
            raise ValueError("session.finish capture counts disagree")
        if len(files.read("events.jsonl", MAX_EVENTS_BYTES).splitlines()) > 16384:
            raise ValueError("diagnostic event limit exceeded")

        discovered = set()
        screens = Counter()
        last = None
        for name in names:
            match = FRAME_RE.fullmatch(name)
            if not match:
                continue
            kind, number_text = match.groups()
            number = int(number_text)
            if number < 1 or number > (16 if kind == "marker" else 96):
                raise ValueError("out-of-range frame file index")
            discovered.add((kind, number))
        if discovered != set(captures):
            raise ValueError("frame metadata/event membership mismatch")
        for kind, number, expected_level in events:
            stem = f"{kind}_{number:04}"
            metadata = json.loads(files.read(stem + ".json", 16384))
            if (metadata.get("format") != EXPECTED_FRAME_FORMAT or
                    integer(metadata, "level") != expected_level):
                raise ValueError("frame type or level disagrees with event")
            indexed = files.read(stem + ".idx", 128000)
            palette = files.read(stem + ".pal", 768)
            if len(indexed) != 128000 or len(palette) != 768:
                raise ValueError("invalid indexed frame or 6-bit palette size")
            if any(channel > 63 for channel in palette):
                raise ValueError("palette contains non-6-bit channel")
            if (integer(metadata, "hash_pixels") != fnv64(indexed) or
                    integer(metadata, "hash_palette") != fnv64(palette)):
                raise ValueError("indexed frame/palette hash mismatch")
            if stem + ".draws.tsv" not in names:
                raise ValueError("frame missing draw-order audit")
            if kind == "marker" and stem + "_recent.jsonl" not in names:
                raise ValueError("marker missing recent-event witness")
            screens[metadata.get("screen", "unknown")] += 1
            last = {"kind": kind, "index": number,
                    "screen": metadata.get("screen"),
                    "level": expected_level, "irq": integer(metadata, "irq")}
        return {
            "source_revision": revision,
            "expected_source_verified": expected_sha is not None,
            "record_count": seq,
            "capture_state": ("finished" if finished else
                              "event_limit_reached" if seq >= 16384 else "missing_finish"),
            "frame_count": sum(kind == "frame" for kind, _, _ in events),
            "marker_count": sum(kind == "marker" for kind, _, _ in events),
            "screens": dict(sorted(screens.items())),
            "error_stages": dict(sorted(errors.items())),
            "last_capture": last,
            "assessment": "native_trace_integrity_only_not_normal_gameplay_or_DOS_parity",
        }
    finally:
        files.close()


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("session", type=Path, help="Diagnostic ZIP or session folder")
    parser.add_argument("--expected-sha", help="Exact 40-character source Git SHA")
    args = parser.parse_args()
    print(json.dumps(audit(args.session, args.expected_sha), sort_keys=True, indent=2))


if __name__ == "__main__":
    main()
