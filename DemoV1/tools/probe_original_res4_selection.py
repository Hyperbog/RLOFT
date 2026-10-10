"""Independent, read-only original RES4 selected-entry triage (no build needed).

Usage:
  python DemoV1/tools/probe_original_res4_selection.py \
      --init /original/RLOFT/INIT --res4 /original/RLOFT/RES4 \
      --lev022 /original/RLOFT/LEV022.DAT --json

If LEV022.DAT is unavailable, explicitly pass --selector 55 for a
fresh original Level 22 only. Do not use 55 as an absolute RES4 index.
This reports raw EH headers, not decoded pixels or DOS graphics parity.
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
        "descriptor_64x64_shape": False,
    }
    if kind == 1 and len(raw) >= 13:
        count, height = struct.unpack_from("<hh", raw, 5)
        units, key, compressed, transformed = raw[9:13]
        out.update(count=count, height=height, width_units=units,
                   transparency=key, compression=compressed,
                   transform=transformed)
        out["descriptor_64x64_shape"] = (
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
        meta = classify(entry(res4, offsets, i))
        kind_count[meta["type"]] = kind_count.get(meta["type"], 0) + 1
        if meta["descriptor_64x64_shape"]:
            candidates.append({
                "index": i,
                "implied_base_unverified": i - selector,
                "compression": meta["compression"],
                "transform": meta["transform"],
                "byte_count": meta["byte_count"],
                "source_selected": i == absolute,
            })
    inside = 0 <= absolute < len(offsets)
    selected = classify(entry(res4, offsets, absolute)) if inside else None
    valid_header = bool(selected and selected["descriptor_64x64_shape"])
    return {
        "schema": "rloft-res4-original-selection-triage-v1",
        "source": "immutable-media-raw-headers-not-DOS-parity",
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
        "source_selected_header_has_ground_shape": valid_header,
        "selected_decode_verified": False,
        "ground_shape_descriptor_candidates": candidates,
        "error": ("selected entry outside original RES4 index"
                  if not inside else
                  ("source-selected entry is not an EH-1 64x64 header"
                   if not valid_header else
                   "descriptor shape matches; decode/pixels remain unverified")),
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
    selector = None
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
    return 0 if result["source_selected_header_has_ground_shape"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
