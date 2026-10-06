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
    raw_config_ascii: bytes
    selectors: tuple[int, ...]
    resource_path: bytes

    @property
    def config_ascii(self) -> bytes:
        return self.raw_config_ascii

    @property
    def path(self) -> bytes:
        return self.resource_path


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
        raw_config_ascii=config,
        selectors=tuple(c - ord("0") for c in config),
        resource_path=data[FIXED_PREFIX:end],
    )


def encode(info: InfoDat) -> bytes:
    if len(info.selectors) != CONFIG_COUNT:
        raise ValueError("selectors must contain exactly 13 values")
    if any(value < 0 or value > 9 for value in info.selectors):
        raise ValueError("selectors must be decimal values 0..9")
    if len(info.resource_path) > 0x7FFF:
        raise ValueError("resource path too long for signed int16 length")

    config_ascii = bytes(ord("0") + value for value in info.selectors)
    return (
        config_ascii
        + struct.pack("<h", len(info.resource_path))
        + info.resource_path
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("info", type=Path)
    args = parser.parse_args()

    raw = args.info.read_bytes()
    info = parse(raw)
    rebuilt = encode(info)

    print(f"file={args.info.name} bytes={len(raw)}")
    print(f"config_ascii={bytes(ord('0') + value for value in info.selectors).decode('ascii')}")
    print("selectors=" + ",".join(str(value) for value in info.selectors))
    print(
        f"path_length={len(info.resource_path)} "
        f"path={info.resource_path.decode('ascii', errors='backslashreplace')!r}"
    )
    print(f"roundtrip={'PASS' if rebuilt == raw else 'FAIL'}")

    if rebuilt != raw:
        raise SystemExit(1)


if __name__ == "__main__":
    main()
