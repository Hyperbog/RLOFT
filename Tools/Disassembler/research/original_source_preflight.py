#!/usr/bin/env python3
"""Independent, read-only DOS original-source identity and entry-map preflight.

No DOSDis/Iced dependency; no original bytes, paths, instructions or relocated
memory are exported. This is source geometry/correlation, NOT a decoder,
executed-code determination, certified MZ/COM loader, or G2 acceptance.
"""
from __future__ import annotations

import argparse
import hashlib
import json
import re
import struct
from pathlib import Path

MAX_SOURCE_BYTES = 128 * 1024 * 1024
MAX_ENTRY_PREFIX = 4096


class SourceValidationError(ValueError):
    pass


def _u16(data: bytes, offset: int) -> int:
    if offset < 0 or offset + 2 > len(data):
        raise SourceValidationError("truncated DOS header or relocation table")
    return struct.unpack_from("<H", data, offset)[0]


def _sha(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()


def read_bounded_original(path: Path, expected_sha256: str) -> bytes:
    if re.fullmatch(r"[0-9A-Fa-f]{64}", expected_sha256) is None:
        raise SourceValidationError("expected full source SHA256 must be 64 hex digits")
    try:
        size = path.stat().st_size
        if size < 1 or size > MAX_SOURCE_BYTES:
            raise SourceValidationError("original source length outside bounded preflight")
        data = path.read_bytes()
    except OSError as exc:
        raise SourceValidationError("cannot read original source") from exc
    if len(data) != size or len(data) > MAX_SOURCE_BYTES:
        raise SourceValidationError("source changed or exceeds limit during read")
    if _sha(data) != expected_sha256.lower():
        raise SourceValidationError("original source SHA256 mismatch")
    return data


def original_geometry(data: bytes, mode: str) -> dict:
    """Return only ORIGINAL byte-backed geometry, never runtime load state."""
    if mode == "com":
        if data[:2] == b"MZ":
            raise SourceValidationError("COM mode rejects MZ source signatures")
        if not (1 <= len(data) <= 0xFF00):
            raise SourceValidationError("COM does not fit explicit origin 0100h")
        return {
            "format": "explicit-headerless-COM",
            "origin": 256,
            "entry_ip": 256,
            "source_entry_offset": 0,
            "mapped_region_file_offset": 0,
            "mapped_region_bytes": len(data),
            "entry_backed_bytes": len(data),
            "relocation_count": None,
            "relocation_unique_sites": None,
            "relocation_overlaps": None,
            "trailing_bytes": 0,
        }
    if mode != "mz":
        raise SourceValidationError("unsupported explicit DOS source mode")
    if len(data) < 0x1C or data[:2] != b"MZ":
        raise SourceValidationError("expected original DOS MZ header")
    rem, pages, relocs, paragraphs = (_u16(data, at) for at in (2, 4, 6, 8))
    cs, ip, table = (_u16(data, at) for at in (0x16, 0x14, 0x18))
    if pages < 1 or rem > 511 or paragraphs < 2:
        raise SourceValidationError("invalid MZ declared pages or header geometry")
    declared = (pages - 1) * 512 + (rem or 512)
    header = paragraphs * 16
    if header > declared or declared > len(data):
        raise SourceValidationError("MZ declared disk extent exceeds source or header")
    if relocs and (table < 0x1C or table + relocs * 4 > header):
        raise SourceValidationError("MZ relocation directory outside declared header")
    load = declared - header
    entry_linear = cs * 16 + ip
    if entry_linear >= load or cs * 16 >= load:
        raise SourceValidationError("MZ entry CS:IP not backed by stored load bytes")
    starts = []
    for index in range(relocs):
        off = _u16(data, table + index * 4)
        seg = _u16(data, table + index * 4 + 2)
        linear = seg * 16 + off
        if linear + 2 > load:
            raise SourceValidationError("MZ relocation write leaves stored load image")
        starts.append(linear)
    # Two writes one byte apart overlap even if their starting offsets differ.
    overlapping = sum(
        1 for a, b in zip(sorted(starts), sorted(starts)[1:]) if b - a < 2
    )
    region_start = header + cs * 16
    region_len = min(0x10000, load - cs * 16)
    return {
        "format": "outer-DOS-MZ-stored-image-only",
        "declared_executable_bytes": declared,
        "header_bytes": header,
        "file_backed_load_bytes": load,
        "initial_relative_cs": cs,
        "entry_ip": ip,
        "source_entry_offset": header + entry_linear,
        "mapped_region_file_offset": region_start,
        "mapped_region_bytes": region_len,
        "entry_backed_bytes": region_len - ip,
        "relocation_count": relocs,
        "relocation_directory_start": table if relocs else None,
        "relocation_directory_end_exclusive": table + relocs * 4 if relocs else None,
        "relocation_unique_sites": len(set(starts)),
        "relocation_overlaps": overlapping,
        "trailing_bytes": len(data) - declared,
    }


def describe(data: bytes, expected_sha: str, mode: str, label: str) -> tuple[dict, dict]:
    geom = original_geometry(data, mode)
    begin = geom["mapped_region_file_offset"]
    end = begin + geom["mapped_region_bytes"]
    return {
        "label": label,  # caller-supplied neutral identifier; never file path
        "original_sha256": expected_sha.lower(),
        "original_bytes": len(data),
        "mapped_region_sha256": _sha(data[begin:end]),
        "geometry": geom,
        "accuracy_status": "source-identity-and-geometry-only",
    }, geom


def compare_entries(first: bytes, a: dict, second: bytes, b: dict, limit: int) -> dict:
    p, q = a["source_entry_offset"], b["source_entry_offset"]
    cap = min(a["entry_backed_bytes"], b["entry_backed_bytes"], limit)
    matched = 0
    while matched < cap and first[p + matched] == second[q + matched]:
        matched += 1
    return {
        "matching_original_entry_prefix_bytes": matched,
        "comparison_bytes_limit": cap,
        "first_different_byte_observed": matched < cap,
        "sample_semantics": "unclassified-source-bytes-not-instructions",
        "candidate_independence": "same-prefix-must-not-be-double-counted",
    }


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--source", required=True, type=Path)
    parser.add_argument("--source-sha256", required=True)
    parser.add_argument("--mode", choices=("mz", "com"), required=True)
    parser.add_argument("--label", default="source-a")
    parser.add_argument("--peer", type=Path)
    parser.add_argument("--peer-sha256")
    parser.add_argument("--peer-mode", choices=("mz", "com"))
    parser.add_argument("--peer-label", default="source-b")
    parser.add_argument("--entry-prefix-limit", type=int, default=512)
    args = parser.parse_args()
    if not 1 <= args.entry_prefix_limit <= MAX_ENTRY_PREFIX:
        parser.error("entry prefix limit must be 1..4096 bytes")
    if (args.peer is None) != (args.peer_sha256 is None) or (
        (args.peer is None) != (args.peer_mode is None)
    ):
        parser.error("--peer, --peer-sha256 and --peer-mode must appear together")
    if any(not re.fullmatch(r"[A-Za-z0-9_.-]{1,48}", label)
           for label in (args.label, args.peer_label)):
        parser.error("source labels must be short neutral identifiers")
    try:
        original = read_bounded_original(args.source, args.source_sha256)
        item_a, geom_a = describe(
            original, args.source_sha256, args.mode, args.label
        )
        result = {
            "schema_version": 1,
            "evidence": "read-only-original-source-preflight",
            "independent_decoder_or_execution": False,
            "originals": [item_a],
            "shared_entry_prefix": None,
            "g2_accuracy_status": "not-certified",
        }
        if args.peer is not None:
            peer = read_bounded_original(args.peer, args.peer_sha256)
            item_b, geom_b = describe(
                peer, args.peer_sha256, args.peer_mode, args.peer_label
            )
            result["originals"].append(item_b)
            result["shared_entry_prefix"] = compare_entries(
                original, geom_a, peer, geom_b, args.entry_prefix_limit
            )
    except SourceValidationError as exc:
        # Deliberately avoid printing private source paths or any bytes.
        parser.error(str(exc))
    print(json.dumps(result, indent=2, sort_keys=True))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
