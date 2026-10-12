#!/usr/bin/env python3
"""Shared, fail-closed REA adapter for RLOFT and other owned projects.

REA is optional; no REA or Ghidra install, network, target execution, or game
media is needed for 'plan'. 'prepare-le' requires authenticated original owner
media and keeps extracted code strictly in a private, ignored checkout area.
This adapter is not a decompiler, a DOS emulator, or evidence of gameplay parity.
"""
from __future__ import annotations

import argparse
import hashlib
import json
import os
from pathlib import Path
import re
import shutil
import subprocess
import sys
import tempfile

from original_cfg import ORIGINALS, original_object

SCHEMA = "rloft-rea-analysis-plan-v1"
PROFILE = "rea-ghidra-static-read-only-v1"
VERSION = "6.4.0"  # audited release; no implicit npx latest or network install
MAX_WINDOW = 0x10000
MAX_TARGET_BYTES = 512 * 1024 * 1024
REPO = Path(__file__).resolve().parents[1]
REGISTRY = Path(__file__).with_name("project_registry.json")
_PROJECTS = None


def projects() -> set[str]:
    global _PROJECTS
    if _PROJECTS is None:
        doc = json.loads(REGISTRY.read_text(encoding="utf-8"))
        if doc.get("schema") != "rloft-project-registry-v1":
            raise ValueError("invalid project ownership registry")
        _PROJECTS = {p["branch"] for p in doc["primary"]}
    return _PROJECTS


def digest(path: Path) -> tuple[str, int]:
    if not path.is_file() or path.is_symlink():
        raise ValueError("target must be a regular, non-symlink file")
    h = hashlib.sha256()
    total = 0
    with path.open("rb") as f:
        for buf in iter(lambda: f.read(1024 * 1024), b""):
            total += len(buf)
            if total > MAX_TARGET_BYTES:
                raise ValueError("analysis target exceeds 512 MiB")
            h.update(buf)
    if not total:
        raise ValueError("empty analysis target")
    return h.hexdigest(), total


def private_directory(candidate: Path, root: Path = REPO) -> Path:
    """Only use a designated local private directory inside the owning checkout.

    A sibling checkout may be passed explicitly through --checkout; no branches
    are ever modified simply because the RLOFT workstream manifest lists them.
    """
    checkout = root.resolve(strict=True)
    if not (checkout / ".gitignore").is_file():
        raise ValueError("checkout must have a .gitignore")
    # The private root has to be unconditionally ignored in this checkout.
    ignore = (checkout / ".gitignore").read_text(encoding="utf-8")
    if "/.rloft-private/" not in ignore and ".rloft-private/" not in ignore:
        raise ValueError("checkout does not ignore .rloft-private/; refuse to write original bytes")
    p = candidate.resolve()
    private = (checkout / ".rloft-private").resolve()
    if p == private or private in p.parents:
        return p
    raise ValueError("private output must remain beneath checkout/.rloft-private/")


def private_write(path: Path, payload: bytes) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    if path.is_symlink():
        raise ValueError("refuse to overwrite symlink")
    fd, temporary = tempfile.mkstemp(prefix=".rea-", dir=path.parent)
    try:
        with os.fdopen(fd, "wb") as f:
            f.write(payload)
        os.replace(temporary, path)
    finally:
        if os.path.exists(temporary):
            os.unlink(temporary)


def make_plan(target: Path, project: str, target_format: str, *,
              architecture: str | None = None,
              entry_point: int | None = None,
              provenance: dict | None = None) -> dict:
    if project not in projects() and project != "external-owned-project":
        raise ValueError("unknown project branch; use external-owned-project for outside checkouts")
    target = target.resolve(strict=True)
    sha, length = digest(target)
    if target_format == "raw-binary":
        if architecture not in ("x86", "x86_64", "arm", "arm64"):
            raise ValueError("raw REA analysis needs explicit supported architecture")
        if entry_point is None or not 0 <= entry_point < 2**64:
            raise ValueError("raw REA analysis needs explicit nonnegative mapped entry")
    elif target_format in ("auto", "dos-mz"):
        if architecture is not None or entry_point is not None:
            raise ValueError("architecture and entry are only accepted for raw-binary")
    else:
        raise ValueError("unsupported REA target format")
    # REA's native DOS importer rejects LE/LX and its Windows P0 accepts PE
    # only. Reject an LE container rather than misreporting it as a DOS MZ.
    with target.open("rb") as f:
        prefix = f.read(min(length, 0x4000))
    if target_format in ("auto", "dos-mz") and prefix[:2] == b"MZ" and len(prefix) >= 0x40:
        offset = int.from_bytes(prefix[0x3c:0x40], "little")
        if 0 <= offset <= length-2:
            with target.open("rb") as f:
                f.seek(offset)
                signature = f.read(2)
            if signature in (b"LE", b"LX", b"NE"):
                raise ValueError("REA MZ importer does not support LE/LX/NE; use authenticated prepare-le")
    args = ["rea", "inspect", str(target), "--provider", "ghidra", "--json"]
    if target_format != "auto":
        args += ["--target-format", target_format]
    if target_format == "raw-binary":
        args += ["--architecture", architecture, "--entry-point", hex(entry_point)]
    return {
        "schema": SCHEMA, "analysis_profile": PROFILE,
        "rea_version_pin": VERSION, "project": project,
        "source": {"path": str(target), "sha256": sha, "size": length},
        "format": target_format, "architecture": architecture,
        "mapped_file_start": hex(entry_point) if entry_point is not None else None,
        "provider": "ghidra", "command": args, "provenance": provenance,
        "original_byte_evidence": bool(provenance and provenance.get("authenticated_original")),
        "engine_analysis_verified": False,
        "dos_runtime_parity": False, "native_gameplay_parity": False,
        "limitations": [
            "static provider output requires a separate installed REA and Ghidra",
            "pseudocode and CFG are candidates, not original game semantics",
            "raw LE code windows omit loader relocation context and original data objects",
            "original game media must remain in locally ignored private paths",
        ],
    }


def prepare_le(args) -> dict:
    checkout = args.checkout.resolve(strict=True)
    base = private_directory(args.output_dir, checkout)
    expected = ORIGINALS[args.module]
    source = args.original.resolve(strict=True)
    original_sha, length = digest(source)
    if original_sha != expected["sha256"] or length != expected["size"]:
        raise ValueError("original CODE identity did not match full-file pinned SHA-256")
    if args.start < 0 or args.length <= 0 or args.length > MAX_WINDOW:
        raise ValueError("LE code slice must be 1..65536 bytes and nonnegative offset")
    # Original LE object offset is *not* a DOS real-mode segment address.
    if args.start + args.length > expected["code_size"]:
        raise ValueError("slice crosses authenticated LE code-object boundary")
    code, meta = original_object(source.read_bytes(), args.module)
    region = code[args.start:args.start + args.length]
    mapped_base = 0x10000 + args.start
    target = base / (args.module + "-" + format(args.start, "05x") +
                     "-" + format(args.length, "x") + ".bin")
    # Reject an existing symlink and ensure the output location is private.
    private_directory(target.parent, checkout)
    private_write(target, region)
    attestation = {
        "authenticated_original": True, "original_module": args.module,
        "original_file_sha256": original_sha,
        "extracted_code_object_sha256": hashlib.sha256(code).hexdigest(),
        "code_object_offset": hex(args.start),
        "code_window_bytes": args.length,
        "window_sha256": hashlib.sha256(region).hexdigest(),
        "original_code_object_entry": hex(meta["entry"]),
        "lea_base_for_raw_window": hex(mapped_base),
        "relocations_applied": False,
    }
    plan = make_plan(target, args.project, "raw-binary", architecture="x86",
                     entry_point=mapped_base, provenance=attestation)
    private_write(target.with_suffix(".plan.json"),
                  (json.dumps(plan, indent=2, sort_keys=True) + "\n").encode())
    return plan


def load_plan(path: Path) -> dict:
    item = json.loads(path.read_text(encoding="utf-8"))
    if item.get("schema") != SCHEMA or item.get("analysis_profile") != PROFILE:
        raise ValueError("unknown REA plan/profile")
    source = item.get("source") or {}
    p = Path(source["path"])
    sha, length = digest(p)
    if sha != source["sha256"] or length != source["size"]:
        raise ValueError("REA analysis input changed since its plan")
    expected = make_plan(p, item["project"], item["format"],
                         architecture=item.get("architecture"),
                         entry_point=(int(item["mapped_file_start"], 16)
                                      if item.get("mapped_file_start") else None),
                         provenance=item.get("provenance"))
    if item != expected:
        raise ValueError("REA plan was modified after target preparation")
    return item


def run_readonly(plan: dict, output: Path, binary: str, timeout: int,
                 *, checkout: Path) -> dict:
    private_directory(output.parent, checkout)
    if not (10 <= timeout <= 900):
        raise ValueError("REA operation deadline outside 10..900 seconds")
    executable = shutil.which(binary)
    if not executable:
        raise ValueError("REA CLI not installed; use audited version and external Ghidra")
    argv = [executable] + plan["command"][1:]
    # No auto-install, no npx, no browser, no target execution. 'inspect' is
    # a static analysis command. REA may still start the user's Ghidra process.
    proc = subprocess.run(argv, capture_output=True, timeout=timeout, check=False)
    if proc.returncode:
        raise ValueError("REA inspect failed (exit code " + str(proc.returncode) +
                         "); diagnostics withheld from public output")
    if len(proc.stdout) > 32 * 1024 * 1024:
        raise ValueError("REA returned more than 32 MiB; refuse oversized evidence")
    try:
        evidence = json.loads(proc.stdout)
    except (ValueError, UnicodeDecodeError) as exc:
        raise ValueError("REA did not return valid JSON") from exc
    private_write(output, (json.dumps(evidence, sort_keys=True, indent=2) + "\n").encode())
    return {"status": "REA_STATIC_INSPECT_COMPLETED", "evidence_path": str(output),
            "original_dos_parity": False, "native_gameplay_parity": False}


def main(argv=None) -> int:
    p = argparse.ArgumentParser(description=__doc__)
    actions = p.add_subparsers(dest="operation", required=True)
    prep = actions.add_parser("prepare-le", help="authenticate LE CODE and prepare private raw Ghidra slice")
    prep.add_argument("--module", required=True, choices=sorted(ORIGINALS))
    prep.add_argument("--original", type=Path, required=True)
    prep.add_argument("--start", type=lambda s: int(s, 0), required=True)
    prep.add_argument("--length", type=lambda s: int(s, 0), required=True)
    prep.add_argument("--checkout", type=Path, default=REPO)
    prep.add_argument("--output-dir", type=Path, required=True)
    prep.add_argument("--project", choices=sorted(projects()), default="code1-decompilation")

    generic = actions.add_parser("plan", help="hash arbitrary target and record offline REA command")
    generic.add_argument("--target", type=Path, required=True)
    generic.add_argument("--project", choices=sorted(projects()) + ["external-owned-project"], required=True)
    generic.add_argument("--format", choices=["auto", "raw-binary", "dos-mz"], default="auto")
    generic.add_argument("--architecture", choices=["x86", "x86_64", "arm", "arm64"])
    generic.add_argument("--entry-point", type=lambda s: int(s, 0))
    generic.add_argument("--checkout", type=Path, default=REPO)
    generic.add_argument("--out", type=Path, required=True)

    run = actions.add_parser("run", help="execute installed REA static inspect against unchanged plan")
    run.add_argument("--plan", type=Path, required=True)
    run.add_argument("--rea", default="rea")
    run.add_argument("--checkout", type=Path, default=REPO)
    run.add_argument("--out", type=Path, required=True)
    run.add_argument("--timeout", type=int, default=300)

    args = p.parse_args(argv)
    try:
        if args.operation == "prepare-le":
            result = prepare_le(args)
        elif args.operation == "plan":
            private_directory(args.out.parent, args.checkout)
            result = make_plan(args.target, args.project, args.format,
                               architecture=args.architecture,
                               entry_point=args.entry_point)
            private_write(args.out, (json.dumps(result, indent=2, sort_keys=True) + "\n").encode())
        else:
            result = run_readonly(load_plan(args.plan), args.out, args.rea,
                                  args.timeout, checkout=args.checkout)
        safe = {"status": "REA_PLAN_PREPARED" if args.operation != "run" else result["status"],
                "project": result.get("project"), "format": result.get("format"),
                "authenticated_original": result.get("original_byte_evidence", False),
                "original_dos_parity": False, "native_gameplay_parity": False}
        print(json.dumps(safe, sort_keys=True))
        return 0
    except (OSError, ValueError, KeyError, subprocess.TimeoutExpired,
            subprocess.SubprocessError) as exc:
        print("REA_BRIDGE_REJECTED: " + str(exc), file=sys.stderr)
        return 1


if __name__ == "__main__":
    raise SystemExit(main())
