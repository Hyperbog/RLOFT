#!/usr/bin/env python3
"""Build *local*, hash-verified linear x86 disassembly references.

Never commit generated text dumps of the proprietary game binaries.
The text is a lookup aid only; original bytes and checked control-flow
boundaries outrank linear-sweep instruction guesses.
"""
from __future__ import annotations

import argparse
import hashlib
import re
import subprocess
import tempfile
from dataclasses import dataclass
from pathlib import Path


@dataclass(frozen=True)
class Original:
    filename: str
    size: int
    sha256: str
    code_start: int
    code_length: int


ORIGINALS = (
    Original("CODE.1", 277787,
             "fd9b709ceee190e6a70870f9012ebea3bc437cae6bc74e0150d6f922fa750fed",
             0x2C724, 0x11302),
    Original("CODE.2", 558689,
             "853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314",
             0x3AF24, 0x3A852),
)

ROW = re.compile(r"^\s*([0-9a-f]+):\s*((?:[0-9a-f]{2}\s+)+)(.*)$")


def build_one(original: Original, source: Path, dest: Path, executable: str) -> tuple[Path, Path]:
    data = source.read_bytes()
    actual = hashlib.sha256(data).hexdigest()
    if len(data) != original.size or actual != original.sha256:
        raise ValueError(
            f"{source}: unsupported original (size {len(data)}, SHA256 {actual}); "
            f"expected size {original.size}, SHA256 {original.sha256}")
    obj = data[original.code_start: original.code_start + original.code_length]
    if len(obj) != original.code_length:
        raise ValueError(f"{source}: truncated executable object")
    with tempfile.TemporaryDirectory(prefix="rloft-disasm-") as work:
        binary = Path(work) / "object1.bin"
        binary.write_bytes(obj)
        result = subprocess.run(
            [executable, "-D", "-z", "-b", "binary", "-m", "i386", "-M", "intel", str(binary)],
            check=True, capture_output=True, text=True)

    stem = original.filename.replace(".", "")
    dump = dest / f"{stem}_OBJECT1_DISASSEMBLY.txt"
    index = dest / f"{stem}_OBJECT1_ADDRESS_INDEX.tsv"
    preface = (
        f"ORIGINAL: {original.filename}\n"
        f"SHA256: {original.sha256}\n"
        f"OBJECT1 FILE START: 0x{original.code_start:X}\n"
        f"OBJECT1 BYTE LENGTH: 0x{original.code_length:X}\n"
        "ADDR RULE: physical file offset = object1 logical address + OBJECT1 FILE START\n"
        "IMPORTANT: automatic linear-sweep x86 decode, NOT verified function/control-flow boundaries.\n"
        "Mixed data, branches, overlapping instructions and embedded tables must be checked against the\n"
        "immutable original bytes and the audited Code1/Code2 source before accepting any interpretation.\n"
        "This reference is not evidence of original-versus-native gameplay or timing equivalence.\n"
        "\n")
    dump.write_text(preface + result.stdout, encoding="utf-8")
    rows = ["logical_hex\tphysical_hex\tbytes_hex\tlinear_disassembly\n"]
    matched = 0
    for line in result.stdout.splitlines():
        m = ROW.match(line)
        if not m:
            continue
        logical = int(m.group(1), 16)
        opcode = bytes.fromhex(m.group(2))
        if logical + len(opcode) > len(obj) or obj[logical:logical + len(opcode)] != opcode:
            raise ValueError(f"{original.filename}: disassembler byte mismatch at {logical:X}")
        asm = m.group(3).strip()
        rows.append(f"0x{logical:05X}\t0x{original.code_start + logical:06X}\t"
                    f"{opcode.hex()}\t{asm}\n")
        matched += len(opcode)
    if matched != len(obj):
        raise ValueError(f"{original.filename}: index spans {matched} bytes, object has {len(obj)}")
    index.write_text("".join(rows), encoding="utf-8")
    return dump, index


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--code1", type=Path, help="Path to original CODE.1")
    parser.add_argument("--code2", type=Path, help="Path to original CODE.2")
    parser.add_argument("--out", type=Path, default=Path(".rloft-disassembly-cache"),
                        help="Local, untracked output directory")
    parser.add_argument("--objdump", default="objdump", help="GNU objdump executable")
    args = parser.parse_args()
    if args.code1 is None and args.code2 is None:
        parser.error("supply --code1 and/or --code2")
    chosen = [(ORIGINALS[0], args.code1), (ORIGINALS[1], args.code2)]
    # Validate every requested file before generating ANY output.
    for original, path in chosen:
        if path is None:
            continue
        source = path.read_bytes()
        digest = hashlib.sha256(source).hexdigest()
        if len(source) != original.size or digest != original.sha256:
            parser.error(f"{path}: original-file SHA256 or length mismatch")
    args.out.mkdir(parents=True, exist_ok=True)
    for original, path in chosen:
        if path is not None:
            dump, index = build_one(original, path, args.out, args.objdump)
            print(f"verified {original.filename}: {dump} / {index}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
