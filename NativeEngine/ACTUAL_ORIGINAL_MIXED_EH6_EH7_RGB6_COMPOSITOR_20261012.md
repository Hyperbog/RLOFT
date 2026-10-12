# Verified integrated original EH6 + EH7 + CODE.2 DAC6 frame compositor (2026-10-12)

**Checkpoint:** `native-engine:0783c7f802ebd350c7ee5fa730b234729d58ff99`, [source-only Linux Actions #38272329991](https://github.com/Hyperbog/RLOFT/actions/runs/38272329991) **29/29 CTests PASS**. This is the **first verified native full-page RGB6 output integrating an authentic original wall and an authentic original scene shape in configurable sequential drawing order**. It does **not** prove live original scene emission, DOS camera, actor animation, source draw order, dynamic VGA DAC, or DOS frame equality.

## Native runtime interface

`native_source_scene_compositor.hpp/.cpp` adds `compose_native_source_scene_rgb6` to the independent NativeEngine. It consumes the existing original-backed stages:

- Original mutable `NativeWorldRuntime`, original `LEVELS.DAT` material tables, original `RES4` EH6 textures and original CODE.2 `32F8C/33124` wall emitters;
- Source-authenticated CODE.2 `35680/35730` fixed-point wall height and `338C8` sampled EH6 source columns, with original 4-plane A000 VGA writes;
- Source-authenticated original `RES3` EH7 shape decoded from actual compressed bytes, original `SINTAB` program, and original CODE.2 `36B14/369D8` VGA column-pass masks;
- Original 32-level CODE.2 palette table and 15-bank `COLORS` DAC6 scanout into complete **320×400** RGB6 frames.

Every draw command is explicitly caller-ordered, including source-cell, wall side/path, phase, sign, sprite ID/subframe, scale, shading, destination row/column and page. This is **not** a recovered original renderer call scheduler: a real CODE.2 raycast/depth/visibility source must still supply those inputs. The batch publishes **both** its four VGA planes **and** 0x0440 mutable world visibility **only when the entire draw and palette conversion succeed**; failed late sprite, palette or wall conversion rolls back all state.

## Actual original-data and independent Python verification

The exact portable C++ compiler artifact for `0783c7f8` was run locally against **authentic original** `CODE.2`, `LEVELS.DAT`, `RES3`, `RES4`, `SINTAB`, `COLORS`, and `INIT`, pinned to independently known original SHA256 identities. The reference inputs were read from the owner's private files; **none of the original files was copied into Git or Actions**.

The fixed, explicitly diagnostic scene uses original **level 0 cell (5,0), code6**, original **RES4 wall texture 70** (source column42, CODE.2 mirrored wall path), original **RES3 EH7 entry0**, original **SINTAB scale126**, original level0 COLORS palette **bank4**, caller-supplied projected wall height **256**, sprite x9 width3, original CODE.2 grouped VGA plane mask and input ECX 0.

- **2 source-backed draw commands:** 1 original wall, 1 original sprite.
- **256 original wall texel reads**, 1 EH7 source read, **4 EH7 destination-store opcode instances** producing **12 plane writes**, total **268 indexed VGA plane writes**.
- Both source-defined orders are run: **wall→sprite** and **sprite→wall**; those orders produce **different** results under deliberately overlapping diagnostics.
- **Four complete framebuffer digests independently matched:** both orders × (4×64-KiB indexed VGA planes **and** complete 320×400 RGB6 image). The independent Python script reads the seven original files directly, reimplements LZSS, original level texture selection, SINTAB raster instructions, original CODE.2 mask/plane layout and COLORS RGB6 conversion without calling any native C++ code.
- The C++ original-data auditor additionally confirms late invalid RES3 ID **does not modify VGA pixels or mutable 0x0440 map flags**.
- **0 independently detected mismatches.** Exact canonical fingerprints:

```text
wall then sprite:
 four-plane VGA SHA256 2e261e427132275f30adf64c272971a0c9bd610a06c5f1e32eeecffd77781ee3
 RGB6 320x400 SHA256  aa461db9dbeb255341327d22bc41e57197a86926b1b99dcdda0ff5e11434ff1e

sprite then wall:
 four-plane VGA SHA256 04494d64f6c525eb36483f0a0189612c4d3e59178e6ebf9c9f8240df34df303a
 RGB6 320x400 SHA256  4a7b11e32e8d87ae154dc34a18709f773e845128fde996db2664b8658bf0c092
```

The original-only independent comparison report (metadata only), compiled native JSON and independent Python source are retained as local `RLOFT_native_mixed_compositor_evidence_0783c7f8.zip`; they contain **no copyrighted game files**. The exact source-only Linux compiled executables are in `RLOFT-native-source-compositor-overlap-0783c7f8.zip` (original media excluded). The new regression `native_source_scene_compositor_tests.cpp` proves synthetic in-bounds wall+sprite, expected visible overwrite, different output under reverse ordering, palette correctness, rejected source ids, invalid projected height and atomic rollback. 29/29 tests passed after correcting an initial test fixture namespace typo; no test was skipped or weakened.

## Status / remaining fidelity work

**Verified and now integrated:** original LEVELS→RES4 source/material lookup, original CODE.2 source wall pair, exact diagnostic CODE.2 wall-height arithmetic, RES4 texel sampling, real RES3 EH7/SINTAB opcode rasterization, CODE.2 VGA masks, 4-plane framebuffer, selected original COLORS DAC6 RGB output, original map visible-cell mutation, and cross-component atomic failure rollback.

**Not verified:** live original camera/ray traversal and wall-side choosing, sprite frame selection/animation, original runtime depth sorting and call order, projected bounds/widths/clip, dynamic VGA DAC page selection and palette fades, original game screenshot parity, live NPC/events/AI/save and user-accepted playable Windows game. **Next:** source-proven CODE.2 ray/wall record traversal and scene ordering, then capture and compare real DOS save-state frames. This composite **is a diagnostic proof**, not a faithful screenshot from the game.

Manual-only Windows release policy remains unchanged; independent NativeEngine remains separate from DemoV1 gameplay and primary faithful decomp/recomp branch.
