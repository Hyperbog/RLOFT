#!/usr/bin/env python3
"""Master Ravenloft media-dump orchestration."""
from __future__ import annotations

import json
import shutil
from dataclasses import dataclass
from pathlib import Path
from typing import Callable

import asset_dumper
import exporter
import media_inventory as inventory
import rloft_anm as anm
import support_dumper

ProgressCallback = Callable[[str, int, int, str], None]


@dataclass(frozen=True)
class DumpOptions:
    graphics_format: str = "PNG (lossless indexed)"
    graphics_palette: int = 0
    audio_format: str = "FLAC (lossless)"
    cinematic_frame_format: str = "PNG"
    cinematic_video_format: str = "MKV (FFV1 lossless)"
    cinematic_fps: float = 15.0
    cinematic_frames: bool = True
    cinematic_videos: bool = False
    preserve_native: bool = True
    export_support: bool = True


def _stage_progress(callback: ProgressCallback | None, stage: str):
    if callback is None:
        return None

    def inner(done: int, total: int, message: str) -> None:
        callback(stage, done, total, message)

    return inner


def dump_cinematics(
    source_dir: Path,
    destination: Path,
    options: DumpOptions,
    progress: ProgressCallback | None = None,
    frames_destination: Path | None = None,
    video_destination: Path | None = None,
    timeline_destination: Path | None = None,
) -> dict[str, object]:
    source_dir = Path(source_dir)
    destination = Path(destination)
    frames_root = Path(frames_destination) if frames_destination else destination / "frames"
    video_root = Path(video_destination) if video_destination else destination / "video"
    timeline_root = Path(timeline_destination) if timeline_destination else destination / "timelines"
    originals_root = destination / "original_anm"
    timeline_root.mkdir(parents=True, exist_ok=True)

    records = []
    for ordinal, name in enumerate(inventory.ANM_NAMES, 1):
        path = source_dir / name
        if not path.is_file():
            continue
        movie = anm.parse_anm(path)
        stem = path.stem

        if options.preserve_native:
            originals_root.mkdir(parents=True, exist_ok=True)
            shutil.copyfile(path, originals_root / name)

        if options.cinematic_frames:
            exporter.export_frames(
                movie,
                frames_root / stem,
                options.cinematic_frame_format,
                _stage_progress(progress, f"cinematic:{stem}:frames"),
            )

        video_file = None
        if options.cinematic_videos:
            suffix = exporter.VIDEO_FORMATS[options.cinematic_video_format]
            video_root.mkdir(parents=True, exist_ok=True)
            video_file = video_root / f"{stem}{suffix}"
            exporter.export_video(
                movie,
                video_file,
                options.cinematic_video_format,
                options.cinematic_fps,
                progress=_stage_progress(progress, f"cinematic:{stem}:video"),
            )

        exporter.write_timeline(movie, path, timeline_root / f"{stem}.json")
        records.append({
            "source": name,
            "frames": movie.frame_count,
            "default_wait": movie.default_wait,
            "frames_exported": options.cinematic_frames,
            "video": str(video_file) if video_file else None,
        })
        if progress:
            progress("cinematics", ordinal, len(inventory.ANM_NAMES), f"Completed {name}")

    manifest = {
        "frame_format": options.cinematic_frame_format,
        "video_format": options.cinematic_video_format if options.cinematic_videos else None,
        "video_fps": options.cinematic_fps if options.cinematic_videos else None,
        "cinematics": records,
        "timing_note": (
            "ANM default_wait is preserved in each timeline. Constant-FPS video is a presentation "
            "export; exact original timing remains represented by ANM/timeline data."
        ),
    }
    destination.mkdir(parents=True, exist_ok=True)
    (destination / "cinematics_manifest.json").write_text(
        json.dumps(manifest, indent=2) + "\n", encoding="utf-8"
    )
    return manifest


def dump_all(
    source_dir: Path,
    destination: Path,
    options: DumpOptions = DumpOptions(),
    progress: ProgressCallback | None = None,
    verify_hashes: bool = True,
) -> dict[str, object]:
    source_dir = Path(source_dir)
    destination = Path(destination)
    validation = inventory.validate_source_dir(source_dir, verify_hashes=verify_hashes)
    if validation["missing"] or validation["mismatched"]:
        raise RuntimeError(
            "Source directory is not the certified complete 33-file corpus; "
            "run validation for missing/mismatched details."
        )

    destination.mkdir(parents=True, exist_ok=True)
    (destination / "source_validation.json").write_text(
        json.dumps(validation, indent=2) + "\n", encoding="utf-8"
    )

    result: dict[str, object] = {}
    result["graphics"] = asset_dumper.dump_graphics(
        source_dir,
        destination / "graphics",
        options.graphics_format,
        palette_index=options.graphics_palette,
        preserve_native=options.preserve_native,
        progress=_stage_progress(progress, "graphics"),
    )
    result["audio"] = asset_dumper.dump_audio(
        source_dir,
        destination / "audio",
        options.audio_format,
        preserve_native=options.preserve_native,
        progress=_stage_progress(progress, "audio"),
    )
    result["music"] = asset_dumper.dump_music(
        source_dir,
        destination / "music",
        preserve_native=options.preserve_native,
        progress=_stage_progress(progress, "music"),
    )
    result["cinematics"] = dump_cinematics(
        source_dir,
        destination / "cinematics",
        options,
        progress,
    )
    if options.export_support:
        if progress:
            progress("support", 0, 1, "Extracting palette/text/timbre/renderer support")
        support_dumper.dump_support_files(source_dir, destination / "support")
        result["support"] = {
            "palettes": "support/palettes",
            "subtitles_and_text": "support/text",
            "opl2_timbres": "support/opl2",
            "aria_banks": "support/aria",
            "resource_mappings": "support/init",
            "renderer_support": "support/renderer_support",
        }
        if progress:
            progress("support", 1, 1, "Support extraction complete")

    summary = {
        "source_certified": validation["certified_complete"],
        "lossless_defaults": {
            "graphics": options.graphics_format,
            "sampled_audio": options.audio_format,
            "music": "XMI/XDIR original",
            "cinematic_frames": options.cinematic_frame_format,
            "cinematic_video": options.cinematic_video_format if options.cinematic_videos else "not requested",
        },
        "sections": result,
    }
    (destination / "master_manifest.json").write_text(
        json.dumps(summary, indent=2) + "\n", encoding="utf-8"
    )
    return summary
