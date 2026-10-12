# Native Alpha 0.5: interactive original-world preview — 2026-10-12

**Branch:** `native-engine`. **Product:** the independent Windows `rloft_native.exe`, not `demo-v1` and not the original-DOS recompilation. **Status:** integrated into the native Win32 source; exact-HEAD MSVC packaging / user field test not yet verified.

## What changed

The native asset browser now has an **Explore LEVELS.DAT (preview)** button next to the original-install selector. When a GOG `game.gog` image or loose original installation has already been opened, it reads the original `LEVELS.DAT` through the existing read-only `NativeMedia` source and the strict `decode_native_world_payload` implementation. It opens a **separate**, resizable Windows first-person preview that does not replace the existing original-resources/palettes browser.

The window provides:

- Original map/level selection **0 through 31** with PageUp/PageDown.
- A clearly identified, deterministic **host preview spawn** chosen from passable original cells, preferring more neighboring passable cells and central proximity. It is not a claimed original player/SAVE.DAT start.
- Keyboard stepping **W/S** or **Up/Down**, strafing **A/D**, turning **Left/Right**, level respawn **R**, and close **Esc**. Movement uses the existing bounded `move_native_probe_camera` traversal and original `normal_collision` cell classification.
- A resizable 320×200 CPU-generated image presented by standard GDI as BGRA with aspect-preserving scaling, live level/cell/hit statistics, and a permanent on-screen warning that this is **PROVISIONAL HOST RAYCAST / SYNTHETIC COLORS — NOT DOS GRAPHICS**.
- No reads or writes to user `CHARS`, `SAVE.DAT`, or other mutable original files. The preview owns its decoded world copy. Reopening/replacing the installation in the asset browser does not rebind the open preview's data.

**This milestone does not replace the earlier source-backed original EH6/EH7 RGB6 compositor.** The provisional host DDA renderer intentionally uses fake colors because original automatic camera, wall selection, viewport clipping, and draw-call scheduling remain unverified. The new UI path exercises original-world loading, basic input, and collision as a user-accessible integration slice; it is **not Alpha 1 fidelity or normal original-game progression**.

## Authentic source-data smoke check

User-provided original `LEVELS.DAT` (499,878 bytes; SHA-256 `50d0b1d13f81733a1ac55296d00fcba9aef002202bc00341f1062c37fd017d91`) was read directly and its original 32-record level table checked. All **32 of 32** original map records have at least one ordinarily passable (`cell.code==0`, not special-center) cell, so all original maps contain potential preview spawn locations; this *does not* certify renderer screenshots or movement against DOS.

No new reconstruction-only test suite was added for this UI change. The existing native scene-view, world-runtime and media tests remain in CMake. An exact-head Windows/MSVC build and launch with original GOG media is the immediate acceptance step; last separately accepted 29/29 source-only Linux CI does not prove the newly connected Windows view.

## Field-test procedure / acceptance

1. Build source-identified `rloft_native.exe` from this `native-engine` branch under Windows/MSVC and run its existing native CTests.
2. Open your original installed GOG/DOS directory using the existing `Open original installation...` button or `--install <path>`.
3. Choose **Explore LEVELS.DAT (preview)**; check it opens without errors at level 0 and that W/A/S/D/arrow navigation moves or blocks on original map walls. Verify PageUp/PageDown cover all 32 levels and Esc only closes the preview.
4. Check original `game.gog`, `CHARS`, `SAVE.DAT` remain byte-identical before/after. Diagnose the first reproducible failure with level/cell/input and screenshot.
5. **Do not** report this as original first-person parity, original startup, combat, save progress, or a playable alpha. Exact DOS camera and texture/sprite presentation, original input ticks, actor occupancy, events, animation, and real start position are all still open.

## Next product milestone

Replace the host DDA image generator behind the view interface with the original-CODE.2 automatic ray/scene command generator and proven four-plane RGB6 wall/sprite compositor. After verified original DOS frame and input comparisons, connect original party, gameplay, events and protected save behavior. This UI bridge is valuable only as a run-and-inspect shell for that verified engine work.
