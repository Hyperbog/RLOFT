#!/usr/bin/env python3
"""ANM frame/video export with reconstructed engine timing and SFX sync."""
from __future__ import annotations

import json
import os
import re
import subprocess
import tempfile
from dataclasses import dataclass
from pathlib import Path
from typing import Callable, Iterable

from PIL import Image

import res_assets
import rloft_anm as anm
import runtime_paths

ProgressCallback = Callable[[int, int, str], None]

FRAME_FORMATS = {
    "PNG": ".png",
    "BMP": ".bmp",
    "TIFF": ".tiff",
    "JPEG": ".jpg",
    "WEBP": ".webp",
}

VIDEO_FORMATS = {
    "MP4 (H.264)": ".mp4",
    "MKV (H.264)": ".mkv",
    "MKV (FFV1 lossless)": ".mkv",
    "MOV (H.264)": ".mov",
    "AVI (MPEG-4)": ".avi",
    "WebM (VP9)": ".webm",
    "GIF": ".gif",
}

# CODE.2 2E614 waits on the IRQ0 counter returned by 30700. The unmodified
# IBM-PC PIT rate is 1193182 / 65536 Hz. The cinematic frame loop uses the ANM
# header wait unless a type-3/type-8 image path is active, in which case caller
# state selects 6 ticks (ordinary CINE00..11) or 2 ticks (CINE12 path).
PIT_HZ = 1193182.0 / 65536.0
ORDINARY_IMAGE_WAIT = 6
CINE12_IMAGE_WAIT = 2
QUEUED_SFX_BASE = 49
IGNORED_QUEUED_SFX = {3, 15, 0x88}

# Exact average presentation rates for the certified CINE00..CINE12 corpus.
# Native playback is variable-rate; these are whole-cinematic averages, not a
# replacement for per-frame timing.
CERTIFIED_NATIVE_AVERAGE_FPS = {
    "CINE00.ANM": 3.623409111904,
    "CINE01.ANM": 3.295065976980,
    "CINE02.ANM": 3.710881518710,
    "CINE03.ANM": 3.710881518710,
    "CINE04.ANM": 3.710881518710,
    "CINE05.ANM": 3.710881518710,
    "CINE06.ANM": 3.578044249362,
    "CINE07.ANM": 3.337254943645,
    "CINE08.ANM": 3.357229246315,
    "CINE09.ANM": 3.444475328600,
    "CINE10.ANM": 3.246670830023,
    "CINE11.ANM": 3.243956608947,
    "CINE12.ANM": 15.061751209606,
}


@dataclass(frozen=True)
class RenderedFrame:
    index: int
    pixels: bytes
    palette: bytes
    chunks: tuple[anm.Chunk, ...]


@dataclass(frozen=True)
class SfxEvent:
    frame: int
    seconds: float
    archive: int
    resource_index: int
    command: int
    encoded_id: int


def render_frames(source: anm.AnmFile) -> Iterable[RenderedFrame]:
    framebuffer = bytearray(anm.FRAME_PIXELS)
    palette = bytes(768)
    for frame in source.frames:
        for chunk in frame.chunks:
            cid = chunk.chunk_id
            if cid == 0:
                palette = anm.vga_palette_to_rgb(chunk.payload)
            elif cid == 1:
                framebuffer[:] = bytes(anm.FRAME_PIXELS)
                palette = bytes(768)
            elif cid == 2:
                framebuffer = anm.decode_type2(chunk.payload)
            elif cid == 3:
                anm.apply_type3(framebuffer, chunk.payload)
            elif cid == 4:
                raise anm.AnmError(
                    f"frame {frame.index}: type-4 control chunk encountered; "
                    "the supplied Ravenloft corpus is documented as not using it"
                )
            elif cid == 8:
                anm.apply_type8(framebuffer, chunk.payload)
        yield RenderedFrame(frame.index, bytes(framebuffer), palette, frame.chunks)


def frame_image(frame: RenderedFrame) -> Image.Image:
    image = Image.frombytes("P", (anm.WIDTH, anm.HEIGHT), frame.pixels)
    image.putpalette(frame.palette)
    return image


def frame_rgb(frame: RenderedFrame) -> Image.Image:
    return frame_image(frame).convert("RGB")


def cinematic_number(source_name: str | Path | None) -> int | None:
    if source_name is None:
        return None
    match = re.search(r"CINE(\d{2})", Path(source_name).name.upper())
    return int(match.group(1)) if match else None


def native_wait_ticks(source: anm.AnmFile, source_name: str | Path | None) -> list[int]:
    number = cinematic_number(source_name)
    alternate = CINE12_IMAGE_WAIT if number == 12 else ORDINARY_IMAGE_WAIT
    waits: list[int] = []
    for frame in source.frames:
        image_path = any(chunk.chunk_id in (3, 8) for chunk in frame.chunks)
        waits.append(alternate if image_path else source.default_wait)
    return waits


def frame_start_times(
    source: anm.AnmFile,
    source_name: str | Path | None,
    fps: float | None,
) -> tuple[list[float], list[float]]:
    """Return frame start times and display durations in seconds.

    fps=None uses reconstructed engine/IRQ0 timing. A numeric FPS produces a
    constant-rate presentation derivative.
    """
    starts: list[float] = []
    durations: list[float] = []
    current = 0.0
    if fps is not None:
        if fps <= 0:
            raise anm.AnmError("video frame rate must be greater than zero")
        duration = 1.0 / fps
        for _ in source.frames:
            starts.append(current)
            durations.append(duration)
            current += duration
        return starts, durations

    for ticks in native_wait_ticks(source, source_name):
        starts.append(current)
        duration = ticks / PIT_HZ if ticks > 0 else 0.0
        durations.append(duration)
        current += duration
    return starts, durations


def native_timing_summary(source: anm.AnmFile, source_name: str | Path | None) -> dict[str, object]:
    waits = native_wait_ticks(source, source_name)
    positive = [w for w in waits if w > 0]
    rates = sorted({round(PIT_HZ / w, 6) for w in positive})
    total = sum(waits) / PIT_HZ
    displayed = len(positive)
    return {
        "timer_hz": PIT_HZ,
        "wait_ticks": sorted(set(waits)),
        "instantaneous_fps": rates,
        "logical_frame_count": source.frame_count,
        "displayed_frame_count": displayed,
        "average_logical_frame_rate": (source.frame_count / total) if total > 0 else None,
        "average_displayed_frame_rate": (displayed / total) if total > 0 else None,
        "duration_seconds": total,
        "zero_wait_frames": source.frame_count - displayed,
        "variable_rate": len(set(waits)) > 1,
    }


def export_frames(
    source: anm.AnmFile,
    destination: Path,
    image_format: str,
    progress: ProgressCallback | None = None,
) -> None:
    image_format = image_format.upper()
    if image_format not in FRAME_FORMATS:
        raise anm.AnmError(f"unsupported frame format: {image_format}")
    destination.mkdir(parents=True, exist_ok=True)
    suffix = FRAME_FORMATS[image_format]

    for frame in render_frames(source):
        image = frame_image(frame)
        if image_format in {"JPEG", "WEBP"}:
            image = image.convert("RGB")
        output = destination / f"frame_{frame.index:06d}{suffix}"
        save_args: dict[str, object] = {}
        if image_format == "JPEG":
            save_args.update(quality=100, subsampling=0)
        elif image_format == "WEBP":
            save_args.update(lossless=True)
        image.save(output, format=image_format, **save_args)
        if progress:
            progress(frame.index + 1, source.frame_count, f"Frames: {output.name}")


def _ffmpeg() -> str:
    resolved = runtime_paths.bundled_path("ffmpeg.exe" if os.name == "nt" else "ffmpeg")
    if resolved is None:
        raise anm.AnmError("FFmpeg was not found")
    return str(resolved)


def _video_codec_args(video_format: str) -> list[str]:
    if video_format == "MP4 (H.264)":
        return ["-c:v", "libx264", "-crf", "18", "-pix_fmt", "yuv420p"]
    if video_format == "MKV (H.264)":
        return ["-c:v", "libx264", "-crf", "18", "-pix_fmt", "yuv420p"]
    if video_format == "MKV (FFV1 lossless)":
        return ["-c:v", "ffv1", "-level", "3", "-g", "1"]
    if video_format == "MOV (H.264)":
        return ["-c:v", "libx264", "-crf", "18", "-pix_fmt", "yuv420p"]
    if video_format == "AVI (MPEG-4)":
        return ["-c:v", "mpeg4", "-q:v", "2"]
    if video_format == "WebM (VP9)":
        return ["-c:v", "libvpx-vp9", "-crf", "20", "-b:v", "0"]
    if video_format == "GIF":
        return ["-f", "gif"]
    raise anm.AnmError(f"unsupported video format: {video_format}")


def _audio_codec_args(video_format: str) -> list[str]:
    if video_format.startswith("MKV"):
        return ["-c:a", "flac"]
    if video_format in ("MP4 (H.264)", "MOV (H.264)"):
        return ["-c:a", "aac", "-b:a", "256k"]
    if video_format == "AVI (MPEG-4)":
        return ["-c:a", "pcm_s16le"]
    if video_format == "WebM (VP9)":
        return ["-c:a", "libopus", "-b:a", "192k"]
    return []


def cinematic_sfx_events(
    source: anm.AnmFile,
    source_name: str | Path | None,
    fps: float | None,
) -> list[SfxEvent]:
    starts, _durations = frame_start_times(source, source_name, fps)
    result: list[SfxEvent] = []
    for frame, seconds in zip(source.frames, starts):
        for chunk in frame.chunks:
            if chunk.chunk_id not in (6, 9) or len(chunk.payload) != 2:
                continue
            encoded = int.from_bytes(chunk.payload, "little")
            if chunk.chunk_id == 6:
                if encoded in IGNORED_QUEUED_SFX:
                    continue
                archive = 6
                resource_id = QUEUED_SFX_BASE + encoded
            else:
                # CODE.2 146B8 explicitly selects archive 10 before 27F24(id),
                # then restores archive 6 after direct cinematic playback.
                archive = 10
                resource_id = encoded
            result.append(
                SfxEvent(
                    frame.index,
                    seconds,
                    archive,
                    resource_id,
                    chunk.chunk_id,
                    encoded,
                )
            )
    return result


def _write_sfx_inputs(
    game_dir: Path,
    temp_dir: Path,
    events: list[SfxEvent],
) -> list[tuple[Path, SfxEvent]]:
    resource_maps: dict[int, dict[int, res_assets.EhResource]] = {}
    result: list[tuple[Path, SfxEvent]] = []
    for ordinal, event in enumerate(events):
        if event.archive not in resource_maps:
            archive = res_assets.ResArchive(
                Path(game_dir) / f"RES{event.archive}",
                event.archive,
            )
            resource_maps[event.archive] = {
                r.index: r
                for r in archive.resources()
                if r.resource_type == 4
            }
        resource = resource_maps[event.archive].get(event.resource_index)
        if resource is None:
            continue
        _declared, voc = res_assets.length_prefixed_blob(resource)
        path = temp_dir / (
            f"sfx_{ordinal:03d}_res{event.archive}_{event.resource_index:03d}.voc"
        )
        path.write_bytes(voc)
        result.append((path, event))
    return result


def _concat_quote(path: Path) -> str:
    return str(path.resolve()).replace("'", "'\\''").replace("\\", "/")


def export_video(
    source: anm.AnmFile,
    destination: Path,
    video_format: str,
    fps: float | None,
    ffmpeg_path: str | None = None,
    progress: ProgressCallback | None = None,
    source_path: Path | None = None,
    game_dir: Path | None = None,
    sync_audio: bool = True,
) -> dict[str, object]:
    if video_format not in VIDEO_FORMATS:
        raise anm.AnmError(f"unsupported video format: {video_format}")
    ffmpeg = ffmpeg_path or _ffmpeg()
    destination.parent.mkdir(parents=True, exist_ok=True)
    source_name = source_path.name if source_path else None
    starts, durations = frame_start_times(source, source_name, fps)

    with tempfile.TemporaryDirectory(prefix="rloft_anm_") as temp_name:
        temp = Path(temp_name)
        concat_lines = ["ffconcat version 1.0"]
        written = 0
        last_path: Path | None = None

        for rendered, duration in zip(render_frames(source), durations):
            # Zero-wait command frames are real ANM states but are not displayed
            # for a measurable interval. Their state naturally carries into the
            # next rendered frame.
            if duration <= 0 and rendered.index + 1 < source.frame_count:
                if progress:
                    progress(rendered.index + 1, source.frame_count, "Preparing native-timing video")
                continue
            image_path = temp / f"frame_{rendered.index:06d}.png"
            frame_rgb(rendered).save(image_path, format="PNG")
            if duration <= 0:
                duration = 1.0 / PIT_HZ
            concat_lines.append(f"file '{_concat_quote(image_path)}'")
            concat_lines.append(f"duration {duration:.12f}")
            last_path = image_path
            written += 1
            if progress:
                progress(rendered.index + 1, source.frame_count, "Preparing video")

        if last_path is None:
            raise anm.AnmError("cinematic produced no renderable frames")
        # concat demuxer needs the last file repeated to honor its duration.
        concat_lines.append(f"file '{_concat_quote(last_path)}'")
        concat_path = temp / "frames.ffconcat"
        concat_path.write_text("\n".join(concat_lines) + "\n", encoding="utf-8")

        command = [
            ffmpeg, "-hide_banner", "-loglevel", "error", "-y",
            "-f", "concat", "-safe", "0", "-i", str(concat_path),
        ]

        sfx_inputs: list[tuple[Path, SfxEvent]] = []
        if sync_audio and game_dir is not None and video_format != "GIF":
            sfx_inputs = _write_sfx_inputs(
                Path(game_dir),
                temp,
                cinematic_sfx_events(source, source_name, fps),
            )
            for path, _event in sfx_inputs:
                command += ["-i", str(path)]

        command += ["-map", "0:v:0", "-fps_mode", "vfr"]
        if sfx_inputs:
            filters = []
            labels = []
            for input_index, (_path, event) in enumerate(sfx_inputs, start=1):
                delay_ms = max(0, round(event.seconds * 1000.0))
                label = f"a{input_index}"
                filters.append(f"[{input_index}:a]adelay={delay_ms}:all=1[{label}]")
                labels.append(f"[{label}]")
            if len(labels) == 1:
                filters.append(f"{labels[0]}anull,apad[aout]")
            else:
                filters.append(
                    "".join(labels)
                    + f"amix=inputs={len(labels)}:normalize=0,apad[aout]"
                )
            command += ["-filter_complex", ";".join(filters), "-map", "[aout]"]
            command += _audio_codec_args(video_format)
            command += ["-shortest"]
        else:
            command += ["-an"]

        command += _video_codec_args(video_format)
        command += [str(destination)]

        result = subprocess.run(
            command,
            stdout=subprocess.DEVNULL,
            stderr=subprocess.PIPE,
            text=True,
            **runtime_paths.silent_subprocess_kwargs(),
        )
        if result.returncode != 0:
            raise anm.AnmError(
                f"FFmpeg failed with exit code {result.returncode}: {result.stderr.strip()}"
            )

    timing = native_timing_summary(source, source_name) if fps is None else {
        "constant_fps": fps,
        "duration_seconds": source.frame_count / fps,
    }
    return {
        "timing": timing,
        "frames_written": written,
        "sfx_events_muxed": len(sfx_inputs),
        "sampled_audio_sync": bool(sfx_inputs),
        "music_note": (
            "ANM music commands are preserved in the timeline/XMI-MIDI exports; "
            "the video mux does not synthesize MIDI through an arbitrary soundfont."
        ),
    }


def write_timeline(source: anm.AnmFile, source_path: Path, destination: Path) -> None:
    waits = native_wait_ticks(source, source_path.name)
    starts, durations = frame_start_times(source, source_path.name, None)
    timeline = []
    for frame, ticks, start, duration in zip(source.frames, waits, starts, durations):
        timeline.append({
            "frame": frame.index,
            "start_seconds": start,
            "duration_seconds": duration,
            "wait_ticks": ticks,
            "chunks": [anm.chunk_record(chunk) for chunk in frame.chunks],
        })

    manifest = {
        "source": source_path.name,
        "width": anm.WIDTH,
        "height": anm.HEIGHT,
        "frame_count": source.frame_count,
        "default_wait": source.default_wait,
        "native_timer_hz": PIT_HZ,
        "native_timing": native_timing_summary(source, source_path.name),
        "sampled_sfx_events": [
            {
                "frame": event.frame,
                "seconds": event.seconds,
                "archive": event.archive,
                "resource_index": event.resource_index,
                "command": event.command,
                "encoded_id": event.encoded_id,
            }
            for event in cinematic_sfx_events(source, source_path.name, None)
        ],
        "timeline": timeline,
    }
    destination.parent.mkdir(parents=True, exist_ok=True)
    destination.write_text(json.dumps(manifest, indent=2) + "\n", encoding="utf-8")
