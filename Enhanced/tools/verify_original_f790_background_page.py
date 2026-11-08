"""Verify original CODE.2 112EC/F790 VGA save/restore and native model.

Default run checks source; --original-code2 CODE.2 additionally verifies the
immutable original instruction windows and direct E8 call edges.
"""
from __future__ import annotations
import argparse
import hashlib
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
CODE2_SHA256 = "853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314"
BASE = 0x3AF24
WINDOWS = {
    0x11316: "6800140000e82cfd010083c40468400100006a106a006a6250a3687f0000e8837f010083c414",
    0xF79B: "68400100006a106a006a62ff35687f0000e87b9b010083c414",
    0x292BC: "535683ec048b5424148b5c24188b44241c8b4c242085d27d0401d031d285db7d0401d931db",
    0x2932C: "535683ec048b5424148b5c24188b44241c8b4c242085d27d0401d031d285db7d0401d931db",
    0x2930D: "85c07e1485c97e1051505352ff742420e8f20d000083c41483c4045e5bc390",
    0x2937D: "85c07e1485c97c1051505352ff742420e8e60d000083c41483c4045e5bc3",
    0x2A114: "c800000056575251538b5d148b75188b550c8b4d108b7d0853568bc28bd9e8010a00008bf35b03d98bd3",
    0x2A178: "c800000056575251538b5d148b75188b550c8b4d108b7d0853568bc28bd9e89d0900008bf387f75b03d9",
    0x2A1B4: "66b8020151575266bac40366ef5a518bcaf3a403fb594975f55f59d0e480fc107ce266bac403b4ff66ef5b595a5f5ec9c3",
    0x1307C: "f6442418027418ff35687f0000e822e0010083c404e866c80000e8957d0000",
}

def direct_calls(binary: bytes,target: int) -> tuple[int,...]:
    return tuple(i for i in range(len(binary)-4)
                 if binary[i] == 0xE8 and
                 i + 5 + int.from_bytes(binary[i+1:i+5],"little",signed=True) == target)

def check_binary(path: Path) -> None:
    raw = path.read_bytes()
    assert len(raw) == 558689, "not original CODE.2 length"
    assert hashlib.sha256(raw).hexdigest() == CODE2_SHA256, "not original CODE.2 bytes"
    obj = raw[BASE:BASE+0x3A852]
    for at,encoded in WINDOWS.items():
        expected=bytes.fromhex(encoded)
        assert obj[at:at+len(expected)] == expected, f"original CODE.2 address {at:05X}"
    assert 0x11334 in direct_calls(obj,0x292BC)
    assert 0xF7AC in direct_calls(obj,0x2932C)
    assert direct_calls(obj,0x2A114) == (0x2931D,)
    assert direct_calls(obj,0x2A178) == (0x2938D,)
    assert 0x1131B in direct_calls(obj,0x3104C)
    print(f"PASS: CODE.2 {len(WINDOWS)} background instruction windows and source VGA copy direction")

def check_source() -> None:
    h=(ROOT / "DemoV1/include/rloft/original_f790_background_page.hpp").read_text()
    t=(ROOT / "DemoV1/tests/original_f790_background_page_tests.cpp").read_text()
    cmake=(ROOT / "DemoV1/CMakeLists.txt").read_text()
    host=(ROOT / "DemoV1/src/win32_main.cpp").read_text()
    modal=(ROOT / "DemoV1/src/party_modal_presenter.cpp").read_text()
    for value in ("kRow = 0x62", "kColumn = 0", "kHeight = 0x10", "kWidth = 0x140",
                  "screen.capture(kRow,kColumn,kHeight,kWidth)",
                  "screen.restore(*backing_,kRow,kColumn)"):
        assert value in h, value
    assert "static_assert(Page::kSavedPixels == 0x1400" in t
    assert "original_f790_background_page_tests" in cmake
    assert "ui.open_party_panels(*actor);" in host
    assert "PartyModalPresenter::draw(" in modal
    assert "OriginalF790BackgroundPage" not in modal and "OriginalF790BackgroundPage" not in host, (
        "unverified live wiring")
    print("PASS: original rectangle mapped to unwired indexed-native snapshot")

def main() -> None:
    ap=argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--original-code2",type=Path)
    ap.add_argument("--binary-only",action="store_true")
    args=ap.parse_args()
    if not args.binary_only:
        check_source()
    if args.original_code2:
        check_binary(args.original_code2)
    elif args.binary_only:
        ap.error("--binary-only requires --original-code2")

if __name__ == "__main__": main()
