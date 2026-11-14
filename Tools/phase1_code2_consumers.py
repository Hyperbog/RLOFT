#!/usr/bin/env python3
"""Read-only CODE.2 consumer discovery; emits metadata, never original binary bytes."""
from __future__ import annotations
import argparse
import collections
import hashlib
import json
import re
import struct
import subprocess
import tempfile
from pathlib import Path

GAMES = {
    "menzo": {
        "dir": "Menzoberranzan", "exe": "CODE.2", "npc": "NPCS.DAT",
        "level": "LEVELS.DAT", "res": "RES4", "expected":
        "4a4fa725dddea00cb62ff44f44c48999cf6ed4c899b16c21826e8141ec7fda96",
    },
    "stone": {
        "dir": "Ravenloft - Stone Prophet", "exe": "PROPHET/CODE.2",
        "npc": "CD_Extracted/NPCS.DAT", "level": "CD_Extracted/LEVELS.DAT",
        "res": "PROPHET/RES4", "expected":
        "00e225ec8f141a8fdb72ff0547fe93b2ea3aabdb4ec3b59d9261a5268de9203e",
    },
}
LEXEMES = ("NPCS.DAT", "LEVELS.DAT", "OBJECTS.DAT", "RES", "INIT", "CHARS")

# Exact disassembled instruction witnesses: matched against authenticated CODE.2.
# No opaque byte patterns, guessed call targets, or source-code string grep.
CONSUMERS = {
    "menzo": {
        0x1AF55: "push 0xff0", 0x1AF63: "push 0x4",
        0x1AF82: "imul ebx,DWORD PTR ds:0x3a600,0x58",
        0x1B051: "push 0xffc", 0x1B087: "shl eax,0x6",
        0x1B08A: "add ebx,eax", 0x1BE41: "push 0x824",
        0x1BE53: "cmp eax,0x824", 0x1C210: "push 0x514",
        0x1C21E: "cmp eax,0x514", 0x1B69E: "push 0x1070",
        0x041C0: "imul ecx,esi,0x41c",
    },
    "stone": {
        0x20EDD: "push 0x124c", 0x20EEB: "push 0x4",
        0x20F0A: "imul ebx,DWORD PTR ds:0x480fc,0x58",
        0x20FD9: "push 0x1258", 0x21012: "shl ebx,0x6",
        0x21015: "add ebx,edx", 0x21E71: "push 0x824",
        0x21E83: "cmp eax,0x824", 0x2222D: "push 0x514",
        0x2223B: "cmp eax,0x514", 0x216B9: "push 0x1070",
        0x0473C: "imul ecx,esi,0x41c",
    },
}


def authenticated_consumer_witnesses(rows, title: str):
    by_addr = {addr: f"{opcode} {args}".strip() for addr,opcode,args in rows}
    for addr,expected in CONSUMERS[title].items():
        actual = by_addr.get(addr)
        if actual != expected:
            raise ValueError(f"{title} CODE.2 {addr:#x}: "
                             f"{actual!r} != expected instruction {expected!r}")
    return {"passed": len(CONSUMERS[title]),
            "instructions": [{"code_object_offset": pc, "decoded": ins}
                             for pc,ins in CONSUMERS[title].items()]}



def word(b: bytes, i: int) -> int:
    return struct.unpack_from("<I", b, i)[0]


def le_image(file: Path, expected: str):
    blob = file.read_bytes()
    actual = hashlib.sha256(blob).hexdigest()
    if actual != expected:
        raise ValueError(f"{file.name} CODE.2 SHA-256 mismatch {actual}")
    origin = 0x25D24
    if blob[origin:origin+2] != b"MZ":
        raise ValueError("nested DOS MZ absent")
    le = origin + word(blob, origin+0x3C)
    if blob[le:le+2] != b"LE":
        raise ValueError("nested LE absent")
    obj_table = le + word(blob, le+0x40)
    pages = origin + word(blob, le+0x80)
    output = []
    for i in range(2):
        record = obj_table + i*24
        virtual_size = word(blob, record)
        first_page = word(blob, record+12)
        count = word(blob, record+16)
        start = pages + (first_page-1)*4096
        if not count or virtual_size > 1_000_000 or start >= len(blob):
            raise ValueError("unsupported LE object")
        available = max(0, min(virtual_size, count*4096, len(blob)-start))
        output.append(blob[start:start+available] +
                      bytes(virtual_size-available))
    return output[0], output[1], {"sha256": actual,
                "le_file_offset": le, "code_bytes": len(output[0]),
                "data_bytes": len(output[1])}


def disassembly(code: bytes):
    with tempfile.TemporaryDirectory() as directory:
        image = Path(directory) / "object.bin"
        image.write_bytes(code)
        result = subprocess.run(
            ["objdump", "-D", "-b", "binary", "-m", "i386", "-M", "intel",
             "--no-show-raw-insn", str(image)],
            check=True, text=True, capture_output=True,
        )
    rows = []
    for line in result.stdout.splitlines():
        m = re.match(r"^\s*([0-9a-f]+):\s+([a-z][a-z0-9_.]*)\s*(.*)$", line)
        if m:
            rows.append((int(m[1], 16), m[2], m[3].strip()))
    return rows


def named_references(code: bytes, data: bytes, rows):
    names = {}
    for name in LEXEMES:
        pat = name.encode("ascii")
        offsets = [m.start() for m in re.finditer(re.escape(pat), data, re.I)
                   if m.end() < len(data) and data[m.end()] == 0]
        refs = []
        for offset in offsets:
            # Match *decoded* operand addresses: no bare byte-sequence guessing.
            target = f"0x{offset:x}"
            for i, (pc, instruction, operands) in enumerate(rows):
                if instruction not in ("push", "mov", "lea"):
                    continue
                # Only a direct immediate is a candidate string address.
                # Offsets inside [register + displacement] may numerically
                # equal a data-string address but are unrelated references.
                immediate = re.search(r"(?:^push 0x|^mov [a-z][a-z0-9]*,0x)", instruction + " " + operands)
                if not immediate:
                    continue
                vals = re.findall(r"0x([a-f0-9]+)", operands)
                if len(vals) == 1 and int(vals[0], 16) == offset:
                    contexts = [
                        {"offset": rows[j][0], "asm": rows[j][1] + " " + rows[j][2]}
                        for j in range(max(0, i-3), min(len(rows), i+30))
                    ]
                    refs.append({"data_offset": offset, "code_offset": pc,
                                 "instruction": instruction, "nearby": contexts})
        names[name] = {"data_offsets": offsets,
                       "candidate_code_references": refs[:12],
                       "total_reference_candidates": len(refs)}
    return names


def operand_witnesses(rows):
    values = (0x58, 0x824, 0x2A4, 0x514, 0x45, 0x41,
              0x1070, 0x41C, 0x248, 0x920)
    out = {}
    for value in values:
        hits = []
        for i, (pc, instruction, operands) in enumerate(rows):
            if instruction not in ("imul", "push", "cmp", "add", "sub", "mov",
                                   "lea", "shl"):
                continue
            # Immediate evidence is a candidate; memory displacement alone
            # is never considered a verified field reference.
            matches = re.findall(r"0x([a-f0-9]+)", operands)
            if not any(int(v, 16) == value for v in matches):
                continue
            hits.append({"at": pc, "instruction": instruction + " " + operands,
                         "next": [
                             f"{rows[j][0]:05x}: {rows[j][1]} {rows[j][2]}"
                             for j in range(i+1, min(len(rows), i+5))
                         ]})
        out[f"0x{value:x}"] = {"count": len(hits), "examples": hits[:24]}
    return out


def archive_records(path: Path):
    data = path.read_bytes()
    count = word(data, 0)
    if not 1 <= count <= 200000 or 4+4*count > len(data):
        raise ValueError(f"{path}: invalid count")
    offsets = struct.unpack_from(f"<{count}I", data, 4)
    if offsets[0] != 4+4*count or offsets[-1] > len(data) or any(
        a > b for a,b in zip(offsets, offsets[1:])
    ):
        raise ValueError(f"{path}: invalid offsets")
    return [data[a:b] for a,b in zip(offsets, (*offsets[1:], len(data)))]


def type_headers(archive: Path):
    samples = collections.defaultdict(list)
    for index, entry in enumerate(archive_records(archive)):
        if len(entry) < 10 or entry[:2] != b"EH":
            continue
        kind = entry[2]
        hlen = struct.unpack_from("<h", entry, 3)[0]
        if kind in (5, 6):
            if hlen < 0 or len(entry) < 5+hlen:
                raise ValueError(f"bad EH header {archive}:{index}")
            samples[(kind, hlen)].append((index, entry))
    out = {}
    for (kind, hlen), items in sorted(samples.items()):
        columns = []
        for col in range(hlen):
            freq = collections.Counter(entry[5+col] for _,entry in items)
            columns.append({"offset": col,
                            "distinct_values": len(freq),
                            "nonzero_entries": len(items)-freq.get(0,0),
                            "most_common": [{"value": x, "count": n}
                                            for x,n in freq.most_common(4)]})
        lengths = [len(rec)-5-hlen for _,rec in items]
        out[f"type{kind}_header{hlen}"] = {
            "count": len(items), "first_record": items[0][0],
            "first_record_size": len(items[0][1]), "payload_min": min(lengths),
            "payload_max": max(lengths), "columns": columns,
            "length_at_le16_header_0_matches": sum(
                struct.unpack_from("<H", b, 5)[0] == len(b)-5-hlen
                for _, b in items) if hlen >= 2 else None,
            "length_at_le32_header_0_matches": sum(
                word(b, 5) == len(b)-5-hlen for _, b in items)
                if hlen >= 4 else None,
        }
    return out


def npc_extensions(path: Path):
    b = path.read_bytes()
    count = word(b, 0)
    stride = (len(b)-4)//count
    if 4+count*stride != len(b) or stride != 0x58:
        raise ValueError(f"NPC record size not 88: {path}")
    records = [b[4+i*stride:4+(i+1)*stride] for i in range(count)]
    return {"records": count, "stride": stride,
            "after_strahd_0x45_byte_column_nonzero": [
                sum(r[i] != 0 for r in records)
                for i in range(0x45, 0x58)],
            "first_record_extra_nonzero_positions": [
                i for i in range(0x45,0x58) if records[0][i]
            ]}


def level_extra(path: Path):
    b=path.read_bytes()
    n=word(b,0)
    offsets=struct.unpack_from(f"<{n}I",b,4)
    sizes=collections.Counter()
    sparse=collections.Counter()
    for a,z in zip(offsets,(*offsets[1:],len(b))):
        rec=b[a:z]
        boundaries=struct.unpack_from("<7I",rec,1)
        width,height=struct.unpack_from("<hh",rec,0x1D+1)
        inferred=boundaries[1]-29-2*width*height
        if inferred <= 0 or boundaries[1] > len(rec):
            raise ValueError("invalid level header span")
        sizes[inferred]+=1
        # Capture only counts per byte position: no asset payload.
        for pos, byte in enumerate(rec[29+0x2a4:29+inferred],0x2a4):
            if byte:
                sparse[pos]+=1
    return {"records":n,"fixed_header_bytes_histogram":dict(sizes),
            "extension_start":0x2a4,
            "extension_end":max(sizes),
            "populated_extension_positions":len(sparse),
            "extension_nonzero_position_examples": [
                {"header_offset":p,"levels_nonzero":v}
                for p,v in sorted(sparse.items())[:30]]}


def audit(root: Path):
    output = {}
    for title,cfg in GAMES.items():
        folder=root/cfg["dir"]
        code,data,meta=le_image(folder/cfg["exe"],cfg["expected"])
        rows=disassembly(code)
        meta["decoded_instruction_starts"]=len(rows)
        output[title]={
            "original_executable":meta,
            "verified_consumers": authenticated_consumer_witnesses(rows,title),
            "strings_and_candidate_readsites": named_references(code,data,rows),
            "instruction_immediate_candidates": operand_witnesses(rows),
            "type5_type6": type_headers(folder/cfg["res"]),
            "npc_extension": npc_extensions(folder/cfg["npc"]),
            "level_header_extension":level_extra(folder/cfg["level"]),
        }
    return output


def main():
    ap=argparse.ArgumentParser()
    ap.add_argument("--root",required=True,type=Path)
    ap.add_argument("--json",required=True,type=Path)
    args=ap.parse_args()
    result=audit(args.root)
    args.json.parent.mkdir(parents=True,exist_ok=True)
    args.json.write_text(json.dumps(result,indent=2)+"\n")
    for title,item in result.items():
        print(f"{title}: CODE2 sha256={item['original_executable']['sha256']}"
              f" asm-instructions={item['original_executable']['decoded_instruction_starts']}")
        print(f"  authenticated code witnesses: "
              f"{item['verified_consumers']['passed']}/{len(CONSUMERS[title])}")
        for k,v in item["strings_and_candidate_readsites"].items():
            print(f"  {k}: data={v['data_offsets']}"
                  f" candidates={v['total_reference_candidates']}")
        for k,v in item["instruction_immediate_candidates"].items():
            print(f"  immediate {k}: {v['count']} occurrences")
        for k,v in item["type5_type6"].items():
            print(f"  {k}: {v['count']} records, payload "
                  f"{v['payload_min']}..{v['payload_max']}")
        print(f"  NPC: {item['npc_extension']['records']} at stride 0x58")
        print(f"  level header: {item['level_header_extension']['fixed_header_bytes_histogram']}")


if __name__=="__main__":
    main()
