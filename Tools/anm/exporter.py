#!/usr/bin/env python3
"""Export services shared by the Ravenloft ANM GUI and command-line tools."""

from __future__ import annotations

import json
import os
import shutil
import subprocess
from dataclasses import dataclass
from pathlib import Path
from typing import Callable, Iterable

from PIL import Image

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


@dataclass(frozen=True)
class RenderedFrame:
    index: int
    pixels: bytes
    palette: bytes
    chunks: tuple[anm.Chunk, ...]


def render_frames(source: anm.AnmFile) -> Iterable[RenderedFrame]:
    """Replay an ANM into complete 320x400 indexed frames."""
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

        yield RenderedFrame(
            index=frame.index,
            pixels=bytes(framebuffer),
            palette=palette,
            chunks=frame.chunks,
        )


def frame_image(frame: RenderedFrame) -> Image.Image:
    image = Image.frombytes("P", (anm.WIDTH, anm.HEIGHT), frame.pixels)
    image.putpalette(frame.palette)
    return image


def frame_rgb(frame: RenderedFrame) -> Image.Image:
    return frame_image(frame).convert("RGB")


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


def _video_command(
    ffmpeg: str,
    destination: Path,
    video_format: str,
    fps: float,
) -> list[str]:
    if fps <= 0:
        raise anm.AnmError("video frame rate must be greater than zero")
    if video_format not in VIDEO_FORMATS:
        raise anm.AnmError(f"unsupported video format: {video_format}")

    base = [
        ffmpeg,
        "-hide_banner",
        "-loglevel",
        "error",
        "-y",
        "-f",
        "rawvideo",
        "-pixel_format",
        "rgb24",
        "-video_size",
        f"{anm.WIDTH}x{anm.HEIGHT}",
        "-framerate",
        str(fps),
        "-i",
        "-",
        "-an",
    ]

    if video_format == "MP4 (H.264)":
        base += ["-c:v", "libx264", "-crf", "18", "-pix_fmt", "yuv420p"]
    elif video_format == "MKV (H.264)":
        base += ["-c:v", "libx264", "-crf", "18", "-pix_fmt", "yuv420p"]
    elif video_format == "MKV (FFV1 lossless)":
        base += ["-c:v", "ffv1", "-level", "3", "-g", "1"]
    elif video_format == "MOV (H.264)":
        base += ["-c:v", "libx264", "-crf", "18", "-pix_fmt", "yuv420p"]
    elif video_format == "AVI (MPEG-4)":
        base += ["-c:v", "mpeg4", "-q:v", "2"]
    elif video_format == "WebM (VP9)":
        base += ["-c:v", "libvpx-vp9", "-crf", "20", "-b:v", "0"]
    elif video_format == "GIF":
        base += ["-f", "gif"]

    base.append(str(destination))
    return base


def export_video(
    source: anm.AnmFile,
    destination: Path,
    video_format: str,
    fps: float,
    ffmpeg_path: str | None = None,
    progress: ProgressCallback | None = None,
) -> None:
    if ffmpeg_path:
        ffmpeg = ffmpeg_path
    else:
        resolved = runtime_paths.bundled_path("ffmpeg.exe" if os.name == "nt" else "ffmpeg")
        ffmpeg = str(resolved) if resolved else None
    if not ffmpeg:
        raise anm.AnmError(
            "FFmpeg was not found. Install FFmpeg and ensure 'ffmpeg' is on PATH."
        )

    destination.parent.mkdir(parents=True, exist_ok=True)
    command = _video_command(ffmpeg, destination, video_format, fps)
    process = subprocess.Popen(
        command,
        stdin=subprocess.PIPE,
        stdout=subprocess.DEVNULL,
        stderr=subprocess.PIPE,
    )
    assert process.stdin is not None

    try:
        for frame in render_frames(source):
            process.stdin.write(frame_rgb(frame).tobytes())
            if progress:
                progress(frame.index + 1, source.frame_count, "Encoding video")
        process.stdin.close()
        stderr = process.stderr.read().decode("utf-8", errors="replace") if process.stderr else ""
        return_code = process.wait()
    except Exception:
        process.kill()
        process.wait()
        raise

    if return_code != 0:
        raise anm.AnmError(f"FFmpeg failed with exit code {return_code}: {stderr.strip()}")


def write_timeline(source: anm.AnmFile, source_path: Path, destination: Path) -> None:
    timeline = [
        {
            "frame": frame.index,
            "chunks": [anm.chunk_record(chunk) for chunk in frame.chunks],
        }
        for frame in source.frames
    ]
    manifest = {
        "source": source_path.name,
        "width": anm.WIDTH,
        "height": anm.HEIGHT,
        "frame_count": source.frame_count,
        "default_wait": source.default_wait,
        "timeline": timeline,
    }
    destination.write_text(json.dumps(manifest, indent=2) + "\n", encoding="utf-8")
