#!/usr/bin/env python3
"""Independently validate the shipped SINTAB and CODE.2 renderer evidence.

From repository root:
    python DemoV1/tools/verify_renderer_offline.py /game/SINTAB /game/CODE.2

No network, GitHub Actions, third-party packages, or proprietary assets in the
repository are needed. Exit status is nonzero when an invariant fails.
"""
from __future__ import annotations

import argparse
import hashlib
from pathlib import Path
import sys

from audit_sintab_opcode_inventory import audit as audit_sintab
from reference_sintab_pixels import (
    EXPECTED_FINGERPRINTS,
    fingerprints,
    verify_descriptor_cases,
)

CODE2_BYTES = 558689
CODE2_SHA256 = "853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314"
OBJECT2_FILE_START = 0x75F24
MASK_TABLE_OFFSETS = (0xC668, 0xC688, 0xC6A8)


def verify_code2_masks(data: bytes) -> int:
    if len(data) != CODE2_BYTES or hashlib.sha256(data).hexdigest() != CODE2_SHA256:
        raise ValueError("CODE.2 does not match the certified original executable")
    if data[OBJECT2_FILE_START + 0xC029] != 1:
        raise ValueError("CODE.2 initial individual-column dispatcher flag is not one")
    if data[OBJECT2_FILE_START + 0xEF4D] != 0:
        raise ValueError("CODE.2 initial VGA page is not zero")

    tables = [
        data[OBJECT2_FILE_START + start:OBJECT2_FILE_START + start + 32]
        for start in MASK_TABLE_OFFSETS
    ]
    if any(len(table) != 32 for table in tables):
        raise ValueError("CODE.2 VGA plane lookup table was truncated")

    cases = 0
    for phase in range(4):
        for run_width in range(1, 8):
            table_index = phase * 8 + run_width
            actual = [table[table_index] for table in tables]
            expected = [0, 0, 0]
            for pixel in range(run_width):
                physical_x = phase + pixel
                expected[physical_x // 4] |= 1 << (physical_x & 3)
            if actual != expected:
                raise ValueError(
                    f"CODE.2 mask mismatch phase={phase} width={run_width}: "
                    f"{actual} != {expected}"
                )
            cases += 1
    return cases


def verify(sintab_path: Path, code2_path: Path) -> dict[str, int | str]:
    sintab = sintab_path.read_bytes()
    code2 = code2_path.read_bytes()
    opcode_result = audit_sintab(sintab)
    mask_cases = verify_code2_masks(code2)
    actual_pixels = fingerprints(sintab)
    if actual_pixels != EXPECTED_FINGERPRINTS:
        raise ValueError("SINTAB pixel fingerprints differ from independent golden values")
    verify_descriptor_cases(sintab)
    return {
        "sintab_sha256": opcode_result["sha256"],
        "code2_sha256": hashlib.sha256(code2).hexdigest(),
        "scale_blocks": opcode_result["blocks"],
        "generated_code_bytes": opcode_result["generated_code_bytes"],
        "source_reads": opcode_result["source_reads"],
        "indexed_vga_stores": opcode_result["indexed_vga_stores"],
        "patch_boundaries": opcode_result["patch_instruction_boundaries"],
        "vga_mask_cases": mask_cases,
        "corpus_fingerprint_0102": f"{actual_pixels[0x0102]:016x}",
        "corpus_fingerprint_3412": f"{actual_pixels[0x3412]:016x}",
    }


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("sintab", type=Path, help="Original SINTAB binary")
    parser.add_argument("code2", type=Path, help="Original CODE.2 binary")
    args = parser.parse_args(argv)
    try:
        result = verify(args.sintab, args.code2)
    except (OSError, ValueError) as exc:
        print(f"OFFLINE RENDERER AUDIT FAILED: {exc}", file=sys.stderr)
        return 1
    print("OFFLINE RENDERER AUDIT PASSED")
    for key, value in result.items():
        print(f"{key}: {value}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
