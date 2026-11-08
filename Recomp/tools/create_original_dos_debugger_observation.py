#!/usr/bin/env python3
"""Generate a *candidate*, not attested, DOS capture receipt from debugger bytes.

The DOSBox-X saved-state VGA capture is independent of the original DOS
protected-mode guest's data memory. This tool extracts guest camera/heading
from an actual external debugger dump instead of taking them from the
recompilation. It cannot prove the authenticity of the external dump itself.
Keep all original and screenshot data private; print hashes only.
"""
from __future__ import annotations
import argparse
import hashlib
import json
from pathlib import Path
import struct
import sys

KNOWN_CODE2_SHA256 = "853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314"
MIN_DGROUP = 0x255D8
BYTES_320x400 = 320 * 400
RGB6_BYTES = 256 * 3


def digest(payload: bytes) -> str:
    return hashlib.sha256(payload).hexdigest()


def require(valid: bool, detail: str) -> None:
    if not valid:
        raise ValueError(detail)


def candidate(
    original_code2: bytes, debugger_dgroup: bytes,
    original_indices: bytes, original_rgb6: bytes,
    screenshot_state_report: dict, source_keys: bytes, tick: int,
    boundary: str = "1FC14_1A510",
) -> dict:
    require(digest(original_code2) == KNOWN_CODE2_SHA256,
            "Original retail CODE.2 identity does not match project anchor")
    require(len(debugger_dgroup) >= MIN_DGROUP,
            "Protected-mode debugger DS:0 dump too small for original camera")
    require(len(original_indices) == BYTES_320x400,
            "Original DOS indexed framebuffer must contain 128000 bytes")
    require(len(original_rgb6) == RGB6_BYTES and
            all(value < 64 for value in original_rgb6),
            "Original DOS DAC bank must have 256 RGB6 triples")
    require(type(tick) is int and 0 <= tick <= 1000000,
            "Source tick must be an independently counted integer")
    require(boundary in ("1FC14_1A510", "1F8FC_1A5B0"),
            "Unsupported original presentation boundary")
    require(0 < len(source_keys) <= 4096 and len(source_keys) % 2 == 0,
            "Source keyboard trace must contain LE16 words, not host ASCII")
    require(isinstance(screenshot_state_report,dict),
            "DOSBox-X exporter JSON missing")
    require(screenshot_state_report.get("private_dos_raw_export") ==
            "EXACT_SCREENSHOT_AND_STATE_AGREEMENT",
            "No original DOSBox-X exact screenshot/state agreement")
    require(screenshot_state_report.get("state_vs_screenshot_matching_rgb_pixels") ==
            BYTES_320x400 and
            screenshot_state_report.get("state_vs_screenshot_different_rgb_pixels") == 0,
            "DOSBox-X screenshot/save state do not agree")
    require(screenshot_state_report.get("synchronized_frame_evidence") == "strong",
            "DOS capture lacks synchronized-frame evidence")
    require(screenshot_state_report.get("state_original_page_sha256") ==
            digest(original_indices) and
            screenshot_state_report.get("original_dac_rgb6_sha256") ==
            digest(original_rgb6),
            "DOS save state hashes differ from original screenshot/export")
    crtc=screenshot_state_report.get("crtc_start")
    require(type(crtc) is int and crtc in (0,32768),
            "Unverified original DOS CRTC start")
    level=struct.unpack_from("<I",debugger_dgroup,0xC6C8)[0]
    x=struct.unpack_from("<I",debugger_dgroup,0x255D0)[0]
    y=struct.unpack_from("<I",debugger_dgroup,0x255D4)[0]
    heading=struct.unpack_from("<I",debugger_dgroup,0xC658)[0]
    require(level == 22, "Debugger dump is not original Level 22")
    require(0 <= heading < 360,
            "Debugger heading invalid: data segment or pause site unverified")
    # Receipt values are read directly from independent DOS memory, not
    # copied from a native reconstructed screenshot or guessed camera.
    return {
        "schema": "rloft-vga-observation-v1",
        "role": "original_dos",
        "level": level,
        "frame_boundary": boundary,
        "camera_x_16_16": x,
        "camera_y_16_16": y,
        "heading": heading,
        "crtc_start": crtc,
        "source_tick": tick,
        "keyboard_trace_sha256": digest(source_keys),
        "indices_sha256": digest(original_indices),
        "rgb6_sha256": digest(original_rgb6),
        "debugger_dgroup_sha256": digest(debugger_dgroup),
        "retail_code2_sha256": digest(original_code2),
        "debugger_dump_origin": "USER_SUPPLIED_PROTECTED_MODE_DS_ZERO_DUMP",
        "source_tick_origin": "INDEPENDENTLY_COUNTED_EXTERNAL_DOS_EVENTS_REQUIRED",
        "original_dos_execution_independently_attested": False,
        "note": "Debugger dump and event-count provenance must be reviewed independently.",
    }


def main() -> int:
    p=argparse.ArgumentParser(description=__doc__)
    for name in ("retail-code2", "dgroup-dump", "dos-index",
                 "dos-palette", "dosbox-state-report", "keyboard-trace",
                 "out"):
        p.add_argument("--"+name,required=True,type=Path)
    p.add_argument("--source-tick",required=True,type=int)
    p.add_argument("--frame-boundary",default="1FC14_1A510",
                   choices=("1FC14_1A510","1F8FC_1A5B0"))
    a=p.parse_args()
    try:
        outcome=candidate(
            a.retail_code2.read_bytes(),a.dgroup_dump.read_bytes(),
            a.dos_index.read_bytes(),a.dos_palette.read_bytes(),
            json.loads(a.dosbox_state_report.read_text("utf-8")),
            a.keyboard_trace.read_bytes(),a.source_tick,a.frame_boundary)
        a.out.write_text(json.dumps(outcome,indent=2,sort_keys=True)+"\n",
                         encoding="utf-8")
        print("RLOFT_PRIVATE_DOS_DEBUGGER_OBSERVATION_CREATED"
              " original_code2=SHA256_VERIFIED"
              " camera=EXTERNAL_DOS_MEMORY"
              " screenshot_state=EXACT_RGB"
              " independent_execution_attestation=NOT_CERTIFIED"
              " private_raw_files=NOT_EXPORTED_TO_GIT")
        return 0
    except (ValueError,OSError,KeyError,TypeError,json.JSONDecodeError) as exc:
        print("RLOFT_PRIVATE_DOS_DEBUGGER_OBSERVATION_FAIL: "+str(exc),
              file=sys.stderr)
        return 2


if __name__=="__main__":
    raise SystemExit(main())
