"""No-retail-media original-world format fixtures and independent comparison gates."""
from __future__ import annotations
import json
from pathlib import Path
import subprocess
import sys
import tempfile
import unittest

from original_world_oracle import OracleError, audit, compare, inspect_levels, inspect_sintab

def put16(buf,off,num):buf[off:off+2]=int(num).to_bytes(2,"little")
def put32(buf,off,num):buf[off:off+4]=int(num).to_bytes(4,"little")

def levels_fixture():
    a=29
    b=a+0x2a4+8
    c=b+4
    d=c+11
    e=d+11
    f=e+11
    g=f+1300
    rec=bytearray(g+4+4+4)
    for k,x in enumerate((a,b,c,d,e,f,g)):put32(rec,1+4*k,x)
    put16(rec,a+1,2);put16(rec,a+3,2)
    for k in (0x28a,0x28c,0x28e):put16(rec,a+k,1)
    put16(rec,a+0x2a4,0x2000)
    rec[f]=1;rec[f+1]=12
    put16(rec,g,2);put16(rec,g+2,2)
    img=bytearray(132)
    put32(img,0,32)
    for id in range(32):
        put32(img,4+id*4,len(img))
        img.extend(rec)
    return img,b,f

def sintab_fixture():
    sz=0x205
    img=bytearray(0x1f42+127*(2+sz)+512)
    put32(img,0x1b38,127)
    put32(img,0x1b3c,508)
    put32(img,0x1b40,128)
    put32(img,0x1f3e,127*sz)
    cursor=0x1f42
    for k in range(127):
        put16(img,cursor,sz);cursor+=2
        for i in range(129):put16(img,cursor+2*i,0x204)
        img[cursor+sz-1]=0xc3;cursor+=sz
    return img

def native_from(doc):
    return {"schema":"rloft-native-world-system/v1",
            **{key:doc["summary"][key] for key in
               ("level_count","level_cells","controls","event_slots",
                "event_commands","level_archive_bytes","sintab_bytes",
                "sintab_scales","sintab_scale_bytes")},
            "levels":[{key:item[key] for key in
                ("id","w","h","cells","controls","a","b","c","events","commands")}
                for item in doc["levels"]]}

class OriginalWorldOracleTests(unittest.TestCase):
    def test_all_levels_scales_hashes_and_no_false_parity(self):
        a=audit(bytes(levels_fixture()[0]),bytes(sintab_fixture()))
        self.assertEqual(a["summary"]["level_cells"],128)
        self.assertEqual(a["summary"]["bit2000_cell_words"],32)
        self.assertEqual(len(a["levels"]),32)
        self.assertEqual(len(a["sintab_blocks"]),127)
        self.assertEqual(len(a["levels"][22]["cells_sha256"]),64)
        self.assertFalse(a["native_gameplay_parity"])
        self.assertFalse(a["pinned_original"])

    def test_pinned_identity_rejects_synthetic_input(self):
        with self.assertRaisesRegex(OracleError,"SHA256"):
            audit(bytes(levels_fixture()[0]),bytes(sintab_fixture()),True)

    def test_native_report_structural_agreement_and_targeted_diff(self):
        orig=audit(bytes(levels_fixture()[0]),bytes(sintab_fixture()))
        native=native_from(orig)
        result=compare(orig,native)
        self.assertEqual(result["status"],"STRUCTURAL_MATCH")
        self.assertFalse(result["native_cell_bytes_compared"])
        native["levels"][22]["controls"]=11
        result=compare(orig,native)
        self.assertEqual(result["first_divergence"]["field"],"levels/22/controls")
        self.assertEqual(result["difference_count"],1)

    def test_control_corruption(self):
        img,b,_=levels_fixture()
        put16(img,132+b,25)
        with self.assertRaisesRegex(OracleError,"control list"):
            inspect_levels(bytes(img))

    def test_event_opcode_corruption(self):
        img,_,f=levels_fixture()
        img[132+f+1]=13
        with self.assertRaisesRegex(OracleError,"opcode"):
            inspect_levels(bytes(img))

    def test_level_record_alias_corruption(self):
        img,_,_=levels_fixture()
        put32(img,8,132)
        with self.assertRaisesRegex(OracleError,"directory"):
            inspect_levels(bytes(img))

    def test_sintab_code_end_corruption(self):
        img=sintab_fixture()
        img[0x1f42+2+0x205-1]=0
        with self.assertRaisesRegex(OracleError,"RET"):
            inspect_sintab(bytes(img))

    def test_sintab_lookup_corruption(self):
        img=sintab_fixture()
        put16(img,0x1f42+2,0x205)
        with self.assertRaisesRegex(OracleError,"offsets"):
            inspect_sintab(bytes(img))

    def test_sintab_reserved_bytes_corruption(self):
        img=sintab_fixture()
        img[-1]=5
        with self.assertRaisesRegex(OracleError,"trailer"):
            inspect_sintab(bytes(img))

    def test_cli_private_original_bytes_not_in_result(self):
        with tempfile.TemporaryDirectory() as folder:
            root=Path(folder)
            lv=root/"LEVELS.DAT";sin=root/"SINTAB"
            nat=root/"native.json";out=root/"report.json"
            lv.write_bytes(levels_fixture()[0])
            sin.write_bytes(sintab_fixture())
            nat.write_text(json.dumps(native_from(audit(lv.read_bytes(),sin.read_bytes()))))
            run=subprocess.run([sys.executable,
                 str(Path(__file__).with_name("original_world_oracle.py")),
                 "--levels",str(lv),"--sintab",str(sin),
                 "--native-report",str(nat),"--report",str(out)],
                 capture_output=True,text=True)
            self.assertEqual(run.returncode,0,run.stderr)
            result=json.loads(out.read_text())
            self.assertEqual(result["status"],"STRUCTURAL_MATCH")
            self.assertFalse(result["native_gameplay_parity"])
            self.assertNotIn(str(root),out.read_text())
if __name__=="__main__":
    unittest.main()
