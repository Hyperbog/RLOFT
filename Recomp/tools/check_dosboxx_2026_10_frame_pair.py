#!/usr/bin/env python3
"""Check that one DOSBox-X screenshot and save state describe the same frame.

Read-only, version-specific audit. Never promote a screen match to
independent DOS/native parity without original CRTC/DAC/index equality.
Requires Pillow for PNG reading; no original game media are distributed.
"""
import argparse
import hashlib
import json
import zipfile
from pathlib import Path

from PIL import Image

VERSION = b"DOSBox-X 2026.10.01 (SDL2)"
STATE_SIZE = 2643024
CRTC_OFFSET = 0x82F8C
DAC_OFFSET = 0x82FB1
PALETTE_OFFSET = 0x82FD1
VRAM_OFFSET = 0x84A40
PIXELS = 320 * 400


def sha256(payload):
    return hashlib.sha256(payload).hexdigest()


def require(predicate, why):
    if not predicate:
        raise ValueError(why)


def screenshot_pixels(path):
    image = Image.open(path).convert("RGB")
    width, height = image.size
    if (width, height) == (320, 400):
        crop = image
        kind = "native 320x400 DOSBox screenshot"
    elif (width, height) == (640, 400):
        crop = image.crop((0, 0, 640, 400)).resize(
            (320, 400), Image.Resampling.NEAREST)
        kind = "640x400 horizontally doubled; downsampled"
    elif (width, height) == (640, 800):
        crop = image.resize((320, 400), Image.Resampling.NEAREST)
        kind = "640x800 2x frame; downsampled"
    elif (width, height) == (642, 452):
        # Only this exact known SDL2 window/chrome geometry was
        # independently validated on the owner's first screenshot.
        crop = image.crop((1, 51, 641, 451)).resize(
            (320, 400), Image.Resampling.NEAREST)
        kind = "known 642x452 SDL2 screenshot with window chrome"
    else:
        raise ValueError(
            "Unknown screenshot dimensions %dx%d; cannot guess crop" %
            (width, height))
    return crop.tobytes(), kind


def compare(original, observed):
    require(len(original) == len(observed), "Image byte lengths differ")
    count = sum(original[i:i+3] == observed[i:i+3]
                for i in range(0, len(original), 3))
    per_row = []
    for y in range(400):
        a = original[y*960:(y+1)*960]
        b = observed[y*960:(y+1)*960]
        per_row.append(sum(a[x:x+3] == b[x:x+3]
                           for x in range(0, 960, 3)))
    return count, per_row


def main():
    arg = argparse.ArgumentParser(description=__doc__)
    arg.add_argument("--state", required=True, type=Path)
    arg.add_argument("--screenshot", required=True, type=Path)
    arg.add_argument("--report", required=True, type=Path)
    arg.add_argument("--native-index", type=Path)
    arg.add_argument("--native-palette", type=Path)
    opt = arg.parse_args()
    with zipfile.ZipFile(opt.state) as archive:
        require(archive.read("DOSBox-X_Version") == VERSION,
                "Unverified DOSBox-X save-state version")
        require(archive.read("Machine_Type") == b"MCH_VGA",
                "Saved state is not VGA")
        state = archive.read("Vga")
        program = archive.read("Program_Name").decode("ascii", "replace")
    require(len(state) == STATE_SIZE, "Unrecognized serialized VGA size")
    crtc = state[CRTC_OFFSET:CRTC_OFFSET+25]
    require(crtc[:4] == bytes([0x5F, 0x4F, 0x50, 0x82]),
            "CRTC layout signature mismatch")
    require(crtc[19] == 0x28 and crtc[23] == 0xE3,
            "Unsupported VGA scanout stride/mode")
    require(state[DAC_OFFSET] == 6 and state[DAC_OFFSET+1] == 255,
            "Unexpected RGB precision or DAC PEL mask")
    pal = state[PALETTE_OFFSET:PALETTE_OFFSET+768]
    require(len(pal) == 768 and max(pal) <= 63, "Bad RGB6 DAC")
    video = state[VRAM_OFFSET:VRAM_OFFSET+2*1024*1024]
    start = (crtc[12] << 8) | crtc[13]
    line_compare = crtc[24] | ((crtc[7] & 0x10) << 4) | (
        (crtc[9] & 0x40) << 3)
    require(start in (0, 0x8000) and line_compare >= 399,
            "Display split/paging requires independent scanout logic")
    indexed = video[start*4:start*4+PIXELS]
    require(len(indexed) == PIXELS, "Truncated selected video page")
    colors = [
        bytes(((v << 2) | (v >> 4) for v in pal[i:i+3]))
        for i in range(0, 768, 3)
    ]
    state_rgb = b"".join(colors[i] for i in indexed)
    shot, shot_format = screenshot_pixels(opt.screenshot)
    matched, rows = compare(state_rgb, shot)
    # Report world vs HUD separately. Skeleton animation and portrait
    # overlays can change while the largely static terrain/frame remains
    # unchanged. These are audits, not automatic parity verdicts.
    region_bounds = {
        "world_viewport": (32, 0, 288, 256),
        "lower_hud": (0, 260, 320, 400),
        "top_50_rows": (0, 0, 320, 50),
        "mid_world": (32, 100, 288, 200),
        "right_party_panel": (211, 320, 320, 400),
    }
    regions = {}
    for key, (x0, y0, x1, y1) in region_bounds.items():
        good = 0
        for y in range(y0, y1):
            for x in range(x0, x1):
                off = (y * 320 + x) * 3
                good += state_rgb[off:off+3] == shot[off:off+3]
        total = (x1-x0) * (y1-y0)
        regions[key] = {
            "matching_rgb_pixels": good,
            "different_rgb_pixels": total-good,
            "matching_percent": round(good*100/total, 3),
        }
    palette_colors = set(colors)
    valid_colors = sum(shot[i:i+3] in palette_colors
                       for i in range(0, len(shot), 3))
    report = {
        "program": program,
        "dosbox_version": VERSION.decode(),
        "state_sha256": sha256(opt.state.read_bytes()),
        "screenshot_sha256": sha256(opt.screenshot.read_bytes()),
        "screenshot_format": shot_format,
        "crtc_start": start,
        "crtc_line_compare": line_compare,
        "original_dac_rgb6_sha256": sha256(pal),
        "state_original_page_sha256": sha256(indexed),
        "state_vs_screenshot_matching_rgb_pixels": matched,
        "state_vs_screenshot_different_rgb_pixels": PIXELS-matched,
        "state_vs_screenshot_match_pct": round(100*matched/PIXELS, 3),
        "screenshot_pixels_in_saved_dac": valid_colors,
        "per_row_rgb_match_counts": rows,
        "region_match_diagnostics": regions,
        "screenshot_includes_window_chrome": (
            shot_format == "known 642x452 SDL2 screenshot with window chrome"),
        "frame_sync_caution": (
            "A window-chrome screenshot or an emulator render buffer can "
            "represent a different instant from the serialized VGA page. "
            "Do not accept exact DOS/native frame parity unless original "
            "screen pixels and hardware scanout coincide."),

        "synchronized_frame_evidence": (
            "strong" if matched == PIXELS else
            "partial/not exact; review timing, VGA scanout and game animation"),
        "dos_native_parity": "NOT_VERIFIED",
    }
    if opt.native_index or opt.native_palette:
        require(opt.native_index is not None and opt.native_palette is not None,
                "Supply both native .idx and RGB6 .pal")
        nidx = opt.native_index.read_bytes()
        npal = opt.native_palette.read_bytes()
        require(len(nidx) == PIXELS and len(npal) == 768,
                "Native reference dimensions are wrong")
        ncol = [bytes(((v << 2) | (v >> 4) for v in npal[i:i+3]))
                for i in range(0, 768, 3)]
        native_rgb = b"".join(ncol[i] for i in nidx)
        native_rgb_original_dac = b"".join(colors[i] for i in nidx)
        original_vs_native, _ = compare(state_rgb, native_rgb)
        native_vs_shot, _ = compare(native_rgb, shot)
        corrected_vs_shot, _ = compare(native_rgb_original_dac, shot)
        report["native"] = {
            "indexed_frame_sha256": sha256(nidx),
            "rgb6_palette_sha256": sha256(npal),
            "palette_entries_different": [
                i for i in range(256) if pal[i*3:i*3+3] != npal[i*3:i*3+3]
            ],
            "original_saved_page_vs_native_rgb_matching_pixels": original_vs_native,
            "native_vs_screenshot_matching_rgb_pixels": native_vs_shot,
            "native_indices_using_original_dac_vs_screenshot_matching_rgb_pixels": corrected_vs_shot,
            "note": "These comparisons are NOT synchronized retail parity if the original screenshot disagrees with its state."
        }
    opt.report.parent.mkdir(parents=True, exist_ok=True)
    opt.report.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    print(json.dumps({k: v for k,v in report.items()
                      if k not in ("per_row_rgb_match_counts", "native")}, indent=2))


if __name__ == "__main__":
    main()
