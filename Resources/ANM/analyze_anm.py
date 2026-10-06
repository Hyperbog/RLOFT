#!/usr/bin/env python3
"""Inspect Ravenloft: Strahd's Possession ANM cinematic files.

The outer grammar and image-packet validation implemented here are derived from
Ravenloft CODE.2, not from a modern export format. This tool does not modify
source files and does not claim image export or ANM regeneration support.
"""
from __future__ import annotations

import argparse
import collections
import hashlib
import json
import struct
from dataclasses import dataclass
from pathlib import Path

FRAME_WIDTH = 320
FRAME_HEIGHT = 400
TYPE8_SIZE = 4 * 0x5000


@dataclass(frozen=True)
class Chunk:
    chunk_id: int
    payload: bytes


@dataclass(frozen=True)
class Frame:
    chunks: tuple[Chunk, ...]


@dataclass(frozen=True)
class Animation:
    frame_count: int
    default_wait: int
    frames: tuple[Frame, ...]


def u16(data: bytes, off: int) -> int:
    return struct.unpack_from("<H", data, off)[0]


def i32(data: bytes, off: int) -> int:
    return struct.unpack_from("<i", data, off)[0]


def parse_anm(data: bytes) -> Animation:
    if len(data) < 4:
        raise ValueError("ANM is shorter than its 4-byte header")
    frame_count, default_wait = struct.unpack_from("<HH", data, 0)
    pos = 4
    frames: list[Frame] = []

    for frame_index in range(frame_count):
        if pos + 2 > len(data):
            raise ValueError(f"frame {frame_index}: missing chunk count")
        chunk_count = u16(data, pos)
        pos += 2
        chunks: list[Chunk] = []
        for chunk_index in range(chunk_count):
            if pos + 6 > len(data):
                raise ValueError(
                    f"frame {frame_index} chunk {chunk_index}: truncated header"
                )
            chunk_id = u16(data, pos)
            chunk_size = i32(data, pos + 2)
            pos += 6
            if chunk_size < 0:
                raise ValueError(
                    f"frame {frame_index} chunk {chunk_index}: negative size"
                )
            end = pos + chunk_size
            if end > len(data):
                raise ValueError(
                    f"frame {frame_index} chunk {chunk_index}: payload exceeds EOF"
                )
            chunks.append(Chunk(chunk_id, data[pos:end]))
            pos = end
        frames.append(Frame(tuple(chunks)))

    if pos != len(data):
        raise ValueError(f"{len(data) - pos} trailing bytes after final frame")
    return Animation(frame_count, default_wait, tuple(frames))


def serialize_anm(anim: Animation) -> bytes:
    out = bytearray(struct.pack("<HH", anim.frame_count, anim.default_wait))
    if anim.frame_count != len(anim.frames):
        raise ValueError("frame_count does not match frame list")
    for frame in anim.frames:
        out += struct.pack("<H", len(frame.chunks))
        for chunk in frame.chunks:
            out += struct.pack("<Hi", chunk.chunk_id, len(chunk.payload))
            out += chunk.payload
    return bytes(out)


def validate_type2(payload: bytes) -> dict[str, int]:
    """Validate CODE.2 type-2 row RLE without changing pixel geometry."""
    pos = 0
    decoded = 0
    for row in range(FRAME_HEIGHT):
        row_bytes = 0
        while row_bytes < FRAME_WIDTH:
            if pos >= len(payload):
                raise ValueError(f"type 2: EOF in row {row}")
            op = payload[pos]
            pos += 1
            if op < 0x80:
                count = op + 1
                if pos + count > len(payload):
                    raise ValueError(f"type 2: literal exceeds payload in row {row}")
                pos += count
            else:
                count = 0x101 - op
                if pos >= len(payload):
                    raise ValueError(f"type 2: repeat value missing in row {row}")
                pos += 1
            row_bytes += count
            if row_bytes > FRAME_WIDTH:
                raise ValueError(f"type 2: row {row} expands past 320 pixels")
        decoded += row_bytes
    if pos != len(payload):
        raise ValueError(f"type 2: {len(payload) - pos} unconsumed bytes")
    return {"consumed": pos, "decoded_pixels": decoded}


def validate_type3(payload: bytes) -> dict[str, int]:
    """Validate CODE.2 type-3 sparse vertical delta commands."""
    if len(payload) < 2:
        raise ValueError("type 3: missing outer-count word")
    pos = 0
    outer_count = u16(payload, pos)
    pos += 2
    x = 0
    max_x = 0
    max_y = 0

    for outer in range(outer_count):
        if pos + 4 > len(payload):
            raise ValueError(f"type 3: truncated outer record {outer}")
        delta_x, group_count = struct.unpack_from("<HH", payload, pos)
        pos += 4
        x += delta_x
        if x >= FRAME_WIDTH:
            raise ValueError(f"type 3: x={x} exceeds 319")
        max_x = max(max_x, x)
        y = 0
        for group in range(group_count):
            if pos + 4 > len(payload):
                raise ValueError(
                    f"type 3: truncated group {group} in outer record {outer}"
                )
            delta_y, count = struct.unpack_from("<HH", payload, pos)
            pos += 4
            y += delta_y
            if y + count > FRAME_HEIGHT:
                raise ValueError(f"type 3: y run exceeds 400 at x={x}")
            if pos + count > len(payload):
                raise ValueError(f"type 3: pixel run exceeds payload at x={x}")
            pos += count
            y += count
            max_y = max(max_y, y)

    if pos != len(payload):
        raise ValueError(f"type 3: {len(payload) - pos} unconsumed bytes")
    return {
        "consumed": pos,
        "outer_count": outer_count,
        "max_x": max_x,
        "max_y_exclusive": max_y,
    }


def inspect(path: Path) -> dict[str, object]:
    source = path.read_bytes()
    anim = parse_anm(source)
    rebuilt = serialize_anm(anim)
    if rebuilt != source:
        raise ValueError("parse/serialize round trip changed bytes")

    counts: collections.Counter[int] = collections.Counter()
    sizes: dict[int, collections.Counter[int]] = collections.defaultdict(collections.Counter)
    type2_count = 0
    type3_count = 0
    for frame in anim.frames:
        for chunk in frame.chunks:
            counts[chunk.chunk_id] += 1
            sizes[chunk.chunk_id][len(chunk.payload)] += 1
            if chunk.chunk_id == 2:
                validate_type2(chunk.payload)
                type2_count += 1
            elif chunk.chunk_id == 3:
                validate_type3(chunk.payload)
                type3_count += 1
            elif chunk.chunk_id == 8 and len(chunk.payload) != TYPE8_SIZE:
                raise ValueError(
                    f"type 8 payload has {len(chunk.payload)} bytes, expected {TYPE8_SIZE}"
                )

    return {
        "file": path.name,
        "bytes": len(source),
        "sha256": hashlib.sha256(source).hexdigest(),
        "frame_count": anim.frame_count,
        "default_wait": anim.default_wait,
        "chunk_counts": {str(k): counts[k] for k in sorted(counts)},
        "chunk_sizes": {
            str(k): {str(sz): n for sz, n in sorted(sizes[k].items())}
            for k in sorted(sizes)
        },
        "validated_type2_chunks": type2_count,
        "validated_type3_chunks": type3_count,
        "byte_exact_round_trip": True,
    }


def main() -> None:
    ap = argparse.ArgumentParser()
    ap.add_argument("files", nargs="+", type=Path)
    ap.add_argument("--json", action="store_true", help="emit JSON instead of text")
    args = ap.parse_args()

    results = [inspect(path) for path in args.files]
    if args.json:
        print(json.dumps(results, indent=2, sort_keys=True))
        return
    for item in results:
        counts = ", ".join(f"{k}:{v}" for k, v in item["chunk_counts"].items())
        print(
            f'{item["file"]}: frames={item["frame_count"]} '
            f'default_wait={item["default_wait"]} chunks=[{counts}] '
            f'roundtrip=OK'
        )


if __name__ == "__main__":
    main()
