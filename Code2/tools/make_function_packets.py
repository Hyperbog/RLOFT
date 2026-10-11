#!/usr/bin/env python3
"""Offline work packets for source-backed CODE.1/CODE.2 matching experiments.

No network, no compiler guess, no byte-match claim. Generated local packets
are *private*; do not add raw disassembly or original binary data to Git.
"""
from __future__ import annotations

import argparse
import hashlib
import json
from pathlib import Path
import re
import shutil
import subprocess
import tempfile

SPECS = {
    "CODE.1": {
        "size": 277787,
        "sha256": "fd9b709ceee190e6a70870f9012ebea3bc437cae6bc74e0150d6f922fa750fed",
        "object_offset": 0x2C724,
        "object_length": 0x11302,
        "object_sha256": "c0bf2c313aa8a24c9a0c0a3ce498f87a1d09299bb826413acaeb844412007203",
    },
    "CODE.2": {
        "size": 558689,
        "sha256": "853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314",
        "object_offset": 0x3AF24,
        "object_length": 0x3A852,
        "object_sha256": "014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3",
    },
}


def parse_offset(value: str | int) -> int:
    if isinstance(value, int) and not isinstance(value, bool):
        offset = value
    elif isinstance(value, str) and re.fullmatch(r"(?:0[xX][0-9A-Fa-f]+|[0-9]+)", value):
        offset = int(value, 0) if value.lower().startswith("0x") else int(value, 10)
    else:
        raise ValueError("offset must be an integer or 0x-prefixed hexadecimal")
    if offset < 0:
        raise ValueError("offset cannot be negative")
    return offset


def load_original(binary: Path) -> tuple[str, dict, bytes]:
    name = binary.name.upper()
    if name not in SPECS:
        raise ValueError("expected exact filename CODE.1 or CODE.2")
    spec = SPECS[name]
    original = binary.read_bytes()
    if len(original) != spec["size"] or hashlib.sha256(original).hexdigest() != spec["sha256"]:
        raise ValueError(f"{name} is not the pinned original version; refusing address extraction")
    lo, hi = spec["object_offset"], spec["object_offset"] + spec["object_length"]
    obj = original[lo:hi]
    if len(obj) != spec["object_length"]:
        raise ValueError("object extent truncated")
    if spec["object_sha256"] and hashlib.sha256(obj).hexdigest() != spec["object_sha256"]:
        raise ValueError("object SHA-256 mismatch")
    return name, spec, obj


def parse_targets(path: Path, object_length: int) -> list[tuple[int, int]]:
    source = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(source, list) or not source:
        raise ValueError("targets JSON must be a nonempty list")
    ranges = []
    seen = set()
    for row in source:
        if not isinstance(row, dict) or set(row) != {"start", "end"}:
            raise ValueError("each target must have exactly start/end")
        start, end = parse_offset(row["start"]), parse_offset(row["end"])
        if start >= end or end > object_length:
            raise ValueError(f"invalid half-open object-1 span: {start:#x}..{end:#x}")
        if (start, end) in seen:
            raise ValueError("duplicate span")
        ranges.append((start, end))
        seen.add((start, end))
    return ranges


def build_packet(
    name: str, spec: dict, obj: bytes, start: int, end: int, out: Path,
    *, with_disassembly: bool = False, objdump: str | None = None,
) -> dict:
    if start < 0 or start >= end or end > len(obj):
        raise ValueError("span outside executable object-1")
    if out.exists():
        raise FileExistsError(f"refusing to overwrite an existing packet: {out}")
    if with_disassembly and not objdump:
        raise ValueError("objdump is required for --disassemble")
    segment = obj[start:end]
    meta = {
        "schema": "rloft-local-decomp-work-packet-v1",
        "original_file": name,
        "original_file_size": spec["size"],
        "original_file_sha256": spec["sha256"],
        "object_1_file_offset": hex(spec["object_offset"]),
        "object_1_length": spec["object_length"],
        "object_relative_start": hex(start),
        "object_relative_end_exclusive": hex(end),
        "physical_file_start": hex(spec["object_offset"] + start),
        "physical_file_end_exclusive": hex(spec["object_offset"] + end),
        "selected_bytes": len(segment),
        "selected_sha256": hashlib.sha256(segment).hexdigest(),
        "classification": "provisional_span_requires_instruction_boundary_proof",
        "compiler_family": "Watcom_candidate_not_fingerprinted",
        "compiler_match": "not_tested",
        "native_runtime_parity": "not_tested",
        "public_upload": "not_authorized",
    }
    # Do not create destination until disassembler and all input gates pass.
    asm = None
    if with_disassembly:
        with tempfile.TemporaryDirectory(prefix="rloft-object1-") as td:
            local = Path(td) / "object1.bin"
            local.write_bytes(obj)  # scratch only; never a Git asset
            result = subprocess.run([
                str(objdump), "-D", "-b", "binary", "-m", "i386", "-M", "intel",
                f"--start-address={start}", f"--stop-address={end}", str(local),
            ], text=True, capture_output=True, timeout=30, check=False)
            if result.returncode != 0:
                raise RuntimeError(f"objdump failed: {result.stderr.strip()}")
            asm = ("# LOCAL ORIGINAL SOURCE - DO NOT COMMIT OR AUTO-UPLOAD\n"
                   "# Linear-sweep candidate only; jump tables and shared tails require proof.\n"
                   "# objdump text is NOT guaranteed paste-ready GAS assembly for decomp.me.\n\n"
                   + result.stdout)
    out.mkdir(parents=True, exist_ok=False)
    (out / "packet.json").write_text(json.dumps(meta, indent=2, sort_keys=True) + "\n", encoding="utf-8")
    (out / "notes.md").write_text(
        f"# Local work packet: {name} object-1 {start:#x}..{end:#x} (exclusive)\n\n"
        "- Verify actual instruction starts/stops, code-versus-data and any cross-entry tails.\n"
        "- Consult canonical Code1/Code1.cpp or Code2/Code2.cpp, original call graph and relocations.\n"
        "- Fingerprint compiler version, flags, ABI, register convention and LE fixups before matching.\n"
        "- Test Watcom 10.0a as a **hypothesis**, not an identified original compiler.\n"
        "- Distinguish exact recompiled-byte match, source semantics and Windows runtime parity.\n"
        "- Work locally. decomp.me stores submitted scratches; ask owner before public upload.\n"
        "- Original assembly and executable bytes must not be committed to Git.\n",
        encoding="utf-8")
    if asm is not None:
        (out / "candidate_disassembly.txt").write_text(asm, encoding="utf-8")
    return meta


def main() -> None:
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument("--binary", type=Path, required=True, help="Exact original CODE.1 or CODE.2")
    p.add_argument("--targets", type=Path, required=True, help="JSON list of {start,end} object-relative spans")
    p.add_argument("--output", type=Path, required=True, help="New private output directory, outside Git")
    p.add_argument("--disassemble", action="store_true", help="Add local candidate objdump disassembly")
    p.add_argument("--objdump", default="objdump", help="GNU objdump executable (e.g. objdump.exe on Windows)")
    args = p.parse_args()
    name, spec, obj = load_original(args.binary)
    targets = parse_targets(args.targets, len(obj))
    if args.output.exists():
        p.error("refusing to overwrite any existing output directory")
    executable = shutil.which(args.objdump) if args.disassemble else None
    if args.disassemble and executable is None:
        p.error("objdump not found; install GNU binutils or omit --disassemble")
    args.output.mkdir(parents=True, exist_ok=False)
    for start, end in targets:
        build_packet(name, spec, obj, start, end,
                     args.output / f"{name.lower().replace('.', '_')}_{start:05x}_{end:05x}",
                     with_disassembly=args.disassemble, objdump=executable)
    print(f"PASS: {len(targets)} locally authenticated work packets in {args.output}")


if __name__ == "__main__":
    main()
