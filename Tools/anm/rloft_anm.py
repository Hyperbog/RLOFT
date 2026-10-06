#!/usr/bin/env python3
"""Ravenloft: Strahd's Possession ANM inspector and frame extractor.

Implements the certified image/container grammar documented in
Resources/ANM/FORMAT.md. The original ANM bytes remain controlling evidence.
"""

from __future__ import annotations

import argparse
import json
import struct
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import BinaryIO

WIDTH = 320
HEIGHT = 400
FRAME_PIXELS = WIDTH * HEIGHT
RAW_PLANAR_SIZE = 0x14000
RAW_PLANAR_Y = 51
RAW_PLANAR_HEIGHT = 256


class AnmError(RuntimeError):
    pass


@dataclass(frozen=True)
class Chunk:
    chunk_id: int
    payload: bytes


@dataclass(frozen=True)
class Frame:
    index: int
    chunks: tuple[Chunk, ...]


@dataclass(frozen=True)
class AnmFile:
    frame_count: int
    default_wait: int
    frames: tuple[Frame, ...]


class Reader:
    def __init__(self, fp: BinaryIO):
        self.fp = fp

    def read_exact(self, size: int) -> bytes:
        data = self.fp.read(size)
        if len(data) != size:
            raise AnmError(f"unexpected EOF: wanted {size} bytes, got {len(data)}")
        return data

    def u16(self) -> int:
        return struct.unpack("<H", self.read_exact(2))[0]

    def s32(self) -> int:
        return struct.unpack("<i", self.read_exact(4))[0]


def parse_anm(path: Path) -> AnmFile:
    with path.open("rb") as fp:
        r = Reader(fp)
        frame_count = r.u16()
        default_wait = r.u16()
        frames: list[Frame] = []

        for frame_index in range(frame_count):
            chunk_count = r.u16()
            chunks: list[Chunk] = []
            for _ in range(chunk_count):
                chunk_id = r.u16()
                chunk_size = r.s32()
                if chunk_id > 9:
                    raise AnmError(
                        f"frame {frame_index}: unsupported chunk id {chunk_id}"
                    )
                if chunk_size < 0:
                    raise AnmError(
                        f"frame {frame_index}: negative chunk size {chunk_size}"
                    )
                chunks.append(Chunk(chunk_id, r.read_exact(chunk_size)))
            frames.append(Frame(frame_index, tuple(chunks)))

        trailing = fp.read()
        if trailing:
            raise AnmError(f"{len(trailing)} trailing bytes after declared stream")

    return AnmFile(frame_count, default_wait, tuple(frames))


def decode_type2(payload: bytes) -> bytearray:
    """Decode CODE.2 type-2 full-frame row RLE to 320x400 chunky pixels."""
    out = bytearray(FRAME_PIXELS)
    pos = 0

    for y in range(HEIGHT):
        x = 0
        row_base = y * WIDTH
        while x < WIDTH:
            if pos >= len(payload):
                raise AnmError(f"type 2: payload ended during row {y}")
            op = payload[pos]
            pos += 1

            if op < 0x80:
                count = op + 1
                if x + count > WIDTH or pos + count > len(payload):
                    raise AnmError(f"type 2: invalid literal run on row {y}")
                out[row_base + x : row_base + x + count] = payload[pos : pos + count]
                pos += count
                x += count
            else:
                count = 0x101 - op
                if x + count > WIDTH or pos >= len(payload):
                    raise AnmError(f"type 2: invalid repeated run on row {y}")
                value = payload[pos]
                pos += 1
                out[row_base + x : row_base + x + count] = bytes([value]) * count
                x += count

        if x != WIDTH:
            raise AnmError(f"type 2: row {y} decoded to {x} pixels")

    if pos != len(payload):
        raise AnmError(f"type 2: {len(payload) - pos} unconsumed payload bytes")
    return out


def _u16_from(data: bytes, pos: int) -> tuple[int, int]:
    if pos + 2 > len(data):
        raise AnmError("truncated uint16 in chunk payload")
    return struct.unpack_from("<H", data, pos)[0], pos + 2


def apply_type3(framebuffer: bytearray, payload: bytes) -> None:
    """Apply CODE.2 type-3 sparse planar delta to the chunky framebuffer."""
    pos = 0
    outer_count, pos = _u16_from(payload, pos)
    x = 0

    for _ in range(outer_count):
        delta_x, pos = _u16_from(payload, pos)
        group_count, pos = _u16_from(payload, pos)
        x += delta_x
        if x >= WIDTH:
            raise AnmError(f"type 3: x={x} outside {WIDTH}-pixel framebuffer")

        y = 0
        for _ in range(group_count):
            delta_y, pos = _u16_from(payload, pos)
            run_count, pos = _u16_from(payload, pos)
            y += delta_y
            if y + run_count > HEIGHT:
                raise AnmError(
                    f"type 3: vertical run y={y}, count={run_count} exceeds framebuffer"
                )
            if pos + run_count > len(payload):
                raise AnmError("type 3: truncated color run")

            for color in payload[pos : pos + run_count]:
                framebuffer[y * WIDTH + x] = color
                y += 1
            pos += run_count

    if pos != len(payload):
        raise AnmError(f"type 3: {len(payload) - pos} unconsumed payload bytes")


def apply_type8(framebuffer: bytearray, payload: bytes) -> None:
    """Apply CODE.2 raw four-plane 320x256 image at framebuffer y=51..306."""
    if len(payload) != RAW_PLANAR_SIZE:
        raise AnmError(
            f"type 8: expected 0x{RAW_PLANAR_SIZE:X} bytes, got 0x{len(payload):X}"
        )

    plane_size = 0x5000
    bytes_per_scanline = 80
    for plane in range(4):
        plane_data = payload[plane * plane_size : (plane + 1) * plane_size]
        for y in range(RAW_PLANAR_HEIGHT):
            src_row = y * bytes_per_scanline
            dst_y = RAW_PLANAR_Y + y
            for byte_x in range(bytes_per_scanline):
                x = byte_x * 4 + plane
                framebuffer[dst_y * WIDTH + x] = plane_data[src_row + byte_x]


def vga_palette_to_rgb(payload: bytes) -> bytes:
    if len(payload) != 768:
        raise AnmError(f"type 0: expected 768 palette bytes, got {len(payload)}")
    # VGA DAC entries are 6-bit components. Expand 0..63 to 0..255.
    return bytes((component * 255 + 31) // 63 for component in payload)


def chunk_record(chunk: Chunk) -> dict[str, object]:
    record: dict[str, object] = {
        "id": chunk.chunk_id,
        "size": len(chunk.payload),
    }
    if chunk.chunk_id in (5, 7) and len(chunk.payload) == 4:
        record["words"] = list(struct.unpack("<hh", chunk.payload))
    elif chunk.chunk_id in (6, 9) and len(chunk.payload) == 2:
        record["word"] = struct.unpack("<H", chunk.payload)[0]
    return record


def inspect(anm: AnmFile, source: Path) -> None:
    counts = {i: 0 for i in range(10)}
    print(f"file: {source}")
    print(f"frames: {anm.frame_count}")
    print(f"default_wait: {anm.default_wait}")
    print("frames:")
    for frame in anm.frames:
        desc = []
        for chunk in frame.chunks:
            counts[chunk.chunk_id] += 1
            desc.append(f"{chunk.chunk_id}:{len(chunk.payload)}")
        print(f"  {frame.index:5d}: {' '.join(desc)}")
    print("chunk_counts:")
    for chunk_id, count in counts.items():
        if count:
            print(f"  {chunk_id}: {count}")


def save_png(framebuffer: bytearray, palette: bytes, path: Path) -> None:
    try:
        from PIL import Image
    except ImportError as exc:
        raise AnmError(
            "Pillow is required for PNG extraction; install with: "
            "python -m pip install Pillow"
        ) from exc

    image = Image.frombytes("P", (WIDTH, HEIGHT), bytes(framebuffer))
    image.putpalette(palette)
    image.save(path, optimize=False)


def extract(anm: AnmFile, source: Path, output_dir: Path) -> None:
    output_dir.mkdir(parents=True, exist_ok=True)
    frames_dir = output_dir / "frames"
    frames_dir.mkdir(exist_ok=True)

    framebuffer = bytearray(FRAME_PIXELS)
    palette = bytes(768)
    timeline: list[dict[str, object]] = []

    for frame in anm.frames:
        frame_events: list[dict[str, object]] = []
        for chunk in frame.chunks:
            cid = chunk.chunk_id
            if cid == 0:
                palette = vga_palette_to_rgb(chunk.payload)
            elif cid == 1:
                framebuffer[:] = bytes(FRAME_PIXELS)
                palette = bytes(768)
            elif cid == 2:
                framebuffer = decode_type2(chunk.payload)
            elif cid == 3:
                apply_type3(framebuffer, chunk.payload)
            elif cid == 4:
                raise AnmError(
                    f"frame {frame.index}: type-4 control chunk encountered; "
                    "supplied corpus was documented as not using this type"
                )
            elif cid == 8:
                apply_type8(framebuffer, chunk.payload)

            frame_events.append(chunk_record(chunk))

        save_png(framebuffer, palette, frames_dir / f"frame_{frame.index:06d}.png")
        timeline.append({"frame": frame.index, "chunks": frame_events})

    manifest = {
        "source": source.name,
        "width": WIDTH,
        "height": HEIGHT,
        "frame_count": anm.frame_count,
        "default_wait": anm.default_wait,
        "timeline": timeline,
    }
    (output_dir / "timeline.json").write_text(
        json.dumps(manifest, indent=2) + "\n", encoding="utf-8"
    )


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description="Inspect/extract Ravenloft ANM cinematics")
    sub = parser.add_subparsers(dest="command", required=True)

    p_inspect = sub.add_parser("inspect", help="print ANM frame/chunk structure")
    p_inspect.add_argument("anm", type=Path)

    p_extract = sub.add_parser("extract", help="extract reconstructed PNG frames")
    p_extract.add_argument("anm", type=Path)
    p_extract.add_argument("output", type=Path)

    return parser


def main(argv: list[str] | None = None) -> int:
    args = build_parser().parse_args(argv)
    try:
        anm = parse_anm(args.anm)
        if args.command == "inspect":
            inspect(anm, args.anm)
        elif args.command == "extract":
            extract(anm, args.anm, args.output)
        return 0
    except (OSError, AnmError) as exc:
        print(f"error: {exc}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    raise SystemExit(main())
