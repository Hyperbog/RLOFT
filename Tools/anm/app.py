#!/usr/bin/env python3
"""Windows/PyInstaller entry point for the Ravenloft Asset Extraction Suite."""
from __future__ import annotations

import os
import subprocess
import sys

import runtime_paths


def _enable_windows_dpi_awareness() -> None:
    if os.name != "nt":
        return
    try:
        import ctypes
        ctypes.windll.shcore.SetProcessDpiAwareness(2)
    except Exception:
        try:
            import ctypes
            ctypes.windll.user32.SetProcessDPIAware()
        except Exception:
            pass


def _self_test() -> int:
    # Imports exercise the modules that the frozen build needs.
    import tkinter  # noqa: F401
    import PIL  # noqa: F401
    import asset_dumper  # noqa: F401
    import exporter  # noqa: F401
    import gui  # noqa: F401
    import master_dumper  # noqa: F401
    import media_inventory  # noqa: F401
    import res_assets  # noqa: F401
    import rloft_anm  # noqa: F401
    import support_dumper  # noqa: F401
    import xmidi  # noqa: F401

    for required in (
        "assets/strahd_frame_main.png",
        "assets/strahd_parchment.png",
        "assets/strahd_title_logo.png",
    ):
        if runtime_paths.bundled_path(required) is None:
            return 20

    ffmpeg = runtime_paths.bundled_path("ffmpeg.exe" if os.name == "nt" else "ffmpeg")
    if ffmpeg is None:
        return 21
    try:
        proc = subprocess.run(
            [str(ffmpeg), "-version"],
            stdout=subprocess.DEVNULL,
            stderr=subprocess.DEVNULL,
            timeout=15,
            **runtime_paths.silent_subprocess_kwargs(),
        )
    except Exception:
        return 22
    return 0 if proc.returncode == 0 else 23


def main() -> int:
    if "--self-test" in sys.argv:
        return _self_test()

    _enable_windows_dpi_awareness()
    import gui
    gui.main()
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
