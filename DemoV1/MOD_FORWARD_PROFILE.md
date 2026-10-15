# Ravenloft enhanced PC port — mod-forward profile

**Policy:** [Universal Mod-Forward Development Methodology](../MOD_FORWARD_METHODOLOGY.md) and [Enhanced PC Port Charter](ENHANCED_PC_PORT_CHARTER.md).  
**Active branch:** `demo-v1` · **Target:** native Windows x64 · **Status:** architectural guidance, **not a shipped mod SDK, package loader or editor**.

## Vanilla and source ownership

The unmodified base installation uses authentic content imported locally from the player's legally owned Ravenloft files. Original game data and verified game rules are the baseline; enhanced Windows input, rendering, audio and persistence may replace obsolete DOS mechanisms. A graphical improvement must not silently change world geometry, encounters, character state, movement or scripts.

- **Historical evidence:** immutable original `CODE.1`, `CODE.2`, `DAT`, `RES`, `SINTAB` and analysis files; they are never overwritten by mods.
- **Importer:** `RavenloftBootAssets`, `GameSource` and typed resource decoders currently resolve original-media identities. Existing numeric RES3/RES4/RES8 selectors and original addresses remain **private provenance**, not public stable mod APIs.
- **World simulation:** `ExplorationRuntime` and the existing scene/draw-call models own actor/world geometry and gameplay state. Presentation must consume those plans without mutating gameplay.
- **Presentation:** `ExplorationPresenter` composites RES8 environment, RES4 ground, walls, RES3 type-7 sprites and the party HUD; `EnhancedType7Compositor` already consumes typed decoded pixels/masks, so a future validated resolver can supply replacement rasters without changing draw order or hitboxes.
- **Audio:** native MIDI/audio players play existing authored cues. Typed audio-event bindings, overrides and independent persistent buses are **not yet a tested mod-facing interface**.
- **Persistence:** safe versioned native save/load and active-mod identity still need implementation; the incomplete title Continue and in-game Load/Save are not substitutes.

## Rules for the next rendering and content changes

1. Fix the current **Level 22 tree/NPC placement and palette** using actual authored scene plans, original resource provenance, native frame capture and reproducible Windows screenshots. Never adjust one tree's position by hardcoding a level-specific coordinate in the compositor.
2. Define a canonical **typed resource identity/resolver** before publishing mod-accessible asset overrides. Namespaced symbolic IDs will be stable independently of RES file numbers and physical offsets. Until a tested schema exists, these IDs are a design requirement, **not a compatibility promise**.
3. Resolve decoded imagery and palettes through narrow interfaces. Vanilla results must remain unchanged without an explicit override. Do not force original x86/VGA code paths into the public resolver.
4. Keep gameplay collision, inventory rules, actor placement and script triggers separate from cosmetic sprite/palette replacement. A cosmetic-only replacement must not alter deterministic game state.
5. Future menus should use validated page/action bindings with modal input ownership; arbitrary DLL/plugin execution is not a default mod mechanism. Audio assignments and save metadata must also be explicitly versioned.
6. Validate dimensions, transparency masks, indices, dependencies, sizes and incompatible overrides before modifying live framebuffers or world state. Record the winning resource source and fail clearly on unresolved or conflicting IDs.

## Status and release boundary

| Capability | Current evidence | Mod-facing status |
| --- | --- | --- |
| Authored original-media extraction and typed decoders | Existing `GameSource` and `RavenloftBootAssets` | Internal |
| Native sprite compositing with explicit transparency mask | `enhanced_type7_compositor_tests` | Internal; can become resolver-backed |
| Native zero-pixel draw diagnostics | `Type7SceneZeroPixelDraw` and capture sidecar | Internal observability |
| Namespaced registry / deterministic override resolver | No validated game-loader integration | Proposed |
| Versioned mod manifests / dependency ordering | No runtime package-loader acceptance | Proposed |
| Sprite, level and audio editors using runtime schemas | No shipped editor contract | Proposed |
| Save/load with mod-set migration | Gameplay persistence incomplete | Proposed |

**Verification:** C++ source/CTest and Windows CI establish compiler and synthetic behavior only. A real Windows Level 22 capture and owner acceptance are required to close graphical defects. Never raise release gates merely because this profile exists.

**Next extension seam:** isolate typed scene asset lookup from the original RES index in `ExplorationPresenter` while preserving the original read path as the no-mod default. First collect exact native sprite placement/color evidence; only then introduce a validated resolver and tests for vanilla, replacement, additive and rejected input cases.
