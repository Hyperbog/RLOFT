"""Source-only guard: all 17 original CODE.2 F790 caller argument origins.

This verifier intentionally does not assert that the native 70-Hz input
loop reproduces all original DOS event cadences. The connected native
bridge is not yet live; manual Windows and DOS acceptance are separate.
"""
from __future__ import annotations

import argparse
import hashlib
import struct
from pathlib import Path

ROOT=Path(__file__).resolve().parents[2]
SHA="853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314"
OBJ1=0x3AF24
TARGET=0xF790
GLOBAL=(
    0x1134C,0x1173B,0x1178E,0x11B63,0x127EC,0x128A3,
    0x12AFA,0x12BA4,0x12D50,0x12DFD,0x131BB)
EMPTY=(0x1184A,0x11882,0x118FF)
LOCAL=(0x119FC,0x11F47)
SELECTED=(0x130DE,)
ALL=GLOBAL+EMPTY+LOCAL+SELECTED

def check_original(path:Path)->None:
    original=path.read_bytes()
    assert len(original)==558689
    assert hashlib.sha256(original).hexdigest()==SHA
    b=original[OBJ1:OBJ1+0x3A852]
    callsites={
        i for i in range(len(b)-4)
        if b[i]==0xE8 and
        i+5+struct.unpack_from("<i",b,i+1)[0]==TARGET
    }
    assert callsites==set(ALL),f"F790 callers changed {callsites^set(ALL)}"
    assert len(GLOBAL)==11 and len(EMPTY)==3 and len(LOCAL)==2 and len(SELECTED)==1
    held=bytes.fromhex("0fbf0500860000")
    known=bytes.fromhex("0fb60502860000")
    for at in GLOBAL:
        context=b[at-72:at]
        assert known in context and held in context,hex(at)
        assert context.rfind(known)<context.rfind(held),hex(at)
    for at in EMPTY:
        assert b[at-4:at]==bytes.fromhex("6a006aff"),hex(at)
    assert b[LOCAL[0]-4:LOCAL[0]]==bytes.fromhex("ff342455")
    assert b[LOCAL[1]-8:LOCAL[1]]==bytes.fromhex("ff742408ff742414")
    assert b[SELECTED[0]-3:SELECTED[0]]==bytes.fromhex("6a0053")
    assert b[0xF7B4:0xF7B4+15]==bytes.fromhex("803dac6d000000740f689c750000e8")
    assert b[0xF7DB:0xF7DB+9]==bytes.fromhex("83fbff0f8498050000")
    print("PASS: 17 original E8 callers: 11 global, 3 empty, 2 local, 1 selected")
    print("PASS: pending 6DAC precedence and ordinary -1 empty-hand gate")

def check_source()->None:
    bridge=(ROOT/"DemoV1/include/rloft/original_f790_runtime_event.hpp").read_text()
    catalog=(ROOT/"DemoV1/include/rloft/original_f790_caller_catalog.hpp").read_text()
    catalog_test=(ROOT/"DemoV1/tests/original_f790_caller_catalog_tests.cpp").read_text()
    runtime=(ROOT/"DemoV1/include/rloft/original_f790_runtime_description.hpp").read_text()
    tests=(ROOT/"DemoV1/tests/original_f790_runtime_description_tests.cpp").read_text()
    host=(ROOT/"DemoV1/src/win32_main.cpp").read_text()
    presenter=(ROOT/"DemoV1/src/party_modal_presenter.cpp").read_text()
    presenter_header=(ROOT/"DemoV1/include/rloft/party_modal_presenter.hpp").read_text()
    cmake=(ROOT/"DemoV1/CMakeLists.txt").read_text()
    for sample in ("args.pending_6dac","pending_text_759c",
                   "args.item_id,args.known","sequence.source_event_f790",
                   "one_shared_game_rng","observe_tick_30700"):
        assert sample in bridge,sample
    assert "live_inventory.container_weight_half_units" in runtime
    assert "original_count!=objects.size()" in runtime
    assert "intentionally_invalid" in tests and "EXACT 759C" in tests
    # The original text-only branch can run at host redraw cadence:
    # unlike F66C it never samples shared RNG or 30700. Actual F790
    # events and the RES2 ribbon stay explicitly NOT live-wired.
    assert "OriginalF790RuntimeDescription::compose(" in presenter
    assert 'original_objects_bytes_(assets.source().read("OBJECTS.DAT"))' in presenter
    assert 'original_code2_descriptions_(assets.source().read("CODE.2"))' in presenter
    assert 'original_t_descriptions_(assets.source().read("T"))' in presenter
    assert "original_objects_bytes_" in presenter_header
    assert "OriginalF790RuntimeEvent" not in presenter
    assert "OriginalF790ModalSequence" not in presenter
    assert "original_random_" not in presenter and "next15(" not in presenter
    assert "framebuffer,font1_,0x65," in presenter
    assert "OriginalF790RuntimeEvent" not in host, "unverified live F790 hookup"
    assert "original_f790_runtime_description_tests" in cmake
    assert "OriginalF790CallerCatalog::arguments_at" in bridge
    assert "unknown CODE.2 F790 E8 site" in bridge
    assert "kDirectCalls" in catalog
    for at in ALL:
        assert f"0x{at:05x}U" in catalog, f"source catalog missing {at:05X}"
    assert "ownership_counts" in tests
    assert "original_f790_caller_catalog_tests" in cmake
    assert "verified_addresses" in catalog_test
    assert "original_f790_runtime_event_contract_tests" in cmake
    print("PASS: live original F790 text-only, cached DATA/T and mutable inventory")
    print("PASS: no live F790 event or gameplay RNG at VGA presentation cadence")

def main()->None:
    ap=argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--original-code2",type=Path)
    args=ap.parse_args()
    check_source()
    if args.original_code2:check_original(args.original_code2)

if __name__=="__main__":
    main()
