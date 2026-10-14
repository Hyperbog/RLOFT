# Part 23 — Enhanced PC Level 22: automap label viewport repair

**Date:** 2026-10-14. **Project:** enhanced native PC port. **Branch:** `demo-v1`.

## Owner defect tracking (kept separate)

Level 22 has four unresolved owner-reported areas: **(1) graphics / tree placement / colors, (2) inventory, (3) automap, (4) gameplay top menu**. The **title-screen Continue** action is a separate save/load feature; it is not counted as Level 22.

The last owner playback confirmed continuous cinematics and character creation into Level 22, with audible audio. The owner has not yet accepted the level's visual fidelity, inventory operations, automap or full top menu.

## Concrete defect and code change

The automap terrain tiles and player marker were already clipped to the native 48 × 47-cell map field, i.e. **y=6..389** and **x=4..238**, but `AutomapAnnotationPresenter` still used a *larger* character-only clipping rectangle: y=6..393 and x=4..239. Text placed near the bottom and right borders could overwrite the original RES0 map chrome / control sprites.

- `DemoV1/include/rloft/automap_annotation_presenter.hpp`: use **the shared** `AutomapViewportPresenter::kMapMinY/MaxY/MinX/MaxX` constants, eliminating duplicated off-by-four/off-by-one boundaries.
- `DemoV1/src/automap_annotation_presenter.cpp`: the visibility precheck now recognizes labels partly entering from the left or top and labels whose far end leaves the map. Such labels are clipped pixel-by-pixel by the existing glyph blitter, rather than disappearing due to the old `end <= viewportMax` predicate.
- `DemoV1/tests/automap_annotation_presenter_tests.cpp`: verify y=389/x=238 remains drawable, the next row/column cannot change, partially visible left-edge labels remain drawn, and wholly off-map text is ignored.
- `.github/workflows/demo-v1-build.yml`: include the original `automap_annotation_presenter_tests` C++ target in the **focused Windows Alpha compilation and CTest** suite.

No changes to the intro, title, character creation, audio, world physics, world-item state, or original save data were needed.

## Verified build checkpoint

- Source compiled: **`46d1733d6a145ee7dd7e0eb68f848cdc4216814f`**.
- [Windows Actions run #?](https://github.com/Hyperbog/RLOFT/actions/runs/38654822818): **success**, all **38/38 focused Windows CTests passed**, continuous launcher PowerShell syntax and artifact publication passed.
- Published artifact: **`Ravenloft-Continuous-Alpha-Windows-x64`**, GitHub artifact **11935999884**.
- Downloaded published ZIP was verified independently: ZIP has no bad members; EXE is a Windows PE (`MZ`), **961,536 bytes**; SHA-256 matches `ravenloft_demo_v1.exe.sha256`; `build-source-sha.txt` matches the compiled commit; the launcher has a single owned-media path initialization.
- This build has **not** been playtested on the owner's Windows PC. The new *annotation-specific* defect is source-and-synthetic-test addressed, **not field accepted**.

## Remaining work before Alpha field acceptance

1. **Level 22 graphics:** original RES3 type-7 tree/NPC placement, original RES4 texture composition, scene shading and palette still need a current-version Windows image/video and direct correction. Don't infer pixel parity from source validation.
2. **Inventory:** beyond the earlier mirrored bracer and modal keyboard ownership repairs, actual equipment slot/held cursor/container interactions need reproducible live item/slot tests.
3. **Automap:** verify new label clipping, player arrow/chrome, pan buttons and Shift+arrow panning on actual Windows output.
4. **Top menu:** Automap, Quit, Options (session-local music toggle) currently have handlers. Rest, Pray, Memorize, Load and Save require real gameplay/persistence logic and remain unavailable.
5. **Separate title Continue:** genuine native save/restore is required; must not fake resume by starting a new party or wiping the existing game.

The preexisting `ALPHA-BUILD` gate remains met. No other Alpha gate should be closed from this CI-only regression.
