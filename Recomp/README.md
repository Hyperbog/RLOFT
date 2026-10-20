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

## Authentic SINTAB-driven original CODE.2 Level 22 rays

[Original 03838 SINTAB + 32954 world raycast checkpoint](ORIGINAL_32954_SINTAB_RAYCAST_20261018.md): a compiled original `181B4` loader now feeds authentic Level 22 cells to source `334C4`, with original `03838` loading the preserved SINTAB and authentic `20608` *default startup camera values* replayed (tile 4,4; 270°). Compiled `32954`, original fixed-point helpers and wall/depth record generators executed **256 rays**, materialized **256 wall/depth records** and marked **231 cells** on [successful Windows/MSVC #39100156051](https://github.com/Hyperbog/RLOFT/actions/runs/39100156051) (**72/72 CTests plus real-media validation**). The `20608` entire game entry and original-to-recomp independent DOS parity are **not** proved; the generated records are not yet connected to the genuine RES4 frame compositor, so this is not a rendered playable world or R04 acceptance.

## Original SAVE10 Level 5 deserialization and outdoor renderer checkpoint

[Source-executed original `07938` save load and Level 5 outdoor dispatch](ORIGINAL_07938_SAVE5_OUTDOOR_SOURCE_20261018.md): original CODE.2 initialized DGROUP + `SAVE10/SAVE.DAT` enter compiled `07938/06CAD`, which invokes compiled `181B4` for the standalone mutable `LEV005.DAT`. The same guest world now drives authentic `334C4/32954` rays and compiled `335DC/3263C/338C8` wall composition; see [passing source-world Linux CI](https://github.com/Hyperbog/RLOFT/actions/runs/39135916368). Original compiled `1F1C8` additionally selects **two authentic RES4 textured outdoor backgrounds via `34A74` callbacks**, verified by [dedicated CI](https://github.com/Hyperbog/RLOFT/actions/runs/39136699731). Those callbacks authenticate which textures and branches are used but **do not yet render the sky/ground textured pixels**. `34A74/342E4/34954`, RES3 scenery/trees, UI layers, original DOS pixel comparison, and continuous CODE.1→CODE.2 boot are still open.


## Current source-scene checkpoint (supersedes older historical graphics notes)

[Part 21 original SAVE5 quads, RES3 shape and independent DOS oracle handoff](ORIGINAL_SAVE5_RES3_SCENE_AND_DOS_CAPTURE_20261018.md) records the verified current source execution. Original CODE.2 source now deserializes the saved Level 5 state, renders source sky/ground (RES4:634/635), 256-ray world walls, one source-visible world quad through compiled 34DE8 → 34BB0 → 33E6C, and 12 original RES0 HUD blits. The original NPCS/OBJECTS/INIT descriptor banks were initialized in the same graphics session. Source NPC type 61 resolved authenticated RES3 shape 1829; original 2F29C/37590 decompressed its 4,219-byte EH7 body and passed cache reuse. A compiled 2C294/2B6D0 scene projection gate is connected but yields **zero visible sprite draw requests** at the actual saved Level 5 camera, so **36CEC sprite pixel rendering is still unimplemented**. This is neither the complete outdoors game nor a reference DOS capture. Latest source verification [saved Level 5 #39164949396](https://github.com/Hyperbog/RLOFT/actions/runs/39164949396), [Level 11 world regression #39164949379](https://github.com/Hyperbog/RLOFT/actions/runs/39164949379), [Windows #39164949386](https://github.com/Hyperbog/RLOFT/actions/runs/39164949386), all passed. No original-media pixel files were published.

## Part 24 — original DOSBox-X 2026.10.01 DAC provenance hook (2026-10-20)

The [instrumented DOSBox-X capture kit](tools/dosboxx_dac_capture/README.md) pins
upstream VGA source to Git blob `509d42fbead67a56594e49cec55549bc6fcc6d7c`
and adds opt-in, post-lockout/mask capture at the actual `0x3C8/0x3C9`
VGA byte write handlers, preserving the individual bytes from `REP OUTSB`.
Its JSONL converter rejects unverified upstream hooks, overflow,
truncated DAC histories and original-save palette divergence.

The [dedicated CI](https://github.com/Hyperbog/RLOFT/actions/workflows/recomp-dosboxx-dac-hook.yml)
passed exact release-source patching, compiled standalone C++11 hook
tests, and compiled an isolated harness using the **actual patched
upstream VGA handler bodies**. This is **not** yet a completed Windows
DOSBox-X executable, an original Ravenloft runtime capture or an accepted
native graphics parity check. Keep original-game SAV, port captures,
palette bytes and indexed frames private. The retail eight-entry
`COLORS` bank-3 update caller remains unverified.

## Part 26 — original COLORS bank selection explains DOS DAC entries 246–253

[Authenticated COLORS→CODE.2 scene selector chain](ORIGINAL_DOS_DAC_RUNTIME_BANK3_20261020.md)
now identifies the source of the eight-color change: **original COLORS bank 0
exactly equals gameplay DAC submission 28**, while **COLORS bank 3 equals
submissions 29/30 and independently saved original VGA DAC**. These banks
differ only in 24 RGB6 channels at indices 246–253; no skeleton RES image
patch or guessed palette interpolation is needed to explain the transition.
Source object-1 18C8D → 1F8E0 updates palette selector C034 using the original
Level-5 scene descriptor at 81AC; authenticated source loader 325B0 reads
the selected COLORS bank into C054; wrapper 29234 → 29CA0 emits the DAC
bytes. The owner's SAV independently contains selector C034=3 and an exact
COLORS bank-3 palette at C054. A SHA-pinned
[palette-selection audit](tools/audit_original_colors_scene_palette.py)
passed [original-media Linux CI](https://github.com/Hyperbog/RLOFT/actions/runs/39402847517).
**Source bank identity is verified; exact dynamic producer call ancestry,
emulator-save frame phase and DOS/native indexed pixel parity are not.**

## Part 27 — Native source scene palette now uses full COLORS bank (2026-10-20)

[Native palette integration](SOURCE_SCENE_PALETTE_NATIVE_INTEGRATION_20261020.md)
replaces the prior 246–253 bank-3 splice with **complete 768-byte bank
selection** from original CODE.2 level scene descriptors at object-2
0x81AC + 3*level. For the authentic Level-5 save the source table
selects bank 3. The native palette loader stages the selected complete
original COLORS bank at 0xC054 and runs compiled original CODE.2 32150
brightness and 29CA0 DAC output. There is no copied eight-color overlay
or special case for skeleton sprites.

[Authentic saved-world Linux gate](https://github.com/Hyperbog/RLOFT/actions/runs/39419812617)
**passed** scene-selector and malformed-input unit tests, unchanged RES3
skeleton/RES0 HUD/world compilation, and **full 768-byte** native output
palette equality against owner-original COLORS bank 3. The --world
LEVELS-only diagnostic remains an explicitly named bank0 fallback because
it has no original CODE.2 scene table. Full original boot sequence,
retail dynamic setter call ancestry, animation-phase synchronization and
DOS/native indexed pixel parity remain NOT VERIFIED. This is a native
saved-world renderer integration, not a completed playable game.

## Part 28 — host-ordered original CODE.1 → CODE.2 source boot and COLORS loader

[Implementation and acceptance boundary](ORIGINAL_STAGED_BOOT_AND_PALETTE_20261020.md):
new non-playable `rloft_code1_code2_staged_boot` target executes the
reconstructed CODE.1 CC1C/CD97/FCE8/FC98 constructor/startup checkpoint
(with its original 508C **host stop**) and then, under explicit host
scheduling, enters a CODE.2 session executing original 16D04 INIT,
2E768/2E7F0 INFO/11 resource initialization, authenticated source
scene palette selection, source-derived 325B0 read-only COLORS bank
open/seek/read, compiled original 32150 brightness processing and
compiled original 29CA0 VGA DAC submission (769 real adapter-port calls).
**This is a staged host progression, not a reconstructed DOS guest transfer.**
The same `325B0` source-derived file callbacks also replace host-side
palette copying in the native saved-world renderer. No original media or
save state is needed to initialize the CODE.2 resource/palette session
once owner-original INIT/INFO/RES/CODE.2/COLORS files are provided.

[Passing owner-original Windows regression](https://github.com/Hyperbog/RLOFT/actions/runs/39509429048)
verified the assembled startup chain, its source media identity, full
scene bank-3 VGA submission, and existing source startup/rendering tests.
The Windows engineering milestone ZIP includes the stage runner; it
is **not a playable executable** and takes an extracted read-only CD
resource directory for RES9 and RES10 (not a mounted `game.gog` image).
The unresolved CODE.1 game-entry overlay switch, CODE.2 runtime loop,
original CPU call ancestry and DOS/native frame parity remain open.
