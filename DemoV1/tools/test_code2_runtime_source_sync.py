#!/usr/bin/env python3
"""Guard three authenticated CODE.2 corrections used by the native demo.

The normal CTest mode reads only the reconstructed public C++ source.
--original optionally checks privately supplied original DOS bytes after
verifying the *complete* original CODE.2 SHA-256. Neither mode executes DOS.
"""
from __future__ import annotations

import argparse
import hashlib
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
ORIGINAL_SHA256 = "853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314"
ORIGINAL_LENGTH = 558689
CODE_OBJECT_PHYSICAL_OFFSET = 0x3AF24

# Original object-1 machine-code offsets, NOT full-file physical offsets.
# Exact full-file authentication is mandatory before using the mapping.
WINDOWS = (
    (0x37607, "a3 f2 05 01 00"),          # MOV [105F2], EAX
    (0x3760C, "66 89 15 f6 05 01 00"),    # MOV WORD [105F6], DX
    (0x3761E, "a0 62 26 01 00"),          # MOV AL, [12662]
    (0x39940, "8d 7b 20 3b 73 24"),       # LEA EDI,[EBX+20] / CMP ESI,[EBX+24]
    (0x3999E, "8b 57 04"),                # MOV EDX,[EDI+4]
    (0x39A39, "8b 43 24 8b 00 8b 13"),    # MOV EAX,[EBX+24]; MOV EAX,[EAX]; MOV EDX,[EBX]
    (0x39A40, "83 c0 2c 39 d0"),          # ADD EAX,2C; CMP EAX,EDX
)


def bounded(source: str, start: str, end: str) -> str:
    if source.count(start) != 1:
        raise ValueError("missing/ambiguous CODE.2 source start " + start)
    offset = source.index(start)
    finish = source.find(end, offset + len(start))
    if finish < 0:
        raise ValueError("missing CODE.2 function end " + end)
    return source[offset:finish]


def require(body: str, items: tuple[str, ...], name: str) -> None:
    for item in items:
        if item not in body:
            raise ValueError(name + " source missing " + item)


def verify_source(source: str) -> None:
    vector = bounded(
        source,
        "[[nodiscard]] u32 function_375fe(",
        "static_assert(0x37642u-0x375FEu==68u);",
    )
    require(vector, (
        "function_375fe(Machine& m,Runtime126Boundary& b,",
        "store_le32(dg(m,0x105F2u),old_offset);",
        "store_le16(dg(m,0x105F6u),old_selector);",
        "if(extender_mode>=2u && extender_mode<=6u)",
    ), "375FE interrupt vector")
    if "(void)old_offset" in vector or "(void)old_selector" in vector:
        raise ValueError("375FE lost original saved handler ownership")

    free = bounded(
        source,
        "void function_398e0(Machine& m,u32 arena,u32 payload)",
        "static_assert(0x399EBu-0x398E0u==267u);",
    )
    require(free, (
        "const u32 sentinel=arena+0x20u;",
        "u32 at=load_le32(dg(m,arena+0x24u));",
        "while(at!=sentinel && at<block)",
        "at=load_le32(dg(m,at+8u));",
        "u32 prev=load_le32(dg(m,at+4u));",
    ), "398E0 original ordered free-list successor")
    if "if(block>=at)at=arena+0x20u;" in free:
        raise ValueError("398E0 reverted to the broken rover successor")

    cleanup = bounded(
        source,
        "void function_39a2c(Machine& m,Runtime141Boundary& b)",
        "static_assert(0x39A8Au-0x39A2Cu==94u);",
    )
    require(cleanup, (
        "const u32 descriptor=load_le32(dg(m,node+0x24u));",
        "if(load_le32(dg(m,descriptor))+0x2Cu!=load_le32(dg(m,node)))",
    ), "39A2C descriptor compare")
    if "0x2Cu!=node)" in cleanup:
        raise ValueError("39A2C incorrectly compares with node address")


def verify_original(original: bytes) -> None:
    if len(original) != ORIGINAL_LENGTH or hashlib.sha256(original).hexdigest() != ORIGINAL_SHA256:
        raise ValueError("CODE.2 original full-file SHA-256/length mismatch")
    for logical, seq in WINDOWS:
        expected = bytes.fromhex(seq)
        physical = CODE_OBJECT_PHYSICAL_OFFSET + logical
        if original[physical:physical + len(expected)] != expected:
            raise ValueError(f"CODE.2 original x86 mismatch at logical {logical:05X}")


def verify_mutation_rejection(source: str) -> None:
    mutations = (
        ("store_le16(dg(m,0x105F6u),old_selector);", "(void)old_selector;"),
        ("while(at!=sentinel && at<block)", "while(at<block)"),
        ("0x2Cu!=load_le32(dg(m,node)))", "0x2Cu!=node)"),
    )
    for good, wrong in mutations:
        if source.count(good) != 1:
            raise ValueError("regression mutation target missing/ambiguous: " + good)
        corrupted = source.replace(good, wrong, 1)
        try:
            verify_source(corrupted)
        except ValueError:
            continue
        raise ValueError("CODE.2 source guard missed a known erroneous variant")


def verify_successors() -> None:
    # Independent *model*, not a native runtime test. The canonical walk
    # assumes address-ordered circular lists with the sentinel at the end.
    sentinel = 0x10020
    items = (0x10100, 0x10300, 0x10500)
    for block, expected in (
        (0x10080, 0x10100),
        (0x10200, 0x10300),
        (0x10400, 0x10500),
        (0x10600, sentinel),
    ):
        successor = next((x for x in items if x >= block), sentinel)
        if successor != expected:
            raise ValueError("source-ordered successor case failed")


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--source", type=Path, default=ROOT / "Code2" / "Code2.cpp")
    parser.add_argument("--original", type=Path, help="private authentic CODE.2, never uploaded to CI")
    args = parser.parse_args()
    try:
        source = args.source.read_text(encoding="utf-8")
        verify_source(source)
        verify_mutation_rejection(source)
        verify_successors()
        if args.original:
            verify_original(args.original.read_bytes())
    except (OSError, ValueError) as exc:
        parser.exit(1, f"CODE2_RUNTIME_SYNC_REJECTED: {exc}\n")
    print("CODE2_RUNTIME_SOURCE_SYNC_PASS" + (" ORIGINAL_X86_PASS" if args.original else ""))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
