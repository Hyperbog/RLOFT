#!/usr/bin/env python3
"""Offline original CODE.2 -> native modal resource/placement contract.

The immutable original CODE.2 is authoritative; Code2.cpp is the existing
canonical reconstruction and not by itself proof of byte fidelity.
Without --original-code2, CI checks that the canonical/native mapping
has not silently diverged. With --original-code2, verify identity and
the actual machine instruction sequences at original logical offsets.
No game assets or reversibly complete disassembly are stored here.
"""
from __future__ import annotations

import argparse
import hashlib
import re
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
ORIGINAL_SHA256 = (
    "853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314"
)
ORIGINAL_LENGTH = 558689
OBJECT1_START = 0x3AF24

# Distinct source callsites and their exact operand/call instruction bytes.
# The original helper address in all three cases is 0x2E510.
ORIGINAL_INSTRUCTION_SIGNATURES = {
    0x129B3: "6a026a25ff356c7f00006a73e84cbb0100",
    0x10AAF: "6a024850ff356c7f000068c4000000e84dda0100",
    0x10ACE: "6a0283c50955ff356c7f000068ce000000e82cda0100",
    # Source text label is centered within lane+6, span 0x3C (not 0x50).
    0x12AC1: "6a01689c750000a16c7f00006a3c83c0065068a4000000e82bda0100",
}


def compact(s: str) -> str:
    return re.sub(r"\s+", "", s)


def function(text: str, name: str) -> str:
    match = re.search(r"\b(?:void|u32|s32|bool|int|std::size_t|PartyModalCoverage)\s+"
                      + re.escape(name) + r"\s*\(", text)
    if match is None:
        raise AssertionError(f"missing original/native function {name}")
    start = text.find("{", match.end())
    if start == -1:
        raise AssertionError(f"missing opening brace for {name}")
    depth = 0
    for i in range(start, len(text)):
        if text[i] == "{":
            depth += 1
        elif text[i] == "}":
            depth -= 1
            if depth == 0:
                return compact(text[start:i + 1])
    raise AssertionError(f"unclosed function {name}")


def require(containing: str, expected: str, label: str) -> None:
    if compact(expected) not in containing:
        raise AssertionError(f"{label}: required CODE/native contract absent: {expected}")


def check_source() -> None:
    original = (ROOT / "Code2/Code2.cpp").read_text(encoding="utf-8")
    native = (ROOT / "DemoV1/src/party_modal_presenter.cpp").read_text(
        encoding="utf-8")
    wrapper = (ROOT / "DemoV1/include/rloft/original_party_modal_blit.hpp").read_text(
        encoding="utf-8")
    assets = (ROOT / "DemoV1/src/resource_assets.cpp").read_text(
        encoding="utf-8")
    assert ORIGINAL_SHA256 in original, "canonical Code2 original identity changed"

    e = function(original, "function_12988")
    require(e, "b.call_2e510(0x73u,dst,0x25u,2u)", "12988 RES2 panel")
    require(e, "b.call_30508(0xA4u,dst+6u,0x3Cu,0x759Cu,1u)",
            "12988 burden label geometry")
    c = function(original, "function_10a70")
    require(c, "b.app21->call_3e510(0xC4u,dst,x,2u)",
            "10A70 transition panel")
    require(c, "b.app21->call_3e510(0xCEu,dst,static_cast<u32>(x0+9),2u)",
            "10A70 opened panel")
    resolve = function(original, "function_2e510")
    require(resolve, "function_2e4a4(m,b,source,arg,0u,0xFFFFFFFFu,image)",
            "2E510 complete-frame dispatcher")
    iterate = function(original, "function_2e4a4")
    require(iterate, "count==0xFFFFFFFFu", "2E4A4 frame-count sentinel")
    require(iterate, "constu32stride=load_le16(dg(m,image+4u))",
            "2E4A4 original descriptor frame-height stride")

    enc = function(native, "PartyModalPresenter::draw_encumbrance_panel")
    require(enc, "OriginalPartyModalBlit::complete_2e510("
                 "framebuffer,assets_.type1(2,0x25),0x73,lane)",
            "native RES2:0x25 all-frames")
    require(enc, "framebuffer,font1_,0xA4,lane+6,0x3C",
            "native original burden label geometry")

    cnt = function(native, "PartyModalPresenter::draw_open_container")
    require(cnt, "OriginalPartyModalBlit::complete_2e510("
                 "framebuffer,assets_.type1(2,graphic),0xC4,lane)",
            "native RES2 transition all-frames")
    require(cnt, "OriginalPartyModalBlit::complete_2e510("
                 "framebuffer,panel_art,0xCE,lane)",
            "native RES2 open-panel all-frames")
    require(cnt, "assets_.type1(2,InventoryScreenModel::"
                 "original_container_res2_graphic(desc.subtype))",
            "native open-panel bank and selector")

    # CODE.2 112EC invokes 11268 for one actor lane at a time;
    # 11268 emits the selection outline after inventory/container.
    actor_fn = function(original, "function_11268")
    require(actor_fn, "function_1118c(m,b,slot)", "11268 inventory dispatch")
    require(actor_fn, "slot==load_le32(dg(m,0x7F70u))",
            "11268 selected actor after per-lane content")
    outer_fn = function(original, "function_112ec")
    require(outer_fn, "for(u32slot=0u;slot<4u;++slot)",
            "112EC original actor-major iteration")
    require(outer_fn, "function_11268(m,b,slot)", "112EC per-actor dispatch")
    native_draw = function(native, "PartyModalPresenter::draw")
    require(native_draw, "draw_open_container("
            "framebuffer,*inventory,character_mode,actor_index)",
            "native container before next actor")
    require(native_draw, "if(actor_index==selected_actor)"
            "ExplorationHudPresenter::draw_outline(",
            "native original per-actor selection outline")
    assert "draw_open_container(framebuffer,*inventory,character_mode);" \
           not in native_draw, "post-loop container overlay violates original"
    require(compact(wrapper), "framebuffer.draw(resource,row,column)",
            "original complete-blit wrapper")
    require(compact(wrapper), "framebuffer.draw_frame(resource,frame,row,column)",
            "distinct selected-frame wrapper")
    require(compact(assets), 'res2_.emplace(source_.read("RES2"))',
            "load original RES2 not a mock or fallback")
    print("PASS: 12988/10A70/2E510/2E4A4 canonical-to-native contract")


def check_original(path: Path) -> None:
    data = path.read_bytes()
    assert len(data) == ORIGINAL_LENGTH, "wrong CODE.2 size"
    assert hashlib.sha256(data).hexdigest() == ORIGINAL_SHA256, (
        "wrong CODE.2 version/SHA-256")
    for address, hx in ORIGINAL_INSTRUCTION_SIGNATURES.items():
        expected = bytes.fromhex(hx)
        actual = data[OBJECT1_START + address:
                      OBJECT1_START + address + len(expected)]
        assert actual == expected, (
            f"CODE.2 original instruction mismatch at 0x{address:05X}")
    print("PASS: immutable original CODE.2 hash and four binary callsite signatures")


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--original-code2", type=Path, default=None,
                        help="Optional private original CODE.2 for actual-byte check")
    args = parser.parse_args()
    check_source()
    if args.original_code2 is not None:
        check_original(args.original_code2)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
