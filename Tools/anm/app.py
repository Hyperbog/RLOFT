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
    import game_source  # noqa: F401
    import gui  # noqa: F401
    import master_dumper  # noqa: F401
    import media_inventory  # noqa: F401
    import res_assets  # noqa: F401
    import rloft_anm  # noqa: F401
    import support_dumper  # noqa: F401
    import tooltips  # noqa: F401
    import title_skin  # noqa: F401
    import window_chrome  # noqa: F401
    import xmidi  # noqa: F401

    try:
        skin = title_skin.render_window(900, 680, 1.0)
        header = title_skin.render_header(900, 104, 1.0)
        if skin.size != (900, 680) or skin.mode != "RGB":
            return 24
        if header.size != (900, 104) or header.mode != "RGB":
            return 24
        if title_skin.menu_panel().size != (208, 191):
            return 24
        if title_skin.ravenloft_logo().mode != "RGBA":
            return 24
        header = title_skin.render_header(900, 104, 1.0)
        if header.size != (900, 104) or header.mode != "RGB":
            return 24
    except Exception:
        return 24
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


def _gui_self_test() -> int:
    _enable_windows_dpi_awareness()
    try:
        import gui
        import window_chrome
        app = gui.RavenloftExtractorGui()
        app.update_idletasks()
        app.update()
        chrome = getattr(app, "_window_chrome", None)
        if chrome is None:
            app.destroy()
            return 26
        if os.name == "nt" and (not chrome.enabled or not chrome.hwnd):
            app.destroy()
            return 27
        if not hasattr(app, "titlebar") or not hasattr(app, "header_canvas"):
            app.destroy()
            return 28
        if getattr(app, "_edit_menu", None) is None:
            app.destroy()
            return 32

        # Exercise the same Tk resize path used by the decorative right border.
        from types import SimpleNamespace
        start_w = app.winfo_width()
        start_x = app.winfo_rootx()
        start_y = app.winfo_rooty()
        # Resize hit targets are logical edge regions rather than visible
        # overlay widgets so they cannot cover the decorative frame.
        press = SimpleNamespace(
            x_root=start_x + start_w - 1,
            y_root=start_y + 100,
            widget=app,
        )
        chrome._begin_edge_resize(press)
        if chrome._resize_hit != window_chrome.HTRIGHT:
            app.destroy()
            return 31
        motion = SimpleNamespace(
            x_root=press.x_root + 24,
            y_root=press.y_root,
            widget=app,
        )
        chrome._perform_resize(motion)
        app.update_idletasks()
        app.update()
        if app.winfo_width() < start_w:
            chrome._end_resize(motion)
            app.destroy()
            return 30
        chrome._end_resize(motion)

        app.destroy()
        return 0
    except Exception:
        return 29


def main() -> int:
    if "--self-test" in sys.argv:
        return _self_test()
    if "--gui-self-test" in sys.argv:
        return _gui_self_test()

    _enable_windows_dpi_awareness()
    import gui
    gui.main()
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
