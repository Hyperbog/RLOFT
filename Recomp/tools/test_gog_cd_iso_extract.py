#!/usr/bin/env python3
"""Synthetic ISO9660 regressions for local read-only game.gog RES9/RES10 extraction."""
from pathlib import Path
import subprocess
import sys
import tempfile

SECTOR = 2048


def record(name, extent, size, is_dir):
    file_id = name.encode("ascii") if isinstance(name, str) else bytes(name)
    width = 33 + len(file_id)
    if width % 2:
        width += 1
    result = bytearray(width)
    result[0] = width
    result[2:6] = extent.to_bytes(4, "little")
    result[6:10] = extent.to_bytes(4, "big")
    result[10:14] = size.to_bytes(4, "little")
    result[14:18] = size.to_bytes(4, "big")
    result[25] = 2 if is_dir else 0
    result[28:30] = (1).to_bytes(2, "little")
    result[30:32] = (1).to_bytes(2, "big")
    result[32] = len(file_id)
    result[33:33+len(file_id)] = file_id
    return bytes(result)


def make_iso():
    data = bytearray(SECTOR*32)
    pvd = 16*SECTOR
    data[pvd] = 1
    data[pvd+1:pvd+6] = b"CD001"
    data[pvd+6] = 1
    rr = record(b"\x00", 20, SECTOR, True)
    data[pvd+156:pvd+156+len(rr)] = rr
    root = b"".join((record(b"\x00", 20, SECTOR, True),
                     record(b"\x01", 20, SECTOR, True),
                     record("RLOFT", 21, SECTOR, True)))
    data[20*SECTOR:20*SECTOR+len(root)] = root
    sub = b"".join((record(b"\x00", 21, SECTOR, True),
                    record(b"\x01", 20, SECTOR, True),
                    record("RES9;1", 22, 6, False),
                    record("RES10;1", 23, 9, False)))
    data[21*SECTOR:21*SECTOR+len(sub)] = sub
    data[22*SECTOR:22*SECTOR+6] = b"RES9OK"
    data[23*SECTOR:23*SECTOR+9] = b"RES10TEST"
    return bytes(data)


def run(exe, image, output):
    return subprocess.run([str(exe), "--image", str(image), "--out", str(output)],
                          capture_output=True, text=True, timeout=30)


def test(exe):
    with tempfile.TemporaryDirectory() as temp:
        root = Path(temp)
        payload = make_iso()
        iso = root / "game.gog"
        iso.write_bytes(payload)
        one = root / "private_iso"
        good = run(exe, iso, one)
        assert good.returncode == 0, good.stderr
        assert (one/"RES9").read_bytes() == b"RES9OK"
        assert (one/"RES10").read_bytes() == b"RES10TEST"
        duplicate = run(exe, iso, one)
        assert duplicate.returncode != 0, "Refused-to-overwrite gate failed"
        # GOG releases sometimes store CD images as raw Mode1/2352 sectors.
        raw = root / "game_raw.gog"
        with raw.open("wb") as f:
            for i in range(len(payload)//SECTOR):
                f.write(b"\x00"*16)
                f.write(payload[i*SECTOR:(i+1)*SECTOR])
                f.write(b"\x00"*288)
        two = root/"private_raw"
        raw_status = run(exe, raw, two)
        assert raw_status.returncode == 0, raw_status.stderr
        assert (two/"RES9").read_bytes() == b"RES9OK"
        assert (two/"RES10").read_bytes() == b"RES10TEST"
        bad = root/"corrupt.gog"
        bad.write_bytes(b"\x00"*2000)
        rejected = run(exe, bad, root/"private_bad")
        assert rejected.returncode != 0
        assert not (root/"private_bad"/"RES9").exists()
        print("SYNTHETIC_GOG_CD_ISO9660_EXTRACT_PASS iso2048=YES raw2352=YES "
              "immutable_output=YES malformed_rejected=YES")


if __name__ == "__main__":
    if len(sys.argv) != 2:
        raise SystemExit("Usage: test_gog_cd_iso_extract.py <executable>")
    test(Path(sys.argv[1]).resolve())
