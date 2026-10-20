#!/usr/bin/env python3
"""Convert an instrumented DOSBox-X DAC JSONL capture + same-pause .sav.

No events are generated from CODE.2, COLORS, the native port, or the SAV.
The data must come solely from the DOSBox-X VGA write handlers.
Provenance remains DECLARED/UNATTESTED until capture is independently reviewed.
"""
import argparse
import hashlib
import json
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import verify_dosboxx_palette_port_trace as verifier

HOOK_ID = "dosboxx-v2026.10.01-dac-v1"
UPSTREAM_VGA_BLOB = "509d42fbead67a56594e49cec55549bc6fcc6d7c"
MAX_RAW_BYTES = 27000000
EXPECTED_HEADER = {
    "rloft_hook": HOOK_ID,
    "upstream_vga_blob": UPSTREAM_VGA_BLOB,
    "capture_stage": "accepted_after_lockout_and_rgb6_mask",
    "max_events": 200000,
}
RAW_EVENT_FIELDS = {"seq", "port", "value", "guest_cs_hint", "guest_eip_hint"}


def require(ok, explanation):
    if not ok:
        raise ValueError(explanation)


def normalize(raw):
    require(len(raw) <= MAX_RAW_BYTES, "Raw capture exceeds safety bound")
    lines = raw.splitlines()
    require(2 <= len(lines) <= 200001, "Raw capture length or count invalid")
    try:
        meta = json.loads(lines[0])
    except (UnicodeDecodeError, json.JSONDecodeError) as exc:
        raise ValueError("Invalid DAC capture header") from exc
    require(type(meta) is dict and meta == EXPECTED_HEADER,
            "Wrong upstream tag, source fingerprint, or capture stage")
    events = []
    for index, row in enumerate(lines[1:]):
        try:
            item = json.loads(row)
        except (UnicodeDecodeError, json.JSONDecodeError) as exc:
            raise ValueError("Truncated/invalid raw DAC capture line") from exc
        require(isinstance(item, dict) and not item.get("overflow"),
                "DAC recorder overflow: capture rejected")
        require(set(item) == RAW_EVENT_FIELDS,
                "Unexpected DOSBox-X DAC hook event fields")
        for name in RAW_EVENT_FIELDS:
            require(type(item[name]) is int, "Non-integer raw DAC field")
        require(item["seq"] == index and item["port"] in (0x3C8, 0x3C9) and
                0 <= item["value"] <= 255 and
                0 <= item["guest_cs_hint"] <= 65535 and
                0 <= item["guest_eip_hint"] <= 0xffffffff,
                "Invalid raw event sequence, port, or guest hint")
        events.append({k: item[k] for k in ("seq", "port", "value")})
    return events, {
        "upstream_git_blob": UPSTREAM_VGA_BLOB,
        "raw_JSONL_sha256": hashlib.sha256(raw).hexdigest(),
        "raw_event_count": len(events),
        "cpu_hints": "RAW_CS_EIP_NOT_NORMALIZED_OR_PROVEN_INSTRUCTION_ADDRESS",
    }


def convert(raw, saved, code2, capture_id):
    require(0 < len(capture_id) <= 100, "Invalid private capture identifier")
    require(hashlib.sha256(code2).hexdigest() == verifier.CODE2_SHA,
            "Original retail CODE.2 SHA256 mismatch")
    events, record = normalize(raw)
    trace = {
        "schema": verifier.SCHEMA,
        "provenance": {
            "origin": "independent_retail_dosboxx_io_hook",
            "program_sha256": verifier.CODE2_SHA,
            "dosbox_version": verifier.DOSBOX_VERSION.decode(),
            "saved_state_sha256": verifier.digest(saved.read_bytes()),
            "capture_id": capture_id,
            # User explicitly guarantees pause + copy; not external proof.
            "flush_before_snapshot": True,
        },
        "events": events,
    }
    result = verifier.validate(trace, saved.read_bytes(), verifier.snapshot(saved))
    return trace, {**record, "replay_result": result,
                   "retail_cpu_call_ancestry": "NOT_VERIFIED",
                   "source": "USER_DECLARED_INSTRUMENTED_DOSBOXX_JSONL"}


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument("--raw-jsonl", type=Path, required=True)
    p.add_argument("--state", type=Path, required=True)
    p.add_argument("--code2", type=Path, required=True)
    p.add_argument("--capture-id", required=True)
    p.add_argument("--owner-confirms-paused-copy", action="store_true", required=True,
                   help="Owner attests JSONL copy and SAV were taken during same emulator pause")
    p.add_argument("--output", type=Path, required=True)
    args = p.parse_args()
    require(args.owner_confirms_paused_copy,
            "Same-pause raw capture copy has not been declared")
    trace, report = convert(args.raw_jsonl.read_bytes(), args.state,
                            args.code2.read_bytes(), args.capture_id)
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(trace, indent=2) + "\n", encoding="utf-8")
    args.output.with_suffix(".receipt.json").write_text(
        json.dumps(report, indent=2) + "\n", encoding="utf-8")
    print("DOSBOXX_DAC_CAPTURE_NORMALIZED_EQUAL_UNATTESTED "
          "writes=%d snapshot_sha256=%s SOURCE_CALLER=NOT_VERIFIED" %
          (len(trace["events"]), trace["provenance"]["saved_state_sha256"]))


if __name__ == "__main__":
    main()
