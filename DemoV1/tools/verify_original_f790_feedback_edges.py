"""Source-owned feedback F790 edges, with optional original CODE.2 oracle.

No live event dispatch is assumed. This checker complements the C++ state
regression and guards against linking original RNG to the host redraw loop.
"""
from __future__ import annotations
import argparse
import hashlib
import struct
from pathlib import Path
ROOT=Path(__file__).resolve().parents[2]
SHA="853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314"
BASE=0x3AF24
WINDOWS={
    0x116F5:"572bc949b000f2ae4f8a06",
    0x11717:"c605ac6d000001e8ddef010083c036a3a86d0000",
    0x1176A:"c605ac6d000001e88aef010083c036a3a86d0000",
    0x13184:"803dac6d0000007436e86ed501003b05a86d00007629",
    0x131AA:"c705a86d000000000000c605ac6d000000e8d0c5ffff",
}
CALLERS=(0x1173B,0x1178E,0x131BB)

def verify_binary(path:Path)->None:
    raw=path.read_bytes()
    assert len(raw)==558689 and hashlib.sha256(raw).hexdigest()==SHA, "not original CODE.2"
    binary=raw[BASE:BASE+0x3A852]
    for offset,hex_bytes in WINDOWS.items():
        expected=bytes.fromhex(hex_bytes)
        assert binary[offset:offset+len(expected)]==expected,hex(offset)
    for at in CALLERS:
        assert binary[at]==0xE8
        assert at+5+struct.unpack_from("<i",binary,at+1)[0]==0xF790,hex(at)
    print("PASS: 5 original feedback instruction windows + 3 F790 E8 call targets")

def verify_source()->None:
    header=(ROOT/"DemoV1/include/rloft/original_f790_feedback_edges.hpp").read_text()
    cpp=(ROOT/"DemoV1/tests/original_f790_feedback_edges_tests.cpp").read_text()
    snapshot=(ROOT/"DemoV1/include/rloft/original_f790_feedback_source_snapshot.hpp").read_text()
    snapshot_test=(ROOT/"DemoV1/tests/original_f790_feedback_source_snapshot_tests.cpp").read_text()
    runtime=(ROOT/"DemoV1/src/inventory_runtime.cpp").read_text()
    host=(ROOT/"DemoV1/src/win32_main.cpp").read_text()
    cmake=(ROOT/"DemoV1/CMakeLists.txt").read_text()
    for fragment in (
        "InventoryOrdinarySwapStatus::LearnedMageSpell",
        "InventoryOrdinarySwapStatus::MageScrollRejected",
        "MageScribeFeedbackKind::CapacityReached",
        "MageScribeFeedbackKind::AlreadyKnown",
        "0x1173bU","0x1178eU","0x131bbU",
        "after_expire_13184","pending_6dac!=1U",
    ):
        assert fragment in header,fragment
    assert "observed_irq_tick_30700<=" in runtime
    assert "*mage_scribe_feedback->deadline_6da8" in runtime
    assert "mage_scribe_feedback.reset();" in runtime
    assert "inventory.expire_scribe_feedback_13184(0x1234U)" in cpp
    assert "inventory.expire_scribe_feedback_13184(0x1235U)" in cpp
    assert "inventory.expire_scribe_feedback_13184(0U)" in cpp
    assert "add_test(NAME original_f790_feedback_edges_tests COMMAND original_f790_feedback_edges_tests)" in cmake
    assert "add_test(NAME original_f790_feedback_source_snapshot_tests COMMAND original_f790_feedback_source_snapshot_tests)" in cmake
    for fragment in ("after_scribe(", "after_expiry(", "owned_text_759c",
                     "feedback.message(spell_name)", "arguments_now()",
                     "OriginalF790RuntimeEvent::arguments_for_direct_call"):
        assert fragment in snapshot,fragment
    assert "const auto t=original_spell_fixture();" in snapshot_test
    assert 'FIREBALL LEARNED!' in snapshot_test
    assert "0x1173bU" in snapshot_test
    assert "0x1178eU" in snapshot_test
    assert "0x131bbU" in snapshot_test
    assert "inv.mage_scribe_feedback.reset();" in snapshot_test
    assert "NAME original_f790_feedback_binary_contract_tests" in cmake
    assert "(void)runtime.inventory().expire_scribe_feedback_13184(" in host, (
        "host expiry handler changed: re-audit original event/RNG cadence")
    assert "OriginalF790FeedbackEdges" not in host
    print("PASS: source feedback transitions, strict unsigned expiry and unwired host boundary")

def main()->None:
    ap=argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--original-code2",type=Path)
    ap.add_argument("--binary-only",action="store_true")
    args=ap.parse_args()
    if not args.binary_only:verify_source()
    if args.original_code2:verify_binary(args.original_code2)
    elif args.binary_only:ap.error("--binary-only requires --original-code2")
if __name__=="__main__":main()
