#!/usr/bin/env python3
"""Read-only CODE.2 resource dispatcher and level-event interpreter finder.

Candidate windows are investigation leads, not verified function identities.
Authenticated original LE code is disassembled to instruction metadata only.
"""
from __future__ import annotations
import argparse
import collections
import hashlib
import json
import re
import runpy
from pathlib import Path
from struct import unpack_from

WINDOWS = {
    "menzo": {
        "level_event_vm": (0x200, 0x720),
        "EH_resource_open": (0x34B80, 0x34F20),
    },
    "stone": {
        "level_event_vm": (0x298, 0x7B0),
        "EH_resource_open": (0x40B40, 0x40ED0),
    },
}


def bounded_instructions(rows, code: bytes, start: int, end: int):
    if start < 0 or end > len(code) or start >= end:
        raise ValueError("bounded disassembly range outside authenticated code")
    observed = []
    tables = []
    for pc, op, args in rows:
        if not (start <= pc < end):
            continue
        observed.append({"offset": pc, "op": op, "args": args})
        if op == "jmp" and "DWORD PTR" in args and "*4" in args:
            m = re.search(r"\\+0x([0-9a-f]+)\\]", args)
            if m:
                table = int(m.group(1), 16)
                if 0 <= table < len(code) and table + 4*16 <= len(code):
                    pointers = list(unpack_from("<16I", code, table))
                    offsets = [v for v in pointers if v < len(code)]
                    tables.append({"jump_at": pc, "table_offset": table,
                                   "entry_values": pointers[:16],
                                   "entries_within_object": len(offsets)})
    return {"range": [start,end], "instructions": observed,
            "jump_table_candidates": tables}



def around(rows, index: int, before: int = 12, after: int = 17):
    return [
        {"offset": pc, "instruction": op, "operands": arg}
        for pc, op, arg in rows[max(0, index - before):
                             min(len(rows), index + after + 1)]
    ]


def includes_immediate(arg: str, value: int):
    return any(int(v, 16) == value
               for v in re.findall(r"0x([0-9a-f]+)", arg))


def find_magic(rows):
    hits = []
    for i, (pc, opcode, args) in enumerate(rows):
        if opcode not in ("cmp", "mov", "test"):
            continue
        if includes_immediate(args, 0x4845) or includes_immediate(args, 0x4548):
            hits.append({"at": pc, "opcode": opcode, "operand": args,
                         "context": around(rows, i, 13, 23)})
    return {"count": len(hits), "candidates": hits[:12]}


def find_table_switches(rows):
    out = []
    for i, (pc, opcode, args) in enumerate(rows):
        if opcode != "jmp" or "DWORD PTR" not in args:
            continue
        if not any(scale in args for scale in ("*4", "*8")):
            continue
        earlier = rows[max(0, i - 36):i]
        score = 0
        if any(op == "cmp" and includes_immediate(arg, 7)
               for _, op, arg in earlier):
            score += 5
        if any(op in ("movzx", "movsx") and "BYTE PTR" in arg
               for _, op, arg in earlier):
            score += 3
        if any(op == "cmp" and includes_immediate(arg, 5)
               for _, op, arg in earlier):
            score += 1
        if any(op == "cmp" and includes_immediate(arg, 6)
               for _, op, arg in earlier):
            score += 1
        if score > 0:
            out.append({"at": pc, "score": score,
                        "context": around(rows, i, 21, 9)})
    out.sort(key=lambda r: (-r["score"], r["at"]))
    return {"count": len(out), "candidates": out[:18]}


def find_event_scales(rows):
    out = []
    for i, (pc, opcode, args) in enumerate(rows):
        if opcode != "imul" or not includes_immediate(args, 0x1A):
            continue
        near = rows[max(0, i-65):min(len(rows), i+95)]
        scores = collections.Counter()
        if any(op == "cmp" and includes_immediate(arg, 5)
               for _, op, arg in near):
            scores["cmd_count_limit_5"] = 1
        if any(op == "cmp" and includes_immediate(arg, 12)
               for _, op, arg in near):
            scores["opcode_limit_12"] = 1
        if any(op == "imul" and includes_immediate(arg, 5)
               for _, op, arg in near):
            scores["command_stride_5"] = 1
        if any(op == "movzx" and "BYTE PTR" in arg
               for _, op, arg in near):
            scores["bytecode_fetch"] = 1
        out.append({"at": pc, "instruction": opcode + " " + args,
                    "nearby_signals": dict(scores),
                    "context": around(rows, i, 14, 18)})
    return {"count": len(out), "candidates": out[:24]}


def find_value_reads(rows, value: int, instruction="cmp"):
    out = []
    for i, (pc, op, arg) in enumerate(rows):
        if op != instruction or not includes_immediate(arg, value):
            continue
        if instruction == "cmp" and not ("BYTE PTR" in arg or "WORD PTR" in arg):
            continue
        out.append({"at": pc, "instruction": op + " " + arg,
                    "context": around(rows, i, 7, 11)})
    return {"count": len(out), "candidates": out[:15]}


def ordinal_events(root: Path):
    game = root / "Ravenloft - Stone Prophet"
    # No loaded original data bytes are emitted, only differences and lengths.
    from struct import unpack_from
    outcome = {}
    for source, p in (
        ("installed", game / "PROPHET/RES9"),
        ("cd", game / "CD_Extracted/RES9")
    ):
        b = p.read_bytes()
        n = unpack_from("<I", b)[0]
        a = unpack_from(f"<{n}I", b, 4)
        start, end = a[574], a[575]
        entry = b[start:end]
        wc = unpack_from("<H", entry)[0]
        outcome[source] = {
            "sha256": hashlib.sha256(b).hexdigest(),
            "entry574_length": len(entry),
            "entry574_archive_offset": start,
            "word_count": wc,
            "two_bank_wordcode_shape": len(entry) == 2+4*wc,
            "one_bank_wordcode_shape": len(entry) == 2+2*wc,
        }
    return outcome


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--root", type=Path, required=True)
    parser.add_argument("--repo-root", type=Path, default=Path("."))
    parser.add_argument("--json", type=Path, required=True)
    args = parser.parse_args()
    consumer = runpy.run_path(str(args.repo_root / "Tools/phase1_code2_consumers.py"))
    result = {}
    for title in ("menzo", "stone"):
        cfg = consumer["GAMES"][title]
        folder = args.root / cfg["dir"]
        code, _, meta = consumer["le_image"](
            folder / cfg["exe"], cfg["expected"])
        rows = consumer["disassembly"](code)
        result[title] = {
            "code2_sha256": meta["sha256"],
            "machine_instructions": len(rows),
            "EH_signature_comparisons": find_magic(rows),
            "indirect_dispatch_tables": find_table_switches(rows),
            "candidate_26byte_level_script_slots": find_event_scales(rows),
            "type5_byte_comparisons": find_value_reads(rows, 5),
            "type6_byte_comparisons": find_value_reads(rows, 6),
            "opcode12_comparisons": find_value_reads(rows, 12),
            "bounded_dispatch_windows": {
                name: bounded_instructions(rows, code, *region)
                for name,region in WINDOWS[title].items()
            },
        }
        print(f"{title}: EH cmp={result[title]['EH_signature_comparisons']['count']}, "
              f"jmp tables={result[title]['indirect_dispatch_tables']['count']}, "
              f"imul_1A={result[title]['candidate_26byte_level_script_slots']['count']}, "
              f"cmp byte 5={result[title]['type5_byte_comparisons']['count']}, "
              f"cmp byte 6={result[title]['type6_byte_comparisons']['count']}")
        for key in ("EH_signature_comparisons",
                    "candidate_26byte_level_script_slots",
                    "indirect_dispatch_tables"):
            vals = result[title][key]["candidates"]
            print(f"  {key} candidates:", [hex(x["at"]) for x in vals[:12]])
    result["stone_res9_record574"] = ordinal_events(args.root)
    args.json.parent.mkdir(parents=True, exist_ok=True)
    args.json.write_text(json.dumps(result, indent=2) + "\n",
                         encoding="utf-8")


if __name__ == "__main__":
    main()
