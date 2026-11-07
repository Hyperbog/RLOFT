#!/usr/bin/env python3
"""Validate source-only Windows engineering packages; block premature releases."""
from __future__ import annotations

import argparse
import hashlib
import json
import re
from pathlib import Path

EXECUTABLES = frozenset({
    "rloft_code2_level22_window.exe",
    "rloft_gog_cd_extract.exe",
    "rloft_original_code2_cine00_player.exe",
})
LAUNCHERS = frozenset({"Launch-RLOFT-Level22.ps1", "Play-RLOFT-Level22.cmd"})
FILES = EXECUTABLES | LAUNCHERS | {"README-BUILD.txt", "SHA256SUMS.txt"}
HASH_LINE = re.compile(r"([0-9a-fA-F]{64}) \*([^/\\\r\n]+)")
SOURCE_SHA = re.compile(r"(?m)^SOURCE_COMMIT=([0-9a-fA-F]{40})\r?$")


class GateError(Exception):
    pass


def require(condition: bool, why: str) -> None:
    if not condition:
        raise GateError(why)


def validate_package(directory: Path) -> str:
    require(directory.is_dir() and not directory.is_symlink(),
            "package path must be a real directory")
    entries = list(directory.iterdir())
    names = {entry.name for entry in entries}
    require(names == FILES, f"package file set mismatch: missing={sorted(FILES - names)} "
            f"unexpected={sorted(names - FILES)}")
    require(all(entry.is_file() and not entry.is_symlink() for entry in entries),
            "package must contain only ordinary files (no symlinks/directories)")
    for exe in sorted(EXECUTABLES):
        with (directory / exe).open("rb") as file:
            require(file.read(2) == b"MZ", f"not a Windows PE executable: {exe}")
    for launcher in sorted(LAUNCHERS):
        require((directory / launcher).stat().st_size > 0,
                f"empty launcher: {launcher}")
    readme = (directory / "README-BUILD.txt").read_text(encoding="utf-8-sig")
    require("BRANCH=Recomp" in readme, "package is not sourced from Recomp")
    match = SOURCE_SHA.search(readme)
    require(match is not None, "package README lacks 40-character source commit")
    checksums = (directory / "SHA256SUMS.txt").read_text(
        encoding="utf-8-sig").splitlines()
    require(len(checksums) == len(FILES) - 1, "SHA256SUMS entry count mismatch")
    seen: set[str] = set()
    for line in checksums:
        item = HASH_LINE.fullmatch(line)
        require(item is not None, "malformed SHA256SUMS line")
        expected, name = item.groups()
        require(name in FILES - {"SHA256SUMS.txt"} and name not in seen,
                f"unexpected or duplicate SHA256SUMS filename: {name}")
        seen.add(name)
        actual = hashlib.sha256((directory / name).read_bytes()).hexdigest()
        require(actual == expected.lower(), f"SHA256 mismatch: {name}")
    require(seen == FILES - {"SHA256SUMS.txt"},
            "SHA256SUMS does not cover every packaged artifact")
    return readme


def validate_production(ledger: Path, readme: str) -> None:
    gates = json.loads(ledger.read_text(encoding="utf-8"))
    entries = gates.get("gates", [])
    require(gates.get("total") == 12 and len(entries) == 12,
            "release acceptance ledger is incomplete")
    expected = {f"R{i:02d}" for i in range(1, 13)}
    require({item.get("id") for item in entries} == expected,
            "release acceptance ledger IDs do not match R01-R12")
    pending = [item["id"] for item in entries if item.get("accepted") is not True]
    require(not pending, "production release blocked by unaccepted gates: " +
            ", ".join(pending))
    require("EXPERIMENTAL" not in readme.upper(),
            "production release cannot carry an experimental README")


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--package", required=True, type=Path)
    parser.add_argument("--mode", choices=("engineering", "production"),
                        default="engineering")
    parser.add_argument("--ledger", type=Path, default=
                        Path(__file__).resolve().parents[1] / "ACCEPTANCE_GATES.json")
    args = parser.parse_args()
    try:
        readme = validate_package(args.package)
        if args.mode == "production":
            validate_production(args.ledger, readme)
    except (GateError, OSError, ValueError, KeyError) as exc:
        print(f"RLOFT_PACKAGE_GATE_FAIL: {exc}")
        return 1
    print(f"RLOFT_PACKAGE_GATE_PASS mode={args.mode} "
          f"binaries={len(EXECUTABLES)} source_only=YES checksums=VERIFIED")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
