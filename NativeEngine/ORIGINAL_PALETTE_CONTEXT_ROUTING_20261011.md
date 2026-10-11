# Original indexed-asset palette routing is a RUNTIME CONTEXT — Windows-verified milestone

**Date:** 2026-10-11. **Branch:** `native-engine`. The project owner confirmed **different images inside the same RES archive require different palettes**, and multiple RES archives contain mixed graphics. This is a key original-engine behavior, not a collection packaging mistake.

## Controlling research

[Existing original palette router on the default project branch](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/Tools/anm/palette_router.py) already reconstructs concrete CODE.2 callsite rules and the CODE.2 DGROUP `0x81AC` 32-entry level-palette table. [Original COLORS FORMAT audit](https://github.com/Hyperbog/RLOFT/blob/native-engine/Resources/MISC/COLORS/FORMAT.md) proves file stride and original `COLORS` loader `325B0`: 15 banks × 768 DAC6 RGB bytes, selected **by caller and current game state**, with optional brightness transformation and palette applications. **RES EH1 records contain indexed pixels, not an attached universal palette.** CODE.2 additionally applies specific index-to-index remaps for some presentations (e.g. original 19-entry menu cursor map).

Specific indexed-record lookups supported by the research:

- RES0 16..30: world-mode triplets, original banks 0..4, exactly `(record-16)/3`.
- RES0 groups 31..35, 36..40, 41..45, 46..50, 51..55: corresponding world-mode families, bank `record-base`.
- RES0 63,64,89 → bank 10; 65,66,90 → bank 11; 62 and 97..105 → bank 12; 77,116,117 → bank 13.
- RES8 45..71 → bank 4 under the reconstructed environment profile 1 / level-22 context. RES8 8..44 can use bank 3 or 4, depending on level/profile.
- Level-dependent world artwork from other archives derives its bank from the CODE.2 level display table. Without an active known level, a bank 4 world preview is **contextual**, NOT proof of its palette during all game states.
- RES5/6/10 do not have a general proven bank in the audited routing research; an authoritative palette should NOT be invented. Some records may be reused under multiple palettes even when one fixed callsite is known.

## New independent native implementation

- `include/rloft_native/native_palette_context.hpp` + `src/native_palette_context.cpp`: typed `resolve_native_palette(archive,record, optional level)`, preserving **exact callsite / active level context / contextual preview / unresolved** evidence classes and alternative banks. `native_world_palette_bank(level)` is the reconstructed original 32-level table; no decomp gameplay executable or libraries are used.
- `tests/native_palette_context_tests.cpp` validates **mixed palettes WITHIN RES0**, the five world-mode groups, exact bank 10/11/12/13 callsites, world banks at levels 0,3,5,29 and all 32 table entries, RES8 variants, explicitly unresolved archives, and rejected out-of-range levels/path traversal.
- Windows asset browser now has a first `AUTO (source context)` palette selector option, default enabled. It independently resolves each browsed EH1 record. On known rules, the sprite and palette swatches switch together. The inspector report includes evidence classification and original CODE.2 reasoning. On unresolved rules, previous preview bank remains **explicitly labeled provisional/unresolved**.
- Manual original COLORS 0..14 selection remains available and reversible. `Title cursor` is still a **specific CODE.2 menu callsite**: in automatic mode bank 12 with original 19-index recoloring. Browsing ordinary RES records does not perform that specialized cursor recolor.
- **Limit:** the eventual playable replacement engine must model palette as a **mutable render-context state** (current level, modal screen, current VGA palette, fade/brightness transforms, runtime palette changes). A resource record/ID alone cannot certify every eventual draw instance. This router is a provenance-aware asset preview, not a shortcut around native game state.

## Executed Windows evidence

**[GitHub Actions Windows run #38170601091](https://github.com/Hyperbog/RLOFT/actions/runs/38170601091)** completed SUCCESS on Windows Server 2022 / MSVC Release x64; **13/13 CTests passed** including `native_palette_context_tests`. Build packaged and uploaded [asset-only artifact #11714032953](https://github.com/Hyperbog/RLOFT/actions/runs/38170601091/artifacts/11714032953), downloaded and independently checked.

**Exact COMPILED source Git SHA:** `b1892e8448734f3ad8018a0a3911fe9ed59d582c`. Documentation edits after this revision are NOT included in its binary.

- GitHub outer artifact ZIP SHA256: `c504d58cf6ecbb16d5e7ab90771d7e8291ee23ca1822db018e263367c26b525c`.
- User distribution ZIP SHA256: `ae67118f6180b3b44a027ae8d9b2fe008e5479f3b6e5cf774e279122e12ac295`.
- EXE SHA256: `0cf0f8049603e74f2e9d3a77a6e7162c1a5e9047b3eff2137739d4c876875d9b`.
- Inner distribution includes only `rloft_native.exe`, README, SHA256SUMS and provenance JSON; no original user-owned assets. Build provenance records `gameplay_verified=false`.
- No changes were made to the separate `demo-v1` decomp/recomp runtime.

## Acceptance boundary and next actions

**PASS:** independent native Windows compiler/link, 13 original-format/renderer/RES/palette CTests, final asset-free package and exact byte hashes.

**Pending:** real user-GOG visual check of per-record AUTO mapping. In new browser, open the original game, select `AUTO (source context)`, browse RES0 records 63,65,62,77 and compare resulting palette reports (respectively 10/11/12/13), then choose RES3 and inspect the **contextual preview** warning (bank 4 is not universally correct). Confirm manual overrides continue working. Report exceptions by archive, record and current bank. The prior real GOG media-indexing 19/19 success remains valid but is not equivalent to this new runtime-context feature being field-verified.

**Next native renderer work:** use the reconstructed original frame/level/modal palette update events to determine actual active COLORS bank, partial VGA DAC updates, brightness and index remapping at each draw—not infer palette solely from RES path.
