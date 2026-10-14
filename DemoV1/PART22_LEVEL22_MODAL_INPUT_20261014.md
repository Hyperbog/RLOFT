# Part 22 — Enhanced PC Level 22: keyboard ownership and modal controls

**Date:** October 14, 2026  
**Active source revision:** `5c9e87b468ca2e6ffb863b5cb159dbbfffca8842`  
**Verified Windows run:** [GitHub Actions #870](https://github.com/Hyperbog/RLOFT/actions/runs/38630980570) — **37/37 focused CTests passed**, native MSVC executable compiled, one-click launcher syntax validated, Windows Alpha artifact published.  
**Artifact:** `Ravenloft-Continuous-Alpha-Windows-x64`, GitHub artifact ID **11926048873**; independently checked ZIP archive, native PE executable, embedded compiled-source SHA and EXE SHA-256 **27973c6259b5267d59a48d0023cfbc13d5ff95a334b51d7b01b1d2e4ae2e911f**.  
**Release status:** **Pre-Alpha**; this artifact has not undergone an owner on-PC gameplay test.

## Defect separation

The owner reported **four Level 22 areas**: graphics/placement/colors, inventory, automap, and top-menu actions. **Title-screen Continue is a fifth, separate title/save feature**, not part of the Level 22 defect count.

## Real code changes in this checkpoint

### Automap

- Introduced `EnhancedAutomapKeyboard` in a header-only, portable C++17 native adapter.
- Arrow keys now pan using the **same existing `AutomapModalController::apply_action` IDs** as clickable map arrows. Shift+arrows use the existing page-pan IDs 12–15.
- `Win32Host::set_automap_mode` routes the keys exclusively to the map, not to exploration's movement controller. The host clears previous key events and held movement on modal entry/exit.
- Both modal close paths, mouse action 11 and Escape, surrender keyboard ownership explicitly.
- `enhanced_automap_keyboard_tests` validates all directions, page steps, unknown keys, map bounds, and close behavior. `verify_enhanced_level22_ui.py` additionally checks Win32 entry/exit wiring and injects negative mutations.

### Inventory

- While the real inventory modal owns input, **arrow keys no longer register as movement** behind the inventory panel. The existing **D drop-item command** continues to generate its one-shot translated input event and cannot repeat uncontrollably.
- `set_inventory_mode` clears held exploration movement on entry and exit, removing the possibility of a modal-era key press moving the party when the panel closes.
- Previously repaired missing mirrored bracer/gauntlet rendering and the C++ test remain intact.
- This does **not** certify equipment slot targeting, container behavior, item cursor painting or dragging as correct. The owner reported broader inventory bugs; a reproducible slot/item example is still needed.

### Source/build validation

- The focused Windows suite added the new keyboard C++ target and reduced a duplicate automap test name.
- The CI preflight runs `verify_enhanced_level22_ui.py` **before** lengthy C++ compilation, rejecting modal input regressions and false-positive source-mutation tests.
- The latest suite passed **37/37**. This verifies C++ compilation, small synthetic navigation models and source integration; it does **not** prove the actual pixels or interactions are correct on the owner's PC.
- The working CINE00/CINE12, logos, music, lady/cat, card flip and complete character-creation modules are unchanged.

## Remaining native Alpha work

1. **Level 22 graphics:** owner reports tree placement, textures and colors are wrong. Existing RES3 per-object placement, RES4 floor, backdrop color clamp and palette lookup have only passed synthetic tests; obtain a same-source real viewport screenshot to identify the incorrect projection, draw order, palette/shade or source mapping. Do not invent replacement scenery.
2. **Inventory:** beyond modal-key isolation and repaired mirrored equipment art, use an actual item/slot repro to fix runtime targeting and item-state transitions.
3. **Automap:** owner reports slight remaining bugs; verify keyboard arrows, map marker clipping and map chrome with new artifact. Capture a screenshot of the incorrect region if problems persist.
4. **Top edge menu:** Automap, Quit and native Options (music toggle) operate. Rest, Pray, Memorize, Load and Save remain incomplete. Do not show fake success or mutate party/world without real spell/rest/save support.
5. **Separate title Continue:** remains blocked on genuine safe native save/load and full party/world restoration. It must not silently start a new party or discard gameplay progress.

**No Alpha gameplay/visual acceptance gate has been closed solely due to this successful Windows build.** The previously recorded ALPHA-BUILD acceptance remains independent of the owner's next field test.
