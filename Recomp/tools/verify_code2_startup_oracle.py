#!/usr/bin/env python3
"""Fail-closed original CODE.2 startup binary oracle; no original bytes embedded."""
from __future__ import annotations
import argparse
import hashlib
from pathlib import Path
import struct

SIZE = 558689
SHA256 = "853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314"
CODE_START = 0x3AF24
DATA_START = 0x75F24
SPANS = {
    (0x353A0, 0x353A2): "326483fa1d1597f096d675f0ac348108412053844fdb555a3f36c2d41c83d772",
    (0x35418, 0x3562F): "bde266affb92d4e977a8ffa8c405642e28dbc80430321181264abcb9e424c01b",
    (0x395E2, 0x3962D): "4372327a1ecb72c421622e01ebc4cab18320202d60e5afc3a8aef6c39268e410",
    (0x3A16E, 0x3A176): "74746f375879b05d7aadc4187242c99b7d25fb421f1fc0838ecd09bafb068f5c",
    (0x3A2AD, 0x3A38A): "02856358bd8ddcf0da443beeaf6075fc791d33dad995e97b6cee847084cc26f6",
    (0x3A1B3, 0x3A2AD): "295974dbdf1b3a0994b765cbe78bf2269747ef5a65b7931a0cd5b5eafdc8b6e8",
    (0x3968E, 0x39752): "405b8d6871e820a7d8191df7f56818f68d3e2e9acc0a724f81c857fba0634c74",
    (0x39752, 0x39837): "65a82679dd6bd7fba5d3bb3c38183732817da3a0d5e924af7de8409da0d1617e",
    (0x37B42, 0x37C37): "f5e4e045387d36c2d8615cd13d6f9e11bfea4c14c60672607e5b9a13446d4737",
    (0x39589, 0x395E2): "4d01c84b06650061922e0f5406c5c18225ab3984583f9a96aba138a7a8cef038",
    (0x3A38A, 0x3A395): "7e62ec735424ad532845ef46794017a8ea470bb8b115dd89020ef44624cdbf51",
    (0x39838, 0x398DB): "e296f5ba2bc6f038404bcf31e0e6d87e07284883ec293f9e1b04d083d627a9c5",
    (0x398E0, 0x399E6): "5c5a84c8a78495f92bba9cafe6754f6f68fe8ed90c33c0f86944141ea1ea2460",
}
EXPECTED_INITIALIZERS = (0x3968E, 0x3A1B3, 0x3A2AD, 0x3A16E)


def verify(image: bytes) -> dict[str, int]:
    if len(image) != SIZE or hashlib.sha256(image).hexdigest() != SHA256:
        raise ValueError("CODE.2 is not the authenticated original executable")
    for (start, end), expected in SPANS.items():
        actual = hashlib.sha256(image[CODE_START+start:CODE_START+end]).hexdigest()
        if actual != expected:
            raise ValueError(f"original instruction span {start:05X}..{end:05X} differs")
    # Source instructions: JMP 35418; EAX=FF, E8 to 395E2; EDX=EAX;
    # <= priority selection (JA, not JAE); MOV word[126EC],SS.
    windows = {
        0x353A0: bytes.fromhex("eb76"),
        0x3561E: bytes.fromhex("b8ff000000"),
        0x35623: bytes.fromhex("e8ba3f0000"),
        0x395E7: bytes.fromhex("89c2"),
        0x39602: bytes.fromhex("3846017705"),
        0x39607: bytes.fromhex("89f3"),
        0x3A16E: bytes.fromhex("668c15ec260100"),
        0x37BC2: bytes.fromhex("89f8 89f3 e86d1c0000"),
        0x39842: bytes.fromhex("83c007"),
        0x398CE: bytes.fromhex("830e01"),
        0x398E0: bytes.fromhex("5657511e8eda"),
    }
    for offset, expected in windows.items():
        if image[CODE_START+offset:CODE_START+offset+len(expected)] != expected:
            raise ValueError(f"original opcode differs at {offset:05X}")
    observed = []
    for i in range(4):
        flags, priority, target = struct.unpack_from("<BBI", image, DATA_START+0x12720+i*6)
        if flags != 0 or priority != 0x20 or target != EXPECTED_INITIALIZERS[i]:
            raise ValueError(f"original initializer table entry {i} differs")
        observed.append((priority, -i, target))
    # Five 26-byte stream descriptors are loaded from the original data object.
    # The sixth record terminates source function_3a1b3's initialization loop.
    streams = tuple(struct.unpack_from("<I", image, DATA_START + 0xF6FC + i*0x1A + 0x0C)[0]
                    for i in range(6))
    if streams != (1, 2, 2, 3, 2, 0):
        raise ValueError(f"original initialized stream descriptor table changed: {streams!r}")
    # Arena and environment argv outputs are runtime state, originally zero.
    if image[DATA_START+0x12688:DATA_START+0x12690] != bytes(8):
        raise ValueError("original CODE.2 environment pointer table is not initially zero")
    winner = min(observed)[2]  # Later record replaces earlier equal priority.
    if winner != 0x3A16E:
        raise ValueError("first initializer violates original equal-priority tie rule")
    return {"verified_spans": len(SPANS), "verified_opcodes": len(windows),
            "callback_table_entries": len(observed), "first_initializer": winner,
            "next_unresolved_initializer": 0x3A2AD,
            "verified_original_stream_descriptors": len(streams) - 1}


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("original_code2", type=Path)
    args = parser.parse_args()
    try:
        result = verify(args.original_code2.read_bytes())
    except (ValueError, OSError) as exc:
        parser.exit(1, f"ORIGINAL_CODE2_STARTUP_REJECTED: {exc}\n")
    print("ORIGINAL_CODE2_STARTUP_ORACLE=PASS")
    for key, value in result.items():
        print(f"{key}={value:#x}" if key.endswith("initializer") else f"{key}={value}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
