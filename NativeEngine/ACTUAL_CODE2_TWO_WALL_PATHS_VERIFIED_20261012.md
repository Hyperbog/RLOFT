# Actual CODE.2 32F8C/33124 original wall-pair emission — verified source and VGA data

**Date:** 2026-10-12. **Scope:** original two-path wall resource selection, original texture U mirroring, source-coded shade and mutable visibility, integrated authentic EH6 pixel rendering. **Not a proof of original DOS camera choice, projected height, depth sorting or full-frame screenshot.**

## Exactly verified original CODE.2 byte domains

Owner-provided original `CODE.2` SHA256:
`853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.

Extract original executable object 1 from original physical file bytes `[0x3AF24:0x3AF24+0x3A852]`, SHA256:
`014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.

Exact original object-1 functions and separately SHA-pinned byte spans:

- `32F8C..33121`, **406 bytes**, SHA256 `57fe0d8c39e1b19e52b567db39df43c04131f3d3ab50ef8d0c0fb72a684825ae`.
- `33124..332B2`, **399 bytes**, SHA256 `a3f6583594908a90514a3f33ccd54f72b9660a464b7e13d2a2f202432b438a7f`.

These authentic original binary routines prove two distinct 28-byte emitted wall-record paths (the separately translated CODE.2 project has original interpreter counterparts):

| Instruction-proven field | 32F8C path | 33124 path |
|---|---|---|
| Original phase input | dword `255E8` | dword `255F0` |
| Source 64-column selector | `(phase >> 4) & 0xFC0` | same |
| Original sentinel field | dword `25610` | dword `25604` |
| Sentinel equals `0xFFFF0000` | **Reflect column, increment word 255F2** | **Leave column unchanged, increment word 255EA** |
| Sentinel differs | Column unchanged | **Reflect column** |
| Original mirror arithmetic | `0xFC0 - selector` | `0xFC0 - selector` |
| Original first RES4 source word | `257A2`, LEVELS header **bank 3** | `256A2`, header **bank 1** |
| Original second RES4 source word | `25722`, LEVELS header **bank 2** | `25622`, header **bank 0** |
| Original pair destination slots | render record `+0x04/+0x08` | same |
| Original source selector destination | render record `+0x10` | same |
| Original current cell shading | cell word `(cell & 0xC000) >> 14` | same |
| Original visibility | cell word OR `0x0440` | same |
| Original special-frame neighbor | `0x0100` with caller-direction `0x0080` replacement | same |
| Original live variant | conditional `0x1000` -> low 16 bits of `1E9A0`, except neighbor replacement | same |

**Important:** Original live ray direction, sentinel branch, the `35730` depth, screen position, neighbor direction, post-emission perspective selection and actual CODE.2 traversal are upstream runtime decisions. NativeEngine's API `emit_native_code2_wall_record` takes them as **explicit caller inputs**. It implements only the exact two-source-record calculation and its bounded map visibility OR. `draw_native_code2_wall_record_column` accepts an explicit first/second source-word slot, projected height, shade and VGA destination and forwards the selected real texture into the separately proven CODE.2 `338C8` native EH6 sampler.

## Native code and synthetic regression

- Added `NativeEngine/include/rloft_native/native_code2_wall_emission.hpp` and `src/native_code2_wall_emission.cpp`.
- Added full original private-local data auditor `src/native_code2_wall_emission_audit_main.cpp`, which enforces both original CODE.2 source-window SHA256 digests and validates real `LEVELS.DAT` and `RES4` ownership before any draw.
- Added `tests/native_code2_wall_emission_tests.cpp` covering both CODE.2 paths, both sentinel/mirroring outcomes, original bank/source IDs, actual EH6 row indices, shade bits, source visibility `0x0440`, caller-stale record rejection, source-bank bounds and VGA draw failures.
- Updated CMake and portable Linux source-only Actions. No original copyrighted CODE.2 or RES4 bytes are committed or included in CI.
- Exact compiled source [`5501ca2ecaef867be0834996d8a40efec60f7a03`](https://github.com/Hyperbog/RLOFT/commit/5501ca2ecaef867be0834996d8a40efec60f7a03), [NativeEngine source-only Linux Actions #38207468265](https://github.com/Hyperbog/RLOFT/actions/runs/38207468265), **26/26 CTests PASS**. Exhaustively verified **all 65,536 low-word source phases × both paths × both sentinel conditions = 262,144/262,144 selector outcomes** with zero regression failures.

## Authentic original input: 32-level resource-pair inventory

Independent original-byte inspection of authentic LEVELS.DAT fixed lookup headers, using the actual CODE.2 table paths:

| Original source metric | Verified |
|---|---:|
| Original level maps | **32/32** |
| Each map's 64 tile code definitions | **2,048** |
| CODE.2 source-path pairs per code | **4,096** |
| Source-bank references evaluated | **8,192** |
| Code definitions whose first/second wall paths have different pairs | **433** |
| Source path pairs with two different texture IDs inside pair | **1,902** |
| Distinct side-labelled source pairs | **883** |
| Distinct referenced wide RES4 EH6 original textures | **452** (previous world material gate) |

All 8,192 original candidates are in the authentic 600-resource wide RES4 texture bank (the other 67 originals are narrow 32-wide). The fifth original LEVELS 64-word header bank holds the uniform marker `0xB5B1`, never treated as a resource table.

## Real original executable C++ versus independent Python verification

The exact native GitHub-built source `3035ea5acc503dbb64f446975c6aa52e6c3a0fb1` (26/26 CTests, [Actions #38207328287](https://github.com/Hyperbog/RLOFT/actions/runs/38207328287)) was run against owner's authentic CODE.2/LEVELS.DAT/RES4/INIT. An independent original-file Python CODE.2/LEVELS selector and EH6 LZSS+VGA raster implementation independently recreated the selected RES4 IDs, mirrored U coordinates and complete 4×64KiB framebuffer without consulting C++ output.

Authentic diagnostic cell is level 0 **(5,0)**, original tile code **6**, with original source four-bank IDs `[70,64,63,70]`. Call phase `0x5555`, explicit sentinel false/true, two path choices, both source words, height 128, shade subtraction 5, page 0, destination columns 8..15. This is a **diagnostic**, not the actual DOS camera/frame.

Actual eight original C++-rendered selected IDs:
```text
32F8C, sentinel false: [70,63] at U=21
32F8C, sentinel true:  [70,63] at U=42
33124, sentinel false: [64,70] at U=42
33124, sentinel true:  [64,70] at U=21
```

- **8/8 original resource calls**, spanning **3 distinct authentic EH6 textures**, independently identical.
- **4/4 original mirror decisions and texture-source U coordinates** independently identical.
- **1,024/1,024 original-source texel reads and indexed VGA-plane writes** independently identical.
- Independently computed four-plane framebuffer SHA256 equals compiled native C++ hash:
  `d1c89d79663d20a41c946be446ceabb4087ff72704ad2e47a70337dc49ba7c43`.
- **Zero differences** between the metadata-only independent and native reports.
- Local-only evidence: `rloft_original_code2_wall_emission_authentic_bank.json`, `rloft_original_code2_wall_independent_comparison.json` and independent source `rloft_code2_wall_original_oracle.py`. Source plus compiled Linux auditor binaries in local `RLOFT-native-CODE2-authentic-dual-bank-3035ea5a.zip`. No original game bytes in these artifacts.

## Remaining work / precise boundary

**VERIFIED:** two CODE.2 wall-record source paths, original opposite mirror conditions, 64-column U selector, original map header resource bank pair routing, source-coded shade class and visible cell bit, read-only original source admission, selected original EH6 drawing via integer 16.16 four-plane sampler.

**OPEN:** original live DDA/raycast and ray direction, where `32F8C` vs `33124` is called from, original `35730` depth, projection/clip and screen placement, dynamic state mutation and neighbor-direction choice from full raycaster, sprite depth/composition, COLORS palette output parity, DOS full-frame screenshot agreement, actor AI/events/saves and playable Windows acceptance. Do not claim finished renderer or game and do not conflate this NativeEngine branch with the separate faithful decomp/recomp project. Windows release workflows remain owner-manual.
