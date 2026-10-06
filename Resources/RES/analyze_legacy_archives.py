#!/usr/bin/env python3
"""Validate historical lineage between Ravenloft RES11/RES12 and active split archives."""
from __future__ import annotations

import argparse
import collections
import difflib
import hashlib
import json
import struct
from pathlib import Path

CODE2_SHA256 = "853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314"
CODE2_TEMPLATE_FILE_OFFSET = 0x8072C
CHARACTER_RECORD_BYTES = 0x248
LEGACY_TEMPLATE_WORDS = 141
TEMPLATE_NAMES = [
    "GLORIANNA", "VELIKA", "MISCHA", "VUKO", "TATIAN",
    "IRMGARDE", "ANSHELM", "RYSZARD", "VLADISLAV", "FHALKEN",
]

ACTIVE_ARITY = {
    0x03: 2, 0x04: 2, 0x05: 1, 0x06: 1, 0x07: 0, 0x08: 2,
    0x09: 2, 0x0A: 1, 0x0B: 1, 0x0C: 1, 0x0D: 2, 0x0E: 0,
    0x0F: 3, 0x10: 3, 0x11: 0, 0x12: 1, 0x13: 2, 0x14: 1,
    0x15: 0, 0x16: 1, 0x17: 1, 0x18: 1, 0x19: 1, 0x1A: 2,
    0x1B: 0, 0x1C: 1, 0x1D: 1, 0x1E: 1, 0x1F: 1, 0x20: 1,
    0x21: 2, 0x22: 2, 0x23: 4, 0x24: 1, 0x25: 3, 0x26: 3,
    0x27: 1, 0x28: 1, 0x29: 1,
}
LEGACY_ARITY = dict(ACTIVE_ARITY)
LEGACY_ARITY[0x05] = LEGACY_TEMPLATE_WORDS


def parse_res(path: Path) -> list[bytes]:
    data = path.read_bytes()
    if len(data) < 4:
        raise ValueError(f"{path}: short RES")
    count = struct.unpack_from("<i", data, 0)[0]
    if count < 0 or 4 + 4 * count > len(data):
        raise ValueError(f"{path}: invalid entry count")
    offsets = struct.unpack_from(f"<{count}i", data, 4) if count else ()
    if count and offsets[0] != 4 + 4 * count:
        raise ValueError(f"{path}: first entry offset does not follow table")
    if any(b < a for a, b in zip(offsets, offsets[1:])):
        raise ValueError(f"{path}: decreasing entry offset")
    return [
        data[o:(offsets[i + 1] if i + 1 < count else len(data))]
        for i, o in enumerate(offsets)
    ]


def entry_type(entry: bytes):
    if not entry:
        return "empty"
    if len(entry) >= 5 and entry[:2] == b"EH":
        return entry[2]
    return "raw"


def type_runs(entries: list[bytes]):
    out = []
    if not entries:
        return out
    start = 0
    current = entry_type(entries[0])
    for i, entry in enumerate(entries[1:], 1):
        t = entry_type(entry)
        if t != current:
            out.append((start, i - 1, current))
            start, current = i, t
    out.append((start, len(entries) - 1, current))
    return out


def digest(entry: bytes) -> bytes:
    return hashlib.sha256(entry).digest()


def exact_match_count(old: list[bytes], new: list[bytes]) -> int:
    hashes = {digest(e) for e in new}
    return sum(digest(e) in hashes for e in old)


def matching_blocks(old: list[bytes], new: list[bytes]):
    return [
        x for x in difflib.SequenceMatcher(
            a=[digest(x) for x in old],
            b=[digest(x) for x in new],
            autojunk=False,
        ).get_matching_blocks()
        if x.size
    ]


def decoded_size_type6(entry: bytes) -> int:
    if len(entry) < 7 or entry[:2] != b"EH" or entry[2] != 6:
        raise ValueError("not type-6")
    return struct.unpack_from("<H", entry, 5)[0]


def decode_container(entry: bytes) -> list[int]:
    if len(entry) < 2:
        raise ValueError("short wordcode container")
    count = struct.unpack_from("<H", entry, 0)[0]
    expected = 2 + count * 4
    if len(entry) != expected:
        raise ValueError(f"wordcode container {len(entry)} != {expected}")
    program = list(struct.unpack_from(f"<{count}H", entry, 2))
    companion = struct.unpack_from(f"<{count}H", entry, 2 + count * 2)
    if any(companion):
        raise ValueError("non-zero companion bank")
    return program


def decode_program(words: list[int], arity: dict[int, int]):
    ip = 0
    out = []
    while ip < len(words):
        start = ip
        op = words[ip]
        ip += 1
        if op > 0x29:
            raise ValueError(f"opcode {op:#x} at word {start}")
        if op in (0x00, 0x01):
            if ip >= len(words):
                raise ValueError(f"opcode {op:#x} missing count")
            count = words[ip]
            ip += 1
            if ip + count > len(words):
                raise ValueError(f"opcode {op:#x} list overrun")
            args = [count] + words[ip:ip + count]
            ip += count
        elif op == 0x02:
            begin = ip
            if ip >= len(words):
                raise ValueError("opcode 0x02 missing selector")
            ip += 1
            while True:
                if ip + 1 >= len(words):
                    raise ValueError("opcode 0x02 unterminated list")
                condition = words[ip]
                ip += 2
                if condition == 0x21:
                    break
            args = words[begin:ip]
        else:
            count = arity[op]
            if ip + count > len(words):
                raise ValueError(f"opcode {op:#x} operand overrun")
            args = words[ip:ip + count]
            ip += count
        out.append({"ip": start, "opcode": op, "args": args})
    return out


def targets(ins: dict):
    op, args = ins["opcode"], ins["args"]
    if op in (0x08, 0x09, 0x0D, 0x21, 0x22):
        return [args[1]]
    if op in (0x12, 0x18):
        return [args[0]]
    if op == 0x0F:
        return [] if args[1] == 0xFFFF else [args[1]]
    if op == 0x01:
        return args[1:1 + args[0]]
    if op == 0x03:
        return [args[0]]
    if op in (0x13, 0x1A):
        return [args[1]]
    return []


def first_args(instructions, opcode: int):
    return [x["args"][0] for x in instructions if x["opcode"] == opcode]


def signed16(value: int) -> int:
    return value if value < 0x8000 else value - 0x10000


def template_name(words: list[int]) -> str:
    return bytes(x & 0xFF for x in words[:16]).split(b"\0", 1)[0].decode("ascii")


def expand_template_prefix(words: list[int]) -> bytes:
    if len(words) != 141:
        raise ValueError("legacy template must contain 141 words")
    out = bytearray(0x10A)
    for i, value in enumerate(words[:16]):
        out[i] = value & 0xFF
    out[0x12] = words[16] & 0xFF
    out[0x13] = words[17] & 0xFF
    out[0x14] = words[18] & 0xFF
    for i in range(3):
        out[0x15 + i] = words[20 + i] & 0xFF
    struct.pack_into("<H", out, 0x18, words[19] & 0xFFFF)
    out[0x1A] = words[23] & 0xFF
    out[0x1B] = words[24] & 0xFF
    struct.pack_into("<h", out, 0x1C, signed16(words[25]))
    struct.pack_into("<h", out, 0x1E, signed16(words[26]))
    for i in range(3):
        struct.pack_into("<h", out, 0x20 + 2 * i, signed16(words[27 + i]))
        struct.pack_into("<i", out, 0x26 + 4 * i, signed16(words[30 + i]))
    abilities = [words[33], words[35], words[36], words[37], words[38], words[39], words[34]]
    for i, value in enumerate(abilities):
        out[0x32 + 2 * i] = value & 0xFF
        out[0x33 + 2 * i] = value & 0xFF
    for i in range(21):
        struct.pack_into("<h", out, 0x40 + 2 * i, signed16(words[40 + i]))
    out[0x6A:0x10A] = struct.pack("<80H", *words[61:141])
    return bytes(out)


def xdir_sequence_count(entry: bytes) -> int:
    if len(entry) < 31 or entry[:2] != b"EH" or entry[2] != 3:
        raise ValueError("not type-3 XDIR")
    declared = struct.unpack_from("<I", entry, 5)[0]
    blob = entry[9:9 + declared]
    if blob[:4] != b"FORM" or blob[8:12] != b"XDIR" or blob[12:16] != b"INFO":
        raise ValueError("unexpected XDIR envelope")
    if int.from_bytes(blob[16:20], "big") != 2:
        raise ValueError("unexpected XDIR INFO length")
    return struct.unpack_from("<H", blob, 20)[0]


def size_runs(entries: list[bytes]):
    out = []
    start = 0
    previous = decoded_size_type6(entries[0])
    for i, entry in enumerate(entries[1:], 1):
        current = decoded_size_type6(entry)
        if current != previous:
            out.append((start, i - 1, previous))
            start, previous = i, current
    out.append((start, len(entries) - 1, previous))
    return out


def analyze(root: Path, code2_path: Path):
    A = {i: parse_res(root / f"RES{i}") for i in (3, 4, 5, 6, 9, 10, 11, 12)}
    r3, r4, r5, r6 = A[3], A[4], A[5], A[6]
    r9, r10, r11, r12 = A[9], A[10], A[11], A[12]

    expected11 = [(0, 4127, 7), (4128, 4779, 6), (4780, 4853, 3), (4854, 5113, 4)]
    expected12 = [(0, 369, 1), (370, 406, "raw"), (407, 666, 4)]
    if type_runs(r11) != expected11:
        raise ValueError("unexpected RES11 partition")
    if type_runs(r12) != expected12:
        raise ValueError("unexpected RES12 partition")
    if r11[4854:5114] != r12[407:667]:
        raise ValueError("RES11/RES12 legacy VOC banks differ")

    legacy_t6 = r11[4128:4780]
    if size_runs(legacy_t6) != [(0, 584, 8192), (585, 651, 4096)]:
        raise ValueError("unexpected RES11 texture split")
    if size_runs(r4) != [(0, 599, 8192), (600, 666, 4096)]:
        raise ValueError("unexpected RES4 texture split")

    exact = {
        "res11_type7_to_res3": exact_match_count(r11[:4128], r3),
        "res11_type6_to_res4": exact_match_count(legacy_t6, r4),
        "res11_type3_to_res5": exact_match_count(r11[4780:4854], r5),
        "res11_type4_to_res6": exact_match_count(r11[4854:5114], r6),
        "res12_type1_to_res9_typed": exact_match_count(r12[:370], r9[:517]),
        "res12_type4_to_res10": exact_match_count(r12[407:667], r10),
        "res12_type4_to_res6": exact_match_count(r12[407:667], r6),
    }
    expected_exact = {
        "res11_type7_to_res3": 2402,
        "res11_type6_to_res4": 609,
        "res11_type3_to_res5": 0,
        "res11_type4_to_res6": 236,
        "res12_type1_to_res9_typed": 255,
        "res12_type4_to_res10": 5,
        "res12_type4_to_res6": 236,
    }
    if exact != expected_exact:
        raise ValueError(f"exact-match census changed: {exact}")

    t7 = matching_blocks(r11[:4128], r3)
    if not any(x.a == 3221 and x.b == 3316 and x.size >= 249 for x in t7):
        raise ValueError("missing RES11->RES3 class-2 anchor")
    if not any(x.a == 4055 and x.b == 4158 and x.size >= 65 for x in t7):
        raise ValueError("missing RES11->RES3 class-3 anchor")
    gfx = matching_blocks(r12[:370], r9[:517])
    if not any(x.a == 0 and x.b == 0 and x.size >= 31 for x in gfx):
        raise ValueError("missing RES12->RES9 initial graphics anchor")
    if not any(x.a == 51 and x.b == 103 and x.size >= 7 for x in gfx):
        raise ValueError("missing RES12->RES9 interaction-base anchor")

    lm = r11[4780:4854]
    lp0 = [xdir_sequence_count(x) for x in lm[:37]]
    lp1 = [xdir_sequence_count(x) for x in lm[37:]]
    fp0 = [xdir_sequence_count(x) for x in r5[:49]]
    fp1 = [xdir_sequence_count(x) for x in r5[49:]]
    if lp0 != lp1 or fp0 != fp1:
        raise ValueError("paired XMIDI bank profiles do not agree")

    legacy = []
    inline = []
    bad = []
    timers = set()
    freq = collections.Counter()
    for pid in range(37):
        words = decode_container(r12[370 + pid])
        decoded = decode_program(words, LEGACY_ARITY)
        starts = {x["ip"] for x in decoded}
        for ins in decoded:
            freq[ins["opcode"]] += 1
            if ins["opcode"] == 0x0F:
                timers.add(ins["args"][0])
            if ins["opcode"] == 0x05:
                inline.append((pid, ins["ip"], template_name(ins["args"]), ins["args"]))
            for target in targets(ins):
                if target not in starts:
                    bad.append((pid, ins["ip"], ins["opcode"], target))
        legacy.append((words, decoded))
    if bad:
        raise ValueError(f"legacy VM bad targets: {bad[:5]}")

    code2 = code2_path.read_bytes()
    if hashlib.sha256(code2).hexdigest() != CODE2_SHA256:
        raise ValueError("CODE.2 identity mismatch")
    static = []
    for tid, expected in enumerate(TEMPLATE_NAMES):
        off = CODE2_TEMPLATE_FILE_OFFSET + tid * CHARACTER_RECORD_BYTES
        record = code2[off:off + CHARACTER_RECORD_BYTES]
        name = record[:16].split(b"\0", 1)[0].decode("ascii")
        if name != expected:
            raise ValueError(f"template {tid}: {name!r} != {expected!r}")
        static.append(record)
    id_by_name = {name: i for i, name in enumerate(TEMPLATE_NAMES)}
    if {name for _, _, name, _ in inline} != set(TEMPLATE_NAMES):
        raise ValueError("inline template names do not cover final template set")

    active = [
        decode_program(decode_container(r9[517 + pid]), ACTIVE_ARITY)
        for pid in range(38)
    ]
    for pid in range(37):
        if first_args(legacy[pid][1], 0x19) != first_args(active[pid], 0x19):
            raise ValueError(f"program {pid}: text-base mismatch")

    similarity = []
    saw_ir = False
    for pid, ip, name, payload in inline:
        tid = id_by_name[name]
        prefix = expand_template_prefix(payload)
        final = static[tid][:0x10A]
        equal = sum(a == b for a, b in zip(prefix, final))
        similarity.append((name, pid, equal))
        if pid == 14 and name == "IRMGARDE":
            if first_args(active[14], 0x05):
                raise ValueError("active program 14 still contains template operation")
            if first_args(active[37], 0x05) != [5, 5]:
                raise ValueError("active program 37 does not apply IRMGARDE twice")
            for op in (0x17, 0x19, 0x27):
                if first_args(active[14], op) != first_args(active[37], op):
                    raise ValueError("program 14/37 identity mismatch")
            saw_ir = True
        elif tid not in first_args(active[pid], 0x05):
            raise ValueError(f"program {pid}: legacy {name} ID {tid} absent in active program")
    if not saw_ir:
        raise ValueError("IRMGARDE migration not observed")

    base_matches = 0
    for pid in range(37):
        old = first_args(legacy[pid][1], 0x17)
        new = first_args(active[pid], 0x17)
        if len(old) == len(new) == 1:
            oi, ni = 51 + old[0], 103 + new[0]
            if oi < 370 and ni < 517 and r12[oi] == r9[ni]:
                base_matches += 1

    return {
        "res11_runs": type_runs(r11),
        "res12_runs": type_runs(r12),
        "exact_match_counts": exact,
        "legacy_type6_runs": size_runs(legacy_t6),
        "final_type6_runs": size_runs(r4),
        "legacy_xmidi_half_profile": dict(collections.Counter(lp0)),
        "final_xmidi_half_profile": dict(collections.Counter(fp0)),
        "legacy_programs": 37,
        "legacy_program_words": sum(len(x[0]) for x in legacy),
        "legacy_instructions": sum(len(x[1]) for x in legacy),
        "bad_targets": len(bad),
        "timer_slots": sorted(timers),
        "legacy_opcode05_words": 141,
        "legacy_opcode05_occurrences": len(inline),
        "inline_templates": [(pid, ip, name) for pid, ip, name, _ in inline],
        "template_ids": list(enumerate(TEMPLATE_NAMES)),
        "template_prefix_similarity": similarity,
        "shared_text_bases": 37,
        "legacy_interaction_base": 51,
        "final_interaction_base": 103,
        "legacy_wordcode_displacement": 319,
        "base_resource_exact_matches": base_matches,
        "shared_legacy_voc_entries": 260,
        "opcode_frequency": dict(sorted(freq.items())),
    }


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("root", type=Path, help="directory containing RES3/4/5/6/9/10/11/12")
    ap.add_argument("--code2", type=Path, required=True)
    ap.add_argument("--json", action="store_true")
    args = ap.parse_args()
    r = analyze(args.root, args.code2)
    if args.json:
        print(json.dumps(r, indent=2))
        return 0
    print(f"RES11 partitions: {r['res11_runs']}")
    print(f"RES12 partitions: {r['res12_runs']}")
    for key, value in r["exact_match_counts"].items():
        print(f"{key}: {value}")
    print(f"legacy type-6 split: {r['legacy_type6_runs']}")
    print(f"final type-6 split:  {r['final_type6_runs']}")
    print(f"legacy XMIDI half profile: {r['legacy_xmidi_half_profile']}")
    print(f"final XMIDI half profile:  {r['final_xmidi_half_profile']}")
    print(
        f"legacy VM: {r['legacy_programs']} programs, {r['legacy_program_words']} words, "
        f"{r['legacy_instructions']} instructions, {r['bad_targets']} bad targets"
    )
    print(
        f"opcode 05 legacy payload: {r['legacy_opcode05_words']} words; "
        f"occurrences={r['legacy_opcode05_occurrences']}"
    )
    print("template IDs:", ", ".join(f"{i}={name}" for i, name in r["template_ids"]))
    for pid, ip, name in r["inline_templates"]:
        print(f"  program {pid:2d} @ word {ip:3d}: {name}")
    print(
        f"text bases preserved: {r['shared_text_bases']}/37; "
        f"interaction base {r['legacy_interaction_base']} -> {r['final_interaction_base']}; "
        f"base resource exact matches {r['base_resource_exact_matches']}/37"
    )
    print("template-prefix equal bytes (of 266):")
    for name, pid, count in r["template_prefix_similarity"]:
        print(f"  {name:<10} program {pid:2d}: {count}/266")
    print("legacy lineage validation: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
