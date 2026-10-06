#!/usr/bin/env python3
"""Analyze Ravenloft LEVELS.DAT 10-byte cell records and compact families.

Labels are limited to semantics proved from certified CODE.2. Unknown bytes remain raw.
"""
from __future__ import annotations

import argparse
import collections
import struct
from pathlib import Path

PREAMBLE = 0x1D
HEADER_SIZE = 0x2A4


def u32(data: bytes, off: int) -> int:
    return struct.unpack_from('<I', data, off)[0]


def s16(data: bytes, off: int) -> int:
    return struct.unpack_from('<h', data, off)[0]


def archive_records(data: bytes) -> list[bytes]:
    count = u32(data, 0)
    offsets = list(struct.unpack_from(f'<{count}I', data, 4))
    return [
        data[start: offsets[i + 1] if i + 1 < count else len(data)]
        for i, start in enumerate(offsets)
    ]


def parse_level(record: bytes):
    bounds = struct.unpack_from('<7I', record, 1)
    if bounds[0] != PREAMBLE:
        raise ValueError('unexpected payload start')
    header = record[PREAMBLE:PREAMBLE + HEADER_SIZE]
    width = s16(header, 1)
    height = s16(header, 3)
    counts = {
        'family_a': s16(header, 0x28A),
        'family_c': s16(header, 0x28C),
        'family_b': s16(header, 0x28E),
    }

    cur = bounds[1]
    records10 = []
    for column in range(width):
        n = s16(record, cur)
        cur += 2
        for _ in range(n):
            raw = record[cur:cur + 10]
            if len(raw) != 10:
                raise ValueError('truncated 10-byte record')
            records10.append((column, raw))
            cur += 10
    if cur != bounds[2]:
        raise ValueError('10-byte list boundary mismatch')

    p = bounds[2]
    fam_types = {}
    for name, n, next_bound in (
        ('family_a', counts['family_a'], bounds[3]),
        ('family_b', counts['family_b'], bounds[4]),
        ('family_c', counts['family_c'], bounds[5]),
    ):
        values = []
        for _ in range(n):
            values.append(struct.unpack_from('<H', record, p)[0])
            p += 11
        if p != next_bound:
            raise ValueError(f'{name} boundary mismatch')
        fam_types[name] = values

    return width, height, records10, fam_types


def db_info(path: Path, stride: int):
    data = path.read_bytes()
    count = u32(data, 0)
    if len(data) != 4 + count * stride:
        raise ValueError(
            f'{path.name}: size does not match 4 + count*{stride:#x}'
        )
    names = []
    for i in range(count):
        rec = data[4 + i * stride:4 + (i + 1) * stride]
        names.append(rec.split(b'\0', 1)[0].decode('cp437', errors='replace'))
    return count, names


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument('levels', type=Path)
    parser.add_argument('--objects', type=Path)
    parser.add_argument('--npcs', type=Path)
    args = parser.parse_args()

    records = archive_records(args.levels.read_bytes())
    kinds = collections.Counter()
    controls = collections.Counter()
    by_kind = collections.defaultdict(collections.Counter)
    all10 = []
    families = collections.defaultdict(list)

    for level_id, record in enumerate(records):
        _, height, recs, fams = parse_level(record)
        for column, raw in recs:
            if raw[5] != column or raw[4] >= height:
                raise ValueError(f'level {level_id}: coordinate invariant failed')
            all10.append(raw)
            kinds[raw[0]] += 1
            controls[raw[1]] += 1
            by_kind[raw[0]][raw[1]] += 1
        for name, values in fams.items():
            families[name].extend(values)

    print(f'levels={len(records)} ten_byte_records={len(all10)}')
    print('kinds=' + ' '.join(f'{k}:{v}' for k, v in sorted(kinds.items())))
    print(
        'control_bytes=' +
        ' '.join(f'{k:#04x}:{v}' for k, v in sorted(controls.items()))
    )
    for kind in sorted(by_kind):
        dist = ' '.join(
            f'{value:#04x}:{count}'
            for value, count in sorted(by_kind[kind].items())
        )
        print(f'kind {kind:2d} n={kinds[kind]:4d} control={dist}')

    group2 = [record for record in all10 if record[0] == 1]
    print(f'group2_candidates={len(group2)}')
    print(
        'group2_position=' +
        ' '.join(
            f'{value}:{count}'
            for value, count in sorted(
                collections.Counter(record[6] for record in group2).items()
            )
        )
    )
    print(
        'group2_delta=' +
        ' '.join(
            f'{value:#04x}:{count}'
            for value, count in sorted(
                collections.Counter(record[7] for record in group2).items()
            )
        )
    )

    for name in ('family_a', 'family_b', 'family_c'):
        values = families[name]
        print(
            f'{name}_records={len(values)} unique_types={len(set(values))} '
            f'min={min(values) if values else "-"} '
            f'max={max(values) if values else "-"}'
        )

    if args.npcs:
        count, _ = db_info(args.npcs, 0x45)
        out_of_range = sum(t >= count for t in families['family_a'])
        print(
            f'npcs_count={count} stride=0x45 '
            f'family_a_out_of_range={out_of_range}'
        )

    if args.objects:
        count, names = db_info(args.objects, 0x41)
        for name in ('family_b', 'family_c'):
            out_of_range = sum(t >= count for t in families[name])
            print(
                f'objects_count={count} stride=0x41 '
                f'{name}_out_of_range={out_of_range}'
            )
        print(
            'object_types_3c_41=' +
            ' | '.join(f'{i:#04x}:{names[i]}' for i in range(0x3C, 0x42))
        )


if __name__ == '__main__':
    main()
