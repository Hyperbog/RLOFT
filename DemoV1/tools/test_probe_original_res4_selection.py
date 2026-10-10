"""Deterministic synthetic tests for build-free original RES4 header triage."""
from __future__ import annotations

import importlib.util
import struct
import subprocess
import sys
import tempfile
from pathlib import Path

SCRIPT = Path(__file__).with_name("probe_original_res4_selection.py")
spec = importlib.util.spec_from_file_location("probe_original_res4_selection", SCRIPT)
assert spec and spec.loader
module = importlib.util.module_from_spec(spec)
spec.loader.exec_module(module)


def archive() -> bytes:
    not_type1 = b"EH\x07\x00\x00\x00\x00"
    type1 = b"EH\x01\x08\x00\x01\x00\x40\x00\x08\xff\x00\x00"
    type1 += bytes(i & 255 for i in range(4096))
    count = 2
    out = bytearray(4 + count * 4)
    struct.pack_into("<I", out, 0, count)
    for index, entry in enumerate((not_type1, type1)):
        struct.pack_into("<I", out, 4 + index * 4, len(out))
        out.extend(entry)
    return bytes(out)


def init(base: int) -> bytes:
    out = bytearray(0x10)
    struct.pack_into("<H", out, 0x0E, base)
    return bytes(out)


def test_model() -> None:
    res4 = archive()
    fail = module.inspect(init(0), res4, 0)
    assert fail["source_absolute_entry"] == 0
    assert fail["source_entry_in_bounds"]
    assert fail["source_selected_header"]["type"] == "EH-7"
    assert not fail["source_selected_header_has_ground_shape"]
    assert not fail["selected_decode_verified"]
    assert fail["ground_shape_descriptor_candidates"] == [{
        "index": 1,
        "implied_base_unverified": 1,
        "compression": 0,
        "transform": 0,
        "byte_count": 4109,
        "source_selected": False,
    }]
    assert fail["res4_entry_count"] == 2
    good_header = module.inspect(init(1), res4, 0)
    assert good_header["source_selected_header_has_ground_shape"]
    assert good_header["source_selected_header"]["first_13_bytes_hex"] == (
        "45 48 01 08 00 01 00 40 00 08 FF 00 00")
    assert not good_header["selected_decode_verified"]
    assert good_header["ground_shape_descriptor_candidates"][0]["source_selected"]
    outside = module.inspect(init(0), res4, 55)
    assert outside["source_absolute_entry"] == 55
    assert not outside["source_entry_in_bounds"]
    assert outside["source_selected_header"] is None
    assert len(outside["ground_shape_descriptor_candidates"]) == 1
    assert module.read_offsets(res4) == [12, 19]
    assert module.classify(b"")["type"] == "not-EH"
    try:
        module.read_offsets(struct.pack("<II", 1, 0))
    except ValueError:
        pass
    else:
        raise AssertionError("accepted bad RES4 table")


def test_cli() -> None:
    with tempfile.TemporaryDirectory() as temporary:
        root = Path(temporary)
        (root / "INIT").write_bytes(init(0))
        (root / "RES4").write_bytes(archive())
        lev = bytearray(0x1D + 12)
        struct.pack_into("<h", lev, 0x1D + 7, 0)
        (root / "LEV022.DAT").write_bytes(lev)
        base = [sys.executable, str(SCRIPT), "--init", str(root / "INIT"),
                "--res4", str(root / "RES4"),
                "--lev022", str(root / "LEV022.DAT"), "--json"]
        failed = subprocess.run(base, capture_output=True, text=True, check=False)
        assert failed.returncode == 1
        assert '"source_selected_header"' in failed.stdout
        assert '"ground_shape_descriptor_candidates"' in failed.stdout
        (root / "INIT").write_bytes(init(1))
        good = subprocess.run(base, capture_output=True, text=True, check=False)
        assert good.returncode == 0
        assert '"selected_decode_verified": false' in good.stdout
        wrong = subprocess.run(
            base + ["--selector", "55"],
            capture_output=True, text=True, check=False)
        assert wrong.returncode != 0
        assert "disagrees with original LEV022 header" in wrong.stderr


if __name__ == "__main__":
    test_model()
    test_cli()
    print("PASS: build-free RES4 selected-entry metadata and failure evidence")
