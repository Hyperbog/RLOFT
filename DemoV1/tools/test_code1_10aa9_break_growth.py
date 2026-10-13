#!/usr/bin/env python3
"""Guard original CODE.1 10BB4 DS-break comparison in the native source.

The public test checks the translated source without distributing game bytes.
With --original it also authenticates the owner's full CODE.1 and the
original x86 operand sequence. This is not a DOS runtime parity test.
"""
from __future__ import annotations

import argparse
import hashlib
from pathlib import Path

ORIGINAL_SHA256 = "fd9b709ceee190e6a70870f9012ebea3bc437cae6bc74e0150d6f922fa750fed"
ROOT = Path(__file__).resolve().parents[2]
BEGIN = "[[nodiscard]]u32 function_10aa9("
END = "static_assert(0x10C46u-0x10AA9u==413u);"
EXPECTED = (
    "u32 desired=size+limit;",
    "u32 old=gb.extend_10dd7?gb.extend_10dd7(desired):0xffffffffu;",
    "if(old==0xffffffffu||old>0xfffffff8u||desired<=old)return 0u;",
    "raw=old;usable=desired-old;",
)
# Source address -> authenticated physical offset + original bytes.
# The complete-file SHA is mandatory before trusting these LE object offsets.
WITNESSES = (
    (0x10B84, 0x3D2A8, bytes.fromhex("03 35 f0 52 00 00")),  # ADD ESI,[52F0]
    (0x10B97, 0x3D2BB, bytes.fromhex("56 e8 3a 02 00 00")),  # PUSH ESI / CALL 10DD7
    (0x10BB4, 0x3D2D8, bytes.fromhex("39 c6 0f 86")),        # CMP ESI,EAX / JBE
    (0x10BBC, 0x3D2E0, bytes.fromhex("29 c6")),              # SUB ESI,EAX
)


def verify_source(source: str) -> None:
    if source.count(BEGIN) != 1:
        raise ValueError("CODE.1 10AA9 must have exactly one source definition")
    start = source.index(BEGIN)
    end = source.find(END, start)
    if end < start:
        raise ValueError("CODE.1 10AA9 boundary was not found")
    body = source[start:end]
    positions = [body.find(part) for part in EXPECTED]
    if any(pos < 0 for pos in positions) or positions != sorted(positions):
        raise ValueError("CODE.1 10AA9 has lost original desired/old comparison and extent")
    if "||size<=old" in body or "usable=size-old;" in body:
        raise ValueError("CODE.1 10AA9 has reverted to the wrong requested-size arithmetic")


def verify_original(path: Path) -> None:
    media = path.read_bytes()
    if hashlib.sha256(media).hexdigest() != ORIGINAL_SHA256:
        raise ValueError("Original CODE.1 file identity differs; refusing x86 evidence")
    for logical, physical, instruction in WITNESSES:
        if media[physical:physical + len(instruction)] != instruction:
            raise ValueError(f"Original CODE.1 x86 instruction mismatch at {logical:05X}")


def self_test(source: str) -> None:
    # A regression must distinguish the previous incorrect translation.
    for bad in (source.replace("desired<=old", "size<=old", 1),
                source.replace("usable=desired-old;", "usable=size-old;", 1)):
        if bad == source:
            raise ValueError("CODE.1 10AA9 expected semantic token missing")
        try:
            verify_source(bad)
        except ValueError:
            continue
        raise ValueError("CODE.1 10AA9 guard failed to reject a prior incorrect variant")


def main() -> int:
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument("--source", type=Path, default=ROOT / "Code1" / "Code1.cpp")
    p.add_argument("--original", type=Path, help="optional, private original CODE.1")
    args = p.parse_args()
    try:
        source = args.source.read_text(encoding="utf-8")
        verify_source(source)
        self_test(source)
        if args.original:
            verify_original(args.original)
    except (OSError, ValueError) as exc:
        p.exit(1, f"CODE1_10AA9_REJECTED: {exc}\n")
    print("CODE1_10AA9_SOURCE_PASS" + (" ORIGINAL_X86_PASS" if args.original else ""))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
