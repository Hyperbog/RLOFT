#!/usr/bin/env python3
"""Fail-closed synchronization gate for independent DOS/recomp indexed frames.

This script checks bytes and provided observation receipts. It cannot
authenticate how either observation was recorded; the result is never a
certificate of DOS execution. Original artwork stays in private .idx/.pal
files and does not appear in report output.
"""
from __future__ import annotations

import argparse
import hashlib
import json
from pathlib import Path
import sys

from compare_dos_indexed_frames import INDEX_BYTES, DAC_BYTES, validate


SCHEMA = "rloft-vga-observation-v1"
FIELDS = (
    "level", "frame_boundary", "camera_x_16_16", "camera_y_16_16",
    "heading", "crtc_start", "source_tick", "keyboard_trace_sha256",
)
FRAME_BOUNDARIES = ("1F8FC_1A5B0", "1FC14_1A510")
HEX = set("0123456789abcdef")


def require(ok: bool, detail: str) -> None:
    if not ok:
        raise ValueError(detail)


def document(path: Path) -> dict:
    parsed = json.loads(path.read_text(encoding="utf-8"))
    require(isinstance(parsed, dict), "Observation must be a JSON object")
    return parsed


def sha(raw: bytes) -> str:
    return hashlib.sha256(raw).hexdigest()


def receipt(path: Path, role: str, idx: bytes, palette: bytes) -> dict:
    doc = document(path)
    require(doc.get("schema") == SCHEMA, "Unknown observation schema")
    require(doc.get("role") == role, "Observation role does not match input")
    require(set(FIELDS).issubset(doc), "Missing synchronization field")
    require(doc["level"] == 22 and type(doc["level"]) is int,
            "Only authenticated Level 22 observations can use this gate")
    require(doc["frame_boundary"] in FRAME_BOUNDARIES,
            "Unknown original presentation boundary")
    require(doc["crtc_start"] in (0, 32768) and type(doc["crtc_start"]) is int,
            "Unverified CRTC page")
    require(type(doc["heading"]) is int and 0 <= doc["heading"] < 360,
            "Invalid heading")
    for field in ("camera_x_16_16", "camera_y_16_16"):
        require(type(doc[field]) is int and 0 <= doc[field] <= 0xFFFFFFFF,
                "Invalid camera coordinate: " + field)
    require(type(doc["source_tick"]) is int and doc["source_tick"] >= 0,
            "Invalid source tick")
    trace = doc["keyboard_trace_sha256"]
    require(isinstance(trace, str) and len(trace) == 64 and
            set(trace).issubset(HEX), "Invalid input-event trace SHA256")
    require(doc.get("indices_sha256") == sha(idx),
            "Frame bytes differ from observation SHA256")
    require(doc.get("rgb6_sha256") == sha(palette),
            "DAC bytes differ from observation SHA256")
    return doc


def check_pair(dos_index: Path, native_index: Path,
               dos_palette: Path, native_palette: Path,
               dos_observation: Path, native_observation: Path,
               dosbox_state_report: Path) -> dict:
    # Raw-byte validation precedes all receipt acceptance. The
    # independent screenshot/save-state exporter is a mandatory DOS gate.
    comparison = validate(dos_index, native_index,
                          dos_palette, native_palette)
    di, ni = dos_index.read_bytes(), native_index.read_bytes()
    dp, np = dos_palette.read_bytes(), native_palette.read_bytes()
    require(len(di) == len(ni) == INDEX_BYTES and
            len(dp) == len(np) == DAC_BYTES,
            "Frame or palette size changed during receipt validation")
    original = receipt(dos_observation, "original_dos", di, dp)
    reconstructed = receipt(native_observation, "compiled_recomp", ni, np)
    report = document(dosbox_state_report)
    require(report.get("private_dos_raw_export") ==
            "EXACT_SCREENSHOT_AND_STATE_AGREEMENT",
            "DOS state was not exported after exact screenshot agreement")
    require(report.get("state_vs_screenshot_matching_rgb_pixels") == INDEX_BYTES,
            "Original DOS screenshot and VGA state differ")
    require(report.get("state_vs_screenshot_different_rgb_pixels") == 0,
            "DOS state/screenshot mismatch count is nonzero")
    require(report.get("synchronized_frame_evidence") == "strong",
            "DOS state/screenshot sync evidence absent")
    require(report.get("state_original_page_sha256") == sha(di) and
            report.get("original_dac_rgb6_sha256") == sha(dp),
            "DOS saved-state receipt does not authenticate the supplied bytes")
    require(report.get("crtc_start") == original["crtc_start"],
            "Original DOS observation CRTC page differs from save state")
    require(report.get("crtc_line_compare", -1) >= 399 and
            type(report.get("crtc_line_compare")) is int,
            "Unverified original VGA line compare")
    differences = {
        key: {"dos": original[key], "recomp": reconstructed[key]}
        for key in FIELDS if original[key] != reconstructed[key]
    }
    # Never compare a source 1F8FC compositor page with an unrelated
    # 1FC14 page or a different keyboard/camera/tick presentation.
    require(not differences,
            "Frames are not synchronized: " + ", ".join(differences))
    # Exact data parity alone is not source provenance attestation.
    return {
        "schema": "rloft-synchronized-dos-indexed-comparison-v1",
        "synchronization_fields_equal": True,
        "screenshot_save_state_exact": True,
        "pixel_and_palette_equal": comparison["pixel_and_palette_equal"],
        "indices": comparison["indices"],
        "palette_rgb6": comparison["palette_rgb6"],
        "dos_index_sha256": sha(di),
        "recomp_index_sha256": sha(ni),
        "dos_rgb6_sha256": sha(dp),
        "recomp_rgb6_sha256": sha(np),
        "original_dos_execution_independently_attested": False,
        "retail_dos_parity_certified": False,
        "note": "Matched receipts and bytes require independent debugger provenance review before DOS parity certification.",
    }


def main() -> int:
    p = argparse.ArgumentParser(description=__doc__)
    for argument in ("dos-index", "native-index", "dos-palette",
                     "native-palette", "dos-observation",
                     "native-observation", "dosbox-state-report"):
        p.add_argument("--" + argument, type=Path, required=True)
    p.add_argument("--report", type=Path)
    args = p.parse_args()
    try:
        outcome = check_pair(args.dos_index, args.native_index,
                             args.dos_palette, args.native_palette,
                             args.dos_observation, args.native_observation,
                             args.dosbox_state_report)
    except (OSError, ValueError, KeyError, TypeError,
            json.JSONDecodeError) as exc:
        print("DOS_SYNC_FRAME_GATE_FAIL: " + str(exc), file=sys.stderr)
        return 2
    rendered = json.dumps(outcome, indent=2, sort_keys=True) + "\n"
    if args.report:
        args.report.write_text(rendered, encoding="utf-8")
    print(rendered)
    return 0 if outcome["pixel_and_palette_equal"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
