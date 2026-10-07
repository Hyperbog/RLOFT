"""Custom frameless Windows chrome for the Ravenloft extractor.

The application draws its own title bar and decorative border. On Windows 11,
this controller removes the stock non-client frame while retaining taskbar
presence, system move/snap behavior, resize handles, minimize, maximize/restore,
and close actions.
"""
from __future__ import annotations

import ctypes
import os
import tkinter as tk


WM_NCLBUTTONDOWN = 0x00A1
HTCAPTION = 2
HTLEFT = 10
HTRIGHT = 11
HTTOP = 12
HTTOPLEFT = 13
HTTOPRIGHT = 14
HTBOTTOM = 15
HTBOTTOMLEFT = 16
HTBOTTOMRIGHT = 17

GWL_STYLE = -16
GWL_EXSTYLE = -20
WS_CAPTION = 0x00C00000
WS_THICKFRAME = 0x00040000
WS_MINIMIZEBOX = 0x00020000
WS_MAXIMIZEBOX = 0x00010000
WS_SYSMENU = 0x00080000
WS_EX_TOOLWINDOW = 0x00000080
WS_EX_APPWINDOW = 0x00040000

SW_MINIMIZE = 6
SW_RESTORE = 9
SW_MAXIMIZE = 3
SWP_NOMOVE = 0x0002
SWP_NOSIZE = 0x0001
SWP_NOZORDER = 0x0004
SWP_FRAMECHANGED = 0x0020
GA_ROOT = 2


class FramelessWindow:
    def __init__(self, root: tk.Tk, border_color: str = "#6a6661") -> None:
        self.root = root
        self.border_color = border_color
        self.enabled = os.name == "nt"
        self.hwnd: int | None = None
        self._handles: list[tk.Frame] = []

        if not self.enabled:
            return

        self.root.update_idletasks()
        self.root.overrideredirect(True)
        self.root.update_idletasks()

        user32 = ctypes.windll.user32
        self.hwnd = int(user32.GetAncestor(self.root.winfo_id(), GA_ROOT))
        if not self.hwnd:
            self.hwnd = int(self.root.winfo_id())

        self._apply_windows_style()
        self._create_resize_handles()
        self.root.bind("<Configure>", self._layout_resize_handles, add="+")

    def _get_window_long(self, index: int) -> int:
        user32 = ctypes.windll.user32
        if ctypes.sizeof(ctypes.c_void_p) == 8:
            user32.GetWindowLongPtrW.restype = ctypes.c_longlong
            return int(user32.GetWindowLongPtrW(self.hwnd, index))
        user32.GetWindowLongW.restype = ctypes.c_long
        return int(user32.GetWindowLongW(self.hwnd, index))

    def _set_window_long(self, index: int, value: int) -> None:
        user32 = ctypes.windll.user32
        if ctypes.sizeof(ctypes.c_void_p) == 8:
            user32.SetWindowLongPtrW(self.hwnd, index, ctypes.c_longlong(value))
        else:
            user32.SetWindowLongW(self.hwnd, index, value)

    def _apply_windows_style(self) -> None:
        if not self.enabled or not self.hwnd:
            return
        style = self._get_window_long(GWL_STYLE)
        style &= ~WS_CAPTION
        style |= WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU
        self._set_window_long(GWL_STYLE, style)

        exstyle = self._get_window_long(GWL_EXSTYLE)
        exstyle &= ~WS_EX_TOOLWINDOW
        exstyle |= WS_EX_APPWINDOW
        self._set_window_long(GWL_EXSTYLE, exstyle)

        ctypes.windll.user32.SetWindowPos(
            self.hwnd,
            0,
            0,
            0,
            0,
            0,
            SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED,
        )

    def bind_drag(self, widget: tk.Misc) -> None:
        widget.bind("<ButtonPress-1>", self._start_move, add="+")
        widget.bind("<Double-Button-1>", lambda _event: self.toggle_maximize(), add="+")

    def _start_move(self, _event=None) -> None:
        if not self.enabled or not self.hwnd:
            return
        if self.is_maximized():
            ctypes.windll.user32.ShowWindow(self.hwnd, SW_RESTORE)
            self.root.update_idletasks()
        ctypes.windll.user32.ReleaseCapture()
        ctypes.windll.user32.SendMessageW(self.hwnd, WM_NCLBUTTONDOWN, HTCAPTION, 0)

    def _start_resize(self, hit_test: int) -> None:
        if not self.enabled or not self.hwnd or self.is_maximized():
            return
        ctypes.windll.user32.ReleaseCapture()
        ctypes.windll.user32.SendMessageW(
            self.hwnd,
            WM_NCLBUTTONDOWN,
            hit_test,
            0,
        )

    def minimize(self) -> None:
        if self.enabled and self.hwnd:
            ctypes.windll.user32.ShowWindow(self.hwnd, SW_MINIMIZE)
        else:
            self.root.iconify()

    def toggle_maximize(self) -> bool:
        if self.enabled and self.hwnd:
            if self.is_maximized():
                ctypes.windll.user32.ShowWindow(self.hwnd, SW_RESTORE)
                maximized = False
            else:
                ctypes.windll.user32.ShowWindow(self.hwnd, SW_MAXIMIZE)
                maximized = True
            self.root.after(20, self._layout_resize_handles)
            return maximized

        if self.root.state() == "zoomed":
            self.root.state("normal")
            return False
        self.root.state("zoomed")
        return True

    def is_maximized(self) -> bool:
        if self.enabled and self.hwnd:
            return bool(ctypes.windll.user32.IsZoomed(self.hwnd))
        return self.root.state() == "zoomed"

    def close(self) -> None:
        self.root.destroy()

    def _create_resize_handles(self) -> None:
        if not self.enabled:
            return

        specs = (
            ("size_we", HTRIGHT),
            ("size_we", HTLEFT),
            ("size_ns", HTTOP),
            ("size_ns", HTBOTTOM),
            ("size_nw_se", HTTOPLEFT),
            ("size_ne_sw", HTTOPRIGHT),
            ("size_ne_sw", HTBOTTOMLEFT),
            ("size_nw_se", HTBOTTOMRIGHT),
        )
        for cursor, hit in specs:
            frame = tk.Frame(
                self.root,
                background=self.border_color,
                cursor=cursor,
                borderwidth=0,
                highlightthickness=0,
            )
            frame.bind(
                "<ButtonPress-1>",
                lambda _event, code=hit: self._start_resize(code),
            )
            self._handles.append(frame)
        self._layout_resize_handles()

    def _layout_resize_handles(self, _event=None) -> None:
        if not self._handles:
            return
        if self.is_maximized():
            for frame in self._handles:
                frame.place_forget()
            return

        w = max(1, self.root.winfo_width())
        h = max(1, self.root.winfo_height())
        edge = 5
        corner = 12

        right, left, top, bottom, tl, tr, bl, br = self._handles
        left.place(x=0, y=corner, width=edge, height=max(1, h - 2 * corner))
        right.place(x=max(0, w - edge), y=corner, width=edge, height=max(1, h - 2 * corner))
        top.place(x=corner, y=0, width=max(1, w - 2 * corner), height=edge)
        bottom.place(x=corner, y=max(0, h - edge), width=max(1, w - 2 * corner), height=edge)
        tl.place(x=0, y=0, width=corner, height=corner)
        tr.place(x=max(0, w - corner), y=0, width=corner, height=corner)
        bl.place(x=0, y=max(0, h - corner), width=corner, height=corner)
        br.place(x=max(0, w - corner), y=max(0, h - corner), width=corner, height=corner)

        for frame in self._handles:
            frame.lift()
