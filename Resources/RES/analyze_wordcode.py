#!/usr/bin/env python3
"""Decode and validate Ravenloft RES9 interaction/display wordcode programs."""
from __future__ import annotations

import argparse
import json
import struct
from collections import Counter
from pathlib import Path

FIXED_ARITY = {
    0x03: 2, 0x04: 2, 0x05: 1, 0x06: 1, 0x07: 0, 0x08: 2,
    0x09: 2, 0x0A: 1, 0x0B: 1, 0x0C: 1, 0x0D: 2, 0x0E: 0,
    0x0F: 3, 0x10: 3, 0x11: 0, 0x12: 1, 0x13: 2, 0x14: 1,
    0x15: 0, 0x16: 1, 0x17: 1, 0x18: 1, 0x19: 1, 0x1A: 2,
    0x1B: 0, 0x1C: 1, 0x1D: 1, 0x1E: 1, 0x1F: 1, 0x20: 1,
    0x21: 2, 0x22: 2, 0x23: 4, 0x24: 1, 0x25: 3, 0x26: 3,
    0x27: 1, 0x28: 1, 0x29: 1,
}

NAMES = {
    0x00: "load_choice_texts", 0x01: "load_choice_targets",
    0x02: "conditional_dialog", 0x03: "set_choice_target",
    0x04: "set_choice_text", 0x05: "apply_party_template",
    0x06: "give_item", 0x07: "return_nested",
    0x08: "branch_if_has_item", 0x09: "branch_if_global_flag_set",
    0x0A: "set_global_flag", 0x0B: "clear_global_flag",
    0x0C: "toggle_global_flag", 0x0D: "branch_if_global_flag_clear",
    0x0E: "prompt_choices", 0x0F: "configure_timer",
    0x10: "draw_resource", 0x11: "display_refresh",
    0x12: "call_wordcode", 0x13: "append_choice",
    0x14: "remove_choice", 0x15: "request_interaction_exit",
    0x16: "suppress_npc_type", 0x17: "set_resource_subbase",
    0x18: "jump", 0x19: "set_text_base",
    0x1A: "prepend_choice", 0x1B: "terminate",
    0x1C: "play_sfx", 0x1D: "start_music",
    0x1E: "set_local_flag", 0x1F: "clear_local_flag",
    0x20: "toggle_local_flag", 0x21: "branch_if_local_flag_set",
    0x22: "branch_if_local_flag_clear", 0x23: "draw_random_resource",
    0x24: "play_cinematic_and_terminate", 0x25: "draw_resource_managed",
    0x26: "draw_resource_alt", 0x27: "set_sound_subbase",
    0x28: "remove_item", 0x29: "spawn_loose_item",
}


def parse_res(data: bytes):
    if len(data) < 4:
        raise ValueError("short RES")
    count = struct.unpack_from("<i", data, 0)[0]
    if count < 0 or 4 + 4 * count > len(data):
        raise ValueError("invalid RES count/table")
    offsets = struct.unpack_from(f"<{count}i", data, 4) if count else ()
    if count and offsets[0] != 4 + 4 * count:
        raise ValueError("first RES offset does not follow table")
    for a, b in zip(offsets, offsets[1:]):
        if b < a:
            raise ValueError("RES offsets decrease")
    entries = [data[o:(offsets[i + 1] if i + 1 < count else len(data))]
               for i, o in enumerate(offsets)]
    return entries


def decode_container(entry: bytes):
    if len(entry) < 2:
        raise ValueError("short wordcode container")
    count = struct.unpack_from("<H", entry, 0)[0]
    expected = 2 + count * 4
    if len(entry) != expected:
        raise ValueError(f"container bytes {len(entry)} != 2 + 4*{count} ({expected})")
    program = list(struct.unpack_from(f"<{count}H", entry, 2))
    zero_bank = list(struct.unpack_from(f"<{count}H", entry, 2 + count * 2))
    if any(zero_bank):
        raise ValueError("serialized companion bank is not all zero")
    return program


def decode_program(words: list[int]):
    ip = 0
    out = []
    while ip < len(words):
        start = ip
        op = words[ip]
        ip += 1
        if op > 0x29:
            raise ValueError(f"opcode {op:#x} at word {start} exceeds dispatcher range")
        if op in (0x00, 0x01):
            if ip >= len(words):
                raise ValueError(f"opcode {op:#x} missing count at {start}")
            count = words[ip]
            ip += 1
            if ip + count > len(words):
                raise ValueError(f"opcode {op:#x} list overruns program at {start}")
            args = [count] + words[ip:ip + count]
            ip += count
        elif op == 0x02:
            if ip >= len(words):
                raise ValueError(f"opcode 0x02 missing voice selector at {start}")
            begin = ip
            ip += 1
            while True:
                if ip + 1 >= len(words):
                    raise ValueError(f"opcode 0x02 unterminated condition/text list at {start}")
                condition = words[ip]
                ip += 2
                if condition == 0x21:
                    break
            args = words[begin:ip]
        else:
            arity = FIXED_ARITY[op]
            if ip + arity > len(words):
                raise ValueError(f"opcode {op:#x} operands overrun at {start}")
            args = words[ip:ip + arity]
            ip += arity
        out.append({"ip": start, "opcode": op, "name": NAMES[op], "args": args})
    return out


def targets(ins):
    op, a = ins["opcode"], ins["args"]
    if op in (0x08, 0x09, 0x0D, 0x21, 0x22):
        return [a[1]]
    if op in (0x12, 0x18):
        return [a[0]]
    if op == 0x0F:
        return [] if a[1] == 0xFFFF else [a[1]]
    if op == 0x01:
        return a[1:1 + a[0]]
    if op == 0x03:
        return [a[0]]
    if op in (0x13, 0x1A):
        return [a[1]]
    return []


def analyze(res9: Path, init: Path | None):
    entries = parse_res(res9.read_bytes())
    bank_start = 517
    init_values = None
    if init is not None:
        raw = init.read_bytes()
        if len(raw) < 0x24:
            raise ValueError("INIT shorter than 0x24-byte leading table")
        interaction_base = struct.unpack_from("<H", raw, 0x1E)[0]
        displacement = struct.unpack_from("<H", raw, 0x22)[0]
        bank_start = interaction_base + displacement
        init_values = {"interaction_base": interaction_base, "wordcode_displacement": displacement}
    if bank_start + 38 > len(entries):
        raise ValueError("active 38-program wordcode bank exceeds RES9 entry count")

    reports = []
    freq = Counter()
    bad_targets = []
    for pid in range(38):
        entry_index = bank_start + pid
        words = decode_container(entries[entry_index])
        decoded = decode_program(words)
        starts = {x["ip"] for x in decoded}
        for ins in decoded:
            freq[ins["opcode"]] += 1
            for target in targets(ins):
                if target not in starts:
                    bad_targets.append({"program": pid, "ip": ins["ip"], "opcode": ins["opcode"], "target": target})
        reports.append({
            "program_id": pid,
            "res9_entry": entry_index,
            "word_count": len(words),
            "instruction_count": len(decoded),
            "instructions": decoded,
        })

    timer_slots = sorted({ins["args"][0] for p in reports for ins in p["instructions"] if ins["opcode"] == 0x0F})
    invalid_timer_slots = [s for s in timer_slots if s > 4]
    unused = [op for op in range(0x2A) if freq[op] == 0]
    return {
        "res9_entry_count": len(entries),
        "wordcode_bank_start": bank_start,
        "init": init_values,
        "program_count": len(reports),
        "total_program_words": sum(p["word_count"] for p in reports),
        "total_instructions": sum(p["instruction_count"] for p in reports),
        "opcode_frequency": {f"0x{op:02X}": freq[op] for op in range(0x2A)},
        "unused_opcodes": [f"0x{op:02X}" for op in unused],
        "timer_slots_used": timer_slots,
        "invalid_timer_slots": invalid_timer_slots,
        "bad_control_flow_targets": bad_targets,
        "programs": reports,
    }


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("res9", type=Path)
    ap.add_argument("--init", type=Path)
    ap.add_argument("--json", action="store_true")
    ap.add_argument("--disassemble", action="store_true", help="include per-instruction text in non-JSON output")
    args = ap.parse_args()
    r = analyze(args.res9, args.init)
    if args.json:
        print(json.dumps(r, indent=2))
        return 0
    print(f"RES9 entries: {r['res9_entry_count']}")
    print(f"active wordcode bank: {r['wordcode_bank_start']}..{r['wordcode_bank_start'] + r['program_count'] - 1}")
    print(f"programs: {r['program_count']}; words: {r['total_program_words']}; decoded instructions: {r['total_instructions']}")
    print(f"unused shipped opcodes: {', '.join(r['unused_opcodes'])}")
    print(f"timer slots used: {r['timer_slots_used']}")
    print(f"bad control-flow targets: {len(r['bad_control_flow_targets'])}")
    if r['invalid_timer_slots']:
        print(f"invalid timer slots: {r['invalid_timer_slots']}")
        return 1
    if r['bad_control_flow_targets']:
        return 1
    if args.disassemble:
        for prog in r["programs"]:
            print(f"\nprogram {prog['program_id']} / RES9:{prog['res9_entry']} ({prog['word_count']} words)")
            for ins in prog["instructions"]:
                a = " ".join(f"0x{x:04X}" for x in ins["args"])
                print(f"  {ins['ip']:04X}: {ins['opcode']:02X} {ins['name']:<30} {a}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
