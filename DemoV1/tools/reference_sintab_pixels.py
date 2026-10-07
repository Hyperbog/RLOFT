#!/usr/bin/env python3
"""Independent indexed-pixel checksum reference for original SINTAB bytecode.

Usage: python3 DemoV1/tools/reference_sintab_pixels.py /path/to/SINTAB

Unlike the C++ microcode executor, this uses a simple direct original-opcode
walker and a flat single-plane VGA buffer. It does not embed original data.
"""
from __future__ import annotations

import argparse
import hashlib
from pathlib import Path
import struct

EXPECTED_SHA = "19b6bc15f1be616bb8ff49dc9029995a1a0413fae06addc9fd41ddef761d0e87"
EXPECTED_SIZE = 333757
EXPECTED_FINGERPRINTS = {
    0x0102: 0x0658C6DB74B80324,
    0x3412: 0x0D284A7FB40210EE,
}
FNV_START = 0xCBF29CE484222325
FNV_PRIME = 0x100000001B3
MASK = (1 << 64) - 1
PLANE_BYTES_CHECKED = 21000


def mix(h: int, byte: int) -> int:
    return ((h ^ byte) * FNV_PRIME) & MASK


def run_code(code: bytes, source: bytes, lookup: int) -> tuple[bytes, int, int]:
    pc = 0
    eax = 0
    loads = 0
    writes = 0
    plane = bytearray(PLANE_BYTES_CHECKED)
    while pc < len(code):
        op = code[pc:pc + 2]
        if op == b"\x8a\x86" and pc + 6 <= len(code):
            displacement = struct.unpack_from("<i", code, pc + 2)[0]
            if not 0 <= displacement < len(source):
                raise ValueError(f"source displacement outside reference buffer: {displacement}")
            eax = (eax & 0xFFFFFF00) | source[displacement]
            pc += 6
            loads += 1
        elif op == b"\x88\xc4":
            eax = (eax & 0xFFFF00FF) | ((eax & 255) << 8)
            pc += 2
        elif op == b"\x29\xc8":
            eax = (eax - lookup) & 0xFFFFFFFF
            pc += 2
        elif op in (b"\x88\x87", b"\x88\xa7") and pc + 6 <= len(code):
            destination = struct.unpack_from("<I", code, pc + 2)[0]
            if destination >= len(plane) or destination % 80:
                raise ValueError(f"unaligned/out-of-frame VGA displacement: {destination}")
            plane[destination] = (eax >> (8 if op[1] == 0xA7 else 0)) & 255
            pc += 6
            writes += 1
        elif code[pc] == 0xC3 and pc == len(code) - 1:
            break
        else:
            raise ValueError(f"unsupported generated opcode at {pc:#x}")
    return bytes(plane), loads, writes


def fingerprints(data: bytes) -> dict[int, int]:
    if len(data) != EXPECTED_SIZE or hashlib.sha256(data).hexdigest() != EXPECTED_SHA:
        raise ValueError("input is not the certified original 333,757-byte SINTAB")
    source = bytes((i * 31 + 17) & 255 for i in range(4096))
    result = {ecx: FNV_START for ecx in EXPECTED_FINGERPRINTS}
    cursor = 0x1F42
    reads = writes = 0
    for block_index in range(127):
        (block_size,) = struct.unpack_from("<H", data, cursor)
        cursor += 2
        body = data[cursor:cursor + block_size]
        cursor += block_size
        if len(body) != block_size or block_size < 0x205:
            raise ValueError(f"malformed scale block {block_index}")
        code = body[0x204:]
        for lookup in EXPECTED_FINGERPRINTS:
            plane, loaded, stored = run_code(code, source, lookup)
            if lookup == 0x0102:
                reads += loaded
                writes += stored
            block_digest = FNV_START
            for pixel in plane:
                block_digest = mix(block_digest, pixel)
            global_digest = mix(result[lookup], block_index)
            for byte in block_digest.to_bytes(8, "little"):
                global_digest = mix(global_digest, byte)
            result[lookup] = global_digest
    if cursor != len(data) - 512 or any(data[cursor:]):
        raise ValueError("SINTAB block data or final zero trailer mismatch")
    if (reads, writes) != (11587, 23910):
        raise ValueError(f"pixel-op counts differ from original: {(reads, writes)}")
    return result


def main() -> None:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("sintab", type=Path)
    args = ap.parse_args()
    actual = fingerprints(args.sintab.read_bytes())
    for lookup, value in actual.items():
        expected = EXPECTED_FINGERPRINTS[lookup]
        print(f"lookup {lookup:#06x}: reference fingerprint {value:#018x}")
        if value != expected:
            raise ValueError(f"pixel fingerprint mismatch: expected {expected:#018x}")


if __name__ == "__main__":
    main()
