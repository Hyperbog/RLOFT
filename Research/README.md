# Decompilation — CODE.1 and CODE.2

**Active branch:** [`Decomp`](https://github.com/Hyperbog/RLOFT/tree/Decomp) (repository default). **Scope:** original LE DOS/4GW reverse engineering and human-readable C++ reconstruction. **Not:** the recompiled DOS execution host or the native Windows game.

## Existing source paths (intentionally retained)

| Component | Location | Description |
| --- | --- | --- |
| Reconstructed CODE.1 | [`Code1/Code1.cpp`](../Code1/Code1.cpp) | Original CODE.1 C++ reconstruction and progress/evidence ledger |
| Reconstructed CODE.2 | [`Code2/Code2.cpp`](../Code2/Code2.cpp) | Original CODE.2 C++ reconstruction and progress/evidence ledger |
| Decompilation tools & proofs | [`Code2/tools/`](../Code2/tools/) and [`ResearchInfra/`](../ResearchInfra/) | Byte witnesses, DOSDis comparison and static indexing |
| Common source analysis | [`SharedDecomp/`](../SharedDecomp/) | Shared algorithm/format checks and tests |
| Data and resources | [`DatFiles/`](../DatFiles/), [`Resources/`](../Resources/), [`Runtime/`](../Runtime/) | Source-format and runtime research |
| Project coordination | [`docs/`](../docs/) | Branch ownership, accepted source identifiers, cross-branch evidence |

**Why `Code1/` and `Code2/` were not moved:** existing analysis scripts, include directives, source references and the separate recompilation build all depend on these exact relative paths. A cosmetic mass move would break the tools and historic source links. This `Decomp/` directory is the dedicated entry point for decompilation; it can receive new decomp-only tools/docs while the established source remains intact.

## Parallel recompilation and game branches

- [`Recomp` → `Recomp/`](https://github.com/Hyperbog/RLOFT/tree/Recomp/Recomp): source recompilation, original runtime shims, linkage and Windows source tests.
- The earlier `demo-v1` workstream is historical; its branch no longer exists in the current branch list.
- [`DnD-Classics` → `MASTER/`](https://github.com/Hyperbog/RLOFT/tree/DnD-Classics/MASTER): immutable original game files.

## DOSDis inputs

DosDis `CODE1.json` and `CODE2.json` provided privately on 2026-10-17 are **candidate-unverified analysis reports**, not a proven decompilation. Source hashes for the provided originals: CODE.1 `fd9b709ceee190e6a70870f9012ebea3bc437cae6bc74e0150d6f922fa750fed`; CODE.2 `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`. Their instruction bytes/addresses can guide source research, but all inferred boundaries require original-byte checks. Do not commit full disassembly JSON (which embeds original instruction bytes); keep it in ignored local research storage.

**Source split baseline:** `85327ed7f25eccbb6184f28360913402d1f5cb3d`. The old `Recomp/` contents are preserved in Git history and on the `recomp` branch, not deleted from the project.

## Verified CODE.2 original VGA graphics bridge

Source-verified [graphics hardware/source map](../Code2/graphics/ORIGINAL_VGA_SOURCE_MAP.md), [32-bit planar-address helper](../Code2/graphics/original_vga_math.hpp), and [read-only original-file span verifier](../Code2/graphics/verify_original_vga.py) are shared from the `recomp` branch as exact matching Git blobs. The original 2AB38 function returns a **32-bit guest address**, whereas a host VGA aperture separately maps its low 16 address bits to each of four planes. The source span verifier checks the owner's original CODE.2 SHA and nine instruction-range SHA values without checking in original bytes.

To validate with a legally owned original: `python3 Code2/graphics/verify_original_vga.py --code2 /path/to/CODE.2`. CI may run `--self-test` without original game files. A host VGA test double and a synthetic frame are not original game frame reproduction.

## Compiled CODE.2 graphics evidence

[Compiled graphics-to-VGA-host protocol and indexed frame evidence](../Code2/graphics/COMPILED_GRAPHICS_PATH.md) is synchronized from `recomp`, alongside the [original VGA source map](../Code2/graphics/ORIGINAL_VGA_SOURCE_MAP.md). Verified on Linux and Windows: **67/67 recomp CTests**, including authentic compiled CODE.2 drawing, page/split handling and palette intensity, but **the frame imagery still uses synthetic input**, not original RES game scenery. The host implementation, Windows build artifacts and accepted R-gates belong to the separate `recomp` branch. Both source-evidence Markdown documents are copied as matching Git blobs; neither ships copyrighted original media.

## Verified real game RES image handling

The independent `recomp` workstream now uses a shared [read-only, bounded RES type-1 parser/decoder](../Code2/graphics/original_res_image_archive.hpp) and executes original CODE.2 `2A20C` image packing → `2F29C` cached resource lookup → `2975C` draw dispatch → `2A380` VGA raster. The [source-referenced original-media report](../Code2/graphics/REAL_RES_TYPE1_SOURCE_RENDER.md) records exact source-binary witnesses, observed original `RES0:15` and `RES1:336` frames, and limitations: the original DOS archive loader `2FD9C`, complete scene compositor and DOS parity are not yet integrated. Preserve this distinction in decompilation source conclusions. The [read-only CODE.2 SHA verifier](../Code2/graphics/verify_original_vga.py) now checks **32** VGA, INIT, RES and first-person renderer code spans against the owner's original binary. The executable adapter itself remains only in `recomp`; no proprietary RES bytes or render captures are copied to this branch.

## Original CODE.2 unloaded RES stream execution

Recomp now executes the **original source `2975C → 2F29C → 2ECB4 → 2EB60 → 2FD9C → 2A380` chain** with read-only host seek/read/position callbacks and bounded original guest memory. The original nine-byte item starts unloaded and is populated by CODE.2 itself; a subsequent `2F29C` call is a cache hit with no further file seek. The original `2FD9C` raw-image path is source-executed. Source `2FB04`/ `2FA30` dispatch and packing execute on compressed images, while the `37700` completion behavior is still supplied by a bounded host decoder and must not be claimed fully decompiled. Windows [milestone #39089791043](https://github.com/Hyperbog/RLOFT/actions/runs/39089791043) and [Linux #39089790975](https://github.com/Hyperbog/RLOFT/actions/runs/39089790975) both passed 68/68 tests; original RES0:15 and RES1:336 frame hashes matched the previous source-backed render. The [source audit](../Code2/graphics/REAL_RES_TYPE1_SOURCE_RENDER.md) and [21-span SHA verifier](../Code2/graphics/verify_original_vga.py) are kept byte-identical with Recomp. These facts help decompilation without merging the separate runnable host graphics adapter.

## Verified 37700/2E7F0 and RES4 graphics source milestones

Original CODE.2 `37700`'s stop flag is **guest memory byte `2B6C2`**, tested at four original instruction addresses; `2FA30` changes it on finished output. [The corrected source-side 37700 control](../Code2/graphics/original_37700_control.hpp) supersedes the diagnostic's former count-only decoder. The giant `Code2.cpp` translation unit still has a legacy parameter-stop version, which must be consolidated later. In Recomp, original CODE.2 `2E7F0` builds unloaded resource slots through read-only DOS-open callbacks, but the complete game startup/INIT file handling remains missing.

The [RES4 source research and original-file evidence](../Code2/graphics/RES4_FIRST_PERSON_SOURCE.md) now establishes `type 6` as EH **header size zero** followed by a separate decoded-size word, and compiled CODE.2 `3395C`/`338C8` samples authentic column-major RES4 textures. Original RES4:0/100/500 were validated in the [Windows 70/70-test milestone #39092086421](https://github.com/Hyperbog/RLOFT/actions/runs/39092086421). This is a texture-column raster diagnostic, **not** an assembled first-person world or DOS-parity proof. The original `37590` typed loader remains host-backed.

All corresponding source-format helpers, binary verifier, research notes, and the default-branch copy of the scheduled Windows milestone workflow are synchronized to `code1-decompilation`. The native test harness and graphics executable remain on `recomp`, and no copyrighted RES images were copied.

## Verified canonical CODE.2 source and RES4 wall-compositor integration (2026-10-18)

The corrected canonical [Code2.cpp](../Code2/Code2.cpp) now invokes the live guest-stop-controlled `37700` using [source helper](../Code2/graphics/original_37700_control.hpp) instead of its obsolete argument-based stop condition. Original `37590` now executes through CODE.2 buffered input and compression callbacks in the independent `recomp` graphics host. The separate source startup gate compiles `16D04` INIT-header reading, `2E768` INFO.DAT flags, and `2E7F0` unloaded table initialization for **all eleven active RES0..RES10** from the authentic installed/CD layouts. Source `335DC`/`3263C` two-pass compositor and `338C8` read authentic RES4 source textures using **synthetic** sweep records; real camera/world projection, occlusion, HUD, and continuous CODE.1-to-CODE.2 gameplay entry remain **unimplemented**.

[Windows milestone #39096508171](https://github.com/Hyperbog/RLOFT/actions/runs/39096508171) passed **71/71 CTests**, all eleven authentic archive startup checks, and genuine RES4:0/100/500/600 compositor/direct-sampler indexed parity. [Original-byte 32-region verifier](../Code2/graphics/verify_original_vga.py) and [full scope/limits](../Code2/graphics/CANONICAL_STARTUP_AND_WALL_SWEEP_20261018.md) have been synchronized as exact Git blobs from Recomp. **R04 unaccepted**. The buildable recomp test executables, read-only DOS host adapters and original graphics assets have not been merged into this Decomp branch.

## Original level grid reaches CODE.2 334C4

A new [bounded, read-only `LEVELS.DAT` parser](../Code2/graphics/original_levels_grid.hpp) uses the original 32-level directory, 0x1D-byte record preamble, 0x2A4-byte header and 16-bit packed cell grid without modifying the owner's bytes. The [verified source audit](../Code2/graphics/ORIGINAL_LEVEL_VIEW_PREP_20261018.md) records the compiled original `334C4` view-preparation path: 115,584 actual archived grid cells pass through original guest offsets `0x25618` and `0x198E0`, with every nontransient bit preserved while `0x0040` is cleared before the source `32954` callback. [Windows/MSVC #39097776940](https://github.com/Hyperbog/RLOFT/actions/runs/39097776940) passed **72/72 CTests** and verified the original LEVELS.DAT SHA for all 32 levels. The camera/raycast `32954` itself is **not yet executed**, so this is not DOS level rendering parity or a completed game boot. Decomp owns the same byte-identical parser/source report; executable test and Windows packaging remain on Recomp.
