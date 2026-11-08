"""Read-only baseline preflight for the Ravenloft faithful-native workstream.

Reports verifiable local checkout/source state. This is not a Windows build,
CTest pass, original DOS comparison, or source-game semantic proof.
"""
from __future__ import annotations

import argparse
import hashlib
import json
from pathlib import Path
import re
import subprocess
import sys

SOURCE_SHA256 = {
    "CODE.1": "fd9b709ceee190e6a70870f9012ebea3bc437cae6bc74e0150d6f922fa750fed",
    "CODE.2": "853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314",
    "SINTAB": "19b6bc15f1be616bb8ff49dc9029995a1a0413fae06addc9fd41ddef761d0e87",
}
REQUIRED_DOCS = (
    "DemoV1/README.md",
    "DemoV1/ORIGINAL_EXECUTION_FIDELITY_CHARTER.md",
    "DemoV1/CONTINUOUS_GAME_INTEGRATION_CONTRACT_20261009.md",
    "Code1/DECOMPILATION_CONTINUITY.md",
    "Code2/CODE2_CONTINUITY_ADDENDUM.md",
)
ADD_TEST = re.compile(r"\badd_test\s*\(\s*NAME\s+([A-Za-z_][A-Za-z_0-9]*)\b", re.I)


def sha256_file(path: Path) -> str:
    hash_ = hashlib.sha256()
    with path.open("rb") as stream:
        for chunk in iter(lambda: stream.read(1024 * 1024), b""):
            hash_.update(chunk)
    return hash_.hexdigest()


def declarations(cmake_text: str) -> dict:
    # CMake '#' comments are ignored, including comments before a multiline NAME.
    # This counts source declarations, not tests configured or executed.
    uncommented = "\n".join(line.split("#", 1)[0] for line in cmake_text.splitlines())
    names = ADD_TEST.findall(uncommented)
    seen = set()
    duplicates = []
    for name in names:
        if name in seen and name not in duplicates:
            duplicates.append(name)
        seen.add(name)
    return {
        "source_declarations": len(names),
        "unique_names": len(seen),
        "duplicate_names": duplicates,
        "explanation": "CMake source declarations only; conditional registration, compilation and CTest acceptance are separate gates",
    }


def run_git(root: Path, *args: str) -> tuple[bool, str]:
    try:
        result = subprocess.run(["git", "-C", str(root), *args],
                                capture_output=True, text=True, check=False,
                                timeout=8)
    except (FileNotFoundError, OSError, subprocess.TimeoutExpired) as exc:
        return False, str(exc)
    return result.returncode == 0, result.stdout.strip() if result.returncode == 0 else result.stderr.strip()


def preflight(root: Path, originals: dict[str, Path],
              expected_head: str | None = None,
              require_clean: bool = False) -> tuple[dict, int]:
    problems = []
    root = root.resolve()
    found = [str(p) for p in REQUIRED_DOCS if not (root / p).is_file()]
    if found:
        problems.append("missing authoritative continuity documents")
    cmake = root / "DemoV1/CMakeLists.txt"
    if cmake.is_file():
        tests = declarations(cmake.read_text(encoding="utf-8"))
        if tests["duplicate_names"]:
            problems.append("duplicate CMake test names")
    else:
        tests = {"source_declarations": 0, "unique_names": 0,
                 "duplicate_names": [], "explanation": "CMakeLists.txt missing"}
        problems.append("missing DemoV1/CMakeLists.txt")
    ok_head, head = run_git(root, "rev-parse", "HEAD")
    if not ok_head:
        problems.append("Git HEAD unavailable: cannot establish current source revision")
    ok_branch, branch = run_git(root, "branch", "--show-current")
    ok_status, status = run_git(root, "status", "--porcelain", "--untracked-files=normal")
    git = {"available": ok_head, "head": head if ok_head else None,
           "branch": branch if ok_branch else None,
           "working_tree_clean": not bool(status) if ok_status else None}
    if expected_head and (not ok_head or not head.startswith(expected_head)):
        problems.append("expected Git HEAD mismatch or unavailable")
    if require_clean and (not ok_status or bool(status)):
        problems.append("dirty Git working tree or status unavailable")
    result_originals = {}
    for filename, path in originals.items():
        if filename not in SOURCE_SHA256:
            problems.append("unrecognized original-file identity: " + filename)
            continue
        try:
            actual = sha256_file(path)
            match = actual == SOURCE_SHA256[filename]
            result_originals[filename] = {"sha256": actual,
                                          "expected_sha256": SOURCE_SHA256[filename],
                                          "matches_known_uploaded_version": match}
            if not match:
                problems.append("original file differs from recorded version: " + filename)
        except OSError:
            result_originals[filename] = {"available": False}
            problems.append("original file could not be read: " + filename)
    payload = {
        "status": "PASS" if not problems else "FAIL",
        "root": str(root), "git": git, "missing_docs": found,
        "originals": result_originals, "ctest": tests,
        "checks": problems,
        "acceptance": {
            "original_binary_identity": "verified only for individually supplied matching files",
            "current_Windows_Full": "NOT CHECKED",
            "original_DOS_parity": "NOT CHECKED",
            "source_semantics": "NOT CHECKED",
        },
    }
    return payload, 0 if not problems else 2


def main(argv: list[str] | None = None) -> int:
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument("--repo-root", type=Path,
                   default=Path(__file__).resolve().parents[2])
    p.add_argument("--original-code1", type=Path)
    p.add_argument("--original-code2", type=Path)
    p.add_argument("--original-sintab", type=Path)
    p.add_argument("--expected-head", help="Reject a stale local checkout")
    p.add_argument("--require-clean", action="store_true")
    p.add_argument("--json", action="store_true")
    args = p.parse_args(argv)
    originals = {name: path for name, path in (
        ("CODE.1", args.original_code1), ("CODE.2", args.original_code2),
        ("SINTAB", args.original_sintab)) if path is not None}
    result, status = preflight(args.repo_root, originals,
                               args.expected_head, args.require_clean)
    if args.json:
        print(json.dumps(result, indent=2, sort_keys=True))
    else:
        print("Native research preflight:", result["status"])
        print("Git HEAD:", result["git"]["head"] or "unavailable")
        print("Git branch:", result["git"]["branch"] or "unavailable")
        print("Source CTest declarations:", result["ctest"]["source_declarations"])
        for name, data in result["originals"].items():
            print(name, "MATCH" if data.get("matches_known_uploaded_version") else "MISMATCH/UNAVAILABLE")
        for warning in result["checks"]:
            print("FAIL:", warning)
        print("Windows Full and original DOS parity: NOT CHECKED")
    return status


if __name__ == "__main__":
    sys.exit(main())
