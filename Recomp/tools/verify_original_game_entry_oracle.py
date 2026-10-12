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
    # Additional exact original-source evidence for full authentic startup
    # 3968E argv, 375FE callback/IVT, 30BB4 vectors and sound.cfg opening.
    (0x30BB4, 0x30C37): "4221c6df674a551e33c4583c389fbf389af064535d9511e4f3495c84ecd31577",
    (0x290D0, 0x290F2): "fd8b11cd2114c52c6973e9dd4ed72f67fa12423a68a15d497218f8208d8a8303",
    (0x30CC4, 0x30CD4): "1912784b0f711b5e59a7424a041c50bcf73e40e712ea286986988512ed58fe05",
    (0x3104C, 0x310B0): "e85ea5e1dd35d5aa9f202f42f40a9dfdcf690ebdf24c9e318d1d61549df4fda4",
    (0x37B34, 0x37C37): "204051c822e62670029c5f0131c0fce993e377f451867e1dee26dc000e1413a4",
    (0x39838, 0x398E0): "d7612a34176a8652888a565e47880de947f63e4c71fcdf4577de1058052a9f47",
    (0x30D30, 0x30D52): "9c2366aaeadffa4565568dfe3bd0cf8c75ac09a93ae1fbf63d3260844ac4f4dd",
    (0x379D0, 0x37A01): "8bd4126f640ec6f565a8e4069e0dbd027d32ee12da3ec9d6ff52ce2151aa7365",
    (0x312A0, 0x312B2): "eec2a720f602d4a18f3edd6549db9d07351f1d985978cdf1f31f0405b7c941e7",
    (0x37E77, 0x37EF0): "f71b8ca083fc298c0ee0434ad200aa196357adf332d6323f11e0e64637b9d462",
    (0x3968E, 0x39752): "405b8d6871e820a7d8191df7f56818f68d3e2e9acc0a724f81c857fba0634c74",
    (0x35349, 0x353A0): "024e4e497736be8bc8ab6c704596a56e4791f24b3f9d74f317e420f917d88349",
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

    # Code bytes are checked independently, not inferred from C++ naming.
    # 375FE records the old far callback into DGROUP 105F2/105F6,
    # then reads the DOS extender mode at 12662. 105FC is actually argc
    # written by 3968E, and *not* the vector-mode source.
    exact_windows = (
        (0x37607, bytes.fromhex("a3 f2 05 01 00 66 89 15 f6 05 01 00")),
        (0x3761E, bytes.fromhex("a0 62 26 01 00")),
        (0x37623, bytes.fromhex("3c 02 72 10 3c 06 77 0c")),
    )
    for off, expected in exact_windows:
        if original[CODE_START + off:CODE_START + off + len(expected)] != expected:
            raise ValueError(f"CODE.2 authentic 375FE side effect {off:05X} changed")
    if original[DATA_START + 0x143C:DATA_START + 0x1446] != bytes.fromhex("73 6f 75 6e 64 2e 63 66 67 00"):
        raise ValueError("original CODE.2 sound.cfg filename literal missing")
    print("ORIGINAL_375FE_FAR_CALLBACK_SAVE_AND_MODE=3/3 VERIFIED")
    print("ORIGINAL_SOUND_CFG_FILENAME=VERIFIED")

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
