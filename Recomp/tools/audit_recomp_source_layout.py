#!/usr/bin/env python3
"""Deterministic, media-free CMake/compiled-fragment inventory for Recomp.

This is a structural guard, not an assertion of semantic CODE.2 parity.
Only explicitly referenced C++ inputs and source-local .inc fragments are
treated as hard dependencies. Historical research files are never deleted
by this tool; large translation units are reported as refactor candidates.
"""
from __future__ import annotations

import argparse
import json
import re
from collections import Counter
from pathlib import Path

CPP_INPUT = re.compile(r"\b((?:src|tests)/[A-Za-z0-9_./-]+\.cpp|test_[A-Za-z0-9_./-]+\.cpp)\b")
TARGET = re.compile(r"\badd_executable\s*\(\s*([A-Za-z_][A-Za-z_0-9]*)", re.I)
LIBRARY_TARGET = re.compile(r"\badd_library\s*\(\s*([A-Za-z_][A-Za-z_0-9]*)", re.I)
MODULE_INCLUDE = re.compile(r'^\s*include\s*\(\s*["\']?(cmake/[A-Za-z_0-9./-]+\.cmake)["\']?\s*\)', re.I | re.M)
CTEST = re.compile(r"\badd_test\s*\(\s*NAME\s+([A-Za-z_][A-Za-z_0-9]*)", re.I)
FRAGMENT_INCLUDE = re.compile(r'^\s*#\s*include\s+"([^"]+\.inc)"', re.M)


def audit(root: Path) -> dict:
    """Return deterministic errors and warnings without touching source files."""
    root = root.resolve()
    project = root / "Recomp"
    cmake_path = project / "CMakeLists.txt"
    errors: list[str] = []
    warnings: list[str] = []
    if not cmake_path.is_file():
        return {"errors": ["Missing Recomp/CMakeLists.txt"], "warnings": []}
    # Recursively audit repo-local CMake modules; an unreferenced or missing
    # compiled .cpp must remain an error after the build graph is modularized.
    modules_seen: set[Path] = set()
    def read_build(path: Path) -> str:
        source = path.read_text(encoding="utf-8")
        pieces = [source]
        for name in MODULE_INCLUDE.findall(source):
            module = (project / name).resolve()
            if not module.is_relative_to(project.resolve()) or not module.is_file():
                errors.append(f"Missing or escaped CMake module: {name}")
            elif module in modules_seen:
                errors.append(f"Duplicate or cyclic CMake module include: {name}")
            else:
                modules_seen.add(module)
                pieces.append(read_build(module))
        return "\n".join(pieces)
    cmake = read_build(cmake_path)
    cpp_references = set(CPP_INPUT.findall(cmake))
    cpp_files = {
        p.relative_to(project).as_posix()
        for directory in (project / "src", project / "tests")
        if directory.is_dir()
        for p in directory.rglob("*.cpp")
    }
    cpp_files.update(
        p.relative_to(project).as_posix()
        for p in project.glob("*.cpp")
    )
    for filename in sorted(cpp_references - cpp_files):
        errors.append(f"CMake references missing C++ input: {filename}")
    for filename in sorted(cpp_files - cpp_references):
        errors.append(f"Unreferenced compiled C++ input: {filename}")

    for kind, pattern in (("executable target", TARGET),
                          ("library target", LIBRARY_TARGET),
                          ("CTest name", CTEST)):
        seen = Counter(pattern.findall(cmake))
        for name, count in sorted(seen.items()):
            if count > 1:
                errors.append(f"Duplicate {kind}: {name} ({count} declarations)")

    fragments = set((project / "src").glob("*.inc"))
    includes: dict[Path, list[Path]] = {}
    for p in sorted((project / "src").rglob("*")):
        if not p.is_file() or p.suffix not in (".cpp", ".hpp", ".inc", ".h"):
            continue
        contents = p.read_text(encoding="utf-8")
        refs = []
        for name in FRAGMENT_INCLUDE.findall(contents):
            target = (p.parent / name).resolve()
            if not target.is_relative_to(root) or not target.is_file():
                errors.append(f"Missing or escaped compiled fragment: {p.relative_to(root)} -> {name}")
            else:
                refs.append(target)
        includes[p.resolve()] = refs

    seen = set()
    visiting = set()
    def walk(p: Path) -> None:
        if p in visiting:
            errors.append(f"Cyclic compiled fragment include: {p.relative_to(root)}")
            return
        if p in seen:
            return
        visiting.add(p)
        for target in includes.get(p, []):
            walk(target)
        visiting.remove(p)
        seen.add(p)
    for source in sorted(includes):
        walk(source)
    referenced_fragments = {target for references in includes.values() for target in references}
    for fragment in sorted(fragments):
        if fragment.resolve() not in referenced_fragments:
            errors.append(f"Orphan compiled fragment: {fragment.relative_to(root)}")

    for p in sorted((project / "src").glob("*")):
        if p.is_file() and p.suffix in (".cpp", ".hpp", ".inc") and p.stat().st_size > 65536:
            warnings.append(f"Large source module (>64 KiB): {p.relative_to(root)} ({p.stat().st_size} bytes)")
    return {
        "errors": sorted(set(errors)),
        "warnings": warnings,
        "cmake_cpp_inputs": len(cpp_references),
        "compiled_fragments": len(fragments),
        "checked_include_owners": len(includes),
    }


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--root", type=Path, default=Path(__file__).resolve().parents[2])
    args = parser.parse_args()
    result = audit(args.root)
    if not result["errors"]:
        from audit_original_source_callbacks import catalog
        try:
            census = catalog(args.root)
            result["original_source_callback_inventory"] = {
                "canonical_function_address_tokens": census["canonical_function_address_tokens"],
                "canonical_function_bodies": census["canonical_function_bodies"],
                "canonical_direct_call_edges": census["canonical_direct_call_edges"],
                "canonical_callback_abi_occurrences": census["canonical_callback_abi_occurrences"],
                "live_guest_callback_wirings": census["live_guest_callback_wirings"],
                "nullable_wirings": census["conditional_or_explicit_null_wirings"],
                "unresolved_guard_sites": census["unresolved_guard_sites"],
                "dynamic_callgraph": "NOT_CERTIFIED",
            }
        except (OSError, UnicodeError, ValueError) as exc:
            result["errors"].append("Original CODE2 callback census failed: " + str(exc))
    print(json.dumps(result, indent=2))
    return 1 if result["errors"] else 0


if __name__ == "__main__":
    raise SystemExit(main())
