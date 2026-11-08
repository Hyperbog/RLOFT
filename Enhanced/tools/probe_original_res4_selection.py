"""Independent original CODE.2 37590 EH6 / 37700 LZSS RES4 oracle.

Read PRIVATE ORIGINAL files locally; no game assets are transmitted or
written into Git. The exact Level22 selection is u16(INIT+0x0E) +
signed(level header+7). This script decodes the selected 4096-byte
source texture and fingerprints it independently from native C++.
It does not prove the final native VGA, DOS pixels or IRQ equivalence.

python DemoV1/tools/probe_original_res4_selection.py \
  --init INIT --res4 RES4 --lev022 LEV022.DAT --json
"""
from __future__ import annotations

import argparse
import hashlib
import json
import struct
from pathlib import Path


def u32(data: bytes, offset: int) -> int:
    return struct.unpack_from("<I", data, offset)[0]


def read_offsets(raw: bytes) -> list[int]:
    if len(raw) < 4:
        raise ValueError("RES4 shorter than 4-byte entry count")
    count = u32(raw, 0)
    if count > (len(raw) - 4) // 4:
        raise ValueError("RES4 index table exceeds file")
    table_end = 4 + count * 4
    offsets = [u32(raw, 4 + i * 4) for i in range(count)]
    if offsets and offsets[0] != table_end:
        raise ValueError("RES4 entry 0 does not start after offset table")
    if any(a > b or a > len(raw) for a, b in
           zip(offsets, offsets[1:] + [len(raw)])):
        raise ValueError("RES4 nonmonotonic or out-of-file entry offset")
    return offsets


def entry(raw: bytes, offsets: list[int], index: int) -> bytes:
    start = offsets[index]
    stop = offsets[index + 1] if index + 1 < len(offsets) else len(raw)
    return raw[start:stop]


def eh6_layout(raw: bytes) -> tuple[int, int]:
    if len(raw) < 7 or raw[:3] != b"EH\x06":
        raise ValueError("source resource is not original EH6 block")
    extended = struct.unpack_from("<h", raw, 3)[0]
    if extended < 0 or extended > len(raw) - 7:
        raise ValueError("invalid original EH6 extended header")
    pos = 5 + extended
    return pos + 2, struct.unpack_from("<H", raw, pos)[0]


def original_37700_lzss(source: bytes, count: int) -> tuple[bytes, int]:
    """Exact source bit control/backref order, independent of native C++."""
    history = [-1] * 4096
    history[:0xFEE] = [0xFE] * 0xFEE
    ring = 0xFEE
    inp = 0
    flags = 0
    result = bytearray()
    while len(result) < count:
        flags >>= 1
        if not flags & 0x100:
            if inp >= len(source):
                raise ValueError("original EH6 LZSS control EOF")
            flags = source[inp] | 0xFF00
            inp += 1
        if flags & 1:
            if inp >= len(source):
                raise ValueError("original EH6 LZSS literal EOF")
            value = source[inp]
            inp += 1
            result.append(value)
            history[ring] = value
            ring = (ring + 1) & 0xFFF
        else:
            if inp + 1 >= len(source):
                raise ValueError("original EH6 LZSS backref EOF")
            low, high = source[inp:inp+2]
            inp += 2
            at = low | ((high & 0xF0) << 4)
            length = (high & 15) + 3
            for j in range(length):
                if len(result) == count:
                    break
                value = history[(at + j) & 0xFFF]
                if value < 0:
                    raise ValueError("original EH6 LZSS undefined dictionary")
                result.append(value)
                history[ring] = value
                ring = (ring + 1) & 0xFFF
    return bytes(result), inp


def classify(raw: bytes) -> dict:
    label = "not-EH"
    kind = -1
    if len(raw) >= 3 and raw[:2] == b"EH":
        kind = raw[2]
        label = f"EH-{kind}"
    out = {
        "type": label,
        "byte_count": len(raw),
        "first_13_bytes_hex": raw[:13].hex(" ").upper(),
        "source_ground_candidate": False,
    }
    if kind == 6 and len(raw) >= 7:
        try:
            at, declared = eh6_layout(raw)
            out.update(runtime_uncompressed_length=declared,
                       original_eh6_payload_bytes=len(raw)-at,
                       runtime_cache_type_after_2EF38=1)
            out["source_ground_candidate"] = declared == 4096
        except ValueError as exc:
            out["header_error"] = str(exc)
    elif kind == 1 and len(raw) >= 13:
        count, height = struct.unpack_from("<hh", raw, 5)
        units, key, compression, transformed = raw[9:13]
        out.update(count=count, height=height, width_units=units,
                   transparency=key, compression=compression,
                   transform=transformed)
        out["source_ground_candidate"] = (
            (count == 1 and height == 64 and units == 8)
            or (transformed != 0 and count == 64 and height == 1
                and units == 8))
    return out


def inspect(original_init: bytes, res4: bytes, selector: int) -> dict:
    if len(original_init) < 0x10:
        raise ValueError("INIT truncated before original CODE.2 +0x0E")
    offsets = read_offsets(res4)
    base = struct.unpack_from("<H", original_init, 0x0E)[0]
    absolute = base + selector
    kind_count: dict[str, int] = {}
    candidates = []
    for i in range(len(offsets)):
        raw = entry(res4, offsets, i)
        meta = classify(raw)
        kind_count[meta["type"]] = kind_count.get(meta["type"], 0) + 1
        if meta["source_ground_candidate"]:
            candidates.append({
                "index": i,
                "source_eh_type": meta["type"],
                "implied_base_unverified": i - selector,
                "byte_count": meta["byte_count"],
                "source_selected": i == absolute,
            })
    inside = 0 <= absolute < len(offsets)
    selected_raw = entry(res4, offsets, absolute) if inside else b""
    selected = classify(selected_raw) if inside else None
    shape = bool(selected and selected["source_ground_candidate"])
    original_pixels = None
    bytes_consumed = None
    err = ""
    if not inside:
        err = "selected entry outside original RES4 index"
    elif not shape:
        err = "source-selected entry not an original runtime 4096-byte candidate"
    elif selected_raw[:3] == b"EH\x06":
        try:
            at, length = eh6_layout(selected_raw)
            original_pixels, bytes_consumed = original_37700_lzss(
                selected_raw[at:], length)
            if bytes_consumed != len(selected_raw) - at:
                raise ValueError("source EH6 stream contains unused trailing bytes")
        except ValueError as exc:
            err = str(exc)
    else:
        err = "EH1 selected: this independent probe only decodes EH6"
    valid = original_pixels is not None and len(original_pixels) == 4096
    fingerprint = (hashlib.sha256(original_pixels).hexdigest()
                   if valid else None)
    fnv = None
    if valid:
        value = 2166136261
        for octet in original_pixels:
            value = ((value ^ octet) * 16777619) & 0xFFFFFFFF
        fnv = f"{value:08x}"
    return {
        "schema": "rloft-res4-original-selection-triage-v2",
        "source": "private-original-EH6-37700-decode-not-DOS-VGA-parity",
        "source_code2_rule": "u16(INIT+0x0E) + s16(LevelState.header+7)",
        "init_sha256": hashlib.sha256(original_init).hexdigest(),
        "res4_sha256": hashlib.sha256(res4).hexdigest(),
        "res4_byte_count": len(res4),
        "source_init_base": base,
        "source_level_relative_selector": selector,
        "source_absolute_entry": absolute,
        "res4_entry_count": len(offsets),
        "type_counts": dict(sorted(kind_count.items())),
        "source_entry_in_bounds": inside,
        "source_selected_header": selected,
        "source_selected_header_has_ground_shape": shape,
        "selected_decode_verified": valid,
        "selected_uncompressed_pixels": 4096 if valid else 0,
        "selected_payload_bytes_consumed": bytes_consumed,
        "selected_pixels_sha256": fingerprint,
        "selected_pixels_fnv1a32": fnv,
        "ground_shape_descriptor_candidates": candidates,
        "error": "none" if valid else err,
    }


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--init", type=Path, required=True)
    ap.add_argument("--res4", type=Path, required=True)
    ap.add_argument("--lev022", type=Path)
    ap.add_argument("--selector", type=int)
    ap.add_argument("--json", action="store_true")
    args = ap.parse_args()
    if args.lev022 is None and args.selector is None:
        ap.error("provide original --lev022 or explicit --selector")
    if args.lev022 is not None:
        lev = args.lev022.read_bytes()
        if len(lev) < 0x1D + 9:
            ap.error("LEV022.DAT truncated before source level header +7")
        selector = struct.unpack_from("<h", lev, 0x1D + 7)[0]
        if args.selector is not None and args.selector != selector:
            ap.error("explicit selector disagrees with original LEV022 header")
    else:
        selector = args.selector
    result = inspect(args.init.read_bytes(), args.res4.read_bytes(), selector)
    if args.json:
        print(json.dumps(result, indent=2, sort_keys=True))
    else:
        for key, value in result.items():
            print(f"{key}={json.dumps(value, sort_keys=True)}")
    return 0 if result["selected_decode_verified"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
