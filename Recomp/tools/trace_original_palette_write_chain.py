#!/usr/bin/env python3
"""Authenticate the retail CODE.2 palette-buffer -> DAC direct-call graph.

Static LE object-1 E8-rel32 and instruction-marker evidence only. This is
NOT a control-flow graph, a DOS execution trace or proof that the retail
8-color grayscale slice is installed by these direct calls.

Read-only. No original executable bytes are printed or copied to output.
"""

import argparse
import hashlib
import json
import struct
from pathlib import Path

ORIGINAL_SHA = "853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314"
LE_OBJ1_FILE_OFFSET = 0x3AF24
# Counts are original *byte-level direct near-call candidates*. Function
# pointers, jumps, other objects and data-looking E8 bytes are not covered.
TARGETS = {
    0x29234: (21, "submit_entire_768_byte_palette"),
    0x29248: (0, "partial_DAC_wrapper"),
    0x29CA0: (2, "RGB6_DAC_output_stream"),
    0x32150: (1, "palette_brightness_transform"),
    0x32220: (14, "palette_sequence_forward"),
    0x322EC: (14, "palette_sequence_reverse"),
    0x323BC: (0, "copy_palette_color_range"),
    0x32404: (2, "fill_palette_color_range"),
    0x32468: (4, "blend_palette_color_range"),
    0x324F8: (0, "rotate_palette_color_triplets"),
    0x325B0: (23, "read_COLORS_palette_bank"),
}
SOURCE_BUFFERS = {0xC054: "world_or_menu_COLORS_staging",
                  0xC355: "ANM_palette_staging",
                  0xF9D0: "palette_effect_temporary"}
EXPECTED_DIRECT_OUTPUT_SITES = {
    0xC054: (0x05A08, 0x05A70, 0x05E99, 0x06434, 0x0ECCC,
             0x13A86, 0x13CED, 0x13E68, 0x194D8, 0x19598,
             0x198DD, 0x1997A, 0x19C62, 0x1FAE8, 0x20497, 0x2051D),
    0xC355: (0x1367C, 0x13944),
    0xF9D0: (0x322BB, 0x32387, 0x324EB),
}


def sha256(data):
    return hashlib.sha256(data).hexdigest()


def direct_calls(blob, target):
    """Enumerate encoded near CALL rel32 whose effective target is target."""
    return tuple(offset for offset in range(len(blob) - 4)
                 if blob[offset] == 0xE8
                 and offset + 5 + struct.unpack_from("<i", blob, offset + 1)[0] == target)


def verify(condition, message):
    if not condition:
        raise ValueError(message)


def inspect(original):
    verify(sha256(original) == ORIGINAL_SHA, "Original CODE.2 SHA256 mismatch")
    object1 = original[LE_OBJ1_FILE_OFFSET:]
    verify(len(object1) == 317245, "Unexpected original LE object-1 payload")
    calls = {}
    for target, (expected_count, label) in TARGETS.items():
        sites = direct_calls(object1, target)
        verify(len(sites) == expected_count, "Unexpected %s direct-call candidates" % label)
        calls[label] = {
            "target": "0x%05X" % target,
            "direct_e8_candidates": len(sites),
            "object_1_call_sites": ["0x%05X" % site for site in sites],
        }

    # Full-indexed-palette submission 29234 pushes byte length 0x300,
    # start DAC index zero, palette buffer, then E8 rel32 to 29CA0.
    verify(object1[0x29234:0x2923B] == bytes.fromhex("68 00 03 00 00 6a 00"),
           "29234 is not the original 768-byte, index-zero wrapper")
    verify(0x2923F in direct_calls(object1, 0x29CA0),
           "29234 no longer invokes the DAC output at 2923F")
    verify(0x2925B in direct_calls(object1, 0x29CA0),
           "29248 partial palette wrapper no longer invokes 29CA0")

    # 29CA0 selects 0x3C8, writes starting color index, selects 0x3C9,
    # and streams palette RGB6 bytes in bounded 0xC0-byte batches.
    dac = object1[0x29CA0:0x29CE9]
    verify(bytes.fromhex("66 ba c8 03 ee 66 ba c9 03") in dac,
           "Original DAC 3C8/3C9 register/port sequence changed")
    verify(bytes.fromhex("81 fb c0 00 00 00") in dac
           and bytes.fromhex("b9 c0 00 00 00") in dac
           and dac.count(bytes.fromhex("f3 6e")) == 2,
           "Original 192-byte retrace-chunk streaming evidence changed")
    # The *literal* mov dx,0x3C8/0x3C9 signatures each appear once in
    # object1; dynamic port computation elsewhere is not ruled out.
    for opcode in (bytes.fromhex("66 ba c8 03"), bytes.fromhex("66 ba c9 03")):
        verify(object1.count(opcode) == 1,
               "Additional literal MOV DX,DAC-port signature detected")

    classified = {}
    all_output_sites = set()
    for ptr, expected_sites in EXPECTED_DIRECT_OUTPUT_SITES.items():
        # Caller has a nearby push imm32 for the original staging pointer.
        # A page-toggle/store may lie between the push and CALL.
        marker = b"\x68" + struct.pack("<I", ptr)
        for site in expected_sites:
            caller_window = object1[max(0, site-16):site]
            verify(marker in caller_window,
                   "Palette pointer push does not match caller at 0x%05X" % site)
            verify(sum((b"\x68" + struct.pack("<I", x)) in caller_window
                       for x in SOURCE_BUFFERS) == 1,
                   "Ambiguous DAC staging pointer at 0x%05X" % site)
        classified[SOURCE_BUFFERS[ptr]] = {
            "guest_buffer": "0x%05X" % ptr,
            "direct_submit_count": len(expected_sites),
            "direct_submit_sites": ["0x%05X" % i for i in expected_sites],
        }
        all_output_sites.update(expected_sites)
    verify(all_output_sites == set(direct_calls(object1, 0x29234)),
           "Unclassified direct 29234 palette output caller")

    # All known direct COLORS bank readers target the *same* guest
    # staging buffer. Previous strict audit checks each 256-count push.
    loader_sites = direct_calls(object1, 0x325B0)
    loader_pointer = b"\x68\x54\xc0\x00\x00"
    for site in loader_sites:
        verify(loader_pointer in object1[max(site - 48, 0):site],
               "COLORS direct caller lacks staging pointer near 0x%05X" % site)
        verify(b"\x68\x00\x01\x00\x00" in object1[max(site - 48, 0):site],
               "COLORS direct caller lacks 256-entry load near 0x%05X" % site)

    return {
        "evidence_grade": "AUTHENTICATED_ORIGINAL_CODE2_STATIC_DIRECT_E8_BYTE_WINDOWS",
        "original_sha256": ORIGINAL_SHA,
        "original_LE_object1_file_offset": "0x%05X" % LE_OBJ1_FILE_OFFSET,
        "direct_call_inventory": calls,
        "DAC_29234_buffer_sources": classified,
        "verified_hardware_output": {
            "function": "0x29CA0",
            "start_index_port": "0x3C8",
            "DAC_RGB6_data_port": "0x3C9",
            "maximum_RGB6_bytes_per_retrace_batch": 192,
            "full_palette_wrapper": "0x29234",
            "full_wrapper_color_start": 0,
            "full_wrapper_RGB6_bytes": 768,
            "direct_call_candidates_to_partial_wrapper_29248": 0,
            "literal_mov_dx_DAC_port_signatures_per_port": 1,
        },
        "retail_bank3_slice_246_to_253_caller": "NOT_VERIFIED",
        "runtime_DOS_native_frame_parity": "NOT_VERIFIED",
        "limitations": [
            "Object-1 static direct E8-rel32 inventory is not complete CFG or DOS observed reachability",
            "0 direct callers is not proof a function cannot run via jump/function pointer/other object",
            "Dynamic VGA port selection or original LE fixups outside inspected path remain possible",
            "Source RGB6 DAC equality is separate from reconstructing palette-update caller ordering",
        ],
    }


def self_test():
    sample = bytearray(96)
    sample[0] = 0xE8
    struct.pack_into("<i", sample, 1, 60 - 5)
    sample[20] = 0xE8
    struct.pack_into("<i", sample, 21, 60 - 25)
    sample[40] = 0xE8
    struct.pack_into("<i", sample, 41, -7)
    verify(direct_calls(sample, 60) == (0, 20), "near-call decoder regression")
    verify(direct_calls(sample, 38) == (40,), "signed near-call decoder regression")
    try:
        inspect(b"invalid original game binary")
    except ValueError as exc:
        verify("SHA256 mismatch" in str(exc), "Unverified-media rejection regressed")
    else:
        raise AssertionError("Unverified original was accepted")
    print("SELF_TEST_PASS palette call decode, signed displacement, authenticity rejection")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--code2", type=Path, help="Owner-owned unmodified original CODE.2")
    parser.add_argument("--output", type=Path, help="Optional local evidence-only JSON report")
    parser.add_argument("--self-test", action="store_true")
    args = parser.parse_args()
    if args.self_test:
        self_test()
    if args.code2 is None:
        if not args.self_test:
            parser.error("--code2 is required unless --self-test is given")
        return
    report = inspect(args.code2.read_bytes())
    if args.output:
        args.output.parent.mkdir(parents=True, exist_ok=True)
        args.output.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    print("AUTHENTIC_PALETTE_STATIC_CHAIN_PASS "
          "full_DAC_callers=21 COLORS_full_bank_loaders=23 "
          "effect_fade_calls=28 direct_partial_DAC_calls=0 "
          "retail_slice_246_253_caller=NOT_VERIFIED DOS_FRAME_PARITY=NOT_VERIFIED")


if __name__ == "__main__":
    main()
