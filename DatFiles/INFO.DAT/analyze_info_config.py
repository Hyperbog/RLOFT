#!/usr/bin/env python3
"""Validate INFO.DAT resource-location selectors against certified CODE.2."""
from __future__ import annotations

import argparse
import hashlib
import struct
from pathlib import Path

INFO_SHA256 = "4ffa6ac5d5b5afd103bd67d84d3a372b401b3b19407b0c68c44bb0dfd9e32c30"
CODE2_SHA256 = "853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314"
OBJ1_FILE_OFF = 0x3AF24
OBJ1_SIZE = 0x3A852
DATA_FILE_BASE = 0x75F24


def sha256(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()


def logical(code2: bytes, addr: int, size: int) -> bytes:
    start = OBJ1_FILE_OFF + addr
    return code2[start : start + size]


def expect(code2: bytes, addr: int, expected: bytes, label: str) -> None:
    actual = logical(code2, addr, len(expected))
    if actual != expected:
        raise ValueError(
            f"{label}: CODE.2 logical 0x{addr:X}: "
            f"{actual.hex()} != {expected.hex()}"
        )


def c_string(code2: bytes, dgroup_off: int) -> bytes:
    start = DATA_FILE_BASE + dgroup_off
    end = code2.index(0, start)
    return code2[start:end]


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("info", type=Path)
    parser.add_argument("code2", type=Path)
    args = parser.parse_args()

    info = args.info.read_bytes()
    code2 = args.code2.read_bytes()

    if sha256(info) != INFO_SHA256:
        raise ValueError("unexpected INFO.DAT SHA-256")
    if sha256(code2) != CODE2_SHA256:
        raise ValueError("unexpected CODE.2 SHA-256")
    if len(info) < 15:
        raise ValueError("INFO.DAT too short")

    config = info[:13]
    if any(byte < 0x30 or byte > 0x39 for byte in config):
        raise ValueError("INFO configuration prefix is not decimal ASCII")
    selectors = [byte - 0x30 for byte in config]

    path_len = struct.unpack_from("<h", info, 13)[0]
    if path_len < 0 or 15 + path_len != len(info):
        raise ValueError("invalid INFO.DAT path extent")
    path = info[15:]

    # Reader: 13-byte input at F054, clear F061, then subtract ASCII '0'.
    expect(
        code2,
        0x2E78E,
        bytes.fromhex("6a 0d 68 54 f0 00 00"),
        "13-byte selector read",
    )
    expect(
        code2,
        0x2E79D,
        bytes.fromhex("6a 50 6a 00 68 61 f0 00 00"),
        "path buffer clear",
    )
    expect(
        code2,
        0x2E7B0,
        bytes.fromhex("40 80 a8 53 f0 00 00 30 83 f8 0d 7c f3"),
        "13-byte ASCII normalization loop",
    )

    # Main setup passes literal "RES" and count 11 to the resource subsystem.
    expect(
        code2,
        0x20444,
        bytes.fromhex("6a 0b 68 30 11 00 00 e8 9c ee 00 00"),
        "RES family setup call",
    )

    # 2E7F0 tests selector[i]. A nonzero selector enters the path-prefix branch.
    expect(
        code2,
        0x2E830,
        bytes.fromhex("80 b8 54 f0 00 00 00 74 74"),
        "selector indexed test",
    )
    expect(
        code2,
        0x2E839,
        bytes.fromhex("be 61 f0 00 00"),
        "INFO path source",
    )
    expect(
        code2,
        0x2E85A,
        bytes.fromhex("be b8 13 00 00"),
        "path separator source",
    )

    if c_string(code2, 0x1130) != b"RES":
        raise ValueError("DGROUP 0x1130 is not RES")
    if c_string(code2, 0x1384) != b"INFO.DAT":
        raise ValueError("DGROUP 0x1384 is not INFO.DAT")
    if c_string(code2, 0x13B8) != b"\\":
        raise ValueError("DGROUP 0x13B8 is not the path separator")

    # The certified object has only the F054 base references above; the two
    # trailing selectors have no direct absolute reference. Combined with the
    # sole proved RES setup count of 11, their runtime meaning remains open.
    obj = code2[OBJ1_FILE_OFF : OBJ1_FILE_OFF + OBJ1_SIZE]
    direct_f054 = obj.count(struct.pack("<I", 0xF054))
    direct_f05f = obj.count(struct.pack("<I", 0xF05F))
    direct_f060 = obj.count(struct.pack("<I", 0xF060))
    if direct_f054 != 2 or direct_f05f != 0 or direct_f060 != 0:
        raise ValueError(
            "unexpected selector-address references: "
            f"F054={direct_f054}, F05F={direct_f05f}, F060={direct_f060}"
        )

    print(f"info_sha256={sha256(info)} PASS")
    print(f"code2_sha256={sha256(code2)} PASS")
    print(f"selectors_ascii={config.decode('ascii')} normalized={selectors}")
    print(f"path={path.decode('ascii')!r}")

    prefix = path.decode("ascii")
    for index in range(11):
        filename = (
            f"{prefix}\\RES{index}"
            if selectors[index] != 0
            else f"RES{index}"
        )
        print(f"selector[{index}]={selectors[index]} -> {filename}")

    print(
        f"selector[11]={selectors[11]} -> "
        "no CODE.2 RES-loader consumption proved"
    )
    print(
        f"selector[12]={selectors[12]} -> "
        "no CODE.2 RES-loader consumption proved"
    )
    print(
        f"absolute_refs F054={direct_f054} "
        f"F05F={direct_f05f} F060={direct_f060}"
    )
    print("INFO resource-location audit: PASS")


if __name__ == "__main__":
    main()
