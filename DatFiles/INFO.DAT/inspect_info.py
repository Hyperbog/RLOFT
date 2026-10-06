#!/usr/bin/env python3
"""Inspect and losslessly re-encode Ravenloft INFO.DAT."""
from __future__ import annotations

import argparse
import struct
from dataclasses import dataclass
from pathlib import Path

CONFIG_COUNT = 13
FIXED_PREFIX = CONFIG_COUNT + 2


@dataclass(frozen=True)
class InfoDat:
    config_ascii: bytes
    selectors: tuple[int, ...]
    path: bytes


def parse(data: bytes) -> InfoDat:
    if len(data) < FIXED_PREFIX:
        raise ValueError(f"INFO.DAT shorter than {FIXED_PREFIX} bytes")

    config = data[:CONFIG_COUNT]
    if any(c < ord("0") or c > ord("9") for c in config):
        raise ValueError("configuration prefix contains a non-decimal ASCII byte")

    path_len = struct.unpack_from("<h", data, CONFIG_COUNT)[0]
    if path_len < 0:
        raise ValueError(f"negative path length {path_len}")

    end = FIXED_PREFIX + path_len
    if end != len(data):
        raise ValueError(
            f"file size {len(data)} != 15 + path_length {path_len} = {end}"
        )

    return InfoDat(
        config_ascii=config,
        selectors=tuple(c - ord("0") for c in config),
        path=data[FIXED_PREFIX:end],
    )


def encode(info: InfoDat) -> bytes:
    if len(info.config_ascii) != CONFIG_COUNT:
        raise ValueError("config_ascii must be exactly 13 bytes")
    if any(c < ord("0") or c > ord("9") for c in info.config_ascii):
        raise ValueError("config_ascii must contain decimal ASCII bytes")
    if len(info.path) > 0x7FFF:
        raise ValueError("path too long for signed int16 length")

    return info.config_ascii + struct.pack("<h", len(info.path)) + info.path


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("info", type=Path)
    args = parser.parse_args()

    raw = args.info.read_bytes()
    info = parse(raw)
    rebuilt = encode(info)

    print(f"file={args.info.name} bytes={len(raw)}")
    print(f"config_ascii={info.config_ascii.decode('ascii')}")
    print("selectors=" + ",".join(str(value) for value in info.selectors))
    print(
        f"path_length={len(info.path)} "
        f"path={info.path.decode('ascii', errors='backslashreplace')!r}"
    )
    print(f"roundtrip={'PASS' if rebuilt == raw else 'FAIL'}")

    if rebuilt != raw:
        raise SystemExit(1)


if __name__ == "__main__":
    main()
