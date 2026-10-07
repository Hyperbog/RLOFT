"""Windows-style secondary edit menu for extractor text fields."""
from __future__ import annotations

from dataclasses import dataclass
import tkinter as tk
from tkinter import ttk
import weakref


@dataclass
class _History:
    values: list[str]
    index: int
    applying: bool = False


class EditMenuController:
    """Adds right-click editing and lightweight undo/redo to entry controls."""

    EDIT_CLASSES = ("Entry", "TEntry", "TSpinbox", "TCombobox")

    def __init__(self, root: tk.Misc) -> None:
        self.root = root
        self.target: tk.Misc | None = None
        self.histories: weakref.WeakKeyDictionary[tk.Misc, _History] = (
            weakref.WeakKeyDictionary()
        )
        self.menu = tk.Menu(root, tearoff=False)
        self.menu.add_command(label="Undo", command=self.undo, accelerator="Ctrl+Z")
        self.menu.add_command(label="Redo", command=self.redo, accelerator="Ctrl+Y")
        self.menu.add_separator()
        self.menu.add_command(label="Cut", command=self.cut, accelerator="Ctrl+X")
        self.menu.add_command(label="Copy", command=self.copy, accelerator="Ctrl+C")
        self.menu.add_command(label="Paste", command=self.paste, accelerator="Ctrl+V")
        self.menu.add_command(label="Delete", command=self.delete_selection, accelerator="Del")
        self.menu.add_separator()
        self.menu.add_command(label="Select All", command=self.select_all, accelerator="Ctrl+A")

        for cls in self.EDIT_CLASSES:
            root.bind_class(cls, "<Button-3>", self._popup, add="+")
            root.bind_class(cls, "<Shift-F10>", self._popup_keyboard, add="+")
            root.bind_class(cls, "<KeyRelease>", self._changed_event, add="+")
            root.bind_class(cls, "<<Paste>>", self._changed_event, add="+")
            root.bind_class(cls, "<<Cut>>", self._changed_event, add="+")
            root.bind_class(cls, "<<Clear>>", self._changed_event, add="+")
            root.bind_class(cls, "<Control-z>", self._undo_event, add="+")
            root.bind_class(cls, "<Control-y>", self._redo_event, add="+")
            root.bind_class(cls, "<Control-Shift-Z>", self._redo_event, add="+")

    @staticmethod
    def _text(widget: tk.Misc) -> str:
        try:
            return str(widget.get())
        except (tk.TclError, AttributeError):
            return ""

    @staticmethod
    def _editable(widget: tk.Misc) -> bool:
        try:
            state = str(widget.cget("state"))
        except (tk.TclError, AttributeError):
            state = "normal"
        return state not in ("disabled", "readonly")

    @staticmethod
    def _has_selection(widget: tk.Misc) -> bool:
        try:
            return bool(widget.selection_present())
        except (tk.TclError, AttributeError):
            return False

    def _history(self, widget: tk.Misc) -> _History:
        history = self.histories.get(widget)
        if history is None:
            value = self._text(widget)
            history = _History([value], 0)
            self.histories[widget] = history
        return history

    def _record(self, widget: tk.Misc) -> None:
        history = self._history(widget)
        if history.applying:
            return
        value = self._text(widget)
        if history.values[history.index] == value:
            return
        del history.values[history.index + 1:]
        history.values.append(value)
        # Keep memory bounded while preserving plenty of normal edit history.
        if len(history.values) > 200:
            history.values = history.values[-200:]
        history.index = len(history.values) - 1

    def _changed_event(self, event) -> None:
        widget = event.widget
        self.root.after_idle(lambda w=widget: self._record(w))

    def _replace(self, widget: tk.Misc, value: str) -> None:
        history = self._history(widget)
        history.applying = True
        try:
            widget.delete(0, tk.END)
            widget.insert(0, value)
            try:
                widget.icursor(tk.END)
            except (tk.TclError, AttributeError):
                pass
        finally:
            history.applying = False

    def _undo_event(self, event) -> str:
        self.target = event.widget
        self._record(event.widget)
        self.undo()
        return "break"

    def _redo_event(self, event) -> str:
        self.target = event.widget
        self.redo()
        return "break"

    def undo(self) -> None:
        widget = self.target
        if widget is None or not self._editable(widget):
            return
        history = self._history(widget)
        self._record(widget)
        if history.index <= 0:
            return
        history.index -= 1
        self._replace(widget, history.values[history.index])

    def redo(self) -> None:
        widget = self.target
        if widget is None or not self._editable(widget):
            return
        history = self._history(widget)
        if history.index >= len(history.values) - 1:
            return
        history.index += 1
        self._replace(widget, history.values[history.index])

    def cut(self) -> None:
        widget = self.target
        if widget is None or not self._editable(widget):
            return
        widget.event_generate("<<Cut>>")
        self.root.after_idle(lambda w=widget: self._record(w))

    def copy(self) -> None:
        if self.target is not None:
            self.target.event_generate("<<Copy>>")

    def paste(self) -> None:
        widget = self.target
        if widget is None or not self._editable(widget):
            return
        widget.event_generate("<<Paste>>")
        self.root.after_idle(lambda w=widget: self._record(w))

    def delete_selection(self) -> None:
        widget = self.target
        if widget is None or not self._editable(widget):
            return
        try:
            widget.delete("sel.first", "sel.last")
        except tk.TclError:
            return
        self._record(widget)

    def select_all(self) -> None:
        widget = self.target
        if widget is None:
            return
        try:
            widget.selection_range(0, tk.END)
            widget.icursor(tk.END)
        except (tk.TclError, AttributeError):
            pass

    def _update_menu_state(self, widget: tk.Misc) -> None:
        editable = self._editable(widget)
        selected = self._has_selection(widget)
        history = self._history(widget)
        self._record(widget)

        self.menu.entryconfigure(
            "Undo", state="normal" if editable and history.index > 0 else "disabled"
        )
        self.menu.entryconfigure(
            "Redo",
            state=(
                "normal"
                if editable and history.index < len(history.values) - 1
                else "disabled"
            ),
        )
        self.menu.entryconfigure(
            "Cut", state="normal" if editable and selected else "disabled"
        )
        self.menu.entryconfigure(
            "Copy", state="normal" if selected else "disabled"
        )
        try:
            self.root.clipboard_get()
            can_paste = editable
        except tk.TclError:
            can_paste = False
        self.menu.entryconfigure(
            "Paste", state="normal" if can_paste else "disabled"
        )
        self.menu.entryconfigure(
            "Delete", state="normal" if editable and selected else "disabled"
        )

    def _show(self, widget: tk.Misc, x_root: int, y_root: int) -> str:
        self.target = widget
        try:
            widget.focus_set()
        except (tk.TclError, AttributeError):
            pass
        self._update_menu_state(widget)
        try:
            self.menu.tk_popup(int(x_root), int(y_root))
        finally:
            try:
                self.menu.grab_release()
            except tk.TclError:
                pass
        return "break"

    def _popup(self, event) -> str:
        return self._show(event.widget, event.x_root, event.y_root)

    def _popup_keyboard(self, event) -> str:
        widget = event.widget
        x = widget.winfo_rootx() + max(8, widget.winfo_width() // 3)
        y = widget.winfo_rooty() + max(8, widget.winfo_height())
        return self._show(widget, x, y)


def install(root: tk.Misc) -> EditMenuController:
    return EditMenuController(root)
