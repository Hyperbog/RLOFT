#!/usr/bin/env python3
"""Fail-closed external GPU readback comparability gate, not a GPU benchmark.

Usage: compare_renderer_candidates.py --reference ref.rgba --capture bgfx.json
      --capture sdl_gpu.json. Inputs come from real GPU capture hosts; this
checker cannot itself certify that a metadata author actually used a GPU.
"""
from __future__ import annotations

import argparse
import hashlib
import json
import math
from pathlib import Path
import statistics

WIDTH, HEIGHT, CHANNELS = 320, 400, 4
EXPECTED_BYTES = WIDTH * HEIGHT * CHANNELS
SCHEMA = "rloft-renderer-readback-v1"
BACKENDS = {"bgfx", "sdl_gpu", "godot"}


def digest(raw: bytes) -> str:
    return hashlib.sha256(raw).hexdigest()


def parse_capture(path: Path, reference_sha: str) -> dict[str, object]:
    obj = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(obj, dict) or obj.get("schema") != SCHEMA:
        raise ValueError(f"{path}: incorrect capture schema")
    backend = obj.get("backend")
    if backend not in BACKENDS:
        raise ValueError(f"{path}: unknown backend {backend!r}")
    if obj.get("capture_kind") != "GPU_READBACK":
        raise ValueError(f"{path}: benchmark needs actual GPU readback metadata")
    for key in ("source_pixels_sha256", "source_palette_sha256", "source_commit"):
        value = obj.get(key)
        if not isinstance(value, str) or not value.strip():
            raise ValueError(f"{path}: missing {key}")
    for key in ("platform_id", "graphics_device", "graphics_api"):
        value = obj.get(key)
        if not isinstance(value, str) or not value.strip():
            raise ValueError(f"{path}: missing environment field {key}")
    file = obj.get("rgba_path")
    if not isinstance(file, str) or not file or Path(file).is_absolute() or ".." in Path(file).parts:
        raise ValueError(f"{path}: readback path must be a relative local capture file")
    raw = (path.parent / file).read_bytes()
    if len(raw) != EXPECTED_BYTES:
        raise ValueError(f"{path}: readback size is {len(raw)}, not {EXPECTED_BYTES}")
    if digest(raw) != reference_sha:
        raise ValueError(f"{path}: indexed/palette RGBA readback DIFFERENT from CPU reference")
    timings = obj.get("frame_times_ms")
    if not isinstance(timings, list) or len(timings) < 120 or any(
        isinstance(v, bool) or not isinstance(v, (int, float)) or
        not math.isfinite(v) or v <= 0 for v in timings
    ):
        raise ValueError(f"{path}: >=120 positive finite measured frame timings required")
    for key in ("first_frame_ms", "peak_rss_mb", "cold_build_seconds"):
        value = obj.get(key)
        if isinstance(value, bool) or not isinstance(value, (int, float)) or not math.isfinite(value) or value < 0:
            raise ValueError(f"{path}: invalid {key}")
    times = sorted(float(v) for v in timings)
    p95 = times[math.ceil(len(times) * 0.95) - 1]
    return {
        "backend": backend,
        "readback_sha256": reference_sha,
        "source_pixels_sha256": obj["source_pixels_sha256"],
        "source_palette_sha256": obj["source_palette_sha256"],
        "source_commit": obj["source_commit"],
        "platform_id": obj["platform_id"],
        "graphics_device": obj["graphics_device"],
        "graphics_api": obj["graphics_api"],
        "frames": len(times),
        "median_frame_ms": round(statistics.median(times), 4),
        "p95_frame_ms": round(p95, 4),
        "first_frame_ms": obj["first_frame_ms"],
        "peak_rss_mb": obj["peak_rss_mb"],
        "cold_build_seconds": obj["cold_build_seconds"],
    }


def compare(reference: Path, capture_paths: list[Path]) -> dict[str, object]:
    if len(capture_paths) < 2:
        raise ValueError("at least two independent candidate readbacks required")
    ref = reference.read_bytes()
    if len(ref) != EXPECTED_BYTES:
        raise ValueError("CPU reference must be 320x400 RGBA8 (512000 bytes)")
    sha = digest(ref)
    results = [parse_capture(path, sha) for path in capture_paths]
    backends = [str(x["backend"]) for x in results]
    if len(set(backends)) != len(backends):
        raise ValueError("each backend must have exactly one capture")
    for key in ("source_pixels_sha256", "source_palette_sha256", "source_commit"):
        if len({str(v[key]) for v in results}) != 1:
            raise ValueError(f"backend captures use different {key}")
    platform_ids = {str(v["platform_id"]) for v in results}
    devices = {str(v["graphics_device"]) for v in results}
    # Cross-device output equivalence is useful; benchmark speed is NOT
    # comparable unless all backends ran on the same host and GPU.
    return {"schema": SCHEMA, "reference_rgba_sha256": sha,
            "pixel_equivalence": "PASS",
            "performance_comparable": len(platform_ids) == 1 and len(devices) == 1,
            "candidates": results,
            "note": "Metadata records are user-supplied; actual GPU origin is not independently attested."}


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--reference", type=Path, required=True)
    parser.add_argument("--capture", type=Path, action="append", required=True)
    parser.add_argument("--json-out", type=Path)
    args = parser.parse_args()
    try:
        result = compare(args.reference, args.capture)
    except (OSError, ValueError, TypeError, json.JSONDecodeError) as exc:
        parser.exit(status=1, message=f"READBACK_REJECTED: {exc}\n")
    output = json.dumps(result, sort_keys=True, indent=2)
    if args.json_out:
        args.json_out.write_text(output + "\n", encoding="utf-8")
    print("PIXEL_EQUIVALENCE_PASS" + (" PERFORMANCE_COMPARABLE" if result["performance_comparable"] else " PERFORMANCE_NOT_COMPARABLE"))
    print(output)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
