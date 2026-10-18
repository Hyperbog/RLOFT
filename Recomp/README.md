# Ravenloft CODE.1/CODE.2 — recompilation workstream

**Owning branch:** `recomp`. Original-source recompilation, DOS/4GW compatibility, native link/startup experiments and independent original-image tests. **Not** the canonical enhanced Windows game: that is [`demo-v1`](https://github.com/Hyperbog/RLOFT/tree/demo-v1/DemoV1).

## Build and test (Windows x64 / MSVC)

```powershell
cmake -S Recomp -B build/recomp -A x64
cmake --build build/recomp --config Debug --parallel 2
ctest --test-dir build/recomp -C Debug --output-on-failure
```

**Linux GCC/Clang:** run `bash dev/linux.sh test` from the repository root; [Linux-first workstream setup](../dev/README.md) covers dependencies. [Verified Linux C++ run #39082582548](https://github.com/Hyperbog/RLOFT/actions/runs/39082582548) passed **63/63 CTests** on its recorded revision.

**Windows milestone builds:** [schedule, manual selection, artifact downloads and limits](WINDOWS_MILESTONE_BUILDS.md). [First R04 probe #39082555268](https://github.com/Hyperbog/RLOFT/actions/runs/39082555268) compiled and passed **63/63 MSVC CTests** while R04 remains unaccepted. Artifacts are engineering tests, not playable-game releases.

The owning [.github/workflows/recomp-source-link-gate.yml](../.github/workflows/recomp-source-link-gate.yml) is scoped to `recomp` and packages **engineering executables, not a playable game**. The reconstructed `Code1/` and `Code2/` directories remain at their established paths because these are compile inputs. [Workstream/branch guide](BRANCH_GUIDE.md) documents ownership.

## Acceptance boundary

Historical research reached R01–R03 of 12 planned gates, with R04 DOS/DPMI memory/startup investigation incomplete; the older milestone count is **not a current measure of enhanced-game completeness**. The last historical README claimed Windows source run #38325531978 with 63/63 CTests at its stated revision. Retest current HEAD before updating any verification claim. Neither a successful C++ linker nor bounded source entry tests proves original DOS boot, complete original-machine equivalence or player gameplay.

## Evidence and historical notes

The previous extensive milestone log remains in [README_HISTORY_20261017.md](README_HISTORY_20261017.md), in its original directory to preserve relative links to source-address audits. Original-file identity checks, DOS memory selectors and initializers must remain independently attributable to CODE.1/CODE.2, with no proprietary originals in build artifacts.

[Current GitHub branch/CI observation](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/docs/LIVE_STATUS.md) is automated, but does not establish recompilation semantic acceptance.

## Graphics recompilation — original CODE.2 VGA

[Graphics source map, binary SHA witnesses, and remaining integration boundaries](../Code2/graphics/ORIGINAL_VGA_SOURCE_MAP.md) document the original 0x29953 mode setup, 0x2AB38 planar pixel address, 0x29CA0 DAC6 stream and 0x2ABF4 CRTC page start. The separate [portable host component](src/original_vga_host.hpp) and [CTest](src/test_original_vga_host.cpp) implement and test four 64 KiB video planes, palette bytes, latches, GC read and write modes 0–3, bit-mask/rotate/AND/OR/XOR and synthetic indexed scanout. The original CODE.2 image was independently authenticated for the six cited instruction ranges, and the proof helper is shared exactly with `code1-decompilation`.

**Important:** the source graphics calls are not yet hooked to the host adapter, original VGA timing/CRTC modes are incomplete and no real game frame has been produced by recompilation. This is a graphics *hardware-layer regression milestone*, not a playable graphics release.

## Original Ravenloft RES image rendering (verified)

[Actual RES0/RES1 type-1 graphics source integration](../Code2/graphics/REAL_RES_TYPE1_SOURCE_RENDER.md) now decodes owner-owned RES bytes via a bounded read-only adapter and calls **original reconstructed CODE.2 `2A20C` image packing, `2F29C` cached item lookup, `2975C` resource drawing and `2A380` VGA output**. Authentic `RES0:15` and `RES1:336` rendered on a Windows CI runner with original `COLORS` bank 0, with original source hashes checked, and 68/68 CTests passed. [Windows milestone #39088036213](https://github.com/Hyperbog/RLOFT/actions/runs/39088036213) provides the current engineering executable. Original resources and their decoded frames are never bundled in CI downloads. Full DOS resource file opens, composed 3D/gameplay rendering, and visual parity remain open.

## Unloaded original CODE.2 resource-loader milestone

[Original RES loader source and evidence](../Code2/graphics/REAL_RES_TYPE1_SOURCE_RENDER.md): the original reconstructed `2975C` drawing request now invokes **`2F29C` → `2ECB4` → `2EB60` → `2FD9C`** through read-only archive seek/read/position callbacks. Unlike the previous diagnostic, no already-loaded guest image is installed; CODE.2 populates its own image cache, and repeated lookups do not re-read the archive. The raw type-1 raster loader follows the original decoded byte-stream path; compressed type-1 method 3 invokes the corrected canonical CODE.2 `37700` via original `2FB04`/`2FA30`. [Linux #39089790975](https://github.com/Hyperbog/RLOFT/actions/runs/39089790975), [Windows #39089791043](https://github.com/Hyperbog/RLOFT/actions/runs/39089791043) passed **68/68 CTests**. On preserved authentic RES0:15 and RES1:336, indexed SHA outputs matched the preloaded path. This does **not** certify DOS startup, complete world rendering or acceptance gate R04.

## CODE.2 37700, source resource initialization and RES4 milestone

[Original source and authentic RES4 report](../Code2/graphics/RES4_FIRST_PERSON_SOURCE.md): recovered instruction-derived `37700` live guest `2B6C2` termination is used in the compiled CODE.2 compressed-data pipeline with original `2F9E4` byte reads and `2FA30` output. The source `2E7F0` archive initializer now allocates unloaded guest records through read-only DOS-open callbacks (isolated graphics-subsystem startup only, **not** game entry-point boot). RES4 type-6 resources now use source `2ECB4` dispatch and **compiled `3395C` + `338C8`** to render 128-texel texture columns with shade subtraction into indexed VGA frames. Type-6 uses compiled original `37590`, and source `335DC`/`3263C` now execute on synthetic wall-sweep records with real RES4 texture bytes.

[Linux GCC #39091993852](https://github.com/Hyperbog/RLOFT/actions/runs/39091993852) and [Windows/MSVC #39092086421](https://github.com/Hyperbog/RLOFT/actions/runs/39092086421) passed **70/70 CTests**. Windows additionally authenticated real RES4 entries 0, 100 and 500 (each 8,192 visible source-index pixels) and preserved previous RES0/RES1 frame hashes; no original art was shipped. Test output is **a flat first-person texture-column diagnostic**, not a real level view, 3D camera, full interface or playable game. R04 remains open.

Usage on owner-supplied originals:

```powershell
.\rloft_original_code2_graphics_frame.exe --res4 "C:\Games\RLOFT\RES4" 100 128 0 "C:\Temp\RES4-preview" "C:\Games\RLOFT\COLORS" 0 0
```

Creates owner-local `.idx` and `.bmp` graphics; do not redistribute extracted original media.

## Canonical compressed resources, INIT/INFO and 11-archive source startup

[Windows/MSVC milestone #39096508171](https://github.com/Hyperbog/RLOFT/actions/runs/39096508171) **passed 71/71 CTests** and verified authentic source `INIT` header, `INFO.DAT` path flags, and all 11 `RES0..RES10` archived item tables (installed + CD roots). Corrected canonical `37700`, compiled `37590`, and original `335DC` → `3263C` → `338C8` compositor produce identical indexed frames to the direct sampler for RES4:0/100/500/600. Source `334C4`/`32954` camera and level-driven occlusion are **not** connected; neither is continuous CODE.1-to-CODE.2 game boot. R04 remains **unaccepted**. [Technical report](../Code2/graphics/CANONICAL_STARTUP_AND_WALL_SWEEP_20261018.md).

## Real LEVELS.DAT into original CODE.2 world view preparation

The [source-level 334C4 grid integration](../Code2/graphics/ORIGINAL_LEVEL_VIEW_PREP_20261018.md) now transfers all 32 authentic LEVELS.DAT headers and 115,584 original 16-bit packed cells into their **original CODE.2 guest locations**, runs **compiled `334C4`** visibility reset, and stops at `32954` with an explicit unexecuted camera/raycast boundary. The media-free synthetic CTest also supplies marked cells and verifies the reset preserves every flag except `0x0040`. [Windows 72/72 milestone #39097776940](https://github.com/Hyperbog/RLOFT/actions/runs/39097776940) additionally validates original-level SHA and all 32 source-initialization results. This is not CODE.1 game boot, ray-driven wall generation, world scene parity or R04 acceptance.
