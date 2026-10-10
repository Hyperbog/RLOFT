"""Check the original CODE.2 instructions governing F66C inventory ribbon.

No original game data are embedded or copied into the repository. Run locally:
  python3 DemoV1/tools/verify_original_feedback_ribbon_bytes.py /path/to/CODE.2
"""
import hashlib
import sys
from pathlib import Path

SHA256 = "853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314"
WINDOWS = {
    0xF673: "6a01ff74241ce80a0c02008d781089fa89f8c1fa1f2bc2d1f88d7803",
    0xF68F: "b8a000000081e7fcff000029f889c2bb08000000c1fa1f89c7f7fb83c40885d2750383ef04",
    0xF6B4: "8d5704bb5000000089d0c1fa1ff7fb85d2750383ef08",
    0xF6CA: "b838010000bd4001000089fb29f829fd8d770489042439ef7d64",
    0xF6E4: "39fb750b6a026a03576a626a00eb43",
    0xF6F3: "3b1c247d35b95000000089f289f0c1fa1ff7f985d2750b6a026a03536a626a01eb21",
    0xF715: "6a026a03536a62e8811502008d0440c1e80f83c00250eb096a026a03536a626a05",
    0xF736: "e821a0010083c41483c30883c60839eb7c9c",
    0xF748: "6a01ff74241c68400100006a006a65e8ac0d0200",
    0xF7B4: "803dac6d000000740f689c750000e8a5feffffe9ad050000",
    0x30CA2: "e8f5ffffff85c0741869106d4ec64181c239300000891089d0c1e81025ff7f0000c3",
    # 112EC invokes F790 AFTER 4 actor columns and VGA backing.
    0x11316: "6800140000e82cfd010083c40468400100006a106a006a6250a3687f0000e8837f010083c4140fb60502860000500fbf050086000050e83fe4ffff",
    # 30288: RES0 font metrics, signed-byte glyph index, unsigned widths.
    0x30288: "53568b357cb602008b5c240c6a00ff742414e8fdefff",
    0x302A6: "85db741489f1803b00740d0fbe030fb64401044301c2ebee89d089357cb602005e5bc3",
    # 30508: source signed SAR-1 centering; font widths are summed first.
    0x30547: "31d285db74188b0d7cb60200803b00740d0fbe030fb64401044301c2ebee8b44241029d08b54240cd1f8ff74241801c2ff742418891580b60200e842feffff",
    # 30C9C always returns the shared DGROUP RNG address, 0xF6F4.
    # 30CA2's zero-pointer branch is unreachable in this executable.
    0x30C9C: "b8f4f60000c3",
    0x30CC4: "e8d3ffffff85c074068b5424048910c3",
}

def verify(path: Path) -> None:
    data = path.read_bytes()
    assert len(data) == 558689, "CODE.2 size mismatch"
    assert hashlib.sha256(data).hexdigest() == SHA256, "CODE.2 SHA256 mismatch"
    for logical, hex_bytes in WINDOWS.items():
        expected = bytes.fromhex(hex_bytes)
        physical = 0x3AF24 + logical
        actual = data[physical:physical + len(expected)]
        assert actual == expected, f"CODE.2 instruction mismatch at 0x{logical:05X}"
    print(f"PASS: original CODE.2 F66C/F790/30CA2, {len(WINDOWS)} exact instruction windows")

if __name__ == "__main__":
    if len(sys.argv) != 2:
        raise SystemExit("usage: verify_original_feedback_ribbon_bytes.py ORIGINAL_CODE.2")
    verify(Path(sys.argv[1]))
