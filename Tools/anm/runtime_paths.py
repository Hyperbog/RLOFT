"""Runtime helpers for source and PyInstaller builds."""
from __future__ import annotations

import os
import shutil
import subprocess
import sys
from pathlib import Path


def bundled_path(name: str) -> Path | None:
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


def silent_subprocess_kwargs() -> dict[str, object]:
    """Prevent FFmpeg/helper processes from opening foreground consoles on Windows."""
    if os.name != "nt":
        return {}
    kwargs: dict[str, object] = {}
    creationflags = getattr(subprocess, "CREATE_NO_WINDOW", 0)
    if creationflags:
        kwargs["creationflags"] = creationflags
    startupinfo = subprocess.STARTUPINFO()
    startupinfo.dwFlags |= subprocess.STARTF_USESHOWWINDOW
    startupinfo.wShowWindow = 0
    kwargs["startupinfo"] = startupinfo
    return kwargs
