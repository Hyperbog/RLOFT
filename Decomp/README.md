# Decompilation — CODE.1 and CODE.2

**Active branch:** `code1-decompilation` (repository default). **Scope:** original LE DOS/4GW reverse engineering and human-readable C++ reconstruction. **Not:** the recompiled DOS execution host or the native Windows game.

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

- [`recomp` → `Recomp/`](https://github.com/Hyperbog/RLOFT/tree/recomp/Recomp): source recompilation, original runtime shims, linkage and Windows source tests.
- [`demo-v1` → `DemoV1/`](https://github.com/Hyperbog/RLOFT/tree/demo-v1/DemoV1): enhanced native player game.
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

The independent `recomp` workstream now uses a shared [read-only, bounded RES type-1 parser/decoder](../Code2/graphics/original_res_image_archive.hpp) and executes original CODE.2 `2A20C` image packing → `2F29C` cached resource lookup → `2975C` draw dispatch → `2A380` VGA raster. The [source-referenced original-media report](../Code2/graphics/REAL_RES_TYPE1_SOURCE_RENDER.md) records exact source-binary witnesses, observed original `RES0:15` and `RES1:336` frames, and limitations: the original DOS archive loader `2FD9C`, complete scene compositor and DOS parity are not yet integrated. Preserve this distinction in decompilation source conclusions. The [read-only CODE.2 SHA verifier](../Code2/graphics/verify_original_vga.py) now checks **15** VGA and RES code spans against the owner's original binary. The executable adapter itself remains only in `recomp`; no proprietary RES bytes or render captures are copied to this branch.
