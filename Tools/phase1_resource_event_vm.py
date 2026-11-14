#!/usr/bin/env python3
"""Phase 1: cross-title level event slots and RES9 interaction VM; read-only."""
from __future__ import annotations
import argparse
import collections
import hashlib
import json
import runpy
import struct
from pathlib import Path

GAMES = {
    "strahd": ("Ravenloft - Strahd's Possession",
               "RLOFT/LEVELS.DAT", "CD_Extracted/RLOFT/RES9"),
    "menzo": ("Menzoberranzan", "LEVELS.DAT", "CD_Extracted/RES9"),
    "stone": ("Ravenloft - Stone Prophet",
              "CD_Extracted/LEVELS.DAT", "PROPHET/RES9"),
}


def digest(b: bytes) -> str:
    return hashlib.sha256(b).hexdigest()


def indexed(b: bytes, label: str):
    if len(b) < 8:
        raise ValueError(f"{label}: truncated directory")
    count = struct.unpack_from("<I", b)[0]
    if not 1 <= count <= 100000 or 4 + 4 * count > len(b):
        raise ValueError(f"{label}: invalid record count")
    offsets = struct.unpack_from(f"<{count}I", b, 4)
    if offsets[0] != 4 + 4 * count or offsets[-1] > len(b):
        raise ValueError(f"{label}: out-of-bounds index")
    if any(a > z for a, z in zip(offsets, offsets[1:])):
        raise ValueError(f"{label}: nonmonotone index")
    return [b[a:z] for a, z in zip(offsets, (*offsets[1:], len(b)))]


def level_events(path: Path):
    content = path.read_bytes()
    levels = indexed(content, path.name)
    count_values = collections.Counter()
    opcodes = collections.Counter()
    samples = collections.defaultdict(list)
    invalid_count = []
    invalid_span = []
    active = 0
    commands = 0
    for lev, record in enumerate(levels):
        if len(record) < 29:
            raise ValueError(f"level {lev}: too short")
        ends = struct.unpack_from("<7I", record, 1)
        begin, end = ends[5], ends[6]
        if end - begin != 0x514 or end > len(record):
            invalid_span.append(lev)
            continue
        segment = record[begin:end]
        for slot in range(50):
            off = slot * 26
            n = segment[off]
            count_values[n] += 1
            if n > 5:
                invalid_count.append([lev, slot, n])
                continue
            if n:
                active += 1
            for j in range(n):
                op = segment[off + 1 + j * 5]
                opcodes[op] += 1
                commands += 1
                if len(samples[op]) < 3:
                    samples[op].append([lev, slot, j])
    return {
        "file": str(path.name), "sha256": digest(content),
        "level_records": len(levels), "slots": len(levels) * 50,
        "script_span_valid": len(levels) - len(invalid_span),
        "invalid_span_level_ids": invalid_span,
        "invalid_command_counts": invalid_count[:24],
        "invalid_count_total": len(invalid_count),
        "physical_slot_bytes": 26,
        "command_record_bytes": 5,
        "active_slots": active, "active_commands": commands,
        "count_distribution": {str(k): v for k, v in sorted(count_values.items())},
        "opcode_distribution": {str(k): v for k, v in sorted(opcodes.items())},
        "opcode_samples": {str(k): v for k, v in sorted(samples.items())},
        "compatible_with_strahd_slot_layout":
            not invalid_span and not invalid_count,
        "strahd_known_max_opcode12":
            all(op <= 12 for op in opcodes),
    }


def wordcode(path: Path, funcs: dict, title: str):
    content = path.read_bytes()
    entries = indexed(content, path.name)
    raw = [(i, rec) for i, rec in enumerate(entries)
           if rec and not (len(rec) >= 5 and rec[:2] == b"EH")]
    summary = {
        "file": path.name, "sha256": digest(content),
        "archive_entries": len(entries), "nonempty_raw_entries": len(raw),
        "shape_ok": 0, "two_bank_shape": 0, "one_bank_shape": 0,
        "companion_zero": 0, "stradh_vm_parse_ok": 0,
        "parse_failures": [], "word_count_histogram": {},
        "opcode_distribution": {}, "opcode_samples": {},
        "control_targets_outside_instr_boundaries": 0,
        "invalid_control_target_examples": []
    }
    word_counts = collections.Counter()
    opcode_dist = collections.Counter()
    op_samples = collections.defaultdict(list)
    for i, rec in raw:
        if len(rec) < 2:
            summary["parse_failures"].append([i, "short"])
            continue
        words = struct.unpack_from("<H", rec)[0]
        two_bank = len(rec) == 2 + words * 4
        one_bank = len(rec) == 2 + words * 2
        if two_bank:
            summary["two_bank_shape"] += 1
        if one_bank:
            summary["one_bank_shape"] += 1
        if title == "stone" and one_bank:
            program = list(struct.unpack_from(f"<{words}H", rec, 2))
        elif two_bank:
            try:
                program = funcs["decode_container"](rec)
                summary["companion_zero"] += 1
            except ValueError as ex:
                summary["parse_failures"].append([i, str(ex)])
                continue
        else:
            summary["parse_failures"].append([i, "neither wordcode shape"])
            continue
        summary["shape_ok"] += 1
        word_counts[words] += 1
        try:
            instructions = funcs["decode_program"](program, funcs["ACTIVE_ARITY"])
        except (ValueError, KeyError, IndexError) as ex:
            summary["parse_failures"].append([i, f"Strahd VM: {ex}"])
            continue
        summary["stradh_vm_parse_ok"] += 1
        valid = {ins["ip"] for ins in instructions}
        for ins in instructions:
            op = ins["opcode"]
            opcode_dist[op] += 1
            if len(op_samples[op]) < 4:
                op_samples[op].append([i, ins["ip"]])
            for target in funcs["targets"](ins):
                if target not in valid:
                    summary["control_targets_outside_instr_boundaries"] += 1
                    if len(summary["invalid_control_target_examples"]) < 20:
                        summary["invalid_control_target_examples"].append(
                            [i, ins["ip"], op, target])
    summary["word_count_histogram"] = {str(k): v for k, v in sorted(word_counts.items())}
    summary["opcode_distribution"] = {str(k): v for k, v in sorted(opcode_dist.items())}
    summary["opcode_samples"] = {str(k): v for k, v in sorted(op_samples.items())}
    summary["parse_failures"] = summary["parse_failures"][:50]
    return summary


def compare_stone(root: Path, funcs: dict):
    folder = root / "Ravenloft - Stone Prophet"
    installed = folder / "PROPHET/RES9"
    extracted = folder / "CD_Extracted/RES9"
    a = indexed(installed.read_bytes(), "installed RES9")
    b = indexed(extracted.read_bytes(), "CD RES9")
    differing = [i for i in range(min(len(a),len(b))) if a[i] != b[i]]
    out = {"installed_entries": len(a), "cd_entries": len(b),
           "changed_indices": differing, "record_diffs": []}
    for idx in differing:
        left, right = a[idx], b[idx]
        positions = [j for j in range(min(len(left),len(right)))
                     if left[j] != right[j]]
        shape = len(left) == len(right) and len(left) >= 2
        if shape:
            wc = struct.unpack_from("<H", left)[0]
            shape = len(left) == 2 + 2 * wc
        else:
            wc = None
        item = {"index": idx, "installed_bytes": len(left),
                "cd_bytes": len(right), "byte_changes": len(positions),
                "first_difference": positions[0] if positions else None,
                "two_bank_wordcode_shape": shape,
                "one_bank_wordcode_shape": len(left) == 2 + 2 * wc if wc is not None else False}
        if shape:
            try:
                lhs = funcs["decode_program"](
                    list(struct.unpack_from(f"<{wc}H", left, 2)), funcs["ACTIVE_ARITY"])
                rhs = funcs["decode_program"](
                    list(struct.unpack_from(f"<{wc}H", right, 2)), funcs["ACTIVE_ARITY"])
                instr_a = {i["ip"]: i for i in lhs}
                instr_b = {i["ip"]: i for i in rhs}
                different_instructions = [
                    {"ip": p, "opcode_a": instr_a[p]["opcode"],
                     "opcode_b": instr_b[p]["opcode"],
                     "operand_count": len(instr_a[p]["args"])}
                    for p in sorted(set(instr_a) & set(instr_b))
                    if instr_a[p] != instr_b[p]
                ]
                item["stradh_vm_parse_both"] = True
                item["changed_instruction_descriptors"] = different_instructions[:25]
                item["code_bank_byte_changes"] = sum(
                    2 <= p < 2 + 2 * wc for p in positions)
                item["companion_bank_byte_changes"] = sum(
                    2 + 2 * wc <= p for p in positions)
            except (ValueError, KeyError, IndexError) as ex:
                item["stradh_vm_parse_both"] = False
                item["reason"] = str(ex)
        out["record_diffs"].append(item)
    return out


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--root", required=True, type=Path)
    ap.add_argument("--repo-root", default=".", type=Path)
    ap.add_argument("--json", required=True, type=Path)
    args = ap.parse_args()
    root = args.root.resolve(strict=True)
    module_path = (args.repo_root / "Resources/RES/analyze_legacy_archives.py").resolve()
    funcs = runpy.run_path(str(module_path))
    report = {}
    for name, (dirname, lev, res) in GAMES.items():
        base = root / dirname
        report[name] = {
            "level_event_slots": level_events(base / lev),
            "res9_wordcode": wordcode(base / res, funcs, name)
        }
    report["stone_res9_installed_cd"] = compare_stone(root, funcs)
    args.json.parent.mkdir(parents=True, exist_ok=True)
    args.json.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    for title in GAMES:
        a = report[title]["level_event_slots"]
        w = report[title]["res9_wordcode"]
        print(f"{title}: event slots {a['active_slots']}/{a['slots']} "
              f"active commands={a['active_commands']} "
              f"Strahd-compatible shape={a['compatible_with_strahd_slot_layout']} "
              f"opcodes={a['opcode_distribution']}")
        print(f"{title}: RES9 raw {w['nonempty_raw_entries']} shape {w['shape_ok']} "
              f"companion zero {w['companion_zero']} "
              f"Strahd VM parse {w['stradh_vm_parse_ok']} "
              f"bad targets {w['control_targets_outside_instr_boundaries']}")
    print("stone installed/CD RES9:", report["stone_res9_installed_cd"])


if __name__ == "__main__":
    main()
