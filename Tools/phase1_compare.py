#!/usr/bin/env python3
"""Phase 1: read-only cross-title binary format survey; not a gameplay decoder."""
from __future__ import annotations

import argparse
import collections
import hashlib
import json
import struct
from pathlib import Path

GAMES = {
    "strahd": ("Ravenloft - Strahd's Possession", "RLOFT", "CD_Extracted/RLOFT"),
    "menzo": ("Menzoberranzan", "", "CD_Extracted"),
    "stone": ("Ravenloft - Stone Prophet", "PROPHET", "CD_Extracted"),
}
STOCK_STRIDE = {"NPCS.DAT": 0x45, "OBJECTS.DAT": 0x41}
CORE = ("LEVELS.DAT", "NPCS.DAT", "OBJECTS.DAT", "INFO.DAT", "INIT")


def sha(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as stream:
        for block in iter(lambda: stream.read(1024 * 1024), b""):
            digest.update(block)
    return digest.hexdigest()


def u32(data: bytes, pos: int = 0) -> int:
    return struct.unpack_from("<I", data, pos)[0]


def select(installed: Path, cd: Path, filename: str):
    # Survey preference only; original DOS loader precedence NOT established.
    local = installed / filename
    disc = cd / filename
    if local.is_file():
        return local, "installed", (disc if disc.is_file() else None)
    if disc.is_file():
        return disc, "CD", None
    return None, "absent", None


def records(data: bytes) -> list[bytes]:
    if len(data) < 8:
        raise ValueError("short offset-table archive")
    count = u32(data)
    if not 1 <= count <= 100000 or 4 + 4 * count > len(data):
        raise ValueError("implausible offset-table count")
    offsets = struct.unpack_from(f"<{count}I", data, 4)
    if offsets[0] != 4 + 4 * count or offsets[-1] > len(data):
        raise ValueError("archive offset bounds mismatch")
    if any(a > b for a, b in zip(offsets, offsets[1:])):
        raise ValueError("decreasing record offsets")
    return [data[a:b] for a, b in zip(offsets, (*offsets[1:], len(data)))]


def res(path: Path) -> dict:
    data = path.read_bytes()
    items = records(data)
    types = collections.Counter()
    header_sizes = collections.defaultdict(collections.Counter)
    malformed = 0
    raw_ids = []
    for i, payload in enumerate(items):
        if not payload:
            types["empty"] += 1
        elif payload[:2] == b"EH" and len(payload) >= 5:
            kind = payload[2]
            types[f"EH{kind}"] += 1
            header_len = struct.unpack_from("<h", payload, 3)[0]
            header_sizes[str(kind)][str(header_len)] += 1
            if header_len < 0 or 5 + header_len > len(payload):
                malformed += 1
        else:
            types["raw"] += 1
            raw_ids.append(i)
    return {"bytes": len(data), "sha256": hashlib.sha256(data).hexdigest(),
            "entries": len(items), "types": dict(sorted(types.items())),
            "eh_header_lengths": {k: dict(v) for k, v in sorted(header_sizes.items())},
            "invalid_eh_header_extents": malformed, "raw_ids": raw_ids}


def fixed(path: Path, filename: str) -> dict:
    data = path.read_bytes()
    row = {"bytes": len(data), "sha256": hashlib.sha256(data).hexdigest()}
    if len(data) < 4:
        return row
    n = u32(data)
    row.update(le32_at_0=n, strahd_stride=STOCK_STRIDE[filename],
               strahd_count_stride_fits=(len(data) == 4 + n * STOCK_STRIDE[filename]))
    row["exact_stride_if_le32_is_count"] = (
        (len(data) - 4) // n if n and (len(data) - 4) % n == 0 else None
    )
    stride = row.get("exact_stride_if_le32_is_count")
    if stride and 0 < stride <= 256:
        terminators = printable = 0
        name_span = 20 if filename == "NPCS.DAT" else 40
        for i in range(n):
            name = data[4 + i * stride:4 + i * stride + name_span]
            end = name.find(bytes((0,)))
            if end >= 0:
                terminators += 1
                if all(32 <= byte < 127 for byte in name[:end]):
                    printable += 1
        row["first_name_span"] = name_span
        row["name_terminated_count"] = terminators
        row["name_printable_count"] = printable
    return row


def levels(path: Path) -> dict:
    data = path.read_bytes()
    row = {"bytes": len(data), "sha256": hashlib.sha256(data).hexdigest()}
    if len(data) < 8:
        row["directory_valid"] = False
        return row
    n = u32(data)
    row["le32_at_0"] = n
    if not 1 <= n <= 65535 or 4 + 4 * n > len(data):
        row["directory_valid"] = False
        return row
    offsets = struct.unpack_from(f"<{n}I", data, 4)
    if (offsets[0] != 4 + 4 * n or offsets[-1] > len(data)
            or any(a >= b for a, b in zip(offsets, offsets[1:]))):
        row["directory_valid"] = False
        return row
    row.update(directory_valid=True, records=n)
    preamble = grids = scripts = plausible = 0
    dimensions = collections.Counter()
    header_span_hypotheses = collections.Counter()
    for a, b in zip(offsets, (*offsets[1:], len(data))):
        rec = data[a:b]
        if len(rec) < 29:
            continue
        bounds = struct.unpack_from("<7I", rec, 1)
        if not (bounds[0] == 29 and bounds[-1] <= len(rec)
                and all(a <= b for a, b in zip(bounds, bounds[1:]))):
            continue
        preamble += 1
        if bounds[0] + 5 <= len(rec):
            width, height = struct.unpack_from("<hh", rec, bounds[0] + 1)
            if 0 < width <= 256 and 0 < height <= 256:
                plausible += 1
                dimensions[f"{width}x{height}"] += 1
                header_span_hypotheses[bounds[1] - 29 - width * height * 2] += 1
                if bounds[1] == 29 + 0x2A4 + width * height * 2:
                    grids += 1
        if bounds[6] - bounds[5] == 0x514:
            scripts += 1
    row.update(strahd_preamble_records=preamble, strahd_grid_span_matches=grids,
               strahd_script_span_matches=scripts, plausible_dimensions=plausible,
               dimensions=dict(sorted(dimensions.items())),
               fixed_header_candidate_bytes=dict(sorted(header_span_hypotheses.items())))
    return row


def info(path: Path) -> dict:
    data = path.read_bytes()
    row = {"bytes": len(data), "sha256": hashlib.sha256(data).hexdigest()}
    valid = len(data) >= 15 and all(48 <= c <= 57 for c in data[:13])
    if valid:
        count = struct.unpack_from("<h", data, 13)[0]
        valid = count >= 0 and 15 + count == len(data)
    row["strahd_selector_layout_fits"] = valid
    if valid:
        row.update(selectors=data[:13].decode("ascii"), path_byte_length=count)
    return row


def inspect(root: Path, name: str) -> dict:
    title, installed_rel, cd_rel = GAMES[name]
    base = root / title
    installed, cd = base / installed_rel, base / cd_rel
    if not base.is_dir():
        raise ValueError(f"missing game root: {base}")
    result = {"title": name, "res": {}, "core": {}, "saves": {}}
    for idx in range(13):
        filename = f"RES{idx}"
        file, origin, other = select(installed, cd, filename)
        if file is None:
            continue
        entry = {"file": file.relative_to(base).as_posix(), "origin": origin,
                 "within_original_11_group_setup": idx <= 10}
        try:
            entry.update(res(file))
        except ValueError as error:
            entry["structural_error"] = str(error)
        if other:
            entry["cd_copy_sha256"] = sha(other)
            entry["identical_installed_and_cd"] = entry.get("sha256") == entry["cd_copy_sha256"]
        result["res"][filename] = entry
    for filename in CORE:
        file, origin, other = select(installed, cd, filename)
        if not file:
            result["core"][filename] = {"status": "absent"}
            continue
        if filename == "LEVELS.DAT":
            row = levels(file)
        elif filename in STOCK_STRIDE:
            row = fixed(file, filename)
        elif filename == "INFO.DAT":
            row = info(file)
        else:
            row = {"bytes": file.stat().st_size, "sha256": sha(file),
                   "format": "opaque"}
        row.update(file=file.relative_to(base).as_posix(), origin=origin)
        if other:
            row["cd_sha256_matches"] = sha(other) == row["sha256"]
        result["core"][filename] = row
    for filename in ("CHARS", "SAVE.DAT"):
        result["saves"][filename] = [
            {"file": f.relative_to(base).as_posix(), "bytes": f.stat().st_size}
            for f in sorted(base.rglob(filename)) if f.is_file()
        ][:30]
    return result


def markdown(audit: dict) -> str:
    lines = ["# Phase 1: cross-title file-format foundation", "",
             "Source: read-only DnD-Classics checkout. File preference in this survey "
             "does not prove original DOS search order.", "",
             "## RES archives", "",
             "| Game | Archive | Source | Entries | EH types | Raw | Empty | Bad EH spans |",
             "| --- | --- | --- | ---: | --- | ---: | ---: | ---: |"]
    for name, game in audit.items():
        for filename, row in game["res"].items():
            types = row.get("types", {})
            eh = ", ".join(f"{k}:{n}" for k,n in types.items() if k.startswith("EH"))
            lines.append(f"| {name} | {filename} | {row['origin']} | "
                         f"{row.get('entries', 'INVALID')} | {eh or 'none'} | "
                         f"{types.get('raw',0)} | {types.get('empty',0)} | "
                         f"{row.get('invalid_eh_header_extents', 'INVALID')} |")
    lines += ["", "## Core data: Strahd layout compatibility probes", "",
              "| Game | File | Bytes | Probe |", "| --- | --- | ---: | --- |"]
    for name, game in audit.items():
        for filename, row in game["core"].items():
            if row.get("status") == "absent":
                lines.append(f"| {name} | {filename} | - | absent |")
                continue
            if filename == "LEVELS.DAT":
                probe = (f"index={row['directory_valid']} records={row.get('records','?')} "
                         f"preamble={row.get('strahd_preamble_records','?')} "
                         f"grid={row.get('strahd_grid_span_matches','?')} "
                         f"scripts={row.get('strahd_script_span_matches','?')} "
                         f"header-candidates={row.get('fixed_header_candidate_bytes',{})}")
            elif filename in STOCK_STRIDE:
                probe = (f"word0={row.get('le32_at_0','?')} "
                         f"Strahd-stride-fits={row.get('strahd_count_stride_fits')} "
                         f"exact-stride={row.get('exact_stride_if_le32_is_count')} "
                         f"ASCII-NUL-names={row.get('name_printable_count','?')}/"
                         f"{row.get('le32_at_0','?')}")
            elif filename == "INFO.DAT":
                probe = f"Strahd-selector-layout-fits={row['strahd_selector_layout_fits']}"
            else:
                probe = "opaque; size and SHA-256 only"
            lines.append(f"| {name} | {filename} | {row['bytes']} | {probe} |")
    lines += ["", "## Interpretation limits", "",
              "- Matching outer archive structure does not prove matching codecs or IDs.",
              "- Raw entries may be intentional and are not evidence of unused files.",
              "- Strahd fixed-record geometry is only a hypothesis for other games.",
              "- CHARS/SAVE sizes are reported without interpreting serialization.",
              "- Cinematics (ANM, DFA, ACT) are unparsed in this first component."]
    return "\n".join(lines) + "\n"


def main() -> None:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--root", required=True, type=Path)
    ap.add_argument("--json", required=True, type=Path)
    ap.add_argument("--markdown", required=True, type=Path)
    args = ap.parse_args()
    audit = {key: inspect(args.root.resolve(strict=True), key) for key in GAMES}
    args.json.parent.mkdir(parents=True, exist_ok=True)
    args.json.write_text(json.dumps(audit, indent=2) + "\n", encoding="utf-8")
    text = markdown(audit)
    args.markdown.write_text(text, encoding="utf-8")
    print(text)


if __name__ == "__main__":
    main()
