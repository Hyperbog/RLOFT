#!/usr/bin/env python3
"""Audit OBJECTS.DAT renderer auxiliary bytes +0x2B/+0x2C."""
from __future__ import annotations

import argparse
import collections
import struct
from pathlib import Path

RECORD_SIZE = 0x41


def records(path: Path) -> list[bytes]:
    data = path.read_bytes()
    if len(data) < 4:
        raise ValueError("OBJECTS.DAT too short")
    count = struct.unpack_from("<I", data, 0)[0]
    expected = 4 + count * RECORD_SIZE
    if len(data) != expected:
        raise ValueError(f"size {len(data)} != expected {expected}")
    return [
        data[4 + i * RECORD_SIZE : 4 + (i + 1) * RECORD_SIZE]
        for i in range(count)
    ]


def name(record: bytes) -> str:
    raw = record[:40]
    end = raw.find(b"\0")
    if end < 0:
        raise ValueError("record name lacks NUL")
    return raw[:end].decode("ascii", errors="strict")


def analyze(path: Path) -> None:
    recs = records(path)
    step = collections.Counter(r[0x2B] for r in recs)
    byte_2c = collections.Counter(r[0x2C] for r in recs)

    print(f"records={len(recs)}")
    print("scene_influence_step=" + " ".join(
        f"{value:#04x}:{count}" for value, count in sorted(step.items())
    ))
    print("byte_2c=" + " ".join(
        f"{value:#04x}:{count}" for value, count in sorted(byte_2c.items())
    ))

    nonzero_step = [(i, name(r), r[0x2B]) for i, r in enumerate(recs) if r[0x2B]]
    nonzero_2c = [(i, name(r), r[0x2C]) for i, r in enumerate(recs) if r[0x2C]]

    print(f"nonzero_scene_influence_step={len(nonzero_step)}")
    for i, n, value in nonzero_step:
        print(f"  id={i:3d} step={value:3d} name={n}")

    print(f"nonzero_byte_2c={len(nonzero_2c)}")
    for i, n, value in nonzero_2c:
        print(f"  id={i:3d} value={value:#04x} name={n}")

    if nonzero_step:
        print("note=nonzero +0x2B values require executable-path review")
    else:
        print("verified=all supplied +0x2B bytes are zero")

    if nonzero_2c:
        print("note=nonzero +0x2C values require preservation and review")
    else:
        print("verified=all supplied +0x2C bytes are zero")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("objects", type=Path)
    args = parser.parse_args()
    analyze(args.objects)


if __name__ == "__main__":
    main()
