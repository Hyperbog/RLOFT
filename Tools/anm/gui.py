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
import exporter
import master_dumper
import media_inventory
import runtime_paths
import support_dumper
import tooltips
import title_skin


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
        self.minsize(max(860, int(900 * launch_scale)), max(650, int(680 * launch_scale)))
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
        self.source_status = tk.StringVar(value="Choose the Ravenloft resource folder.")
        self.status = tk.StringVar(value="Ready.")
        self.progress_value = tk.DoubleVar(value=0)

        self.graphics_format = tk.StringVar(value="PNG (lossless indexed)")
        self.graphics_palette = tk.IntVar(value=0)
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
        self._wrap_labels: list[ttk.Label] = []
        self._skin_photo: ImageTk.PhotoImage | None = None
        self._skin_job: str | None = None
        self._tooltips: list[object] = []

        self._build_styles()
        self._build_ui()
        self._refresh_native_timing_text()
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

        # Resizing changes available layout space only.  Screen resolution
        # establishes the initial UI scale once at launch; explicit UI-size
        # controls are the only way to change fonts/buttons afterward.
        self._resize_wraps_for_width(width)
        self._fit_listboxes_to_height(height)
        self._schedule_skin()

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
        self._resize_wraps_for_width(max(1, self.winfo_width()))
        self._fit_listboxes_to_height(max(1, self.winfo_height()))
        self._schedule_skin()

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
        side, top, bottom = title_skin.chrome_insets(self._ui_scale)
        if hasattr(self, "header"):
            self.header.grid_configure(padx=(side, side), pady=(top, 0))
        if hasattr(self, "body"):
            self.body.grid_configure(padx=(side, side))
        if hasattr(self, "footer"):
            self.footer.grid_configure(padx=(side, side), pady=(0, bottom))

    def _schedule_skin(self) -> None:
        if self._skin_job:
            try:
                self.after_cancel(self._skin_job)
            except tk.TclError:
                pass
        self._skin_job = self.after(45, self._render_skin)

    def _render_skin(self) -> None:
        self._skin_job = None
        width = max(1, self.winfo_width())
        height = max(1, self.winfo_height())
        image = title_skin.render_window(width, height, self._ui_scale)
        self._skin_photo = ImageTk.PhotoImage(image)
        self.skin_canvas.delete("all")
        self.skin_canvas.create_image(0, 0, anchor="nw", image=self._skin_photo)

    def _build_ui(self) -> None:
        self.columnconfigure(0, weight=1)
        self.rowconfigure(1, weight=1)

        # Full-window original title-screen skin.  It is created first so all
        # interactive widgets naturally stack above it.
        self.skin_canvas = tk.Canvas(
            self,
            background=BG,
            highlightthickness=0,
            borderwidth=0,
        )
        self.skin_canvas.place(x=0, y=0, relwidth=1, relheight=1)

        self.header = ttk.Frame(self, padding=(16, 8))
        self.header.grid(row=0, column=0, sticky="ew")
        self._padding_widgets.append((self.header, (16, 8)))
        self.header.columnconfigure(0, weight=1)
        ttk.Label(
            self.header,
            text="Ravenloft Asset Extraction Suite",
            style="Title.TLabel",
        ).grid(row=0, column=0, sticky="w")
        zoom = ttk.Frame(self.header)
        zoom.grid(row=0, column=1, sticky="e")
        ttk.Label(zoom, text="UI size").pack(side="left", padx=(0, 6))
        ttk.Button(zoom, text="−", width=3, command=self._zoom_out).pack(side="left")
        ttk.Button(
            zoom,
            textvariable=self.ui_zoom_text,
            width=6,
            command=self._zoom_reset,
        ).pack(side="left", padx=4)
        ttk.Button(zoom, text="+", width=3, command=self._zoom_in).pack(side="left")

        self.body = ttk.Frame(self, padding=(14, 8, 14, 8))
        self.body.grid(row=1, column=0, sticky="nsew")
        self._padding_widgets.append((self.body, (14, 8, 14, 8)))
        self.body.columnconfigure(0, weight=1)
        self.body.rowconfigure(1, weight=1)
        self.body.bind("<Configure>", self._resize_wraps)

        source = ttk.Frame(self.body, padding=(2, 2, 2, 10))
        source.grid(row=0, column=0, sticky="ew")
        source.columnconfigure(1, weight=1)
        ttk.Label(source, text="Resource folder").grid(row=0, column=0, sticky="w", padx=(0, 8))
        ttk.Entry(source, textvariable=self.source_dir).grid(row=0, column=1, sticky="ew")
        ttk.Button(source, text="Browse…", command=self._choose_source_dir).grid(row=0, column=2, padx=(8, 0))
        validate = ttk.Button(source, text="Validate 33 files", command=self._validate_source)
        validate.grid(row=0, column=3, padx=(8, 0))
        self._action_buttons.append(validate)
        ttk.Label(source, textvariable=self.source_status, style="Subtle.TLabel").grid(row=1, column=0, columnspan=4, sticky="ew", pady=(7, 0))

        self.notebook = ttk.Notebook(self.body)
        self.notebook.grid(row=1, column=0, sticky="nsew")
        self._build_cinematics_tab()
        self._build_graphics_tab()
        self._build_audio_tab()
        self._build_support_tab()
        self._build_all_tab()

        self.footer = ttk.Frame(self, padding=(14, 0, 14, 12))
        self.footer.grid(row=2, column=0, sticky="ew")
        self._padding_widgets.append((self.footer, (14, 0, 14, 12)))
        self.footer.columnconfigure(0, weight=1)
        ttk.Progressbar(self.footer, variable=self.progress_value, maximum=100).grid(row=0, column=0, sticky="ew")
        ttk.Label(self.footer, textvariable=self.status, style="Subtle.TLabel").grid(row=1, column=0, sticky="ew", pady=(6, 0))
        self._apply_chrome_insets()
        self.after(10, self._schedule_skin)

    def _new_tab(self, title: str) -> ttk.Frame:
        tab = ttk.Frame(self.notebook, padding=14)
        self._padding_widgets.append((tab, (14,)))
        tab.columnconfigure(0, weight=1)
        self.notebook.add(tab, text=title)
        return tab

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
        button = ttk.Button(tab, text="Extract Selected Cinematics", command=self._start_cinematics)
        button.grid(row=3, column=0, sticky="e", pady=(10, 0))
        self._action_buttons.append(button)
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
        ttk.Label(box, text="Palette bank").grid(row=3, column=0, sticky="w", pady=(8, 0))
        ttk.Spinbox(box, from_=0, to=14, textvariable=self.graphics_palette, width=6).grid(row=3, column=1, sticky="w", padx=8, pady=(8, 0))

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

        self._folder_row(box, 5, "Destination", self.graphics_dest)
        self._note(
            tab,
            1,
            "PNG is sufficient for most modern-port artwork. RES3 NPCs/monsters, placed objects, loose items, and effects are now reconstructed as transparent native-resolution images. The associated-file options retain exact indexed/source data for preservation and renderer validation.",
        )
        button = ttk.Button(tab, text="Extract Selected Graphics", command=self._start_graphics)
        button.grid(row=2, column=0, sticky="e", pady=(10, 0))
        self._action_buttons.append(button)

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
        button = ttk.Button(tab, text="Extract Selected Audio & Music", command=self._start_audio)
        button.grid(row=2, column=0, sticky="e", pady=(10, 0))
        self._action_buttons.append(button)

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
        button = ttk.Button(tab, text="Extract Selected Support Assets", command=self._start_support)
        button.grid(row=2, column=0, sticky="e", pady=(10, 0))
        self._action_buttons.append(button)

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
        button = ttk.Button(tab, text="Dump Everything", command=self._start_all)
        button.grid(row=2, column=0, sticky="e", pady=(10, 0))
        self._action_buttons.append(button)

    def _resize_wraps_for_width(self, window_width: int) -> None:
        # Keep explanatory text inside the current client area. The value is
        # intentionally conservative because the notebook/tab/frame borders
        # consume some horizontal space.
        width = max(260, window_width - round(120 * self._ui_scale))
        for label in self._wrap_labels:
            label.configure(wraplength=width)

    def _resize_wraps(self, event) -> None:
        self._resize_wraps_for_width(event.width)

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

    def _choose_source_dir(self) -> None:
        value = filedialog.askdirectory(title="Choose Ravenloft resource folder")
        if value:
            self.source_dir.set(value)
            self._auto_destinations(Path(value))
            self._validate_source()

    @staticmethod
    def _choose_dir(variable: tk.StringVar) -> None:
        value = filedialog.askdirectory(title="Choose output folder")
        if value:
            variable.set(value)

    def _auto_destinations(self, root: Path) -> None:
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

    def _source_path(self) -> Path:
        root = Path(self.source_dir.get())
        if not root.is_dir():
            raise RuntimeError("Choose a valid Ravenloft resource folder.")
        return root

    def _validate_source(self) -> None:
        try:
            result = media_inventory.validate_source_dir(self._source_path(), verify_hashes=True)
            if result["certified_complete"]:
                self.source_status.set("All 33 audited media/resource files match.")
            else:
                self.source_status.set(f"Missing {len(result['missing'])}; mismatched {len(result['mismatched'])}.")
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
            palette = int(self.graphics_palette.get())
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
            graphics_palette=int(self.graphics_palette.get()),
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
