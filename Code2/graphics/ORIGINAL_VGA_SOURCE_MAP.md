# CODE.2 original VGA graphics source map and compiled graphics boundary

**Date:** 2026-10-17/18. **Scope:** binary-verified graphics hardware protocol, source-compiled CODE.2 drawing callbacks, VGA host interface, actual synthetic-input indexed frame generation. **Not:** a complete game renderer using original RES resources, DOS runtime, or verified screenshot parity. See [compiled source execution and frame protocol](COMPILED_GRAPHICS_PATH.md).

The [original type-1 resource/cache/render audit](REAL_RES_TYPE1_SOURCE_RENDER.md) records six additional binary-authenticated CODE.2 function ranges and successful RES0/RES1 image rendering using the genuine game archive bytes. The [source-span verifier](verify_original_vga.py) now covers **32** original CODE.2 windows, including recovered 37700 control, source `2E7F0` resource initialization and compiled RES4 texture sampling ([new report](RES4_FIRST_PERSON_SOURCE.md)). This is not full DOS/game scene parity.

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
| `0x2A380..0x2A478` | `f5d3f72361f32920058ab1527607be381717c93abeac8b0d2e1ee72d0c0fea91` | Original `2A380` planar image/frame drawing, 4 planes, transparent vs opaque source branches; compiled with a host VGA callback and tested using synthetic image records |
| `0x32150..0x3221F` | `298469caa887ee56b8f119c7d38f923f41d1f6c6fec26e475cd73a68303f515f` | Original `32150`: 256-entry RGB6 palette intensity adjustment keyed by the active profile's byte +6Ch |
| `0x325B0..0x3261C` | `176ec8713ca3d022e11e6380294f56d8f1fe77e54864a562f3fbf05f19a375c8` | Original `325B0` file-backed RGB6 bank loader: seeks `1 + 768 * bank`, reads `count*3` bytes, applies `32150`, closes file; still requires DOS file boundary to run end-to-end |

**Additional source to integrate next:** `2AC10` updates CRTC line-compare registers for split/bank presentation; `2ABC4` polls `0x3DA` bit 3 until retrace transition. `32954..32F4B` computes original ray/traversal and wall emission; `338C8` and nearby routines sample wall texture columns, including VGA map-mask writes. Automap `0139C` and draw helpers `00B6C`/`00CB0` position map imagery; `2975C` wraps resource lookup `2F29C` with draw `2A380`. See the existing original-binary correction at [post-certification graphics corrections](../POST_CERTIFICATION_GRAPHICS_CORRECTIONS_2026-10-07.md) and [original CODE.2 textured VGA probe](https://github.com/Hyperbog/RLOFT/blob/native-engine/NativeEngine/ALPHA_CODE2_QUADRANT_VGA_WINDOWS_ACCEPTANCE_20261012.md). Those native-engine tests used partially provisional camera/background and were not original DOS screenshot comparisons.

## Implemented in this graphics pass

- **Shared reconstructive math:** [`Code2/graphics/original_vga_math.hpp`](original_vga_math.hpp) models the exact original four-plane address and full 32-bit guest linear addresses (host VRAM plane access maps to a 16-bit aperture), with native-only RGB6→RGB8 display conversion. This pure helper is copied with the same Git blob into both `recomp` and `code1-decompilation`; no original media is embedded.
- **Recomp-only hardware test double:** [`Recomp/src/original_vga_host.hpp`](https://github.com/Hyperbog/RLOFT/blob/recomp/Recomp/src/original_vga_host.hpp) implements four 64-KiB byte planes, sequencer register-2 plane map masks, graphics-controller set/reset/rotate/bit-mask/read-plane registers and latches, hardware write modes 0–3 (including AND/OR/XOR with prior latch contents), indexed VGA DAC `0x3C8/0x3C9`, and CRTC start address registers `0x0C/0x0D`. Output is **indexed** until palette lookup; raw palette bytes are preserved as **6-bit VGA values**.
- **Portable regression:** [`Recomp/src/test_original_vga_host.cpp`](https://github.com/Hyperbog/RLOFT/blob/recomp/Recomp/src/test_original_vga_host.cpp) checks address/plane boundaries and 32-bit original linear addresses plus 16-bit host aperture mapping, read/write plane isolation, read-before-write latches, all four VGA write modes, rotate, bit masks, logical XOR and set/reset, 6-bit DAC stream/auto-increment, synthetic 320×400 scanout, CRTC start page displacement, unimplemented-register rejection and graphics bounds. Added to `Recomp/CMakeLists.txt` as `original_code2_planar_vga_dac6_crtc_host_boundary`.

## Compiled CODE.2 integration (verified)

The original reconstructed `Code2/Code2.cpp` is **compiled and executed** by the separate [recomp graphics executable](https://github.com/Hyperbog/RLOFT/blob/recomp/Recomp/src/code2_compiled_graphics_frame.cpp). This harness attaches real `Graphics2Boundary` and `Application61Boundary` callbacks for VGA port IN/OUT and A0000 memory access, then invokes the original reconstructed `29930`, `2AB98`, `2ABC4`, `2AB38`, `2A380`, `29AA4`/`29AF0`, `2AB6C`, `2ABF4`, `2AC10`, `29CA0` and `32150` functions.

Covered hardware now includes four video planes, latches and VGA write modes 0–3, deterministic retrace, original 320×200/400 mode selection, host 400-physical-scanline output, CRTC display-page start and line-compare split control. The reconstructed `2A380` draws a synthetic image record; a real indexed host framebuffer (`128000` bytes) and 8-bit Windows BMP are saved in Windows milestone packages.

Original `COLORS` banks (15 × 768-byte RGB6 records, one initial count byte) can be loaded **read-only** by the diagnostic from a user-provided file; the compiled CODE.2 `32150` brightness transform and `29CA0` DAC6 transfer produce the displayed colors. The original CODE.2 file-backed loader `325B0` is traced but not yet executed through the DOS file interface. [Recomp testing code](https://github.com/Hyperbog/RLOFT/blob/recomp/Recomp/tools/test_compiled_graphics_artifact.py) independently validates the bank offset, original brightness arithmetic, BMP header/dimensions/index retention and invalid-data rejection.

**Proven CI:** [Linux GCC #39086139521](https://github.com/Hyperbog/RLOFT/actions/runs/39086139521), [Windows MSVC #39086139502](https://github.com/Hyperbog/RLOFT/actions/runs/39086139502), and [Windows source-link #39086139589](https://github.com/Hyperbog/RLOFT/actions/runs/39086139589) each passed **67/67 CTests** at their recorded source revisions. The [first verified compiled-frame Windows artifact](https://github.com/Hyperbog/RLOFT/actions/runs/39085661346/artifacts/12120363240) contains original-source-created synthetic input `.idx` and `.bmp` images.

## Original unloaded RES image loading, now verified

The [real RES image/source report](REAL_RES_TYPE1_SOURCE_RENDER.md) now establishes actual execution of `2975C` → `2F29C` → `2ECB4` → `2EB60` → `2FD9C` → `2A380`. A bounded read-only host stream implements the original `31334` seek, `312EC` guest buffer read and `3131C` cursor query calls. Source CODE.2 allocates and populates the original nine-byte cache, then uses a cache hit without a second file seek. Windows [milestone #39089791043](https://github.com/Hyperbog/RLOFT/actions/runs/39089791043) passed **68/68 tests** and rendered original `RES0:15` and `RES1:336` with exactly the previously verified indexed-frame hashes. **Original `37700` live guest stop semantics now execute through the corrected source-side recovery** ([report](RES4_FIRST_PERSON_SOURCE.md)); canonical `Code2.cpp` has replaced the fixed stop argument with the live guest stop-controlled decoder. Source `2E7F0` initializes archive slots, and original RES4 type-6 column sampling reaches compiled `3395C`/`338C8`. Compiled original `37590` and `335DC`/`3263C` wall compositor tests now pass. Continuous CODE.1 game boot and camera-driven 3D scene composition remain open.

## Explicitly missing and blocked

- **Real game scene input:** no original RES1/RES3/RES4 sprites, textures or interface records are yet supplied to `2A380`, and the full scene and HUD compositor has not executed.
- **Full DOS/VGA integration:** guest runtime/interrupts, VGA memory-map and chain modes beyond the tested unchained path, exact CRTC physical-timing and display scan, read mode 1, attribute-controller behavior and hardware vblank are incomplete. Host `03DAh` returns **deterministic simulated retrace**, not a real device signal.
- **Source file loading:** optional user-owned COLORS uses the documented bank schema and original `32150`/`29CA0` processing, but the original `325B0` DOS file-open/seek/read/close chain is not yet wired to the host.
- **Pixel parity:** no independent DOSBox/real DOS frame reference, matching original gameplay camera or full R04 runtime acceptance. Unit-test success is neither DOS equivalence nor a playable game.

The original x86 raster `29AA4` performs a latch read before writing `[EBX]`, and the host handles it through the reconstructed callback. The host deliberately reports unsupported port accesses; additional VGA modes must be evidence-backed rather than silently accepted.

## Next graphics work

1. Load an original resource image record from user-owned RES archives through the proven extraction pipeline; execute original `2A380`/related planar render code with **real** palette, pixels and record dimensions. Keep copyright media private and supply only source-independent test hashes publicly.
2. Route the compiled CODE.2 resource manager and graphics callbacks from the actual runtime/main-loop path through the same host, rather than calling specific functions from a diagnostic harness.
3. Integrate remaining original compositor/render paths, CRTC scan timing and attribute-controller semantics needed by authentic game scenes; add test traces for page/split transitions.
4. Capture real DOS reference scenes for identical original resource files and compare indexed buffer, palette and final bitmap. Do not mark fidelity complete without those comparisons.
5. Synchronize only original-byte-backed findings and portable geometry/protocol helpers into `code1-decompilation`. Keep the runnable recomp graphics host owned by `recomp`.
