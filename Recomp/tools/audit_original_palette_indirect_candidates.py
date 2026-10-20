#!/usr/bin/env python3
"""Authenticated original CODE.2 palette indirect-reference candidate census.

Negative findings apply to raw exact E8/E9/EB and LE32 encodings only.
They do not exclude relocation fixups, runtime pointers, other LE objects,
computed jumps, or code/data ambiguity. Not a DOS execution trace.
No original executable bytes are emitted or committed.
"""
import argparse
import hashlib
import json
import struct
from pathlib import Path

SHA256 = "853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314"
OBJ1 = 0x3AF24
TARGETS = {
    0x29248: "partial_DAC_wrapper",
    0x323BC: "palette_range_copy",
    0x324F8: "color_triplet_rotation",
}
MUTATION_CALLERS = {
    0x32404: (0x0ECBF, 0x324DE),
    0x32468: (0x05481, 0x05A63, 0x322AE, 0x3237A),
}


def relative_hits(data, target, opcode, width):
    sites = []
    for p in range(len(data)-width):
        if data[p] != opcode:
            continue
        disp = struct.unpack_from("<i" if width == 5 else "<b", data, p + 1)[0]
        if p + width + disp == target:
            sites.append(p)
    return sites


def audit(original):
    if hashlib.sha256(original).hexdigest() != SHA256:
        raise ValueError("Original CODE.2 SHA256 mismatch; refusing input")
    suffix = original[OBJ1:]
    # Original's physical suffix may contain non-code bytes: this is not
    # a disassembled CFG or proof about the runtime loader's fixup results.
    unresolved = {}
    for target, label in TARGETS.items():
        marker = struct.pack("<I", target)
        encoded_addresses = [
            p for p in range(len(suffix)-3)
            if suffix[p:p+4] == marker
        ]
        entry = {
            "target_object1_offset": f"0x{target:05X}",
            "E8_direct_call_byte_candidates": relative_hits(suffix, target, 0xE8, 5),
            "E9_direct_jump_byte_candidates": relative_hits(suffix, target, 0xE9, 5),
            "EB_short_jump_byte_candidates": relative_hits(suffix, target, 0xEB, 2),
            "literal_le32_byte_candidates": encoded_addresses,
        }
        if any(entry[key] for key in entry if key != "target_object1_offset"):
            raise ValueError("Original palette pointer/jump evidence changed: " + label)
        unresolved[label] = entry

    for target, expected in MUTATION_CALLERS.items():
        if relative_hits(suffix, target, 0xE8, 5) != list(expected):
            raise ValueError("Original palette mutator direct calls changed: " + hex(target))

    # 0x05481 / 0x05A63 blend 6 RGB triplets, first palette index 0xEC (236).
    blend_marker = b"\x6a\x06\x68\xec\x00\x00\x00"
    for site in (0x05481, 0x05A63):
        if blend_marker not in suffix[site-30:site]:
            raise ValueError("Retail six-entry palette-blend arguments changed")

    # 0x0ECBF uses start 0x90 (144), count 0x6F (111) to fill through 254.
    fill_marker = b"\x6a\x6f\x6a\x00\x6a\x00\x6a\x00\x68\x90\x00\x00\x00"
    if fill_marker not in suffix[0xECBF-40:0xECBF]:
        raise ValueError("Retail 144..254 palette-fill arguments changed")
    if b"\x68\x00\x01\x00\x00" not in suffix[0x324DE-30:0x324DE]:
        raise ValueError("Retail full-palette black-fill arguments changed")

    return {
        "original_code2_sha256": SHA256,
        "evidence_grade": "AUTHENTICATED_RETAIL_STATIC_BYTE_CENSUS_NOT_DOS_TRACE",
        "unresolved_target_raw_reference_counts": unresolved,
        "authenticated_mutations": {
            "blend_32468_direct_calls": 4,
            "blend_05481_05A63_indices": "236..241",
            "fill_32404_direct_calls": 2,
            "fill_0ECBF_indices": "144..254",
            "fill_324DE_indices": "0..255",
        },
        "eight_entry_bank3_retail_caller": "NOT_VERIFIED",
        "relocation_based_indirect_references": "NOT_AUDITED",
        "original_DOS_runtime_DAC_events": "NOT_CAPTURED",
        "native_frame_pixel_parity": "NOT_VERIFIED",
    }


def self_test():
    try:
        audit(b"not original")
    except ValueError as error:
        assert "SHA256 mismatch" in str(error)
    else:
        raise AssertionError("Unverified executable accepted")
    fixture = bytearray(24)
    fixture[0] = 0xE9
    struct.pack_into("<i", fixture, 1, 15 - 5)
    fixture[8] = 0xEB
    struct.pack_into("<b", fixture, 9, 15 - 10)
    assert relative_hits(fixture, 15, 0xE9, 5) == [0]
    assert relative_hits(fixture, 15, 0xEB, 2) == [8]
    print("SELF_TEST_PASS signed E9/EB decoding and original SHA rejection")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--code2", type=Path)
    parser.add_argument("--self-test", action="store_true")
    parser.add_argument("--report", type=Path, help="Private scalar-only JSON evidence")
    args = parser.parse_args()
    if args.self_test:
        self_test()
    if args.code2 is None:
        if not args.self_test:
            parser.error("Supply --code2 or --self-test")
        return
    report = audit(args.code2.read_bytes())
    if args.report:
        args.report.parent.mkdir(parents=True, exist_ok=True)
        args.report.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    print("AUTHENTIC_ORIGINAL_PALETTE_INDIRECT_CENSUS_PASS "
          "raw_E8_E9_EB_and_direct_le32_candidates=0 "
          "for 29248/323BC/324F8; original_DAC_trace=NOT_CAPTURED "
          "bank3_246_253_caller=NOT_VERIFIED")


if __name__ == "__main__":
    main()
