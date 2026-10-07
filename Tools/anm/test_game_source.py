#!/usr/bin/env python3
"""Regression tests for flexible Ravenloft install/CD source discovery."""
from __future__ import annotations

import os
import struct
import tempfile
import unittest
from pathlib import Path

import game_source


SECTOR = 2048


def _put_le32_both(buffer: bytearray, offset: int, value: int) -> None:
    buffer[offset:offset + 4] = struct.pack("<I", value)
    buffer[offset + 4:offset + 8] = struct.pack(">I", value)


def _dir_record(extent: int, size: int, flags: int, name: bytes) -> bytes:
    length = 33 + len(name) + (1 if len(name) % 2 == 0 else 0)
    record = bytearray(length)
    record[0] = length
    _put_le32_both(record, 2, extent)
    _put_le32_both(record, 10, size)
    record[25] = flags
    record[28] = 1
    record[31] = 1
    record[32] = len(name)
    record[33:33 + len(name)] = name
    return bytes(record)


def _write_iso(path: Path, raw2352: bool = False) -> None:
    sectors = [bytearray(SECTOR) for _ in range(48)]

    pvd = sectors[16]
    pvd[0] = 1
    pvd[1:6] = b"CD001"
    pvd[6] = 1
    root_rec = _dir_record(20, SECTOR, 2, b"\x00")
    pvd[156:156 + len(root_rec)] = root_rec

    term = sectors[17]
    term[0] = 255
    term[1:6] = b"CD001"
    term[6] = 1

    files = [
        (21, b"RES0;1", b"iso-res0"),
        (22, b"INIT;1", b"iso-init"),
        (23, b"COLORS;1", b"iso-colors"),
        (24, b"T;1", b"iso-text"),
        (25, b"NPCS.DAT;1", b"iso-npcs"),
        (26, b"OBJECTS.DAT;1", b"iso-objects"),
    ]

    root = sectors[20]
    pos = 0
    for rec in (
        _dir_record(20, SECTOR, 2, b"\x00"),
        _dir_record(20, SECTOR, 2, b"\x01"),
    ):
        root[pos:pos + len(rec)] = rec
        pos += len(rec)

    for extent, name, payload in files:
        rec = _dir_record(extent, len(payload), 0, name)
        root[pos:pos + len(rec)] = rec
        pos += len(rec)
        sectors[extent][:len(payload)] = payload

    with path.open("wb") as out:
        for sector in sectors:
            if not raw2352:
                out.write(sector)
            else:
                raw = bytearray(2352)
                raw[15] = 1
                raw[16:16 + SECTOR] = sector
                out.write(raw)


class GameSourceTests(unittest.TestCase):
    def test_game_gog_folder_is_scanned_and_materialized(self) -> None:
        with tempfile.TemporaryDirectory() as temp:
            root = Path(temp)
            _write_iso(root / "game.gog")

            resolved = game_source.resolve_source(root)
            self.assertEqual(resolved.kind, "CD image")
            self.assertTrue(resolved.has("RES0"))
            self.assertTrue(resolved.has("NPCS.DAT"))
            self.assertEqual(resolved.read("INIT"), b"iso-init")

            prepared = game_source.prepare_source_dir(root)
            self.assertEqual((prepared / "RES0").read_bytes(), b"iso-res0")
            self.assertEqual((prepared / "OBJECTS.DAT").read_bytes(), b"iso-objects")

    def test_raw_2352_bin_image_is_supported(self) -> None:
        with tempfile.TemporaryDirectory() as temp:
            root = Path(temp)
            image = root / "ravenloft.bin"
            _write_iso(image, raw2352=True)

            resolved = game_source.resolve_source(image)
            self.assertTrue(resolved.has("COLORS"))
            self.assertEqual(resolved.read("T"), b"iso-text")

    def test_nested_loose_install_directory_is_accepted_directly(self) -> None:
        with tempfile.TemporaryDirectory() as temp:
            root = Path(temp)
            media = root / "Some Store" / "Ravenloft" / "RLOFT"
            media.mkdir(parents=True)
            for name, payload in {
                "RES0": b"loose-res0",
                "INIT": b"loose-init",
                "COLORS": b"loose-colors",
                "T": b"loose-text",
                "NPCS.DAT": b"loose-npcs",
                "OBJECTS.DAT": b"loose-objects",
            }.items():
                (media / name).write_bytes(payload)

            resolved = game_source.resolve_source(root)
            self.assertEqual(resolved.kind, "loose install/media files")
            prepared = game_source.prepare_source_dir(root)
            self.assertEqual(prepared.resolve(), media.resolve())

    def test_loose_files_override_cd_image_for_modded_installs(self) -> None:
        with tempfile.TemporaryDirectory() as temp:
            root = Path(temp)
            _write_iso(root / "game.gog")
            override = root / "RLOFT"
            override.mkdir()
            (override / "RES0").write_bytes(b"modded-res0")
            (override / "INIT").write_bytes(b"modded-init")

            resolved = game_source.resolve_source(root)
            self.assertEqual(resolved.kind, "mixed loose files + CD image")
            self.assertEqual(resolved.read("RES0"), b"modded-res0")
            self.assertEqual(resolved.read("COLORS"), b"iso-colors")

            prepared = game_source.prepare_source_dir(root)
            self.assertEqual((prepared / "RES0").read_bytes(), b"modded-res0")
            self.assertEqual((prepared / "COLORS").read_bytes(), b"iso-colors")

    def test_validation_accepts_noncanonical_partial_source(self) -> None:
        with tempfile.TemporaryDirectory() as temp:
            root = Path(temp)
            media = root / "game"
            media.mkdir()
            (media / "RES0").write_bytes(b"not-the-audited-res0")
            (media / "INIT").write_bytes(b"not-the-audited-init")
            (media / "COLORS").write_bytes(b"not-the-audited-colors")

            result = game_source.validate_source(root, verify_hashes=True)
            self.assertTrue(result["recognized"])
            self.assertFalse(result["certified_complete"])
            self.assertGreater(len(result["missing"]), 0)
            self.assertGreater(len(result["mismatched"]), 0)


if __name__ == "__main__":
    unittest.main()
