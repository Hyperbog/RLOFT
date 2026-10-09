#!/usr/bin/env python3
"""Lossless 320x400 indexed-frame comparison; never synthesize DOS data.

A DOS reference MUST be supplied independently. Matching camera poses do
not prove matched IRQ timing or animation state; this tool only measures
pixel/palette differences at the provided capture instants.
"""
from __future__ import annotations

import argparse
import hashlib
import json
from pathlib import Path

PIXEL_COUNT = 320 * 400
PALETTE_COUNT = 256 * 3
NATIVE_LABEL = "native-preview-not-original-DOS"
DOS_LABEL = "original-DOS"


def read_capture(path: Path, provenance: str) -> tuple[tuple[int, int, int, int], bytes, bytes]:
    """Require exact pose, original byte lengths and declared provenance."""
    manifest = path.read_text(encoding="utf-8")
    lines: dict[str, list[str]] = {}
    for line in manifest.splitlines():
        parts = line.split("\t")
        if parts and parts[0] not in lines:
            lines[parts[0]] = parts[1:]
    if not lines.get("format", [""])[0].startswith(
        "rloft-native-indexed-capture-v"
    ) and lines.get("format", [""])[0] != "rloft-dos-indexed-capture-v1":
        raise ValueError(f"{path}: unrecognized indexed capture format")
    if lines.get("provenance") != [provenance]:
        raise ValueError(f"{path}: expected provenance {provenance!r}")
    if lines.get("dimensions") != ["320", "400"]:
        raise ValueError(f"{path}: expected raw indexed dimensions 320 x 400")
    if lines.get("palette") != ["256x3", "VGA-6bit-raw"]:
        raise ValueError(f"{path}: palette must be raw 6-bit VGA RGB triplets")
    try:
        x, y = (int(v) for v in lines["cell"])
        pose = (int(lines["level"][0]), x, y, int(lines["facing"][0]))
    except (KeyError, ValueError, IndexError) as exc:
        raise ValueError(f"{path}: invalid capture pose") from exc
    pixels = path.with_suffix(".idx").read_bytes()
    palette = path.with_suffix(".pal").read_bytes()
    if len(pixels) != PIXEL_COUNT or len(palette) != PALETTE_COUNT:
        raise ValueError(f"{path}: capture byte length is not 320x400/768")
    if max(palette) > 63:
        raise ValueError(f"{path}: palette is not raw 6-bit DAC data")
    return pose, pixels, palette


def inventory(folder: Path, provenance: str) -> dict[tuple[int, int, int, int], tuple[bytes, bytes]]:
    """Fail on duplicate pose rather than silently comparing different ticks."""
    captures: dict[tuple[int, int, int, int], tuple[bytes, bytes]] = {}
    if not folder.is_dir():
        raise ValueError(f"{folder}: capture folder does not exist")
    for manifest in sorted(folder.glob("frame_*.tsv")):
        pose, pixels, palette = read_capture(manifest, provenance)
        if pose in captures:
            raise ValueError(f"{folder}: duplicate pose {pose}; select a single tick")
        captures[pose] = (pixels, palette)
    return captures


def compare(
    native_folder: Path, reference_folder: Path, reference_kind: str = "dos"
) -> dict:
    if reference_kind not in {"dos", "native"}:
        raise ValueError("reference kind must be dos or native")
    other_provenance = DOS_LABEL if reference_kind == "dos" else NATIVE_LABEL
    native = inventory(native_folder, NATIVE_LABEL)
    reference = inventory(reference_folder, other_provenance)
    shared = sorted(set(native) & set(reference))
    report: dict = {
        "comparison": f"native-vs-{reference_kind}",
        "meaning": "raw-indexed-frame-only; no DOS IRQ or animation equivalence",
        "native_count": len(native),
        "reference_count": len(reference),
        "unmatched_native": [list(key) for key in sorted(set(native) - set(reference))],
        "unmatched_reference": [list(key) for key in sorted(set(reference) - set(native))],
        "matched_count": len(shared),
        "exact_matches": 0,
        "frames": [],
    }
    for pose in shared:
        left, left_pal = native[pose]
        right, right_pal = reference[pose]
        count = sum(a != b for a, b in zip(left, right))
        palette_diff = sum(a != b for a, b in zip(left_pal, right_pal))
        first = next((i for i, (a, b) in enumerate(zip(left, right)) if a != b), None)
        exact = count == 0 and palette_diff == 0
        report["exact_matches"] += int(exact)
        report["frames"].append({
            "pose": list(pose),
            "pixel_mismatches": count,
            "pixel_mismatch_percent": round(count * 100.0 / PIXEL_COUNT, 6),
            "palette_byte_mismatches": palette_diff,
            "first_mismatch_xy": None if first is None else [first % 320, first // 320],
            "native_sha256": hashlib.sha256(left).hexdigest(),
            "reference_sha256": hashlib.sha256(right).hexdigest(),
            "exact_bytes": exact,
        })
    return report


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--native", type=Path, required=True)
    parser.add_argument("--reference", type=Path, required=True)
    parser.add_argument("--reference-kind", choices=("dos", "native"), default="dos")
    parser.add_argument("--out", type=Path)
    args = parser.parse_args()
    try:
        report = compare(args.native, args.reference, args.reference_kind)
    except (ValueError, OSError) as exc:
        parser.exit(2, f"Capture validation error: {exc}\n")
    output = json.dumps(report, indent=2, sort_keys=True) + "\n"
    if args.out:
        args.out.write_text(output, encoding="utf-8")
    else:
        print(output, end="")
    if report["matched_count"] == 0:
        return 2
    return int(report["matched_count"] != report["exact_matches"])


if __name__ == "__main__":
    raise SystemExit(main())
