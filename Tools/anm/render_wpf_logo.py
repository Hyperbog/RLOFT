#!/usr/bin/env python3
"""Reconstruct the WPF branding from the indexed logo archived in title_skin.

GitHub's old binary PNG transport produced a truncated image. Rather than
embedding that damaged copy in the modern executable, regenerate the crest
deterministically from the original indexed bitmap + palette definitions.
"""
from __future__ import annotations

import base64
import sys
import zlib
from pathlib import Path

from PIL import Image

import title_skin


def make_logo(destination: Path) -> Path:
    destination = Path(destination)
    # These two constants are extracted original game data, not approximated
    # colored UI icons. Pixel indices are preserved until RGBA conversion.
    data = zlib.decompress(base64.b64decode(title_skin._RAVENLOFT_LOGO_PIXELS_B64))
    width, height = 240, 114
    if len(data) != width * height:
        raise RuntimeError(
            f"Original Ravenloft crest indices: expected {width * height}, got {len(data)}"
        )
    palette = bytes.fromhex(title_skin._RAVENLOFT_LOGO_PALETTE_HEX)
    if len(palette) % 3 or len(palette) > 768:
        raise RuntimeError("Original Ravenloft crest palette is invalid")
    indexed = Image.frombytes("P", (width, height), data)
    indexed.putpalette(palette + bytes(768 - len(palette)))
    background = data[0]
    rgba = indexed.convert("RGBA")
    rgba.putalpha(
        Image.frombytes(
            "L", (width, height),
            bytes(0 if pixel == background else 255 for pixel in data)
        )
    )
    destination.parent.mkdir(parents=True, exist_ok=True)
    rgba.save(destination, "PNG", optimize=True)
    with Image.open(destination) as reopened:
        reopened.verify()
    return destination


if __name__ == "__main__":
    if len(sys.argv) != 2:
        raise SystemExit("Usage: render_wpf_logo.py OUTPUT.png")
    print(make_logo(Path(sys.argv[1])))
