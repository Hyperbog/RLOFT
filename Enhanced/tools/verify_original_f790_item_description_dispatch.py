"""Check original CODE.2 F790 category jump dispatch and native integration gate."""
from __future__ import annotations
import argparse
import hashlib
import struct
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SHA256 = "853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314"
BASE = 0x3AF24
CATEGORIES = (0xFA85,)*3 + (0xFB3D,)*4 + (0xF8D4,)*3
WINDOWS = {
    0xF7E4: "89dac1e2068b35d855020001dabf9c75000001d6",
    0xF812: "a1d8550200807c0237007465837c242800745e",
    0xF8AF: "89dfc1e706a1d855020001df8a44072f2c023c090f87740200000fb6c02eff248568f70000",
    0xF8D4: "89dfc1e70601df897c24088b3dd8550200037c24080fb66f3a0fb64738",
    0xFB3D: "6a3dbf9c75000029c94931c0f2aef7d14981c19c75000089df",
    0xFD58: "53e8660f020083c404689c750000e801f9ffff83c404e88d090200",
}
def check_binary(path: Path) -> None:
    raw=path.read_bytes()
    assert len(raw)==558689 and hashlib.sha256(raw).hexdigest()==SHA256, "wrong original CODE.2"
    b=raw[BASE:BASE+0x3A852]
    for at,encoded in WINDOWS.items():
        original=bytes.fromhex(encoded)
        assert b[at:at+len(original)]==original,f"original CODE.2 bytes changed {at:05X}"
    assert struct.unpack_from("<10I",b,0xF768)==CATEGORIES, "original item category jump table changed"
    print("PASS: CODE.2 six F790 byte windows, ten original category slots (3/4/3)")
def check_source() -> None:
    desc=(ROOT/"DemoV1/src/inventory_item_description.cpp").read_text(encoding="utf-8")
    header=(ROOT/"DemoV1/include/rloft/original_f790_item_category_route.hpp").read_text(encoding="utf-8")
    host=(ROOT/"DemoV1/src/win32_main.cpp").read_text(encoding="utf-8")
    cmake=(ROOT/"DemoV1/CMakeLists.txt").read_text(encoding="utf-8")
    # Original F790 dispatches 2..11 into three original text-formatting
    # branches. Today's provisional flat held-item helper is NOT a substitute.
    assert 'return name+": WEIGHT:"+weight_text(item.weight_half_units);' in desc
    assert "OriginalF790ModalSequence" not in host, "do not live-wire provisional description"
    assert "static_cast<std::uint8_t>(category-2U)" in header
    assert "original_f790_item_category_route_tests" in cmake
    assert "original_f790_item_description_dispatch_tests" in cmake
    print("PASS: native simplified item description is gated from live F790 integration")
def main() -> None:
    ap=argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--original-code2",type=Path)
    ap.add_argument("--binary-only",action="store_true")
    args=ap.parse_args()
    if not args.binary_only:check_source()
    if args.original_code2:check_binary(args.original_code2)
    elif args.binary_only:ap.error("--binary-only requires --original-code2")
if __name__=="__main__":main()
