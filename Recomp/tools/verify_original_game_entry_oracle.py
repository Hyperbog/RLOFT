#!/usr/bin/env python3
"""Authenticate independent original CODE.2 evidence for native-source 20608 execution.

This never emulates game code. It checks the *shipped executable bytes*,
not the C++ implementation, against immutable span identities. The input is
the owner's legally obtained CODE.2; commercial data are not committed.
"""
from __future__ import annotations

import argparse
import hashlib
import sys
from pathlib import Path

ORIGINAL_SHA256 = "853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314"
ORIGINAL_BYTES = 558689
CODE_START = 0x3AF24
DATA_START = 0x75F24
SPAN_DIGESTS = {
    # Actual game application main, resource prelude, libc getenv/atoi.
    (0x20608, 0x207D1): "fdaaf94da4840edf65fe06495c9e2763e0b17c8b235f8b7c71af01eba467730c",
    (0x20440, 0x205B4): "251865897280e0fefbea7b94194e985643b6147384f73b7855ed75e116e7327f",
    (0x2F2EC, 0x2F465): "61dec3d1bfe306b0cb80f4defa5d269d123b2c8adb31ec5f9fb9b416d5f0f8a4",
    (0x352EE, 0x353A0): "dc9ce7cc10ff7742da359d8c740fc0d003cd2cf2edb2f706e701f15da9dd94f1",
    (0x375FE, 0x376A0): "0e0b2d098d9bd173c1877bede1d9405cb3d4d639c6fe3a4dbe98fc6faa4f65f1",
}
ORIGINAL_SETTINGS = (
    (0x114C, b"ALSIU\0"),
    (0x1154, b"KHSSDKJ\0"),
    (0x115C, b"CANLW\0"),
    (0x1164, b"SEPVON\0"),
)


def verify(original: bytes) -> None:
    if len(original) != ORIGINAL_BYTES:
        raise ValueError(f"CODE.2 size mismatch: {len(original)} != {ORIGINAL_BYTES}")
    sha = hashlib.sha256(original).hexdigest()
    if sha != ORIGINAL_SHA256:
        raise ValueError(f"CODE.2 SHA-256 mismatch: {sha}")

    for (start, stop), digest in SPAN_DIGESTS.items():
        raw = original[CODE_START + start:CODE_START + stop]
        if hashlib.sha256(raw).hexdigest() != digest:
            raise ValueError(f"CODE.2 original-source span {start:05X}:{stop:05X} mismatch")
        print(f"ORIGINAL_CODE2_SPAN={start:05X}:{stop:05X} VERIFIED bytes={len(raw)}")

    for offset, raw in ORIGINAL_SETTINGS:
        if original[DATA_START + offset:DATA_START + offset + len(raw)] != raw:
            raise ValueError(f"CODE.2 original getenv key {offset:05X} mismatch")
    print("ORIGINAL_20608_ENVIRONMENT_KEYS=4/4 VERIFIED")

    # The *real* original CODE.2 ctype byte table supplies decimal-digit
    # bit 0x20; our synthetic test has a separately flagged imitation.
    for digit in range(ord("0"), ord("9")+1):
        if not (original[DATA_START + 0x165C + digit + 1] & 0x20):
            raise ValueError(f"original atoi decimal class absent for {digit:02X}")
    print("ORIGINAL_35349_DIGIT_CLASS=10/10 VERIFIED")

    if original[CODE_START + 0x20608:CODE_START + 0x2060D] != b"\x53\x56\x57\x55\xbd":
        raise ValueError("CODE.2 actual main prologue mismatch")
    if original[CODE_START + 0x2F2EC:CODE_START + 0x2F2EE] != b"\x53\x66":
        raise ValueError("CODE.2 audio/resource prelude mismatch")
    print("ORIGINAL_20608_TO_2F2EC_BYTE_IDENTITY=VERIFIED")
    print("NATIVE_CXX_SOURCE_RUNTIME_PARITY=NOT_PROVEN")
    print("ORIGINAL_DOS_BOOT=NOT_TESTED")


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("code2", type=Path, help="Unmodified original CODE.2")
    args = parser.parse_args()
    try:
        verify(args.code2.read_bytes())
    except (OSError, ValueError) as exc:
        print(f"ORIGINAL_GAME_ENTRY_ORACLE_FAIL: {exc}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
