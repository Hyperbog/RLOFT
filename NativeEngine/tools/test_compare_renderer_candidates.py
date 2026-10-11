"""Synthetic-only self-tests for fail-closed external GPU comparison metadata."""
from __future__ import annotations

import json
from pathlib import Path
import sys
from tempfile import TemporaryDirectory

sys.path.insert(0, str(Path(__file__).resolve().parent))
from compare_renderer_candidates import compare  # noqa: E402


def reject(fn):
    try:
        fn()
    except (ValueError, OSError, TypeError):
        return
    raise AssertionError("invalid benchmark input was accepted")


def main() -> None:
    with TemporaryDirectory(prefix="rloft-renderer-evidence-") as temp:
        root = Path(temp)
        (root / "cpu.rgba").write_bytes(bytes([11, 22, 33, 255]) * (320 * 400))
        capture = {
            "schema": "rloft-renderer-readback-v1", "capture_kind": "GPU_READBACK",
            "source_pixels_sha256": "pixels-fixed", "source_palette_sha256": "palette-fixed",
            "source_commit": "fixture-revision", "platform_id": "synthetic-machine",
            "graphics_device": "synthetic-gpu", "graphics_api": "mock-only",
            "rgba_path": "gpu.rgba", "frame_times_ms": [16.7] * 120,
            "first_frame_ms": 50, "peak_rss_mb": 60, "cold_build_seconds": 10,
        }
        (root / "gpu.rgba").write_bytes((root / "cpu.rgba").read_bytes())
        a, b = root / "bgfx.json", root / "sdl_gpu.json"
        def save(path, backend, **edits):
            path.write_text(json.dumps({**capture, "backend": backend, **edits}), encoding="utf-8")
        save(a, "bgfx")
        save(b, "sdl_gpu")
        result = compare(root / "cpu.rgba", [a, b])
        assert result["pixel_equivalence"] == "PASS"
        assert result["performance_comparable"] is True
        assert len(result["candidates"]) == 2
        save(b, "sdl_gpu", platform_id="different-machine")
        assert not compare(root / "cpu.rgba", [a, b])["performance_comparable"]
        save(b, "sdl_gpu", rgba_path="missing.rgba")
        reject(lambda: compare(root / "cpu.rgba", [a, b]))
        save(b, "sdl_gpu", rgba_path="gpu.rgba", frame_times_ms=[1.0] * 119)
        reject(lambda: compare(root / "cpu.rgba", [a, b]))
        save(b, "sdl_gpu", rgba_path="gpu.rgba", frame_times_ms=[1.0] * 120,
             source_pixels_sha256="mismatched-frame")
        reject(lambda: compare(root / "cpu.rgba", [a, b]))
        save(b, "sdl_gpu", frame_times_ms=[1.0] * 120, capture_kind="CPU_SYNTHETIC")
        reject(lambda: compare(root / "cpu.rgba", [a, b]))
        save(b, "sdl_gpu", frame_times_ms=[1.0] * 120, rgba_path="../escape.rgba")
        reject(lambda: compare(root / "cpu.rgba", [a, b]))
        save(b, "sdl_gpu", frame_times_ms=[1.0] * 120, rgba_path="gpu.rgba")
        reject(lambda: compare(root / "cpu.rgba", [a, b, b]))
        (root / "gpu.rgba").write_bytes(bytes([0]) * (320 * 400))
        reject(lambda: compare(root / "cpu.rgba", [a, b]))
        reject(lambda: compare(root / "cpu.rgba", [a]))
    print("SYNTHETIC_COMPARATOR_PASS (no actual GPU claims)")


if __name__ == "__main__":
    main()
