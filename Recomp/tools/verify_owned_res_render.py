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
import tempfile

KNOWN = {
    "RES0": ("5e30584947c9627e31b5dd2573ad75c2d3cc9fe796ac669e0ca5d02d6db7e61d", 15, 0),
    "RES1": ("c13b8ae42f7d3b65c06d8413627f40a2a155a7404da44abfc158b38a4e5efd19", 336, 1),
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
    with tempfile.TemporaryDirectory(prefix="rloft-private-source-raster-") as tmp:
        for archive,(sha,entry,slot) in KNOWN.items():
            if entry is None: continue
            stem=pathlib.Path(tmp)/archive
            command=[str(executable),"--res",str(root/archive),
                     str(entry),str(slot),"0",str(stem),
                     str(root/"COLORS"),"0","0"]
            result=subprocess.run(command,capture_output=True,text=True,timeout=45)
            if result.returncode:
                # Exception labels have no raw asset bytes.
                raise RuntimeError(f"{archive}:{entry}: compiled CODE.2 render failed: "
                                   f"{result.stderr[-1500:]}")
            if "ORIGINAL_RES_TYPE1_COMPILED_2A20C_2F29C_2975C_2A380_PASS" not in result.stdout:
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
    print("ORIGINAL_RES_REAL_MEDIA_DRAW_PASS; FULL_WORLD_RENDER=UNVERIFIED")
    return 0

if __name__=="__main__":
    raise SystemExit(main())
