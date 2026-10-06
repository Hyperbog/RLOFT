#!/usr/bin/env python3
"""Responsive Strahd-themed GUI for the Ravenloft master asset extractor."""
from __future__ import annotations

import queue
import re
import threading
import tkinter as tk
from pathlib import Path
from tkinter import filedialog, messagebox, ttk

from PIL import Image, ImageDraw, ImageFont, ImageTk

import asset_dumper
import exporter
import master_dumper
import media_inventory
import support_dumper


BG = "#170e0d"
PANEL = "#241512"
PANEL2 = "#34211b"
FIELD = "#100b0a"
TEXT = "#eadfc9"
MUTED = "#b9aa90"
GOLD = "#d1ae63"
BORDER = "#72543c"
ACCENT = "#741c1c"
ACCENT_HOT = "#9b2b27"
TROUGH = "#0a0707"

GRAPHICS_ARCHIVES = ("RES0", "RES1", "RES2", "RES3", "RES4", "RES7", "RES8", "RES9", "RES11", "RES12")
AUDIO_ARCHIVES = ("RES6", "RES10", "RES11", "RES12")
MUSIC_ARCHIVES = ("RES5", "RES11")
TIMING_MODES = ("Native engine timing", "Custom FPS")


class RavenloftExtractorGui(tk.Tk):
    def __init__(self) -> None:
        super().__init__()
        self.title("Ravenloft Asset Extraction Suite")
        self.geometry("1180x820")
        self.minsize(820, 620)
        self.resizable(True, True)
        self.configure(background=BG)

        self.source_dir = tk.StringVar()
        self.source_status = tk.StringVar(value="Choose the Ravenloft resource folder.")
        self.status = tk.StringVar(value="Ready.")
        self.progress_value = tk.DoubleVar(value=0)

        self.graphics_format = tk.StringVar(value="PNG (lossless indexed)")
        self.graphics_palette = tk.IntVar(value=0)
        self.graphics_native = tk.BooleanVar(value=True)
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
        self.cin_timing_mode = tk.StringVar(value="Native engine timing")
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
        self._banner_photo: ImageTk.PhotoImage | None = None
        self._banner_job: str | None = None

        self._build_styles()
        self._build_ui()
        self.after(100, self._poll_events)

    def _build_styles(self) -> None:
        self.style = ttk.Style(self)
        if "clam" in self.style.theme_names():
            self.style.theme_use("clam")
        s = self.style
        s.configure("TFrame", background=BG)
        s.configure("TLabel", background=BG, foreground=TEXT)
        s.configure("Title.TLabel", background=BG, foreground=GOLD, font=("Georgia", 18, "bold"))
        s.configure("Subtle.TLabel", background=BG, foreground=MUTED)
        s.configure("TLabelframe", background=PANEL, bordercolor=BORDER, lightcolor=BORDER, darkcolor="#0d0706", relief="solid")
        s.configure("TLabelframe.Label", background=PANEL, foreground=GOLD, font=("Georgia", 10, "bold"))
        s.configure("TNotebook", background=BG, bordercolor=BORDER, lightcolor=BORDER, darkcolor=BORDER, tabmargins=(2, 2, 2, 0))
        s.configure("TNotebook.Tab", background=PANEL2, foreground=MUTED, bordercolor=BORDER, padding=(12, 7))
        s.map("TNotebook.Tab", background=[("selected", ACCENT), ("active", PANEL)], foreground=[("selected", "#fff2d2"), ("active", GOLD)])
        s.configure("TButton", background=PANEL2, foreground=GOLD, bordercolor=BORDER, lightcolor=BORDER, darkcolor="#0d0706", padding=(10, 6))
        s.map("TButton", background=[("pressed", ACCENT), ("active", ACCENT_HOT), ("disabled", PANEL)], foreground=[("disabled", "#665c53"), ("pressed", "#fff4d4")])
        s.configure("TCheckbutton", background=PANEL, foreground=TEXT, indicatorcolor=FIELD, bordercolor=BORDER)
        s.map("TCheckbutton", background=[("active", PANEL)], foreground=[("active", GOLD), ("disabled", "#665c53")], indicatorcolor=[("selected", ACCENT), ("disabled", PANEL2)])
        for name in ("TEntry", "TSpinbox"):
            s.configure(name, fieldbackground=FIELD, foreground=TEXT, insertcolor=TEXT, bordercolor=BORDER, lightcolor=BORDER, darkcolor=BORDER)
            s.map(name, fieldbackground=[("disabled", PANEL2)], foreground=[("disabled", "#665c53")])
        s.configure("TCombobox", fieldbackground=FIELD, background=PANEL2, foreground=TEXT, arrowcolor=GOLD, bordercolor=BORDER)
        s.map("TCombobox", fieldbackground=[("readonly", FIELD), ("disabled", PANEL2)], foreground=[("readonly", TEXT), ("disabled", "#665c53")], arrowcolor=[("readonly", GOLD)])
        s.configure("Horizontal.TProgressbar", background=ACCENT, troughcolor=TROUGH, bordercolor=BORDER, lightcolor=ACCENT_HOT, darkcolor=ACCENT)
        self.option_add("*TCombobox*Listbox.background", FIELD)
        self.option_add("*TCombobox*Listbox.foreground", TEXT)
        self.option_add("*TCombobox*Listbox.selectBackground", ACCENT)
        self.option_add("*TCombobox*Listbox.selectForeground", "#fff4d4")

    @staticmethod
    def _font(size: int, bold: bool = False) -> ImageFont.ImageFont:
        candidates = [
            Path("C:/Windows/Fonts/georgiab.ttf" if bold else "C:/Windows/Fonts/georgia.ttf"),
            Path("/usr/share/fonts/truetype/dejavu/DejaVuSerif-Bold.ttf" if bold else "/usr/share/fonts/truetype/dejavu/DejaVuSerif.ttf"),
        ]
        for path in candidates:
            if path.is_file():
                return ImageFont.truetype(str(path), size)
        return ImageFont.load_default()

    def _schedule_banner(self) -> None:
        if self._banner_job:
            self.after_cancel(self._banner_job)
        self._banner_job = self.after(50, self._render_banner)

    def _render_banner(self) -> None:
        self._banner_job = None
        width = max(1, self.banner.winfo_width())
        height = 142
        image = Image.new("RGB", (width, height), BG)
        draw = ImageDraw.Draw(image)
        for y in range(height):
            tone = 22 + ((y * 7) % 9)
            draw.line((0, y, width, y), fill=(tone + 7, tone, max(0, tone - 1)))
        draw.rectangle((1, 1, width - 2, height - 2), outline="#9a7547", width=2)
        draw.rectangle((7, 7, width - 8, height - 8), outline="#482b22", width=3)
        for x in range(16, width - 16, 34):
            draw.polygon([(x, 8), (x + 10, 20), (x, 32), (x - 10, 20)], outline="#7b5c3d", fill="#2a1715")
            draw.line((x - 7, 20, x + 7, 20), fill="#a22d27", width=2)
        title_font = self._font(34, True)
        sub_font = self._font(13, True)
        title = "RAVENLOFT"
        subtitle = "STRAHD'S POSSESSION  •  ASSET EXTRACTION SUITE"
        bbox = draw.textbbox((0, 0), title, font=title_font, stroke_width=1)
        tx = (width - (bbox[2] - bbox[0])) // 2
        ty = 43
        draw.text((tx + 2, ty + 3), title, font=title_font, fill="#250706")
        draw.text((tx, ty), title, font=title_font, fill="#c8b7a0", stroke_width=1, stroke_fill="#6d1717")
        sb = draw.textbbox((0, 0), subtitle, font=sub_font)
        sx = (width - (sb[2] - sb[0])) // 2
        draw.text((sx, 92), subtitle, font=sub_font, fill="#d0aa60")
        self._banner_photo = ImageTk.PhotoImage(image)
        self.banner.delete("all")
        self.banner.create_image(0, 0, anchor="nw", image=self._banner_photo)

    def _build_ui(self) -> None:
        self.columnconfigure(0, weight=1)
        self.rowconfigure(2, weight=1)
        self.header = ttk.Frame(self, padding=(16, 10))
        self.header.grid(row=0, column=0, sticky="ew")
        ttk.Label(self.header, text="Ravenloft Asset Extraction Suite", style="Title.TLabel").grid(row=0, column=0, sticky="w")
        self.banner = tk.Canvas(self, height=142, background=BG, highlightthickness=0)
        self.banner.grid(row=1, column=0, sticky="ew")
        self.banner.bind("<Configure>", lambda _e: self._schedule_banner())

        self.body = ttk.Frame(self, padding=(14, 10, 14, 10))
        self.body.grid(row=2, column=0, sticky="nsew")
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

        footer = ttk.Frame(self, padding=(14, 0, 14, 12))
        footer.grid(row=3, column=0, sticky="ew")
        footer.columnconfigure(0, weight=1)
        ttk.Progressbar(footer, variable=self.progress_value, maximum=100).grid(row=0, column=0, sticky="ew")
        ttk.Label(footer, textvariable=self.status, style="Subtle.TLabel").grid(row=1, column=0, sticky="ew", pady=(6, 0))
        self.after(10, self._schedule_banner)

    def _new_tab(self, title: str) -> ttk.Frame:
        tab = ttk.Frame(self.notebook, padding=14)
        tab.columnconfigure(0, weight=1)
        tab.rowconfigure(99, weight=1)
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
        box = tk.Listbox(parent, selectmode=tk.EXTENDED, exportselection=False, height=height, background=FIELD, foreground=TEXT, selectbackground=ACCENT, selectforeground="#fff4d4", highlightbackground=BORDER, highlightcolor=GOLD, relief="solid", borderwidth=1, activestyle="none")
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
        self._selection_buttons(select, 1, 1, self.cin_list)

        out = ttk.LabelFrame(tab, text="Outputs", padding=10)
        out.grid(row=1, column=0, sticky="ew", pady=(10, 0))
        out.columnconfigure(1, weight=1)
        ttk.Checkbutton(out, text="Individual frames", variable=self.cin_frames).grid(row=0, column=0, sticky="w")
        ttk.Combobox(out, textvariable=self.cin_frame_format, values=tuple(exporter.FRAME_FORMATS), state="readonly", width=22).grid(row=0, column=1, sticky="w", padx=8)
        self._folder_row(out, 1, "Frame destination", self.cin_frames_dest)
        ttk.Checkbutton(out, text="Rendered video", variable=self.cin_video).grid(row=2, column=0, sticky="w", pady=(6, 0))
        ttk.Combobox(out, textvariable=self.cin_video_format, values=tuple(exporter.VIDEO_FORMATS), state="readonly", width=24).grid(row=2, column=1, sticky="w", padx=8, pady=(6, 0))
        self._folder_row(out, 3, "Video destination", self.cin_video_dest)

        timing = ttk.Frame(out)
        timing.grid(row=4, column=0, columnspan=3, sticky="ew", pady=(7, 0))
        ttk.Label(timing, text="Timing").pack(side="left")
        combo = ttk.Combobox(timing, textvariable=self.cin_timing_mode, values=TIMING_MODES, state="readonly", width=22)
        combo.pack(side="left", padx=(6, 10))
        combo.bind("<<ComboboxSelected>>", lambda _e: self._update_timing_state())
        ttk.Label(timing, text="Custom FPS").pack(side="left")
        self.cin_fps_entry = ttk.Entry(timing, textvariable=self.cin_fps, width=8)
        self.cin_fps_entry.pack(side="left", padx=(6, 16))
        ttk.Checkbutton(timing, text="Sync sampled SFX into video", variable=self.cin_sync_audio).pack(side="left")

        flags = ttk.Frame(out)
        flags.grid(row=5, column=0, columnspan=3, sticky="w", pady=(7, 0))
        ttk.Checkbutton(flags, text="Timeline JSON", variable=self.cin_timeline).pack(side="left")
        ttk.Checkbutton(flags, text="Preserve original ANM", variable=self.cin_native).pack(side="left", padx=(16, 0))

        self._note(tab, 2, "Native timing reconstructs the CODE.2 frame-wait path. Sampled ANM SFX commands are muxed from RES6 at their frame timestamps; music commands remain XMI/MIDI/timeline data rather than being synthesized through an arbitrary soundfont.")
        button = ttk.Button(tab, text="Extract Selected Cinematics", command=self._start_cinematics)
        button.grid(row=3, column=0, sticky="e", pady=(12, 0))
        self._action_buttons.append(button)
        self._update_timing_state()

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
        ttk.Checkbutton(box, text="Preserve native EH records and canonical palette-index bytes", variable=self.graphics_native).grid(row=4, column=0, columnspan=3, sticky="w", pady=(8, 0))
        self._folder_row(box, 5, "Destination", self.graphics_dest)
        self._note(tab, 1, "Select individual RES files or All to dump the complete graphics category.")
        button = ttk.Button(tab, text="Extract Selected Graphics", command=self._start_graphics)
        button.grid(row=2, column=0, sticky="e", pady=(12, 0))
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
        button.grid(row=2, column=0, sticky="e", pady=(12, 0))
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
        button.grid(row=1, column=0, sticky="e", pady=(12, 0))
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
        button.grid(row=1, column=0, sticky="e", pady=(12, 0))
        self._action_buttons.append(button)

    def _resize_wraps(self, event) -> None:
        width = max(400, event.width - 80)
        for label in self._wrap_labels:
            label.configure(wraplength=width)

    def _update_timing_state(self) -> None:
        self.cin_fps_entry.configure(state="normal" if self.cin_timing_mode.get() == "Custom FPS" else "disabled")

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
        self._run("Extracting selected graphics…", lambda: asset_dumper.dump_graphics(root, dest, self.graphics_format.get(), palette, self.graphics_native.get(), self._progress, archives=selected))

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
        except Exception as exc:
            messagebox.showerror("Cannot extract", str(exc))
            return
        base = frame_root or video_root or root / "RLOFT_DUMP" / "cinematics"
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
        self._run("Extracting selected cinematics…", lambda: master_dumper.dump_cinematics(root, base, options, self._master_progress, frame_root, video_root, base / "timelines", names=names))

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
