#!/usr/bin/env python3
"""Tk GUI for the Ravenloft ANM cinematic extractor."""
from __future__ import annotations

import queue
import threading
import tkinter as tk
from pathlib import Path
from tkinter import filedialog, messagebox, ttk

from PIL import Image, ImageTk

import exporter
import ravenloft_resources as rr
import rloft_anm as anm


class AnmExtractorGui(tk.Tk):
    def __init__(self) -> None:
        super().__init__()
        self.title("Ravenloft ANM Extractor")
        self.geometry("920x690")
        self.minsize(820, 620)

        self.source = tk.StringVar()
        self.theme = tk.StringVar(value="Modern")
        self.game_dir = tk.StringVar()
        self.export_frames_var = tk.BooleanVar(value=True)
        self.frame_format = tk.StringVar(value="PNG")
        self.frame_dest = tk.StringVar()
        self.export_video_var = tk.BooleanVar(value=False)
        self.video_format = tk.StringVar(value="MP4 (H.264)")
        self.video_dest = tk.StringVar()
        self.fps = tk.StringVar(value="15")
        self.write_timeline_var = tk.BooleanVar(value=True)
        self.status = tk.StringVar(value="Choose a CINE*.ANM file.")
        self.progress_value = tk.DoubleVar(value=0)

        self._theme_images: list[ImageTk.PhotoImage] = []
        self._event_queue: queue.Queue[tuple] = queue.Queue()
        self._build_styles()
        self._build_ui()
        self.after(100, self._poll_events)

    def _build_styles(self) -> None:
        style = ttk.Style(self)
        if "clam" in style.theme_names():
            style.theme_use("clam")
        style.configure("Modern.TFrame", background="#20242b")
        style.configure("Modern.TLabel", background="#20242b", foreground="#f1f3f5")
        style.configure("Modern.Title.TLabel", background="#20242b", foreground="#ffffff", font=("TkDefaultFont", 16, "bold"))
        style.configure("Strahd.TFrame", background="#100f10")
        style.configure("Strahd.TLabel", background="#100f10", foreground="#d8d0c1")
        style.configure("Strahd.Title.TLabel", background="#100f10", foreground="#c8b68a", font=("TkDefaultFont", 16, "bold"))
        style.configure("Strahd.TButton", background="#3a3436", foreground="#e8dfce", bordercolor="#7a181b", focusthickness=2, focuscolor="#7a181b")
        style.map("Strahd.TButton", background=[("active", "#581b1e")])

    def _build_ui(self) -> None:
        self.root_frame = ttk.Frame(self, padding=16, style="Modern.TFrame")
        self.root_frame.pack(fill="both", expand=True)

        self.header = ttk.Frame(self.root_frame, style="Modern.TFrame")
        self.header.pack(fill="x", pady=(0, 12))
        self.title_label = ttk.Label(self.header, text="Ravenloft ANM Extractor", style="Modern.Title.TLabel")
        self.title_label.pack(side="left")
        self.theme_combo = ttk.Combobox(self.header, textvariable=self.theme, values=("Modern", "Strahd's Possession"), state="readonly", width=20)
        self.theme_combo.pack(side="right")
        self.theme_combo.bind("<<ComboboxSelected>>", lambda _e: self.apply_theme())

        self.theme_art = ttk.Label(self.root_frame, style="Modern.TLabel")

        source_panel = ttk.LabelFrame(self.root_frame, text="Source cinematic", padding=10)
        source_panel.pack(fill="x", pady=6)
        source_panel.columnconfigure(1, weight=1)
        self._row_file(source_panel, 0, "ANM file", self.source, self._choose_source)

        self.game_row = ttk.Frame(source_panel)
        self.game_row.grid(row=1, column=0, columnspan=3, sticky="ew", pady=(8, 0))
        self.game_row.columnconfigure(1, weight=1)
        ttk.Label(self.game_row, text="Ravenloft folder").grid(row=0, column=0, sticky="w", padx=(0, 8))
        ttk.Entry(self.game_row, textvariable=self.game_dir).grid(row=0, column=1, sticky="ew")
        ttk.Button(self.game_row, text="Browse…", command=self._choose_game_dir).grid(row=0, column=2, padx=(8, 0))

        export_panel = ttk.LabelFrame(self.root_frame, text="Exports", padding=10)
        export_panel.pack(fill="x", pady=6)
        export_panel.columnconfigure(1, weight=1)

        ttk.Checkbutton(export_panel, text="Export individual frames", variable=self.export_frames_var).grid(row=0, column=0, sticky="w")
        ttk.Combobox(export_panel, textvariable=self.frame_format, values=tuple(exporter.FRAME_FORMATS), state="readonly", width=18).grid(row=0, column=1, sticky="w", padx=8)
        ttk.Button(export_panel, text="Choose frame folder…", command=self._choose_frame_dest).grid(row=0, column=2, sticky="e")
        ttk.Entry(export_panel, textvariable=self.frame_dest).grid(row=1, column=0, columnspan=3, sticky="ew", pady=(5, 10))

        ttk.Checkbutton(export_panel, text="Export video", variable=self.export_video_var).grid(row=2, column=0, sticky="w")
        ttk.Combobox(export_panel, textvariable=self.video_format, values=tuple(exporter.VIDEO_FORMATS), state="readonly", width=22).grid(row=2, column=1, sticky="w", padx=8)
        ttk.Button(export_panel, text="Choose video file…", command=self._choose_video_dest).grid(row=2, column=2, sticky="e")
        ttk.Entry(export_panel, textvariable=self.video_dest).grid(row=3, column=0, columnspan=3, sticky="ew", pady=(5, 8))

        options = ttk.Frame(export_panel)
        options.grid(row=4, column=0, columnspan=3, sticky="ew")
        ttk.Label(options, text="Video FPS").pack(side="left")
        ttk.Entry(options, textvariable=self.fps, width=8).pack(side="left", padx=(6, 20))
        ttk.Checkbutton(options, text="Write timeline.json", variable=self.write_timeline_var).pack(side="left")

        action_panel = ttk.Frame(self.root_frame, style="Modern.TFrame")
        action_panel.pack(fill="x", pady=(12, 0))
        self.start_button = ttk.Button(action_panel, text="Extract / Export", command=self._start)
        self.start_button.pack(side="right")
        self.progress = ttk.Progressbar(action_panel, variable=self.progress_value, maximum=100)
        self.progress.pack(side="left", fill="x", expand=True, padx=(0, 12))
        self.status_label = ttk.Label(self.root_frame, textvariable=self.status, style="Modern.TLabel", anchor="w")
        self.status_label.pack(fill="x", pady=(8, 0))

    def _row_file(self, parent: ttk.Widget, row: int, label: str, variable: tk.StringVar, command) -> None:
        ttk.Label(parent, text=label).grid(row=row, column=0, sticky="w", padx=(0, 8))
        ttk.Entry(parent, textvariable=variable).grid(row=row, column=1, sticky="ew")
        ttk.Button(parent, text="Browse…", command=command).grid(row=row, column=2, padx=(8, 0))

    def _choose_source(self) -> None:
        value = filedialog.askopenfilename(title="Choose Ravenloft ANM", filetypes=[("Ravenloft cinematics", "*.ANM *.anm"), ("All files", "*")])
        if not value:
            return
        self.source.set(value)
        source = Path(value)
        if not self.frame_dest.get():
            self.frame_dest.set(str(source.parent / f"{source.stem}_frames"))
        if not self.video_dest.get():
            self.video_dest.set(str(source.parent / f"{source.stem}.mp4"))
        found = rr.find_game_dir(source)
        if found and not self.game_dir.get():
            self.game_dir.set(str(found))
        if self.theme.get() == "Strahd's Possession":
            self.apply_theme()

    def _choose_game_dir(self) -> None:
        value = filedialog.askdirectory(title="Choose Ravenloft game folder")
        if value:
            self.game_dir.set(value)
            if self.theme.get() == "Strahd's Possession":
                self.apply_theme()

    def _choose_frame_dest(self) -> None:
        value = filedialog.askdirectory(title="Choose frame output folder")
        if value:
            self.frame_dest.set(value)

    def _choose_video_dest(self) -> None:
        label = self.video_format.get()
        suffix = exporter.VIDEO_FORMATS[label]
        value = filedialog.asksaveasfilename(title="Choose video destination", defaultextension=suffix, filetypes=[(label, f"*{suffix}"), ("All files", "*")])
        if value:
            self.video_dest.set(value)

    def _theme_banner(self, assets: rr.StrahdThemeAssets) -> Image.Image:
        """Compose a banner only from shipped RES0 imagery."""
        background = assets.frame.resize((760, 475), Image.Resampling.NEAREST).crop((0, 0, 760, 125))
        ornament = assets.ornament.resize((240, 128), Image.Resampling.NEAREST)
        x = (background.width - ornament.width) // 2
        background.paste(ornament, (x, -2))
        return background

    def apply_theme(self) -> None:
        strahd = self.theme.get() == "Strahd's Possession"
        prefix = "Strahd" if strahd else "Modern"
        self.root_frame.configure(style=f"{prefix}.TFrame")
        self.header.configure(style=f"{prefix}.TFrame")
        self.title_label.configure(style=f"{prefix}.Title.TLabel")
        self.status_label.configure(style=f"{prefix}.TLabel")
        self.start_button.configure(style="Strahd.TButton" if strahd else "TButton")

        if not strahd:
            self.theme_art.pack_forget()
            self._theme_images.clear()
            self.status.set("Modern theme enabled.")
            return

        game_dir = Path(self.game_dir.get()) if self.game_dir.get() else None
        if not game_dir:
            source = Path(self.source.get()) if self.source.get() else Path.cwd()
            game_dir = rr.find_game_dir(source)
        if not game_dir:
            self.status.set("Strahd theme: choose a Ravenloft folder containing RES0 and COLORS.")
            return
        try:
            assets = rr.StrahdThemeAssets.load(game_dir)
            art = ImageTk.PhotoImage(self._theme_banner(assets))
            self._theme_images[:] = [art]
            self.theme_art.configure(image=art, style="Strahd.TLabel")
            children = self.root_frame.winfo_children()
            before = children[2] if len(children) > 2 else None
            self.theme_art.pack(fill="x", pady=(0, 6), before=before)
            self.status.set(f"Strahd theme loaded from {game_dir}")
        except (OSError, rr.ResourceError) as exc:
            self.status.set(f"Strahd theme unavailable: {exc}")

    def _validate(self) -> tuple[Path, float]:
        source = Path(self.source.get())
        if not source.is_file():
            raise anm.AnmError("Choose a valid ANM source file")
        if not self.export_frames_var.get() and not self.export_video_var.get() and not self.write_timeline_var.get():
            raise anm.AnmError("Select at least one output")
        if self.export_frames_var.get() and not self.frame_dest.get():
            raise anm.AnmError("Choose a destination folder for frames")
        if self.export_video_var.get() and not self.video_dest.get():
            raise anm.AnmError("Choose a destination file for video")
        try:
            fps = float(self.fps.get())
        except ValueError as exc:
            raise anm.AnmError("Video FPS must be a number") from exc
        if fps <= 0:
            raise anm.AnmError("Video FPS must be greater than zero")
        return source, fps

    def _start(self) -> None:
        try:
            source, fps = self._validate()
        except anm.AnmError as exc:
            messagebox.showerror("Cannot export", str(exc))
            return
        self.start_button.configure(state="disabled")
        self.progress_value.set(0)
        self.status.set("Parsing cinematic…")
        threading.Thread(target=self._worker, args=(source, fps), daemon=True).start()

    def _worker(self, source_path: Path, fps: float) -> None:
        def progress(done: int, total: int, text: str) -> None:
            self._event_queue.put(("progress", done, total, text))
        try:
            source = anm.parse_anm(source_path)
            if self.export_frames_var.get():
                exporter.export_frames(source, Path(self.frame_dest.get()), self.frame_format.get(), progress)
            if self.export_video_var.get():
                exporter.export_video(source, Path(self.video_dest.get()), self.video_format.get(), fps, progress=progress)
            if self.write_timeline_var.get():
                base = Path(self.frame_dest.get()) if self.frame_dest.get() else Path(self.video_dest.get()).parent
                base.mkdir(parents=True, exist_ok=True)
                exporter.write_timeline(source, source_path, base / "timeline.json")
            self._event_queue.put(("done",))
        except Exception as exc:
            self._event_queue.put(("error", str(exc)))

    def _poll_events(self) -> None:
        try:
            while True:
                event = self._event_queue.get_nowait()
                if event[0] == "progress":
                    _, done, total, text = event
                    self.progress_value.set((done / max(1, total)) * 100)
                    self.status.set(text)
                elif event[0] == "done":
                    self.progress_value.set(100)
                    self.status.set("Export complete.")
                    self.start_button.configure(state="normal")
                    messagebox.showinfo("Ravenloft ANM Extractor", "Export complete.")
                elif event[0] == "error":
                    self.start_button.configure(state="normal")
                    self.status.set("Export failed.")
                    messagebox.showerror("Export failed", event[1])
        except queue.Empty:
            pass
        self.after(100, self._poll_events)


def main() -> None:
    app = AnmExtractorGui()
    app.mainloop()


if __name__ == "__main__":
    main()
