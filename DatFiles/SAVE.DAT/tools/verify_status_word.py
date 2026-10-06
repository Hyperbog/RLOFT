#!/usr/bin/env python3
"""Verify SAVE character-status mappings against raw T and certified CODE.2."""
from __future__ import annotations

import argparse
import hashlib
import struct
from pathlib import Path

T_SHA256 = "a3f1e19e40f226c8cf150a43aacd9c378ee35a1294d2c9dd72870bfdf1e16315"
CODE2_SHA256 = "853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314"
XOR_KEY = 0xDC

STATUS_LABELS = (
    "POSSESSED",
    "HELD",
    "DISEASED",
    "POISONED",
    "PARALYZED",
)

# Exact decoded-byte hashes for T[351..354].  The verifier deliberately pins
# the raw records without embedding the long copyrighted story dialogue here.
VENGEFUL_WARNING_SHA256 = (
    "44c17c84a2526a70fa56475d37798d2b2551396e290b6411328739ee30bdafef",
    "ea19443d0a11507be962b8632be615e91d8cd911ebfecedb89f472995eb24df8",
    "d744f56fbd79cf0f8865cb21ac1821ff21b9a3c84c710a83ec68dda476c22d44",
    "81610e3d17d97216f298ec6ee0df3ef35df33e127df230ca1a4908ccb5623cfa",
)
VENGEFUL_WARNING_LENGTHS = (93, 186, 195, 156)

STATUS_DISPLAY_OFF = 0x4DBD0
WARNING_CLEAR_OFF = 0x52792
WARNING_SELECT_OFF = 0x52885
WARNING_SET_OFF = 0x528AC
RNG_OFF = 0x6BBC6
PRESENTATION_MAP_OFF = 0x5A320


def parse_t(data: bytes) -> list[bytes]:
    if len(data) < 4:
        raise AssertionError("T shorter than count")
    count = struct.unpack_from("<I", data, 0)[0]
    offsets = struct.unpack_from(f"<{count}I", data, 4)
    records: list[bytes] = []
    for i, off in enumerate(offsets):
        if off + 2 > len(data):
            raise AssertionError(f"T[{i}] length outside file")
        length = struct.unpack_from("<H", data, off)[0]
        end = off + 2 + length
        limit = offsets[i + 1] if i + 1 < count else len(data)
        if end > limit:
            raise AssertionError(f"T[{i}] overlaps next record/EOF")
        records.append(bytes(b ^ XOR_KEY for b in data[off + 2:end]))
    return records


def verify_t(path: Path) -> None:
    data = path.read_bytes()
    sha = hashlib.sha256(data).hexdigest()
    if sha != T_SHA256:
        raise AssertionError(f"T SHA-256 {sha} != {T_SHA256}")
    records = parse_t(data)
    if len(records) != 1073:
        raise AssertionError(f"T record count {len(records)} != 1073")

    labels = tuple(records[653 + i].decode("cp437", errors="strict") for i in range(5))
    if labels != STATUS_LABELS:
        raise AssertionError(f"T[653..657] {labels!r} != {STATUS_LABELS!r}")

    lengths = tuple(len(records[351 + i]) for i in range(4))
    hashes = tuple(hashlib.sha256(records[351 + i]).hexdigest() for i in range(4))
    if lengths != VENGEFUL_WARNING_LENGTHS:
        raise AssertionError(f"T[351..354] lengths {lengths} != {VENGEFUL_WARNING_LENGTHS}")
    if hashes != VENGEFUL_WARNING_SHA256:
        raise AssertionError("T[351..354] decoded-byte hashes changed")

    print(f"T SHA-256: {sha}")
    print("T[653..657] status labels: " + ", ".join(labels))
    print("T[351..354] Vengeful-Spirit warning records: PASS")


def require(data: bytes, off: int, expected: bytes, label: str) -> None:
    actual = data[off:off + len(expected)]
    if actual != expected:
        raise AssertionError(
            f"{label} mismatch at physical 0x{off:X}: {actual.hex()} != {expected.hex()}"
        )


def verify_code2(path: Path) -> None:
    data = path.read_bytes()
    sha = hashlib.sha256(data).hexdigest()
    if sha != CODE2_SHA256:
        raise AssertionError(f"CODE.2 SHA-256 {sha} != {CODE2_SHA256}")

    # Status screen initializes mask 0x0100 and T index 0x028D (653), then
    # increments the T index and doubles the mask until 0x1000.
    require(
        data,
        STATUS_DISPLAY_OFF,
        bytes.fromhex(
            "bd88000000"      # mov ebp,0x88 (display y)
            "bb00010000"      # mov ebx,0x100 (first status mask)
            "bf8c000000"
            "be1c000000"
            "c74424048d020000"  # T index 653
        ),
        "status-display initializer",
    )
    require(
        data,
        0x4DC4B,
        bytes.fromhex("ff44240401db4681fb001000007e95"),
        "status-display index/mask loop",
    )

    # Possession maintenance clears 0x0040 before evaluating the warning path.
    require(
        data,
        WARNING_CLEAR_OFF,
        bytes.fromhex("80a3f0180200bf"),
        "Vengeful warning-state clear",
    )

    # rand(); ((rand * 4) >> 15); T index = 0x15F + result.
    require(
        data,
        WARNING_SELECT_OFF,
        bytes.fromhex(
            "e83c930100"
            "8d048500000000"
            "c1e80f"
            "8d5032"
            "055f010000"
            "5056"
            "89152cbf0000"
        ),
        "Vengeful warning T-index selection",
    )
    require(
        data,
        WARNING_SET_OFF,
        bytes.fromhex("808bf018020040"),
        "Vengeful warning-state set",
    )

    # Engine RNG returns (state >> 16) & 0x7FFF, proving a 0..32767 range.
    require(
        data,
        RNG_OFF,
        bytes.fromhex(
            "e8f5ffffff85c07418"
            "69106d4ec641"
            "81c239300000"
            "891089d0c1e81025ff7f0000c3"
        ),
        "15-bit RNG",
    )

    # Presentation-state classifier checks death, paralysis, 0x0040, poison,
    # disease in that order and returns distinct presentation codes.
    presentation = data[PRESENTATION_MAP_OFF:PRESENTATION_MAP_OFF + 0x90]
    for sig, label in (
        (bytes.fromhex("80e740"), "death presentation test"),
        (bytes.fromhex("80e710"), "paralysis presentation test"),
        (bytes.fromhex("80e340"), "0x0040 presentation test"),
        (bytes.fromhex("80e708"), "poison presentation test"),
        (bytes.fromhex("80e404"), "disease presentation test"),
    ):
        if sig not in presentation:
            raise AssertionError(f"{label} missing from presentation mapper")

    print(f"CODE.2 SHA-256: {sha}")
    print("status display 0x0100..0x1000 -> T[653..657]: PASS")
    print("Vengeful-Spirit 0x0040 clear/select/set path: PASS")
    print("15-bit RNG proof for four-way T[351..354] selection: PASS")
    print("status presentation classifier signatures: PASS")


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("t_file", type=Path)
    ap.add_argument("code2", type=Path)
    args = ap.parse_args()
    verify_t(args.t_file)
    verify_code2(args.code2)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
