"""CODE.2 117A0 ordinary-slot event fanout, strictly source-derived.

This is an independent oracle for event *multiplicity*, argument
provenance and order. It does not prove Windows host timing or DOS pixels.
"""
from __future__ import annotations
import argparse
import hashlib
import struct
from pathlib import Path

ROOT=Path(__file__).resolve().parents[2]
SHA="853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314"
OBJ1_BASE=0x3AF24
WINDOWS={
    0x1172B:"0fb60502860000500fbf050086000050e850e0ffff",
    0x11828:"5756e801feffff83c40885c00f8496020000",
    0x1183A:"6a006affe8018b000083c4086a006affe841dfffff",
    0x11872:"6a006affe8c98a000083c4086a006affe809dfffff",
    0x118DF:"ff742408575356e819fbffff83c41085c0744156",
    0x118FB:"6a006affe88cdeffff",
    0x119F8:"ff342455e88fddffff",
}
F790=(0x1173B,0x1178E,0x1184A,0x11882,0x118FF,0x119FC)
CALLS={
    0x1182A:0x11630,0x1183E:0x1A344,
    0x1184A:0xF790,0x11876:0x1A344,
    0x11882:0xF790,0x118E6:0x11404,
    0x118FF:0xF790,0x119FC:0xF790,
    0x1173B:0xF790,0x1178E:0xF790,
}

def verify_original(path:Path)->None:
    source=path.read_bytes()
    assert len(source)==558689
    assert hashlib.sha256(source).hexdigest()==SHA
    obj=source[OBJ1_BASE:OBJ1_BASE+0x3A852]
    for at,pattern in WINDOWS.items():
        expected=bytes.fromhex(pattern)
        assert obj[at:at+len(expected)]==expected,f"original bytes {at:05X}"
    for at,target in CALLS.items():
        assert obj[at]==0xE8 and \
            at+5+struct.unpack_from("<i",obj,at+1)[0]==target,hex(at)
    assert obj[0x11832:0x1183A]==bytes.fromhex(
        "85c00f8496020000"),"rejection must skip 1184A"
    assert obj[0x118FB:0x11904]==bytes.fromhex(
        "6a006affe88cdeffff"),"container uses empty F790"
    assert obj[0x119F8:0x11A01]==bytes.fromhex(
        "ff342455e88fddffff"),"normal swap local EBP/known tuple"
    print("PASS: seven original 117A0 windows, 10 call targets,")
    print("PASS: success 1173B->1184A, reject 1178E only,")
    print("PASS: Tome 11882, container 118FF, local swap 119FC")

def verify_contract()->None:
    model=(ROOT/"DemoV1/include/rloft/original_f790_ordinary_slot_event_plan.hpp").read_text()
    tests=(ROOT/"DemoV1/tests/original_f790_ordinary_slot_event_plan_tests.cpp").read_text()
    source=(ROOT/"DemoV1/include/rloft/original_f790_feedback_source_snapshot.hpp").read_text()
    cmake=(ROOT/"DemoV1/CMakeLists.txt").read_text()
    runner=(ROOT/"DemoV1/tools/run_f790_source_feedback_loop.py").read_text()
    host=(ROOT/"DemoV1/src/win32_main.cpp").read_text()
    for site in F790:
        assert f"0x{site:05x}U" in model or (
            site in (0x1173B,0x1178E) and
            f"0x{site:05x}U" in
                (ROOT/"DemoV1/include/rloft/original_f790_feedback_edges.hpp").read_text()
        ),hex(site)
    for text in (
        "hand_before_117a0","actor_slot_before_117a0",
        "0x1173bU","0x1184aU","0x1178eU",
        "0x11882U","0x118ffU","0x119fcU",
        "OriginalF790FeedbackSourceSnapshot",
        "pending_text()",
        "after.held.object_id!=-1",
    ):
        assert text in model,text
    assert "HeldItemState before_transaction_hand_8600" in source
    assert "before_transaction_hand_8600.object_id" in source
    assert "status==InventoryOrdinarySwapStatus::LearnedMageSpell" in source
    for text in ("calls.count==2U","0x1173bU","0x1184aU",
                 "0x1178eU","0x11882U","0x118ffU",
                 "0x119fcU","inventory.held={-1,1U}"):
        assert text in tests,text
    assert "original_f790_ordinary_slot_event_plan_tests" in cmake
    assert "NAME original_f790_ordinary_slot_event_oracle_tests" in cmake
    assert "verify_original_f790_ordinary_slot_event_plan.py" in runner
    # Source-owned *diagnostic projection*, not a VGA or RNG dispatch.
    assert "ordinary_slot_before_117a0(" in host
    assert "OriginalF790OrdinarySlotEventPlan::" in host
    assert "f790_trace_t_archive" in host
    assert 'trace->event("f790.source.ordinary"' in host
    assert 'trace->event("f790.source.flags"' in host
    assert "source_event_f790(" not in host
    assert "dispatch_source_call(" not in host
    print("PASS: source status mapping, pre-consumption identity and")
    print("PASS: two-event scribe success, no premature host redraw/RNG binding")

def main()->None:
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument("--original-code2",type=Path)
    p.add_argument("--binary-only",action="store_true")
    args=p.parse_args()
    if not args.binary_only:verify_contract()
    if args.original_code2:verify_original(args.original_code2)
    elif args.binary_only:p.error("--binary-only requires --original-code2")

if __name__=="__main__":
    main()
