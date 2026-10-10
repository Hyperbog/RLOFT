# Level22 CODE.2 original RES2 and actor-major modal draw-order audit

**Checkpoint date:** 2026-10-10. **User mandate:** every gameplay-code change is original-file verified BEFORE commit; no modernizations, no speculative substitutes. This document is a reproducible original-versus-native proof, not a DOS-visual acceptance claim.

## Immutable file authority checked for this batch

The original user-supplied `CODE.2` was independently checked in the working runtime before making the source changes:

| Property | Verified |
| --- | --- |
| Original `CODE.2` byte length | **558,689** |
| Original SHA-256 | `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314` |
| Object-1 extraction | file `0x3AF24`, length `0x3A852` |
| Object-1 SHA-256 | `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3` |
| Canonical reconstructed source | `Code2/Code2.cpp`, functions listed below |
| Original control-flow confirmation | target-specific GNU `objdump` ranges listed below; never a blind whole-file guess |

The following original file offsets and instruction contracts were checked against the actual shipped CODE.2, then reconciled with the current canonical `Code2.cpp`. `file offset = 0x3AF24 + logical offset`:

| Original logical offset | Physical offset | Disassembly/canonical contract | Previously incorrect native behavior | Source-verified correction |
| --- | --- | --- | --- | --- |
| `0x129B3..129C3` | `0x4D8D7` | `12988` pushes `RES2, 0x25, actor lane, y=0x73` and calls `2E510` | `draw_frame(...,0)` on encumbrance bitmap | `OriginalPartyModalBlit::complete_2e510(RES2:0x25,0x73,lane)` |
| `0x10AAF..10AC2` | `0x4B9D3` | `10A70` phase 1/2 invokes `2E510(y=0xC4,lane,0x14 + subtype*2 + phase-1,RES2)` | One source frame of transition art | Whole original RES2 descriptor |
| `0x10ACE..10AE3` | `0x4B9F2` | `10A70` phase 3 invokes `2E510(y=0xCE,lane,9+subtype,RES2)` | One source frame of opened-container art | Whole original RES2 descriptor |
| `0x12AC1..12ADC` | `0x4D9E5` | `12988` burden-rank label `30508(y=0xA4,x=lane+6,span=0x3C,font=1)` | Centered across all `0x50` actor-lane pixels | Exact `lane+6` and `0x3C` region |
| `0x11268..112E9` | `0x4C18C` | `11268` draws actor base then inventory/character (including nested container when equipped), THEN `2E374` selection outline for selected actor, including inactive | Selection outline once after all four actor lanes | Outline after that actor's content, even for inactive selected actor |
| `0x112EC..11313` | `0x4C210` | `112EC` loops actor index 0..3 and calls `11268` for each lane | Post-loop overlay of all container contexts | Render each actor's own container before its outline, then advance lane |

Canonical `Code2/Code2.cpp` `function_2e510` resolves `2F29C` resource and calls `function_2e4a4(...,start=0,count=0xFFFFFFFF)`; `function_2e4a4` advances its destination row by descriptor WORD+4 (per-frame height). The original binary at `0x2E4A4..2E535` independently confirms the `-1` count sentinel, frame iteration, stride and `2E510` dispatch. **All three RES2 panel operations use this complete-descriptor path, not `2975C`'s selected-frame path.**

## Native source changes and call graph

- `DemoV1/src/party_modal_presenter.cpp`: full-descriptor RES2 drawing for encumbrance, container transition and opened-container panel, retaining original bank, graphic selector, row and lane.
- Exact original burden-rank centered span `x=lane+6,width=0x3C` at `y=0xA4`, with existing original T-text index `0x268+rank`.
- `PartyModalPresenter::draw_open_container` now renders **one actor's** context. `PartyModalPresenter::draw` calls it immediately after that active actor's inventory art, BEFORE the selection outline. Inactive selected columns now receive their outline at the point original `11268` emits it, before the next lane is drawn.
- This preserves `112EC`'s lane-major drawing and prevents resource blits from later container overlays erasing a selection outline or being sequenced after unrelated actor draws.
- `DemoV1/include/rloft/party_modal_presenter.hpp`: single-actor container draw signature; no player-facing API/asset format change.
- The separate original `2975C` single-frame drawer remains unchanged. Original installed `RES2` is loaded unchanged from original game files via the lazy bank-2 decoder committed previously; no derived/replacement artwork or original game assets added.

## Repeatable verification and test gates

The new `DemoV1/tools/test_original_modal_graphics_contract.py` has two evidence layers:

1. **Normal offline CTest:** checks original canonical `Code2.cpp` callsite contracts (`12988`, `10A70`, `11268`, `112EC`, `2E510`, `2E4A4`) against the actual native source draw calls, bank 2 lookup, selection order and original burden label geometry; prevents a regression to single-frame RES2 or after-loop container rendering.
2. **Original binary check (local, not CI):** with `--original-code2 /path/to/CODE.2`, additionally validates the exact original file size/SHA-256 and four **actual instruction-byte windows** at `129B3`, `10AAF`, `10ACE`, `12AC1`. It does not check in a copyrighted disassembly dump or original archive.

The original file SHA and the four small original-instruction windows were verified directly in the working environment for this batch. An independent source-level cross-check inspected **21 corresponding original/canonical/native assertions** before commit. Existing `original_party_modal_blit_tests` now includes synthetic three-frame/two-row examples at **all three original RES2 rows** `0x73`, `0xC4`, `0xCE`, with pixel, height/stride and draw-order assertions. These are synthetic test rasters, **not replacement game assets**.

```sh
python3 DemoV1/tools/test_original_modal_graphics_contract.py \
  --original-code2 /path/to/original/CODE.2
```

**Build policy:** the new CMake Python test brings the current source to **182 registered CTests** when Python is available, versus the last user-authorized successful Windows Full build **#649, 179/179**. Neither this batch nor the two earlier source-only fixes have been built. **DO NOT claim these regressions have passed CTest or that the graphical UI has DOS pixel acceptance.** Only the source/byte-level verification described above was performed.

## Deliberately unresolved and not guessed

The full `12988` interactive modal lifecycle includes page selection, separate original actor-base redraw, pointer restoration and wait-release/wait-press chronology. This batch changes only the proved drawing operations and lane order, **not** unresolved original interrupt timings. The `10BBC` source enters `10A70` only for the container-equipped item and matching source context; existing native context/legacy-fixture fallback and missing original phase-retrace timing require separate source-level acceptance. The entire actor text/compositing path, original VGA backing page and cursor interaction still require original-DOS visual/event comparison.

Current project gate remains **fully playable, source-faithful Level22 through the 22-frame original mist ending before independent Level23 gameplay**; full original game before modernization. Neither 49.8% provisional engineering readiness nor original-DOS acceptance is changed by this source-only batch.
