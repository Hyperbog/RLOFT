#!/usr/bin/env python3
"""Structural/semantic validator for Ravenloft LEVELS.DAT and LEVnnn.DAT.

Only relationships proved from the certified CODE.2 loader/writer and source bytes
are named here. Unknown gameplay fields remain address/offset based.
"""
from __future__ import annotations

import argparse
import hashlib
import struct
from dataclasses import dataclass
from pathlib import Path
from typing import List, Tuple

PREAMBLE_SIZE = 0x1D
BASE_HEADER_SIZE = 0x2A4
TAIL_BLOB_SIZE = 0x514
LEGACY_RECORD_SIZE = 0x0B
DIRECT_RECORD_SIZE = 0x16
LIST_RECORD_SIZE = 0x0A


def u16(buf: bytes, off: int) -> int:
    return struct.unpack_from("<H", buf, off)[0]


def u32(buf: bytes, off: int) -> int:
    return struct.unpack_from("<I", buf, off)[0]


def s16(buf: bytes, off: int) -> int:
    return struct.unpack_from("<h", buf, off)[0]


@dataclass(frozen=True)
class CoreHeader:
    width: int
    height: int
    count_258a2: int
    count_258a4: int
    count_258a6: int
    field_258b2: int
    mode_258b4: int


@dataclass(frozen=True)
class RecordLayout:
    marker: int
    bounds: Tuple[int, int, int, int, int, int, int]
    core: CoreHeader
    tail_text_length: int


@dataclass(frozen=True)
class LegacyCompactRecord:
    type_id: int
    x_cell: int
    x_fraction: int
    y_cell: int
    y_fraction: int
    flags_byte: int

    @property
    def x_fixed_u32(self) -> int:
        return ((self.x_cell & 0xFFFF) << 16) | self.x_fraction

    @property
    def y_fixed_u32(self) -> int:
        return ((self.y_cell & 0xFFFF) << 16) | self.y_fraction


def parse_preamble(record: bytes) -> Tuple[int, Tuple[int, ...]]:
    if len(record) < PREAMBLE_SIZE:
        raise ValueError("record shorter than 0x1D-byte preamble")
    marker = record[0]
    bounds = struct.unpack_from("<7I", record, 1)
    if bounds[0] != PREAMBLE_SIZE:
        raise ValueError(f"first boundary is {bounds[0]:#x}, expected 0x1D")
    if list(bounds) != sorted(bounds):
        raise ValueError(f"non-monotonic preamble boundaries: {bounds}")
    return marker, bounds


def parse_core(record: bytes, start: int) -> CoreHeader:
    end = start + BASE_HEADER_SIZE
    if end > len(record):
        raise ValueError("truncated 0x2A4-byte base header")
    h = record[start:end]
    return CoreHeader(
        width=s16(h, 0x001),
        height=s16(h, 0x003),
        count_258a2=s16(h, 0x28A),
        count_258a4=s16(h, 0x28C),
        count_258a6=s16(h, 0x28E),
        field_258b2=u16(h, 0x29A),
        mode_258b4=h[0x29C],
    )


def require_nonnegative(core: CoreHeader) -> None:
    values = (
        ("width", core.width),
        ("height", core.height),
        ("count_258a2", core.count_258a2),
        ("count_258a4", core.count_258a4),
        ("count_258a6", core.count_258a6),
    )
    for name, value in values:
        if value < 0:
            raise ValueError(f"negative {name}: {value}")


def parse_column_lists(
    record: bytes, start: int, width: int, height: int, *, validate_coordinates: bool
) -> Tuple[int, List[int], int]:
    pos = start
    counts: List[int] = []
    total_records = 0
    for column in range(width):
        if pos + 2 > len(record):
            raise ValueError(f"truncated column count at column {column}")
        count = s16(record, pos)
        pos += 2
        if count < 0:
            raise ValueError(f"negative column count {count} at column {column}")
        counts.append(count)
        for item in range(count):
            end = pos + LIST_RECORD_SIZE
            if end > len(record):
                raise ValueError(f"column {column} record {item} runs past EOF")
            item_bytes = record[pos:end]
            if validate_coordinates:
                # CODE.2 1E8C8 indexes the list by column and compares byte +4 to row.
                # The archive bytes additionally prove byte +5 duplicates the owning column.
                row = item_bytes[4]
                stored_column = item_bytes[5]
                if row >= height:
                    raise ValueError(
                        f"column {column} record {item}: row byte {row} >= height {height}"
                    )
                if stored_column != column:
                    raise ValueError(
                        f"column {column} record {item}: byte +5 is {stored_column}"
                    )
            total_records += 1
            pos = end
    return pos, counts, total_records


def decode_legacy_compact(buf: bytes, off: int) -> LegacyCompactRecord:
    if off + LEGACY_RECORD_SIZE > len(buf):
        raise ValueError("truncated 11-byte legacy compact record")
    return LegacyCompactRecord(
        type_id=u16(buf, off + 0),
        x_cell=s16(buf, off + 2),
        x_fraction=u16(buf, off + 4),
        y_cell=s16(buf, off + 6),
        y_fraction=u16(buf, off + 8),
        flags_byte=buf[off + 10],
    )


def validate_legacy_family(
    record: bytes, start: int, count: int, core: CoreHeader, family_name: str
) -> None:
    for i in range(count):
        item = decode_legacy_compact(record, start + i * LEGACY_RECORD_SIZE)
        if not (0 <= item.x_cell < core.width):
            raise ValueError(
                f"{family_name}[{i}] x_cell={item.x_cell} outside width {core.width}"
            )
        if not (0 <= item.y_cell < core.height):
            raise ValueError(
                f"{family_name}[{i}] y_cell={item.y_cell} outside height {core.height}"
            )


def expand_legacy_family_b(item: LegacyCompactRecord, width: int) -> bytes:
    """Reproduce the fields CODE.2 explicitly constructs for archive family B.

    The destination starts zeroed here. CODE.2 explicitly writes offsets 00..07,
    0C..11 and 14..15. This is enough to reproduce all 110 family-B records in
    the supplied LEVELS record 22 / LEV022 pair byte-for-byte.
    """
    out = bytearray(DIRECT_RECORD_SIZE)
    struct.pack_into("<I", out, 0x00, item.y_fixed_u32)
    struct.pack_into("<I", out, 0x04, item.x_fixed_u32)
    struct.pack_into("<h", out, 0x0C, item.y_cell * width + item.x_cell)
    struct.pack_into("<H", out, 0x0E, item.type_id)
    struct.pack_into("<H", out, 0x10, 0)
    flags = ((item.flags_byte & 0x07) << 3) & 0xFE3F
    struct.pack_into("<H", out, 0x14, flags)
    return bytes(out)


def collect_column_records(record: bytes, start: int, width: int) -> Tuple[int, List[Tuple[int, bytes]]]:
    pos = start
    records: List[Tuple[int, bytes]] = []
    for column in range(width):
        count = s16(record, pos)
        pos += 2
        if count < 0:
            raise ValueError(f"negative column count {count} at column {column}")
        for _ in range(count):
            end = pos + LIST_RECORD_SIZE
            if end > len(record):
                raise ValueError("truncated 10-byte column record")
            records.append((column, record[pos:end]))
            pos = end
    return pos, records


def decode_direct_world_record(record: bytes, off: int) -> dict[str, int]:
    if off + DIRECT_RECORD_SIZE > len(record):
        raise ValueError("truncated 22-byte direct world record")
    state = u16(record, off + 0x14)
    return {
        "y_fixed": u32(record, off + 0x00),
        "x_fixed": u32(record, off + 0x04),
        "runtime_flags": u32(record, off + 0x08),
        "cell_index": u16(record, off + 0x0C),
        "type_id": u16(record, off + 0x0E),
        "action_countdown": u16(record, off + 0x10),
        "current_hp": s16(record, off + 0x12),
        "packed_state": state,
        "facing": (state >> 3) & 0x07,
    }


def validate_tail(record: bytes, blob_end: int, core: CoreHeader) -> int:
    pos = blob_end
    if pos + 4 > len(record):
        raise ValueError("missing duplicated width/height tail words")
    tail_width, tail_height = struct.unpack_from("<hh", record, pos)
    pos += 4
    if (tail_width, tail_height) != (core.width, core.height):
        raise ValueError(
            "tail dimensions disagree with base header: "
            f"{tail_width}x{tail_height} vs {core.width}x{core.height}"
        )
    pos += core.width * core.height
    if pos + 4 > len(record):
        raise ValueError("missing tail text length")
    text_length = u32(record, pos)
    pos += 4
    end = pos + text_length
    if end != len(record):
        raise ValueError(
            f"tail ends at {end:#x}, record/file ends at {len(record):#x}"
        )
    return text_length


def validate_legacy_record(record: bytes) -> RecordLayout:
    marker, bounds = parse_preamble(record)
    core = parse_core(record, bounds[0])
    require_nonnegative(core)
    expected_grid_end = bounds[0] + BASE_HEADER_SIZE + 2 * core.width * core.height
    if bounds[1] != expected_grid_end:
        raise ValueError(
            f"grid boundary {bounds[1]:#x} != expected {expected_grid_end:#x}"
        )
    dynamic_end, _, _ = parse_column_lists(
        record, bounds[1], core.width, core.height, validate_coordinates=True
    )
    if dynamic_end != bounds[2]:
        raise ValueError(
            f"legacy dynamic-list end {dynamic_end:#x} != boundary {bounds[2]:#x}"
        )
    if bounds[3] - bounds[2] != core.count_258a2 * LEGACY_RECORD_SIZE:
        raise ValueError("legacy family at 0x258A2 does not match 11-byte count")
    if bounds[4] - bounds[3] != core.count_258a6 * LEGACY_RECORD_SIZE:
        raise ValueError("legacy family at 0x258A6 does not match 11-byte count")
    if bounds[5] - bounds[4] != core.count_258a4 * LEGACY_RECORD_SIZE:
        raise ValueError("legacy family at 0x258A4 does not match 11-byte count")
    validate_legacy_family(record, bounds[2], core.count_258a2, core, "family_A")
    validate_legacy_family(record, bounds[3], core.count_258a6, core, "family_B")
    validate_legacy_family(record, bounds[4], core.count_258a4, core, "family_C")
    if bounds[6] - bounds[5] != TAIL_BLOB_SIZE:
        raise ValueError("legacy 0x514-byte block boundary mismatch")
    text_length = validate_tail(record, bounds[6], core)
    return RecordLayout(marker, bounds, core, text_length)


def validate_direct_record(record: bytes) -> RecordLayout:
    marker, bounds = parse_preamble(record)
    core = parse_core(record, bounds[0])
    require_nonnegative(core)
    expected_grid_end = bounds[0] + BASE_HEADER_SIZE + 2 * core.width * core.height
    if bounds[1] != expected_grid_end:
        raise ValueError(
            f"grid boundary {bounds[1]:#x} != expected {expected_grid_end:#x}"
        )
    pos, _, _ = parse_column_lists(
        record, bounds[1], core.width, core.height, validate_coordinates=True
    )
    if pos >= len(record):
        raise ValueError("missing direct-save single-byte field between column groups")
    pos += 1
    pos, _, _ = parse_column_lists(
        record, pos, core.width, core.height, validate_coordinates=True
    )
    if pos != bounds[2]:
        raise ValueError(
            f"direct dynamic-list end {pos:#x} != boundary {bounds[2]:#x}"
        )
    if bounds[3] - bounds[2] != core.count_258a2 * DIRECT_RECORD_SIZE:
        raise ValueError("direct family at 0x258A2 does not match 22-byte count")
    expected_second = 2 + core.count_258a6 * DIRECT_RECORD_SIZE
    if bounds[4] - bounds[3] != expected_second:
        raise ValueError("direct family at 0x258A6 does not match count-word + 22-byte records")
    serialized_count = s16(record, bounds[3])
    if serialized_count != core.count_258a6:
        raise ValueError(
            f"serialized 0x258A6 count {serialized_count} != header count {core.count_258a6}"
        )
    if bounds[5] - bounds[4] != core.count_258a4 * DIRECT_RECORD_SIZE:
        raise ValueError("direct family at 0x258A4 does not match 22-byte count")
    if bounds[6] - bounds[5] != TAIL_BLOB_SIZE:
        raise ValueError("direct 0x514-byte block boundary mismatch")
    text_length = validate_tail(record, bounds[6], core)
    return RecordLayout(marker, bounds, core, text_length)


def archive_records(data: bytes) -> List[bytes]:
    if len(data) < 4:
        raise ValueError("archive too short")
    count = u32(data, 0)
    directory_end = 4 + count * 4
    if directory_end > len(data):
        raise ValueError("directory runs past EOF")
    offsets = list(struct.unpack_from(f"<{count}I", data, 4))
    if not offsets:
        raise ValueError("empty archive")
    if offsets[0] != directory_end:
        raise ValueError(
            f"first record starts at {offsets[0]:#x}, directory ends at {directory_end:#x}"
        )
    if offsets != sorted(offsets) or len(set(offsets)) != len(offsets):
        raise ValueError("archive offsets are not strictly increasing")
    return [
        data[start : offsets[i + 1] if i + 1 < count else len(data)]
        for i, start in enumerate(offsets)
    ]


def inspect_archive(path: Path) -> None:
    data = path.read_bytes()
    records = archive_records(data)
    total_list_records = 0
    total_compact_records = 0
    print(f"{path.name}: {len(data)} bytes, sha256={hashlib.sha256(data).hexdigest()}")
    print(f"records={len(records)}, directory_size={4 + len(records) * 4:#x}")
    print("id  size    map      counts(258A2/258A6/258A4)  mode  text")
    for i, record in enumerate(records):
        layout = validate_legacy_record(record)
        c = layout.core
        _, _, list_count = parse_column_lists(
            record, layout.bounds[1], c.width, c.height, validate_coordinates=True
        )
        total_list_records += list_count
        total_compact_records += c.count_258a2 + c.count_258a6 + c.count_258a4
        print(
            f"{i:02d}  {len(record):06x}  {c.width:02d}x{c.height:<2d}  "
            f"{c.count_258a2:4d}/{c.count_258a6:4d}/{c.count_258a4:4d}"
            f"                 {c.mode_258b4:3d}  {layout.tail_text_length}"
        )
    print(f"10-byte list records coordinate-checked: {total_list_records}")
    print(f"11-byte compact records coordinate-checked: {total_compact_records}")
    print("archive structural+coordinate validation: PASS")


def inspect_direct(path: Path) -> None:
    data = path.read_bytes()
    layout = validate_direct_record(data)
    c = layout.core
    print(f"{path.name}: {len(data)} bytes, sha256={hashlib.sha256(data).hexdigest()}")
    print(f"marker_byte={layout.marker:#04x} (loader does not consume byte 0)")
    print("boundaries=" + ",".join(f"{v:#x}" for v in layout.bounds))
    print(
        f"map={c.width}x{c.height}, counts(258A2/258A6/258A4)="
        f"{c.count_258a2}/{c.count_258a6}/{c.count_258a4}, "
        f"field_258B2={c.field_258b2:#x}, mode_258B4={c.mode_258b4:#x}, "
        f"tail_text_length={layout.tail_text_length}"
    )
    print("standalone structural+coordinate validation: PASS")


def correlate(archive_path: Path, direct_path: Path, level_id: int) -> None:
    records = archive_records(archive_path.read_bytes())
    if not (0 <= level_id < len(records)):
        raise ValueError(f"level id {level_id} outside archive record range")
    legacy = records[level_id]
    direct = direct_path.read_bytes()
    la = validate_legacy_record(legacy)
    ld = validate_direct_record(direct)
    if (la.core.width, la.core.height) != (ld.core.width, ld.core.height):
        raise ValueError("archive/direct dimensions differ")

    header_a = legacy[la.bounds[0] : la.bounds[0] + BASE_HEADER_SIZE]
    header_d = direct[ld.bounds[0] : ld.bounds[0] + BASE_HEADER_SIZE]
    header_diffs = [i for i, pair in enumerate(zip(header_a, header_d)) if pair[0] != pair[1]]

    grid_a = legacy[la.bounds[0] + BASE_HEADER_SIZE : la.bounds[1]]
    grid_d = direct[ld.bounds[0] + BASE_HEADER_SIZE : ld.bounds[1]]
    grid_diff_bytes = sum(a != b for a, b in zip(grid_a, grid_d))
    cell_count = la.core.width * la.core.height
    words_a = struct.unpack_from(f"<{cell_count}H", grid_a, 0)
    words_d = struct.unpack_from(f"<{cell_count}H", grid_d, 0)
    word_deltas = [a ^ d for a, d in zip(words_a, words_d) if a != d]

    archive_list_end, archive_list_records = collect_column_records(
        legacy, la.bounds[1], la.core.width
    )
    direct_group1_end, direct_group1_records = collect_column_records(
        direct, ld.bounds[1], ld.core.width
    )
    active_animation_count = direct[direct_group1_end]
    direct_group2_end, direct_group2_records = collect_column_records(
        direct, direct_group1_end + 1, ld.core.width
    )

    blob_a = legacy[la.bounds[5] : la.bounds[6]]
    blob_d = direct[ld.bounds[5] : ld.bounds[6]]

    cells = la.core.width * la.core.height
    cell_a = legacy[la.bounds[6] + 4 : la.bounds[6] + 4 + cells]
    cell_d = direct[ld.bounds[6] + 4 : ld.bounds[6] + 4 + cells]
    cell_diff_bytes = sum(a != b for a, b in zip(cell_a, cell_d))

    family_b_matches = None
    if la.core.count_258a6 == ld.core.count_258a6:
        family_b_matches = 0
        for i in range(la.core.count_258a6):
            compact = decode_legacy_compact(legacy, la.bounds[3] + i * LEGACY_RECORD_SIZE)
            expanded = expand_legacy_family_b(compact, la.core.width)
            direct_off = ld.bounds[3] + 2 + i * DIRECT_RECORD_SIZE
            if expanded == direct[direct_off : direct_off + DIRECT_RECORD_SIZE]:
                family_b_matches += 1

    print(f"archive level {level_id} <-> {direct_path.name}")
    print(f"dimensions={la.core.width}x{la.core.height}")
    print(f"header differing bytes={len(header_diffs)}: " + ",".join(f"{i:#x}" for i in header_diffs))
    print(f"16-bit cell-layer differing bytes={grid_diff_bytes}/{len(grid_a)}")
    print(f"16-bit cell words changed={len(word_deltas)}/{cell_count}")
    if word_deltas:
        delta_counts = {}
        for delta in word_deltas:
            delta_counts[delta] = delta_counts.get(delta, 0) + 1
        print(
            "cell-word XOR deltas="
            + ", ".join(f"0x{k:04X}:{v}" for k, v in sorted(delta_counts.items()))
        )
    print(
        "10-byte group-1 byte-identical="
        f"{archive_list_records == direct_group1_records} "
        f"records={len(direct_group1_records)}"
    )
    print(
        f"direct active-animation count={active_animation_count}, "
        f"group-2 records={len(direct_group2_records)}"
    )

    if la.core.count_258a2 == ld.core.count_258a2:
        for i in range(la.core.count_258a2):
            compact = decode_legacy_compact(
                legacy, la.bounds[2] + i * LEGACY_RECORD_SIZE
            )
            live = decode_direct_world_record(
                direct, ld.bounds[2] + i * DIRECT_RECORD_SIZE
            )
            print(
                f"family_A[{i}] type={live['type_id']} "
                f"archive_xy=({compact.x_cell},{compact.y_cell}) "
                f"direct_xy=({live['x_fixed'] >> 16},{live['y_fixed'] >> 16}) "
                f"archive_facing={compact.flags_byte & 7} direct_facing={live['facing']} "
                f"hp={live['current_hp']} action={live['action_countdown']} "
                f"flags=0x{live['runtime_flags']:08X}"
            )

    print(f"0x514 block byte-identical={blob_a == blob_d}")
    print(f"final byte-layer differing bytes={cell_diff_bytes}/{len(cell_a)}")
    changed_0440 = {
        i for i, (a, d) in enumerate(zip(words_a, words_d)) if (a ^ d) == 0x0440
    }
    cache_2e = {i for i, value in enumerate(cell_d) if value == 0x2E}
    if changed_0440 or cache_2e:
        print(
            "0x0440-changed cells vs direct cache 0x2E="
            f"{len(changed_0440)}/{len(cache_2e)}, exact_set_match={changed_0440 == cache_2e}"
        )
    if family_b_matches is not None:
        print(
            "legacy family-B deterministic expansion matches direct records="
            f"{family_b_matches}/{la.core.count_258a6}"
        )


def main() -> None:
    parser = argparse.ArgumentParser()
    sub = parser.add_subparsers(dest="mode", required=True)
    p_archive = sub.add_parser("archive", help="validate LEVELS.DAT")
    p_archive.add_argument("path", type=Path)
    p_direct = sub.add_parser("direct", help="validate a standalone LEVnnn.DAT")
    p_direct.add_argument("path", type=Path)
    p_corr = sub.add_parser("correlate", help="compare an archive record with a standalone save")
    p_corr.add_argument("archive", type=Path)
    p_corr.add_argument("direct", type=Path)
    p_corr.add_argument("level_id", type=int)
    args = parser.parse_args()
    if args.mode == "archive":
        inspect_archive(args.path)
    elif args.mode == "direct":
        inspect_direct(args.path)
    else:
        correlate(args.archive, args.direct, args.level_id)


if __name__ == "__main__":
    main()
