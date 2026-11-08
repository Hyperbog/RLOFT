#!/usr/bin/env python3
"""Exact indexed VGA comparison with a separately captured original DOS frame.

This program never creates a DOS reference or treats synthetic frames as evidence.
Input .idx is 320x400 bytes (8-bit palette indices, row-major top-down).
Optional .pal is 256 RGB6 triplets in DAC index order. Neither is a BMP.
"""
from __future__ import annotations

import argparse
import hashlib
import json
from pathlib import Path
import sys

WIDTH = 320
HEIGHT = 400
INDEX_BYTES = WIDTH * HEIGHT
DAC_BYTES = 256 * 3


def data(path: Path, expected: int) -> bytes:
    content = path.read_bytes()
    if len(content) != expected:
        raise ValueError(f"Invalid byte length for {path.name}: {len(content)}, expected {expected}")
    return content


def compare(reference: bytes, reconstructed: bytes, width: int) -> dict:
    if len(reference) != len(reconstructed):
        raise ValueError("Unequal buffer lengths")
    mismatch = 0
    first = None
    changed_rows: set[int] = set()
    changed_values: set[tuple[int, int]] = set()
    for i, (a, b) in enumerate(zip(reference, reconstructed)):
        if a == b:
            continue
        mismatch += 1
        if first is None:
            first = {"x": i % width, "y": i // width,
                     "reference_value": a, "reconstructed_value": b}
        changed_rows.add(i // width)
        changed_values.add((a, b))
    return {
        "equal": mismatch == 0,
        "mismatched_bytes": mismatch,
        "first_mismatch": first,
        "affected_rows": len(changed_rows),
        "unique_byte_transitions": len(changed_values),
    }


def validate(reference_idx: Path, native_idx: Path,
             reference_pal: Path | None = None,
             native_pal: Path | None = None) -> dict:
    if (reference_pal is None) != (native_pal is None):
        raise ValueError("Original-DOS and reconstructed palette files must be supplied together")
    original = data(reference_idx, INDEX_BYTES)
    native = data(native_idx, INDEX_BYTES)
    result = {
        "format": "rloft-indexed-vga-exact-comparison-v1",
        "resolution": [WIDTH, HEIGHT],
        "frame_bytes": INDEX_BYTES,
        "reference_sha256": hashlib.sha256(original).hexdigest(),
        "reconstructed_sha256": hashlib.sha256(native).hexdigest(),
        "indices": compare(original, native, WIDTH),
        "palette_rgb6": None,
        "pixel_and_palette_equal": False,
        "reference_provenance_verified": False,
        "note": "File content comparison cannot independently authenticate original DOS execution.",
    }
    if reference_pal is not None and native_pal is not None:
        original_dac = data(reference_pal, DAC_BYTES)
        native_dac = data(native_pal, DAC_BYTES)
        if any(channel > 63 for channel in original_dac + native_dac):
            raise ValueError("RGB6 palette contains channel above 63")
        result["palette_rgb6"] = compare(original_dac, native_dac, 3)
        result["palette_reference_sha256"] = hashlib.sha256(original_dac).hexdigest()
        result["palette_reconstructed_sha256"] = hashlib.sha256(native_dac).hexdigest()
        result["pixel_and_palette_equal"] = (
            result["indices"]["equal"] and result["palette_rgb6"]["equal"])
    return result


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--dos-index", type=Path, required=True,
                        help="Independent DOSBox-X or real-DOS capture; never generated from the recomp")
    parser.add_argument("--native-index", type=Path, required=True)
    parser.add_argument("--dos-palette", type=Path, default=None,
                        help="Optional raw 256 x RGB6 original DOS DAC table")
    parser.add_argument("--native-palette", type=Path, default=None)
    parser.add_argument("--report", type=Path, default=None,
                        help="Optional JSON receipt (contains hashes, no original media)")
    args = parser.parse_args()
    try:
        result = validate(args.dos_index, args.native_index,
                          args.dos_palette, args.native_palette)
    except (OSError, ValueError) as exc:
        print(f"DOS_INDEXED_FRAME_COMPARISON_FAIL: {exc}", file=sys.stderr)
        return 2
    receipt = json.dumps(result, indent=2, sort_keys=True) + "\n"
    print(receipt)
    if args.report is not None:
        args.report.write_text(receipt, encoding="utf-8")
    if not result["indices"]["equal"]:
        return 1
    if result["palette_rgb6"] is not None and not result["palette_rgb6"]["equal"]:
        return 1
    print("DOS_INDEXED_FRAME_BYTE_MATCH=YES; DOS_CAPTURE_PROVENANCE_STILL_REQUIRES_INDEPENDENT_ATTESTATION")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
