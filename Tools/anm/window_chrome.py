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
        self._resize_hit: int | None = None
        self._resize_origin: tuple[int, int, int, int, int, int] | None = None
        self._move_origin: tuple[int, int, int, int] | None = None
        # Clicking a maximized title bar must not restore the window. Windows
        # restores a maximized window only after the user actually drags it.
        # Keep the initial press pending until pointer motion crosses a small
        # threshold, then restore and continue the drag from that point.
        self._maximized_drag_origin: tuple[int, int, int, int] | None = None

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
        self._bind_resize_edges()

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
        # Tk supplies all resize hit targets itself. Leaving WS_THICKFRAME set
        # makes Windows 11 draw a native top resize strip over the custom skin
        # and keeps a second non-client sizing path alive. Remove both native
        # caption and sizing frame so there is exactly one window border.
        style &= ~(WS_CAPTION | WS_THICKFRAME)
        style |= WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU
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
        # Keep movement entirely in Tk. Entering the native non-client modal
        # loop from a Tk callback can re-enter Tcl/Tk and was still capable of
        # terminating the frozen application on real Windows 11 systems.
        widget.bind("<ButtonPress-1>", self._begin_move, add="+")
        widget.bind("<B1-Motion>", self._perform_move, add="+")
        widget.bind("<ButtonRelease-1>", self._end_move, add="+")
        widget.bind("<Double-Button-1>", lambda _event: self.toggle_maximize(), add="+")

    def _begin_move(self, event) -> str:
        # The titlebar occupies the full client width. Let its extreme edges
        # remain resize targets before treating the press as a window move.
        hit_test = self._edge_hit_from_event(event)
        if hit_test is not None:
            return self._begin_resize(event, hit_test)

        self._move_origin = None
        self._maximized_drag_origin = None
        if self.is_maximized():
            # A plain click on a maximized title bar is not a restore command.
            # Save enough information to restore only if the user begins an
            # actual drag. event.x/y are titlebar-local and preserve the
            # cursor's vertical grab point after restoring.
            self._maximized_drag_origin = (
                int(event.x_root),
                int(event.y_root),
                int(getattr(event, "x", 0)),
                int(getattr(event, "y", 0)),
            )
            return "break"

        self._move_origin = (
            int(event.x_root),
            int(event.y_root),
            self.root.winfo_x(),
            self.root.winfo_y(),
        )
        return "break"

    def _perform_move(self, event) -> str:
        if self._maximized_drag_origin is not None:
            start_x, start_y, local_x, local_y = self._maximized_drag_origin
            dx = int(event.x_root) - start_x
            dy = int(event.y_root) - start_y
            if abs(dx) < 4 and abs(dy) < 4:
                return "break"

            # Restore only after real drag motion. Position the restored window
            # under the pointer before continuing so it behaves like ordinary
            # Windows title-bar dragging rather than jumping to its old origin.
            self.toggle_maximize()
            self.root.update_idletasks()
            width = max(1, self.root.winfo_width())
            # The maximized titlebar may be much wider than the restored
            # window. Preserve the horizontal grab position proportionally,
            # but keep it away from the far edges for predictable movement.
            screen_width = max(1, self.root.winfo_screenwidth())
            ratio = max(0.05, min(0.95, start_x / screen_width))
            grab_x = max(8, min(width - 8, round(width * ratio)))
            grab_y = max(0, min(32, local_y))
            window_x = int(event.x_root) - grab_x
            window_y = int(event.y_root) - grab_y
            self.root.geometry(f"+{window_x}+{window_y}")
            self.root.update_idletasks()
            self._maximized_drag_origin = None
            self._move_origin = (
                int(event.x_root),
                int(event.y_root),
                self.root.winfo_x(),
                self.root.winfo_y(),
            )
            return "break"

        if self._move_origin is None:
            return "break"
        start_x, start_y, window_x, window_y = self._move_origin
        x = window_x + int(event.x_root) - start_x
        y = window_y + int(event.y_root) - start_y
        self.root.geometry(f"+{x}+{y}")
        return "break"

    def _end_move(self, _event=None) -> str:
        self._move_origin = None
        self._maximized_drag_origin = None
        return "break"

    def _begin_resize(self, event, hit_test: int) -> str:
        """Begin a border resize without entering Windows' modal NC loop.

        Calling WM_NCLBUTTONDOWN synchronously from a Tk callback can re-enter
        Tcl/Tk while the native sizing loop owns the thread. Keep resizing in
        Tk instead so border clicks cannot terminate the process.
        """
        if self.is_maximized():
            return "break"

        self.root.update_idletasks()
        self._resize_hit = hit_test
        self._resize_origin = (
            int(event.x_root),
            int(event.y_root),
            self.root.winfo_x(),
            self.root.winfo_y(),
            max(1, self.root.winfo_width()),
            max(1, self.root.winfo_height()),
        )
        try:
            event.widget.grab_set()
        except tk.TclError:
            pass
        return "break"

    @staticmethod
    def _resized_geometry(
        hit_test: int,
        dx: int,
        dy: int,
        x: int,
        y: int,
        width: int,
        height: int,
        min_width: int,
        min_height: int,
    ) -> tuple[int, int, int, int]:
        left = hit_test in (HTLEFT, HTTOPLEFT, HTBOTTOMLEFT)
        right = hit_test in (HTRIGHT, HTTOPRIGHT, HTBOTTOMRIGHT)
        top = hit_test in (HTTOP, HTTOPLEFT, HTTOPRIGHT)
        bottom = hit_test in (HTBOTTOM, HTBOTTOMLEFT, HTBOTTOMRIGHT)

        new_x, new_y = x, y
        new_w, new_h = width, height

        if left:
            proposed = width - dx
            if proposed >= min_width:
                new_w = proposed
                new_x = x + dx
            else:
                new_w = min_width
                new_x = x + (width - min_width)
        elif right:
            new_w = max(min_width, width + dx)

        if top:
            proposed = height - dy
            if proposed >= min_height:
                new_h = proposed
                new_y = y + dy
            else:
                new_h = min_height
                new_y = y + (height - min_height)
        elif bottom:
            new_h = max(min_height, height + dy)

        return new_x, new_y, new_w, new_h

    def _perform_resize(self, event) -> str | None:
        if self._resize_hit is None or self._resize_origin is None:
            return None

        start_x, start_y, x, y, width, height = self._resize_origin
        dx = int(event.x_root) - start_x
        dy = int(event.y_root) - start_y
        min_width, min_height = self.root.minsize()

        new_x, new_y, new_w, new_h = self._resized_geometry(
            self._resize_hit,
            dx,
            dy,
            x,
            y,
            width,
            height,
            max(1, int(min_width)),
            max(1, int(min_height)),
        )
        self.root.geometry(f"{new_w}x{new_h}+{new_x}+{new_y}")
        return "break"

    def _end_resize(self, event=None) -> str | None:
        if self._resize_hit is None and self._resize_origin is None:
            return None
        self._resize_hit = None
        self._resize_origin = None
        widget = getattr(event, "widget", None)
        if widget is not None:
            try:
                widget.grab_release()
            except tk.TclError:
                pass
        return "break"


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

    @staticmethod
    def _edge_hit_test(
        x: int,
        y: int,
        width: int,
        height: int,
        edge: int = 10,
        corner: int = 20,
    ) -> int | None:
        """Return a resize hit code for a point in client coordinates.

        No visible Tk widgets are placed over the decorative frame. The
        logical hit area is intentionally wider than the painted rail so the
        border is practical to grab on high-DPI Windows displays while the
        artwork remains completely unobscured.
        """
        width = max(1, int(width))
        height = max(1, int(height))
        x = int(x)
        y = int(y)
        edge = max(1, int(edge))
        corner = max(edge, int(corner))

        in_left_corner = x < corner
        in_right_corner = x >= width - corner
        in_top_corner = y < corner
        in_bottom_corner = y >= height - corner

        if in_left_corner and in_top_corner and (x < edge or y < edge):
            return HTTOPLEFT
        if in_right_corner and in_top_corner and (x >= width - edge or y < edge):
            return HTTOPRIGHT
        if in_left_corner and in_bottom_corner and (x < edge or y >= height - edge):
            return HTBOTTOMLEFT
        if in_right_corner and in_bottom_corner and (x >= width - edge or y >= height - edge):
            return HTBOTTOMRIGHT

        if x < edge:
            return HTLEFT
        if x >= width - edge:
            return HTRIGHT
        if y < edge:
            return HTTOP
        if y >= height - edge:
            return HTBOTTOM
        return None

    def _edge_hit_from_event(self, event) -> int | None:
        if self.is_maximized():
            return None
        self.root.update_idletasks()
        x = int(event.x_root) - self.root.winfo_rootx()
        y = int(event.y_root) - self.root.winfo_rooty()
        return self._edge_hit_test(
            x,
            y,
            max(1, self.root.winfo_width()),
            max(1, self.root.winfo_height()),
        )

    def _begin_edge_resize(self, event) -> str | None:
        hit_test = self._edge_hit_from_event(event)
        if hit_test is None:
            return None
        return self._begin_resize(event, hit_test)

    def _bind_resize_edges(self) -> None:
        """Keep resize hit targets logical so the artwork stays unobscured."""
        if not self.enabled:
            return
        self.root.bind("<ButtonPress-1>", self._begin_edge_resize, add="+")
        self.root.bind("<B1-Motion>", self._perform_resize, add="+")
        self.root.bind("<ButtonRelease-1>", self._end_resize, add="+")

