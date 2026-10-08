#!/usr/bin/env python3
"""Scrollable ttk.Notebook pages with an always-visible action footer.

Tkinter's notebook sizes each tab to the content's requested height. On
smaller windows the bottom controls would be clipped with no scrollbar.
Keeping the content in a canvas viewport and actions outside that viewport
fixes the problem without changing any extraction function.
"""
from __future__ import annotations

import tkinter as tk
from tkinter import ttk


class ScrollableNotebookPage(ttk.Frame):
    def __init__(
        self,
        notebook: ttk.Notebook,
        title: str,
        *,
        background: str,
        padding: int = 14,
    ) -> None:
        super().__init__(notebook)
        self.columnconfigure(0, weight=1)
        self.rowconfigure(0, weight=1)

        self.canvas = tk.Canvas(
            self,
            background=background,
            highlightthickness=0,
            borderwidth=0,
            height=290,
            takefocus=0,
        )
        self.canvas.grid(row=0, column=0, sticky="nsew")
        self.scrollbar = ttk.Scrollbar(
            self, orient="vertical", command=self.canvas.yview
        )
        self.scrollbar.grid(row=0, column=1, sticky="ns")
        self.canvas.configure(yscrollcommand=self.scrollbar.set)

        self.content = ttk.Frame(self.canvas, padding=padding)
        self.content.columnconfigure(0, weight=1)
        self._window_id = self.canvas.create_window(
            (0, 0), window=self.content, anchor="nw"
        )

        self.action_bar = ttk.Frame(self, padding=(14, 7, 14, 8))
        self.action_bar.grid(row=1, column=0, columnspan=2, sticky="ew")
        self.action_bar.columnconfigure(0, weight=1)
        ttk.Separator(self.action_bar, orient="horizontal").grid(
            row=0, column=0, columnspan=2, sticky="ew", pady=(0, 7)
        )

        self.canvas.bind("<Configure>", self._resize_viewport, add="+")
        self.content.bind("<Configure>", self._content_changed, add="+")
        notebook.add(self, text=title)

    def _resize_viewport(self, event) -> None:
        # Width follows the viewport, but height remains natural so content
        # can be scrolled when there is less space than requested.
        self.canvas.itemconfigure(self._window_id, width=max(1, event.width))
        self._refresh_scrollregion()

    def _content_changed(self, _event=None) -> None:
        self._refresh_scrollregion()

    def _refresh_scrollregion(self) -> None:
        bounds = self.canvas.bbox("all")
        if bounds:
            self.canvas.configure(scrollregion=bounds)

    @staticmethod
    def wheel_units(delta: int) -> int:
        # Windows mouse wheels usually report +/-120; high-resolution wheels
        # may emit smaller nonzero deltas, which should still move one unit.
        if delta == 0:
            return 0
        return -max(1, round(abs(delta) / 120)) if delta > 0 else max(
            1, round(abs(delta) / 120)
        )

    def scroll(self, delta: int) -> None:
        units = self.wheel_units(delta)
        if units:
            self.canvas.yview_scroll(units, "units")
