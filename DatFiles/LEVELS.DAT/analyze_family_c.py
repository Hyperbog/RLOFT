#!/usr/bin/env python3
"""Audit LEVELS.DAT family-C loose/pickable item records against OBJECTS.DAT."""
from __future__ import annotations

import argparse
import collections
import struct
from pathlib import Path

REC_SIZE = 11
OBJ_SIZE = 0x41
HEADER_SIZE = 0x2A4


def u32(buf: bytes, off: int) -> int:
    return struct.unpack_from('<I', buf, off)[0]


def s16(buf: bytes, off: int) -> int:
    return struct.unpack_from('<h', buf, off)[0]


def parse_object_names(data: bytes) -> list[str]:
    count = u32(data, 0)
    expected = 4 + count * OBJ_SIZE
    if len(data) != expected:
        raise ValueError(f'OBJECTS.DAT size mismatch: {len(data)} != {expected}')
    names: list[str] = []
    for i in range(count):
        rec = data[4 + i * OBJ_SIZE:4 + (i + 1) * OBJ_SIZE]
        names.append(rec.split(b'\0', 1)[0].decode('cp437', errors='replace'))
    return names


def compact_records(data: bytes, start: int, count: int):
    for i in range(count):
        off = start + i * REC_SIZE
        yield struct.unpack_from('<HhHhHB', data, off)


def main() -> None:
    ap = argparse.ArgumentParser()
    ap.add_argument('levels', type=Path)
    ap.add_argument('objects', type=Path)
    ap.add_argument('--top', type=int, default=20)
    args = ap.parse_args()

    levels = args.levels.read_bytes()
    objects = args.objects.read_bytes()
    names = parse_object_names(objects)

    level_count = u32(levels, 0)
    offsets = struct.unpack_from('<' + 'I' * level_count, levels, 4)

    c_types: collections.Counter[int] = collections.Counter()
    b_types: collections.Counter[int] = collections.Counter()
    c_flags: collections.Counter[int] = collections.Counter()
    b_flags: collections.Counter[int] = collections.Counter()
    nonzero_levels = 0
    max_level_c = 0
    all_c_in_bounds = True
    legacy_0x54_records: list[tuple[int, int, int, int]] = []

    for level_id, record_off in enumerate(offsets):
        b0, b1, b2, b3, b4, b5, b6 = struct.unpack_from('<7I', levels, record_off + 1)
        header_off = record_off + b0
        header = levels[header_off:header_off + HEADER_SIZE]
        width = s16(header, 1)
        height = s16(header, 3)
        count_c = s16(header, 0x28C)
        count_b = s16(header, 0x28E)

        if record_off + b4 + count_c * REC_SIZE != record_off + b5:
            raise ValueError(f'level {level_id}: family-C boundary mismatch')
        if record_off + b3 + count_b * REC_SIZE != record_off + b4:
            raise ValueError(f'level {level_id}: family-B boundary mismatch')

        if count_c:
            nonzero_levels += 1
            max_level_c = max(max_level_c, count_c)

        for record_index, (typ, x, xf, y, yf, flags) in enumerate(compact_records(levels, record_off + b4, count_c)):
            c_types[typ] += 1
            c_flags[flags] += 1
            if typ == 0x54:
                legacy_0x54_records.append((level_id, record_index, x, y))
            if not (0 <= x < width and 0 <= y < height):
                all_c_in_bounds = False

        for typ, x, xf, y, yf, flags in compact_records(levels, record_off + b3, count_b):
            b_types[typ] += 1
            b_flags[flags] += 1

    total_c = sum(c_types.values())
    total_b = sum(b_types.values())
    out_of_db = sorted(t for t in c_types if t >= len(names))

    print(f'levels={level_count}')
    print(f'objects={len(names)} record_size=0x{OBJ_SIZE:X}')
    print(f'family_c_records={total_c} nonzero_levels={nonzero_levels} max_per_level={max_level_c}')
    print(f'family_c_unique_types={len(c_types)} type_range={min(c_types)}..{max(c_types)}')
    print(f'family_c_flags={dict(sorted(c_flags.items()))}')
    print(f'family_c_all_coordinates_in_bounds={all_c_in_bounds}')
    print(f'family_c_out_of_object_database={out_of_db}')
    print(f'family_b_records={total_b} unique_types={len(b_types)} type_range={min(b_types)}..{max(b_types)}')
    print(f'family_b_flags={dict(sorted(b_flags.items()))}')
    source_name = names[0x54] if len(names) > 0x54 else '<out-of-range>'
    target_name = names[0x58] if len(names) > 0x58 else '<out-of-range>'
    print(
        f'legacy_type_0x54_count={c_types[0x54]} source_current_name={source_name!r} '
        f'runtime_remap_target=0x58 target_name={target_name!r}'
    )
    print(f'legacy_type_0x54_locations={legacy_0x54_records}')
    print('top_family_c_types:')
    for typ, count in c_types.most_common(args.top):
        name = names[typ] if typ < len(names) else '<out-of-range>'
        print(f'  {typ:3d} 0x{typ:03X} {count:4d} {name}')


if __name__ == '__main__':
    main()
