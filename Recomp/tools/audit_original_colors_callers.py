#!/usr/bin/env python3
"""Bounded original CODE.2 palette-loader call-site audit.

Confirms the 23 direct near CALLs to original object-1 0x325B0 and
identifies the full-bank 0x100 argument push at each site. This is a
STATIC DIRECT-CALL inventory, NOT a live runtime/DAC trace or a proof
that indirect calls, palette copies and rotations do not occur.

Use only an owner-owned, SHA-authenticated original CODE.2. No original
machine-code bytes are emitted or committed; output contains addresses,
hashes and bounded static classifications only.
"""

import argparse
import hashlib
import json
import struct
from pathlib import Path

ORIGINAL_SHA256 = "853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314"
OBJECT1_PHYSICAL_OFFSET = 0x3AF24
LOADER_ENTRY = 0x325B0
LOADER_END = 0x3261C
LOADER_SHA256 = "176ec8713ca3d022e11e6380294f56d8f1fe77e54864a562f3fbf05f19a375c8"
FULL_COUNT_PUSH = b"\x68\x00\x01\x00\x00"  # push 0x100
EXPECTED_CALLS = (
    0x05463, 0x059AD, 0x05A48, 0x05E8C, 0x06427, 0x08434,
    0x084F9, 0x0ECA5, 0x0ECE7, 0x13A79, 0x13E5B, 0x1901A,
    0x194F8, 0x1958B, 0x1961A, 0x198F8, 0x19A0A, 0x19B34,
    0x19BC2, 0x19BE8, 0x19C55, 0x1FADB, 0x20510,
)


def sha256(data):
    return hashlib.sha256(data).hexdigest()


def direct_near_calls(object_bytes, target):
    """Candidates with an E8 rel32 whose next EIP equals target.

    Bytes alone cannot certify code-vs-data; the known 23 sites are
    authenticated against the retail image and require further CFG
    corroboration before concluding exhaustive executable reachability.
    """
    addresses = []
    for offset in range(0, len(object_bytes) - 4):
        if object_bytes[offset] != 0xE8:
            continue
        displacement = struct.unpack_from("<i", object_bytes, offset + 1)[0]
        if offset + 5 + displacement == target:
            addresses.append(offset)
    return addresses


def preceding_full_count_push(object_bytes, call_site):
    """Return the nearest encoded push 256 in a 48-byte caller window.

    Only a bounded *byte-level argument marker*, NOT an instruction CFG
    or proof about the runtime parameter stack.
    """
    start = max(0, call_site - 48)
    return next((offset for offset in range(call_site - 5, start - 1, -1)
                 if object_bytes[offset:offset + 5] == FULL_COUNT_PUSH), None)


def audit(raw):
    if sha256(raw) != ORIGINAL_SHA256:
        raise ValueError("Original CODE.2 SHA256 mismatch; rejecting unverified input")
    obj = memoryview(raw)[OBJECT1_PHYSICAL_OFFSET:]
    if sha256(obj[LOADER_ENTRY:LOADER_END]) != LOADER_SHA256:
        raise ValueError("CODE.2 object-1 325B0 loader byte-window mismatch")

    calls = direct_near_calls(obj, LOADER_ENTRY)
    if tuple(calls) != EXPECTED_CALLS:
        raise ValueError("Original 325B0 direct-call inventory differs from known image")
    rows = []
    for site in calls:
        marker = preceding_full_count_push(obj, site)
        rows.append({
            "object_1_call_site": f"0x{site:05X}",
            "physical_file_call_site": f"0x{OBJECT1_PHYSICAL_OFFSET + site:05X}",
            "nearest_push_256": None if marker is None else f"0x{marker:05X}",
            "count_argument_candidate": "256" if marker is not None else "UNKNOWN",
        })
    if any(row["count_argument_candidate"] != "256" for row in rows):
        raise ValueError("A direct loader caller lacks the expected full-bank marker")

    return {
        "evidence": "AUTHENTICATED_RETAIL_OBJECT1_STATIC_CALL_BYTES",
        "original_code2_sha256": ORIGINAL_SHA256,
        "loader_object_1_entry": f"0x{LOADER_ENTRY:05X}",
        "loader_window_sha256": LOADER_SHA256,
        "direct_call_candidate_count": len(calls),
        "direct_call_sites": rows,
        "finding": "All 23 authenticated direct E8 callers carry an adjacent push 256; none evidences an 8-entry COLORS load.",
        "limits": [
            "Static E8 direct-call candidates, not complete CFG or observed runtime calls",
            "Indirect calls, pointer-based palette copies, DAC writes and palette rotations not excluded",
            "Original palette indices 246..253 caller sequence and DOS/native frame parity NOT VERIFIED",
        ],
    }


def self_test():
    fixture = bytearray(60)
    fixture[2:7] = FULL_COUNT_PUSH
    fixture[35] = 0xE8
    struct.pack_into("<i", fixture, 36, 52 - 40)
    assert direct_near_calls(fixture, 52) == [35]
    assert preceding_full_count_push(fixture, 35) == 2
    fixture[3] = 8
    assert preceding_full_count_push(fixture, 35) is None
    assert direct_near_calls(fixture, 52) == [35]
    try:
        audit(b"not the retail binary")
    except ValueError as exc:
        assert "SHA256 mismatch" in str(exc)
    else:
        raise AssertionError("Unverified original was accepted")
    print("SELF_TEST_PASS direct E8 displacement and full-bank argument marker")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--code2", type=Path, help="Unmodified owner-owned retail CODE.2")
    parser.add_argument("--output", type=Path, help="Private JSON report destination")
    parser.add_argument("--self-test", action="store_true")
    args = parser.parse_args()
    if args.self_test:
        self_test()
    if args.code2 is None:
        if not args.self_test:
            parser.error("Supply --code2 or --self-test")
        return
    result = audit(args.code2.read_bytes())
    summary = {key: result[key] for key in (
        "evidence", "loader_object_1_entry", "direct_call_candidate_count", "finding", "limits")}
    print(json.dumps(summary, indent=2))
    if args.output:
        args.output.parent.mkdir(parents=True, exist_ok=True)
        args.output.write_text(json.dumps(result, indent=2) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
