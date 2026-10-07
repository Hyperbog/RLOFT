#!/usr/bin/env python3
"""Flexible Ravenloft install/CD source discovery for the extractor.

The extractor historically required the audited 33 files to sit directly in
one selected folder. Real installations are less uniform: GOG keeps the
original CD in game.gog, some installs expose the CD files loosely, Steam-like
layouts can nest the game below an install directory, and preservation copies
may use ordinary ISO/BIN/IMG images.

This module resolves all of those layouts into the flat filename view expected
by the existing certified decoders. Loose files take precedence over an image,
which also makes modded installs behave naturally.
"""
from __future__ import annotations

import hashlib
import json
import os
import shutil
import struct
import tempfile
from dataclasses import dataclass
from pathlib import Path
from typing import Iterable

import media_inventory


LOGICAL_SECTOR = 2048
MAX_SCAN_DEPTH = 6
MAX_SCAN_ENTRIES = 75000
IMAGE_SUFFIXES = {".gog", ".iso", ".bin", ".img"}

# Extra runtime databases used by semantic grouping and likely future tooling.
# They are intentionally not part of the 33-file canonical media checksum set.
AUXILIARY_NAMES = (
    "NPCS.DAT",
    "OBJECTS.DAT",
    "INFO.DAT",
    "LEVELS.DAT",
    "SAVE.DAT",
    "SOUND.CFG",
)
CANONICAL_NAMES = tuple(spec.name for spec in media_inventory.SOURCES)
MATERIALIZE_NAMES = CANONICAL_NAMES + AUXILIARY_NAMES
MATERIALIZE_NAME_SET = {name.upper() for name in MATERIALIZE_NAMES}
ANCHOR_NAMES = ("RES0", "INIT", "COLORS")


class SourceError(RuntimeError):
    pass


def _upper(value: str) -> str:
    return value.upper()


def _iso_name(raw: bytes | str) -> str:
    if isinstance(raw, bytes):
        name = raw.decode("ascii", errors="ignore").upper()
    else:
        name = raw.upper()
    if ";" in name:
        name = name.split(";", 1)[0]
    return name.rstrip(".")


@dataclass(frozen=True)
class IsoEntry:
    extent: int
    size: int
    directory: bool


class Iso9660Image:
    """Minimal ISO-9660 reader for Ravenloft CD images.

    Supports ordinary 2048-byte sectors and the raw 2352-byte layouts observed
    in DOS-era/GOG disc images (user data at byte 16 or 24).
    """

    def __init__(self, path: Path):
        self.path = Path(path).resolve()
        if not self.path.is_file():
            raise SourceError(f"CD image does not exist: {self.path}")
        self.physical_sector, self.user_offset = self._detect_layout()
        self.entries: dict[str, IsoEntry] = {}
        self._visited: set[int] = set()
        self._index_volume()

    def _read_at(self, offset: int, size: int) -> bytes:
        with self.path.open("rb") as fp:
            fp.seek(offset)
            data = fp.read(size)
        if len(data) != size:
            raise SourceError(f"Short read in CD image {self.path.name}")
        return data

    def _detect_layout(self) -> tuple[int, int]:
        for physical, user_offset in ((2048, 0), (2352, 16), (2352, 24)):
            offset = 16 * physical + user_offset
            try:
                sig = self._read_at(offset, 7)
            except SourceError:
                continue
            if sig[:1] == b"\x01" and sig[1:6] == b"CD001" and sig[6:7] == b"\x01":
                return physical, user_offset
        raise SourceError(
            f"{self.path.name} is not a supported ISO-9660 data image"
        )

    def _sector(self, lba: int) -> bytes:
        offset = lba * self.physical_sector + self.user_offset
        return self._read_at(offset, LOGICAL_SECTOR)

    @staticmethod
    def _le32(data: bytes, offset: int) -> int:
        return struct.unpack_from("<I", data, offset)[0]

    def _read_extent(self, extent: int, size: int) -> bytes:
        out = bytearray()
        lba = extent
        while len(out) < size:
            out.extend(self._sector(lba))
            lba += 1
        return bytes(out[:size])

    def _index_volume(self) -> None:
        pvd = self._sector(16)
        if pvd[0] != 1 or pvd[1:6] != b"CD001":
            raise SourceError("ISO primary volume descriptor is missing")
        root = pvd[156:]
        if not root or root[0] < 34:
            raise SourceError("ISO root directory record is invalid")
        self._index_directory(
            self._le32(root, 2),
            self._le32(root, 10),
            depth=0,
        )

    def _index_directory(self, extent: int, size: int, depth: int) -> None:
        if depth > 12 or extent in self._visited:
            return
        self._visited.add(extent)
        data = self._read_extent(extent, size)
        pos = 0
        while pos < len(data):
            length = data[pos]
            if length == 0:
                pos = ((pos // LOGICAL_SECTOR) + 1) * LOGICAL_SECTOR
                continue
            if length < 34 or pos + length > len(data):
                break
            rec = data[pos:pos + length]
            name_len = rec[32]
            if 33 + name_len > len(rec):
                break
            raw_name = rec[33:33 + name_len]
            child_extent = self._le32(rec, 2)
            child_size = self._le32(rec, 10)
            directory = bool(rec[25] & 0x02)
            dot_entry = name_len == 1 and raw_name in (b"\x00", b"\x01")
            if not dot_entry:
                name = _iso_name(raw_name)
                if name and name not in self.entries:
                    self.entries[name] = IsoEntry(
                        extent=child_extent,
                        size=child_size,
                        directory=directory,
                    )
                if directory:
                    self._index_directory(child_extent, child_size, depth + 1)
            pos += length

    def contains(self, filename: str) -> bool:
        entry = self.entries.get(_upper(filename))
        return entry is not None and not entry.directory

    def read(self, filename: str) -> bytes:
        key = _upper(filename)
        entry = self.entries.get(key)
        if entry is None or entry.directory:
            raise SourceError(f"File not found in CD image: {filename}")
        return self._read_extent(entry.extent, entry.size)


@dataclass
class ResolvedSource:
    selected_path: Path
    loose_files: dict[str, Path]
    image: Iso9660Image | None
    image_path: Path | None
    scanned_entries: int

    @property
    def kind(self) -> str:
        loose_count = len(self.loose_files)
        image_count = sum(
            1 for name in MATERIALIZE_NAMES
            if self.image is not None and self.image.contains(name)
        )
        if self.image is not None and loose_count and image_count:
            return "mixed loose files + CD image"
        if self.image is not None:
            return "CD image"
        return "loose install/media files"

    def has(self, name: str) -> bool:
        key = _upper(name)
        return key in self.loose_files or (
            self.image is not None and self.image.contains(key)
        )

    def read(self, name: str) -> bytes:
        key = _upper(name)
        loose = self.loose_files.get(key)
        if loose is not None:
            return loose.read_bytes()
        if self.image is not None and self.image.contains(key):
            return self.image.read(key)
        raise SourceError(f"Ravenloft source file not found: {name}")

    def available_names(self) -> list[str]:
        return [name for name in MATERIALIZE_NAMES if self.has(name)]


def _iter_tree(root: Path, max_depth: int = MAX_SCAN_DEPTH):
    """Yield files beneath root with a bounded, permission-tolerant walk."""
    root = root.resolve()
    stack: list[tuple[Path, int]] = [(root, 0)]
    seen = 0
    while stack and seen < MAX_SCAN_ENTRIES:
        directory, depth = stack.pop()
        try:
            entries = list(directory.iterdir())
        except (OSError, PermissionError):
            continue
        for entry in entries:
            seen += 1
            if seen > MAX_SCAN_ENTRIES:
                break
            try:
                if entry.is_file():
                    yield entry, depth, seen
                elif entry.is_dir() and depth < max_depth:
                    name = entry.name.lower()
                    if name not in {
                        ".git", ".svn", "__pycache__", "rloft_dump",
                        "node_modules", "$recycle.bin",
                    }:
                        stack.append((entry, depth + 1))
            except (OSError, PermissionError):
                continue


def _looks_like_image(path: Path) -> bool:
    if path.suffix.lower() in IMAGE_SUFFIXES or path.name.lower() == "game.gog":
        return True
    return False


def resolve_source(path: Path) -> ResolvedSource:
    """Scan an install folder, loose media folder, or CD image.

    Loose copies of known files win over CD-image copies. This lets users point
    the extractor at a modded installation while still taking missing media
    from the original disc image.
    """
    selected = Path(path).expanduser()
    if not selected.exists():
        raise SourceError(f"Game/media path does not exist: {selected}")
    selected = selected.resolve()

    loose: dict[str, Path] = {}
    image_candidates: list[Path] = []
    scanned = 0

    if selected.is_file():
        image_candidates.append(selected)
        scan_root = selected.parent
        # Also collect loose override files from the image's containing folder.
        for entry, _depth, count in _iter_tree(scan_root, max_depth=2):
            scanned = max(scanned, count)
            key = _upper(entry.name)
            if key in MATERIALIZE_NAME_SET and key not in loose:
                loose[key] = entry
            if _looks_like_image(entry) and entry not in image_candidates:
                image_candidates.append(entry)
    elif selected.is_dir():
        for entry, _depth, count in _iter_tree(selected):
            scanned = max(scanned, count)
            key = _upper(entry.name)
            if key in MATERIALIZE_NAME_SET and key not in loose:
                loose[key] = entry
            if _looks_like_image(entry):
                image_candidates.append(entry)
    else:
        raise SourceError(f"Unsupported game/media path: {selected}")

    # Prefer game.gog, then ordinary ISO, then raw BIN/IMG. Only accept an
    # image as Ravenloft media if it contains core archive anchors.
    image_candidates.sort(
        key=lambda p: (
            0 if p.name.lower() == "game.gog" else 1,
            0 if p.suffix.lower() == ".iso" else 1,
            len(p.parts),
            str(p).lower(),
        )
    )
    chosen_image: Iso9660Image | None = None
    chosen_path: Path | None = None
    for candidate in image_candidates:
        try:
            image = Iso9660Image(candidate)
        except (OSError, SourceError):
            continue
        if sum(image.contains(name) for name in ANCHOR_NAMES) >= 2:
            chosen_image = image
            chosen_path = candidate
            break

    result = ResolvedSource(
        selected_path=selected,
        loose_files=loose,
        image=chosen_image,
        image_path=chosen_path,
        scanned_entries=scanned,
    )
    if sum(result.has(name) for name in ANCHOR_NAMES) < 2:
        raise SourceError(
            "No Ravenloft media found. Select the game install folder, a folder "
            "containing loose RES/INIT/COLORS files, game.gog, or an ISO/BIN/IMG "
            "copy of the original CD."
        )
    return result


def validate_source(path: Path, verify_hashes: bool = True) -> dict[str, object]:
    source = resolve_source(path)
    present: list[str] = []
    missing: list[str] = []
    mismatched: list[dict[str, object]] = []

    for spec in media_inventory.SOURCES:
        if not source.has(spec.name):
            missing.append(spec.name)
            continue
        present.append(spec.name)
        data = source.read(spec.name)
        digest = hashlib.sha256(data).hexdigest() if verify_hashes else None
        if len(data) != spec.size or (verify_hashes and digest != spec.sha256):
            mismatched.append({
                "name": spec.name,
                "expected_size": spec.size,
                "actual_size": len(data),
                "expected_sha256": spec.sha256,
                "actual_sha256": digest,
            })

    auxiliary = [name for name in AUXILIARY_NAMES if source.has(name)]
    return {
        "selected_path": str(source.selected_path),
        "kind": source.kind,
        "image_path": str(source.image_path) if source.image_path else None,
        "expected": len(media_inventory.SOURCES),
        "present": present,
        "missing": missing,
        "mismatched": mismatched,
        "auxiliary": auxiliary,
        "recognized": len(present) > 0,
        # A non-GOG/non-canonical release is still usable. Exact hashes are a
        # provenance signal, not a gate that forbids extraction.
        "usable_complete": not missing,
        "certified_complete": not missing and not mismatched,
        "scanned_entries": source.scanned_entries,
    }


def _source_fingerprint(source: ResolvedSource) -> str:
    h = hashlib.sha256()
    h.update(str(source.selected_path).encode("utf-8", errors="surrogatepass"))
    for key, path in sorted(source.loose_files.items()):
        try:
            stat = path.stat()
            token = f"{key}|{path}|{stat.st_size}|{stat.st_mtime_ns}\n"
        except OSError:
            token = f"{key}|{path}|missing\n"
        h.update(token.encode("utf-8", errors="surrogatepass"))
    if source.image_path is not None:
        try:
            stat = source.image_path.stat()
            token = (
                f"IMAGE|{source.image_path}|{stat.st_size}|{stat.st_mtime_ns}\n"
            )
        except OSError:
            token = f"IMAGE|{source.image_path}|missing\n"
        h.update(token.encode("utf-8", errors="surrogatepass"))
    return h.hexdigest()[:20]


def _can_use_direct_directory(source: ResolvedSource) -> Path | None:
    """Return a flat loose directory when no normalization is necessary."""
    if source.image is not None or not source.loose_files:
        return None
    parents = {path.parent.resolve() for path in source.loose_files.values()}
    if len(parents) != 1:
        return None
    parent = next(iter(parents))
    # Existing decoders use canonical filenames. On Windows case is
    # insensitive, but exact names also keep source-mode tests portable.
    for canonical in source.available_names():
        path = source.loose_files.get(_upper(canonical))
        if path is None or path.name != canonical:
            return None
    return parent


def prepare_source_dir(path: Path) -> Path:
    """Return a flat filesystem directory suitable for existing dumpers.

    Direct loose-media folders are used in place. Nested/mixed/image layouts
    are normalized into a deterministic temp cache. Loose files are hard-linked
    when possible and copied otherwise; CD-image files are extracted once.
    """
    source = resolve_source(path)
    direct = _can_use_direct_directory(source)
    if direct is not None:
        return direct

    fingerprint = _source_fingerprint(source)
    cache_root = (
        Path(tempfile.gettempdir())
        / "RavenloftAssetExtractor"
        / f"source_{fingerprint}"
    )
    marker = cache_root / ".rloft_source.json"
    expected = source.available_names()

    if marker.is_file() and all((cache_root / name).is_file() for name in expected):
        return cache_root

    if cache_root.exists():
        shutil.rmtree(cache_root, ignore_errors=True)
    cache_root.mkdir(parents=True, exist_ok=True)

    for name in expected:
        target = cache_root / name
        loose = source.loose_files.get(_upper(name))
        if loose is not None:
            try:
                os.link(loose, target)
            except OSError:
                shutil.copy2(loose, target)
        else:
            target.write_bytes(source.read(name))

    marker.write_text(
        json.dumps({
            "selected_path": str(source.selected_path),
            "kind": source.kind,
            "image_path": str(source.image_path) if source.image_path else None,
            "files": expected,
            "loose_overrides": {
                name: str(path)
                for name, path in sorted(source.loose_files.items())
                if name in {_upper(item) for item in expected}
            },
        }, indent=2) + "\n",
        encoding="utf-8",
    )
    return cache_root
