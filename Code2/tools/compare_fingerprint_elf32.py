#!/usr/bin/env python3
"""Strict ELF32/i386 experiment comparator for original Ravenloft CODE.2.

Only source-unambiguous R_386_32 relocation sites may be resolved by
explicit symbol -> ORIGINAL CODE.2 DGROUP address. This tool does not claim
Watcom matching: legacy Watcom compilers generally emit OMF, not ELF.
Never upload original game bytes. Not a complete LE relocation engine.
"""
from __future__ import annotations

import argparse
import hashlib
import json
from pathlib import Path
import struct

CODE2_SHA = "853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314"
OBJ_OFFSET = 0x3AF24
OBJ_LENGTH = 0x3A852
PROBES = {
    "set_f0cc": (0x2E740, 0x2E74A, {"f0cc": 0xF0CC}),
    "set_f0c8": (0x2E74C, 0x2E756, {"f0c8": 0xF0C8}),
    "get_2b684": (0x3058C, 0x30592, {"g2b684": 0x2B684}),
    "get_2b680": (0x30594, 0x3059A, {"g2b680": 0x2B680}),
    "call_f0c8": (0x2E758, 0x2E768, {"f0c8": 0xF0C8}),
}


def _u16(data: bytes, off: int) -> int:
    return struct.unpack_from("<H", data, off)[0]


def _u32(data: bytes, off: int) -> int:
    return struct.unpack_from("<I", data, off)[0]


def _check_slice(data: bytes, off: int, count: int) -> bytes:
    if off < 0 or count < 0 or off + count > len(data):
        raise ValueError("truncated ELF object section")
    return data[off:off + count]


def extract_elf_function(path: Path, symbol_name: str) -> tuple[bytes, list[tuple[int, int, str, int]]]:
    """Get function bytes and REL relocations (relative offset,type,symbol,addend).

    Bounds/type checks intentionally exclude ELF64, shared objects, RELA,
    section groups, weak aliases and more advanced object layouts.
    """
    data = path.read_bytes()
    if len(data) < 52 or data[:4] != b"\x7fELF" or data[4:7] != b"\x01\x01\x01":
        raise ValueError("requires little-endian ELF32 relocatable object")
    if _u16(data, 16) != 1 or _u16(data, 18) != 3:
        raise ValueError("requires ELF32 ET_REL machine EM_386")
    shoff, shentsize, shnum, shstr = _u32(data, 32), _u16(data, 46), _u16(data, 48), _u16(data, 50)
    if shentsize != 40 or shnum == 0 or shstr >= shnum:
        raise ValueError("unsupported section header layout")
    sections = []
    for idx in range(shnum):
        raw = _check_slice(data, shoff + 40 * idx, 40)
        sections.append(struct.unpack("<10I", raw))
    shstrtab = _check_slice(data, sections[shstr][4], sections[shstr][5])

    def cstr(table: bytes, at: int) -> str:
        if at >= len(table):
            raise ValueError("string table offset out of range")
        end = table.find(b"\x00", at)
        if end == -1:
            raise ValueError("unterminated string-table entry")
        return table[at:end].decode("ascii", errors="strict")

    names = [cstr(shstrtab, s[0]) for s in sections]
    symtabs = [i for i, s in enumerate(sections) if s[1] == 2]
    if len(symtabs) != 1:
        raise ValueError("requires exactly one SYMTAB")
    symtab_id = symtabs[0]
    sh = sections[symtab_id]
    if sh[9] != 16 or sh[5] % 16 or sh[6] >= shnum:
        raise ValueError("unsupported symbol table layout")
    stab = _check_slice(data, sh[4], sh[5])
    strtab = _check_slice(data, sections[sh[6]][4], sections[sh[6]][5])
    symbols = []
    for i in range(0, len(stab), 16):
        name, value, size, info, other, shndx = struct.unpack_from("<IIIBBH", stab, i)
        symbols.append((cstr(strtab, name), value, size, info, other, shndx))
    funcs = [s for s in symbols if s[0] == symbol_name and (s[3] & 0xF) == 2]
    if len(funcs) != 1:
        raise ValueError("expected exactly one named STT_FUNC")
    func = funcs[0]
    section_id = func[5]
    if section_id == 0 or section_id >= shnum or sections[section_id][1] != 1:
        raise ValueError("function is not in a PROGBITS section")
    section = sections[section_id]
    if not func[2] or func[1] + func[2] > section[5]:
        raise ValueError("invalid or zero function size")
    chunk = _check_slice(data, section[4] + func[1], func[2])
    relocs = []
    for sect in sections:
        if sect[1] == 4 and sect[7] == section_id:
            raise ValueError("RELA relocation not supported")
        if sect[1] != 9 or sect[7] != section_id:
            continue
        if sect[6] != symtab_id or sect[9] != 8 or sect[5] % 8:
            raise ValueError("unsupported REL relocation table")
        raw = _check_slice(data, sect[4], sect[5])
        for i in range(0, len(raw), 8):
            roff, rinfo = struct.unpack_from("<II", raw, i)
            idx, rtype = rinfo >> 8, rinfo & 0xFF
            if idx >= len(symbols):
                raise ValueError("relocation symbol out of range")
            if func[1] <= roff < func[1] + func[2]:
                offset = roff - func[1]
                if offset + 4 > len(chunk):
                    raise ValueError("relocation straddles function end")
                relocs.append((offset, rtype, symbols[idx][0], _u32(chunk, offset)))
    # Duplicated site entries are ambiguous; no silent relocation masking.
    offsets = [r[0] for r in relocs]
    if len(offsets) != len(set(offsets)):
        raise ValueError("multiple relocations at the same location")
    return chunk, sorted(relocs)


def compare_probe(code2: Path, candidate: Path, name: str) -> dict:
    if name not in PROBES:
        raise ValueError(f"unknown pinned CODE.2 probe {name}")
    original = code2.read_bytes()
    if len(original) != 558689 or hashlib.sha256(original).hexdigest() != CODE2_SHA:
        raise ValueError("original CODE.2 identity mismatch")
    start, end, address_map = PROBES[name]
    expected = original[OBJ_OFFSET + start:OBJ_OFFSET + end]
    emitted, relocations = extract_elf_function(candidate, name)
    fixed = bytearray(emitted)
    reloc_results = []
    for off, rtype, symbol, addend in relocations:
        if rtype != 1:  # R_386_32, absolute little-endian DWORD
            raise ValueError(f"unsupported relocation R_386_{rtype} at {off:#x}")
        if symbol not in address_map:
            raise ValueError(f"unrecognized relocation symbol {symbol!r}")
        resolved = (address_map[symbol] + addend) & 0xFFFFFFFF
        struct.pack_into("<I", fixed, off, resolved)
        reloc_results.append({"offset": off, "type": "R_386_32", "symbol": symbol,
                              "original_address": hex(address_map[symbol]), "addend": addend,
                              "resolved": hex(resolved)})
    # Empty/missing relocation is not treated as verified just because an
    # emitted object coincidentally hardcoded the target's physical address.
    if address_map and not reloc_results:
        raise ValueError("no provable relocation for target global")
    for symbol in address_map:
        if symbol not in [r["symbol"] for r in reloc_results]:
            raise ValueError(f"expected relocation missing: {symbol}")
    matches = sum(x == y for x, y in zip(expected, fixed))
    exact = len(fixed) == len(expected) and matches == len(expected)
    return {
        "schema": "rloft-elf32-negative-control-v1",
        "probe": name, "target_range": [hex(start), hex(end)],
        "target_sha256": hashlib.sha256(expected).hexdigest(),
        "emitted_sha256_unrelocated": hashlib.sha256(emitted).hexdigest(),
        "emitted_length": len(emitted), "original_length": len(expected),
        "relocations": reloc_results, "equal_bytes_at_same_offsets": matches,
        "equal_length": len(fixed) == len(expected),
        "relocation_resolved_exact_bytes": exact,
        "compiler_identified": False,
        "dos_runtime_parity": "not_tested",
        "classification": "ELF32_control_not_Watcom_compiler_identification",
        "first_mismatch_offset": next((i for i,(a,b) in enumerate(zip(expected,fixed)) if a != b),
                                      None if len(fixed)==len(expected) else min(len(fixed),len(expected))),
    }


def main() -> None:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--original-code2", type=Path, required=True)
    ap.add_argument("--candidate-elf32", type=Path, required=True)
    ap.add_argument("--probe", choices=sorted(PROBES), required=True)
    ap.add_argument("--output", type=Path, help="Write metadata-only JSON comparison")
    args = ap.parse_args()
    report = compare_probe(args.original_code2, args.candidate_elf32, args.probe)
    content = json.dumps(report, indent=2, sort_keys=True) + "\n"
    if args.output:
        args.output.write_text(content, encoding="utf-8")
    else:
        print(content, end="")

if __name__ == "__main__":
    main()
