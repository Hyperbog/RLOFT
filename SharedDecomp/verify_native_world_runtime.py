#!/usr/bin/env python3
"""Independent, original-file-backed native world runtime evidence verifier.

This validates source payload identity and the CODE.2 *normal* collision gate.
It does not execute DOS code, render scenes or certify gameplay.
"""
from __future__ import annotations

import argparse
from collections import Counter
import hashlib
import json
from pathlib import Path
import struct
import sys

from original_world_oracle import IDENTITY, OracleError, inspect_levels
from workflow import atomic_json

SCHEMA = "rloft-native-world-runtime-evidence/v1"
RESULT_SCHEMA = "rloft-original-native-world-runtime-diff/v1"
SOURCE_LIMIT = 8 * 1024 * 1024
REPORT_LIMIT = 512 * 1024
COUNTER_KEYS = ("total_cells", "visible_cells", "explored_cells",
                "legacy_authoring_markers", "special_center_cells",
                "passable_normal_cells", "ordinary_blocked_cells",
                "special_blocked_cells", "kind1_dynamic_records",
                "animating_dynamic_records")

def require(ok, why):
    if not ok:
        raise OracleError(why)

def u32(data, pos):
    require(0 <= pos <= len(data) - 4, "original directory out of bounds")
    return struct.unpack_from("<I", data, pos)[0]

def p16(value):
    return struct.pack("<H", value)

def p32(value):
    return struct.pack("<I", value)

def expected_original_runtime(data: bytes) -> dict:
    summary, levels = inspect_levels(data)
    require(summary["level_count"] == 32, "not a 32-level original archive")
    offsets = [u32(data, 4+4*n) for n in range(32)] + [len(data)]
    encoded = bytearray(b"RLOFT-native-original-world-source-v1")
    encoded.extend(p32(32))
    aggregate = Counter()
    per_level = []
    for id in range(32):
        rec = data[offsets[id]:offsets[id+1]]
        cut = [u32(rec, 1+4*k) for k in range(7)]
        level = levels[id]
        width, height = level["w"], level["h"]
        cell_count = width*height
        words = struct.unpack_from(f"<{cell_count}H", rec, cut[0]+0x2a4)
        encoded.extend(p16(id)+p16(width)+p16(height)+p32(cell_count))
        encoded.extend(rec[cut[0]+0x2a4:cut[1]])
        cursor = cut[1]
        serialized_controls = bytearray()
        kind1_by_cell = {}
        controls = dynamic = active = 0
        for col in range(width):
            count = struct.unpack_from("<h", rec, cursor)[0]
            require(count >= 0, f"level {id}: negative control count")
            cursor += 2
            for _ in range(count):
                control = rec[cursor:cursor+10]
                require(len(control) == 10 and control[5] == col and
                        control[4] < height,
                        f"level {id}: original control coordinate invalid")
                cursor += 10
                controls += 1
                serialized_controls.extend(p16(col) + control)
                if control[0] == 1:
                    dynamic += 1
                    position = control[6]
                    delta = control[7] if control[7] < 128 else control[7]-256
                    require(position <= 8 and delta in (-1, 0, 1),
                            f"level {id}: invalid original dynamic state")
                    active += delta != 0
                    kind1_by_cell.setdefault((col, control[4]), position)
        require(cursor == cut[2], f"level {id}: original control list overrun")
        encoded.extend(p32(controls) + serialized_controls)
        for j in range(3):
            raw = rec[cut[2+j]:cut[3+j]]
            require(len(raw)%11 == 0, f"level {id}: family record stride")
            encoded.extend(p32(len(raw)//11) + raw)
        encoded.extend(rec[cut[5]:cut[6]])
        start = cut[6]+4
        automap = rec[start:start+cell_count]
        annotation_pos = start+cell_count
        annotations = rec[annotation_pos+4:]
        require(len(automap) == cell_count and
                len(annotations) == u32(rec, annotation_pos),
                f"level {id}: source automap/annotation mismatch")
        encoded.extend(p32(len(automap))+automap)
        encoded.extend(p32(len(annotations))+annotations)
        local = Counter()
        for row in range(height):
            for column in range(width):
                word = words[row*width+column]
                local["total_cells"] += 1
                local["visible_cells"] += bool(word & 0x0040)
                local["explored_cells"] += bool(word & 0x0400)
                local["legacy_authoring_markers"] += bool(word & 0x2000)
                if word & 0x0080:
                    local["special_center_cells"] += 1
                    if kind1_by_cell.get((column, row), -1) == 0:
                        local["passable_normal_cells"] += 1
                    else:
                        local["special_blocked_cells"] += 1
                elif word & 0x003f:
                    local["ordinary_blocked_cells"] += 1
                else:
                    local["passable_normal_cells"] += 1
        local.update(kind1_dynamic_records=dynamic,
                     animating_dynamic_records=active)
        require(level["controls"] == controls,
                f"level {id}: independent source control count differs")
        aggregate.update(local)
        per_level.append({
            "level": id,
            "cells": cell_count,
            "normal_passable": local["passable_normal_cells"],
            "ordinary_blocked": local["ordinary_blocked_cells"],
            "special_blocked": local["special_blocked_cells"],
            "special_centers": local["special_center_cells"],
            "dynamic_records": dynamic,
        })
    return {
        "original_level_file_sha256": hashlib.sha256(data).hexdigest(),
        "retained_source_payload_sha256": hashlib.sha256(encoded).hexdigest(),
        "counters": {key: aggregate[key] for key in COUNTER_KEYS},
        "per_level": per_level,
    }

def compare(data: bytes, native: dict, require_known=True) -> dict:
    require(not require_known or
            hashlib.sha256(data).hexdigest() == IDENTITY["LEVELS.DAT"],
            "LEVELS.DAT SHA256 does not match pinned supplied original")
    original = expected_original_runtime(data)
    require(native.get("schema") == SCHEMA, "invalid native runtime schema")
    for field in ("scene_projection_verified", "event_vm_executed",
                  "original_dos_execution_verified", "gameplay_parity_verified"):
        require(native.get(field) is False, f"unsupported original parity claim: {field}")
    expected = {
        "levels": 32,
        "active_level": 31,  # the native whole-world audit visits 0 through 31
        "original_world_sha256": original["retained_source_payload_sha256"],
        **original["counters"],
    }
    # The unmodified archive walk must not have started any animations.
    mismatches = []
    for field, value in expected.items():
        observed = native.get(field)
        if type(observed) is not type(value) or observed != value:
            mismatches.append({"field": field, "original": value, "native": observed})
    return {
        "schema": RESULT_SCHEMA,
        "status": "ORIGINAL_NORMAL_WORLD_RUNTIME_MATCH" if not mismatches else "DIFFERENT",
        "pinned_original_sha256_match": (
            original["original_level_file_sha256"] == IDENTITY["LEVELS.DAT"]),
        "original_levels_sha256": original["original_level_file_sha256"],
        "retained_payload_sha256": original["retained_source_payload_sha256"],
        "level_count_checked": 32,
        "counter_fields_checked": len(expected),
        "first_divergence": mismatches[0] if mismatches else None,
        "differences": mismatches[:64],
        "difference_count": len(mismatches),
        "per_level_independent_reference": original["per_level"],
        "original_dos_gameplay_parity_verified": False,
        "native_first_person_projection_verified": False,
        "event_vm_verified": False,
        "note": "Normal CODE.2 traversal gate only; not actor occupancy, "
                "secret-mode collision, renderer or save semantics.",
    }

def main(argv=None):
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument("--levels", type=Path, required=True)
    p.add_argument("--native-report", type=Path, required=True)
    p.add_argument("--output", type=Path, required=True)
    p.add_argument("--allow-other-version", action="store_true")
    args = p.parse_args(argv)
    try:
        require(0 < args.levels.stat().st_size <= SOURCE_LIMIT,
                "original levels file size exceeds bound")
        require(0 < args.native_report.stat().st_size <= REPORT_LIMIT,
                "native evidence report size exceeds bound")
        def unique(pairs):
            out = {}
            for key, value in pairs:
                require(key not in out, "duplicate JSON object key")
                out[key] = value
            return out
        def nonfinite(value):
            raise OracleError("nonfinite JSON numeric value")
        native = json.loads(args.native_report.read_text(encoding="utf-8-sig"),
                            object_pairs_hook=unique, parse_constant=nonfinite)
        require(isinstance(native, dict), "native report must be object")
        result = compare(args.levels.read_bytes(), native,
                         require_known=not args.allow_other_version)
        atomic_json(args.output, result)
        print(json.dumps({"status": result["status"],
                          "fields_compared": result["counter_fields_checked"],
                          "original_dos_gameplay_parity_verified": False},
                         sort_keys=True))
        return 0 if result["status"] == "ORIGINAL_NORMAL_WORLD_RUNTIME_MATCH" else 1
    except (OracleError, OSError, TypeError, ValueError, KeyError,
            struct.error, UnicodeError) as err:
        print("RUNTIME ORIGINAL EVIDENCE ERROR:", err, file=sys.stderr)
        return 2

if __name__ == "__main__":
    raise SystemExit(main())
