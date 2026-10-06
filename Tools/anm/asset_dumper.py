#!/usr/bin/env python3
"""Lossless export services for Ravenloft RES graphics, audio, and music."""
from __future__ import annotations

import json
import os
import shutil
import subprocess
import tempfile
from pathlib import Path
from typing import Callable

from PIL import Image

import res_assets as ra
import runtime_paths
import xmidi

ProgressCallback = Callable[[int, int, str], None]

GRAPHICS_FORMATS = {
    "PNG (lossless indexed)": (".png", "PNG"),
    "TIFF (lossless)": (".tiff", "TIFF"),
    "BMP (lossless)": (".bmp", "BMP"),
    "WebP (lossless)": (".webp", "WEBP"),
}
AUDIO_FORMATS = {
    "VOC (original)": ".voc",
    "WAV (PCM lossless)": ".wav",
    "FLAC (lossless)": ".flac",
}
MUSIC_FORMATS = (
    "XMI/XDIR (original)",
    "Standard MIDI (SMF0, exact 120 Hz)",
    "Both XMI + MIDI",
)


def _stem(r: ra.EhResource) -> str:
    return f"RES{r.archive}_{r.index:05d}_T{r.resource_type}"


def _save_indexed(pixels: bytes, size: tuple[int, int], palette: bytes,
                  path: Path, fmt_label: str) -> Path:
    suffix, pil_format = GRAPHICS_FORMATS[fmt_label]
    path = path.with_suffix(suffix)
    image = Image.frombytes("P", size, pixels)
    image.putpalette(palette)
    args: dict[str, object] = {}
    if pil_format == "WEBP":
        image = image.convert("RGB")
        args.update(lossless=True, quality=100)
    elif pil_format == "TIFF":
        args["compression"] = "tiff_lzw"
    image.save(path, format=pil_format, **args)
    return path


def _save_masked(
    pixels: bytes,
    mask: bytes,
    size: tuple[int, int],
    palette: bytes,
    path: Path,
    fmt_label: str,
) -> Path:
    suffix, pil_format = GRAPHICS_FORMATS[fmt_label]
    path = path.with_suffix(suffix)
    indexed = Image.frombytes("P", size, pixels)
    indexed.putpalette(palette)
    image = indexed.convert("RGBA")
    image.putalpha(Image.frombytes("L", size, mask))
    args: dict[str, object] = {}
    if pil_format == "WEBP":
        args.update(lossless=True, quality=100)
    elif pil_format == "TIFF":
        args["compression"] = "tiff_lzw"
    elif pil_format == "BMP":
        # Pillow writes a 32-bit BMP from RGBA. Alpha support varies among
        # consumers, so the optional standalone mask remains the portable
        # preservation aid for BMP workflows.
        pass
    image.save(path, format=pil_format, **args)
    return path


def _type7_folder(resource: ra.EhResource) -> tuple[str, str | None]:
    if resource.archive != 3:
        return f"type7_legacy_RES{resource.archive}", "legacy"
    if resource.index <= 2168:
        return "type7_npc_creatures", "npc_creatures"
    if resource.index <= 3315:
        return "type7_placed_objects", "placed_objects"
    if resource.index <= 4157:
        return "type7_loose_items", "loose_items"
    return "type7_effects", "effects"


def dump_graphics(game_dir: Path, destination: Path, fmt: str,
                  palette_index: int = 0, preserve_native: bool = False,
                  progress: ProgressCallback | None = None,
                  archives: set[int] | None = None,
                  export_indices: bool = True,
                  export_metadata: bool = True,
                  export_masks: bool = False) -> dict:
    if fmt not in GRAPHICS_FORMATS:
        raise ra.AssetError(f"unsupported graphics format: {fmt}")
    resources = [
        r for archive in ra.discover_archives(game_dir)
        if archives is None or archive.archive_number in archives
        for r in archive.resources()
        if r.resource_type in (1, 2, 6, 7)
    ]
    palette = ra.palette_from_colors(Path(game_dir) / "COLORS", palette_index)
    destination.mkdir(parents=True, exist_ok=True)
    records: list[dict[str, object]] = []

    for ordinal, r in enumerate(resources, 1):
        stem = _stem(r)
        if r.resource_type == 7:
            folder_name, bank = _type7_folder(r)
        else:
            folder_name, bank = f"type{r.resource_type}", None
        folder = destination / folder_name
        folder.mkdir(exist_ok=True)
        rec: dict[str, object] = {
            "archive": r.archive,
            "entry": r.index,
            "type": r.resource_type,
        }
        if bank is not None:
            rec["scene_bank"] = bank
        if preserve_native:
            (folder / f"{stem}.eh").write_bytes(r.raw)

        if r.resource_type == 1:
            raster, _ = ra.decode_type1(r)
            if export_indices:
                (folder / f"{stem}.indices").write_bytes(raster.pixels)
            for frame in range(raster.count):
                stride = raster.width * raster.element_height
                pixels = raster.pixels[frame * stride:(frame + 1) * stride]
                _save_indexed(
                    pixels,
                    (raster.width, raster.element_height),
                    palette,
                    folder / f"{stem}_frame{frame:03d}",
                    fmt,
                )
            rec.update(
                width=raster.width,
                height=raster.element_height,
                frames=raster.count,
                rendered=True,
            )

        elif r.resource_type == 2:
            raster, widths = ra.decode_type1(r, font=True)
            if export_indices:
                (folder / f"{stem}.indices").write_bytes(raster.pixels)
            glyph_dir = folder / f"{stem}_glyphs"
            glyph_dir.mkdir(exist_ok=True)
            for glyph in range(raster.count):
                stride = raster.width * raster.element_height
                pixels = raster.pixels[glyph * stride:(glyph + 1) * stride]
                _save_indexed(
                    pixels,
                    (raster.width, raster.element_height),
                    palette,
                    glyph_dir / f"glyph_{glyph:03d}",
                    fmt,
                )
            if export_metadata:
                (folder / f"{stem}_widths.json").write_text(
                    json.dumps(list(widths or b""), indent=2) + "\n",
                    encoding="utf-8",
                )
            rec.update(
                glyph_width=raster.width,
                height=raster.element_height,
                glyphs=raster.count,
                rendered=True,
            )

        elif r.resource_type == 6:
            body = ra.decode_shared_lzss(r)
            if len(body) % 128:
                raise ra.AssetError(f"{stem}: type-6 body not divisible by 128")
            width, height = len(body) // 128, 128
            row_major = bytearray(width * height)
            for x in range(width):
                for y, value in enumerate(body[x * 128:(x + 1) * 128]):
                    row_major[y * width + x] = value
            if export_indices:
                # Keep the exact canonical column-major decoded storage as well
                # as the viewable row-major image.
                (folder / f"{stem}.indices").write_bytes(body)
            _save_indexed(bytes(row_major), (width, height), palette, folder / stem, fmt)
            rec.update(
                width=width,
                height=height,
                canonical_storage="column-major",
                rendered=True,
            )

        else:
            body = ra.decode_shared_lzss(r)
            metadata = ra.type7_metadata(body)
            if export_metadata:
                (folder / f"{stem}.shape.bin").write_bytes(body)
                (folder / f"{stem}.shape.json").write_text(
                    json.dumps(metadata, indent=2) + "\n",
                    encoding="utf-8",
                )
            try:
                raster = ra.decode_type7_raster(body)
            except ra.AssetError as exc:
                # RES11 is a retained legacy/pre-split bank and contains a
                # small number of records whose source pixels are external to
                # the decoded shape body. Preserve their structure rather than
                # fabricating pixels. The active RES3 corpus renders fully.
                rec.update(metadata)
                rec.update(rendered=False, render_error=str(exc))
            else:
                if export_indices:
                    (folder / f"{stem}.indices").write_bytes(raster.pixels)
                if export_masks:
                    Image.frombytes(
                        "L", (raster.width, raster.height), raster.mask
                    ).save(folder / f"{stem}.mask.png", format="PNG")
                _save_masked(
                    raster.pixels,
                    raster.mask,
                    (raster.width, raster.height),
                    palette,
                    folder / stem,
                    fmt,
                )
                rec.update(metadata)
                rec.update(
                    width=raster.width,
                    height=raster.height,
                    rendered=True,
                    alpha_from_compiled_shape=True,
                )

        records.append(rec)
        if progress:
            progress(ordinal, len(resources), f"Graphics: {stem}")

    rendered = sum(1 for record in records if record.get("rendered"))
    structural_only = sum(
        1
        for record in records
        if record.get("type") == 7 and not record.get("rendered")
    )
    manifest = {
        "palette_bank": palette_index,
        "format": fmt,
        "archives": sorted(archives) if archives is not None else "all",
        "options": {
            "native_eh": preserve_native,
            "decoded_indices": export_indices,
            "metadata_and_shape_data": export_metadata,
            "transparency_masks": export_masks,
        },
        "rendered_assets": rendered,
        "structural_only_type7": structural_only,
        "active_res3_type7_note": (
            "All active RES3 compiled scene shapes are reconstructed to native "
            "1:1 raster images, including NPC/creature, object, item and effect banks."
        ),
        "assets": records,
    }
    (destination / "graphics_manifest.json").write_text(
        json.dumps(manifest, indent=2) + "\n", encoding="utf-8"
    )
    return manifest

def _ffmpeg() -> str:
    resolved = runtime_paths.bundled_path("ffmpeg.exe" if os.name == "nt" else "ffmpeg")
    if resolved is None:
        raise ra.AssetError("FFmpeg is required for WAV/FLAC conversion")
    return str(resolved)


def _convert_voc(voc: bytes, output: Path, fmt: str) -> None:
    if fmt == "VOC (original)":
        output.with_suffix(".voc").write_bytes(voc)
        return
    codec = "pcm_s16le" if fmt.startswith("WAV") else "flac"
    suffix = AUDIO_FORMATS[fmt]
    with tempfile.TemporaryDirectory() as temp:
        source = Path(temp) / "source.voc"
        source.write_bytes(voc)
        result = subprocess.run(
            [_ffmpeg(), "-hide_banner", "-loglevel", "error", "-y",
             "-i", str(source), "-map_metadata", "-1", "-c:a", codec,
             str(output.with_suffix(suffix))],
            capture_output=True, text=True,
            **runtime_paths.silent_subprocess_kwargs(),
        )
        if result.returncode:
            raise ra.AssetError(f"FFmpeg failed: {result.stderr.strip()}")


def dump_audio(game_dir: Path, destination: Path, fmt: str,
               preserve_native: bool = True,
               progress: ProgressCallback | None = None,
               archives: set[int] | None = None) -> dict:
    if fmt not in AUDIO_FORMATS:
        raise ra.AssetError(f"unsupported audio format: {fmt}")
    resources = [
        r for a in ra.discover_archives(game_dir)
        if archives is None or a.archive_number in archives
        for r in a.resources()
        if r.resource_type == 4
    ]
    destination.mkdir(parents=True, exist_ok=True)
    records = []
    for ordinal, r in enumerate(resources, 1):
        declared, voc = ra.length_prefixed_blob(r)
        stem = _stem(r)
        if preserve_native:
            (destination / f"{stem}.eh").write_bytes(r.raw)
        if preserve_native or fmt == "VOC (original)":
            (destination / f"{stem}.voc").write_bytes(voc)
        if fmt != "VOC (original)":
            _convert_voc(voc, destination / stem, fmt)
        records.append({"archive": r.archive, "entry": r.index,
                        "declared_bytes": declared, "stored_bytes": len(voc),
                        "length_matches": declared == len(voc)})
        if progress:
            progress(ordinal, len(resources), f"Audio: {stem}")
    manifest = {
        "format": fmt,
        "archives": sorted(archives) if archives is not None else "all",
        "assets": records,
    }
    (destination / "audio_manifest.json").write_text(json.dumps(manifest, indent=2) + "\n")
    return manifest


def dump_music(game_dir: Path, destination: Path,
               fmt: str = "Both XMI + MIDI",
               preserve_native: bool = True,
               progress: ProgressCallback | None = None,
               archives: set[int] | None = None) -> dict:
    if fmt not in MUSIC_FORMATS:
        raise ra.AssetError(f"unsupported music format: {fmt}")

    resources = [
        r for a in ra.discover_archives(game_dir)
        if archives is None or a.archive_number in archives
        for r in a.resources()
        if r.resource_type == 3
    ]
    destination.mkdir(parents=True, exist_ok=True)
    records = []

    export_xmi = fmt in ("XMI/XDIR (original)", "Both XMI + MIDI")
    export_midi = fmt in ("Standard MIDI (SMF0, exact 120 Hz)", "Both XMI + MIDI")

    for ordinal, r in enumerate(resources, 1):
        declared, xmi = ra.length_prefixed_blob(r)
        stem = _stem(r)

        if preserve_native:
            (destination / f"{stem}.eh").write_bytes(r.raw)
        if preserve_native or export_xmi:
            (destination / f"{stem}.xmi").write_bytes(xmi)

        midi_records = []
        if export_midi:
            midi_files, infos = xmidi.convert_all(xmi)
            for midi_bytes, info in zip(midi_files, infos):
                midi_name = f"{stem}_seq{info.sequence_index:02d}.mid"
                (destination / midi_name).write_bytes(midi_bytes)
                midi_records.append({
                    "file": midi_name,
                    "sequence": info.sequence_index,
                    "end_tick_120hz": info.end_tick,
                    "event_count": info.event_count,
                    "ail_for_loop_count": info.loop_for_count,
                    "ail_next_loop_count": info.loop_next_count,
                    "branch_count": info.branch_count,
                })

        records.append({
            "archive": r.archive,
            "entry": r.index,
            "declared_bytes": declared,
            "stored_bytes": len(xmi),
            "starts_form": xmi.startswith(b"FORM"),
            "xmi_preserved": preserve_native or export_xmi,
            "midi_sequences": midi_records,
        })
        if progress:
            progress(ordinal, len(resources), f"Music: {stem}")

    manifest = {
        "format": fmt,
        "archives": sorted(archives) if archives is not None else "all",
        "timing": (
            "SMF derivatives use SMPTE -30 fps with 4 ticks/frame, exactly "
            "120 ticks/second. AIL loop controllers are retained as CC events; "
            "original XMI is canonical for runtime control-flow semantics."
        ),
        "assets": records,
    }
    (destination / "music_manifest.json").write_text(
        json.dumps(manifest, indent=2) + "\n", encoding="utf-8"
    )
    return manifest
