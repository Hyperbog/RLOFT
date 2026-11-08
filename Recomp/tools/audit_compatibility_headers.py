#!/usr/bin/env python3
"""Inventory compatibility forwarding headers without deleting original source.

Companion to audit_recomp_source_layout.py. Only short, direct forwarding
headers in Recomp/src are candidates. An unreferenced file is a deletion
candidate, never automatically a deleted file.
"""
from __future__ import annotations

import argparse
import json
import re
from pathlib import Path

INCLUDE = re.compile(r'^\s*#\s*include\s*["<]([^">]+)[">]', re.M)
SOURCE_EXTENSIONS = {".c", ".cc", ".cpp", ".h", ".hpp", ".inc", ".cxx"}
OTHER_EXTENSIONS = {".py", ".cmake", ".txt", ".sh", ".ps1", ".cmd",
                    ".yml", ".yaml", ".json", ".bat"}
IGNORED_DIRS = {".git", "build", "__pycache__", ".venv", "node_modules"}
MAX_SCAN_BYTES = 2_000_000


def inventory(root: Path) -> dict:
    root = root.resolve()
    src = root / "Recomp" / "src"
    forwarders = {}
    for path in sorted(src.glob("*.hpp")):
        content = path.read_text(encoding="utf-8")
        active = [line.strip() for line in content.splitlines()
                  if line.strip() and not line.lstrip().startswith("//")]
        if (len(active) == 2 and active[0] == "#pragma once"
                and re.fullmatch(r'#include\s+"game/[^"]+\.hpp"', active[1])):
            forwarders[path.name] = {"path": path.relative_to(root).as_posix(),
                                     "target": active[1].split('"')[1],
                                     "source_includes": [], "other_mentions": []}

    tracked = []
    for path in root.rglob("*"):
        if any(part in IGNORED_DIRS for part in path.relative_to(root).parts):
            continue
        if not path.is_file() or path.stat().st_size > MAX_SCAN_BYTES:
            continue
        if (path.suffix.lower() not in SOURCE_EXTENSIONS | OTHER_EXTENSIONS
                and path.name != "CMakeLists.txt"):
            continue
        tracked.append(path)
    for path in sorted(tracked):
        rel = path.relative_to(root).as_posix()
        content = path.read_text(encoding="utf-8", errors="replace")
        if path.suffix.lower() in SOURCE_EXTENSIONS:
            includes = {name.rsplit("/", 1)[-1]
                        for name in INCLUDE.findall(content)}
            for name in includes & forwarders.keys():
                if rel != forwarders[name]["path"]:
                    forwarders[name]["source_includes"].append(rel)
        else:
            for name, item in forwarders.items():
                if name in content and rel != item["path"]:
                    item["other_mentions"].append(rel)
    for item in forwarders.values():
        item["source_includes"].sort()
        item["other_mentions"].sort()
    candidates = [item["path"] for item in forwarders.values()
                  if not item["source_includes"] and not item["other_mentions"]]
    return {"forwarders": [forwarders[name] for name in sorted(forwarders)],
            "proposed_deletion_candidates": candidates,
            "candidate_count": len(candidates),
            "scanned_files": len(tracked),
            "policy": "REPORT_ONLY; deletion requires build and reference review"}


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--root", type=Path,
                        default=Path(__file__).resolve().parents[2])
    args = parser.parse_args()
    result = inventory(args.root)
    print(json.dumps(result, indent=2))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
