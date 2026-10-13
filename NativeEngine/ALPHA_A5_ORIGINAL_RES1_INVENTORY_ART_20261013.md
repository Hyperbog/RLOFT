# Native Alpha A5 — actual original RES1 item artwork in party inventory

**Date:** 2026-10-13  
**Branch:** `native-engine`  
**Implementation checkpoint:** `ec48d572e342db4e339ca4ab18a5ee0f2cea3e56`  
**Status:** Native source code committed; Windows CI requires independent exact-source acceptance. **Pre-Alpha, not DOS inventory UI parity or mutable gameplay.**

## What changed

The A4 `F7` original-CHARS inventory/equipment panel was a source-authenticated **text list** only. A5 adds original visual content instead of a stand-in icon.

- `native_party_item_icon.hpp/.cpp` takes an original `NativeResArchive` containing `RES1`, the **source** absolute resource ID `168 + OBJECTS.DAT[item_id].ui_graphic_index`, and an explicitly provided VGA 6-bit palette. It requires exactly **428** original RES1 records and rejects IDs outside the independent `INIT[+0x02] = 168` item UI namespace **168..335**.
- Each non-empty original resource must be an authentic EH **type 1**. The shared already-tested independent `decode_native_type1_raster` decodes exactly original indexed pixels from its own archive, not rendered placeholders. The source indexed transparent-color key maps to RGBA alpha zero where it applies; index `0xFF` denotes no transparency key in the existing typed decoder's semantics. Valid original zero-length entries remain **empty** and produce **no fabricated texture**. Source-record SHA256, dimensions, frame count and transparency index remain available to source diagnostics.
- For first-party Level22, actual palette colors come from original `COLORS` **world bank 4**, certified by `native_world_palette_bank(22)` and CODE.2 level's active display bank. This is an explicitly labeled *source first-frame preview*; the exact original inventory-modal DAC swaps, additional color-index effects and live animation frame selection are **not** asserted.
- The `F7` GPU panel now draws genuine EH1 icons beside the two actual CODE.1-created actor item rows. Source artifact is cached per original absolute RES1 ID, not inferred from item name/category. Two actors may refer to the same shared source bitmap, which is uploaded once per GL context.
- Textures use **unscaled, padded power-of-two backing images** (preserving every source pixel), with UV maxima to clip the blank padding and transparent alpha blending. This works on basic OpenGL 1.1 rather than assuming modern NPOT texture extensions. Icon display is uniformly fit into an 18×16 native-inspector slot; this is native UI presentation, not a claim about the original DOS inventory slot layout.
- The GL texture cache clears on palette/level changes and is released with the graphics context. No original assets are patched, extracted into the release ZIP, or written to the game folder. F7 remains **read only**.

## Source / acceptance invariants

The original shipped corpus contains **RES1: 428 entries**, 405 EH type-1 resources, 21 intentionally zero-length entries, and two raw terminal portrait markers (RES1:381 and RES1:427). Objects source `INIT[+0x02]` bounds the inventory namespace to **168 source IDs (168..335)**. None of the two known raw marker IDs is in the object-image namespace.

`audit_native_party_item_icon_corpus` traverses all 168 original-resolved slots with this exact native decoder. It preserves the count of nonempty and absent entries, asserts the EH1 decoder/bitmap bounds, checks original/decoded provenance, and forbids a live DOS UI parity claim.

The real original-media executable `rloft_native_startup_audit` now must prove all 168 inventory icon slots decode or are legitimately empty, verify the original Level22 palette selector equals 4, and resolve every item ID from both original CODE.1 audit starter rows through the same original RES1 icon path. The CODE.1 rows contain exactly **ten items**. CI additionally insists `original_inventory_icon_live_ui_parity_verified=false`.

`native_party_item_icon_tests` builds a synthetic RES archive with exactly 428 offset entries and two genuine-header EH1 resources; it tests RGBA source palette channel order, transparent-key alpha, `0xFF` no-key behavior, a valid intentionally empty icon, complete 168-slot namespace traversal, exact lower/upper bounds and corrupt resource rejection.

The Windows x64 build script now requires **43** CTests; [QA PR #22](https://github.com/Hyperbog/RLOFT/pull/22) is the exact-source acceptance request. Do not assert a green CI run without checking actual GitHub Actions job logs and original-media audit execution.

## Remaining playable Alpha gaps

**Still absent:** the real CODE.2 inventory modal rendering behavior, equipped-item moves, category-based compatibility and restrictions, object use/effects, pickup/drop, world item mutable state and persistent equipment alterations. A3 native quicksave preserves **initial** original CHARS; game-time inventory modifications will require a new explicitly versioned host profile schema.

The main 3D camera still lacks true CODE.2 live RES3 sprite-frame selection and ordered composition; Level22 ASSASSIN type9 and its original scripting/encounter path are not yet gameplay-active. A real owner Windows GPU/GUI field playthrough and screenshots remain necessary even if CI is green.

Next: recover original inventory interaction ABI/equipment mutations (or executable-driven Level22 first encounter), and then build a complete first-world state snapshot without misrepresenting original SAVE.DAT.
