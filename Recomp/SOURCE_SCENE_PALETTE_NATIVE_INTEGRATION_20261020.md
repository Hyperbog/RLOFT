# Native CODE.2 scene-selected COLORS palette integration — 2026-10-20

## Implemented (Recomp branch)

The eight-entry copy/overlay at RGB6 indices 246–253 has been **removed** from the shared native renderer palette-loading function, rather than merely disabled.

The source-backed native saved-world rendering modes now obtain the bank ID from the original CODE.2 object-2 scene table:

    selected_bank = CODE2_data[0x81AC + 3 * level_id]

The original level-5 table entry selects bank 3. The resolver in Recomp/src/original_code2_scene_palette.hpp validates level in 0..31, descriptor bounds, and palette bank in 0..14. It is data-driven: other levels can select other banks without recompiling source.

The loader in Recomp/src/code2_compiled_graphics_frame.cpp loads all **768** RGB6 bytes at COLORS offset 1 + bank*768 into the authentic staging position 0xC054, runs compiled CODE.2 32150 brightness processing and 29CA0 VGA DAC writer. Strength 0 is used for saved original scene framing. The existing independent original COLORS bank3 is therefore rendered as an entire bank, not assembled from a guessed eight-color patch. The source format remains read-only and allows user-authored 15-bank replacements without a fixed SHA at runtime; malformed sizes/RGB6 bytes are rejected.

Supported source-input modes: --save-complete-frame, --save-sprite-probe, --save-status-text-fixture, --save-world. The standalone --world LEVELS.DAT diagnostic does not take CODE.2 source data, and retains an explicitly labeled bank0 fallback rather than falsely claiming original scene selection.

**Important state boundary:** the presentation-only scene-bank resolver does not change original guest field 0xC034. The current partial graphics/HUD adapter also uses that field as a mode (0..2) for source RES0 gameplay-frame initialization, and forcibly writing palette-bank 3 there would invalidate existing verified behavior. Full original startup handoff/mutation remains a separate task.

## Evidence and checks

Original retail COLORS bank0 SHA256:
1f60d7831cd7b034ae67c894dc91662f15d4341fbf28e2a954b1b7e3e780f64f

Original retail COLORS bank3 SHA256:
0af8ea393f1f266596af59c1a2013350b51000b7456d377ab284ebbc43db0812

The independent original DOSBox-X capture exactly matches whole bank0 at pass28 and whole bank3 at passes29–30. Level5 scene source selection is bank3. All 744 unaffected RGB6 components and 24 changed ones are independently covered.

Verified Linux authentic original-media run:
https://github.com/Hyperbog/RLOFT/actions/runs/39419812617

Checks include:
- synthetic dynamic descriptor selection (0,3,7), bad level, missing table and out-of-range bank rejection;
- source-authenticated original CODE.2 Level5 table and COLORS 0→3 exact 24-component difference;
- original saved scene, RES3 skeleton, RES0 gameplay HUD/frame, world projection regressions;
- **entire** native output 768-byte RGB6 palette matches owner-original COLORS bank3 from private CI media;
- expected native log marker reports selection from CODE2 table, staging C054 and overlay NONE.

## Explicit limits

This is the data-driven native renderer palette path, **not** a complete playable port. DOSBox-X v1 captured original VGA port writes but no bank-loader CPU call stack; the actual dynamic producer call-site timing between submissions 28→29 remains unverified. The user's save was approximately one animation phase after load. DOS/native indexed pixel parity and animated sprite timing remain unverified. No original media, private SAV or raw I/O logs are committed.
