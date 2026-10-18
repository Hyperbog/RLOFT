#!/usr/bin/env python3
"""Check links and stale branch URLs in current, deliberately maintained documents."""
from __future__ import annotations
from pathlib import Path
from urllib.parse import unquote, urlparse
import re
import sys

ROOT = Path(__file__).resolve().parents[2]
MAINTAINED = (
    "README.md", "AGENTS.md", "Decomp/README.md",
    "docs/INDEX.md", "docs/CURRENT_STATUS.md",
    "docs/BRANCH_OWNERSHIP.md", "docs/DOCUMENTATION_POLICY.md",
    "docs/LIVE_STATUS.md",
)
OBSOLETE = re.compile(
    r"https://github\.com/Hyperbog/RLOFT/(?:tree|blob)/"
    r"(?:disassembler-foundation|extractor-wpf-fluent)(?:/|$)"
)
LINK = re.compile(r"(?<!!)\[[^\]\n]+\]\(([^)\n]+)\)")


def check() -> list[str]:
    errors = []
    for relative in MAINTAINED:
        src = ROOT / relative
        if not src.is_file():
            errors.append(f"{relative}: missing maintained document")
            continue
        body = src.read_text(encoding="utf-8")
        if OBSOLETE.search(body):
            errors.append(f"{relative}: links to renamed workstream branch")
        for target in LINK.findall(body):
            value = target.split("#", 1)[0]
            if not value or value.startswith(("https://", "http://", "mailto:")):
                continue
            if " " in value:
                value = value.split(" ", 1)[0]
            dest = (src.parent / unquote(value)).resolve()
            if not dest.is_relative_to(ROOT) or not dest.exists():
                errors.append(f"{relative}: broken relative link → {target}")
    return errors


if __name__ == "__main__":
    errors = check()
    for error in errors:
        print("ERROR:", error)
    if errors:
        sys.exit(1)
    print(f"PASS: {len(MAINTAINED)} maintained documents, local links and renamed refs")
