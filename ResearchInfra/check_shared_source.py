#!/usr/bin/env python3
"""Read-only Git blob parity check for critical Decomp/Recomp source files.

Run from any checkout containing both branch refs, e.g.:
  python3 ResearchInfra/check_shared_source.py --decomp-ref origin/Decomp --recomp-ref origin/Recomp
No remote fetch, source modification, or Git write is performed.
"""
import argparse
import subprocess
import sys
from pathlib import Path

CRITICAL = (
    "Code1/Code1.cpp",
    "Code2/Code2.cpp",
    "Code2/graphics/original_levels_grid.hpp",
    "Code2/graphics/original_res_image_archive.hpp",
    "Code2/graphics/original_res_type6.hpp",
    "Code2/graphics/original_vga_math.hpp",
)


def blobs(repo: Path, ref: str) -> dict[str, str]:
    proc = subprocess.run(
        ["git", "-C", str(repo), "ls-tree", "-r", ref, "--", *CRITICAL],
        capture_output=True, text=True, check=True,
    )
    result = {}
    for line in proc.stdout.splitlines():
        metadata, path = line.split("\t", 1)
        mode, kind, sha = metadata.split()
        if kind != "blob" or mode not in ("100644", "100755"):
            raise ValueError(f"Unexpected Git object for {path}: {metadata}")
        result[path] = sha
    return result


def differences(left: dict[str, str], right: dict[str, str]) -> list[str]:
    issues = []
    for path in CRITICAL:
        a, b = left.get(path), right.get(path)
        if a is None or b is None:
            issues.append(f"MISSING {path}: Decomp={a or '-'} Recomp={b or '-'}")
        elif a != b:
            issues.append(f"DRIFT {path}: Decomp={a} Recomp={b}")
    return issues


def main(argv=None) -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--repository", type=Path, default=Path(__file__).resolve().parents[1])
    parser.add_argument("--decomp-ref", default="origin/Decomp")
    parser.add_argument("--recomp-ref", default="origin/Recomp")
    parser.add_argument("--self-test", action="store_true")
    args = parser.parse_args(argv)
    if args.self_test:
        fixture = {p: "a" * 40 for p in CRITICAL}
        assert not differences(fixture, fixture.copy())
        changed = fixture.copy()
        changed[CRITICAL[0]] = "b" * 40
        assert len(differences(fixture, changed)) == 1
        del changed[CRITICAL[0]]
        assert len(differences(fixture, changed)) == 1
        print("PASS: identical, divergent and missing source-blob fixtures")
        return 0
    try:
        left = blobs(args.repository, args.decomp_ref)
        right = blobs(args.repository, args.recomp_ref)
    except (OSError, subprocess.CalledProcessError, ValueError) as exc:
        print(f"ERROR: cannot inspect both Git refs: {exc}", file=sys.stderr)
        print("Fetch both refs locally and pass their names explicitly.", file=sys.stderr)
        return 2
    issues = differences(left, right)
    for issue in issues:
        print(issue)
    if issues:
        print(f"FAIL: {len(issues)} shared-source paths differ or are missing")
        return 1
    print(f"PASS: {len(CRITICAL)} critical source paths have identical Git blobs")
    return 0


if __name__ == "__main__":
    sys.exit(main())
