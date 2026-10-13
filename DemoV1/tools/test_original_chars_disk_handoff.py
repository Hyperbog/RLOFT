#!/usr/bin/env python3
"""Fail closed if the native CODE.1 -> CODE.2 disk handoff is bypassed.

Default: public source-only regression. Optional --original: original CODE.2
full-file identity and exact x86/DGROUP evidence, never shipped in CI.
"""
from __future__ import annotations
import argparse
import hashlib
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
HOST = ROOT / "DemoV1/src/win32_main.cpp"
SHA = "853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314"
WINDOWS = (
    (0x204B3, "6a 00 68 40 11 00 00 e8 59 0d 01 00"),
    (0x204EF, "e8 dc 67 ff ff"),
    (0x31218, "0f b7 44 24 08 50 ff 74 24 08 e8 12 6c 00 00"),
    (0x16CD1, "68 10 07 00 00 e8 c5 a5 01 00"),
    (0x16CDF, "68 20 09 00 00"),
    (0x16CE4, "68 e0 18 02 00"),
    (0x16CEB, "e8 fc a5 01 00"),
    (0x16CF3, "53 e8 e3 a5 01 00"),
    (0x16CFC, "e8 db fd ff ff"),
)

def original(data: bytes) -> None:
    if len(data) != 558689 or hashlib.sha256(data).hexdigest() != SHA:
        raise ValueError("CODE.2 full-file identity mismatch")
    for logical, hex_bytes in WINDOWS:
        raw = bytes.fromhex(hex_bytes)
        physical = 0x3AF24 + logical
        if data[physical:physical+len(raw)] != raw:
            raise ValueError(f"CODE.2 original x86 mismatch at {logical:05X}")
    for address in (0x710, 0x1140):
        if data[0x75F24+address:0x75F24+address+6] != b"CHARS\x00":
            raise ValueError(f"CODE.2 original CHARS filename missing at {address:04X}")

def source(text: str) -> None:
    mark = "if (character_flow.completed()) {"
    if text.count(mark) != 1:
        raise ValueError("native completion controller absent or duplicated")
    region = text[text.index(mark):][:12000]
    gates = (
        "const auto image =",
        "if (options.front_end_only) {",
        "runtime_output_path(",
        "out.write(",
        "flow.finish_character_creation(",
        "flow.supervisor_dispatch();",
        "const auto code2_chars =",
        'assets.source().read_runtime("CHARS");',
        "code2_chars->size() != image.size()",
        "std::equal(",
        "flow.finish_code2_startup(true, false);",
        "return run_exploration(",
        "*code2_chars,",
    )
    positions = [region.find(token) for token in gates]
    if any(p < 0 for p in positions) or positions != sorted(positions):
        raise ValueError("CODE.1 -> CODE.2 write/supervisor/read/load order changed: " +
                         str([(t,p) for t,p in zip(gates, positions) if p < 0]))
    if region.count('assets.source().read_runtime("CHARS");') != 1:
        raise ValueError("CODE.2 must read writable CHARS exactly once on handoff")
    if "std::vector<std::uint8_t>(\n                                        image.begin()," in region:
        raise ValueError("CODE.2 still receives an in-memory CODE.1 party image")
    if "return 0;" not in region[positions[1]:positions[2]]:
        raise ValueError("read-only front-end-only still reaches the disk write")

def negative_tests(text: str) -> None:
    variants = (
        ('assets.source().read_runtime("CHARS");', "std::nullopt;"),
        ("*code2_chars,", "std::vector<std::uint8_t>(image.begin(),image.end()),"),
        ("code2_chars->size() != image.size()", "false"),
    )
    for good, bad in variants:
        if text.count(good) < 1:
            raise ValueError("mutation target absent: " + good)
        try:
            source(text.replace(good, bad, 1))
        except ValueError:
            continue
        raise ValueError("source verifier failed to reject a known-bad mutation")

def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--host", type=Path, default=HOST)
    parser.add_argument("--original", type=Path)
    opts = parser.parse_args()
    try:
        text = opts.host.read_text(encoding="utf-8")
        source(text)
        negative_tests(text)
        if opts.original:
            original(opts.original.read_bytes())
    except (OSError, ValueError) as exc:
        parser.exit(1, "CHARS_HANDOFF_REJECTED: " + str(exc) + "\n")
    print("CHARS_DISK_HANDOFF_SOURCE_PASS" +
          (" ORIGINAL_CODE2_PASS" if opts.original else ""))
    return 0

if __name__ == "__main__":
    raise SystemExit(main())
