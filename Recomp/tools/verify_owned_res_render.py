#!/usr/bin/env python3
"""Validate selected genuine owned Ravenloft RES images in compiled CODE.2.

Do not upload, print, or archive copyrighted payloads or generated pixels.
Run only when the original-media checkout is available; normal CI remains
completely media-free. This is a resource/render smoke, NOT game acceptance.
"""
import hashlib
import pathlib
import subprocess
import sys
import struct
import tempfile

KNOWN = {
    "RES0": ("5e30584947c9627e31b5dd2573ad75c2d3cc9fe796ac669e0ca5d02d6db7e61d", 15, 0),
    "RES1": ("c13b8ae42f7d3b65c06d8413627f40a2a155a7404da44abfc158b38a4e5efd19", 336, 1),
    "RES4": ("d8989c50fa58e317ff33465e7baec16e970f40586417d790fd8a1b9005c60f46", 0, None),
    "COLORS": ("e366986bf701ba832aa769bf99f0db90a76af658300ce3d02e7039a970a9baf5", None, None),
}

def main() -> int:
    if len(sys.argv)!=3:
        raise SystemExit("usage: verify_owned_res_render.py <compiled-graphics-exe> <original-RLOFT-dir>")
    executable=pathlib.Path(sys.argv[1]).resolve()
    root=pathlib.Path(sys.argv[2]).resolve()
    for filename,(sha,_,_) in KNOWN.items():
        p=root/filename
        digest=hashlib.sha256(p.read_bytes()).hexdigest()
        if digest!=sha: raise RuntimeError(f"{filename}: original SHA-256 mismatch, abort")
        print(f"VERIFIED_ORIGINAL_FILE={filename} SHA256={digest}")
    # Print structural metadata only, never original payload bytes.
    res4=(root/"RES4").read_bytes()
    entry_count=struct.unpack_from("<I",res4,0)[0]
    for entry in (0,100,500):
        if entry>=entry_count:raise RuntimeError("RES4 directory entry absent")
        offset=struct.unpack_from("<I",res4,4+entry*4)[0]
        type_value=res4[offset+2] if res4[offset:offset+2]==b"EH" else -1
        hdr_size=struct.unpack_from("<H",res4,offset+3)[0] if type_value>=0 else -1
        decoded=struct.unpack_from("<H",res4,offset+5)[0] if type_value>=0 else -1
        print(f"RES4_METADATA entry={entry} type={type_value} header_size={hdr_size} decoded={decoded}")
    # Select one authentic 32-column texture (decoded 4,096 bytes) in
    # addition to the explicit 64-column reference IDs.
    small_type6=next((
        entry for entry in range(entry_count)
        if (lambda off: (
            res4[off:off+2]==b"EH" and
            res4[off+2]==6 and
            struct.unpack_from("<H",res4,off+3)[0]==0 and
            struct.unpack_from("<H",res4,off+5)[0]==4096
        ))(struct.unpack_from("<I",res4,4+entry*4)[0])
    ),None)
    if small_type6 is None:raise RuntimeError("No 32-column original RES4 texture")
    print(f"RES4_32_COLUMN_EXAMPLE entry={small_type6} decoded=4096")
    with tempfile.TemporaryDirectory(prefix="rloft-private-source-raster-") as tmp:
        for archive,(sha,entry,slot) in KNOWN.items():
            if entry is None or slot is None: continue
            stem=pathlib.Path(tmp)/archive
            command=[str(executable),"--res",str(root/archive),
                     str(entry),str(slot),"0",str(stem),
                     str(root/"COLORS"),"0","0"]
            try:
                result=subprocess.run(command,capture_output=True,text=True,timeout=20)
            except subprocess.TimeoutExpired as error:
                trace=(error.stderr or b"").decode("utf-8","replace") if isinstance(error.stderr,bytes) else str(error.stderr)
                raise RuntimeError(f"{archive}:{entry}: timed out during original RES source path. "
                                   f"Last authenticated stage: {trace[-1200:]}")
            if result.returncode:
                # Exception labels have no raw asset bytes.
                raise RuntimeError(f"{archive}:{entry}: compiled CODE.2 render failed: "
                                   f"{result.stderr[-1500:]}")
            if "ORIGINAL_RES_TYPE1_2ECB4_2FD9C_2F29C_2975C_2A380_PASS" not in result.stdout:
                raise RuntimeError(f"{archive}:{entry}: missing actual CODE.2 resource draw acknowledgement")
            pixels=stem.with_suffix(".idx").read_bytes()
            bmp=stem.with_suffix(".bmp").read_bytes()
            if len(pixels)!=128000 or len(bmp)!=129078:
                raise RuntimeError(f"{archive}:{entry}: invalid final indexed frame dimensions")
            nonzero=sum(pixel!=0 for pixel in pixels)
            if nonzero==0:
                raise RuntimeError(f"{archive}:{entry}: authentic record drew no visible indices")
            # Publish hashes and pixel counts only, NEVER original raster bytes.
            print(f"ORIGINAL_RES_RENDER_PASS archive={archive} entry={entry} "
                  f"nonzero_pixels={nonzero} "
                  f"indexed_sha256={hashlib.sha256(pixels).hexdigest()} "
                  f"palettized_bmp_sha256={hashlib.sha256(bmp).hexdigest()}")
        for entry in (0, 100, 500, small_type6):
            stem=pathlib.Path(tmp)/f"RES4-{entry}"
            cmd=[str(executable),"--res4",str(root/"RES4"),
                 str(entry),"128","0",str(stem),str(root/"COLORS"),"0","0"]
            p=subprocess.run(cmd,capture_output=True,text=True,timeout=30)
            if p.returncode:
                raise RuntimeError(f"RES4:{entry}: authentic 338C8 source rendering failed: "
                                   +p.stderr[-1200:])
            if "COMPILED_CODE2_RES4_2ECB4_TYPE6_3395C_338C8_COLUMN_SAMPLER_PASS" not in p.stdout:
                raise RuntimeError(f"RES4:{entry}: missing original column sampler acknowledgement")
            indices=stem.with_suffix(".idx").read_bytes()
            bitmap=stem.with_suffix(".bmp").read_bytes()
            if len(indices)!=128000 or len(bitmap)!=129078:
                raise RuntimeError(f"RES4:{entry}: incorrect indexed-frame dimensions")
            count=sum(v!=0 for v in indices)
            if count==0:raise RuntimeError(f"RES4:{entry}: no authentic texture pixels")
            print(f"ORIGINAL_RES4_COLUMN_RENDER_PASS entry={entry} nonzero_pixels={count} "
                  f"indexed_sha256={hashlib.sha256(indices).hexdigest()} "
                  f"palettized_bmp_sha256={hashlib.sha256(bitmap).hexdigest()}")
    print("ORIGINAL_RES_REAL_MEDIA_DRAW_AND_RES4_COLUMN_PASS; FULL_WORLD_RENDER=UNVERIFIED")
    return 0

if __name__=="__main__":
    raise SystemExit(main())
