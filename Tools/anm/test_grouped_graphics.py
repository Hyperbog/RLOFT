#!/usr/bin/env python3
"""Regression tests for semantic multipart RES3 grouping."""
from __future__ import annotations

import struct
import tempfile
import unittest
from pathlib import Path

import grouped_graphics


def _descriptor(base: int, stride: int, variants: int) -> bytes:
    return struct.pack("<HHBBB", base, 0x8000, stride, variants, 0)


def _write_init(path: Path) -> None:
    words = [0] * 18
    words[3:7] = [0, 10, 20, 30]
    data = bytearray(struct.pack("<18H", *words))
    class1 = [_descriptor(0, 2, 2)] + [_descriptor(4, 1, 1)] * 174
    data += struct.pack("<H", len(class1))
    data += b"".join(class1)
    class3 = [_descriptor(0, 2, 2)] + [_descriptor(4, 1, 1)] * 11
    data += struct.pack("<H", len(class3))
    data += b"".join(class3)
    path.write_bytes(data)


def _npc_record(name: str, base: int, stride: int) -> bytes:
    rec = bytearray(0x45)
    raw = name.encode("ascii")[:19]
    rec[:len(raw)] = raw
    struct.pack_into("<H", rec, 0x14, base)
    rec[0x18] = stride
    rec[0x19] = 4
    rec[0x1A] = 3
    return bytes(rec)


def _write_npcs(path: Path) -> None:
    records = [
        _npc_record("TEST ONE", 0, 2),
        _npc_record("TEST TWO", 4, 2),
    ]
    path.write_bytes(struct.pack("<I", len(records)) + b"".join(records))


def _object_record(name: str, base: int, stride: int) -> bytes:
    rec = bytearray(0x41)
    raw = name.encode("ascii")[:39]
    rec[:len(raw)] = raw
    struct.pack_into("<H", rec, 0x28, base)
    rec[0x2A] = stride
    return bytes(rec)


def _write_objects(path: Path) -> None:
    records = [
        _object_record("MULTIPART", 0, 2),
        _object_record("SINGLE", 2, 1),
    ]
    path.write_bytes(struct.pack("<I", len(records)) + b"".join(records))


class GroupedGraphicsTests(unittest.TestCase):
    def test_parsers_and_group_plan(self) -> None:
        with tempfile.TemporaryDirectory() as temp:
            root = Path(temp)
            _write_init(root / "INIT")
            _write_npcs(root / "NPCS.DAT")
            _write_objects(root / "OBJECTS.DAT")

            bases, class1, class3 = grouped_graphics.parse_init(root / "INIT")
            self.assertEqual(bases, (0, 10, 20, 30))
            self.assertEqual(len(class1), 175)
            self.assertEqual(len(class3), 12)

            groups, warnings = grouped_graphics.build_group_plan(root, 40)
            self.assertEqual(warnings, [])

            npc0 = next(group for group in groups if group.ref == "npcs/000_test_one")
            self.assertEqual(npc0.res3_ids, [0, 1, 2, 3])
            self.assertEqual(npc0.variant_count, 2)
            self.assertEqual([(f.variant, f.view) for f in npc0.frames],
                             [(0, 0), (0, 1), (1, 0), (1, 1)])

            obj = next(group for group in groups if group.ref == "objects/000_multipart")
            self.assertEqual(obj.res3_ids, [20, 21])
            self.assertFalse(any(group.ref == "objects/001_single" for group in groups))

            scene = next(
                group for group in groups
                if group.ref == "scene_descriptors/descriptor_000"
            )
            self.assertEqual(scene.res3_ids, [10, 11, 12, 13])

            effect = next(
                group for group in groups
                if group.ref.startswith("effects/00_fireball_shared_effect_class")
            )
            self.assertEqual(effect.res3_ids, [30, 31, 32, 33])
            self.assertIn("Fireball", effect.extra["certified_producers"])

    def test_missing_optional_dat_files_are_warnings(self) -> None:
        with tempfile.TemporaryDirectory() as temp:
            root = Path(temp)
            _write_init(root / "INIT")
            groups, warnings = grouped_graphics.build_group_plan(
                root,
                40,
                families=(grouped_graphics.FAMILY_NPCS,
                          grouped_graphics.FAMILY_OBJECTS),
            )
            self.assertEqual(groups, [])
            self.assertEqual(len(warnings), 2)
            self.assertIn("NPCS.DAT", warnings[0])
            self.assertIn("OBJECTS.DAT", warnings[1])


if __name__ == "__main__":
    unittest.main()
