# CODE.2 original VGA graphics source map and first host test boundary

**Date:** 2026-10-17. **Scope:** binary-verified graphics hardware protocol plus first reusable, synthetic-testable native interface. **Not:** a complete original game renderer, exact DOS frame reconstruction, native gameplay or pixel acceptance.

## Primary original evidence

Owner-supplied original `CODE.2`: **558,689 bytes**, SHA-256 `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`. LE object 1 begins at physical file offset **`0x3AF24`**. Each listed span is **object-1-relative** and half-open; the SHA-256 is over original physical bytes at `0x3AF24 + [start,end)`, verified directly against owner-supplied `CODE.2`. Do not commit binary excerpts or full DosDis JSON to this repository.

| Original CODE.2 object-1 span | Physical-byte SHA-256 | Observed source behavior |
| --- | --- | --- |
| `0x29953..0x29A84` | `b33f63404acdb74fc53f88adddc841612bd792fe2627a765bda8bec108ef78f0` | Mode 13h BIOS check/set; sequencer `0x3C4`, controller `0x3CE`, CRTC `0x3D4` programming. Guest viewport bookkeeping includes `0x50` byte/plane row, 320 columns and 200/400 logical-height mode variation |
| `0x2AB38..0x2AB6C` | `51b0f554019e0dcca8236ff9894358b7cea837c0f21defb69f5eeef067e623dd` | `2AB38`: EBX = framebuffer_base + `(y<<4) + (y<<6) + (x>>2)` = base + `80*y+x/4`; ECX = `x&3` |
| `0x2AB6C..0x2AB94` | `c4073ea577aef5d2957d1e0864ee096eb7116e8a1e2b13d5db01c4816507186a` | `2AB6C`: selects one of the guest framebuffer bases; base segment addressing and display-CRTC start are **different operations** |
| `0x29AA4..0x29AF0` | `20fb8d840fa06b2ace3cac28eb856c5d3dec9e079351cfc2fd6cf19b6dab0ec2` | Original indexed pixel primitive calculates planar address; writes sequencer map mask via `OUT 0x3C4,AX`, sets GC3 logical mode, then VGA memory |
| `0x29CA0..0x29CE6` | `54243cf9b97b5dbe2b9e75b30b93a3d53ee3ff9c9242fb49c9d042f05074020f` | `29CA0`: palette start index `OUT 0x3C8,AL`, then variable-length RGB DAC stream `OUTSB 0x3C9`, 192-byte maximum per retrace chunk; length is stream bytes, not palette entry count |
| `0x2ABF4..0x2AC10` | `ace61f68b80924da7c40f774c3b95f311b9ed16fd00bdaf7771441099617c7a5` | `2ABF4`: display CRTC start address registers `0x0C` and `0x0D`; calls `2ABC4` to wait for retrace after programming |

**Additional source to integrate next:** `2AC10` updates CRTC line-compare registers for split/bank presentation; `2ABC4` polls `0x3DA` bit 3 until retrace transition. `32954..32F4B` computes original ray/traversal and wall emission; `338C8` and nearby routines sample wall texture columns, including VGA map-mask writes. Automap `0139C` and draw helpers `00B6C`/`00CB0` position map imagery; `2975C` wraps resource lookup `2F29C` with draw `2A380`. See the existing original-binary correction at [post-certification graphics corrections](../POST_CERTIFICATION_GRAPHICS_CORRECTIONS_2026-10-07.md) and [original CODE.2 textured VGA probe](https://github.com/Hyperbog/RLOFT/blob/native-engine/NativeEngine/ALPHA_CODE2_QUADRANT_VGA_WINDOWS_ACCEPTANCE_20261012.md). Those native-engine tests used partially provisional camera/background and were not original DOS screenshot comparisons.

## Implemented in this graphics pass

- **Shared reconstructive math:** [`Code2/graphics/original_vga_math.hpp`](original_vga_math.hpp) models the exact original four-plane address and full 32-bit guest linear addresses (host VRAM plane access maps to a 16-bit aperture), with native-only RGB6→RGB8 display conversion. This pure helper is copied with the same Git blob into both `recomp` and `code1-decompilation`; no original media is embedded.
- **Recomp-only hardware test double:** [`Recomp/src/original_vga_host.hpp`](https://github.com/Hyperbog/RLOFT/blob/recomp/Recomp/src/original_vga_host.hpp) implements four 64-KiB byte planes, sequencer register-2 plane map masks, graphics-controller set/reset/rotate/bit-mask/read-plane registers and latches, hardware write modes 0–3 (including AND/OR/XOR with prior latch contents), indexed VGA DAC `0x3C8/0x3C9`, and CRTC start address registers `0x0C/0x0D`. Output is **indexed** until palette lookup; raw palette bytes are preserved as **6-bit VGA values**.
- **Portable regression:** [`Recomp/src/test_original_vga_host.cpp`](https://github.com/Hyperbog/RLOFT/blob/recomp/Recomp/src/test_original_vga_host.cpp) checks address/plane boundaries and 32-bit original linear addresses plus 16-bit host aperture mapping, read/write plane isolation, read-before-write latches, all four VGA write modes, rotate, bit masks, logical XOR and set/reset, 6-bit DAC stream/auto-increment, synthetic 320×400 scanout, CRTC start page displacement, unimplemented-register rejection and graphics bounds. Added to `Recomp/CMakeLists.txt` as `original_code2_planar_vga_dac6_crtc_host_boundary`.

## Explicitly missing and blocked

The host test double now implements **VGA latches and write modes 0–3** for the supported planar CPU aperture, tested against general VGA register rules (see [OSDev VGA Hardware](https://wiki.osdev.org/VGA_Hardware) and [QEMU's VGA model](https://github.com/qemu/qemu/blob/master/hw/display/vga.c)). It does **not** yet implement VGA read mode 1, chained/odd-even addressing, attribute-controller registers, true CRTC scan/double-scan and line-compare semantics, hardware retrace/IRQ, original dynamic palette tables, RES1/RES3/RES4 typed sprite/display-list calls, SINTAB compositor, CODE.1 graphics equivalence, or routing actual compiled CODE.2 video calls into the adapter.

**Correctness hazard:** the host now models the four VGA write modes, bit-masks, latch reads and ALU operations, but is **not** a complete VGA adapter. Unsupported IO register combinations explicitly return `false`; do not route original graphics that depend on chain-4, read-mode-1, register-6 memory maps, CRTC split-screen or vblank IRQs without implementing those modes. The 320×400 test surface is raw indexed VRAM, not a verified physical output for all original 200/400-line configurations. No BIOS/port/memory traffic from running compiled CODE.2 has yet been connected to the adapter.

Original x86 raster `29AA4` explicitly reads `[ebx]` before writing the same byte, thus loading the four plane latches. The new regression exercises mode-0 direct, XOR, rotate and bit-mask writes; mode-1 latch copying; mode-2 CPU bit expansion; and mode-3 set/reset masking. The VGA hardware semantics are also checked against independently documented hardware behavior; the tests still use **synthetic contents**, not original frames.

## Next integration sequence

1. Validate the new latches/modes 0–3 against the actual compiled `29AA4`, `29B4C`, `27412` and `338C8` instruction/port sequences. Add mode-specific hardware traces and independent original DOS reference inputs; test unsupported chain-4 and read-mode-1 paths fail closed.
2. Emulate complete `29953` VGA mode initializers, page selection, CRTC compare/double-scan, and retrace state transitions. Test before removing any source fallback.
3. Bridge the actual compiled CODE.2 `IN/OUT` operations and A0000 guest-memory accesses into that host, recording function/port traces without invented successful DOS responses.
4. Feed original `COLORS`/RES4/RES3 and texture paths into the compiled renderer; produce indexed-frame SHA/PNG captures, compare with separately captured **real DOS reference frames** before declaring fidelity.
5. Copy new original-byte-verified math/protocol findings to `code1-decompilation`; do not wholesale merge differing engine branches or rewrite original CODE.2 reconstruction based solely on a host simulation.
