"""Independent synthetic EH6 decoder/oracle tests. No original assets in git."""
from __future__ import annotations

import hashlib
import importlib.util
import json
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


def raster() -> bytes:
    return bytes(i & 255 for i in range(4096))


def eh6() -> bytes:
    original=b"EH\x06\x00\x00\x00\x10"
    pieces=[]
    pixels=raster()
    for i in range(0, len(pixels), 8):
        pieces.append(b"\xff"+pixels[i:i+8])
    return original+b"".join(pieces)


def archive() -> bytes:
    items=[b"EH\x07\x00\x00\x00\x00",eh6(),b"EH\x06\x00\x00\x00\x10\xff"]
    result=bytearray(4+len(items)*4)
    struct.pack_into("<I",result,0,len(items))
    for index,item in enumerate(items):
        struct.pack_into("<I",result,4+index*4,len(result))
        result.extend(item)
    return bytes(result)


def init(base:int)->bytes:
    data=bytearray(0x10)
    struct.pack_into("<H",data,0x0E,base)
    return bytes(data)


def test_decode()->None:
    source=eh6()
    at,length=module.eh6_layout(source)
    assert (at,length)==(7,4096)
    pixels,consumed=module.original_37700_lzss(source[at:],length)
    assert pixels==raster()
    assert consumed==len(source)-at==4608
    assert hashlib.sha256(pixels).hexdigest()==hashlib.sha256(raster()).hexdigest()
    try:
        module.original_37700_lzss(b"\x00\xf0\xff",4096)
    except ValueError:
        pass
    else:
        raise AssertionError("undefined LZSS dictionary tail accepted")
    try:
        module.original_37700_lzss(b"\xff",4096)
    except ValueError:
        pass
    else:
        raise AssertionError("truncated LZSS stream accepted")


def test_model()->None:
    res4=archive()
    fail=module.inspect(init(0),res4,0)
    assert fail["source_absolute_entry"]==0
    assert fail["source_selected_header"]["type"]=="EH-7"
    assert not fail["selected_decode_verified"]
    assert len(fail["ground_shape_descriptor_candidates"])==2
    assert fail["ground_shape_descriptor_candidates"][0]["index"]==1
    assert fail["type_counts"]=={"EH-6":2,"EH-7":1}
    good=module.inspect(init(1),res4,0)
    assert good["source_selected_header"]["type"]=="EH-6"
    assert good["source_selected_header"]["runtime_cache_type_after_2EF38"]==1
    assert good["source_selected_header"]["runtime_uncompressed_length"]==4096
    assert good["selected_decode_verified"]
    assert good["selected_uncompressed_pixels"]==4096
    assert good["selected_pixels_sha256"]==hashlib.sha256(raster()).hexdigest()
    assert good["selected_payload_bytes_consumed"]==4608
    assert good["error"]=="none"
    broken=module.inspect(init(2),res4,0)
    assert not broken["selected_decode_verified"]
    assert "EOF" in broken["error"]
    outside=module.inspect(init(0),res4,55)
    assert outside["source_absolute_entry"]==55
    assert not outside["source_entry_in_bounds"]
    assert outside["source_selected_header"] is None
    assert module.read_offsets(res4)==[16,23,4638]
    try:
        module.read_offsets(struct.pack("<II",1,0))
    except ValueError:
        pass
    else:
        raise AssertionError("accepted malformed RES4 index")


def test_cli()->None:
    with tempfile.TemporaryDirectory() as directory:
        root=Path(directory)
        (root/"INIT").write_bytes(init(0))
        (root/"RES4").write_bytes(archive())
        lev=bytearray(0x1D+12)
        struct.pack_into("<h",lev,0x1D+7,0)
        (root/"LEV022.DAT").write_bytes(lev)
        argv=[sys.executable,str(SCRIPT),"--init",str(root/"INIT"),
              "--res4",str(root/"RES4"),
              "--lev022",str(root/"LEV022.DAT"),"--json"]
        result=subprocess.run(argv,capture_output=True,text=True,check=False)
        assert result.returncode==1
        assert json.loads(result.stdout)["source_selected_header"]["type"]=="EH-7"
        (root/"INIT").write_bytes(init(1))
        result=subprocess.run(argv,capture_output=True,text=True,check=False)
        assert result.returncode==0,result.stderr
        assert json.loads(result.stdout)["selected_decode_verified"]
        wrong=subprocess.run(argv+["--selector","55"],
                             capture_output=True,text=True,check=False)
        assert wrong.returncode!=0
        assert "disagrees with original LEV022 header" in wrong.stderr


if __name__=="__main__":
    test_decode()
    test_model()
    test_cli()
    print("PASS: independent CODE.2 EH6 disk→runtime type1/LZSS oracle")
