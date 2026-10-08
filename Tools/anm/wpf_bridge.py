#!/usr/bin/env python3
"""Machine-readable, GUI-independent bridge for the native WPF/Fluent extractor.

Protocol: one JSON request on stdin, JSON-lines events on stdout.
Never serialize a traceback into the stdout protocol. Errors are reported as
{type:"error", message:...}, with a nonzero process exit code.
"""
from __future__ import annotations

import json
import sys
import time
import traceback
from pathlib import Path

import asset_dumper
import game_source
import master_dumper
import media_inventory
import res_assets
import support_dumper

GRAPHICS_DEFAULT = (0, 1, 2, 3, 4, 7, 8, 9, 11, 12)
AUDIO_DEFAULT = (6, 10, 11, 12)
MUSIC_DEFAULT = (5, 11)


def emit(kind: str, **fields: object) -> None:
    print(json.dumps({"type": kind, **fields}, ensure_ascii=True, default=str), flush=True)


_last_progress: dict[str, tuple[float, int]] = {}


def progress(stage: str, done: int, total: int, message: str) -> None:
    """Keep the native Windows message pump responsive during large RES exports.

    The raw decoder can report thousands of frames per second. Throttle only
    transient progress updates, never results or errors. Start/end events and
    percent changes are always emitted.
    """
    now = time.monotonic()
    percent = int(100 * done / total) if total > 0 else -1
    previous_time, previous_percent = _last_progress.get(stage, (0.0, -2))
    if (0 < done < total and percent == previous_percent
            and now - previous_time < 0.15):
        return
    _last_progress[stage] = (now, percent)
    emit("progress", stage=stage, done=done, total=total, message=message)


def _section(stage: str):
    def handler(done: int, total: int, message: str) -> None:
        progress(stage, done, total, message)
    return handler


def _flag(request: dict, key: str, default: bool = True) -> bool:
    return bool(request.get(key, default))


def _archives(request: dict, key: str, default: tuple[int, ...]) -> set[int]:
    return {int(v) for v in request.get(key, default)}


def graphics(source: Path, destination: Path, request: dict) -> dict:
    families = set(request.get("group_families", asset_dumper.GRAPHICS_GROUP_FAMILIES))
    return asset_dumper.dump_graphics(
        source,
        destination,
        str(request.get("graphics_format", "PNG (lossless indexed)")),
        palette_index=request.get("palette_index"),
        preserve_native=_flag(request, "graphics_native", False),
        progress=_section("graphics"),
        archives=_archives(request, "graphics_archives", GRAPHICS_DEFAULT),
        export_indices=_flag(request, "graphics_indices"),
        export_metadata=_flag(request, "graphics_metadata"),
        export_masks=_flag(request, "graphics_masks", False),
        organization=str(request.get(
            "graphics_organization", asset_dumper.GRAPHICS_ORGANIZATION_BOTH
        )),
        group_families=families,
        group_contact_sheets=_flag(request, "group_contact_sheets"),
        group_animated_previews=_flag(request, "group_previews"),
        group_alias_maps=_flag(request, "group_alias_maps"),
    )


def audio(source: Path, destination: Path, request: dict) -> dict:
    result: dict = {}
    if _flag(request, "sampled_audio"):
        result["sampled"] = asset_dumper.dump_audio(
            source, destination / "sampled_audio",
            str(request.get("audio_format", "FLAC (lossless)")),
            preserve_native=_flag(request, "preserve_native"),
            progress=_section("audio"),
            archives=_archives(request, "audio_archives", AUDIO_DEFAULT),
        )
    if _flag(request, "music"):
        result["music"] = asset_dumper.dump_music(
            source, destination / "music",
            fmt=str(request.get("music_format", "Both XMI + MIDI")),
            preserve_native=_flag(request, "preserve_native"),
            progress=_section("music"),
            archives=_archives(request, "music_archives", MUSIC_DEFAULT),
        )
    return result


def cinematics(source: Path, destination: Path, request: dict) -> dict:
    fps = request.get("cinematic_fps")
    options = master_dumper.DumpOptions(
        cinematic_frame_format=str(request.get("cinematic_frame_format", "PNG")),
        cinematic_video_format=str(
            request.get("cinematic_video_format", "MKV (FFV1 lossless)")
        ),
        cinematic_fps=None if fps is None else float(fps),
        cinematic_audio=_flag(request, "cinematic_sync_audio"),
        cinematic_frames=_flag(request, "cinematic_frames"),
        cinematic_videos=_flag(request, "cinematic_videos", False),
        cinematic_timeline=_flag(request, "cinematic_timeline"),
        preserve_native=_flag(request, "preserve_native"),
    )
    names = request.get("cinematic_names")
    return master_dumper.dump_cinematics(
        source,
        destination,
        options,
        progress,
        names=tuple(names) if names else media_inventory.ANM_NAMES,
    )


def support(source: Path, destination: Path, request: dict) -> dict:
    """Export chosen support records without requiring a certified full CD."""
    present = {p.name.upper() for p in source.iterdir() if p.is_file()}
    catalog = [
        ("COLORS", "support_colors", "palettes", support_dumper.dump_colors),
        ("T", "support_text", "text", support_dumper.dump_text),
        ("SAMPLE.AD", "support_sample", "opl2", support_dumper.dump_sample_ad),
        ("INIT", "support_init", "init", support_dumper.dump_init),
        ("SINTAB", "support_sintab", "renderer_support", support_dumper.dump_sintab),
    ]
    records: dict = {}
    for filename, key, folder, exporter in catalog:
        if filename not in present or not _flag(request, key):
            continue
        progress("support", 0, 1, f"Exporting {filename}")
        records[folder] = exporter(source, destination / folder)
        progress("support", 1, 1, f"Completed {filename}")
    if _flag(request, "support_banks") and all(
        name in present for name in ("GM1.BNK", "GM2.BNK")
    ):
        records["aria"] = support_dumper.dump_banks(source, destination / "aria")
    return records


def _run_all(source: Path, destination: Path, request: dict) -> dict:
    """Run independent stages without the older strict hash-match gate."""
    destination.mkdir(parents=True, exist_ok=True)
    result: dict = {}
    present = {path.name.upper() for path in source.iterdir() if path.is_file()}
    if "COLORS" in present:
        selected = set(GRAPHICS_DEFAULT).intersection(
            int(name[3:]) for name in present
            if name.startswith("RES") and name[3:].isdigit()
        )
        if selected:
            local = dict(request, graphics_archives=sorted(selected))
            result["graphics"] = graphics(source, destination / "graphics", local)
    sound = set(AUDIO_DEFAULT).intersection(
        int(name[3:]) for name in present
        if name.startswith("RES") and name[3:].isdigit()
    )
    music_archives = set(MUSIC_DEFAULT).intersection(
        int(name[3:]) for name in present
        if name.startswith("RES") and name[3:].isdigit()
    )
    local_audio = dict(
        request, sampled_audio=bool(sound), music=bool(music_archives),
        audio_archives=sorted(sound), music_archives=sorted(music_archives)
    )
    if sound or music_archives:
        result["audio_music"] = audio(source, destination / "audio_music", local_audio)
    names = [name for name in media_inventory.ANM_NAMES if name in present]
    if names:
        result["cinematics"] = cinematics(
            source, destination / "cinematics",
            dict(request, cinematic_names=names)
        )
    if _flag(request, "export_support"):
        result["support"] = support(source, destination / "support", request)
    (destination / "modern_master_manifest.json").write_text(
        json.dumps({"sections": result, "source": str(source)}, indent=2, default=str)
        + "\n",
        encoding="utf-8",
    )
    return result


def execute(request: dict) -> dict:
    action = str(request.get("action", "")).lower()
    selected = str(request.get("source", "")).strip()
    if not selected:
        raise ValueError("Select an installed game folder or CD image first.")
    source_path = Path(selected).expanduser()
    if action == "validate":
        emit("progress", stage="scan", done=0, total=1,
             message="Scanning installed/loose/CD game source")
        validation = game_source.validate_source(source_path, verify_hashes=True)
        return validation

    source = game_source.prepare_source_dir(source_path)
    output_str = str(request.get("destination", "")).strip()
    if not output_str:
        raise ValueError("Select an output directory.")
    destination = Path(output_str).expanduser()
    destination.mkdir(parents=True, exist_ok=True)
    if action == "graphics":
        return graphics(source, destination, request)
    if action == "audio":
        return audio(source, destination, request)
    if action == "cinematics":
        return cinematics(source, destination, request)
    if action == "support":
        return support(source, destination, request)
    if action == "all":
        return _run_all(source, destination, request)
    raise ValueError(f"Unsupported WPF operation: {action!r}")


def main() -> int:
    if "--self-test" in sys.argv:
        emit("done", result={"backend": "ready", "protocol": 1})
        return 0
    try:
        request = json.loads(sys.stdin.read())
        if not isinstance(request, dict):
            raise ValueError("The request must be a JSON object.")
        result = execute(request)
        # Full manifests can contain tens of thousands of frame records; the
        # engine writes them to disk. Only validation needs a detailed result
        # in the responsive desktop UI.
        if str(request.get("action")) == "validate":
            summary = result
        else:
            summary = {
                "output": str(request.get("destination", "")),
                "sections": sorted(result) if isinstance(result, dict) else [],
            }
        emit("done", result=summary)
        return 0
    except Exception as exc:
        emit("error", message=str(exc))
        traceback.print_exc(file=sys.stderr)
        return 1


if __name__ == "__main__":
    raise SystemExit(main())
