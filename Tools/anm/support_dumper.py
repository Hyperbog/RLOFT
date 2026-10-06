#!/usr/bin/env python3
"""Extract Ravenloft standalone media-support files without losing source bytes."""
from __future__ import annotations

import json
import shutil
import struct
from pathlib import Path

from PIL import Image

import media_inventory as inventory


class SupportError(RuntimeError):
    pass


def _copy_source(source: Path, destination: Path) -> None:
    destination.parent.mkdir(parents=True, exist_ok=True)
    shutil.copyfile(source, destination)


def dump_colors(root: Path, destination: Path) -> dict[str, object]:
    source = Path(root) / "COLORS"
    data = source.read_bytes()
    if len(data) != 11521 or data[0] != 15:
        raise SupportError("COLORS does not match the reconstructed 15-palette geometry")
    destination = Path(destination)
    destination.mkdir(parents=True, exist_ok=True)
    _copy_source(source, destination / "COLORS.original")

    palettes = []
    for index in range(15):
        raw6 = data[1 + index * 768:1 + (index + 1) * 768]
        (destination / f"palette_{index:02d}.rgb6").write_bytes(raw6)
        rgb8 = bytes((v * 255 + 31) // 63 for v in raw6)
        image = Image.new("RGB", (16, 16))
        image.putdata([tuple(rgb8[i:i+3]) for i in range(0, 768, 3)])
        image.resize((256, 256), Image.Resampling.NEAREST).save(
            destination / f"palette_{index:02d}.png", format="PNG"
        )
        palettes.append({
            "index": index,
            "source_offset": 1 + index * 768,
            "colors": 256,
            "canonical_format": "256 RGB triplets, 6-bit VGA DAC components",
        })

    manifest = {"source": "COLORS", "palette_count": 15, "palettes": palettes}
    (destination / "palettes.json").write_text(
        json.dumps(manifest, indent=2) + "\n", encoding="utf-8"
    )
    return manifest


def parse_t(path: Path) -> list[tuple[int, bytes]]:
    data = Path(path).read_bytes()
    if len(data) < 4:
        raise SupportError("T is truncated")
    count = struct.unpack_from("<I", data, 0)[0]
    table_end = 4 + count * 4
    if table_end > len(data):
        raise SupportError("T offset table exceeds EOF")
    offsets = struct.unpack_from(f"<{count}I", data, 4)
    records = []
    for index, offset in enumerate(offsets):
        if offset + 2 > len(data):
            raise SupportError(f"T record {index}: offset outside EOF")
        length = struct.unpack_from("<H", data, offset)[0]
        end = offset + 2 + length
        if end > len(data):
            raise SupportError(f"T record {index}: payload outside EOF")
        encoded = data[offset + 2:end]
        records.append((offset, bytes(b ^ 0xDC for b in encoded)))
    return records


def dump_text(root: Path, destination: Path) -> dict[str, object]:
    source = Path(root) / "T"
    records = parse_t(source)
    destination = Path(destination)
    binary_dir = destination / "decoded_bytes"
    text_dir = destination / "cp437_text"
    binary_dir.mkdir(parents=True, exist_ok=True)
    text_dir.mkdir(parents=True, exist_ok=True)
    _copy_source(source, destination / "T.original")

    manifest_records = []
    for index, (offset, decoded) in enumerate(records):
        (binary_dir / f"T_{index:04d}.bin").write_bytes(decoded)
        # CP437 is a reversible convenience view; decoded bytes remain canonical.
        text = decoded.decode("cp437")
        (text_dir / f"T_{index:04d}.txt").write_text(text, encoding="utf-8")
        manifest_records.append({
            "index": index,
            "offset": offset,
            "length": len(decoded),
            "cp437": text,
        })

    manifest = {
        "source": "T",
        "record_count": len(records),
        "canonical_export": "decoded_bytes/*.bin",
        "text_view": "cp437_text/*.txt",
        "records": manifest_records,
    }
    (destination / "text_manifest.json").write_text(
        json.dumps(manifest, indent=2, ensure_ascii=False) + "\n", encoding="utf-8"
    )
    return manifest


def parse_sample_ad(path: Path) -> list[dict[str, object]]:
    data = Path(path).read_bytes()
    pos = 0
    directory: list[tuple[int, int, int]] = []
    while True:
        if pos + 2 > len(data):
            raise SupportError("SAMPLE.AD directory lacks FF FF terminator")
        patch, bank = data[pos], data[pos + 1]
        pos += 2
        if patch == 0xFF and bank == 0xFF:
            break
        if pos + 4 > len(data):
            raise SupportError("SAMPLE.AD directory entry is truncated")
        offset = struct.unpack_from("<I", data, pos)[0]
        pos += 4
        directory.append((patch, bank, offset))

    result = []
    for ordinal, (patch, bank, offset) in enumerate(directory):
        if offset + 2 > len(data):
            raise SupportError(f"SAMPLE.AD timbre {ordinal}: invalid offset")
        record_length = struct.unpack_from("<H", data, offset)[0]
        record = data[offset:offset + record_length]
        if len(record) != record_length or record_length < 14:
            raise SupportError(f"SAMPLE.AD timbre {ordinal}: invalid record")
        payload = record[2:14]
        fields = struct.unpack("<bBBBBBBBBBBB", payload)
        result.append({
            "ordinal": ordinal,
            "patch": patch,
            "bank": bank,
            "offset": offset,
            "record_length": record_length,
            "record": record,
            "transpose": fields[0],
            "mod_avekm": fields[1],
            "mod_ksl_tl": fields[2],
            "mod_ad": fields[3],
            "mod_sr": fields[4],
            "mod_ws": fields[5],
            "fb_c": fields[6],
            "car_avekm": fields[7],
            "car_ksl_tl": fields[8],
            "car_ad": fields[9],
            "car_sr": fields[10],
            "car_ws": fields[11],
        })
    return result


def dump_sample_ad(root: Path, destination: Path) -> dict[str, object]:
    source = Path(root) / "SAMPLE.AD"
    timbres = parse_sample_ad(source)
    destination = Path(destination)
    records_dir = destination / "timbres"
    records_dir.mkdir(parents=True, exist_ok=True)
    _copy_source(source, destination / "SAMPLE.AD.original")

    manifest_entries = []
    for item in timbres:
        stem = f"bank_{item['bank']:03d}_patch_{item['patch']:03d}"
        (records_dir / f"{stem}.opl2.bin").write_bytes(item["record"])
        public = {k: v for k, v in item.items() if k != "record"}
        (records_dir / f"{stem}.json").write_text(
            json.dumps(public, indent=2) + "\n", encoding="utf-8"
        )
        manifest_entries.append(public)

    manifest = {
        "source": "SAMPLE.AD",
        "format": "Miles AIL 2.x YM3812/OPL2 Global Timbre Library",
        "timbre_count": len(timbres),
        "timbres": manifest_entries,
    }
    (destination / "sample_ad_manifest.json").write_text(
        json.dumps(manifest, indent=2) + "\n", encoding="utf-8"
    )
    return manifest


def dump_banks(root: Path, destination: Path) -> dict[str, object]:
    destination = Path(destination)
    destination.mkdir(parents=True, exist_ok=True)
    records = []
    for name, rom in (("GM1.BNK", "512 KiB ARIA ROM"), ("GM2.BNK", "1 MiB ARIA ROM")):
        source = Path(root) / name
        target = destination / name
        _copy_source(source, target)
        spec = inventory.SOURCE_BY_NAME[name]
        records.append({
            "file": name,
            "bytes": target.stat().st_size,
            "sha256": spec.sha256,
            "classification": "Sierra Semiconductor ARIA General MIDI support bank",
            "hardware_profile": rom,
            "inner_grammar": "not yet reconstructed; preserved byte-for-byte",
        })
    manifest = {"banks": records}
    (destination / "aria_banks.json").write_text(
        json.dumps(manifest, indent=2) + "\n", encoding="utf-8"
    )
    return manifest


def dump_init(root: Path, destination: Path) -> dict[str, object]:
    source = Path(root) / "INIT"
    data = source.read_bytes()
    if len(data) < 36:
        raise SupportError("INIT is truncated")
    words = list(struct.unpack_from("<18H", data, 0))
    names = [
        "res0_ui_graphics_base", "res1_object_ui_base", "res1_portrait_base",
        "res3_class0_base", "res3_class1_base", "res3_class2_base", "res3_class3_base",
        "res4_narrow_texture_base", "res5_alternate_xmidi_offset",
        "res6_sfx_base_0", "res6_sfx_base_1", "res6_sfx_base_2",
        "res6_sfx_base_3", "res6_end_boundary", "res9_graphics_base",
        "res9_interaction_base", "res10_voice_sfx_base", "res9_wordcode_displacement",
    ]
    mapping = dict(zip(names, words))
    destination = Path(destination)
    destination.mkdir(parents=True, exist_ok=True)
    _copy_source(source, destination / "INIT.original")
    (destination / "init_bases.json").write_text(
        json.dumps(mapping, indent=2) + "\n", encoding="utf-8"
    )
    return mapping


def dump_sintab(root: Path, destination: Path) -> dict[str, object]:
    source = Path(root) / "SINTAB"
    data = source.read_bytes()
    destination = Path(destination)
    block_dir = destination / "scale_blocks"
    block_dir.mkdir(parents=True, exist_ok=True)
    _copy_source(source, destination / "SINTAB.original")

    pos = 0x1B38
    if pos + 12 > len(data):
        raise SupportError("SINTAB truncated before dynamic header")
    block_count, lookup_max, aux = struct.unpack_from("<III", data, pos)
    pos += 12
    lookup_count = lookup_max + 1
    lookup_bytes = lookup_count * 2
    if pos + lookup_bytes + 4 > len(data):
        raise SupportError("SINTAB runtime lookup exceeds EOF")
    pos += lookup_bytes
    payload_total = struct.unpack_from("<I", data, pos)[0]
    pos += 4

    blocks = []
    payload_sum = 0
    for index in range(block_count):
        if pos + 2 > len(data):
            raise SupportError(f"SINTAB scale block {index}: missing length")
        size = struct.unpack_from("<H", data, pos)[0]
        pos += 2
        body = data[pos:pos + size]
        if len(body) != size:
            raise SupportError(f"SINTAB scale block {index}: truncated")
        (block_dir / f"scale_{index:03d}.bin").write_bytes(body)
        blocks.append({"index": index, "size": size, "file_offset": pos})
        pos += size
        payload_sum += size

    trailer = data[pos:]
    manifest = {
        "source": "SINTAB",
        "scale_block_count": block_count,
        "runtime_lookup_max": lookup_max,
        "renderer_aux_value": aux,
        "stored_payload_bytes": payload_total,
        "decoded_payload_sum": payload_sum,
        "trailer_bytes": len(trailer),
        "trailer_all_zero": not any(trailer),
        "blocks": blocks,
    }
    (destination / "sintab_manifest.json").write_text(
        json.dumps(manifest, indent=2) + "\n", encoding="utf-8"
    )
    return manifest


def dump_support_files(root: Path, destination: Path) -> dict[str, object]:
    destination = Path(destination)
    result = {
        "palettes": dump_colors(root, destination / "palettes"),
        "text": dump_text(root, destination / "text"),
        "opl2": dump_sample_ad(root, destination / "opl2"),
        "aria": dump_banks(root, destination / "aria"),
        "init": dump_init(root, destination / "init"),
        "sintab": dump_sintab(root, destination / "renderer_support"),
    }
    (destination / "support_manifest.json").write_text(
        json.dumps({
            "sections": list(result),
            "note": "Original carrier files are retained beside decoded views.",
        }, indent=2) + "\n", encoding="utf-8"
    )
    return result
