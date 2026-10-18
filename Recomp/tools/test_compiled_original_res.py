#!/usr/bin/env python3
"""Media-free integration test of real CODE.2 cache/draw route, with EH fixtures.

Original RES files are NEVER stored in CI; owners provide them to --res locally.
"""
from __future__ import annotations
import pathlib
import struct
import subprocess
import sys
import tempfile

def archive(entry: bytes) -> bytes:
    return struct.pack("<III", 2, 12, 12) + entry

def image(method: int, compressed: bool, reorder: int=0) -> bytes:
    # Authored source-format fixture, not an original game raster:
    # one 8x8 frame -> 64 interleaved four-plane bytes.
    body=bytes(range(1,65))
    payload=(b"".join(b"\xff"+body[p:p+8] for p in range(0,64,8))
             if compressed else body)
    return b"EH"+bytes((1,))+struct.pack("<h",8)+struct.pack("<hhBBBB",1,8,1,0xff,method,reorder)+payload

def run(exe: pathlib.Path, root: pathlib.Path, name: str, source: bytes,
        expected_success: bool, expected_first: int=1) -> None:
    original=root/(name+".RES")
    original.write_bytes(source)
    stem=root/name
    command=[str(exe),"--res",str(original),"1","0","0",str(stem)]
    p=subprocess.run(command,capture_output=True,text=True,timeout=25)
    if (p.returncode==0)!=expected_success:
        raise AssertionError(f"{name}: status {p.returncode}: {p.stdout}\n{p.stderr}")
    if not expected_success: return
    pixels=stem.with_suffix(".idx").read_bytes()
    bitmap=stem.with_suffix(".bmp").read_bytes()
    if len(pixels)!=128000 or len(bitmap)!=129078:
        raise AssertionError(name+": invalid indexed/BMP frame sizes")
    if "ORIGINAL_RES_TYPE1_COMPILED_2A20C_2F29C_2975C_2A380_PASS" not in p.stdout:
        raise AssertionError(name+": real resource resolver/renderer route not run")
    x,y=156,196 # centered 8x8 into a 320x400 VGA plane surface
    if pixels[y*320+x]!=expected_first or pixels[y*320+x+1]!=2:
        raise AssertionError(name+": original CODE.2 planar layout does not match payload")
    if not any(pixels): raise AssertionError("No raster output")
    if bitmap[:2]!=b"BM" or struct.unpack_from("<H",bitmap,28)[0]!=8:
        raise AssertionError("Bitmap lost the original palette index encoding")

def main() -> int:
    if len(sys.argv)!=2: raise SystemExit("usage: test_compiled_original_res.py <exe>")
    exe=pathlib.Path(sys.argv[1]).resolve()
    with tempfile.TemporaryDirectory(prefix="rloft-original-res-") as folder:
        root=pathlib.Path(folder)
        run(exe,root,"raw",archive(image(0,False)),True)
        run(exe,root,"lzss",archive(image(3,True)),True)
        # The signed type-1 descriptor must not be accepted if any copy would
        # exceed its declared size or if a method is not established.
        run(exe,root,"truncated",archive(image(0,False)[:-1]),False)
        run(exe,root,"unsupported",archive(image(2,False)),False)
        run(exe,root,"not-eh",archive(b"invalid"),False)
        run(exe,root,"bad-table",struct.pack("<III",2,13,13)+image(0,False),False)
        # Backref 0xFEE before that original CODE.2 dictionary slot is
        # defined: fail closed instead of inventing unknown original bytes.
        undefined=b"EH"+bytes((1,))+struct.pack("<h",8)+struct.pack("<hhBBBB",1,8,1,0xff,3,0)+b"\x00\xee\xf0"
        run(exe,root,"undefined-dict",archive(undefined),False)
        # Reordered type-1 resources must be tested with a source-backed
        # corpus later: no synthetic output equality assumption is made here.
    print("RES_ARCHIVE_TYPED_EH_RAW_AND_LZSS_GAME_CACHE_2F29C_DRAW_2975C_PASS")
    return 0

if __name__=="__main__":
    raise SystemExit(main())
