#!/usr/bin/env python3
"""Media-free RES4 type-6 column-major rendering via compiled CODE.2 338C8."""
import pathlib
import struct
import subprocess
import sys
import tempfile

def payload(columns=32):
    pixels=bytes((column+row)&255 for column in range(columns) for row in range(128))
    encoded=b"".join(b"\xff"+pixels[i:i+8] for i in range(0,len(pixels),8))
    entry=b"EH"+bytes((6,))+struct.pack("<HH",0,len(pixels))+encoded
    return struct.pack("<III",2,12,12)+entry

def test(executable,path,entry,height,shade,stem,success):
    p=subprocess.run([str(executable),"--res4",str(path),str(entry),
                      str(height),str(shade),str(stem)],
                     capture_output=True,text=True,timeout=20)
    if (p.returncode==0)!=success:
        raise AssertionError(f"{path.name}: expected success={success}; {p.stdout}\n{p.stderr}")
    if not success:return
    if "COMPILED_CODE2_RES4_2ECB4_37590_37700_3395C_338C8_PASS" not in p.stdout:
        raise AssertionError("Original CODE.2 sampler not executed")
    data=stem.with_suffix(".idx").read_bytes()
    bmp=stem.with_suffix(".bmp").read_bytes()
    if len(data)!=128000 or len(bmp)!=129078:raise AssertionError("Frame size")
    if data[90*320+144+4] != 14-shade:
        raise AssertionError("Wrong original fixed-point texel sampling")
    if data[90*320+144+5] != 15-shade:
        raise AssertionError("Wrong neighboring VGA plane or texture column")
    if data[79*320+144+4]!=0 or data[208*320+144+4]!=0:
        raise AssertionError("Incorrect original 338C8 vertical bounds")
    if bmp[:2]!=b"BM" or struct.unpack_from("<H",bmp,28)[0]!=8:
        raise AssertionError("Not indexed BMP")
def main():
    if len(sys.argv)!=2:raise SystemExit("usage: test_compiled_res4.py <exe>")
    exe=pathlib.Path(sys.argv[1]).resolve()
    with tempfile.TemporaryDirectory(prefix="rloft-res4-") as td:
        root=pathlib.Path(td)
        original=root/"type6.RES"
        original.write_bytes(payload())
        test(exe,original,1,128,0,root/"type6",True)
        invalid=root/"invalid.RES"
        data=bytearray(payload());data[14]=1  # first typed EH resource type at offset 12+2
        invalid.write_bytes(data)
        test(exe,invalid,1,128,0,root/"not-type6",False)
        truncated=root/"truncated.RES"
        truncated.write_bytes(payload()[:-1])
        test(exe,truncated,1,128,0,root/"no-eof",False)
        test(exe,original,1,0,0,root/"invalid-height",False)
    print("ORIGINAL_RES4_TYPE6_338C8_MULTIPLANE_INDEXED_COLUMNS_PASS")
if __name__=="__main__":main()
