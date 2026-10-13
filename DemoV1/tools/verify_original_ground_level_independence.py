#!/usr/bin/env python3
"""Source-only normal CI guard for CODE.2's level-independent 128-row ground.
Optionally check --original privately authenticated CODE.2 bytes.
"""
from __future__ import annotations
import argparse
import hashlib
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
DEFAULT = ROOT / "DemoV1/src/exploration_presenter.cpp"
SHA = "853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314"
WITNESSES = (
    (0x1F388, "0f bf 05 1f 56 02 00"),            # MOVSX EAX,[2561F] current header
    (0x1F2A5, "c7 05 50 ff 00 00 80 00 00 00"), # FF50=128
    (0x335C8, "c7 05 20 22 02 00 00 08 00 00"),# nearest=0x800
    (0x34A74, "c8 00 00 00 60"),                  # ENTER / PUSHAD
)

def verify_source(text: str) -> None:
    start="bool ExplorationPresenter::original_full_ground_eligible("
    end="void ExplorationPresenter::draw_environment("
    if text.count(start)!=1 or text.count(end)!=1:
        raise ValueError("non-unique source function span")
    region=text[text.index(start):text.index(end)]
    if "frame.source_level_id" in region:
        raise ValueError("original ground path retains a host-only Level22 check")
    for token in (
        "!frame.ground_source_verified",
        "!frame.ground_perspective_ready",
        "frame.ground_raster_source.size()!=",
        "frame.sweep_records.size()!=256U",
        "ray.resource_inner==0x93U",
        "ray.prepass==0U",
        "ray.prepass==1U && ray.resource_outer==0x93U",
    ):
        if token not in region:
            raise ValueError("ground proof requirement lost: "+token)

def verify_original(media: bytes) -> None:
    if len(media)!=558689 or hashlib.sha256(media).hexdigest()!=SHA:
        raise ValueError("original CODE.2 full-file identity failed")
    for addr,raw in WITNESSES:
        want=bytes.fromhex(raw)
        offset=0x3AF24+addr
        if media[offset:offset+len(want)]!=want:
            raise ValueError(f"source instruction differs at {addr:05X}")

def main() -> int:
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--presenter",type=Path,default=DEFAULT)
    parser.add_argument("--original",type=Path)
    args=parser.parse_args()
    try:
        source=args.presenter.read_text(encoding="utf-8")
        verify_source(source)
        mutated=source.replace(
            "if(!frame.ground_source_verified ||",
            "if(frame.source_level_id!=22 ||\n       !frame.ground_source_verified ||",
            1)
        if mutated==source:
            raise ValueError("mutation insertion failed")
        try:
            verify_source(mutated)
        except ValueError:
            pass
        else:
            raise ValueError("Level22-only regression was not detected")
        if args.original:
            verify_original(args.original.read_bytes())
    except (OSError,ValueError) as exc:
        parser.exit(1,f"GROUND_CROSS_LEVEL_REJECTED: {exc}\n")
    print("GROUND_LEVEL_INDEPENDENCE_SOURCE_PASS"+
          (" ORIGINAL_CODE2_PASS" if args.original else ""))
    return 0

if __name__=="__main__":
    raise SystemExit(main())
