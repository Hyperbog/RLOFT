"""Baked title-screen skin for the Ravenloft extractor GUI.

This module contains exact compressed source records from the certified game
corpus, not redrawn approximations:
  * RES0:100 + COLORS palette 12 -- the Strahd's Possession title/menu frame
  * RES0:77  + COLORS palette 13 -- the title-screen red texture

The records are decoded with the same reconstructed type-1 RES decoder used by
the extraction tool.  At runtime no installed Ravenloft files are required.
Artwork is cropped, tiled, or uniformly nearest-neighbour scaled only.
"""
from __future__ import annotations

import base64
from functools import lru_cache

from PIL import Image, ImageDraw

import res_assets


_MENU_ENTRY_B64 = (
"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"
);

_TEXTURE_ENTRY_B64 = (
"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"
);

_PALETTE12_B64 = "AAAAAgIDCgIDBAkECQoEAgQJCQMKBAgLCAsLEgQDGgUEEwgEGwgDEgQJGgQJEwoKGgoJBhEBCRIDBhcBCxoCBhAMCRELDBkKEhMEGxEDERsDGhsDERMKGhEKEhsLGRoJAwQRCQMTBQkSCAwTBAMaCQMbBgoZCQwaEgUSFwUUEQwTGgkREQMbGwQbEQsbGwwcBg8SChETDBgUBg8ZCxIaDRgbERMUGRQUEhoUGhsUEBQcGhQbERkcGxsbIQYEKAYFIwkEKgsEIgUIKQUIIwoJKgsIMA0HIxEDLBEEJRsDLBkDJBEKLBEJIhwMLBgJMhMFNxUFMxkFOBsGMhMJOBUKNBoKORsKJgkTMAkQJRgXNxkRDCIEESIDGiQDEyoCGysDEyIKGiMLEykKGioLGjMFDSsQEyISGyMTFCkQHCoSFCEbGyIbHCkaHDERIycJNCEEOSEFNSoFNSEKOiELMygKOigKJTYJNDUKISMTIisSKioTIiIbKiIcIyoaKSscOSISKDQWNTcZBAIhCgIiBgohCgwhBgIpCgIqCwwpEgMkGgMjEgsiGQsiEgIrGQQsEgoqGwsrBg8gDBIhDhghDRIoDhcoEBQiGRMjEhkiGhsiERQpGxIrExkpGxspDAMwFgUzDhEvGRUxIwooMg4fIhMjKRIjIhsjKhsjIxMrKxMrIhsqKhsrMxkoIws1Mgs2Jhc0Mhg1FSAiGyIiGycjFB8nHCIqHSkpHSIyIiMjKSMjIyojKisjIyMqKiQrJCorKisrMSsqJDEjKzIjLjgkJTArLDEsLjgrMzMkOTUjNDkiOTsiMjMsODQsMzktOTssIyMxKyMyJCoxKysxIyI3KyM4Iio3KSk6MSQyMSwyNywyMSI5Mis5OSw6LTIyLjgxLjQ4MjMyODQyMzkyPDwzMzQ4OTQ6NDk4PT46AAAAAgICBAQEBwcHCgoKDQ0NDw8PEhISFRUVGBgYGhoaHR0dICAgIyMjJSUlKCgoKysrLi4uMDAwMzMzNjY2OTk5Ozs7Pj4+";
_PALETTE13_B64 = "AAAAAAAAAQEBAgICAwMDAwMDBAQEBQUFBgYGCQkJDAwMDw8PEhISFRUVGBgYGxsbHh4eISEhIyMjJiYmKSkpLCwsLy8vMjIyNTU1ODg4Ozs7Pj4+PgA+PgA+PgA+PgA+PgA+PgA+PgA+PgA+PgA+PgA+PgA+PgA+PgA+PgA+PgA+PgA+PgA+PgA+PgA+PgA+AAAAAQAABAEABgIACAMACgMADQQADwUAEQYAFwkAHAwAIg8AJxIALRUAMhgAOBsAOSAAOSQAOikAOi0BOzIBPDUSPTgiPjszAAAAAQAABAAABgAACAAACgAADQAADwAAEQAAFQICGQMDHQUFIQcHJgkJKgoKLgwMMg4ONg8POhEROxgYPB8fPCYmPS0tPjQ0AAAAAAECAQIFAQQIAgULAwcNBAgQBAoTBQsWBw0ZCRAcCxIfDBUiDhclEBooEhwrFiAtGyMwHycyJCs1KC43LDI5MTU8NTk+PgA+PgA+PgA+PgA+PgA+PgA+PgA+PgA+PgA+PgA+PgA+PgA+PgA+PgA+PgA+PgA+PgA+PgA+PgA+PgA+PgA+PgA+PgA+PgA+AAAAAQAABAEABgIACAMACgMADQQADwUAEQYAFwkAHAwAIg8AJxIALRUAMhgAOBsAOSAAOSQAOikAOi0BOzIBPDUSPTgiPjszAAAAAQAABAAABgAACAAACgAADQAADwAAEQAAFQICGQMDHQUFIQcHJgkJKgoKLgwMMg4ONg8POhEROxgYPB8fPCYmPS0tPjQ0AAAAAAECAQIFAQQIAgULAwcNBAgQBAoTBQsWBw0ZCRAcCxIfDBUiDhclEBooEhwrFiAtGyMwHycyJCs1KC43LDI5NTU8NTk+MiIWNCQZNyccOSofOi0jOzAoPDQsPDcwPTo1Pj05AAAAAAAFAAAMAAASAwARBwERCwEQDwIQEgMPFgQPGgQOHgUOIgYNJQ0UKRMbLBoiMCApMycwNy03OjQ+AD4AAD4AAD4AAD4AGwsQIBAXJBYdKRskPgA+PgA+";


def _expand_palette(encoded: str) -> bytes:
    raw = base64.b64decode(encoded)
    if len(raw) != 768:
        raise RuntimeError("baked title palette is corrupt")
    return bytes((value * 255 + 31) // 63 for value in raw)


def _decode_type1_entry(encoded: str, palette: bytes, transparent: int | None) -> Image.Image:
    raw = base64.b64decode(encoded)
    if len(raw) < 5 or raw[:2] != b"EH":
        raise RuntimeError("baked RES0 title resource is corrupt")
    header_size = int.from_bytes(raw[3:5], "little")
    end = 5 + header_size
    resource = res_assets.EhResource(0, -1, raw[2], raw[5:end], raw[end:], raw)
    raster, _ = res_assets.decode_type1(resource)
    height = raster.count * raster.element_height
    indexed = Image.frombytes("P", (raster.width, height), raster.pixels)
    indexed.putpalette(palette)
    if transparent is None:
        return indexed.convert("RGB")
    rgba = indexed.convert("RGBA")
    alpha = indexed.point(lambda value: 0 if value == transparent else 255, mode="L")
    rgba.putalpha(alpha)
    return rgba


@lru_cache(maxsize=1)
def menu_panel() -> Image.Image:
    return _decode_type1_entry(_MENU_ENTRY_B64, _expand_palette(_PALETTE12_B64), 254)


@lru_cache(maxsize=1)
def red_texture() -> Image.Image:
    return _decode_type1_entry(_TEXTURE_ENTRY_B64, _expand_palette(_PALETTE13_B64), None)


BASE_PANEL_SIZE = (208, 191)
TITLE_CROP = (42, 0, 166, 66)
TOP_LEFT = (0, 0, 54, 62)
TOP_RIGHT = (154, 0, 208, 62)
BOTTOM_LEFT = (0, 129, 62, 191)
BOTTOM_RIGHT = (146, 129, 208, 191)
SIDE_LEFT = (0, 57, 39, 140)
SIDE_RIGHT = (169, 57, 208, 140)
BOTTOM_CENTER = (63, 145, 145, 191)


def _scaled_crop(panel: Image.Image, box: tuple[int, int, int, int], scale: float) -> Image.Image:
    crop = panel.crop(box)
    if abs(scale - 1.0) < 0.001:
        return crop
    size = (max(1, round(crop.width * scale)), max(1, round(crop.height * scale)))
    return crop.resize(size, Image.Resampling.NEAREST)


def chrome_insets(scale: float = 1.0) -> tuple[int, int, int]:
    return (
        max(38, round(46 * scale)),
        max(66, round(76 * scale)),
        max(36, round(48 * scale)),
    )


def render_window(width: int, height: int, ui_scale: float = 1.0) -> Image.Image:
    width = max(1, int(width))
    height = max(1, int(height))
    scale = max(0.70, min(1.45, float(ui_scale)))

    texture = red_texture()
    out = Image.new("RGB", (width, height), "#170e0d")
    for y in range(0, height, texture.height):
        for x in range(0, width, texture.width):
            tile = texture.crop((0, 0, min(texture.width, width - x), min(texture.height, height - y)))
            out.paste(tile, (x, y))
    out = Image.blend(out, Image.new("RGB", out.size, "#140b0b"), 0.34)

    panel = menu_panel()
    left = _scaled_crop(panel, SIDE_LEFT, scale)
    right = _scaled_crop(panel, SIDE_RIGHT, scale)
    tl = _scaled_crop(panel, TOP_LEFT, scale)
    tr = _scaled_crop(panel, TOP_RIGHT, scale)
    bl = _scaled_crop(panel, BOTTOM_LEFT, scale)
    br = _scaled_crop(panel, BOTTOM_RIGHT, scale)
    bc = _scaled_crop(panel, BOTTOM_CENTER, scale)
    title = _scaled_crop(panel, TITLE_CROP, scale)

    draw = ImageDraw.Draw(out)
    rail = (105, 103, 98)
    side_x = max(0, round(8 * scale))
    right_x = max(0, width - right.width - side_x)
    top_y = max(0, round(8 * scale))
    bottom_y = max(0, height - bl.height - round(8 * scale))

    rail_y = top_y + max(2, round(15 * scale))
    draw.rectangle((side_x + tl.width - 8, rail_y, max(side_x + tl.width, right_x + 8), rail_y + max(3, round(5 * scale))), fill=rail)
    rail_bottom = max(0, height - round(18 * scale))
    draw.rectangle((side_x + bl.width - 8, rail_bottom, max(side_x + bl.width, right_x + 8), rail_bottom + max(3, round(5 * scale))), fill=rail)

    def paste(img: Image.Image, xy: tuple[int, int]) -> None:
        out.paste(img, xy, img if img.mode == "RGBA" else None)

    paste(tl, (side_x, top_y))
    paste(tr, (right_x, top_y))
    paste(bl, (side_x, bottom_y))
    paste(br, (max(0, width - br.width - side_x), bottom_y))

    # Draw clean vertical stone rails, then place exactly one of the original
    # side ornaments on each edge. Repeating the side crop created the
    # "columns through the background" artifact in earlier builds.
    rail_w = max(4, round(7 * scale))
    left_rail_x = side_x + max(2, round(9 * scale))
    right_rail_x = max(0, width - side_x - max(2, round(9 * scale)) - rail_w)
    rail_top = top_y + max(18, round(24 * scale))
    rail_bottom = max(rail_top, bottom_y + max(6, round(12 * scale)))
    draw.rectangle(
        (left_rail_x, rail_top, left_rail_x + rail_w, rail_bottom),
        fill=rail,
    )
    draw.rectangle(
        (right_rail_x, rail_top, right_rail_x + rail_w, rail_bottom),
        fill=rail,
    )

    ornament_y = max(
        rail_top,
        min(
            rail_bottom - left.height,
            (height - left.height) // 2,
        ),
    )
    paste(left, (side_x, ornament_y))
    paste(right, (right_x, ornament_y))

    # Bottom finial and Strahd's Possession plaque use the exact original
    # title-screen pixels and are always drawn last so they sit above rails.
    paste(
        bc,
        (
            (width - bc.width) // 2,
            max(0, height - bc.height - round(3 * scale)),
        ),
    )
    paste(title, ((width - title.width) // 2, max(0, round(4 * scale))))
    return out
