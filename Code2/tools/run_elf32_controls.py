#!/usr/bin/env python3
"""Repeatable GCC/Clang negative controls for CODE.2 Watcom fingerprints.

Only authorized local original media; compilers must already be installed.
Results are metadata, not copyrighted original bytes or compiler identity.
OMF/Watcom support requires a separate verified importer.
"""
from __future__ import annotations

import argparse
import hashlib
import json
from pathlib import Path
import shutil
import subprocess
import tempfile

from compare_fingerprint_elf32 import CODE2_SHA, PROBES, compare_probe

FLAGS = (
    "-m32", "-O2", "-fno-pic", "-fno-pie", "-ffreestanding",
    "-fno-asynchronous-unwind-tables", "-fno-unwind-tables",
    "-fno-stack-protector", "-fomit-frame-pointer",
    "-fno-function-sections", "-c",
)


def run(original: Path, source: Path, compiler_names: list[str]) -> dict:
    data = original.read_bytes()
    if len(data) != 558689 or hashlib.sha256(data).hexdigest() != CODE2_SHA:
        raise ValueError("not the pinned CODE.2 original; refusing compiler runs")
    outcomes = []
    with tempfile.TemporaryDirectory(prefix="rloft-elf32-controls-") as work:
        for compiler in compiler_names:
            command = shutil.which(compiler)
            if command is None:
                raise FileNotFoundError(f"compiler not found: {compiler}")
            ident = subprocess.run([command, "--version"], capture_output=True,
                                   text=True, timeout=20, check=True).stdout.splitlines()[0]
            out = Path(work) / f"probe-{len(outcomes)}.o"
            result = subprocess.run([command, *FLAGS, str(source), "-o", str(out)],
                                    capture_output=True, text=True, timeout=60)
            if result.returncode:
                raise RuntimeError(f"{compiler} failed (status {result.returncode}): {result.stderr[-1000:]}")
            probes = {name: compare_probe(original, out, name) for name in PROBES}
            outcomes.append({
                "compiler": compiler, "version": ident,
                "options": list(FLAGS),
                "object_sha256": hashlib.sha256(out.read_bytes()).hexdigest(),
                "probes": probes,
                "matches": sum(r["relocation_resolved_exact_bytes"] for r in probes.values()),
                "total_probes": len(probes),
            })
    return {
        "schema": "rloft-elf32-fingerprint-controls-v1",
        "original_code2_sha256": CODE2_SHA,
        "source_sha256": hashlib.sha256(source.read_bytes()).hexdigest(),
        "result_kind": "non_Watcom_ELF32_negative_controls",
        "compiler_identified": False,
        "omf_legacy_toolchain_tested": False,
        "le_relocation_parity_proven": False,
        "compilers": outcomes,
    }


def main() -> None:
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument("--original-code2", type=Path, required=True)
    p.add_argument("--compiler", action="append", dest="compilers", required=True,
                   help="Repeat for GCC and Clang (requires x86 32-bit codegen)")
    p.add_argument("--source", type=Path, default=Path(__file__).parent/"probes"/"elf32_negative_controls.c")
    p.add_argument("--output", type=Path, help="Optional metadata-only JSON report")
    args = p.parse_args()
    report = run(args.original_code2, args.source, args.compilers)
    result = json.dumps(report, sort_keys=True, indent=2) + "\n"
    if args.output:
        args.output.write_text(result, encoding="utf-8")
    else:
        print(result, end="")

if __name__ == "__main__":
    main()
