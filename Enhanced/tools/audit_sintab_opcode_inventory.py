#!/usr/bin/env python3
"""Audit the exact original SINTAB generated rasterizer opcode vocabulary.

Usage: python3 DemoV1/tools/audit_sintab_opcode_inventory.py /game/SINTAB

This does not execute copyrighted x86 data or copy original asset bytes into
the repository. The supplied file is required on the invoking machine.
"""
from __future__ import annotations

import argparse
from collections import Counter
import hashlib
from pathlib import Path
import struct

SHA256 = "19b6bc15f1be616bb8ff49dc9029995a1a0413fae06addc9fd41ddef761d0e87"
FILE_SIZE = 333757
SCALE_OFFSET = 0x1F42
BLOCK_COUNT = 127
INTERNAL_PREFIX = 0x204
EXPECTED = {
    "load_al": 11587,
    "mov_ah_al": 11587,
    "sub_eax_ecx": 11587,
    "store_al": 11947,
    "store_ah": 11963,
    "ret": 127,
}
EXPECTED_CODE_BYTES = 259457
EXPECTED_PATCH_OFFSETS = 127 * 129


def audit(raw: bytes) -> dict[str, object]:
    if len(raw) != FILE_SIZE or hashlib.sha256(raw).hexdigest() != SHA256:
        raise ValueError("SINTAB does not match the certified original file")
    cursor = SCALE_OFFSET
    counts: Counter[str] = Counter()
    code_bytes = 0
    boundaries_checked = 0
    write_offset_min = None
    write_offset_max = None
    for block in range(BLOCK_COUNT):
        if cursor + 2 > len(raw):
            raise ValueError(f"block {block}: truncated size")
        length = struct.unpack_from("<H", raw, cursor)[0]
        cursor += 2
        data = raw[cursor:cursor+length]
        if len(data) != length or length < INTERNAL_PREFIX+1:
            raise ValueError(f"block {block}: invalid size")
        cursor += length
        patches = struct.unpack_from("<129H", data, 0)
        if any(a > b for a, b in zip(patches, patches[1:])):
            raise ValueError(f"block {block}: unordered patch table")
        code = data[INTERNAL_PREFIX:]
        code_bytes += len(code)
        pc = 0
        boundaries = {0}
        while pc < len(code):
            instruction = code[pc:pc+2]
            if instruction == b"\x8a\x86" and pc+6 <= len(code):
                counts["load_al"] += 1
                pc += 6
            elif instruction == b"\x88\xc4":
                counts["mov_ah_al"] += 1
                pc += 2
            elif instruction == b"\x29\xc8":
                counts["sub_eax_ecx"] += 1
                pc += 2
            elif instruction in (b"\x88\x87", b"\x88\xa7") and pc+6 <= len(code):
                displacement = struct.unpack_from("<I", code, pc+2)[0]
                if displacement % 80:
                    raise ValueError(f"block {block}: non-VGA-row store displacement")
                write_offset_min = min(displacement, write_offset_min if write_offset_min is not None else displacement)
                write_offset_max = max(displacement, write_offset_max if write_offset_max is not None else displacement)
                counts["store_al" if instruction[1] == 0x87 else "store_ah"] += 1
                pc += 6
            elif code[pc] == 0xC3 and pc == len(code)-1:
                counts["ret"] += 1
                pc += 1
            else:
                raise ValueError(f"block {block}: unknown instruction at 0x{pc:x}")
            boundaries.add(pc)
        for offset in patches:
            if offset < INTERNAL_PREFIX or offset-INTERNAL_PREFIX not in boundaries:
                raise ValueError(f"block {block}: patch does not align to instruction")
            if offset-INTERNAL_PREFIX >= len(code):
                raise ValueError(f"block {block}: patch outside generated instructions")
            boundaries_checked += 1
    if len(raw)-cursor != 512 or any(raw[cursor:]):
        raise ValueError("SINTAB terminal zero padding mismatch")
    if dict(counts) != EXPECTED:
        raise ValueError(f"original opcode inventory mismatch: {dict(counts)}")
    if code_bytes != EXPECTED_CODE_BYTES or boundaries_checked != EXPECTED_PATCH_OFFSETS:
        raise ValueError("generated code size or patch offset count mismatch")
    return {
        "sha256": SHA256,
        "blocks": BLOCK_COUNT,
        "generated_code_bytes": code_bytes,
        "instructions": sum(counts.values()),
        "source_reads": counts["load_al"],
        "indexed_vga_stores": counts["store_al"] + counts["store_ah"],
        "patch_instruction_boundaries": boundaries_checked,
        "vga_write_displacement_min": write_offset_min,
        "vga_write_displacement_max": write_offset_max,
        "opcode_inventory": dict(counts),
    }


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("sintab", type=Path)
    args = parser.parse_args()
    result = audit(args.sintab.read_bytes())
    for key, value in result.items():
        print(f"{key}: {value}")


if __name__ == "__main__":
    main()
