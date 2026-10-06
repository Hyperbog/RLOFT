#!/usr/bin/env python3
"""Responsive GUI for the Ravenloft master media extraction suite."""
from __future__ import annotations

import queue
import threading
import tkinter as tk
from pathlib import Path
from tkinter import filedialog, messagebox, ttk

from PIL import Image, ImageTk

import asset_dumper
import exporter
import master_dumper
import media_inventory
import rloft_anm as anm
import support_dumper

try:
    import theme_assets
except Exception:
    theme_assets = None


class RavenloftExtractorGui(tk.Tk):
    def __init__(self) -> None:
        super().__init__()
        self.title("Ravenloft Asset Extraction Suite")
        self.geometry("1120x780")
        self.minsize(760, 540)
        self.resizable(True, True)

        self.source_dir = tk.StringVar()
        self.source_status = tk.StringVar(value="Choose the Ravenloft resource folder.")
        self.theme = tk.StringVar(value="Modern")
        self.status = tk.StringVar(value="Ready.")
        self.progress_value = tk.DoubleVar(value=0)

        self.graphics_format = tk.StringVar(value="PNG (lossless indexed)")
        self.graphics_palette = tk.IntVar(value=0)
        self.graphics_native = tk.BooleanVar(value=True)
        self.graphics_dest = tk.StringVar()

        self.audio_format = tk.StringVar(value="FLAC (lossless)")
        self.audio_native = tk.BooleanVar(value=True)
        self.audio_dest = tk.StringVar()
        self.dump_music_var = tk.BooleanVar(value=True)
        self.dump_opl_var = tk.BooleanVar(value=True)
        self.dump_aria_var = tk.BooleanVar(value=True)

        self.cin_all = tk.BooleanVar(value=True)
        self.cin_source = tk.StringVar()
        self.cin_frames = tk.BooleanVar(value=True)
        self.cin_frame_format = tk.StringVar(value="PNG")
        self.cin_frames_dest = tk.StringVar()
        self.cin_video = tk.BooleanVar(value=False)
        self.cin_video_format = tk.StringVar(value="MKV (FFV1 lossless)")
        self.cin_video_dest = tk.StringVar()
        self.cin_fps = tk.StringVar(value="15")
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

        self._events: queue.Queue[tuple] = queue.Queue()
        self._action_buttons: list[ttk.Button] = []
        self._wrap_labels: list[ttk.Label] = []
        self._theme_data = None
        self._banner_photo: ImageTk.PhotoImage | None = None
        self._banner_job: str | None = None

        self._build_styles()
        self._build_ui()
        self.apply_theme()
        self.after(100, self._poll_events)

    def _build_styles(self) -> None:
        style = ttk.Style(self)
        if "clam" in style.theme_names():
            style.theme_use("clam")
        style.configure("Title.TLabel", font=("TkDefaultFont", 17, "bold"))
        style.configure("Strahd.TFrame", background="#111010")
        style.configure("Strahd.TLabel", background="#111010", foreground="#d9d0bf")
        style.configure("Strahd.Title.TLabel", background="#111010", foreground="#c9b98b", font=("TkDefaultFont", 17, "bold"))

    def _build_ui(self) -> None:
        self.columnconfigure(0, weight=1)
        self.rowconfigure(2, weight=1)

        self.header = ttk.Frame(self, padding=(14, 10))
        self.header.grid(row=0, column=0, sticky="ew")
        self.header.columnconfigure(0, weight=1)
        self.title_label = ttk.Label(self.header, text="Ravenloft Asset Extraction Suite", style="Title.TLabel")
        self.title_label.grid(row=0, column=0, sticky="w")
        ttk.Label(self.header, text="Theme").grid(row=0, column=1, padx=(12, 6))
        theme_combo = ttk.Combobox(
            self.header, textvariable=self.theme,
            values=("Modern", "Strahd's Possession"), state="readonly", width=20
        )
        theme_combo.grid(row=0, column=2, sticky="e")
        theme_combo.bind("<<ComboboxSelected>>", lambda _e: self.apply_theme())

        self.banner = tk.Canvas(self, height=0, highlightthickness=0)
        self.banner.grid(row=1, column=0, sticky="ew")
        self.banner.bind("<Configure>", lambda _e: self._schedule_banner())

        self.body = ttk.Frame(self, padding=(14, 0, 14, 10))
        self.body.grid(row=2, column=0, sticky="nsew")
        self.body.columnconfigure(0, weight=1)
        self.body.rowconfigure(1, weight=1)
        self.body.bind("<Configure>", self._resize_wraps)

        source = ttk.LabelFrame(self.body, text="Certified source corpus", padding=10)
        source.grid(row=0, column=0, sticky="ew", pady=(0, 10))
        source.columnconfigure(1, weight=1)
        ttk.Label(source, text="Resource folder").grid(row=0, column=0, sticky="w", padx=(0, 8))
        ttk.Entry(source, textvariable=self.source_dir).grid(row=0, column=1, sticky="ew")
        ttk.Button(source, text="Browse…", command=self._choose_source_dir).grid(row=0, column=2, padx=(8, 0))
        validate = ttk.Button(source, text="Validate 33 files", command=self._validate_source)
        validate.grid(row=0, column=3, padx=(8, 0))
        self._action_buttons.append(validate)
        ttk.Label(source, textvariable=self.source_status).grid(
            row=1, column=0, columnspan=4, sticky="ew", pady=(7, 0)
        )

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
        ttk.Progressbar(footer, variable=self.progress_value, maximum=100).grid(
            row=0, column=0, sticky="ew"
        )
        ttk.Label(footer, textvariable=self.status, anchor="w").grid(
            row=1, column=0, sticky="ew", pady=(6, 0)
        )

    def _new_tab(self, title: str) -> ttk.Frame:
        tab = ttk.Frame(self.notebook, padding=14)
        tab.columnconfigure(0, weight=1)
        self.notebook.add(tab, text=title)
        return tab

    def _folder_row(self, parent, row: int, label: str, variable: tk.StringVar) -> None:
        parent.columnconfigure(1, weight=1)
        ttk.Label(parent, text=label).grid(row=row, column=0, sticky="w", padx=(0, 8), pady=5)
        ttk.Entry(parent, textvariable=variable).grid(row=row, column=1, sticky="ew", pady=5)
        ttk.Button(parent, text="Browse…", command=lambda: self._choose_dir(variable)).grid(
            row=row, column=2, padx=(8, 0), pady=5
        )

    def _note(self, parent, row: int, text: str) -> None:
        label = ttk.Label(parent, text=text, justify="left", wraplength=900)
        label.grid(row=row, column=0, sticky="ew", pady=(10, 0))
        self._wrap_labels.append(label)

    def _build_cinematics_tab(self) -> None:
        tab = self._new_tab("Cinematics")
        select = ttk.LabelFrame(tab, text="Source", padding=10)
        select.grid(row=0, column=0, sticky="ew")
        select.columnconfigure(1, weight=1)
        ttk.Checkbutton(
            select, text="All CINE00.ANM – CINE12.ANM",
            variable=self.cin_all, command=self._update_cin_source_state
        ).grid(row=0, column=0, columnspan=3, sticky="w")
        ttk.Label(select, text="Single ANM").grid(row=1, column=0, sticky="w", pady=(8, 0))
        self.cin_source_entry = ttk.Entry(select, textvariable=self.cin_source)
        self.cin_source_entry.grid(row=1, column=1, sticky="ew", padx=(8, 0), pady=(8, 0))
        self.cin_source_button = ttk.Button(select, text="Browse…", command=self._choose_single_anm)
        self.cin_source_button.grid(row=1, column=2, padx=(8, 0), pady=(8, 0))

        out = ttk.LabelFrame(tab, text="Outputs", padding=10)
        out.grid(row=1, column=0, sticky="ew", pady=(10, 0))
        out.columnconfigure(1, weight=1)
        ttk.Checkbutton(out, text="Individual frames", variable=self.cin_frames).grid(row=0, column=0, sticky="w")
        ttk.Combobox(
            out, textvariable=self.cin_frame_format,
            values=tuple(exporter.FRAME_FORMATS), state="readonly", width=22
        ).grid(row=0, column=1, sticky="w", padx=8)
        self._folder_row(out, 1, "Frame destination", self.cin_frames_dest)

        ttk.Checkbutton(out, text="Rendered video", variable=self.cin_video).grid(row=2, column=0, sticky="w", pady=(6, 0))
        ttk.Combobox(
            out, textvariable=self.cin_video_format,
            values=tuple(exporter.VIDEO_FORMATS), state="readonly", width=24
        ).grid(row=2, column=1, sticky="w", padx=8, pady=(6, 0))
        self._folder_row(out, 3, "Video destination", self.cin_video_dest)

        options = ttk.Frame(out)
        options.grid(row=4, column=0, columnspan=3, sticky="ew", pady=(6, 0))
        ttk.Label(options, text="Presentation FPS").pack(side="left")
        ttk.Entry(options, textvariable=self.cin_fps, width=8).pack(side="left", padx=(6, 18))
        ttk.Checkbutton(options, text="Timeline JSON", variable=self.cin_timeline).pack(side="left", padx=(0, 18))
        ttk.Checkbutton(options, text="Preserve original ANM", variable=self.cin_native).pack(side="left")

        self._note(
            tab, 2,
            "PNG frames plus the original ANM/timeline are the archival default. "
            "MKV/FFV1 is the preferred lossless rendered-video derivative."
        )
        button = ttk.Button(tab, text="Extract Cinematics", command=self._start_cinematics)
        button.grid(row=3, column=0, sticky="e", pady=(12, 0))
        self._action_buttons.append(button)
        self._update_cin_source_state()

    def _build_graphics_tab(self) -> None:
        tab = self._new_tab("Graphics & Textures")
        box = ttk.LabelFrame(tab, text="RES visual assets", padding=10)
        box.grid(row=0, column=0, sticky="ew")
        box.columnconfigure(1, weight=1)
        ttk.Label(box, text="Image format").grid(row=0, column=0, sticky="w")
        ttk.Combobox(
            box, textvariable=self.graphics_format,
            values=tuple(asset_dumper.GRAPHICS_FORMATS), state="readonly", width=26
        ).grid(row=0, column=1, sticky="w", padx=8)
        ttk.Label(box, text="Palette bank").grid(row=1, column=0, sticky="w", pady=(8, 0))
        ttk.Spinbox(box, from_=0, to=14, textvariable=self.graphics_palette, width=6).grid(
            row=1, column=1, sticky="w", padx=8, pady=(8, 0)
        )
        ttk.Checkbutton(
            box,
            text="Preserve native EH records and canonical decoded palette-index bytes",
            variable=self.graphics_native,
        ).grid(row=2, column=0, columnspan=3, sticky="w", pady=(8, 0))
        self._folder_row(box, 3, "Destination", self.graphics_dest)
        self._note(
            tab, 1,
            "Covers type-1 rasters/UI/sprites, type-2 fonts, type-6 32×128/64×128 textures, "
            "and type-7 compiled scene-shape data. Type-7 remains structural until the "
            "SINTAB-coupled renderer is translated faithfully."
        )
        button = ttk.Button(tab, text="Extract All Graphics Assets", command=self._start_graphics)
        button.grid(row=2, column=0, sticky="e", pady=(12, 0))
        self._action_buttons.append(button)

    def _build_audio_tab(self) -> None:
        tab = self._new_tab("Audio & Music")
        box = ttk.LabelFrame(tab, text="Sampled audio / music", padding=10)
        box.grid(row=0, column=0, sticky="ew")
        box.columnconfigure(1, weight=1)
        ttk.Label(box, text="Sampled-audio format").grid(row=0, column=0, sticky="w")
        ttk.Combobox(
            box, textvariable=self.audio_format,
            values=tuple(asset_dumper.AUDIO_FORMATS), state="readonly", width=24
        ).grid(row=0, column=1, sticky="w", padx=8)
        ttk.Checkbutton(
            box, text="Preserve original VOC and native EH carriers", variable=self.audio_native
        ).grid(row=1, column=0, columnspan=3, sticky="w", pady=(8, 0))
        ttk.Checkbutton(box, text="Extract all XMIDI/XDIR sequences", variable=self.dump_music_var).grid(
            row=2, column=0, columnspan=3, sticky="w", pady=(8, 0)
        )
        ttk.Checkbutton(box, text="Extract SAMPLE.AD OPL2 timbres", variable=self.dump_opl_var).grid(
            row=3, column=0, columnspan=3, sticky="w", pady=(8, 0)
        )
        ttk.Checkbutton(box, text="Preserve GM1.BNK / GM2.BNK ARIA banks", variable=self.dump_aria_var).grid(
            row=4, column=0, columnspan=3, sticky="w", pady=(8, 0)
        )
        self._folder_row(box, 5, "Destination", self.audio_dest)
        self._note(
            tab, 1,
            "FLAC and WAV/PCM are lossless derivatives of the original VOC streams. "
            "XMIDI is preserved exactly; Standard MIDI output stays disabled until an "
            "authoritative XMIDI event/timing conversion is implemented."
        )
        button = ttk.Button(tab, text="Extract Audio & Music", command=self._start_audio)
        button.grid(row=2, column=0, sticky="e", pady=(12, 0))
        self._action_buttons.append(button)

    def _build_support_tab(self) -> None:
        tab = self._new_tab("Support Assets")
        box = ttk.LabelFrame(tab, text="Standalone support data", padding=10)
        box.grid(row=0, column=0, sticky="ew")
        options = (
            ("COLORS — 15 VGA palette banks", self.support_palettes),
            ("T — 1,073 decoded text/subtitle records", self.support_text),
            ("SAMPLE.AD — 233 OPL2 timbres", self.support_opl),
            ("GM1.BNK / GM2.BNK — ARIA banks + metadata", self.support_bnk),
            ("INIT — resource namespace / bank mappings", self.support_init),
            ("SINTAB — renderer tables and 127 scale blocks", self.support_sintab),
        )
        for row, (text, variable) in enumerate(options):
            ttk.Checkbutton(box, text=text, variable=variable).grid(row=row, column=0, sticky="w", pady=3)
        self._folder_row(box, len(options), "Destination", self.support_dest)
        button = ttk.Button(tab, text="Extract Selected Support Assets", command=self._start_support)
        button.grid(row=1, column=0, sticky="e", pady=(12, 0))
        self._action_buttons.append(button)

    def _build_all_tab(self) -> None:
        tab = self._new_tab("Dump All")
        box = ttk.LabelFrame(tab, text="Complete certified 33-file media dump", padding=10)
        box.grid(row=0, column=0, sticky="ew")
        desc = ttk.Label(
            box,
            text=(
                "Exports every known graphic, texture, font, compiled shape, sampled sound, "
                "XMIDI sequence, cinematic, palette, subtitle/text record, OPL2 timbre, "
                "renderer support package, and ARIA support bank."
            ),
            justify="left", wraplength=900,
        )
        desc.grid(row=0, column=0, columnspan=3, sticky="ew")
        self._wrap_labels.append(desc)
        ttk.Checkbutton(box, text="Cinematic frame sequences", variable=self.all_frames).grid(
            row=1, column=0, sticky="w", pady=(8, 0)
        )
        ttk.Checkbutton(box, text="Also render FFV1/MKV movies", variable=self.all_videos).grid(
            row=2, column=0, sticky="w", pady=(5, 0)
        )
        ttk.Checkbutton(box, text="Standalone support assets", variable=self.all_support).grid(
            row=3, column=0, sticky="w", pady=(5, 0)
        )
        self._folder_row(box, 4, "Destination", self.all_dest)
        button = ttk.Button(tab, text="Dump Everything", command=self._start_all)
        button.grid(row=1, column=0, sticky="e", pady=(12, 0))
        self._action_buttons.append(button)

    # ---------- responsive/theme ----------

    def _resize_wraps(self, event) -> None:
        width = max(360, event.width - 70)
        for label in self._wrap_labels:
            label.configure(wraplength=width)

    def apply_theme(self) -> None:
        strahd = self.theme.get() == "Strahd's Possession"
        if not strahd:
            self.configure(background="")
            self.header.configure(style="TFrame")
            self.title_label.configure(style="Title.TLabel")
            self.banner.configure(height=0)
            self._theme_data = None
            self._banner_photo = None
            return

        self.header.configure(style="Strahd.TFrame")
        self.title_label.configure(style="Strahd.Title.TLabel")
        self.banner.configure(background="#111010", height=132)
        try:
            if theme_assets is None:
                raise RuntimeError("theme asset module unavailable")
            source = Path(self.source_dir.get())
            if not source.is_dir():
                raise RuntimeError("choose the certified Ravenloft source folder to load the original RES artwork")
            self._theme_data = theme_assets.StrahdThemeAssets.load(source)
            self.status.set("Strahd's Possession theme loaded from original RES0/COLORS artwork.")
            self._schedule_banner()
        except Exception as exc:
            self._theme_data = None
            self.status.set(f"Strahd theme styling enabled; original artwork unavailable: {exc}")

    def _schedule_banner(self) -> None:
        if self._banner_job:
            self.after_cancel(self._banner_job)
        self._banner_job = self.after(60, self._render_banner)

    def _render_banner(self) -> None:
        self._banner_job = None
        if self.theme.get() != "Strahd's Possession" or self._theme_data is None:
            return
        width = max(1, self.banner.winfo_width())
        height = 132
        bg = self._theme_data.frame.resize((width, max(height, int(width * 0.25))), Image.Resampling.NEAREST)
        bg = bg.crop((0, 0, width, height))
        logo = self._theme_data.title_logo.copy()
        scale = min(1.0, (width * 0.42) / logo.width, (height - 8) / logo.height)
        logo = logo.resize((max(1, int(logo.width * scale)), max(1, int(logo.height * scale))), Image.Resampling.NEAREST)
        bg.paste(logo, ((width - logo.width) // 2, (height - logo.height) // 2))
        self._banner_photo = ImageTk.PhotoImage(bg)
        self.banner.delete("all")
        self.banner.create_image(0, 0, anchor="nw", image=self._banner_photo)

    # ---------- file choices ----------

    def _choose_source_dir(self) -> None:
        value = filedialog.askdirectory(title="Choose Ravenloft resource folder")
        if value:
            self.source_dir.set(value)
            self._auto_destinations(Path(value))
            self._validate_source()
            if self.theme.get() == "Strahd's Possession":
                self.apply_theme()

    def _choose_single_anm(self) -> None:
        value = filedialog.askopenfilename(
            title="Choose Ravenloft ANM",
            filetypes=[("Ravenloft ANM", "*.ANM *.anm"), ("All files", "*")],
        )
        if value:
            self.cin_source.set(value)

    @staticmethod
    def _choose_dir(variable: tk.StringVar) -> None:
        value = filedialog.askdirectory(title="Choose output folder")
        if value:
            variable.set(value)

    def _auto_destinations(self, root: Path) -> None:
        base = root / "RLOFT_DUMP"
        defaults = (
            (self.graphics_dest, base / "graphics"),
            (self.audio_dest, base / "audio_music"),
            (self.support_dest, base / "support"),
            (self.cin_frames_dest, base / "cinematics" / "frames"),
            (self.cin_video_dest, base / "cinematics" / "video"),
            (self.all_dest, base),
        )
        for variable, path in defaults:
            if not variable.get():
                variable.set(str(path))

    def _update_cin_source_state(self) -> None:
        state = "disabled" if self.cin_all.get() else "normal"
        self.cin_source_entry.configure(state=state)
        self.cin_source_button.configure(state=state)

    # ---------- validation/thread helpers ----------

    def _source_path(self) -> Path:
        root = Path(self.source_dir.get())
        if not root.is_dir():
            raise RuntimeError("Choose a valid Ravenloft resource folder.")
        return root

    def _validate_source(self) -> None:
        try:
            root = self._source_path()
            result = media_inventory.validate_source_dir(root, verify_hashes=True)
            if result["certified_complete"]:
                self.source_status.set("Certified complete: all 33 files match the audited corpus.")
            else:
                self.source_status.set(
                    f"Missing {len(result['missing'])}; mismatched {len(result['mismatched'])}."
                )
        except Exception as exc:
            self.source_status.set(str(exc))

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
            result = work()
            self._events.put(("done", result))
        except Exception as exc:
            self._events.put(("error", str(exc)))

    def _progress(self, done: int, total: int, text: str) -> None:
        self._events.put(("progress", done, total, text))

    def _master_progress(self, stage: str, done: int, total: int, text: str) -> None:
        self._events.put(("progress", done, total, f"{stage}: {text}"))

    # ---------- actions ----------

    def _start_graphics(self) -> None:
        try:
            root = self._source_path()
            dest = Path(self.graphics_dest.get())
            fmt = self.graphics_format.get()
            palette = int(self.graphics_palette.get())
        except Exception as exc:
            messagebox.showerror("Cannot extract", str(exc))
            return
        self._run(
            "Extracting graphics…",
            lambda: asset_dumper.dump_graphics(
                root, dest, fmt, palette, self.graphics_native.get(), self._progress
            ),
        )

    def _start_audio(self) -> None:
        try:
            root = self._source_path()
            dest = Path(self.audio_dest.get())
            if not str(dest):
                raise RuntimeError("Choose an audio/music destination.")
        except Exception as exc:
            messagebox.showerror("Cannot extract", str(exc))
            return

        def work():
            result = {"audio": asset_dumper.dump_audio(
                root, dest / "sampled_audio", self.audio_format.get(),
                self.audio_native.get(), self._progress
            )}
            if self.dump_music_var.get():
                result["music"] = asset_dumper.dump_music(
                    root, dest / "xmidi", self.audio_native.get(), self._progress
                )
            if self.dump_opl_var.get():
                result["opl2"] = support_dumper.dump_sample_ad(root, dest / "opl2")
            if self.dump_aria_var.get():
                result["aria"] = support_dumper.dump_banks(root, dest / "aria")
            return result

        self._run("Extracting audio and music…", work)

    def _start_support(self) -> None:
        try:
            root = self._source_path()
            dest = Path(self.support_dest.get())
        except Exception as exc:
            messagebox.showerror("Cannot extract", str(exc))
            return

        def work():
            result = {}
            if self.support_palettes.get():
                result["palettes"] = support_dumper.dump_colors(root, dest / "palettes")
            if self.support_text.get():
                result["text"] = support_dumper.dump_text(root, dest / "text")
            if self.support_opl.get():
                result["opl2"] = support_dumper.dump_sample_ad(root, dest / "opl2")
            if self.support_bnk.get():
                result["aria"] = support_dumper.dump_banks(root, dest / "aria")
            if self.support_init.get():
                result["init"] = support_dumper.dump_init(root, dest / "init")
            if self.support_sintab.get():
                result["sintab"] = support_dumper.dump_sintab(root, dest / "renderer_support")
            return result

        self._run("Extracting support assets…", work)

    def _start_cinematics(self) -> None:
        try:
            fps = float(self.cin_fps.get())
            if fps <= 0:
                raise RuntimeError("Presentation FPS must be greater than zero.")
            frame_root = Path(self.cin_frames_dest.get()) if self.cin_frames.get() else None
            video_root = Path(self.cin_video_dest.get()) if self.cin_video.get() else None
            if self.cin_frames.get() and not self.cin_frames_dest.get():
                raise RuntimeError("Choose a frame destination.")
            if self.cin_video.get() and not self.cin_video_dest.get():
                raise RuntimeError("Choose a video destination.")
        except Exception as exc:
            messagebox.showerror("Cannot extract", str(exc))
            return

        if self.cin_all.get():
            try:
                root = self._source_path()
            except Exception as exc:
                messagebox.showerror("Cannot extract", str(exc))
                return
            base = (frame_root or video_root or Path(self.source_dir.get()) / "RLOFT_DUMP" / "cinematics")
            options = master_dumper.DumpOptions(
                cinematic_frame_format=self.cin_frame_format.get(),
                cinematic_video_format=self.cin_video_format.get(),
                cinematic_fps=fps,
                cinematic_frames=self.cin_frames.get(),
                cinematic_videos=self.cin_video.get(),
                preserve_native=self.cin_native.get(),
            )
            self._run(
                "Extracting all cinematics…",
                lambda: master_dumper.dump_cinematics(
                    root, base, options, self._master_progress,
                    frame_root, video_root, base / "timelines"
                ),
            )
            return

        path = Path(self.cin_source.get())
        if not path.is_file():
            messagebox.showerror("Cannot extract", "Choose a valid ANM file.")
            return

        def single():
            movie = anm.parse_anm(path)
            if self.cin_frames.get():
                exporter.export_frames(
                    movie, frame_root / path.stem, self.cin_frame_format.get(), self._progress
                )
            if self.cin_video.get():
                suffix = exporter.VIDEO_FORMATS[self.cin_video_format.get()]
                video_root.mkdir(parents=True, exist_ok=True)
                exporter.export_video(
                    movie, video_root / f"{path.stem}{suffix}",
                    self.cin_video_format.get(), fps, progress=self._progress
                )
            meta_root = frame_root or video_root or path.parent
            meta_root.mkdir(parents=True, exist_ok=True)
            if self.cin_timeline.get():
                exporter.write_timeline(movie, path, meta_root / f"{path.stem}_timeline.json")
            if self.cin_native.get():
                (meta_root / path.name).write_bytes(path.read_bytes())
            return {"source": path.name, "frames": movie.frame_count}

        self._run("Extracting cinematic…", single)

    def _start_all(self) -> None:
        try:
            root = self._source_path()
            dest = Path(self.all_dest.get())
            if not self.all_dest.get():
                raise RuntimeError("Choose a Dump All destination.")
        except Exception as exc:
            messagebox.showerror("Cannot dump", str(exc))
            return
        options = master_dumper.DumpOptions(
            graphics_format=self.graphics_format.get(),
            graphics_palette=int(self.graphics_palette.get()),
            audio_format=self.audio_format.get(),
            cinematic_frame_format=self.cin_frame_format.get(),
            cinematic_video_format="MKV (FFV1 lossless)",
            cinematic_fps=float(self.cin_fps.get()),
            cinematic_frames=self.all_frames.get(),
            cinematic_videos=self.all_videos.get(),
            preserve_native=True,
            export_support=self.all_support.get(),
        )
        self._run(
            "Dumping complete media corpus…",
            lambda: master_dumper.dump_all(root, dest, options, self._master_progress, True),
        )

    # ---------- event loop ----------

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
