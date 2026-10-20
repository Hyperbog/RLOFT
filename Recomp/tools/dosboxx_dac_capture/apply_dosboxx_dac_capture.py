#!/usr/bin/env python3
"""Apply a fail-closed, version-pinned RLOFT DOSBox-X DAC capture hook.

Source tag: joncampbell123/dosbox-x dosbox-x-v2026.10.01
No modification of executable or game original files. Run against the
exact release checkout, not an unverified downstream fork.
"""
import argparse
import hashlib
from pathlib import Path

# GitHub's exact original Git blob identity, not an inferred release name.
ORIGINAL_VGA_GIT_BLOB = "509d42fbead67a56594e49cec55549bc6fcc6d7c"
HOOK_HEADER = "rloft_dac_port_capture.hpp"


def git_blob(raw):
    return hashlib.sha1(b"blob " + str(len(raw)).encode() + b"\0" + raw).hexdigest()


def replace_once(source, old, new, label):
    if source.count(old) != 1:
        raise ValueError("Unverified DOSBox-X anchor (%s): expected exactly once" % label)
    return source.replace(old, new, 1)


def instrument(source):
    source = replace_once(source, '#include "vga.h"\n',
        '#include "vga.h"\n#include "cpu.h"\n#include "'+HOOK_HEADER+'"\n',
        "original VGA header")
    # Do not log ignored (locked-out) DAC writes. Record the actual
    # effective 8-bit index accepted by the DAC handler.
    write_index = (
        "void write_p3c8(Bitu port,Bitu val,Bitu iolen) {\n"
        "    (void)iolen;//UNUSED\n"
        "    (void)port;//UNUSED\n\n"
        "    if (vga.dosboxig.vga_dac_lockout)\n"
        "        return;\n"
    )
    source = replace_once(source, write_index,
        write_index +
        "\n    if (iolen == 1) rloft_dac_capture::record(\n"
        "        0x3c8u, (unsigned int)(uint8_t)val,\n"
        "        (unsigned int)SegValue(cs), (unsigned int)reg_eip);\n",
        "accepted 3c8 write")
    # Log after the original hardware masks a 3C9 byte to 6 bits.
    # Do not infer the original CPU opcode from reg_eip: emulator cores
    # can advance it before dispatch; store only raw CS/EIP *hints*.
    color_mask = (
        "    if (!vga_8bit_dac)\n"
        "        val&=0x3f;\n\n"
        "    if (vga.dac.pel_index < 3) {\n"
    )
    source = replace_once(source, color_mask,
        "    if (!vga_8bit_dac)\n"
        "        val&=0x3f;\n\n"
        "    if (vga.dac.pel_index < 3) {\n"
        "        if (iolen == 1) rloft_dac_capture::record(\n"
        "            0x3c9u, (unsigned int)(uint8_t)val,\n"
        "            (unsigned int)SegValue(cs), (unsigned int)reg_eip);\n",
        "accepted 3c9 write")
    return source


def apply(root, header_text, dry_run=False):
    root = Path(root)
    original_file = root / "src/hardware/vga_dac.cpp"
    output_header = root / "src/hardware" / HOOK_HEADER
    raw = original_file.read_bytes()
    if git_blob(raw) != ORIGINAL_VGA_GIT_BLOB:
        raise ValueError("Wrong DOSBox-X vga_dac.cpp Git blob; refusing patch")
    if output_header.exists():
        raise ValueError("Capture header already exists; refusing overwrite")
    # Decode only after authentication. Original is UTF-8 compatible.
    patched = instrument(raw.decode("utf-8"))
    if not dry_run:
        output_header.write_text(header_text, encoding="utf-8")
        original_file.write_text(patched, encoding="utf-8")
    return patched


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--dosbox-root", type=Path, required=True)
    parser.add_argument("--header", type=Path, required=True,
                        help="The RLOFT GPL C++ capture header")
    parser.add_argument("--check-only", action="store_true")
    args = parser.parse_args()
    patch = apply(args.dosbox_root, args.header.read_text(encoding="utf-8"),
                  args.check_only)
    if patch.count("rloft_dac_capture::record(") != 2:
        raise AssertionError("DOSBox-X insertion incomplete")
    print("DOSBOXX_2026_10_01_DAC_HOOK_%s upstream_blob=%s accepted_handlers=2" %
          ("CHECK_PASS" if args.check_only else "PATCHED",
           ORIGINAL_VGA_GIT_BLOB))


if __name__ == "__main__":
    main()
