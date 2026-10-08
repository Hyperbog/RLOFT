#!/usr/bin/env python3
"""Responsive Strahd-themed GUI for the Ravenloft master asset extractor."""
from __future__ import annotations

import queue
import re
import threading
import tkinter as tk
import tkinter.font as tkfont
from pathlib import Path
from tkinter import filedialog, messagebox, ttk

from PIL import Image, ImageTk

import asset_dumper
import edit_menu
import exporter
import game_source
import master_dumper
import media_inventory
import runtime_paths
import scrollable_tabs
import support_dumper
import tooltips
import title_skin
import window_chrome


BG = "#16090b"
PANEL = "#2a1014"
PANEL2 = "#40161a"
FIELD = "#100708"
TEXT = "#ead9bc"
MUTED = "#c0ad91"
GOLD = "#d5962c"
BORDER = "#aaa49c"
ACCENT = "#77141a"
ACCENT_HOT = "#a52422"
TROUGH = "#090506"

GRAPHICS_ARCHIVES = ("RES0", "RES1", "RES2", "RES3", "RES4", "RES7", "RES8", "RES9", "RES11", "RES12")
AUDIO_ARCHIVES = ("RES6", "RES10", "RES11", "RES12")
MUSIC_ARCHIVES = ("RES5", "RES11")
TIMING_MODES = ("Native engine timing", "Custom FPS")

BASE_WIDTH = 1280
BASE_HEIGHT = 900
MIN_UI_SCALE = 0.68
MAX_UI_SCALE = 1.35
MIN_USER_ZOOM = 0.80
MAX_USER_ZOOM = 1.20
BASE_BANNER_HEIGHT = 150

# Keep the GUI timing readout sourced from the same certified table used by
# the exporter so the displayed rates cannot drift from rendered output.
NATIVE_AVG_FPS = exporter.CERTIFIED_NATIVE_AVERAGE_FPS



class RavenloftExtractorGui(tk.Tk):
    def __init__(self) -> None:
        super().__init__()
        self.title("Ravenloft Asset Extraction Suite")
        screen_w = max(800, self.winfo_screenwidth())
        screen_h = max(600, self.winfo_screenheight())
        launch_scale = min(
            1.0,
            max(MIN_UI_SCALE, (screen_w - 80) / BASE_WIDTH),
            max(MIN_UI_SCALE, (screen_h - 120) / BASE_HEIGHT),
        )
        launch_w = max(900, int(BASE_WIDTH * launch_scale))
        launch_h = max(680, int(BASE_HEIGHT * launch_scale))
        self.geometry(f"{launch_w}x{launch_h}")
        self.minsize(max(820, int(850 * launch_scale)), max(520, int(560 * launch_scale)))
        self.resizable(True, True)
        self.configure(background=BG)
        self._resolution_scale = launch_scale
        self._user_zoom = 1.0
        self.ui_zoom_text = tk.StringVar(value="100%")
        self._ui_scale = launch_scale
        self._scale_job: str | None = None
        self._scalable_listboxes: list[tuple[tk.Listbox, int]] = []
        self._padding_widgets: list[tuple[object, tuple[int, ...]]] = []

        self.source_dir = tk.StringVar()
        self.source_status = tk.StringVar(
            value="Choose a Ravenloft install folder, loose media folder, or CD image."
        )
        self._source_cache_input: str | None = None
        self._source_cache_path: Path | None = None
        self.status = tk.StringVar(value="Ready.")
        self.progress_value = tk.DoubleVar(value=0)

        self.graphics_format = tk.StringVar(value="PNG (lossless indexed)")
        self.graphics_palette_mode = tk.StringVar(value=asset_dumper.PALETTE_MODE_AUTOMATIC)
        self.graphics_palette = tk.IntVar(value=0)
        self.graphics_organization = tk.StringVar(
            value=asset_dumper.GRAPHICS_ORGANIZATION_BOTH
        )
        self.graphics_group_npcs = tk.BooleanVar(value=True)
        self.graphics_group_objects = tk.BooleanVar(value=True)
        self.graphics_group_scene = tk.BooleanVar(value=True)
        self.graphics_group_effects = tk.BooleanVar(value=True)
        self.graphics_group_contact_sheets = tk.BooleanVar(value=True)
        self.graphics_group_previews = tk.BooleanVar(value=True)
        self.graphics_group_alias_maps = tk.BooleanVar(value=True)
        self.graphics_indices = tk.BooleanVar(value=True)
        self.graphics_metadata = tk.BooleanVar(value=True)
        self.graphics_masks = tk.BooleanVar(value=False)
        self.graphics_native = tk.BooleanVar(value=False)
        self.graphics_dest = tk.StringVar()

        self.audio_format = tk.StringVar(value="FLAC (lossless)")
        self.audio_native = tk.BooleanVar(value=True)
        self.audio_dest = tk.StringVar()
        self.dump_sampled_var = tk.BooleanVar(value=True)
        self.dump_music_var = tk.BooleanVar(value=True)
        self.music_format = tk.StringVar(value="Both XMI + MIDI")
        self.dump_opl_var = tk.BooleanVar(value=True)
        self.dump_aria_var = tk.BooleanVar(value=True)

        self.cin_frames = tk.BooleanVar(value=True)
        self.cin_frame_format = tk.StringVar(value="PNG")
        self.cin_frames_dest = tk.StringVar()
        self.cin_video = tk.BooleanVar(value=False)
        self.cin_video_format = tk.StringVar(value="MKV (FFV1 lossless)")
        self.cin_video_dest = tk.StringVar()
        self.cin_meta_dest = tk.StringVar()
        self.cin_timing_mode = tk.StringVar(value="Native engine timing")
        self.native_timing_text = tk.StringVar()
        self.cin_fps = tk.StringVar(value="15")
        self.cin_sync_audio = tk.BooleanVar(value=True)
        self.cin_timeline = tk.BooleanVar(value=True)
        self.cin_native = tk.BooleanVar(value=True)

        self.support_dest = tk.StringVar()
        self.support_palettes = tk.BooleanVar(value=True)
        self.support_text = tk.BooleanVar(value=True)
        self.support_opl = tk.BooleanVar(value=True)
        self.support_bnk = tk.BooleanVar(value=True)
        self.support_init = tk.BooleanVar(value=True)
        self.support_sintab = tk.BooleanVar(value=True)

        self.all_dest = tk.StringVar()
        self.all_frames = tk.BooleanVar(value=True)
        self.all_videos = tk.BooleanVar(value=False)
        self.all_support = tk.BooleanVar(value=True)
        self.all_sync_audio = tk.BooleanVar(value=True)

        self._events: queue.Queue[tuple] = queue.Queue()
        self._action_buttons: list[ttk.Button] = []
        self._tab_pages: dict[ttk.Frame, scrollable_tabs.ScrollableNotebookPage] = {}
        self._wrap_labels: list[ttk.Label] = []
        self._skin_photo: ImageTk.PhotoImage | None = None
        self._skin_job: str | None = None
        self._header_photo: ImageTk.PhotoImage | None = None
        self._header_job: str | None = None
        self._last_client_size: tuple[int, int] | None = None
        self._window_chrome: window_chrome.FramelessWindow | None = None
        self._tooltips: list[object] = []
        self._edit_menu: edit_menu.EditMenuController | None = None

        self._build_styles()
        self._build_ui()
        # Mouse wheels over tab content (including ttk labels/frames) scroll
        # the tab viewport; listboxes and dropdowns retain native scrolling.
        self.bind_all("<MouseWheel>", self._on_tab_mousewheel, add="+")
        self.bind_all("<Button-4>", self._on_tab_mousewheel, add="+")
        self.bind_all("<Button-5>", self._on_tab_mousewheel, add="+")
        self._refresh_native_timing_text()
        self._edit_menu = edit_menu.install(self)
        self._tooltips = tooltips.install(self)
        self.bind("<Configure>", self._on_window_configure, add="+")
        self.after(40, self._apply_ui_scale)
        self.after(100, self._poll_events)

    def _build_styles(self) -> None:
        self.style = ttk.Style(self)
        if "clam" in self.style.theme_names():
            self.style.theme_use("clam")

        base = tkfont.nametofont("TkDefaultFont")
        family = base.actual("family")
        self._font_ui = tkfont.Font(self, family=family, size=9)
        self._font_small = tkfont.Font(self, family=family, size=8)
        self._font_title = tkfont.Font(self, family="Georgia", size=18, weight="bold")
        self._font_heading = tkfont.Font(self, family="Georgia", size=10, weight="bold")
        self._apply_style_metrics(self._ui_scale)

        self.option_add("*TCombobox*Listbox.background", FIELD)
        self.option_add("*TCombobox*Listbox.foreground", TEXT)
        self.option_add("*TCombobox*Listbox.selectBackground", ACCENT)
        self.option_add("*TCombobox*Listbox.selectForeground", "#fff4d4")

    def _apply_style_metrics(self, scale: float) -> None:
        normal = max(7, round(9 * scale))
        small = max(7, round(8 * scale))
        title = max(12, round(18 * scale))
        heading = max(8, round(10 * scale))
        self._font_ui.configure(size=normal)
        self._font_small.configure(size=small)
        self._font_title.configure(size=title)
        self._font_heading.configure(size=heading)

        px = lambda value: max(1, round(value * scale))
        s = self.style
        s.configure("TFrame", background=BG)
        s.configure("TLabel", background=BG, foreground=TEXT, font=self._font_ui)
        s.configure(
            "Title.TLabel",
            background=BG,
            foreground=GOLD,
            font=self._font_title,
        )
        s.configure(
            "Subtle.TLabel",
            background=BG,
            foreground=MUTED,
            font=self._font_small,
        )
        s.configure(
            "TLabelframe",
            background=PANEL,
            bordercolor=BORDER,
            lightcolor=BORDER,
            darkcolor="#0d0706",
            relief="solid",
        )
        s.configure(
            "TLabelframe.Label",
            background=PANEL,
            foreground=GOLD,
            font=self._font_heading,
        )
        s.configure(
            "TNotebook",
            background=BG,
            bordercolor=BORDER,
            lightcolor=BORDER,
            darkcolor=BORDER,
            tabmargins=(px(2), px(2), px(2), 0),
        )
        s.configure(
            "TNotebook.Tab",
            background=PANEL2,
            foreground=MUTED,
            bordercolor=BORDER,
            padding=(px(12), px(7)),
            font=self._font_ui,
        )
        s.map(
            "TNotebook.Tab",
            background=[("selected", ACCENT), ("active", PANEL)],
            foreground=[("selected", "#fff2d2"), ("active", GOLD)],
        )
        s.configure(
            "TButton",
            background=PANEL2,
            foreground=GOLD,
            bordercolor=BORDER,
            lightcolor=BORDER,
            darkcolor="#0d0706",
            padding=(px(10), px(6)),
            font=self._font_ui,
        )
        s.map(
            "TButton",
            background=[("pressed", ACCENT), ("active", ACCENT_HOT), ("disabled", PANEL)],
            foreground=[("disabled", "#665c53"), ("pressed", "#fff4d4")],
        )
        s.configure(
            "TCheckbutton",
            background=PANEL,
            foreground=TEXT,
            indicatorcolor=FIELD,
            bordercolor=BORDER,
            font=self._font_ui,
        )
        s.map(
            "TCheckbutton",
            background=[("active", PANEL)],
            foreground=[("active", GOLD), ("disabled", "#665c53")],
            indicatorcolor=[("selected", ACCENT), ("disabled", PANEL2)],
        )
        for name in ("TEntry", "TSpinbox"):
            s.configure(
                name,
                fieldbackground=FIELD,
                foreground=TEXT,
                insertcolor=TEXT,
                bordercolor=BORDER,
                lightcolor=BORDER,
                darkcolor=BORDER,
                font=self._font_ui,
            )
            s.map(
                name,
                fieldbackground=[("disabled", PANEL2)],
                foreground=[("disabled", "#665c53")],
            )
        s.configure(
            "TCombobox",
            fieldbackground=FIELD,
            background=PANEL2,
            foreground=TEXT,
            arrowcolor=GOLD,
            bordercolor=BORDER,
            font=self._font_ui,
        )
        s.map(
            "TCombobox",
            fieldbackground=[("readonly", FIELD), ("disabled", PANEL2)],
            foreground=[("readonly", TEXT), ("disabled", "#665c53")],
            arrowcolor=[("readonly", GOLD)],
        )
        s.configure(
            "Horizontal.TProgressbar",
            background=ACCENT,
            troughcolor=TROUGH,
            bordercolor=BORDER,
            lightcolor=ACCENT_HOT,
            darkcolor=ACCENT,
        )

    def _on_window_configure(self, event) -> None:
        if event.widget is not self:
            return
        width = max(1, event.width)
        height = max(1, event.height)
        size = (width, height)

        # Pure window movement also generates Configure events. Re-rendering
        # the full PIL-backed frame on every move caused visible flashing and
        # stale-frame artifacts. Only layout/skin work when the client size
        # actually changes.
        if size != self._last_client_size:
            self._last_client_size = size
            self._resize_wraps_for_width(width)
            self._fit_listboxes_to_height(height)
            self._schedule_skin()
            self._schedule_header()
        self._sync_maximize_button()

    def _fit_listboxes_to_height(self, window_height: int) -> None:
        if window_height >= 900:
            factor = 1.0
        elif window_height >= 760:
            factor = 0.78
        else:
            factor = 0.55
        for box, base_height in self._scalable_listboxes:
            rows = max(2, min(base_height, round(base_height * factor)))
            if int(box.cget("height")) != rows:
                box.configure(height=rows)

    def _apply_ui_scale(self) -> None:
        scale = self._resolution_scale * self._user_zoom
        scale = max(MIN_UI_SCALE, min(MAX_UI_SCALE, scale))
        changed = abs(scale - self._ui_scale) >= 0.001
        self._ui_scale = scale

        if changed:
            self._apply_style_metrics(scale)
            for box, _base_height in self._scalable_listboxes:
                box.configure(font=self._font_ui)
            for widget, base_padding in self._padding_widgets:
                scaled = tuple(max(0, round(value * scale)) for value in base_padding)
                try:
                    widget.configure(padding=scaled)
                except tk.TclError:
                    pass

        self._apply_chrome_insets()
        if hasattr(self, "header_canvas"):
            self.header_canvas.configure(height=max(78, round(104 * scale)))
        self._resize_wraps_for_width(max(1, self.winfo_width()))
        self._fit_listboxes_to_height(max(1, self.winfo_height()))
        self._schedule_skin()
        self._schedule_header()

    def _set_user_zoom(self, value: float) -> None:
        value = max(MIN_USER_ZOOM, min(MAX_USER_ZOOM, value))
        if abs(value - self._user_zoom) < 0.001:
            return
        self._user_zoom = value
        self.ui_zoom_text.set(f"{round(value * 100):d}%")
        self._apply_ui_scale()

    def _zoom_out(self) -> None:
        self._set_user_zoom(self._user_zoom - 0.10)

    def _zoom_in(self) -> None:
        self._set_user_zoom(self._user_zoom + 0.10)

    def _zoom_reset(self) -> None:
        self._set_user_zoom(1.0)

    def _apply_chrome_insets(self) -> None:
        side, _top, bottom = title_skin.chrome_insets(self._ui_scale)
        if hasattr(self, "titlebar"):
            # The non-client strip is part of the window itself, not the
            # inset content area. Keep its background flush with both edges.
            self.titlebar.grid_configure(padx=0)
        if hasattr(self, "header_canvas"):
            # The crest field is part of the outer skin, not an inset panel.
            # Let it run full-bleed behind the decorative side ornaments.
            self.header_canvas.grid_configure(padx=0)
        if hasattr(self, "body"):
            self.body.grid_configure(padx=(side, side))
        if hasattr(self, "footer"):
            self.footer.grid_configure(padx=(side, side), pady=(0, bottom))

    def _schedule_skin(self) -> None:
        # Coalesce resize bursts without continually pushing the render farther
        # into the future. This keeps the decorative frame updating throughout
        # a live edge drag instead of snapping only after the drag stops.
        if self._skin_job:
            return
        self._skin_job = self.after(16, self._render_skin)

    def _render_skin(self) -> None:
        self._skin_job = None
        width = max(1, self.winfo_width())
        height = max(1, self.winfo_height())
        image = title_skin.render_window(width, height, self._ui_scale)
        self._skin_photo = ImageTk.PhotoImage(image)
        self.skin_canvas.delete("all")
        self.skin_canvas.create_image(0, 0, anchor="nw", image=self._skin_photo)

    def _schedule_header(self) -> None:
        if self._header_job:
            return
        self._header_job = self.after(16, self._render_header)

    def _render_header(self) -> None:
        self._header_job = None
        if not hasattr(self, "header_canvas"):
            return
        width = max(1, self.header_canvas.winfo_width())
        height = max(1, int(float(self.header_canvas.cget("height"))))
        image = title_skin.render_header(width, height, self._ui_scale)
        self._header_photo = ImageTk.PhotoImage(image)
        self.header_canvas.delete("all")
        self.header_canvas.create_image(0, 0, anchor="nw", image=self._header_photo)

    def _toggle_maximize(self) -> None:
        if self._window_chrome is None:
            return
        self._window_chrome.toggle_maximize()
        self.after(30, self._sync_maximize_button)

    def _sync_maximize_button(self) -> None:
        if not hasattr(self, "maximize_button") or self._window_chrome is None:
            return
        self.maximize_button.configure(
            text="❐" if self._window_chrome.is_maximized() else "□"
        )

    def _build_ui(self) -> None:
        self.columnconfigure(0, weight=1)
        self.rowconfigure(2, weight=1)

        # The skin canvas is the application's actual decorative frame.
        self.skin_canvas = tk.Canvas(
            self,
            background=BG,
            highlightthickness=0,
            borderwidth=0,
        )
        self.skin_canvas.place(x=0, y=0, relwidth=1, relheight=1)

        # Custom non-client title bar. The stock Windows title bar is removed
        # after these controls are created.
        self.titlebar = tk.Frame(
            self,
            background="#210d10",
            height=34,
            borderwidth=0,
            highlightthickness=0,
        )
        self.titlebar.grid(row=0, column=0, sticky="ew")
        self.titlebar.grid_propagate(False)
        self.titlebar.columnconfigure(1, weight=1)

        self.titlebar_title = tk.Label(
            self.titlebar,
            text="Ravenloft Asset Extraction Suite",
            background="#210d10",
            foreground=GOLD,
            font=self._font_heading,
        )
        self.titlebar_title.grid(row=0, column=0, sticky="w", padx=(10, 6))

        chrome_buttons = tk.Frame(self.titlebar, background="#210d10")
        chrome_buttons.grid(row=0, column=3, sticky="e", padx=(0, 10))
        self.minimize_button = tk.Button(
            chrome_buttons,
            text="—",
            command=lambda: self._window_chrome and self._window_chrome.minimize(),
            background="#35161a",
            foreground=GOLD,
            activebackground="#5a2025",
            activeforeground="#fff3d0",
            relief="flat",
            borderwidth=1,
            width=4,
            font=self._font_ui,
        )
        self.minimize_button.pack(side="left", padx=(0, 2))
        self.maximize_button = tk.Button(
            chrome_buttons,
            text="□",
            command=self._toggle_maximize,
            background="#35161a",
            foreground=GOLD,
            activebackground="#5a2025",
            activeforeground="#fff3d0",
            relief="flat",
            borderwidth=1,
            width=4,
            font=self._font_ui,
        )
        self.maximize_button.pack(side="left", padx=2)
        self.close_button = tk.Button(
            chrome_buttons,
            text="×",
            command=lambda: self._window_chrome.close() if self._window_chrome else self.destroy(),
            background="#5a171d",
            foreground="#f0dfc3",
            activebackground="#8a2029",
            activeforeground="#ffffff",
            relief="flat",
            borderwidth=1,
            width=4,
            font=self._font_ui,
        )
        self.close_button.pack(side="left", padx=(2, 0))

        # Ravenloft crest/banner, distinct from the Strahd's Possession menu
        # plaque used elsewhere in the original title screen.
        self.header_canvas = tk.Canvas(
            self,
            height=max(78, round(104 * self._ui_scale)),
            background=BG,
            highlightthickness=0,
            borderwidth=0,
        )
        self.header_canvas.grid(row=1, column=0, sticky="ew")

        self.body = ttk.Frame(self, padding=(14, 8, 14, 8))
        self.body.grid(row=2, column=0, sticky="nsew")
        self._padding_widgets.append((self.body, (14, 8, 14, 8)))
        self.body.columnconfigure(0, weight=1)
        self.body.rowconfigure(1, weight=1)
        self.body.bind("<Configure>", self._resize_wraps)

        source = ttk.Frame(self.body, padding=(2, 2, 2, 10))
        source.grid(row=0, column=0, sticky="ew")
        source.columnconfigure(1, weight=1)
        ttk.Label(source, text="Game / media source").grid(
            row=0, column=0, sticky="w", padx=(0, 8)
        )
        ttk.Entry(source, textvariable=self.source_dir).grid(
            row=0, column=1, sticky="ew"
        )
        ttk.Button(
            source, text="Folder…", command=self._choose_source_dir
        ).grid(row=0, column=2, padx=(8, 0))
        ttk.Button(
            source, text="CD image…", command=self._choose_source_image
        ).grid(row=0, column=3, padx=(6, 0))
        validate = ttk.Button(
            source, text="Scan / Validate", command=self._validate_source
        )
        validate.grid(row=0, column=4, padx=(8, 0))
        self._action_buttons.append(validate)
        ttk.Label(
            source,
            textvariable=self.source_status,
            style="Subtle.TLabel",
        ).grid(row=1, column=0, columnspan=5, sticky="ew", pady=(7, 0))

        self.notebook = ttk.Notebook(self.body)
        self.notebook.grid(row=1, column=0, sticky="nsew")
        self._build_cinematics_tab()
        self._build_graphics_tab()
        self._build_audio_tab()
        self._build_support_tab()
        self._build_all_tab()

        self.footer = ttk.Frame(self, padding=(14, 0, 14, 12))
        self.footer.grid(row=3, column=0, sticky="ew")
        self._padding_widgets.append((self.footer, (14, 0, 14, 12)))
        self.footer.columnconfigure(0, weight=1)
        self.footer.columnconfigure(1, weight=0)
        ttk.Progressbar(
            self.footer,
            variable=self.progress_value,
            maximum=100,
        ).grid(row=0, column=0, columnspan=2, sticky="ew")

        ttk.Label(
            self.footer,
            textvariable=self.status,
            style="Subtle.TLabel",
        ).grid(row=1, column=0, sticky="w", pady=(6, 0))

        # Keep UI-size controls out of the title bar so the custom chrome stays
        # visually clean.  They live with the other status controls instead.
        zoom = ttk.Frame(self.footer)
        zoom.grid(row=1, column=1, sticky="e", padx=(12, 0), pady=(4, 0))
        ttk.Label(zoom, text="UI size").pack(side="left", padx=(0, 6))
        ttk.Button(zoom, text="−", width=3, command=self._zoom_out).pack(side="left")
        ttk.Button(
            zoom,
            textvariable=self.ui_zoom_text,
            width=6,
            command=self._zoom_reset,
        ).pack(side="left", padx=4)
        ttk.Button(zoom, text="+", width=3, command=self._zoom_in).pack(side="left")

        self._apply_chrome_insets()
        self.after(10, self._schedule_skin)
        self.after(10, self._schedule_header)

        self._window_chrome = window_chrome.FramelessWindow(self, border_color="#5e5c59")
        self._window_chrome.bind_drag(self.titlebar)
        self._window_chrome.bind_drag(self.titlebar_title)
        self.after(30, self._sync_maximize_button)

    def _new_tab(self, title: str) -> ttk.Frame:
        page = scrollable_tabs.ScrollableNotebookPage(
            self.notebook, title, background=BG, padding=14
        )
        tab = page.content
        self._tab_pages[tab] = page
        self._padding_widgets.append((tab, (14,)))
        return tab

    def _add_tab_action(self, tab: ttk.Frame, title: str, command) -> None:
        # The extraction control remains visible while the settings scroll.
        # This is critical at 720p, with Windows display scaling, and when
        # resizing the window smaller than the tab's natural content height.
        footer = self._tab_pages[tab].action_bar
        button = ttk.Button(footer, text=title, command=command)
        button.grid(row=1, column=1, sticky="e", pady=(0, 2))
        self._action_buttons.append(button)

    def _on_tab_mousewheel(self, event) -> str | None:
        widget = getattr(event, "widget", None)
        if widget is None:
            return None
        # Do not hijack mouse-wheel selections in resource lists or dropdowns.
        if isinstance(widget, (tk.Listbox, tk.Text, ttk.Combobox)):
            return None
        current = widget
        while current is not None:
            if isinstance(current, scrollable_tabs.ScrollableNotebookPage):
                if getattr(event, "num", None) == 4:
                    delta = 120
                elif getattr(event, "num", None) == 5:
                    delta = -120
                else:
                    delta = int(getattr(event, "delta", 0))
                current.scroll(delta)
                return "break"
            current = getattr(current, "master", None)
        return None

    def _folder_row(self, parent, row: int, label: str, variable: tk.StringVar) -> None:
        parent.columnconfigure(1, weight=1)
        ttk.Label(parent, text=label).grid(row=row, column=0, sticky="w", padx=(0, 8), pady=5)
        ttk.Entry(parent, textvariable=variable).grid(row=row, column=1, sticky="ew", pady=5)
        ttk.Button(parent, text="Browse…", command=lambda: self._choose_dir(variable)).grid(row=row, column=2, padx=(8, 0), pady=5)

    def _note(self, parent, row: int, text: str) -> None:
        label = ttk.Label(parent, text=text, justify="left", wraplength=920)
        label.grid(row=row, column=0, sticky="ew", pady=(10, 0))
        self._wrap_labels.append(label)

    def _listbox(self, parent, values: tuple[str, ...], height: int = 6) -> tk.Listbox:
        box = tk.Listbox(
            parent,
            selectmode=tk.EXTENDED,
            exportselection=False,
            height=max(2, round(height * self._ui_scale)),
            background=FIELD,
            foreground=TEXT,
            selectbackground=ACCENT,
            selectforeground="#fff4d4",
            highlightbackground=BORDER,
            highlightcolor=GOLD,
            relief="solid",
            borderwidth=1,
            activestyle="none",
            font=self._font_ui,
        )
        self._scalable_listboxes.append((box, height))
        for value in values:
            box.insert(tk.END, value)
        box.selection_set(0, tk.END)
        return box

    @staticmethod
    def _list_values(box: tk.Listbox) -> tuple[str, ...]:
        return tuple(box.get(i) for i in box.curselection())

    def _selection_buttons(self, parent, row: int, column: int, box: tk.Listbox) -> None:
        buttons = ttk.Frame(parent)
        buttons.grid(row=row, column=column, sticky="w", pady=(5, 0))
        ttk.Button(buttons, text="All", command=lambda: box.selection_set(0, tk.END)).pack(side="left")
        ttk.Button(buttons, text="None", command=lambda: box.selection_clear(0, tk.END)).pack(side="left", padx=(6, 0))

    def _build_cinematics_tab(self) -> None:
        tab = self._new_tab("Cinematics")
        select = ttk.LabelFrame(tab, text="Cinematic files", padding=10)
        select.grid(row=0, column=0, sticky="ew")
        select.columnconfigure(1, weight=1)
        ttk.Label(select, text="Select one, several, or all").grid(row=0, column=0, sticky="nw", padx=(0, 10))
        self.cin_list = self._listbox(select, media_inventory.ANM_NAMES, 7)
        self.cin_list.grid(row=0, column=1, sticky="ew")
        self.cin_list.bind("<<ListboxSelect>>", lambda _e: self._refresh_native_timing_text())
        self._selection_buttons(select, 1, 1, self.cin_list)

        out = ttk.LabelFrame(tab, text="Outputs", padding=10)
        out.grid(row=1, column=0, sticky="ew", pady=(10, 0))
        out.columnconfigure(1, weight=1)
        ttk.Checkbutton(out, text="Individual frames", variable=self.cin_frames).grid(row=0, column=0, sticky="w")
        ttk.Combobox(out, textvariable=self.cin_frame_format, values=tuple(exporter.FRAME_FORMATS), state="readonly", width=22).grid(row=0, column=1, sticky="w", padx=8)
        self._folder_row(out, 1, "Frame destination", self.cin_frames_dest)
        ttk.Checkbutton(
            out,
            text="Rendered video",
            variable=self.cin_video,
            command=self._update_video_state,
        ).grid(row=2, column=0, sticky="w", pady=(6, 0))
        ttk.Combobox(out, textvariable=self.cin_video_format, values=tuple(exporter.VIDEO_FORMATS), state="readonly", width=24).grid(row=2, column=1, sticky="w", padx=8, pady=(6, 0))
        self._folder_row(out, 3, "Video destination", self.cin_video_dest)
        self._folder_row(out, 4, "Metadata/original destination", self.cin_meta_dest)

        timing = ttk.Frame(out)
        timing.grid(row=5, column=0, columnspan=3, sticky="ew", pady=(7, 0))
        timing.columnconfigure(1, weight=1)
        ttk.Label(timing, text="Timing").grid(row=0, column=0, sticky="w")
        combo = ttk.Combobox(
            timing,
            textvariable=self.cin_timing_mode,
            values=TIMING_MODES,
            state="readonly",
            width=22,
        )
        combo.grid(row=0, column=1, sticky="w", padx=(6, 12))
        combo.bind("<<ComboboxSelected>>", lambda _e: self._update_timing_state())
        ttk.Label(timing, text="Custom FPS").grid(row=0, column=2, sticky="w")
        self.cin_fps_entry = ttk.Entry(timing, textvariable=self.cin_fps, width=8)
        self.cin_fps_entry.grid(row=0, column=3, sticky="w", padx=(6, 0))
        self.cin_fps_entry.bind(
            "<KeyRelease>",
            lambda _e: self._refresh_native_timing_text(),
            add="+",
        )
        self.cin_sync_check = ttk.Checkbutton(
            timing,
            text="Sync sampled SFX into video",
            variable=self.cin_sync_audio,
        )
        self.cin_sync_check.grid(
            row=1,
            column=0,
            columnspan=4,
            sticky="w",
            pady=(6, 0),
        )

        self.native_timing_label = ttk.Label(
            out,
            textvariable=self.native_timing_text,
            style="Subtle.TLabel",
            justify="left",
            anchor="w",
        )
        self.native_timing_label.grid(
            row=6,
            column=0,
            columnspan=3,
            sticky="ew",
            pady=(7, 0),
        )
        self._wrap_labels.append(self.native_timing_label)

        flags = ttk.Frame(out)
        flags.grid(row=7, column=0, columnspan=3, sticky="w", pady=(7, 0))
        ttk.Checkbutton(flags, text="Timeline JSON", variable=self.cin_timeline).pack(side="left")
        ttk.Checkbutton(flags, text="Preserve original ANM", variable=self.cin_native).pack(side="left", padx=(16, 0))

        self._note(tab, 2, "Native timing is variable-rate, not one fixed FPS. Sampled ANM SFX commands are muxed from RES6/RES10 at their reconstructed frame timestamps. Timeline/original files use the metadata destination above.")
        self._add_tab_action(tab, "Extract Selected Cinematics", self._start_cinematics)
        self._update_timing_state()
        self._update_video_state()

    def _build_graphics_tab(self) -> None:
        tab = self._new_tab("Graphics & Textures")
        box = ttk.LabelFrame(tab, text="Visual resource archives", padding=10)
        box.grid(row=0, column=0, sticky="ew")
        box.columnconfigure(1, weight=1)
        ttk.Label(box, text="Files").grid(row=0, column=0, sticky="nw", padx=(0, 10))
        self.graphics_list = self._listbox(box, GRAPHICS_ARCHIVES, 6)
        self.graphics_list.grid(row=0, column=1, sticky="ew")
        self._selection_buttons(box, 1, 1, self.graphics_list)
        ttk.Label(box, text="Image format").grid(row=2, column=0, sticky="w", pady=(9, 0))
        ttk.Combobox(box, textvariable=self.graphics_format, values=tuple(asset_dumper.GRAPHICS_FORMATS), state="readonly", width=28).grid(row=2, column=1, sticky="w", padx=8, pady=(9, 0))
        ttk.Label(box, text="Palette").grid(row=3, column=0, sticky="w", pady=(8, 0))
        palette_controls = ttk.Frame(box)
        palette_controls.grid(row=3, column=1, sticky="w", padx=8, pady=(8, 0))
        self.graphics_palette_mode_combo = ttk.Combobox(
            palette_controls,
            textvariable=self.graphics_palette_mode,
            values=asset_dumper.PALETTE_MODES,
            state="readonly",
            width=24,
        )
        self.graphics_palette_mode_combo.pack(side="left")
        self.graphics_palette_mode_combo.bind(
            "<<ComboboxSelected>>", self._update_palette_state
        )
        ttk.Label(palette_controls, text="Manual bank").pack(side="left", padx=(12, 5))
        self.graphics_palette_spin = ttk.Spinbox(
            palette_controls,
            from_=0,
            to=14,
            textvariable=self.graphics_palette,
            width=5,
        )
        self.graphics_palette_spin.pack(side="left")
        self._update_palette_state()

        extras = ttk.LabelFrame(box, text="Associated files", padding=8)
        extras.grid(row=4, column=0, columnspan=3, sticky="ew", pady=(9, 2))
        ttk.Checkbutton(
            extras,
            text="Canonical palette-index bytes (.indices)",
            variable=self.graphics_indices,
        ).grid(row=0, column=0, sticky="w")
        ttk.Checkbutton(
            extras,
            text="Metadata / compiled-shape data",
            variable=self.graphics_metadata,
        ).grid(row=0, column=1, sticky="w", padx=(18, 0))
        ttk.Checkbutton(
            extras,
            text="Transparency mask PNGs",
            variable=self.graphics_masks,
        ).grid(row=1, column=0, sticky="w", pady=(5, 0))
        ttk.Checkbutton(
            extras,
            text="Native RES/EH records",
            variable=self.graphics_native,
        ).grid(row=1, column=1, sticky="w", padx=(18, 0), pady=(5, 0))

        ttk.Label(box, text="Organization").grid(
            row=5, column=0, sticky="w", pady=(9, 0)
        )
        ttk.Combobox(
            box,
            textvariable=self.graphics_organization,
            values=asset_dumper.GRAPHICS_ORGANIZATIONS,
            state="readonly",
            width=30,
        ).grid(row=5, column=1, sticky="w", padx=8, pady=(9, 0))

        grouping = ttk.LabelFrame(box, text="Multipart grouping", padding=8)
        grouping.grid(row=6, column=0, columnspan=3, sticky="ew", pady=(9, 2))
        ttk.Checkbutton(
            grouping, text="NPCs / creatures", variable=self.graphics_group_npcs
        ).grid(row=0, column=0, sticky="w")
        ttk.Checkbutton(
            grouping, text="Objects / items", variable=self.graphics_group_objects
        ).grid(row=0, column=1, sticky="w", padx=(18, 0))
        ttk.Checkbutton(
            grouping, text="INIT scene descriptors", variable=self.graphics_group_scene
        ).grid(row=0, column=2, sticky="w", padx=(18, 0))
        ttk.Checkbutton(
            grouping, text="Effects / spells", variable=self.graphics_group_effects
        ).grid(row=0, column=3, sticky="w", padx=(18, 0))
        ttk.Checkbutton(
            grouping, text="Contact sheets", variable=self.graphics_group_contact_sheets
        ).grid(row=1, column=0, sticky="w", pady=(5, 0))
        ttk.Checkbutton(
            grouping, text="Animated previews", variable=self.graphics_group_previews
        ).grid(row=1, column=1, sticky="w", padx=(18, 0), pady=(5, 0))
        ttk.Checkbutton(
            grouping, text="Shared-resource maps", variable=self.graphics_group_alias_maps
        ).grid(row=1, column=2, sticky="w", padx=(18, 0), pady=(5, 0))

        self._folder_row(box, 7, "Destination", self.graphics_dest)
        self._note(
            tab,
            1,
            "Automatic palette routing is recommended. Multipart grouping follows NPCS.DAT, OBJECTS.DAT, INIT, and the reconstructed RES3 renderer rather than image similarity. Each group receives a manifest; optional contact sheets, GIF previews, and shared-resource maps are generated for modding/upscaling. NPC/object grouping is skipped with a manifest warning if those DAT files are not beside the selected resources.",
        )
        self._add_tab_action(tab, "Extract Selected Graphics", self._start_graphics)

    def _build_audio_tab(self) -> None:
        tab = self._new_tab("Audio & Music")
        box = ttk.LabelFrame(tab, text="Audio source files", padding=10)
        box.grid(row=0, column=0, sticky="ew")
        box.columnconfigure(1, weight=1)
        box.columnconfigure(3, weight=1)

        ttk.Checkbutton(box, text="Sampled audio", variable=self.dump_sampled_var).grid(row=0, column=0, sticky="nw")
        self.audio_list = self._listbox(box, AUDIO_ARCHIVES, 4)
        self.audio_list.grid(row=0, column=1, sticky="ew", padx=(8, 18))
        self._selection_buttons(box, 1, 1, self.audio_list)

        ttk.Checkbutton(box, text="XMIDI music", variable=self.dump_music_var).grid(row=0, column=2, sticky="nw")
        self.music_list = self._listbox(box, MUSIC_ARCHIVES, 4)
        self.music_list.grid(row=0, column=3, sticky="ew", padx=(8, 0))
        self._selection_buttons(box, 1, 3, self.music_list)

        ttk.Label(box, text="Sample format").grid(row=2, column=0, sticky="w", pady=(10, 0))
        ttk.Combobox(box, textvariable=self.audio_format, values=tuple(asset_dumper.AUDIO_FORMATS), state="readonly", width=24).grid(row=2, column=1, sticky="w", padx=8, pady=(10, 0))
        ttk.Label(box, text="Music format").grid(row=2, column=2, sticky="w", pady=(10, 0))
        ttk.Combobox(box, textvariable=self.music_format, values=asset_dumper.MUSIC_FORMATS, state="readonly", width=32).grid(row=2, column=3, sticky="w", padx=8, pady=(10, 0))
        ttk.Checkbutton(box, text="Preserve original VOC/XMI and native EH carriers", variable=self.audio_native).grid(row=3, column=0, columnspan=4, sticky="w", pady=(8, 0))
        ttk.Checkbutton(box, text="SAMPLE.AD OPL2 timbres", variable=self.dump_opl_var).grid(row=4, column=0, columnspan=2, sticky="w", pady=(6, 0))
        ttk.Checkbutton(box, text="GM1.BNK / GM2.BNK ARIA banks", variable=self.dump_aria_var).grid(row=4, column=2, columnspan=2, sticky="w", pady=(6, 0))
        self._folder_row(box, 5, "Destination", self.audio_dest)
        self._note(tab, 1, "Select individual RES audio/music files or All for the complete category.")
        self._add_tab_action(tab, "Extract Selected Audio & Music", self._start_audio)

    def _build_support_tab(self) -> None:
        tab = self._new_tab("Support Assets")
        box = ttk.LabelFrame(tab, text="Standalone support files", padding=10)
        box.grid(row=0, column=0, sticky="ew")
        options = (
            ("COLORS — 15 VGA palette banks", self.support_palettes),
            ("T — decoded text/subtitle records", self.support_text),
            ("SAMPLE.AD — OPL2 timbres", self.support_opl),
            ("GM1.BNK / GM2.BNK — ARIA banks", self.support_bnk),
            ("INIT — resource namespace mappings", self.support_init),
            ("SINTAB — renderer tables / scale blocks", self.support_sintab),
        )
        for row, (text, variable) in enumerate(options):
            ttk.Checkbutton(box, text=text, variable=variable).grid(row=row, column=0, sticky="w", pady=3)
        self._folder_row(box, len(options), "Destination", self.support_dest)
        self._add_tab_action(tab, "Extract Selected Support Assets", self._start_support)

    def _build_all_tab(self) -> None:
        tab = self._new_tab("Dump All")
        box = ttk.LabelFrame(tab, text="Complete media dump", padding=10)
        box.grid(row=0, column=0, sticky="ew")
        desc = ttk.Label(box, text="Exports every known graphic, texture, font, compiled shape, sampled sound, XMIDI sequence, cinematic, palette, subtitle/text record, OPL2 timbre, renderer support package, and ARIA bank.", justify="left", wraplength=920)
        desc.grid(row=0, column=0, columnspan=3, sticky="ew")
        self._wrap_labels.append(desc)
        ttk.Checkbutton(box, text="Cinematic frame sequences", variable=self.all_frames).grid(row=1, column=0, sticky="w", pady=(8, 0))
        ttk.Checkbutton(box, text="Also render lossless FFV1/MKV movies", variable=self.all_videos).grid(row=2, column=0, sticky="w", pady=(5, 0))
        ttk.Checkbutton(box, text="Sync sampled cinematic SFX", variable=self.all_sync_audio).grid(row=3, column=0, sticky="w", pady=(5, 0))
        ttk.Checkbutton(box, text="Standalone support assets", variable=self.all_support).grid(row=4, column=0, sticky="w", pady=(5, 0))
        self._folder_row(box, 5, "Destination", self.all_dest)
        self._add_tab_action(tab, "Dump Everything", self._start_all)

    def _resize_wraps_for_width(self, window_width: int) -> None:
        # Keep explanatory text inside the current client area. The value is
        # intentionally conservative because the notebook/tab/frame borders
        # consume some horizontal space.
        width = max(260, window_width - round(120 * self._ui_scale))
        for label in self._wrap_labels:
            label.configure(wraplength=width)

    def _resize_wraps(self, event) -> None:
        self._resize_wraps_for_width(event.width)

    def _update_palette_state(self, _event=None) -> None:
        if not hasattr(self, "graphics_palette_spin"):
            return
        manual = self.graphics_palette_mode.get() == asset_dumper.PALETTE_MODE_MANUAL
        self.graphics_palette_spin.configure(state="normal" if manual else "disabled")

    def _graphics_palette_index(self) -> int | None:
        if self.graphics_palette_mode.get() == asset_dumper.PALETTE_MODE_AUTOMATIC:
            return None
        value = int(self.graphics_palette.get())
        if not 0 <= value <= 14:
            raise RuntimeError("Manual palette bank must be between 0 and 14.")
        return value

    def _graphics_group_families(self) -> set[str]:
        selected: set[str] = set()
        if self.graphics_group_npcs.get():
            selected.add("npcs")
        if self.graphics_group_objects.get():
            selected.add("objects")
        if self.graphics_group_scene.get():
            selected.add("scene_descriptors")
        if self.graphics_group_effects.get():
            selected.add("effects")
        if (
            self.graphics_organization.get() != asset_dumper.GRAPHICS_ORGANIZATION_RAW
            and not selected
        ):
            raise RuntimeError("Enable at least one multipart grouping family.")
        return selected

    def _update_timing_state(self) -> None:
        self.cin_fps_entry.configure(
            state="normal" if self.cin_timing_mode.get() == "Custom FPS" else "disabled"
        )
        self._refresh_native_timing_text()

    def _update_video_state(self) -> None:
        # Keep the preference editable even when video output is currently
        # disabled. It takes effect whenever Rendered video is enabled.
        self.cin_sync_check.configure(state="normal")

    def _refresh_native_timing_text(self) -> None:
        timer = exporter.PIT_HZ
        ordinary = timer / exporter.ORDINARY_IMAGE_WAIT
        cine12 = timer / exporter.CINE12_IMAGE_WAIT
        selected = tuple(self._list_values(self.cin_list)) if hasattr(self, "cin_list") else ()

        if self.cin_timing_mode.get() == "Custom FPS":
            try:
                custom = float(self.cin_fps.get())
                self.native_timing_text.set(
                    f"Custom presentation timing: constant {custom:.4f} FPS."
                )
            except ValueError:
                self.native_timing_text.set(
                    "Custom presentation timing: enter a valid FPS."
                )
            return

        averages = [NATIVE_AVG_FPS[name] for name in selected if name in NATIVE_AVG_FPS]
        if len(selected) == 1 and averages:
            name = selected[0]
            image_rate = cine12 if name == "CINE12.ANM" else ordinary
            self.native_timing_text.set(
                f"Native VFR — {name}: average displayed-frame rate "
                f"{averages[0]:.4f} FPS; image cadence {image_rate:.4f} FPS; "
                f"engine timer {timer:.4f} Hz."
            )
            return

        if averages:
            avg_text = (
                f"selected average displayed rates {min(averages):.4f}–"
                f"{max(averages):.4f} FPS"
            )
        else:
            avg_text = "select a cinematic to show its average displayed-frame rate"

        if "CINE12.ANM" in selected or not selected:
            cadence = (
                f"image cadence {ordinary:.4f} FPS (CINE00–11), "
                f"{cine12:.4f} FPS (CINE12)"
            )
        else:
            cadence = f"image cadence {ordinary:.4f} FPS"

        self.native_timing_text.set(
            f"Native VFR — {avg_text}; {cadence}; engine timer {timer:.4f} Hz. "
            "One-tick command frames run at the timer rate; zero-wait command "
            "frames carry state without adding display time."
        )

    def _reset_source_cache(self) -> None:
        self._source_cache_input = None
        self._source_cache_path = None

    def _choose_source_dir(self) -> None:
        value = filedialog.askdirectory(
            title="Choose Ravenloft install or loose media folder"
        )
        if value:
            self.source_dir.set(value)
            self._reset_source_cache()
            self._auto_destinations(Path(value))
            self._validate_source()

    def _choose_source_image(self) -> None:
        value = filedialog.askopenfilename(
            title="Choose Ravenloft CD image",
            filetypes=(
                ("CD images", "*.gog *.iso *.bin *.img"),
                ("GOG CD image", "*.gog"),
                ("ISO image", "*.iso"),
                ("Raw BIN image", "*.bin"),
                ("All files", "*.*"),
            ),
        )
        if value:
            self.source_dir.set(value)
            self._reset_source_cache()
            self._auto_destinations(Path(value))
            self._validate_source()

    @staticmethod
    def _choose_dir(variable: tk.StringVar) -> None:
        value = filedialog.askdirectory(title="Choose output folder")
        if value:
            variable.set(value)

    def _auto_destinations(self, root: Path) -> None:
        root = Path(root)
        if root.is_file():
            root = root.parent
        base = root / "RLOFT_DUMP"
        for variable, path in (
            (self.graphics_dest, base / "graphics"),
            (self.audio_dest, base / "audio_music"),
            (self.support_dest, base / "support"),
            (self.cin_frames_dest, base / "cinematics" / "frames"),
            (self.cin_video_dest, base / "cinematics" / "video"),
            (self.cin_meta_dest, base / "cinematics"),
            (self.all_dest, base),
        ):
            if not variable.get():
                variable.set(str(path))

    def _source_input_path(self) -> Path:
        raw = self.source_dir.get().strip()
        if not raw:
            raise RuntimeError(
                "Choose a Ravenloft install folder, loose media folder, or CD image."
            )
        path = Path(raw).expanduser()
        if not path.exists():
            raise RuntimeError(f"Game/media source does not exist: {path}")
        return path

    def _source_path(self) -> Path:
        selected = self._source_input_path()
        cache_key = str(selected.resolve())
        if (
            self._source_cache_input == cache_key
            and self._source_cache_path is not None
            and self._source_cache_path.is_dir()
        ):
            return self._source_cache_path

        prepared = game_source.prepare_source_dir(selected)
        self._source_cache_input = cache_key
        self._source_cache_path = prepared
        return prepared

    def _validate_source(self) -> None:
        try:
            selected = self._source_input_path()
            result = game_source.validate_source(selected, verify_hashes=True)
            found = len(result["present"])
            total = int(result["expected"])
            mismatched = len(result["mismatched"])
            missing = len(result["missing"])
            kind = str(result["kind"])
            if result.get("image_path"):
                kind += f" ({Path(str(result['image_path'])).name})"

            if result["certified_complete"]:
                status = f"{kind}: {found}/{total} audited files match exactly."
            elif result["usable_complete"]:
                status = (
                    f"{kind}: {found}/{total} files found; {mismatched} differ "
                    "from the audited GOG/CD reference and are accepted."
                )
            else:
                status = (
                    f"{kind}: {found}/{total} audited files found; {missing} missing"
                )
                if mismatched:
                    status += f"; {mismatched} differ from the audited reference"
                status += ". Partial source accepted; unavailable categories may fail."

            auxiliary = set(result.get("auxiliary", ()))
            grouped = [
                name for name in ("NPCS.DAT", "OBJECTS.DAT")
                if name in auxiliary
            ]
            if grouped:
                status += " Grouping data: " + ", ".join(grouped) + "."
            self.source_status.set(status)
            self._reset_source_cache()
        except Exception as exc:
            self.source_status.set(str(exc))

    @staticmethod
    def _archive_numbers(values: tuple[str, ...]) -> set[int]:
        return {int(m.group(1)) for value in values if (m := re.fullmatch(r"RES(\d+)", value))}

    def _cinematic_fps(self) -> float | None:
        if self.cin_timing_mode.get() == "Native engine timing":
            return None
        fps = float(self.cin_fps.get())
        if fps <= 0:
            raise RuntimeError("Custom FPS must be greater than zero.")
        return fps

    def _set_busy(self, busy: bool) -> None:
        state = "disabled" if busy else "normal"
        for button in self._action_buttons:
            button.configure(state=state)

    def _run(self, label: str, work) -> None:
        self._set_busy(True)
        self.progress_value.set(0)
        self.status.set(label)
        threading.Thread(target=self._worker, args=(work,), daemon=True).start()

    def _worker(self, work) -> None:
        try:
            self._events.put(("done", work()))
        except Exception as exc:
            self._events.put(("error", str(exc)))

    def _progress(self, done: int, total: int, text: str) -> None:
        self._events.put(("progress", done, total, text))

    def _master_progress(self, stage: str, done: int, total: int, text: str) -> None:
        self._events.put(("progress", done, total, f"{stage}: {text}"))

    def _start_graphics(self) -> None:
        try:
            root = self._source_path()
            selected = self._archive_numbers(self._list_values(self.graphics_list))
            if not selected:
                raise RuntimeError("Select at least one graphics RES file.")
            if not self.graphics_dest.get():
                raise RuntimeError("Choose a graphics destination.")
            dest = Path(self.graphics_dest.get())
            palette = self._graphics_palette_index()
        except Exception as exc:
            messagebox.showerror("Cannot extract", str(exc))
            return
        self._run(
            "Extracting selected graphics…",
            lambda: asset_dumper.dump_graphics(
                root,
                dest,
                self.graphics_format.get(),
                palette,
                self.graphics_native.get(),
                self._progress,
                archives=selected,
                export_indices=self.graphics_indices.get(),
                export_metadata=self.graphics_metadata.get(),
                export_masks=self.graphics_masks.get(),
                organization=self.graphics_organization.get(),
                group_families=self._graphics_group_families(),
                group_contact_sheets=self.graphics_group_contact_sheets.get(),
                group_animated_previews=self.graphics_group_previews.get(),
                group_alias_maps=self.graphics_group_alias_maps.get(),
            ),
        )

    def _start_audio(self) -> None:
        try:
            root = self._source_path()
            if not self.audio_dest.get():
                raise RuntimeError("Choose an audio/music destination.")
            dest = Path(self.audio_dest.get())
            audio_archives = self._archive_numbers(self._list_values(self.audio_list))
            music_archives = self._archive_numbers(self._list_values(self.music_list))
            if self.dump_sampled_var.get() and not audio_archives:
                raise RuntimeError("Select at least one sampled-audio RES file.")
            if self.dump_music_var.get() and not music_archives:
                raise RuntimeError("Select at least one XMIDI RES file.")
        except Exception as exc:
            messagebox.showerror("Cannot extract", str(exc))
            return

        def work():
            result = {}
            if self.dump_sampled_var.get():
                result["audio"] = asset_dumper.dump_audio(root, dest / "sampled_audio", self.audio_format.get(), self.audio_native.get(), self._progress, archives=audio_archives)
            if self.dump_music_var.get():
                result["music"] = asset_dumper.dump_music(root, dest / "music", fmt=self.music_format.get(), preserve_native=self.audio_native.get(), progress=self._progress, archives=music_archives)
            if self.dump_opl_var.get():
                result["opl2"] = support_dumper.dump_sample_ad(root, dest / "opl2")
            if self.dump_aria_var.get():
                result["aria"] = support_dumper.dump_banks(root, dest / "aria")
            return result
        self._run("Extracting selected audio and music…", work)

    def _start_support(self) -> None:
        try:
            root = self._source_path()
            if not self.support_dest.get():
                raise RuntimeError("Choose a support-asset destination.")
            dest = Path(self.support_dest.get())
        except Exception as exc:
            messagebox.showerror("Cannot extract", str(exc))
            return
        def work():
            result = {}
            if self.support_palettes.get(): result["palettes"] = support_dumper.dump_colors(root, dest / "palettes")
            if self.support_text.get(): result["text"] = support_dumper.dump_text(root, dest / "text")
            if self.support_opl.get(): result["opl2"] = support_dumper.dump_sample_ad(root, dest / "opl2")
            if self.support_bnk.get(): result["aria"] = support_dumper.dump_banks(root, dest / "aria")
            if self.support_init.get(): result["init"] = support_dumper.dump_init(root, dest / "init")
            if self.support_sintab.get(): result["sintab"] = support_dumper.dump_sintab(root, dest / "renderer_support")
            return result
        self._run("Extracting selected support assets…", work)

    def _start_cinematics(self) -> None:
        try:
            root = self._source_path()
            names = self._list_values(self.cin_list)
            if not names:
                raise RuntimeError("Select at least one cinematic.")
            fps = self._cinematic_fps()
            frame_root = Path(self.cin_frames_dest.get()) if self.cin_frames.get() else None
            video_root = Path(self.cin_video_dest.get()) if self.cin_video.get() else None
            if self.cin_frames.get() and not self.cin_frames_dest.get():
                raise RuntimeError("Choose a frame destination.")
            if self.cin_video.get() and not self.cin_video_dest.get():
                raise RuntimeError("Choose a video destination.")
            if not self.cin_meta_dest.get():
                raise RuntimeError("Choose a metadata/original destination.")
            meta_root = Path(self.cin_meta_dest.get())
        except Exception as exc:
            messagebox.showerror("Cannot extract", str(exc))
            return
        base = meta_root
        options = master_dumper.DumpOptions(
            cinematic_frame_format=self.cin_frame_format.get(),
            cinematic_video_format=self.cin_video_format.get(),
            cinematic_fps=fps,
            cinematic_audio=self.cin_sync_audio.get(),
            cinematic_frames=self.cin_frames.get(),
            cinematic_videos=self.cin_video.get(),
            cinematic_timeline=self.cin_timeline.get(),
            preserve_native=self.cin_native.get(),
        )
        self._run(
            "Extracting selected cinematics…",
            lambda: master_dumper.dump_cinematics(
                root,
                base,
                options,
                self._master_progress,
                frame_root,
                video_root,
                base / "timelines",
                names=names,
            ),
        )

    def _start_all(self) -> None:
        try:
            root = self._source_path()
            if not self.all_dest.get():
                raise RuntimeError("Choose a Dump All destination.")
            dest = Path(self.all_dest.get())
        except Exception as exc:
            messagebox.showerror("Cannot dump", str(exc))
            return
        options = master_dumper.DumpOptions(
            graphics_format=self.graphics_format.get(),
            graphics_palette=self._graphics_palette_index(),
            graphics_organization=self.graphics_organization.get(),
            graphics_group_families=tuple(sorted(self._graphics_group_families())),
            graphics_group_contact_sheets=self.graphics_group_contact_sheets.get(),
            graphics_group_animated_previews=self.graphics_group_previews.get(),
            graphics_group_alias_maps=self.graphics_group_alias_maps.get(),
            graphics_indices=self.graphics_indices.get(),
            graphics_metadata=self.graphics_metadata.get(),
            graphics_masks=self.graphics_masks.get(),
            graphics_native=self.graphics_native.get(),
            audio_format=self.audio_format.get(),
            music_format=self.music_format.get(),
            cinematic_frame_format=self.cin_frame_format.get(),
            cinematic_video_format="MKV (FFV1 lossless)",
            cinematic_fps=None,
            cinematic_audio=self.all_sync_audio.get(),
            cinematic_frames=self.all_frames.get(),
            cinematic_videos=self.all_videos.get(),
            cinematic_timeline=True,
            preserve_native=True,
            export_support=self.all_support.get(),
        )
        self._run("Dumping complete media corpus…", lambda: master_dumper.dump_all(root, dest, options, self._master_progress, True))

    def _poll_events(self) -> None:
        try:
            while True:
                event = self._events.get_nowait()
                if event[0] == "progress":
                    _, done, total, text = event
                    self.progress_value.set((done / max(1, total)) * 100)
                    self.status.set(text)
                elif event[0] == "done":
                    self._set_busy(False)
                    self.progress_value.set(100)
                    self.status.set("Extraction complete.")
                    messagebox.showinfo("Ravenloft Asset Extraction Suite", "Extraction complete.")
                elif event[0] == "error":
                    self._set_busy(False)
                    self.status.set("Extraction failed.")
                    messagebox.showerror("Extraction failed", event[1])
        except queue.Empty:
            pass
        self.after(100, self._poll_events)


def main() -> None:
    RavenloftExtractorGui().mainloop()


if __name__ == "__main__":
    main()
