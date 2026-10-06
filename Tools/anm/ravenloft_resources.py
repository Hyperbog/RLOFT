"""Minimal Ravenloft RES/COLORS reader used by the optional GUI theme.

The decoder follows Resources/RES/FORMAT.md and Resources/MISC/COLORS.
Original game resources are loaded at runtime; no extracted artwork is bundled.
"""
from __future__ import annotations

import struct
from dataclasses import dataclass
from pathlib import Path

from PIL import Image


class ResourceError(RuntimeError):
    pass


@dataclass(frozen=True)
class RasterResource:
    width: int
    height: int
    pixels: bytes


def _lzss_4k(data: bytes, expected_size: int) -> bytes:
    history = bytearray([0xFE]) * 4096
    cursor = 0xFEE
    output = bytearray()
    pos = 0
    flags = 0
    flag_bits = 0

    while len(output) < expected_size:
        if flag_bits == 0:
            if pos >= len(data):
                raise ResourceError("LZSS stream ended before flag byte")
            flags = data[pos]
            pos += 1
            flag_bits = 8

        literal = flags & 1
        flags >>= 1
        flag_bits -= 1

        if literal:
            if pos >= len(data):
                raise ResourceError("LZSS stream ended during literal")
            value = data[pos]
            pos += 1
            output.append(value)
            history[cursor] = value
            cursor = (cursor + 1) & 0xFFF
        else:
            if pos + 2 > len(data):
                raise ResourceError("LZSS stream ended during back-reference")
            lo = data[pos]
            hi = data[pos + 1]
            pos += 2
            source = lo | ((hi & 0xF0) << 4)
            length = (hi & 0x0F) + 3
            for offset in range(length):
                value = history[(source + offset) & 0xFFF]
                output.append(value)
                history[cursor] = value
                cursor = (cursor + 1) & 0xFFF
                if len(output) == expected_size:
                    break

    return bytes(output)


class ResArchive:
    def __init__(self, path: Path):
        self.path = Path(path)
        self._bytes = self.path.read_bytes()
        if len(self._bytes) < 4:
            raise ResourceError(f"{self.path.name}: file too small")

        count = struct.unpack_from("<i", self._bytes, 0)[0]
        if count < 0 or 4 + count * 4 > len(self._bytes):
            raise ResourceError(f"{self.path.name}: invalid entry count {count}")
        self.count = count
        self.offsets = list(struct.unpack_from(f"<{count}i", self._bytes, 4))
        expected_first = 4 + 4 * count
        if count and self.offsets[0] != expected_first:
            raise ResourceError(
                f"{self.path.name}: first entry offset {self.offsets[0]} != {expected_first}"
            )
        if any(a > b for a, b in zip(self.offsets, self.offsets[1:])):
            raise ResourceError(f"{self.path.name}: entry offsets are not monotonic")

    def entry(self, index: int) -> bytes:
        if not 0 <= index < self.count:
            raise ResourceError(f"entry {index} outside 0..{self.count - 1}")
        start = self.offsets[index]
        end = self.offsets[index + 1] if index + 1 < self.count else len(self._bytes)
        return self._bytes[start:end]

    def raster(self, index: int) -> RasterResource:
        entry = self.entry(index)
        if len(entry) < 13 or entry[:3] != b"EH\x01":
            raise ResourceError(f"RES entry {index} is not a type-1 raster")
        header_size = struct.unpack_from("<H", entry, 3)[0]
        if header_size < 8 or 5 + header_size > len(entry):
            raise ResourceError(f"RES entry {index} has invalid type-1 header")

        count, element_height, width_units, _field5, compression, _field7 = struct.unpack_from(
            "<HhBBBB", entry, 5
        )
        if element_height <= 0 or width_units == 0:
            raise ResourceError(f"RES entry {index} has invalid raster dimensions")

        width = width_units * 8
        height = count * element_height
        expected = width * height
        payload = entry[5 + header_size :]

        if compression == 0:
            if len(payload) != expected:
                raise ResourceError(
                    f"RES entry {index}: raw payload {len(payload)} != {expected}"
                )
            pixels = payload
        elif compression == 3:
            pixels = _lzss_4k(payload, expected)
        else:
            raise ResourceError(
                f"RES entry {index}: unsupported type-1 compression {compression}"
            )
        return RasterResource(width, height, pixels)


class ColorsFile:
    RECORDS = 15
    COLORS_PER_RECORD = 256
    BYTES_PER_RECORD = COLORS_PER_RECORD * 3

    def __init__(self, path: Path):
        self.path = Path(path)
        self._bytes = self.path.read_bytes()
        expected = 1 + self.RECORDS * self.BYTES_PER_RECORD
        if len(self._bytes) != expected:
            raise ResourceError(
                f"{self.path.name}: expected {expected} bytes, got {len(self._bytes)}"
            )

    def palette(self, index: int = 0) -> bytes:
        if not 0 <= index < self.RECORDS:
            raise ResourceError(f"palette {index} outside 0..{self.RECORDS - 1}")
        start = 1 + index * self.BYTES_PER_RECORD
        raw = self._bytes[start : start + self.BYTES_PER_RECORD]
        return bytes((component * 255 + 31) // 63 for component in raw)


@dataclass
class StrahdThemeAssets:
    """Original game imagery selected for the optional extractor skin."""

    frame: Image.Image
    hud_frame: Image.Image
    menu_strip: Image.Image
    ornament: Image.Image

    @classmethod
    def load(cls, game_dir: Path, palette_index: int = 0) -> "StrahdThemeAssets":
        game_dir = Path(game_dir)
        res0_path = game_dir / "RES0"
        colors_path = game_dir / "COLORS"
        if not res0_path.is_file() or not colors_path.is_file():
            raise ResourceError("Strahd theme requires RES0 and COLORS in the selected folder")

        archive = ResArchive(res0_path)
        palette = ColorsFile(colors_path).palette(palette_index)

        def image(resource_id: int) -> Image.Image:
            raster = archive.raster(resource_id)
            result = Image.frombytes("P", (raster.width, raster.height), raster.pixels)
            result.putpalette(palette)
            return result.convert("RGB")

        return cls(
            frame=image(16),
            hud_frame=image(17),
            menu_strip=image(71),
            ornament=image(91),
        )


def find_game_dir(start: Path) -> Path | None:
    """Look in start and its parents for the minimum theme resources."""
    current = Path(start)
    if current.is_file():
        current = current.parent
    for candidate in (current, *current.parents):
        if (candidate / "RES0").is_file() and (candidate / "COLORS").is_file():
            return candidate
    return None
