"""Delayed, non-intrusive hover tooltips for the extractor GUI."""
from __future__ import annotations

import tkinter as tk
from tkinter import ttk


SPECIAL = {
    "Browse…": "Choose a folder on disk. Nothing is modified until you start an extraction.",
    "Validate 33 files": "Verify the selected Ravenloft media/resource corpus by exact size and SHA-256.",
    "All": "Select every file in this category.",
    "None": "Clear the current category selection.",
    "Individual frames": "Write every cinematic frame as an individual image sequence.",
    "Rendered video": "Render the selected cinematic(s) to the chosen video container and codec.",
    "Sync sampled SFX into video": "Mix ANM-triggered sampled sound effects into the rendered movie at reconstructed native timestamps.",
    "Timeline JSON": "Write a machine-readable timeline containing frame timing and ANM command events.",
    "Preserve original ANM": "Copy the original ANM beside the derived cinematic exports.",
    "Extract Selected Cinematics": "Extract only the cinematics selected in the list above.",
    "Extract Selected Graphics": "Extract graphics from only the selected RES archives.",
    "Sampled audio": "Extract Creative Voice File sampled audio from the selected audio archives.",
    "XMIDI music": "Extract Miles XMIDI/XDIR music from the selected music archives.",
    "SAMPLE.AD OPL2 timbres": "Extract the game's Miles AIL YM3812/OPL2 instrument definitions.",
    "GM1.BNK / GM2.BNK ARIA banks": "Preserve the original ARIA General MIDI support banks and identification metadata.",
    "Extract Selected Audio & Music": "Extract the enabled audio/music categories from only the selected source archives.",
    "Extract Selected Support Assets": "Extract only the checked standalone support-data categories.",
    "Cinematic frame sequences": "Include every cinematic as individual lossless frame images in Dump All.",
    "Also render native-timed FFV1/MKV movies": "Also create lossless movies using reconstructed native variable frame timing.",
    "Sync sampled cinematic SFX": "Mix sampled ANM sound-effect commands into Dump All cinematic videos.",
    "Standalone support assets": "Include palettes, text/subtitles, OPL2 timbres, ARIA banks, INIT mappings, and SINTAB data.",
    "Dump Everything": "Extract the complete audited media/resource corpus using the options selected in the other tabs.",
    "Canonical palette-index bytes (.indices)": "Recommended for a faithful modern port. Keeps the original 8-bit palette indices independently of the preview palette.",
    "Decoded indices (.indices)": "Recommended for a faithful modern port. Keeps the original 8-bit palette indices independently of the preview palette.",
    "Metadata / compiled-shape data": "Write JSON metadata plus decoded type-7 shape data used to reconstruct NPCs, monsters, objects, items, and effects.",
    "Metadata / shape data (.json/.bin)": "Write JSON metadata plus decoded type-7 shape data used to reconstruct NPCs, monsters, objects, items, and effects.",
    "Transparency mask PNGs": "Write a separate 8-bit alpha mask for scene-shape images. Optional when the chosen image format already carries alpha.",
    "Transparency masks (.mask.png)": "Write a separate 8-bit alpha mask for scene-shape images. Optional when the chosen image format already carries alpha.",
    "Native RES/EH records": "Archival option. Preserve each original typed RES record beside the decoded asset.",
    "Original EH records (.eh)": "Archival option. Preserve each original typed RES record beside the decoded asset.",
}


class HoverTip:
    def __init__(self, widget: tk.Widget, text: str, delay_ms: int = 800):
        self.widget = widget
        self.text = text
        self.delay_ms = delay_ms
        self._after: str | None = None
        self._tip: tk.Toplevel | None = None
        widget.bind("<Enter>", self._schedule, add="+")
        widget.bind("<Leave>", self._hide, add="+")
        widget.bind("<ButtonPress>", self._hide, add="+")
        widget.bind("<Destroy>", self._hide, add="+")

    def _schedule(self, _event=None) -> None:
        self._cancel()
        self._after = self.widget.after(self.delay_ms, self._show)

    def _cancel(self) -> None:
        if self._after is not None:
            try:
                self.widget.after_cancel(self._after)
            except tk.TclError:
                pass
            self._after = None

    def _show(self) -> None:
        self._after = None
        if self._tip is not None or not self.widget.winfo_exists():
            return
        try:
            x = self.widget.winfo_pointerx() + 14
            y = self.widget.winfo_pointery() + 18
        except tk.TclError:
            return
        tip = tk.Toplevel(self.widget)
        tip.wm_overrideredirect(True)
        try:
            tip.wm_attributes("-topmost", True)
        except tk.TclError:
            pass
        tip.configure(background="#0d0908")
        label = tk.Label(
            tip,
            text=self.text,
            justify="left",
            background="#17100e",
            foreground="#eadfc9",
            relief="solid",
            borderwidth=1,
            padx=8,
            pady=6,
            wraplength=360,
        )
        label.pack()
        tip.geometry(f"+{x}+{y}")
        self._tip = tip

    def _hide(self, _event=None) -> None:
        self._cancel()
        if self._tip is not None:
            try:
                self._tip.destroy()
            except tk.TclError:
                pass
            self._tip = None


def _default_text(widget: tk.Widget) -> str | None:
    if isinstance(widget, ttk.Button):
        text = str(widget.cget("text"))
        return SPECIAL.get(text, f"Click to {text.lower()}.")
    if isinstance(widget, ttk.Checkbutton):
        text = str(widget.cget("text"))
        return SPECIAL.get(text, f"Enable or disable: {text}.")
    if isinstance(widget, ttk.Combobox):
        return "Choose one of the available output or processing options."
    if isinstance(widget, ttk.Spinbox):
        return "Choose or type a numeric value."
    if isinstance(widget, ttk.Entry):
        return "Enter or edit this path or numeric value."
    if isinstance(widget, ttk.Progressbar):
        return "Overall progress for the current extraction operation."
    if isinstance(widget, ttk.Notebook):
        return "Switch between cinematic, graphics, audio/music, support-data, and complete-dump workflows."
    if isinstance(widget, tk.Listbox):
        return "Select one or more source files. Use Ctrl/Shift for multi-selection, or the All/None buttons."
    return None


def install(root: tk.Widget) -> list[HoverTip]:
    """Attach one delayed tooltip to every interactive control below root."""
    tips: list[HoverTip] = []

    def walk(widget: tk.Widget) -> None:
        text = _default_text(widget)
        if text:
            tips.append(HoverTip(widget, text))
        for child in widget.winfo_children():
            walk(child)

    walk(root)
    return tips
