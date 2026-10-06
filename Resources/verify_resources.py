#!/usr/bin/env python3
"""Aggregate Ravenloft resource certification runner.

The immutable game resources are intentionally not stored in this repository.
Use --corpus to point at the original 33 resource/support files. The search is
recursive but each canonical basename must resolve to exactly one regular file.
"""
from __future__ import annotations

import argparse
import hashlib
import importlib.util
import json
import os
import py_compile
import shutil
import struct
import subprocess
import sys
import tempfile
from pathlib import Path

HERE = Path(__file__).resolve().parent
REPO = HERE.parent
MANIFEST_PATH = HERE / "CANONICAL_MANIFEST.json"


def sha256_file(path: Path) -> str:
    h = hashlib.sha256()
    with path.open("rb") as fh:
        for chunk in iter(lambda: fh.read(1024 * 1024), b""):
            h.update(chunk)
    return h.hexdigest()


def load_manifest() -> list[dict[str, object]]:
    obj = json.loads(MANIFEST_PATH.read_text(encoding="utf-8"))
    files = obj.get("files")
    if obj.get("schema_version") != 1 or not isinstance(files, list) or len(files) != 33:
        raise ValueError("canonical manifest is not schema version 1 with 33 files")
    return files


def locate_unique(root: Path, name: str) -> Path:
    matches = [p for p in root.rglob(name) if p.is_file() and p.name == name]
    if len(matches) != 1:
        raise ValueError(f"{name}: expected exactly one file below {root}, found {len(matches)}")
    return matches[0]


def verify_identities(corpus: Path) -> dict[str, Path]:
    found: dict[str, Path] = {}
    for rec in load_manifest():
        name = str(rec["name"])
        path = locate_unique(corpus, name)
        size = path.stat().st_size
        digest = sha256_file(path)
        if size != int(rec["bytes"]):
            raise ValueError(f"{name}: size {size} != canonical {rec['bytes']}")
        if digest != rec["sha256"]:
            raise ValueError(f"{name}: sha256 {digest} != canonical {rec['sha256']}")
        found[name] = path
        print(f"identity OK  {name:<12} {size:>9}  {digest}")
    return found


def run_script(rel: str, *args: Path | str) -> None:
    cmd = [sys.executable, str(REPO / rel), *(str(x) for x in args)]
    print("+", " ".join(cmd))
    subprocess.run(cmd, cwd=REPO, check=True)


def import_script(rel: str, module_name: str):
    path = REPO / rel
    if str(path.parent) not in sys.path:
        sys.path.insert(0, str(path.parent))
    spec = importlib.util.spec_from_file_location(module_name, path)
    if spec is None or spec.loader is None:
        raise RuntimeError(f"cannot import {path}")
    module = importlib.util.module_from_spec(spec)
    sys.modules[module_name] = module
    spec.loader.exec_module(module)
    return module


def expect_value_error(fn, contains: str) -> None:
    try:
        fn()
    except ValueError as exc:
        if contains not in str(exc):
            raise AssertionError(f"expected ValueError containing {contains!r}, got {exc!r}") from exc
    else:
        raise AssertionError(f"expected ValueError containing {contains!r}")


def self_test() -> None:
    scripts = [
        "Resources/ANM/analyze_anm.py",
        "Resources/RES/analyze_res.py",
        "Resources/RES/analyze_type7.py",
        "Resources/RES/analyze_type7_banks.py",
        "Resources/RES/analyze_wordcode.py",
        "Resources/MISC/COLORS/analyze_colors.py",
        "Resources/MISC/INIT/analyze_init.py",
        "Resources/MISC/INIT/analyze_resource_bases.py",
        "Resources/MISC/SAMPLE.AD/analyze_sample_ad.py",
        "Resources/MISC/SINTAB/analyze_sintab.py",
        "Resources/MISC/T/analyze_t.py",
    ]
    for rel in scripts:
        py_compile.compile(str(REPO / rel), doraise=True)

    res = import_script("Resources/RES/analyze_res.py", "rloft_analyze_res")
    decoded, used = res.lzss(bytes([0x01, 0x41]), 1)
    assert decoded == b"A" and used == 2
    expect_value_error(
        lambda: res.lzss(bytes([0x00, 0xEE, 0xF0]), 1),
        "before CODE.2 defines it",
    )

    anm = import_script("Resources/ANM/analyze_anm.py", "rloft_analyze_anm")
    bad_anm = struct.pack("<HHHHI", 1, 0, 1, 10, 0)
    expect_value_error(lambda: anm.parse_anm(bad_anm), "dispatcher domain 0..9")

    type7 = import_script("Resources/RES/analyze_type7.py", "rloft_analyze_type7")
    body = bytearray(struct.pack("<HhhhhH", 64, 0, 0, 0, 1, 12))
    body += struct.pack("<hhh", 0x0102, 0, 0)
    body += struct.pack("<h", 0)
    expect_value_error(lambda: type7.validate_body(bytes(body)), "outside 0x0000..0x0100")

    manifest = load_manifest()
    assert len({str(x["name"]) for x in manifest}) == 33
    print("self-test OK: syntax, manifest cardinality, partial-LZSS sentinel, ANM domain, type-7 selector domain")


def link_or_copy(src: Path, dst: Path) -> None:
    try:
        os.link(src, dst)
    except OSError:
        shutil.copy2(src, dst)


def full_verify(corpus: Path, dat_root: Path | None) -> None:
    files = verify_identities(corpus)
    anms = [files[f"CINE{i:02d}.ANM"] for i in range(13)]
    ress = [files[f"RES{i}"] for i in range(13)]

    run_script("Resources/ANM/analyze_anm.py", *anms)
    run_script("Resources/RES/analyze_res.py", *ress)
    run_script("Resources/RES/analyze_type7.py", *ress)
    run_script("Resources/RES/analyze_wordcode.py", files["RES9"], "--init", files["INIT"])
    run_script("Resources/MISC/INIT/analyze_init.py", files["INIT"], "--res8", files["RES8"])
    run_script("Resources/MISC/SINTAB/analyze_sintab.py", files["SINTAB"])
    run_script("Resources/MISC/T/analyze_t.py", files["T"])
    run_script("Resources/MISC/COLORS/analyze_colors.py", files["COLORS"])
    run_script("Resources/MISC/SAMPLE.AD/analyze_sample_ad.py", files["SAMPLE.AD"])

    if dat_root is not None:
        npcs = locate_unique(dat_root, "NPCS.DAT")
        objects = locate_unique(dat_root, "OBJECTS.DAT")
        save = locate_unique(dat_root, "SAVE.DAT")
        run_script("Resources/RES/analyze_type7_banks.py", files["INIT"], files["RES3"], objects, npcs)

        with tempfile.TemporaryDirectory(prefix="rloft-resource-audit-") as td:
            root = Path(td)
            needed = {
                "INIT": files["INIT"], "RES0": files["RES0"], "RES1": files["RES1"],
                "RES4": files["RES4"], "RES5": files["RES5"], "RES6": files["RES6"],
                "RES9": files["RES9"], "RES10": files["RES10"], "T": files["T"],
                "OBJECTS.DAT": objects, "SAVE.DAT": save,
            }
            for name, src in needed.items():
                link_or_copy(src, root / name)
            run_script("Resources/MISC/INIT/analyze_resource_bases.py", root)

    print("FULL RESOURCE CERTIFICATION PASS")


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("--corpus", type=Path, help="root containing the 33 immutable resource/support binaries")
    ap.add_argument("--dat-root", type=Path, help="optional root containing NPCS.DAT, OBJECTS.DAT, SAVE.DAT")
    ap.add_argument("--self-test", action="store_true", help="run repository-only invariant tests")
    args = ap.parse_args()

    if args.self_test:
        self_test()
    if args.corpus is not None:
        full_verify(args.corpus.resolve(), args.dat_root.resolve() if args.dat_root else None)
    if not args.self_test and args.corpus is None:
        ap.error("specify --self-test and/or --corpus")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
