"""Runtime path helpers for source and PyInstaller builds."""
from __future__ import annotations

import shutil
import sys
from pathlib import Path


def bundled_path(name: str) -> Path | None:
    """Return a bundled/runtime executable path if available."""
    roots: list[Path] = []
    meipass = getattr(sys, "_MEIPASS", None)
    if meipass:
        roots.append(Path(meipass))
    if getattr(sys, "frozen", False):
        roots.append(Path(sys.executable).resolve().parent)
    roots.append(Path(__file__).resolve().parent)

    for root in roots:
        candidate = root / name
        if candidate.is_file():
            return candidate

    found = shutil.which(name)
    return Path(found) if found else None
