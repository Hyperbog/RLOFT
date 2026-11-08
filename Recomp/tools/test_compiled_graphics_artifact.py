#!/usr/bin/env python3
"""Media-free real CODE.2 graphics executable/palettized BMP integration test."""
import pathlib
import struct
import subprocess
import sys
import tempfile


def main() -> int:
    if len(sys.argv) != 2:
        raise SystemExit("usage: test_compiled_graphics_artifact.py <compiled-CODE2-graphics-exe>")
    exe = pathlib.Path(sys.argv[1]).resolve()
    with tempfile.TemporaryDirectory(prefix="rloft-vga-") as root:
        base = pathlib.Path(root)
        colors = bytearray(1 + 15 * 768)
        colors[0] = 15
        for bank in range(15):
            for color in range(256):
                j = 1 + bank * 768 + color * 3
                colors[j] = (bank + color) % 64
                colors[j + 1] = (63 - color % 64)
                colors[j + 2] = (bank + color // 2) % 64
        colors_path = base / "COLORS.synthetic"
        colors_path.write_bytes(colors)
        stem = base / "code2-owned-bank"
        p = subprocess.run(
            [str(exe), str(stem), str(colors_path), "3", "16"],
            text=True, capture_output=True, timeout=20,
        )
        if p.returncode != 0:
            raise AssertionError(f"Compiled CODE.2 graphics path failed: {p.stderr}\n{p.stdout}")
        if "COMPILED_CODE2_29930" not in p.stdout:
            raise AssertionError("Actual compiled CODE.2 callback path was not acknowledged")
        indices = stem.with_suffix(".idx").read_bytes()
        bmp = stem.with_suffix(".bmp").read_bytes()
        if len(indices) != 320 * 400 or len(bmp) != 54 + 256 * 4 + 320 * 400:
            raise AssertionError("Incorrect captured indexed frame/BMP size")
        if bmp[0:2] != b"BM" or struct.unpack_from("<I", bmp, 10)[0] != 1078:
            raise AssertionError("Incorrect Windows palettized bitmap header")
        if struct.unpack_from("<ii", bmp, 18) != (320, 400):
            raise AssertionError("Incorrect indexed frame dimensions")
        if struct.unpack_from("<H", bmp, 28)[0] != 8:
            raise AssertionError("Bitmap does not retain 8-bit palette indices")
        if not any(indices):
            raise AssertionError("Compiled CODE.2 graphics did not produce colored pixels")
        # Bank 3, palette color 30: R=33 G=33 B=18, peak=33.
        # Actual CODE.2 function_32150 rescales each channel with strength 16.
        r, g, bl = (33, 33, 18)
        peak = 33
        target = peak + (16 * (63 - peak) + 31) // 63
        half = peak // 2
        rgb6 = tuple((ch * target + half) // peak for ch in (r, g, bl))
        expanded = tuple(((x & 63) << 2) | ((x & 63) >> 4) for x in rgb6)
        pixel = bmp[54 + 30 * 4:54 + 30 * 4 + 4]
        if pixel != bytes((expanded[2], expanded[1], expanded[0], 0)):
            raise AssertionError(f"Original CODE.2 palette transform mismatch: {pixel!r}")
        bad = base / "COLORS.invalid"
        bad.write_bytes(b"invalid-not-original")
        reject = subprocess.run(
            [str(exe), str(stem), str(bad), "1", "0"],
            text=True, capture_output=True, timeout=20,
        )
        if reject.returncode == 0:
            raise AssertionError("Malformed proprietary palette format silently accepted")
    print("CODE2_COLORS_PALETTE_BANK_INPUT_PASS")
    print("CODE2_FRAME_320X400_INDEXED_BMP_PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
