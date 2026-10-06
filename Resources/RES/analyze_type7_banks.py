#!/usr/bin/env python3
"""Validate the INIT-driven RES3 type-7 source-bank partition."""
from __future__ import annotations
import argparse
import struct
from pathlib import Path


def u16(data: bytes, off: int) -> int:
    return struct.unpack_from("<H", data, off)[0]


def u32(data: bytes, off: int) -> int:
    return struct.unpack_from("<I", data, off)[0]


def res_count(data: bytes) -> int:
    count = struct.unpack_from("<i", data, 0)[0]
    if count < 0:
        raise ValueError("negative RES count")
    return count


def parse_descriptor_table(data: bytes, off: int):
    count = u16(data, off)
    off += 2
    records = []
    for i in range(count):
        rec = data[off + i * 7 : off + (i + 1) * 7]
        if len(rec) != 7:
            raise ValueError("short descriptor table")
        base, extent = struct.unpack_from("<HH", rec, 0)
        records.append(
            dict(
                local_shape_base=base,
                extent_raw=extent,
                shape_stride=rec[4],
                variant_count=rec[5],
                scene_influence_step=rec[6],
            )
        )
    return records, off + count * 7


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("init", type=Path)
    ap.add_argument("res3", type=Path)
    ap.add_argument("objects", type=Path)
    ap.add_argument("npcs", type=Path)
    args = ap.parse_args()

    init = args.init.read_bytes()
    res3 = args.res3.read_bytes()
    objects = args.objects.read_bytes()
    npcs = args.npcs.read_bytes()

    if len(init) < 0x24:
        raise ValueError("INIT shorter than fixed header")

    header_words = [u16(init, i) for i in range(0, 0x24, 2)]
    bases = header_words[3:7]
    count = res_count(res3)
    if bases != sorted(bases):
        raise ValueError(f"non-monotonic bank bases: {bases}")
    if bases[0] != 0:
        raise ValueError("source class 0 does not begin at RES3:0")
    if not all(base < count for base in bases):
        raise ValueError("bank start outside RES3")

    ends = bases[1:] + [count]
    sizes = [end - start for start, end in zip(bases, ends)]

    class1, off = parse_descriptor_table(init, 0x24)
    class3, off = parse_descriptor_table(init, off)
    if len(class1) != 175:
        raise ValueError(f"class-1 descriptor count {len(class1)} != 175")
    if len(class3) != 12:
        raise ValueError(f"class-3 descriptor count {len(class3)} != 12")

    for source_class, table, bank_size in (
        (1, class1, sizes[1]),
        (3, class3, sizes[3]),
    ):
        allocation_ends = []
        for i, rec in enumerate(table):
            if rec["variant_count"] == 0:
                raise ValueError(f"class {source_class} descriptor {i}: variant_count == 0")
            end = (
                rec["local_shape_base"]
                + rec["shape_stride"] * rec["variant_count"]
            )
            if end > bank_size:
                raise ValueError(
                    f"class {source_class} descriptor {i}: allocation end {end} > bank {bank_size}"
                )
            allocation_ends.append(end)
        if max(allocation_ends) != bank_size:
            raise ValueError(
                f"class {source_class}: max allocation end {max(allocation_ends)} != bank {bank_size}"
            )

    object_count = u32(objects, 0)
    if 4 + object_count * 0x41 != len(objects):
        raise ValueError("OBJECTS.DAT size/count mismatch")
    object_ranges = []
    for i in range(object_count):
        rec = objects[4 + i * 0x41 : 4 + (i + 1) * 0x41]
        base = u16(rec, 0x28)
        stride = rec[0x2A]
        if base >= sizes[2] and not (base == 0 and stride == 0):
            raise ValueError(f"OBJECTS record {i}: local base outside class-2 bank")
        if base + stride > sizes[2]:
            raise ValueError(f"OBJECTS record {i}: base + stride outside class-2 bank")
        object_ranges.append((base, stride))
    if max(base + stride for base, stride in object_ranges) != sizes[2]:
        raise ValueError("OBJECTS maximum base + stride does not close class-2 bank")

    npc_count = u32(npcs, 0)
    if 4 + npc_count * 0x45 != len(npcs):
        raise ValueError("NPCS.DAT size/count mismatch")
    npc_bases = [u16(npcs, 4 + i * 0x45 + 0x14) for i in range(npc_count)]
    if max(npc_bases) >= sizes[0]:
        raise ValueError("NPC local shape base outside class-0 bank")

    print(f"RES3 entries: {count}")
    for source_class, (start, end, size) in enumerate(zip(bases, ends, sizes)):
        print(f"class {source_class}: {start}..{end - 1} ({size})")
    print(f"INIT class-1 descriptors: {len(class1)}; exact bank closure PASS")
    print(f"INIT class-3 descriptors: {len(class3)}; exact bank closure PASS")
    print(
        f"OBJECTS records: {object_count}; class-2 local base/span fit PASS; "
        f"max end={max(base + stride for base, stride in object_ranges)}"
    )
    print(
        f"NPCS records: {npc_count}; class-0 local bases fit PASS; "
        f"max base={max(npc_bases)}"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
