#!/usr/bin/env python3
"""Validate Ravenloft's COLORS VGA palette bank."""
from __future__ import annotations
import argparse, hashlib, json
from pathlib import Path

PALETTE_BYTES = 256 * 3
EXPECTED_SHA256 = "e366986bf701ba832aa769bf99f0db90a76af658300ce3d02e7039a970a9baf5"


def analyze(path: Path):
    data = path.read_bytes()
    if not data:
        raise ValueError("empty COLORS")
    count = data[0]
    expected = 1 + count * PALETTE_BYTES
    if len(data) != expected:
        raise ValueError(f"file size {len(data)} != 1 + {count}*768 ({expected})")
    palettes = []
    for i in range(count):
        start = 1 + i * PALETTE_BYTES
        raw = data[start:start + PALETTE_BYTES]
        if len(raw) != PALETTE_BYTES:
            raise ValueError(f"palette {i} truncated")
        component_max = max(raw)
        if component_max > 63:
            raise ValueError(f"palette {i} component exceeds VGA 6-bit domain: {component_max}")
        palettes.append({
            "index": i,
            "offset": start,
            "bytes": len(raw),
            "component_min": min(raw),
            "component_max": component_max,
            "sha256": hashlib.sha256(raw).hexdigest(),
        })
    return {
        "bytes": len(data),
        "sha256": hashlib.sha256(data).hexdigest(),
        "expected_source_sha256": EXPECTED_SHA256,
        "source_sha256_matches": hashlib.sha256(data).hexdigest() == EXPECTED_SHA256,
        "serialized_palette_count": count,
        "palette_bytes": PALETTE_BYTES,
        "rgb_triplets_per_palette": 256,
        "all_components_in_0_63": True,
        "palettes": palettes,
    }


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("file", type=Path)
    ap.add_argument("--json", action="store_true")
    args = ap.parse_args()
    r = analyze(args.file)
    if args.json:
        print(json.dumps(r, indent=2))
    else:
        print(f"bytes: {r['bytes']}")
        print(f"sha256: {r['sha256']}")
        print(f"palette count: {r['serialized_palette_count']}")
        print(f"palette bytes: {r['palette_bytes']}")
        print(f"RGB triplets/palette: {r['rgb_triplets_per_palette']}")
        print(f"all components in 0..63: {r['all_components_in_0_63']}")
        for p in r["palettes"]:
            print(f"  {p['index']:2d}: off=0x{p['offset']:04X} min={p['component_min']} max={p['component_max']} sha256={p['sha256']}")
    return 0

if __name__ == "__main__":
    raise SystemExit(main())
