#!/usr/bin/env python3
"""Semantic multipart grouping for Ravenloft RES3 scene graphics.

This exporter is additive to the raw RES dump.  It uses the same static tables
that the original renderer uses to select class-0/1/2/3 type-7 shapes, so
creature, object, scene-descriptor, and transient-effect resources can be
organized for modding/upscaling without image-similarity guesses.
"""
from __future__ import annotations

import json
import math
import re
import struct
from dataclasses import dataclass, field
from functools import lru_cache
from pathlib import Path
from typing import Iterable

from PIL import Image, ImageDraw

import palette_router
import res_assets as ra


ORGANIZATION_BOTH = "Raw + grouped (recommended)"
ORGANIZATION_RAW = "Raw only"
ORGANIZATION_GROUPED = "Grouped only"
ORGANIZATIONS = (ORGANIZATION_BOTH, ORGANIZATION_RAW, ORGANIZATION_GROUPED)

FAMILY_NPCS = "npcs"
FAMILY_OBJECTS = "objects"
FAMILY_SCENE = "scene_descriptors"
FAMILY_EFFECTS = "effects"
ALL_FAMILIES = (FAMILY_NPCS, FAMILY_OBJECTS, FAMILY_SCENE, FAMILY_EFFECTS)

FAMILY_LABELS = {
    FAMILY_NPCS: "NPCs / creatures",
    FAMILY_OBJECTS: "Objects / items",
    FAMILY_SCENE: "INIT scene descriptors",
    FAMILY_EFFECTS: "Effects / spells",
}

GRAPHICS_FORMATS = {
    "PNG (lossless indexed)": (".png", "PNG"),
    "TIFF (lossless)": (".tiff", "TIFF"),
    "BMP (lossless)": (".bmp", "BMP"),
    "WebP (lossless)": (".webp", "WEBP"),
}

# These are deliberately producer-oriented labels, not claims that a numeric
# effect type belongs to only one spell.  The shared classes are documented in
# DatFiles/SAVE.DAT/GLOBAL_STATE.md.
EFFECT_LABELS = {
    0: ("fireball_shared_effect_class", ["Fireball", "Wand of Fireballs", "LEVELS opcode 3"]),
    1: ("lightning_bolt_shared_effect_class", ["Lightning Bolt", "LEVELS opcode 3"]),
    2: ("ice_knife", ["Ice Knife"]),
    3: ("cone_of_cold_wand_of_frost", ["Cone of Cold", "Wand of Frost"]),
    4: ("flame_arrow", ["Flame Arrow"]),
    5: ("effect_type_05", []),
    6: ("magic_missile_shared_effect_class", ["Magic Missile", "Wand of Magic Missiles"]),
    7: ("effect_type_07", []),
    8: ("burning_hands_agannazars_scorcher", ["Burning Hands", "Agannazar's Scorcher"]),
    9: ("ice_storm_freezing_sphere", ["Ice Storm", "Otiluke's Freezing Sphere"]),
    10: ("flame_strike", ["Flame Strike"]),
    11: ("holy_symbol_of_ravenkind", ["Holy Symbol of Ravenkind"]),
}


@dataclass(frozen=True)
class Descriptor:
    local_shape_base: int
    extent_raw: int
    shape_stride: int
    variant_count: int
    scene_influence_step: int


@dataclass(frozen=True)
class GroupFrame:
    res3_id: int
    variant: int
    view: int


@dataclass
class MultipartGroup:
    family: str
    directory: str
    source_table: str
    record_index: int
    display_name: str
    source_class: int
    local_shape_base: int
    absolute_shape_base: int
    shape_stride: int
    variant_count: int
    frames: list[GroupFrame]
    evidence: str
    extra: dict[str, object] = field(default_factory=dict)

    @property
    def ref(self) -> str:
        return f"{self.family}/{self.directory}"

    @property
    def res3_ids(self) -> list[int]:
        return [frame.res3_id for frame in self.frames]


def _u16(data: bytes, offset: int) -> int:
    return struct.unpack_from("<H", data, offset)[0]


def _u32(data: bytes, offset: int) -> int:
    return struct.unpack_from("<I", data, offset)[0]


def _fixed_name(data: bytes) -> str:
    raw = data.split(b"\0", 1)[0]
    return raw.decode("cp437", errors="replace").strip()


def _slug(value: str) -> str:
    value = value.strip().lower().replace("&", " and ")
    value = re.sub(r"[^a-z0-9]+", "_", value).strip("_")
    return value or "unnamed"


def parse_init(path: Path) -> tuple[tuple[int, int, int, int], list[Descriptor], list[Descriptor]]:
    data = Path(path).read_bytes()
    if len(data) < 0x24:
        raise ra.AssetError("INIT is shorter than the certified 0x24-byte fixed header")
    words = [_u16(data, offset) for offset in range(0, 0x24, 2)]
    bases = tuple(words[3:7])
    if len(bases) != 4 or tuple(sorted(bases)) != bases:
        raise ra.AssetError(f"INIT RES3 source-bank bases are invalid: {bases}")

    def table(offset: int) -> tuple[list[Descriptor], int]:
        if offset + 2 > len(data):
            raise ra.AssetError("INIT descriptor count is truncated")
        count = _u16(data, offset)
        offset += 2
        records: list[Descriptor] = []
        for _ in range(count):
            raw = data[offset:offset + 7]
            if len(raw) != 7:
                raise ra.AssetError("INIT descriptor table is truncated")
            records.append(
                Descriptor(
                    local_shape_base=_u16(raw, 0),
                    extent_raw=_u16(raw, 2),
                    shape_stride=raw[4],
                    variant_count=raw[5],
                    scene_influence_step=raw[6],
                )
            )
            offset += 7
        return records, offset

    class1, offset = table(0x24)
    class3, _ = table(offset)
    if len(class1) != 175 or len(class3) != 12:
        raise ra.AssetError(
            f"INIT descriptor counts differ from supplied build: "
            f"class1={len(class1)} class3={len(class3)}"
        )
    return bases, class1, class3


def parse_npcs(path: Path) -> list[dict[str, object]]:
    data = Path(path).read_bytes()
    if len(data) < 4:
        raise ra.AssetError("NPCS.DAT is truncated")
    count = _u32(data, 0)
    stride = 0x45
    if 4 + count * stride != len(data):
        raise ra.AssetError("NPCS.DAT count/size mismatch")
    records: list[dict[str, object]] = []
    for index in range(count):
        rec = data[4 + index * stride:4 + (index + 1) * stride]
        records.append({
            "index": index,
            "name": _fixed_name(rec[:20]),
            "local_shape_base": _u16(rec, 0x14),
            "scene_frame_stride": rec[0x18],
            "animation_cycle_divisor": rec[0x19],
            "animation_span": rec[0x1A],
            "damage_reaction_animation_flag": rec[0x1C],
        })
    return records


def parse_objects(path: Path) -> list[dict[str, object]]:
    data = Path(path).read_bytes()
    if len(data) < 4:
        raise ra.AssetError("OBJECTS.DAT is truncated")
    count = _u32(data, 0)
    stride = 0x41
    if 4 + count * stride != len(data):
        raise ra.AssetError("OBJECTS.DAT count/size mismatch")
    records: list[dict[str, object]] = []
    for index in range(count):
        rec = data[4 + index * stride:4 + (index + 1) * stride]
        records.append({
            "index": index,
            "name": _fixed_name(rec[:40]),
            "local_shape_base": _u16(rec, 0x28),
            "shape_stride": rec[0x2A],
            "item_category": rec[0x2F],
            "ui_graphic_index": _u16(rec, 0x2D),
        })
    return records


def _frames_for_range(absolute_start: int, count: int, stride: int) -> list[GroupFrame]:
    stride = max(1, stride)
    frames: list[GroupFrame] = []
    for offset in range(count):
        frames.append(
            GroupFrame(
                res3_id=absolute_start + offset,
                variant=offset // stride,
                view=offset % stride,
            )
        )
    return frames


def build_group_plan(
    game_dir: Path,
    res3_count: int,
    families: Iterable[str] = ALL_FAMILIES,
) -> tuple[list[MultipartGroup], list[str]]:
    """Build source-backed multipart groups without decoding image pixels."""
    game_dir = Path(game_dir)
    selected = set(families)
    unknown = selected.difference(ALL_FAMILIES)
    if unknown:
        raise ra.AssetError(f"unknown grouped graphics families: {sorted(unknown)}")

    bases, class1, class3 = parse_init(game_dir / "INIT")
    bank_ends = (bases[1], bases[2], bases[3], res3_count)
    groups: list[MultipartGroup] = []
    warnings: list[str] = []

    if FAMILY_NPCS in selected:
        path = game_dir / "NPCS.DAT"
        if path.is_file():
            npcs = parse_npcs(path)
            unique_bases = sorted({int(rec["local_shape_base"]) for rec in npcs})
            next_base: dict[int, int] = {}
            for pos, base in enumerate(unique_bases):
                next_base[base] = (
                    unique_bases[pos + 1] if pos + 1 < len(unique_bases) else bank_ends[0]
                )
            for rec in npcs:
                local_base = int(rec["local_shape_base"])
                allocation_end = min(next_base[local_base], bank_ends[0])
                count = max(0, allocation_end - local_base)
                # The class-0 database has no explicit allocation-count field.
                # Distinct definition bases and the class-0 bank boundary give a
                # preservation-safe grouping window; runtime frame selection
                # remains governed by CODE.2's state arithmetic.
                if count <= 1:
                    continue
                stride = max(1, int(rec["scene_frame_stride"]))
                name = str(rec["name"])
                frames = _frames_for_range(bases[0] + local_base, count, stride)
                groups.append(
                    MultipartGroup(
                        family=FAMILY_NPCS,
                        directory=f'{int(rec["index"]):03d}_{_slug(name)}',
                        source_table="NPCS.DAT",
                        record_index=int(rec["index"]),
                        display_name=name or f'NPC {int(rec["index"])}',
                        source_class=0,
                        local_shape_base=local_base,
                        absolute_shape_base=bases[0] + local_base,
                        shape_stride=stride,
                        variant_count=math.ceil(count / stride),
                        frames=frames,
                        evidence=(
                            "NPCS.DAT +0x14/+0x18 plus RES3 class-0 bank boundaries; "
                            "allocation window ends at the next distinct NPC base."
                        ),
                        extra={
                            "animation_cycle_divisor": rec["animation_cycle_divisor"],
                            "animation_span": rec["animation_span"],
                            "damage_reaction_animation_flag": rec["damage_reaction_animation_flag"],
                            "allocation_window_local": [local_base, allocation_end - 1],
                            "allocation_note": (
                                "Window groups all physically associated class-0 shapes. "
                                "variant/view labels are generic organization labels; "
                                "CODE.2 runtime state selects the actual visible frame."
                            ),
                        },
                    )
                )
        else:
            warnings.append(
                "NPCS.DAT was not present beside the resource files; NPC/creature grouping was skipped."
            )

    if FAMILY_OBJECTS in selected:
        path = game_dir / "OBJECTS.DAT"
        if path.is_file():
            for rec in parse_objects(path):
                local_base = int(rec["local_shape_base"])
                raw_stride = int(rec["shape_stride"])
                count = max(1, raw_stride)
                if count <= 1:
                    continue
                absolute = bases[2] + local_base
                if absolute + count > bank_ends[2]:
                    warnings.append(
                        f'OBJECTS.DAT record {rec["index"]} extends outside the class-2 RES3 bank'
                    )
                    continue
                name = str(rec["name"])
                groups.append(
                    MultipartGroup(
                        family=FAMILY_OBJECTS,
                        directory=f'{int(rec["index"]):03d}_{_slug(name)}',
                        source_table="OBJECTS.DAT",
                        record_index=int(rec["index"]),
                        display_name=name or f'Object {int(rec["index"])}',
                        source_class=2,
                        local_shape_base=local_base,
                        absolute_shape_base=absolute,
                        shape_stride=raw_stride,
                        variant_count=1,
                        frames=_frames_for_range(absolute, count, count),
                        evidence=(
                            "OBJECTS.DAT +0x28 local_shape_base and +0x2A shape_stride; "
                            "CODE.2 class-2 scene helper selects a view/state subframe inside the stride."
                        ),
                        extra={
                            "item_category": rec["item_category"],
                            "ui_graphic_index": rec["ui_graphic_index"],
                            "variant_note": (
                                "Normal and thrown class-2 construction clears the live variant selector; "
                                "the exported stride is the proved view/state subrange."
                            ),
                        },
                    )
                )
        else:
            warnings.append(
                "OBJECTS.DAT was not present beside the resource files; object/item grouping was skipped."
            )

    if FAMILY_SCENE in selected:
        for index, desc in enumerate(class1):
            count = desc.shape_stride * desc.variant_count
            if count <= 1:
                continue
            absolute = bases[1] + desc.local_shape_base
            if absolute + count > bank_ends[1]:
                warnings.append(f"INIT class-1 descriptor {index} extends outside its RES3 bank")
                continue
            groups.append(
                MultipartGroup(
                    family=FAMILY_SCENE,
                    directory=f"descriptor_{index:03d}",
                    source_table="INIT class-1 descriptor table",
                    record_index=index,
                    display_name=f"Scene descriptor {index:03d}",
                    source_class=1,
                    local_shape_base=desc.local_shape_base,
                    absolute_shape_base=absolute,
                    shape_stride=desc.shape_stride,
                    variant_count=desc.variant_count,
                    frames=_frames_for_range(absolute, count, desc.shape_stride),
                    evidence=(
                        "INIT class-1 7-byte descriptor: local_shape_base + "
                        "variant * shape_stride + view/state subframe."
                    ),
                    extra={
                        "extent_raw": desc.extent_raw,
                        "scene_influence_step": desc.scene_influence_step,
                    },
                )
            )

    if FAMILY_EFFECTS in selected:
        for index, desc in enumerate(class3):
            count = desc.shape_stride * desc.variant_count
            if count <= 1:
                continue
            absolute = bases[3] + desc.local_shape_base
            if absolute + count > bank_ends[3]:
                warnings.append(f"INIT class-3 effect descriptor {index} extends outside its RES3 bank")
                continue
            slug, producers = EFFECT_LABELS.get(index, (f"effect_type_{index:02d}", []))
            groups.append(
                MultipartGroup(
                    family=FAMILY_EFFECTS,
                    directory=f"{index:02d}_{slug}",
                    source_table="INIT class-3 descriptor table",
                    record_index=index,
                    display_name=f"Effect type {index:02d}",
                    source_class=3,
                    local_shape_base=desc.local_shape_base,
                    absolute_shape_base=absolute,
                    shape_stride=desc.shape_stride,
                    variant_count=desc.variant_count,
                    frames=_frames_for_range(absolute, count, desc.shape_stride),
                    evidence=(
                        "INIT class-3 7-byte descriptor and CODE.2 transient/moving-effect "
                        "renderer: variant * shape_stride + view/state subframe."
                    ),
                    extra={
                        "certified_producers": producers,
                        "extent_raw": desc.extent_raw,
                        "scene_influence_step": desc.scene_influence_step,
                    },
                )
            )

    return groups, warnings


def _save_rgba(image: Image.Image, path: Path, fmt_label: str) -> Path:
    suffix, pil_format = GRAPHICS_FORMATS[fmt_label]
    path = path.with_suffix(suffix)
    args: dict[str, object] = {}
    out = image
    if pil_format == "WEBP":
        args.update(lossless=True, quality=100)
    elif pil_format == "TIFF":
        args["compression"] = "tiff_lzw"
    image.save(path, format=pil_format, **args)
    return path


def _contact_sheet(items: list[tuple[Image.Image, str]], path: Path) -> None:
    if not items:
        return
    tile_w = max(min(128, image.width) for image, _ in items)
    tile_h = max(min(128, image.height) for image, _ in items)
    label_h = 18
    columns = min(8, max(1, math.ceil(math.sqrt(len(items)))))
    rows = math.ceil(len(items) / columns)
    cell_w = tile_w + 16
    cell_h = tile_h + label_h + 16
    sheet = Image.new("RGB", (columns * cell_w, rows * cell_h), (18, 12, 13))
    draw = ImageDraw.Draw(sheet)
    for i, (image, label) in enumerate(items):
        row, col = divmod(i, columns)
        thumb = image.copy()
        thumb.thumbnail((tile_w, tile_h), Image.Resampling.NEAREST)
        x = col * cell_w + (cell_w - thumb.width) // 2
        y = row * cell_h + 6
        backing = Image.new("RGB", thumb.size, (0, 0, 0))
        if thumb.mode == "RGBA":
            backing.paste(thumb, (0, 0), thumb)
        else:
            backing.paste(thumb, (0, 0))
        sheet.paste(backing, (x, y))
        draw.text((col * cell_w + 5, row * cell_h + tile_h + 10), label, fill=(235, 225, 205))
    sheet.save(path, format="PNG")


def _animated_preview(images: list[Image.Image], path: Path, duration_ms: int = 120) -> None:
    if len(images) < 2:
        return
    # Keep previews compact even if a future descriptor contains a very large
    # allocation. The complete native images are still exported individually.
    if len(images) > 96:
        step = len(images) / 96.0
        images = [images[min(len(images) - 1, int(i * step))] for i in range(96)]
    width = max(image.width for image in images)
    height = max(image.height for image in images)
    frames: list[Image.Image] = []
    for image in images:
        canvas = Image.new("RGB", (width, height), (16, 8, 10))
        x = (width - image.width) // 2
        y = (height - image.height) // 2
        if image.mode == "RGBA":
            canvas.paste(image, (x, y), image)
        else:
            canvas.paste(image, (x, y))
        frames.append(canvas)
    frames[0].save(
        path,
        format="GIF",
        save_all=True,
        append_images=frames[1:],
        duration=duration_ms,
        loop=0,
        optimize=False,
    )


def export_grouped(
    game_dir: Path,
    destination: Path,
    fmt: str,
    palette_index: int | None = None,
    families: Iterable[str] = ALL_FAMILIES,
    contact_sheets: bool = True,
    animated_previews: bool = True,
    alias_maps: bool = True,
) -> dict[str, object]:
    """Export semantically grouped RES3 multipart assets and helper previews."""
    if fmt not in GRAPHICS_FORMATS:
        raise ra.AssetError(f"unsupported grouped graphics format: {fmt}")

    game_dir = Path(game_dir)
    destination = Path(destination)
    res3_path = game_dir / "RES3"
    if not res3_path.is_file():
        raise ra.AssetError("Grouped multipart extraction requires RES3")
    archive = ra.ResArchive(res3_path, 3)
    resources = {
        resource.index: resource
        for resource in archive.resources()
        if resource.resource_type == 7
    }
    groups, warnings = build_group_plan(game_dir, archive.count, families)
    destination.mkdir(parents=True, exist_ok=True)

    palette_path = game_dir / "COLORS"
    palette_cache: dict[int, bytes] = {}

    def palette_for(res_id: int) -> bytes:
        bank = (
            palette_router.resolve_palette(3, res_id, 7).bank
            if palette_index is None
            else palette_index
        )
        if bank not in palette_cache:
            palette_cache[bank] = ra.palette_from_colors(palette_path, bank)
        return palette_cache[bank]

    @lru_cache(maxsize=256)
    def decoded_image(res_id: int) -> Image.Image:
        resource = resources.get(res_id)
        if resource is None:
            raise ra.AssetError(f"RES3:{res_id} is not an EH type-7 resource")
        body = ra.decode_shared_lzss(resource)
        raster = ra.decode_type7_raster(body)
        indexed = Image.frombytes("P", (raster.width, raster.height), raster.pixels)
        indexed.putpalette(palette_for(res_id))
        image = indexed.convert("RGBA")
        image.putalpha(Image.frombytes("L", (raster.width, raster.height), raster.mask))
        return image

    resource_members: dict[int, list[str]] = {}
    base_members: dict[int, list[str]] = {}
    for group in groups:
        base_members.setdefault(group.absolute_shape_base, []).append(group.ref)
        for res_id in group.res3_ids:
            resource_members.setdefault(res_id, []).append(group.ref)

    group_records: list[dict[str, object]] = []
    frame_instances = 0
    for group in groups:
        group_dir = destination / group.family / group.directory
        frame_dir = group_dir / "frames"
        frame_dir.mkdir(parents=True, exist_ok=True)

        preview_items: list[tuple[Image.Image, str]] = []
        variant_images: dict[int, list[Image.Image]] = {}
        frame_records: list[dict[str, object]] = []
        for sequence, frame in enumerate(group.frames):
            try:
                image = decoded_image(frame.res3_id).copy()
            except ra.AssetError as exc:
                warnings.append(f"{group.ref}: RES3:{frame.res3_id} could not render: {exc}")
                continue

            file_stem = (
                f"{sequence:04d}_variant_{frame.variant:02d}_"
                f"view_{frame.view:02d}_RES3_{frame.res3_id:05d}"
            )
            output = _save_rgba(image, frame_dir / file_stem, fmt)
            rel_output = output.relative_to(group_dir).as_posix()
            frame_records.append({
                "sequence": sequence,
                "variant": frame.variant,
                "view_or_subframe": frame.view,
                "res3_id": frame.res3_id,
                "file": rel_output,
            })
            preview_items.append(
                (image, f"v{frame.variant:02d}/s{frame.view:02d} R{frame.res3_id}")
            )
            variant_images.setdefault(frame.variant, []).append(image)
            frame_instances += 1

        if contact_sheets and preview_items:
            _contact_sheet(preview_items, group_dir / "contact_sheet.png")
        if animated_previews and preview_items:
            _animated_preview(
                [image for image, _ in preview_items],
                group_dir / "preview.gif",
            )
            if len(variant_images) > 1:
                preview_dir = group_dir / "variant_previews"
                preview_dir.mkdir(exist_ok=True)
                for variant, images in sorted(variant_images.items()):
                    _animated_preview(
                        images,
                        preview_dir / f"variant_{variant:02d}.gif",
                    )

        shared_ids = sorted(
            res_id
            for res_id in group.res3_ids
            if len(resource_members.get(res_id, ())) > 1
        )
        shared_with = sorted({
            other
            for res_id in group.res3_ids
            for other in resource_members.get(res_id, ())
            if other != group.ref
        })
        same_base = sorted(
            other
            for other in base_members.get(group.absolute_shape_base, ())
            if other != group.ref
        )
        manifest = {
            "group_ref": group.ref,
            "family": group.family,
            "source_table": group.source_table,
            "record_index": group.record_index,
            "display_name": group.display_name,
            "source_class": group.source_class,
            "local_shape_base": group.local_shape_base,
            "absolute_res3_base": group.absolute_shape_base,
            "shape_stride": group.shape_stride,
            "variant_count": group.variant_count,
            "multipart_frame_count": len(group.frames),
            "rendered_frame_count": len(frame_records),
            "evidence": group.evidence,
            "extra": group.extra,
            "shared_resource_ids": shared_ids,
            "shared_with": shared_with,
            "same_visual_base_as": same_base,
            "frames": frame_records,
        }
        (group_dir / "manifest.json").write_text(
            json.dumps(manifest, indent=2) + "\n",
            encoding="utf-8",
        )
        group_records.append({
            "group_ref": group.ref,
            "family": group.family,
            "display_name": group.display_name,
            "record_index": group.record_index,
            "absolute_res3_base": group.absolute_shape_base,
            "frames": len(group.frames),
            "shared_with_count": len(shared_with),
        })

    if alias_maps:
        shared = {
            str(res_id): refs
            for res_id, refs in sorted(resource_members.items())
            if len(refs) > 1
        }
        shared_bases = {
            str(base): refs
            for base, refs in sorted(base_members.items())
            if len(refs) > 1
        }
        (destination / "shared_resource_aliases.json").write_text(
            json.dumps({
                "shared_res3_resources": shared,
                "shared_visual_bases": shared_bases,
                "note": (
                    "Multiple definitions can intentionally reference the same original "
                    "RES3 artwork. These are aliases, not extraction collisions."
                ),
            }, indent=2) + "\n",
            encoding="utf-8",
        )

    manifest = {
        "format": fmt,
        "palette_mode": "automatic" if palette_index is None else "manual",
        "manual_palette_bank": palette_index,
        "families": sorted(set(families)),
        "group_count": len(groups),
        "frame_instances": frame_instances,
        "unique_res3_resources": len(resource_members),
        "contact_sheets": contact_sheets,
        "animated_previews": animated_previews,
        "alias_maps": alias_maps,
        "warnings": warnings,
        "naming_note": (
            "variant/view labels are intentionally generic unless CODE.2 proves a "
            "narrower semantic name. Grouping is table/renderer-driven, not visual matching."
        ),
        "groups": group_records,
    }
    (destination / "grouped_manifest.json").write_text(
        json.dumps(manifest, indent=2) + "\n",
        encoding="utf-8",
    )
    return manifest
