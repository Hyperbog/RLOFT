"""Certified Ravenloft RES parsing and decoding primitives."""
from __future__ import annotations

import struct
from dataclasses import dataclass
from pathlib import Path
from typing import Iterable


class AssetError(RuntimeError):
    pass


@dataclass(frozen=True)
class EhResource:
    archive: int
    index: int
    resource_type: int
    header: bytes
    payload: bytes
    raw: bytes


@dataclass(frozen=True)
class Type1Raster:
    count: int
    width: int
    element_height: int
    pixels: bytes


@dataclass(frozen=True)
class Type7Raster:
    width: int
    height: int
    pixels: bytes
    mask: bytes


class ResArchive:
    def __init__(self, path: Path, archive_number: int):
        self.path = Path(path)
        self.archive_number = archive_number
        self.data = self.path.read_bytes()
        if len(self.data) < 4:
            raise AssetError(f"{path.name}: too small")
        self.count = struct.unpack_from("<i", self.data, 0)[0]
        if self.count < 0 or 4 + 4 * self.count > len(self.data):
            raise AssetError(f"{path.name}: invalid entry count")
        self.offsets = list(struct.unpack_from(f"<{self.count}i", self.data, 4))
        if self.count and self.offsets[0] != 4 + 4 * self.count:
            raise AssetError(f"{path.name}: invalid first offset")
        if any(a > b for a, b in zip(self.offsets, self.offsets[1:])):
            raise AssetError(f"{path.name}: non-monotonic offsets")

    def entry(self, index: int) -> bytes:
        start = self.offsets[index]
        end = self.offsets[index + 1] if index + 1 < self.count else len(self.data)
        return self.data[start:end]

    def resources(self) -> Iterable[EhResource]:
        for index in range(self.count):
            raw = self.entry(index)
            if len(raw) < 5 or raw[:2] != b"EH":
                continue
            resource_type = raw[2]
            header_size = struct.unpack_from("<H", raw, 3)[0]
            end = 5 + header_size
            if end > len(raw):
                raise AssetError(f"RES{self.archive_number}:{index}: truncated EH header")
            yield EhResource(self.archive_number, index, resource_type, raw[5:end], raw[end:], raw)


def discover_archives(game_dir: Path) -> list[ResArchive]:
    archives = []
    for number in range(13):
        path = Path(game_dir) / f"RES{number}"
        if path.is_file():
            archives.append(ResArchive(path, number))
    if not archives:
        raise AssetError("No RES0..RES12 archives found in the selected folder")
    return archives


def lzss_4k(data: bytes, expected_size: int) -> bytes:
    history = bytearray([0xFE]) * 4096
    cursor = 0xFEE
    out = bytearray()
    pos = 0
    flags = flag_bits = 0
    while len(out) < expected_size:
        if flag_bits == 0:
            if pos >= len(data):
                raise AssetError("LZSS stream ended before flag byte")
            flags = data[pos]
            pos += 1
            flag_bits = 8
        literal = flags & 1
        flags >>= 1
        flag_bits -= 1
        if literal:
            if pos >= len(data):
                raise AssetError("LZSS stream ended during literal")
            value = data[pos]
            pos += 1
            out.append(value)
            history[cursor] = value
            cursor = (cursor + 1) & 0xFFF
        else:
            if pos + 2 > len(data):
                raise AssetError("LZSS stream ended during back-reference")
            lo, hi = data[pos], data[pos + 1]
            pos += 2
            source = lo | ((hi & 0xF0) << 4)
            length = (hi & 0x0F) + 3
            for i in range(length):
                value = history[(source + i) & 0xFFF]
                out.append(value)
                history[cursor] = value
                cursor = (cursor + 1) & 0xFFF
                if len(out) == expected_size:
                    break
    return bytes(out)


def palette_from_colors(path: Path, index: int) -> bytes:
    data = Path(path).read_bytes()
    if len(data) != 1 + 15 * 768 or not 0 <= index < 15:
        raise AssetError("COLORS must be the 15-bank Ravenloft palette file")
    start = 1 + index * 768
    return bytes((v * 255 + 31) // 63 for v in data[start:start + 768])


def decode_type1(resource: EhResource, font: bool = False) -> tuple[Type1Raster, bytes | None]:
    header = resource.header
    if font:
        if len(header) < 136:
            raise AssetError("type-2 font header is shorter than 136 bytes")
        widths, desc = header[:128], header[128:136]
    else:
        if len(header) < 8:
            raise AssetError("type-1 header is shorter than 8 bytes")
        widths, desc = None, header[:8]
    count, height, width_units, _f5, compression, _f7 = struct.unpack("<HhBBBB", desc)
    # The certified RES12 alternate bank contains five intentional empty
    # type-1 records: count=0, valid geometry fields, and no payload. The
    # original dumper incorrectly treated these as malformed rasters.
    if count == 0 and height > 0 and width_units > 0 and not resource.payload:
        return Type1Raster(0, width_units * 8, height, b""), widths
    if count <= 0 or height <= 0 or width_units <= 0:
        raise AssetError(
            f"invalid raster dimensions (count={count}, height={height}, width_units={width_units})"
        )
    width = width_units * 8
    expected = count * height * width
    if compression == 0:
        pixels = resource.payload
        if len(pixels) != expected:
            raise AssetError("raw raster size mismatch")
    elif compression == 3:
        pixels = lzss_4k(resource.payload, expected)
    else:
        raise AssetError(f"unsupported supplied raster compression {compression}")
    return Type1Raster(count, width, height, pixels), widths


def decode_shared_lzss(resource: EhResource) -> bytes:
    if len(resource.payload) < 2:
        raise AssetError("type-6/7 payload missing decoded-size word")
    size = struct.unpack_from("<H", resource.payload, 0)[0]
    return lzss_4k(resource.payload[2:], size)


def length_prefixed_blob(resource: EhResource) -> tuple[int, bytes]:
    if len(resource.payload) < 4:
        raise AssetError("length-prefixed resource is truncated")
    return struct.unpack_from("<I", resource.payload, 0)[0], resource.payload[4:]


def type7_metadata(body: bytes) -> dict[str, object]:
    if len(body) < 10:
        raise AssetError("short type-7 body")
    source_width, x_min, x_max, y_min, y_max = struct.unpack_from("<Hhhhh", body, 0)
    table_count = x_max - x_min + 1
    if table_count < 0 or 10 + table_count * 2 > len(body):
        raise AssetError("invalid type-7 column table")
    pointers = list(struct.unpack_from(f"<{table_count}H", body, 10)) if table_count else []
    return {"source_width": source_width, "x_min": x_min, "x_max": x_max,
            "y_min": y_min, "y_max": y_max, "column_offsets": pointers}


def decode_type7_raster(body: bytes) -> Type7Raster:
    """Reconstruct a type-7 scene shape at its native 1:1 source scale.

    CODE.2's type-7 descriptors select entry/stop offsets in SINTAB's
    129-entry scale rasterizer table. At the 1:1 SINTAB scale block every
    scale step is one source texel, so selector/2 is the source Y boundary.
    The descriptor's signed adjustment is the source-base displacement used
    by the generated rasterizer before reading [ESI + y].

    This is fully valid for the active RES3 corpus. Some retained RES11 legacy
    records reference source storage that is not embedded in the same body;
    those are reported as non-renderable rather than guessed.
    """
    metadata = type7_metadata(body)
    width = int(metadata["source_width"])
    height = 128
    x_min = int(metadata["x_min"])
    pointers = metadata["column_offsets"]

    if width not in (64, 96, 128):
        raise AssetError(f"unsupported type-7 source width {width}")

    pixels = bytearray(width * height)
    mask = bytearray(width * height)

    for column_index, pointer in enumerate(pointers):
        x = x_min + column_index
        if not 0 <= x < width:
            raise AssetError(f"type-7 column x={x} outside source width {width}")
        pos = int(pointer)
        descriptor_guard = 0

        while True:
            if pos + 2 > len(body):
                raise AssetError("type-7 column list terminator exceeds decoded body")
            stop_selector = struct.unpack_from("<h", body, pos)[0]
            pos += 2
            if stop_selector == 0:
                break

            if pos + 4 > len(body):
                raise AssetError("type-7 descriptor exceeds decoded body")
            signed_adjustment, entry_selector = struct.unpack_from("<hh", body, pos)
            pos += 4

            if (
                entry_selector < 0
                or stop_selector < 0
                or entry_selector > stop_selector
                or stop_selector > 0x100
                or (entry_selector & 1)
                or (stop_selector & 1)
            ):
                raise AssetError(
                    "invalid type-7 raster selectors "
                    f"entry=0x{entry_selector & 0xFFFF:04X} "
                    f"stop=0x{stop_selector & 0xFFFF:04X}"
                )

            y0 = entry_selector // 2
            y1 = stop_selector // 2
            source_start = signed_adjustment + y0
            source_end = signed_adjustment + y1
            if source_start < 0 or source_end > len(body):
                raise AssetError(
                    "type-7 descriptor references source pixels outside this "
                    "decoded body (legacy/external source layout)"
                )

            for y in range(y0, y1):
                target = y * width + x
                pixels[target] = body[signed_adjustment + y]
                mask[target] = 0xFF

            descriptor_guard += 1
            if descriptor_guard > 1024:
                raise AssetError("type-7 column list exceeds descriptor sanity limit")

    return Type7Raster(width, height, bytes(pixels), bytes(mask))
