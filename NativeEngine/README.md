# Ravenloft: Strahd's Possession — Independent Native Engine

## LATEST A1 — validated source CHARS party → Level 22 native entry (2026-10-13)

[Source-only acceptance, new party importer and first-world route](ALPHA_A1_SOURCE_PARTY_LEVEL22_ENTRY_20261013.md). Native CODE.1 completion now validates the original 0x920-byte `CHARS` through independent `NativePartyStart`, then enters the existing original-data GPU level at index 22, original start (4.5,4.5), heading 270°, with two source actors retained and the diagnostic level-cycle/noclip disabled. Media selection opens original cinematic/title/character startup by default; `--play <original-media>` is available, old inspection CLI retained. The first-world GPU view is **not** a completed CODE.2 gameplay session: interactions/events/placed actors/inventory, audio, profile saves and DOS parity remain missing. **Current-HEAD Windows CI compilation and owner visual acceptance remain unverified**; do not call this a first playable Alpha. Next obtain Windows CTest/runtime evidence and implement actual first-level gameplay from source records.

## LATEST A1 — native source CODE.1 stats/name and 0x920 CHARS in memory (2026-10-13)

[Exact original-media source audit and remaining acceptance gates](ALPHA_A1_CODE1_CHARS_IN_MEMORY_20261013.md). The existing DemoV1-derived CODE.1 character presenter, original statistics/roll/editor, name-input/font rules, two-actor completion and 0x920 CHARS serialization are now owned by neutral `Shared/StartupPresentation` and called by the independent native startup window via original RES1/RES7/RES8/CODE.1/OBJECTS.DAT/T assets. Native builds a **memory-only CHARS image** then stops at CODE.2 startup; there is still **no actual world/party runtime import or first playable Alpha**. Portrait hover and name blink are implemented; many original animated transitions/audio are not. Windows/Linux current-HEAD build/CTest and GUI acceptance are not yet confirmed. Do not describe this as fully accepted until exact new CI run evidence exists. Default startup is still the asset inspector, not canonical user game boot.

## CURRENT — Alpha A1 original CINE00/CINE12 cinematic intro (2026-10-13)

[Real-original-file 496-frame cinematic audit and source handoff](ALPHA_A1_ORIGINAL_CINE00_CINE12_20261013.md). The independent native Windows **Original CODE.2 Startup** now plays source **CINE00.ANM (405 frames) → CINE12.ANM (91 frames)** before its original RES0 logos and 14-step title animation, when both CD videos are in the selected read-only game media. The actual DemoV1 `AnmCinematic` source was moved to neutral `Shared/StartupPresentation`, not duplicated. Native `NativeMedia` independently reads original RES0 EH2 subtitle fonts, T text and ANM bytes; original palette and indexed 320×400 frames/tick waits are preserved. [Windows release #38346464375](https://github.com/Hyperbog/RLOFT/actions/runs/38346464375): **38/38 tests passed**; all **496 original frames decoded**, 13 sound commands traced, and source CD CINE00/CINE12/RES0/T/COLORS SHA preserved. **Audio is NOT yet played**, and CODE.1 interactive character presentation, CHARS transfer and normal party/world entry are not integrated; owner Windows visual inspection is still pending. A1 is not complete and this is not the first playable Alpha.

## CURRENT — Alpha A1 original CODE.2 logos and title menu (2026-10-13)

[Visual startup source and acceptance record](ALPHA_A1_RES0_TITLE_MENU_20261013.md): independent `rloft_native.exe` now offers **Original CODE.2 Startup**, using the actual DemoV1 `StartupPresenter` moved to one neutral `Shared/StartupPresentation` library, backed by the read-only `NativeMedia` RES0 EH1/COLORS adapter. The source-pixel logos **RES0 94–96**, title **97**, original **14-step** menu panel animation **98–101**, and selected-menu art **102–105** are rendered into the original 320×400 indexed framebuffer; user mouse selection passes through the real shared CODE.2 supervisor. This **stops explicitly at the CODE.1 character UI boundary**; it does not fabricate character creation or gameplay. [Windows CI #38345217385](https://github.com/Hyperbog/RLOFT/actions/runs/38345217385): **38/38 native tests passed**; real original-file acceptance rendered 12 source RES0 images and 18 menu/selection frames with RES0/COLORS/INIT SHA preservation. DemoV1 still links the same source renderer, not a second copy. Owner desktop testing, original CINE00/CINE12 ANM intro, fade/audio, full CODE.1 presenter, CHARS transfer and world entry remain **open Alpha A1** gates.

## Alpha A1 work underway — shared CODE.2/CODE.1 startup

[Source-preserving A1 extraction checkpoint](ALPHA_A1_SHARED_ORIGINAL_STARTUP_20261012.md): the existing DemoV1 original supervisor boot and CODE.1 character-choice rule engines (including their source tests) were moved **byte-for-byte** to two neutral `Shared/` libraries consumed by both DemoV1 and NativeEngine on `native-engine`. Windows-native install selection now calls the actual shared CODE.2 boot model and reports cold-start versus source-CHARS-present original routing, read-only. [Native 35/35 CTests passed](https://github.com/Hyperbog/RLOFT/actions/runs/38343564098); [unchanged original DemoV1 boot/character tests 2/2 passed](https://github.com/Hyperbog/RLOFT/actions/runs/38343815405). This is boot **logic and rules extraction**, not the interactive original introduction, animated menu, character presenter, actual CHARS import or normal gameplay. The next step is adapter-backed sharing of the existing original animated presenters with the independent native `NativeMedia` original source reader. Native Win32 compile/desktop acceptance from this revision remains open.

## Product roadmap — Alpha → Beta → Final (controlling, 2026-10-12)

**Product:** `NativeEngine/rloft_native.exe`, an independently built Windows replacement engine loading the player's owned Ravenloft files read-only. **Branch:** `native-engine` only. **Separate workstreams:** `DemoV1` (working source-traced startup and character creation), `code1-decompilation` (original behavior evidence), `DnD-Classics` (original source media), and asset-extraction/disassembly tools. Integrate proven implementations across modules; do not rename DemoV1 or fork a second gameplay rewrite.

### Current product state: technical preview, NOT playable Alpha

- **Verified:** native install selection/ISO resource loading; immutable source world across all 32 maps; authentic RES4 wall and floor/ceiling texture IDs; GPU spatial mesh and collision-category checks; 6,204 original compact world actor/fixture/item placements and bounded RES3 frame allocation metadata (not yet drawn); original CODE.2 and VGA diagnostic primitives; automated Windows and portable tests.
- **Actual user field result:** 3D viewport runs but prior GPU exception and incorrect controls have been reported; fixed-axis A/D hand-off is now in the shared navigation path. Live GUI acceptance, stable FPS, and real game entry are separate gates.
- **Not yet implemented for normal gameplay:** original title/creation integrated into *this* native executable, actual party/session, complete source-placed RES3 objects/scenery/trees, dynamic doors and interaction/event VM, enemy AI/combat/spells, inventory/equipment, sound, writable saves, complete quest progression, and full DOS frame parity.
- Versioned `Alpha 0.12` describes the **technical viewport artifact**, not satisfaction of the playable-Alpha exit criteria below. Do not mark Alpha complete on the strength of 34/34 CTests.

### Alpha — FIRST PLAYABLE original-route vertical slice

**Build target:** source-installed launch → original logos/introduction/title → ordinary CODE.1 creation **or** existing CHARS import → valid party and player state → authentic world entry without shortcuts → move/look/automap → interact with at least one source-authored fixture and stateful door → one original scripted encounter, with inventory/combat where encountered → writable *profile* save → exit, restart and reload to the same state.

**Implementation order and dependencies:**

1. **A0 — platform and session ownership:** preserve native install read-only; create a writable profile, separate display renderer from game state, unify portable movement/door state, require exact reproduction of source-level collisions; maintain a single `native-engine` branch. Correct A/D, snapshot/restore, per-level persistent mutation and renderer independent player input.
2. **A1 — original startup re-use:** extract the *existing* DemoV1 `BootFlow`, `AnmCinematic`, `StartupPresenter` and `CharacterCreationFlow` into a neutral shared-source library that both targets can link (never compile `../DemoV1/src` directly in native production); require normal CODE.2→CODE.1→CODE.2 transfer and original 0x920 CHARS payload. Do **not** create a second cinematic, title menu, or character generator.
3. **A2 — authored world entities:** bind original `LEVELS.DAT` 6,204 placements + `INIT`/`NPCS.DAT`/`OBJECTS.DAT` and source `RES3` EH7 frames to visible scene content. Prioritize placed level geometry, multipart outdoor trees and static objects, correct depth/collision, and source-frame choice. Preserve no-draw 147. Reconstruct special doors from original state records, not from fake cube geometry.
4. **A3 — first gameplay loop:** source actor/party positions, at least one interactive original event and inventory transition; portable simulation independent of GPU and presentation; dialog/UI and first original encounter flow. Record any behavior still provisional.
5. **A4 — profile persistence and real field acceptance:** saves/loads never write to the installation; original CHARS import and own profile format distinguish source and native provenance; automated process restart and round-trip; hands-on Windows run through original menu, first action, encounter, save/reload.

**Alpha exit gate:** independently packaged asset-free x64 EXE; reproducible CI pass from exact SHA with relevant original-media audits; recorded uninterrupted end-to-end *owner-played* route above, including post-relaunch state; no mandatory debug teleport or fabricated party; known divergences documented. **Open until those are measured.**

### Beta — CONTENT COMPLETE original campaign

- Implement all original explorable levels, placement/event transitions, door/secret/teleport/quest triggers, stateful scene animation and dialogue.
- Complete class/party rules, equipment, inventory, spells, status, combat/AI, rests, experience and progression using original-file behavior and CODE.1/CODE.2 evidence, including original item and monster tables.
- Complete original cinematics/title/menus, runtime UI, automap, palettes, sound/music and save compatibility strategy (native profile format plus explicitly verified original import/export only).
- Complete playthrough from new party to canonical ending on a regular Windows installation, with source-authenticated checkpoints at major quest transitions. Include save/reload at every major chapter, regression and performance budgets for native, HD and 4K; controller/accessibility and mod API only where they do not block content completeness.
- **Beta exit:** all original campaign content reachable and finishable with no progression-blocking bug, original media immutable, no crashes/corrupt saves in bounded endurance runs; known frame/audio/DOS-behavior differences categorized, not hidden.

### Final — FAITHFUL, STABLE, DISTRIBUTABLE

- Close high-priority DOS behavior/frame/audio differences against recorded original reference cases (camera/raycast, wall-state selection, actor occlusion, UI/cinematics, timers); prove actual equivalence where claimed, explicitly document irreducible platform differences.
- Verified clean Windows installation and update/uninstall, input/controller/screen-mode compatibility, high-DPI 4K performance, crash diagnostics, corrupt-file recovery, backup/migration, and stable reversible mod/resource overlay format.
- Full campaign regression, saves and alternate quest/party scenarios across original media versions; licensing audit and **zero proprietary game data** in any redistributed release.
- **Final exit:** repeatable artifact/source hashes; zero known P0/P1 release blockers; independently reproducible first-to-last playthrough, safe saves and upgrades, documented compatibility/performance matrix and maintained source-grounded evidence.

### Build/release rule

**Advance gates, not labels.** A successful Windows compiler or a 32-map headless screenshot check certifies only that specific subsystem. Original data decoding ≠ scene fidelity; headless CI ≠ live GPU acceptance; a working camera ≠ playable game; DemoV1's frontend success ≠ native frontend integration. Maintain separate checkmarks for *implemented*, *CI validated*, *owner field validated* and *DOS behavior compared*. All new work should close one of A0–A4 or a later named Beta/Final gap. Do not add isolated reconstruction suites unless required to fix a concrete user-facing issue.

## Native Alpha 0.12 — original v0.7 internal resolution modes and native branch consolidation

[Accepted Windows real-original-data source/build and branch-cleanup report](ALPHA012_V07_INTERNAL_RESOLUTION_CONTROLS_AND_BRANCH_CLEANUP_20261012.md). The independent Win32 GPU view now implements the *actual legacy v0.7* five internal render target resolutions **Native/Auto, 1280×720, 1920×1080, 2560×1440, 3840×2160** on F2, using WGL framebuffer objects with aspect-preserving presentation and independent F11 fullscreen (rather than window-size cycling). F1 controls HUD, Shift sprint, owner-requested reversed horizontal mouse-look, F3 nearest/bilinear/mipmap filtering, F4 0/2/4× MSAA when supported, F8 map panel sizes 150/210/300, F12 BMP screenshot and Tab minimap are present; original keyboard Q/E and optional M mouse remain. New original-media audit verifies **444 distinct source RES4 GPU-use materials** across 32 maps. [Windows #38340993133](https://github.com/Hyperbog/RLOFT/actions/runs/38340993133) passed **34/34 native CTests**, full source-media audit and actual GitHub Actions cleanup of **11 obsolete native acceptance branches**. `native-engine` is now the **only native-engine branch**; unrelated project branches, default branch and DnD-Classics original files preserved. The reported GPU runtime error is now **diagnosable with its real exception text**, not yet physically resolved on the user's GPU. Original CODE.2 VGA reference view, live scenery/actors, the old v0.7 editor/mod overrides and DOS frame fidelity are separate incomplete milestones.

## Native GPU Alpha 0.11 — primary geometric exploration, CODE.2 VGA preserved

[Full source and Win64-original-data acceptance checkpoint](ALPHA011_ORIGINAL_GPU_WORLD_EXPLORATION_ACCEPTANCE_20261012.md). The original-file-backed native executable now has **two separate first-person views**: the new primary **Explore Original World (3D GPU)** with world-space depth-tested OpenGL geometry from original `LEVELS.DAT` cell topology, authored RES4 x/y lower+upper wall materials and narrow source floor/ceiling selectors, and the existing **CODE.2 VGA reference view** retained for original frame research. Level22 source wall 147 is deliberately no-draw, not a fabricated wall; CODE.2 environment shades remain visible for missing ceilings. The GPU mode supports Q/E, WASD, optional M/right-click mouse yaw/pitch, F2 real geometry resolution changes, N source texture filtering and Tab memory-only automap. [Win64 CI #38336947761](https://github.com/Hyperbog/RLOFT/actions/runs/38336947761) **34/34 CTests** and **32/32 original levels source-geometry acceptance**, asset-free x64 EXE/ZIP verified. **This is an original-data-backed accelerated exploratory 3D presentation, not DOS frame parity or full game.** Original RES3 EH7 static scenery/trees, live door geometry, dynamic actors, events/UI/gameplay/saves and user GPU desktop field test remain open.

## Alpha 0.10 — screenshot-specific wall tier correction and legacy-v0.7 input/viewport integration

[Evidence, v0.7 comparator, exact source-authored level-1 tile, CI binary digests and GPU migration plan](LEGACY_VIEWER_V07_NATIVE_VIEWPORT_MIGRATION_20261012.md). The user reported in Alpha 0.9 severe wall art substitution/stretch on contact, cramped 256×256 GDI viewport and jerky mouse movement. New Windows preview: source-authored x-normal bank 0 lower/1 upper and y-normal bank 2 lower/3 upper, cell bit `0x0200`, original upper texture-row range; explicit source-viewport top-down crop before Win32 stretching; frame-timed continuous WASD+Q/E movement and relative mouse sampling (no SetCursorPos/expensive redraw per WM_MOUSEMOVE); `Tab` toggles session-only original-map automap with +/- zoom; `N` toggles nearest/smooth GDI presentation filtering. The original filename and source-level texture identifiers are preserved. Field example level1 (14,10) original 0x022C bank [497,62,499,62], header upper rows96. [Windows x64 run #38335193771](https://github.com/Hyperbog/RLOFT/actions/runs/38335193771) **33/33 CTests + 32/32 genuine maps PASS**, 2,029 authored upper wall strips, 160 original EH6 textures decoded; artifact [#11785130189](https://github.com/Hyperbog/RLOFT/actions/runs/38335193771/artifacts/11785130189). Original source unchanged; no copyrighted game files bundled. **Still a host projected world technical alpha**, not DOS frame parity, not GPU high-resolution rendering and not a first-playable game.

## Alpha 0.9: perspective, authenticated original ceiling/floor sources, reliable optional mouse — 2026-10-12

[Source CODE.2 traced IDs, known fidelity limits, and certified Windows 33/33 + real-original 32/32 acceptance](ALPHA09_AUTHORED_SURFACES_PERSPECTIVE_MOUSE_ACCEPTANCE_20261012.md). Following owner field reports that Alpha 0.8 had stretched perspective, flat sky/floor and nonworking mouse-look, Alpha 0.9 uses true DOS horizontally doubled pixel aspect, a zoomed x32..287/y4..259 viewport by default (V toggles full 320x400 indexed frame), corrected host focal camera rather than arbitrary 55/d height, and exact level-authored upper/lower RES4 **32x128 surface textures** selected from CODE.2 `1F317:+0x05` and `1F388:+0x07` plus the original dynamically initialized narrow texture bank. Genuine upper `-1` cases preserve CODE.2 backdrop rather than inventing sky. **Surface TEXTURE IDENTITIES are authentic; 34A74 projection/UV and original live wall-ray selection remain provisional.** Mouse input no longer depends on raw WM_INPUT; explicit M/right-click relative WM_MOUSEMOVE yaw+pitch mode (default off) preserves Q/E native turning. [Win64 build #38330375936](https://github.com/Hyperbog/RLOFT/actions/runs/38330375936) **33/33 CTests PASS** and all 32 authentic levels rendered with 987,256 original floor +842,872 original upper texels; originals unchanged. Direct owner's mouse/visual field verification remains open.

## Alpha 0.8 — wall-contact, original CODE.2 sky/floor and optional Q/E mouse-look (2026-10-12)

[Source-authenticated CODE.2 environment reconstruction, collision regression, controls and accepted Win64 download](ALPHA08_COLLISION_BACKDROP_MOUSE_CONTROLS_20261012.md). Independent `rloft_native.exe` now uses swept host-player clearance (radius 0.26), wall sliding and frame freeze on rejected motion to prevent near-wall texture saturation; it maps 256 rays to the authentic x32..287, y4..259 viewport and draws the source `33E28/33DE0` sky/floor bands with actual LEVELS header +0x298/+0x299 and CODE.2 FE94 129-byte palette shade table, replacing invented flat backdrop. **Q/E** turn instead of horizontal arrows; **M** toggles OPTIONAL raw mouse yaw/pitch; default original-style keyboard fixed horizon retained. Source RES4 materials and CODE.2 EH6/VGA sampler remain original-data-backed, though exact wall-face/dual-bank scheduling and DOS camera parity are not yet complete. [Windows #38328986042](https://github.com/Hyperbog/RLOFT/actions/runs/38328986042): **33/33 Windows tests PASS**, authentic **32/32 original level renders**, source SHA preservation and published Windows executable. [Download CI artifact](https://github.com/Hyperbog/RLOFT/actions/runs/38328986042/artifacts/11781988551). Direct live user mouse/GUI testing still pending. Native source `2b54a176c3a60b0458a06fb11cbd9f6b35357522`, Windows compiled acceptance `f80fcc2202ff33cc88c1feedb6c8b8dbba7c79dc`.

## Alpha 0.7 — original CODE.2 quadrant, fixed-point VGA and 320×400 native window (2026-10-12)

[Complete original-source byte audit, original-file Windows acceptance and download fingerprints](ALPHA_CODE2_QUADRANT_VGA_WINDOWS_ACCEPTANCE_20261012.md). Alpha 0.7 now connects the independently verified original CODE.2 **32954** ray-quadrant state (four signed step/mirror variants) and CODE.2 **338C8** fixed-point 0x800000/height EH6 sampler directly to the interactive Windows 320×400 **four-plane VGA + COLORS DAC6** output path. A single bounded test covers all 3,600 original tenth-degree quadrants. [Windows/MSVC #38327641005](https://github.com/Hyperbog/RLOFT/actions/runs/38327641005) **33/33 tests PASS** and original-media **32/32 maps PASS**, generating **10,097** original-textured wall columns, **1,167,450** source texel reads and **97** unique authentic RES4 textures. Original source files unchanged, no original resources in the downloadable package. **Original live ray/camera, source-word choice, shader/ceiling, sprites and gameplay remain unverified**, and the preview still uses a host-generated DDA.

## Alpha 0.6 — authentic RES4 textures, 32 original maps, Windows media acceptance (2026-10-12)

[Original media-backed textured interactive preview: successful Windows acceptance and exact binary hashes](ALPHA_ORIGINAL_RES4_TEXTURED_PREVIEW_ACCEPTANCE_20261012.md). The independent `rloft_native.exe` GUI now combines source-backed original `LEVELS.DAT` collision/material lookups, actual `RES4` 64×128 EH6 wall texels, exact per-level `COLORS` DAC6 banks and W/A/S/D + arrow-key exploration in its separate 320×200 interactive window. Source-only Linux passed; [isolated Windows/MSVC original-media acceptance #38325198679](https://github.com/Hyperbog/RLOFT/actions/runs/38325198679) passed **32/32 CTests** and a **32/32 authentic-level end-to-end smoke**, with 5,048 source-textured wall columns, 360,182 RES4 texels and 95 distinct source textures, all original source-file hashes unchanged. [Download the original Windows CI ZIP](https://github.com/Hyperbog/RLOFT/actions/runs/38325198679/artifacts/11780003145). **This is Alpha 0.6 preview only:** camera ray paths, wall face selector, heights, floors, actors/events/party/saves and real DOS-frame parity remain open; no personally observed GUI GOG session is certified.

## Native Alpha 0.5: interactive original LEVELS.DAT preview (2026-10-12)

[Native Windows world-preview integration and field acceptance](ALPHA_WORLD_PREVIEW_WINDOWS_20261012.md):
the independent `rloft_native.exe` source now offers **Explore LEVELS.DAT (preview)** after selecting an original GOG or DOS installation. It opens a separate resizable 320×200 *host-provisional* first-person view, reads all 32 original maps, uses their source-backed collision/passability for W/A/S/D and arrow-key navigation, and supports PageUp/PageDown level switching. The supplied original `LEVELS.DAT` was smoke-checked for potential passable spawn cells on every map. Original media and saves are read-only. **Windows/MSVC x64 build has now PASSED 32/32 CTests** in [one-off isolated build #38323574522](https://github.com/Hyperbog/RLOFT/actions/runs/38323574522), producing an asset-free verified executable and ZIP. The new interactive preview still awaits direct user GOG GUI field testing and is not Alpha 1 DOS fidelity. The provisional output uses synthetic colors, not the independently verified original EH6/EH7 framebuffer compositor. The priority is now bringing original camera/ray/call scheduling into this working host shell; do not relabel it as faithful gameplay.

## Original CODE.2 scene staging and 400-record sorting — 2026-10-12

[Detailed class-one/class-two staging and original-media proof](ORIGINAL_CODE2_NON_NPC_SCENE_STAGING_20261012.md):
the independent native C++ core now handles original CODE.2
placed/loose shape-base arithmetic, all 16-bit packed view variants,
original `35798` projection and threshold gating as a **non-published
scene-preparation stage**. The uploaded `OBJECTS.DAT` validated
2,688 authentic definition/view cases with zero mismatches; 131,072
independent synthetic combinations also passed. An audit corrected
the scene-order capacity from 200 to **400** records based on original
`0x2260` buffer stride and `0x190` producer saturation. Both
new modules passed an isolated CMake 2/2 run and sanitizers. Original
animation, source record producers beyond these bounded slices, full
renderer dispatch and DOS-frame parity remain open. **31 registered
native tests are not a 31/31 passing current-head CI claim**; the last
independently accepted full Linux source run remains 29/29.

## Original CODE.2 2C3F1 scene draw-order source checkpoint (2026-10-12)

[Exact original-byte selection and bounded native regression](ORIGINAL_CODE2_SCENE_RECORD_ORDER_20261012.md):
the active 22-byte scene-record buffer now has an independently
source-audited, original-order minimum projected-size selection helper,
including stable ties, 0x7D high-byte visited mark, and draw-index
annotations. Local isolated C++17 tests pass; a new CMake CTest is
registered but **current-head Windows/Linux CI is not yet verified**.
This helper consumes **caller-authored scene records**. Original
2B6D0..2C22E scene generation, automatic camera/depth inputs, type
dispatch, EH7 call scheduling and actual DOS frame parity remain open.
The previous 29/29 Linux suite applies only to its cited earlier SHA.

## CONTROLLING original mixed EH6+EH7 four-plane RGB6 compositing gate — 2026-10-12

[Verified first full 320x400 DAC6 frame from authentic wall and sprite resources](ACTUAL_ORIGINAL_MIXED_EH6_EH7_RGB6_COMPOSITOR_20261012.md). NativeEngine exact source **0783c7f802ebd350c7ee5fa730b234729d58ff99**, [Linux source-only Actions #38272329991](https://github.com/Hyperbog/RLOFT/actions/runs/38272329991) **29/29 CTests PASS**. The engine now composes the original LEVELS/RES4/CODE.2 wall projector and original RES3/SINTAB/CODE.2 EH7 VGA source paths in explicitly caller-chosen order, converts through CODE.2 level COLORS palette to 320x400 DAC6 RGB6, and atomically commits both world visibility and four-plane VGA state. The actual CI-compiled binary was run locally on original CODE.2/LEVELS.DAT/RES3/RES4/SINTAB/COLORS/INIT: 2 ordered wall/sprite calls, 256 wall texels, 1 EH7 source read, **268 VGA plane writes**, 4 complete indexed/RGB6 order-sensitive framebuffer hashes. Another independent Python original-byte decoder matched **all 4/4 full-frame hashes, ZERO differences**, and original-data rollback was validated. All original files remain private; no copyrighted data shipped or committed.

**Important:** This is a diagnostic source-data-backed **mixed compositor**, NOT original DOS camera, ray/occlusion/depth sorting, original actor pose, 3D frame parity, DAC fade/page scheduling, gameplay VM or a playable Windows game. The scene commands and ordering are still explicit caller inputs. Next prioritize exact original raycaster/depth/call scheduler, then original DOS save/screenshot comparisons. Do not link DemoV1 gameplay or trigger manual Windows release Actions.

## CONTROLLING original CODE.2 32-level palette and 15-bank DAC6 VGA scanout (2026-10-12)

[Authenticated CODE.2/COLORS→four-plane RGB6 scanout and independent parity](ACTUAL_ORIGINAL_CODE2_COLORS_RGB6_SCANOUT_20261012.md). Native source `7b1f30b98997ac45adedc5c14f12823565b2ed0e`, [Linux source-only Actions #38257306292](https://github.com/Hyperbog/RLOFT/actions/runs/38257306292): **28/28 CTests PASS**. Recovered all 32 original CODE.2 three-byte level display tuples from object2+0x81AC, 96-byte source SHA `7370bd2fbc47895d0f81826ff98933710d1477b7f36d5882f9ea111e053f8e93`, selecting four of original 15 COLORS DAC6 banks (bank0:1, bank2:8, bank3:11, bank4:12 levels). Original 4-plane VGA pixels now map to exact unscaled 6-bit RGB on 320×200/320×400 source page0/page1 without approximating DAC8/gamma.

Actual GitHub-built C++ auditor ran privately against original CODE.2/COLORS/RES4 and matched a separately authored Python LZSS EH6/VGA/RGB6 implementation: **32/32 level page0 complete-frame SHA256, 1/1 page1 complete-frame SHA256, 15/15 palette banks, four distinct world palette outputs, zero field differences**. Test input is three explicit diagnostic *authentic-texture* strips (224 original EH6 texel reads) plus eight declared synthetic palette-index probes (246..253) used to distinguish world palettes; never misreport as authentic gameplay frames. Canonical independent source-only verifier `SharedDecomp/verify_native_vga_rgb6.py` passed [Research Actions #38257972447](https://github.com/Hyperbog/RLOFT/actions/runs/38257972447) **210/210 Python tests** (39 ResearchInfra +171 SharedDecomp) at `code1-decompilation:31cae2807895eedbadae45812d76d987190188ef`. No licensed media enters Git or Actions.

**Open:** actual DOS live camera/ray traversal, automatic wall/scene resource selection, occlusion/depth, original DAC palette runtime fades and CRTC page choice, true DOS framebuffer parity, actors/VM/save and playable native Windows build. Remain separate from DemoV1; Windows release Actions manual-only.

## CONTROLLING original CODE.2 35680/35730/35798 projection arithmetic gate (2026-10-12)

[Exact original fixed-point projection and textured-wall integration verified](ACTUAL_CODE2_PROJECTION_35680_35730_35798_VERIFIED_20261012.md). Exact native source `52ebe94988d74181bf469d6b1ad9bb400ed84420`, [Linux source-only Actions #38208398620](https://github.com/Hyperbog/RLOFT/actions/runs/38208398620), **27/27 CTests passed**. Implemented exact original CODE.2 low/high 16-bit MUL/ADC fixed-point helper `35680`, unsigned projected wall height `35730` with original depth floor `0x1680`, signed 64-bit intermediate IDIV screen-point projection `35798` with near rejection `0x5800` and x-coordinate preservation, and real HEIGHT→previously proved wall material→RES4 EH6→VGA drawing integration. **786,432 helper arithmetic checks** ran in the native suite.

The compiled auditor was executed privately against authentic CODE.2 and SHA-verified the **three exact source instruction windows**, then the source-independent Python implementation matched **4,096/4,096** deterministic projection cases (114,688 encoded result bytes), **1,339 accepted, 2,757 rejected, 2,177 wall-distance clamps**, zero differences. Complete case digest `7defd11aded87d648697c9448325f96ae3fb8cad28415fc199fcb172852f53fd`. Independent canonical verifier `SharedDecomp/verify_native_code2_projection.py` in `code1-decompilation:38a24e58` passed [source-only Research Actions #38208618987](https://github.com/Hyperbog/RLOFT/actions/runs/38208618987), **200/200 tests** (39 ResearchInfra + 161 SharedDecomp).

**Boundary:** CODE.2 source math and diagnostic projected wall columns are verified; live original player camera/ray traversal, original automatic path/texture/shade selection, screen clipping, sprite occlusion, complete DOS frame parity, gameplay AI/events/saves and Windows playable GUI remain OPEN. Callers supply original projection register values explicitly; do not relabel host DDA or these samples as DOS-authentic full frames. No original copyrighted files in Git/CI; Windows release Actions manual-only; NativeEngine stays separate from DemoV1 and faithful decomp/recomp.

## Controlling authentic CODE.2 two-path wall emission checkpoint (2026-10-12)

[Original 32F8C/33124 wall emitter, texture selection and real VGA verification](ACTUAL_CODE2_TWO_WALL_PATHS_VERIFIED_20261012.md). Original CODE.2 object1 406-byte 32F8C path SHA256 `57fe0d8c39e1b19e52b567db39df43c04131f3d3ab50ef8d0c0fb72a684825ae`, and 399-byte 33124 path SHA256 `a3f6583594908a90514a3f33ccd54f72b9660a464b7e13d2a2f202432b438a7f`. The native implementation `native_code2_wall_emission.*` faithfully maps branch 32F8C to original LEVELS RES4 banks **3/2**, branch 33124 to banks **1/0**, with opposite sentinel-controlled `(phase>>4)&0xFC0` column mirroring, source cell `0xC000` shading class, real variant IDs and `0x0440` visibility. Caller must explicitly supply branch/sentinel/phase/depth/source height and view direction: original 3D ray and projection selection is **NOT** certified.

Native commit `5501ca2ecaef867be0834996d8a40efec60f7a03`, [Linux Actions #38207468265](https://github.com/Hyperbog/RLOFT/actions/runs/38207468265) **26/26 CTests passed**, including all **262,144** low-word phase × wall-path × sentinel cases. Original map header inventory: **32 maps**, **2,048** tile code definitions, **4,096** actual source-path material pairs, **8,192** referenced material IDs, 433 code entries with distinct side pairs, 1,902 pairs with different original face resource IDs. Exact compiled source at `3035ea5a` run privately against original CODE.2/LEVELS.DAT/RES4/INIT produced 8/8 original texture-backed diagnostic draws of three distinct original materials, 1,024 indexed VGA writes, independently matched C++/Python four-plane SHA256 `d1c89d79663d20a41c946be446ceabb4087ff72704ad2e47a70337dc49ba7c43`, zero mismatches. Licensed source files remain outside Git/CI.

Next: CODE.2 original 35730 ray depth and upstream branch source, live camera/clip/texture projection, sprite occlusion/depth, palette frame parity, native AI/events/saves and current Windows acceptance. Original full DOS frame parity and playable engine remain open; NativeEngine stays independent of DemoV1 and faithful code decomp/recomp; Windows release actions remain owner-manual.

## Current controlling original world-to-RES4 surface binding gate (2026-10-12)

[Authentic source material binding and rendered-column milestone](ACTUAL_ORIGINAL_WORLD_RES4_MATERIAL_BINDING_20261012.md). Native commit `06c821e2570b6300fb8b894fe992c84a3ccf3377`, [source-only Linux Actions #38206444852](https://github.com/Hyperbog/RLOFT/actions/runs/38206444852): **25/25 CTests PASS**. Exact CI-built C++ audited original LEVELS.DAT, INIT and RES4 locally: **32/32 levels, 8,192/8,192 tile-code×candidate-bank source bindings, 452 distinct referenced original wide EH6 textures decoded**, 2,894 nonzero + 5,298 zero-valued valid candidate source IDs. The original fifth 64-word header bank is a constant **0xB5B1** and is **NOT** a resource-ID bank. Independently implemented Python original-byte parser/LZSS/4-plane sampler confirmed **all 32 level SHA256 and one real-original diagnostic indexed framebuffer SHA256**, zero differences. Combined domain SHA `565ab170002aa1fcbbf706e6fb3c64429e0d6778f8651f72769639cf6df0a8a5`.

The new `native_world_materials` module joins source-backed original mutable tile/resource variants to actual RES4 EH6 decoding and `native_type6_column_sampler`. Choosing one of the four map material banks, source texture column, camera-projected height, shade, and destination VGA coordinates still requires original CODE.2 camera/view logic; explicit diagnostic draw operands are **not** DOS frame fidelity. Remaining: authentic projection/ray traversal/material selector, wall/sprite occlusion, original palette/frame equality, actor/events/save and playable Windows. No licensed media committed, no DemoV1 gameplay linked, Windows release actions stay manual-only.

## CONTROLLING full RES4 667-texture fixed-point sampler checkpoint (2026-10-12)

[Full original RES4 16.16 texture-column verification](ACTUAL_ORIGINAL_RES4_TYPE6_667_TEXTURE_STRIPS_20261012.md). NativeEngine source ca50db31029311cf8e97a1f48408576b82ffd30d compiled and passed [source-only Linux Actions #38202663907](https://github.com/Hyperbog/RLOFT/actions/runs/38202663907) **24/24 CTests**. New independent native implementation native_type6_column_sampler.hpp/.cpp reconstructs CODE.2 338C8's 128-texel EH6 source column, 0x800000 / projected-height integer 16.16 stepping, byte-index subtraction, 80-byte VGA plane-row writes, page 0/page 1 and transactional draw batches. Original CODE.2 camera, wall material, shade/height operands are EXPLICIT diagnostic inputs, never guessed.

The exact compiled Linux binary was executed locally against authentic original RES4, INIT and COLORS. **All 667 original EH6 resources** (600 wide 64x128, 67 narrow 32x128) and **all 15 COLORS VGA palettes** validated; **320,160 source texel reads and 320,160 masked plane writes** matched a separate independent original-file Python decoder. All **4,019/4,019 per-resource/aggregate typed values matched (zero divergences)**, including every original compressed-entry SHA256, decoded row-major SHA256 and full four-plane indexed-output SHA256. Combined diagnostic digest: cfdbe7d5731a3b0e9f9828f0fb9a5cded21b3d565d103c9f901c34e583da2524. Original media NEVER committed or uploaded to CI.

Canonical research comparator SharedDecomp/verify_native_type6_columns.py introduced in code1-decompilation:cd3b85c020628f64cdb81be049072221eb948495; [research CI #38203008405](https://github.com/Hyperbog/RLOFT/actions/runs/38203008405) **189/189 tests passed** (39 ResearchInfra + 150 SharedDecomp). This is **source/diagnostic texture-pixel parity**, NOT reconstructed CODE.2 live raycast, texture/frame selector, actual camera projected heights, depth/occlusion, screen-composite color output, DOS frame fidelity, native actor/events/save, or playable Windows build. Next integrate source-proven camera and world material/RES4 ID into this sampler and the established EH7+SINTAB+CODE.2 VGA pipeline, then validate complete DOS reference frames. Keep NativeEngine independent of DemoV1 and Windows release workflow owner-manual.

## CONTROLLING source-authored frame allocation milestone — 42,429 valid original RES3 references

[Original authored RES3 frame allocation audit and open fidelity limits](ACTUAL_ORIGINAL_AUTHORED_FRAME_RANGES_20261011.md). Native source **88653be0729ff9d4651a2a50d6f824e626be422a** compiled in [GitHub Linux Actions #38196555130](https://github.com/Hyperbog/RLOFT/actions/runs/38196555130) with **23/23 CTests PASS**. The exact source-only GitHub-built \`rloft_native_scene_frame_ranges_audit\` was run locally against the original \`LEVELS.DAT\`, \`INIT\`, \`NPCS.DAT\`, \`OBJECTS.DAT\` and \`RES3\`: **6,204 authored sources produced 42,429 legal RES3 frame references; all 1,723 distinct referenced original EH7 entries decoded and validated** (1,721 width64, one width96, one width128), with no invalid references. The source-owned family A NPC animation metadata +0x18..+0x1A, family B INIT variant/stride, family C OBJECTS stride/zero-stride, and exact owner-bank boundaries are retained. Native selection requires explicit variant/subframe inputs and never guesses live DOS animation/frame state.

Independent original-byte Python recomputation matched **32/32 per-level hashes and the combined range SHA256** \`a16781d0daeb1ab3aaa1a9f1b8a9a479fc5f5a52ad27ad5311c07a07d23bcf42\` with ZERO differences. Canonical \`code1-decompilation:SharedDecomp/verify_native_scene_frame_ranges.py\` plus negative tests passed [Research Actions #38196899337](https://github.com/Hyperbog/RLOFT/actions/runs/38196899337), **179/179** (39 ResearchInfra + 140 SharedDecomp). Original media never entered Git or Actions.

**Next engineering:** decode original live NPC/view subframe and \`2C294..2C6B6\` depth ordering + frame selection; integrate camera/geometry into existing safe VGA compositor, wall textures and palette, then certify real DOS screenshot/frame output. Range ownership is now verified but original live pose, depth sort, pixels and playable-game parity are NOT. Do not conflate host DDA with DOS raycast or link DemoV1 gameplay. Native Windows Actions remain manual-owner-controlled.

## CONTROLLING original CODE.2 36B14/369D8 VGA dispatch milestone — 22/22 native CTests and original-byte parity

[Complete verified source evidence and limits](ACTUAL_CODE2_TYPE7_VGA_DISPATCH_20261011.md). NativeEngine exact compiled source \`a7220cd0961a55584dc07ba955be18132589f801\` passed [Linux Actions #38194778519](https://github.com/Hyperbog/RLOFT/actions/runs/38194778519) **22/22 CTests**, with the new independent native code \`native_type7_vga_dispatch.*\` and a new actual-source scene audit CLI. The actual GitHub-built binary was run locally against original **CODE.2/SINTAB/RES3**. Its **28/28 original original-DOS VGA mask triplets** and real EH7/SINTAB scale-126 grouped/individual diagnostic modes match an independent Python original-source interpreter: **101/101 source/display-plan check values, zero differences**. Both original-diagnostic modes yielded exact independent VGA plane digest \`01d87427b3ddbcfef2d107eecb9edff0e7c1244aa50f94ebf6db4b9151760a5a\`, with 3 vs 7 source-pass calls, 12 vs 28 masked stores and 28 actual plane writes each. The equal pixel digest applies only with explicit diagnostic ECX=0. Native nonzero-ECX test fixtures verify grouped/individual mode differences and all-or-nothing rollback.

The independent research comparator \`code1-decompilation:SharedDecomp/verify_native_type7_vga_dispatch.py\` at commit \`d80752689851e55ce781b7c41c89dda6ab90c305\` passed [Research CI #38195105393](https://github.com/Hyperbog/RLOFT/actions/runs/38195105393), **169/169 source-only synthetic regressions**. Original media remains outside CI and Git. The full original DOS camera/depth/scale/frame selection, EH6 texture/PAL VGA compositing, actual screen-frame fidelity and game loop/AI/events/save/Windows current build **remain open**. Next: original \`2C294..2C6B6\` emission/visibility/draw-call-to-resource binding and 3D scene projection, then original full DOS frame reference comparison. Do not link DemoV1 game runtime, repeatedly default to the obsolete Level22 demo, or trigger Windows release Actions without proper owner approval.

## CONTROLLING original SINTAB native raster milestone — source and isolated pixel output verified

[Authenticated native SINTAB / EH7 safe raster verification](ACTUAL_ORIGINAL_SINTAB_SAFE_RASTER_VERIFIED_20261011.md). The exact NativeEngine source **a3bc4e24fb18d0c7988dbe1ea261276def6df42d** compiled in [source-only Linux Actions #38194123968](https://github.com/Hyperbog/RLOFT/actions/runs/38194123968) with **21/21 CTests passing**. Source-only SharedDecomp `verify_native_sintab_raster.py`, commit **0a7290fcf2832dfbf8693874b7e3663d1a4da908**, passed [research Actions #38193878447](https://github.com/Hyperbog/RLOFT/actions/runs/38193878447) with **159/159 tests**.

The GitHub-built C++ source was run locally against the owner's authentic original **SINTAB** and **RES3**, without importing bytes into CI. An independent original-source Python implementation reproduced **127/127 code blocks**, **1,282/1,282 block/aggregate typed values**, **834,255/834,255 sample-scale EH7 selector plans**, the complete six-opcode source grammar, and all 11,587 reads / 23,910 VGA store opcodes. **Zero differences**. The bounded portable host raster interpreter additionally consumed an authentic source shape-0 first-column descriptor at scale 126 and produced **one source read and four masked plane stores**, matching a separate Python interpreter's complete VGA-plane SHA256 **e0b7360b0fd1615974122a5a451993b248978affed73333c000bf394ec613ddf**. No DOS x86 is executed.

**Boundary:** Verified original source microcode parsing, selector plans and one diagnostic indexed-pixel execution; NOT original full-frame raycaster/scene projection, shadow/ECX register setup, original SINTAB patch-jump paths, VGA grouping, color palette/EH6 texels, sprites in the live compositor, DOS frame-pixel parity or playable gameplay. Next: hook the proven CODE.2 view-call setup and original resource selection into the safe interpreter, validate complete DOS-reference frames. No DemoV1 game runtime linkage; Windows release Actions remain manual-only.

## CONTROLLING RES3 full-original EH7 corpus milestone — 4,234/4,234 source shapes

[Full authenticated original RES3 validation chapter](ACTUAL_ORIGINAL_RES3_EH7_4234_VERIFIED_20261011.md). The owner-provided RES+OTHER.zip now supplies the complete authentic RES3, confirmed by SHA256 and Git blob identity against DnD-Classics/MASTER. Exact compiled native source **5b50e948b526f54f70c10d6cb5020d09ca13caa0**, [Actions #38192524579](https://github.com/Hyperbog/RLOFT/actions/runs/38192524579) **20/20 portable CTests passed**. That exact GitHub-built C++ executable was run locally against ALL **4,234 original EH7** shape entries: **4,234/4,234 LZSS/geometry/pointer/descriptor decodes passed**. An independent Python decoder confirmed 4,234/4,234 compressed source SHA256 and 4,234/4,234 decoded-body SHA256, **38,106/38,106 per-shape typed fields with ZERO differences**; 123,294 unique referenced column lists and 278,085 raster descriptor occurrences. The complete source is 7,600,070 decoded bytes. No original bytes were committed to Git or uploaded to CI.

Canonical independent source verifier SharedDecomp/verify_native_res3_type7.py at code1-decompilation SHA **e24e8b2587fbbd068fd48ffc856ca5489af34ff2**, [research Actions #38192758031](https://github.com/Hyperbog/RLOFT/actions/runs/38192758031) **149/149 original-free tests passed**.

**Exact completion boundary:** original RES3 4,234-entry native format decoder/data identity VERIFIED; original renderer color/shape projection/frame parity, EH7 SINTAB rasterizer behavior, CODE.2 view subframes, authentic EH6 texture/palette rendering, actor/event gameplay and current Windows/MSVC demo NOT verified. Next work is the source-proved portable renderer and DOS reference frame trace, **not** more source-file acquisition. The original Windows release workflow remains owner-manual. Keep independent DemoV1 game runtime unlinked.

## CONTROLLING authored scene-source milestone — 6,204/6,204 authentic original source bases

[Original-world source instance verification chapter](ACTUAL_ORIGINAL_SCENE_6204_RESOLVED_20261011.md).
NativeEngine source \`5bffd3a3e53a06842d27fbe5c63d574acbc9babd\`
compiled with [Linux Actions #38191784044](https://github.com/Hyperbog/RLOFT/actions/runs/38191784044)
**20/20 CTests passing**; shared research source
\`47cc267de4ca5ea250c8cc9082f1a0dc189811e8\`
passed [source-only Actions #38192139467](https://github.com/Hyperbog/RLOFT/actions/runs/38192139467)
**139/139 regressions**. The real original \`INIT\` (exact Git-blob
verified from DnD-Classics/MASTER), plus original \`LEVELS.DAT\`,
\`NPCS.DAT\`, \`OBJECTS.DAT\`, was read locally by **the actual
GitHub-built native C++ executable**. Independent Python produced
**32/32 exact matching per-level resolved-source SHA256** and the
entire original-source scene-candidate digest:
\`aeb7e334ba23bc114d5444617451495c4bda238d772343f597a9e3b1755d348c\`.
This verifies 1,258 NPC + 4,324 placed + 622 loose original world
records, their CODE.2 source-class-resolved RES3 **BASE** IDs,
positions, and the single original type 0x54→0x58 item alias.

The C++ native EH7 display-list decoder and INIT four-bank source
catalog are compiled and synthetic CTest-verified, but **actual original
RES3 content has not yet been passed through that new binary**; RES3
is present in DnD-Classics Git but not local test media. That complete
4,234-entry live decoder run is the next exact-file acceptance gate.
Neither original view/animation subframe nor SINTAB x86 rasterized
pixels are verified. The host raycast diagnostic must not be called
the finished DOS engine. Do not link DemoV1 game runtime, commit
original payload data, or trigger manual-only Windows release workflow
without owner authorization.

## CONTROLLING verified native scene view/resource-header milestone — 288/288 original sections (2026-10-11)

[Evidence chapter](ACTUAL_SCENE_HEADER_288_AND_PROBE_20261011.md).
Exact compiled Linux SHA \`0850d312a76a52715eec459063a47b393396382d\`;
[Actions #38190025508](https://github.com/Hyperbog/RLOFT/actions/runs/38190025508),
**18/18 CTests PASS**. New independent NativeEngine now retains **all
676 original header bytes per map** (five 64-entry resource mapping
tables), resolves conservative original-backed resource candidates,
and has a bounded camera movement / 320-column raycast **diagnostic
host probe**. The latter uses original level topology and proven
normal collision but **is not the original DOS traversal, projection
or renderer**; its artificial RGB debug image is *not game artwork*.

The actual CI-built native executable was executed locally against
the owner's authentic \`LEVELS.DAT\`. Independent original byte parser
and actual native SHA-256 output match **288/288 map section digests**
(32 maps × complete header, cells, 10-byte controls, events, automap,
annotations and each A/B/C record family). New header-only source
SHA-256 matched **32/32**. This required a real-world malformed JSON
separator fix; the fixed compiled executable now emits valid JSON.
The canonical comparator is
\`code1-decompilation:SharedDecomp/verify_native_world_fingerprints.py\`,
which now requires the header digest too; [research CI
#38190191412](https://github.com/Hyperbog/RLOFT/actions/runs/38190191412)
passed **129/129** synthetic source-only tests.

**Next controlling work:** replace host DDA with original CODE.2-proven
orientation/traversal, bind validated RES3/EH7 and SINTAB microcode
semantics safely to the renderer, EH6 textures/VGA palettes, then
integrate animation, true party motion, NPC/event VM and SAVE
compatibility. No original media is committed; current Windows/MSVC
build and field/gameplay/visual parity remain unverified, and Windows
Actions stay manual-only. Keep independent \`demo-v1\` game runtime
unlinked.

## CONTROLLING native persistent-world milestone — actual original 32-level runtime verified on Linux

[Compiled source and full 32-level normal-collision evidence](ACTUAL_ORIGINAL_RUNTIME_32_LEVELS_VERIFIED_20261011.md).
At exact native source `20543d8fd3a6c61aad7c886d6d1699f1fbf07005`,
[Linux CI #38186427291](https://github.com/Hyperbog/RLOFT/actions/runs/38186427291)
passed **17/17 portable CTests**. New `NativeWorldRuntime` keeps 32 level
states, mutable 16-bit cell/automap and original ten-byte control records,
source-hash-bound snapshots, evidence-backed ordinary/dynamic collision,
kind-1 animation tick and bounded renderer-input marked-cell inventories.
The **actual compiled GitHub Linux runtime auditor** was independently
checked against the owner's original `LEVELS.DAT`, and exact source-level
control/normal-collision outcomes match all **115584 cells** (87983
normally passable, 26857 ordinary blocked, 744 special blocked; 745 special
centers, 324 kind-1 dynamic records). Python independently reproduced the
**full retained-world source SHA-256**
`a8232782ff8654f9b6c6651300b3fbb2235e58d2fadf61fe2221e11dbbc8bdcd`,
including cells, controls, A/B/C records, all event slots, automap and
annotations. Canonical `SharedDecomp/verify_native_world_runtime.py`
adds reusable cross-language gates, with [research CI
#38186651456](https://github.com/Hyperbog/RLOFT/actions/runs/38186651456)
**124/124 source-only tests passing**.

**Completion boundary:** fully decoded original world + native
state/collision/animation primitives YES; **not** proven player traversal,
original raycaster, graphics projection/occlusion, event VM, actor AI, save
compatibility, Windows/MSVC current build, or playable full game. The
Windows release workflow remains manual-only. Keep separate `demo-v1`
out of independent native runtime linkage. Next priority: first-person
ray traversal/RES3-SINTAB/EH6 renderer and the authenticated original
event/movement behavior, not additional generic JSON audit tools.

## CONTROLLING SOURCE-ONLY MILESTONE — actual NativeEngine byte round-trip VERIFIED on Linux (2026-10-11)

**Exact compiled C++ source:** `4676ebf50668480512861c9fd981d6e36402da82`.
**GitHub Actions Linux source-only run [#38185185120](https://github.com/Hyperbog/RLOFT/actions/runs/38185185120): 16/16 CTests PASS**, including the native bank and new world-payload tests. The full portable engine sources compile. A prior Linux SIGILL was traced to an EH2 font validator falling off a `bool` function; the explicit non-inert return repaired genuine undefined behavior, without disabling the test. The manually authorized Windows/MSVC workflow remains unchanged and **has NOT** built this newer revision.

The two GitHub-built Linux auditors from **that exact source** were downloaded and executed locally on the owner's real `LEVELS.DAT` and `SINTAB`, each pinned to its SHA-256. NativeEngine produced a complete 32-level world report (115,584 cells, 1,659 controls, 394 event slots / 555 commands, 127 SINTAB scale blocks), and its native typed-world payload round-trip returned `decoded_section_roundtrip_equal=true`. A separately implemented Python original-byte oracle independently verified **256/256 source section SHA-256 hashes**: all 32 levels × packed cells, controls, event slots, automap, annotations and the A/B/C compact families. Full original `LEVELS.DAT` SHA also matched between the independent reader and native executable. This supersedes the prior unbuilt source-only parity note for **world data preservation**.

The local isolated source directory included copies of the authentic `LEVELS.DAT` and `SINTAB`, plus **empty synthetic INIT, RES0 and T signature files solely to satisfy NativeMedia directory recognition**. This is *not* a validated playable original installation; no game input was uploaded to CI, shipped in the executable ZIP, or committed. The verification evidence is retained as the local user-facing report `rloft_native_original_sections_256_verified.json`; source and binary SHA/provenance details are recorded in [the milestone documentation](ACTUAL_ORIGINAL_WORLD_256_LINUX_VERIFIED_20261011.md).

**Project completion boundary:** Compiled Linux content decoder and byte-exact source preservation YES; current Windows/MSVC executable validation, original DOS runtime execution, 3D renderer/texture parity, AI/NPC/event VM and playable independent native game NO. The independent `demo-v1` gameplay implementation must never be linked into NativeEngine.

## New typed-world SHA-256 round-trip gate — source only, not yet a Windows build

[Full report contract and implementation](WORLD_FINGERPRINT_COMPARISON_20261011.md):
\`native_world_fingerprints\` now reconstructs the **full 32-level typed
world source sections**, verifies each decoded section byte-for-byte
against the user-owned original image, and emits **256 section SHA-256
hashes** across cells, controls, all event-slot bytes, automap,
annotations, and the three eleven-byte families. The new
\`rloft_native_payload_audit\` is a fourth internal C++ executable
configured in CMake, not yet a certified/packaged Windows binary.
Existing \`native_world_payload_tests\` now include three standard SHA-256
vectors and structural mutation rejection. **Current native-engine
HEAD has not been compiled or CTest-certified on Windows.**

The independent canonical \`SharedDecomp/verify_native_world_fingerprints.py\`
compares these native hashes against independent original-file parsing.
On **actual owner-supplied original bytes**, an isolated locally compiled
C++17 SHA implementation and independent Python \`hashlib\` agreed on
**385/385 hashes** (256 per-level sections, 127 SINTAB blocks, two full
original input files); four separate local byte-mutation controls were
all detected. This validates SHA/corpus evidence, **not** the exact
\`native-engine\` source compile or native gameplay. Canonical research CI
passed **106/106 source-only tests** including native-shaped synthetic
fingerprint comparison. Windows Actions remain manual-only, requiring
explicit owner authorization. Next is compiling the exact native source
and independently comparing the real \`rloft_native_payload_audit\` output,
then integrating verified world state into first-person simulation/rendering.

## Typed original-world payload continuation — SOURCE ONLY, UNBUILT

[Original-world payload retention](WORLD_PAYLOAD_RETENTION_20261011.md) at native
commit `19ada509fe03c5718a49cc6ce08b2154f5c233e2` adds
`native_world_payload`: all 32 original packed cell grids, column-owned
10-byte controls, complete 11-byte world families, full event-slot data and
automap/annotations, plus isolated mutable cell/automap copies. The standalone
`native_world_payload_tests` is registered as the **17th CTest**. This is a
real implementation step toward native simulation, not full gameplay or DOS
execution parity. **No build/CTest of this newer source has been run**;
the last certified MSVC artifact remains pinned to original
`44cc2ccc950aa686d6563b9eda6cd804b1b08cf8`. Do not trigger native
Windows Actions without explicit owner approval. Independent source-byte
fingerprints are available under canonical
`code1-decompilation/SharedDecomp/original_world_oracle.py`; native decoded
payload bytes are not yet compared against those hashes.

## Whole original RES + LEVELS + SINTAB content system: WINDOWS VERIFIED

[**Canonical whole-content audit / MSVC certification**](WHOLE_CONTENT_WINDOWS_CERTIFICATION_20261011.md) supersedes individual resource-preview checkpoints. Real Windows x64 MSVC [Actions run #38174139897](https://github.com/Hyperbog/RLOFT/actions/runs/38174139897) compiled exact native source `44cc2ccc950aa686d6563b9eda6cd804b1b08cf8`, passed **16/16 Windows CTests**, then successfully decoded/validated **12,562/12,562 real source RES0..RES12 records**, all known EH families including 8,362 compiled shape display lists, all **32 original world records / 115,584 packed cells / 1,659 controls / 555 active event commands**, and all **127 SINTAB renderer scale packages** from `DnD-Classics` owned original media. New independent C++ all-bank/world systems and their fail-closed native command-line auditors are distributed with the GUI, not the decomp demo. The six-file asset-free ZIP and all **three Win64 executables** were downloaded and SHA256 verified; no copyrighted original files were shipped. **This is not yet native gameplay, scene rasterizer parity, active event VM or audio/cinematic playback.** Next work must be integrated first-person level simulation and rendering, not more one-record previews.


## Original RES archive categories and native world textures — 14/14 MSVC verified

[**Archive type classifications and EH6 Windows build**](RES_ARCHIVE_TYPES_AND_EH6_WINDOWS_20261011.md): The user observed no graphics in RES3/4/5/6/10 because the earlier browser was **EH1-only**; the original shipped corpus shows RES3=4,234 EH7 compiled shapes, RES4=667 EH6 first-person textures, RES5=98 EH3 XMIDI, RES6=266 EH4 VOC sound effects, RES10=219 EH4 VOC voices. **These archives were not broken or missing.** RES9 also holds 517 EH1 graphics but was omitted from the selector. Independent native update now decodes **RES4 EH6 32×128/64×128 LZSS/column-major textures**, browses RES9 EH1 graphics, and explains why RES3 geometry/SINTAB and sound archives cannot be displayed as EH1 sprites. [Windows MSVC run #38172310617](https://github.com/Hyperbog/RLOFT/actions/runs/38172310617) compiled SHA `744036693e84619d136d69dab157e9de25a2086e`, passed **14/14 CTests**, and the asset-free Windows ZIP/PE x64 EXE were independently SHA-verified. Real GOG visual texture browsing is pending; this does **not** implement DOS per-draw texture shading or EH7 scene rendering. No DemoV1 linkage.


## Original palette routing is per callsite and game state — Windows verified 2026-10-11

[**Contextual palette source/build audit**](ORIGINAL_PALETTE_CONTEXT_ROUTING_20261011.md): user-confirmed mixed original palettes within individual RES archives are **correct game design**. Per-record research in original CODE.2 and the existing project [palette router](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/Tools/anm/palette_router.py) establishes specific RES0 IDs using banks 0–4, 10, 11, 12 and 13, plus original world-level palette state and genuinely unknown context. Native `src/native_palette_context.cpp` independently models original **exact callsite / level context / preview-only / unresolved** decisions and the original 32-level bank table. The Windows RES inspector now defaults to `AUTO (source context)` and shows evidence labels while retaining manual 0–14 bank overrides; it does **not** claim every record has one immutable bank. Actual Windows MSVC [run #38170601091](https://github.com/Hyperbog/RLOFT/actions/runs/38170601091) compiled SHA `b1892e8448734f3ad8018a0a3911fe9ed59d582c`, passed **13/13 CTests** and uploaded an independently verified asset-free x64 Windows ZIP. Real original-GOG visual confirmation of this new AUTO routing is still pending; eventual native gameplay must model mutable VGA palette at each scene/render operation. The decomp demo remains a separate untouched project.


## Native indexed sprite color context — VERIFIED Windows code, GOG visual test pending

[**Original palette correction report (2026-10-11)**](WINDOWS_PALETTE_CONTEXT_CORRECTION_20261011.md): the prior RES browser applied **COLORS bank 12 to every sprite**, even though original gameplay screens use different banks (CODE.1 character creation bank 8, CODE.2 menu bank 12, initial logos 14/9). It also omitted CODE.2's original **19-index title cursor translation**. New independent `rloft_native_assets` code distinguishes raw RES indices from the original title cursor and exposes a **15-bank chooser**. Actual Windows x64 MSVC [run #38169819411](https://github.com/Hyperbog/RLOFT/actions/runs/38169819411) compiled source `1c9440303a5c83ff6834a3f87cfdf9afc8dbfa19`, passed **12/12 CTests** including asymmetric RGB/DAC6→BGRA color-channel and cursor remap tests, and uploaded a verified asset-free ZIP. This new visual fix is **not yet user-verified against original GOG sprite colors**; other resources can require other original palette semantics. Do not apply a universal R/B swap or inversion, and do not modify the separate demo.


## Verified independent native RES image browser — Windows 2026-10-11

**[MSVC / 11-of-11 Windows CTest acceptance](WINDOWS_NATIVE_RES_BROWSER_VERIFIED_20261011.md)**: the independent `rloft_native.exe` now has its own native EH1 raw/LZSS decoder, original INIT/RES0 cursor resolver, and **RES0–RES8/RES10 numeric image browser** with previous/next image navigation. Windows Actions [run #38168898735](https://github.com/Hyperbog/RLOFT/actions/runs/38168898735) compiled this specific source SHA `d29914503fb6d6bb3a65954257d0ac145f0ceaf0`, passed **11/11 CTests**, uploaded an asset-free ZIP, and its actual EXE/ZIP checksums and Windows AMD64 PE format were independently verified. Download [the archived Windows artifact](https://github.com/Hyperbog/RLOFT/actions/runs/38168898735/artifacts/11712864857). **The new raster browser is NOT YET USER-TESTED against the original GOG installation**, although earlier real original file indexing (19/19, 69 files, COLORS palette12) is verified in its own previous build. The decomp demo remains entirely separate; no original assets are distributed or extracted in bulk. Later documentation-only HEAD revisions are not this compiled source.


## First real GOG install verified; independent raster decoding underway

[**User's original GOG field test**](WINDOWS_REAL_GOG_ASSET_FIELD_TEST_20261011.md): the previously compiled independent native asset loader opened the actual original GOG `game.gog` as **ISO2048**, indexed **69 source files**, reported **19/19 recognized resources validated, zero missing/invalid**, and displayed the **original COLORS palette 12**. This is real installation-backed **structural validation**, not proof of full sprite/cinematic/world/gameplay decoding.

[**Native EH1 raster milestone**](NATIVE_EH1_RASTER_MILESTONE_20261011.md): the next independent source implementation now decodes raw and original LZSS-4K compressed RES EH type-1 images, resolves the original default cursor from INIT/RES0, displays a native GDI original-sprite preview when available, and adds `--verify-raster` with separate failure codes. A new native corruption/LZSS/selection test brings Windows **required registered CTests from 9 to 10**. **These latest source changes have not been MSVC compiled or tested against the owned GOG installation; the prior 9/9 verified build corresponds only to `fd314b89`.** No decomp demo code is linked or original assets distributed.


## VERIFIED Windows x64 independent native asset build — October 11

**[Download and verification evidence](WINDOWS_NATIVE_ASSETS_VERIFIED_BUILD_20261011.md):** `rloft_native.exe` was compiled on Windows Server 2022 using **MSVC 19.44**, all **9/9 Windows CTests passed**, and the asset-free ZIP was downloaded and independently checked. [Actual GitHub build #38164976411](https://github.com/Hyperbog/RLOFT/actions/runs/38164976411). **Exact compiled Git revision:** `fd314b89e96db07c739176d25f1fc9be94ecaa19` (later README/docs HEADs are not compiled revisions). Native engine is not the demo recomp, and this build does **not** implement gameplay. First **real GOG asset-loader field test remains pending**, although an older diagnostic bootstrap did display real GOG menu artwork. Primary output: `RLOFT-NativeEngine-Windows-x64-ASSET-PREVIEW.zip` (no original files). ZIP SHA-256: `5ee2cdb58f3aa0ea10c715a2841b6be3eb0f411abb957132c4aa1150a857713c`.


**Authoritative direction (October 11, 2026):** [Native asset runtime architecture](ASSET_RUNTIME_ARCHITECTURE_20261011.md).

This is a NEW Windows replacement game engine, conceptually like OpenMW/KEX. It reads the player's **original game assets** and will eventually implement its own graphics, audio, game-state, world and gameplay systems. It is **not** the original-source decompilation/recompilation and does **not** link or execute the `demo-v1` game runtime. The working decomp demo continues as an independent project and can supply research/evidence about original formats and behavior.

## Active implementation — asset loading

- Native, read-only original media backend [`native_media.hpp`](include/rloft_native/native_media.hpp), [`native_media.cpp`](src/native_media.cpp): index DOS loose files or GOG `game.gog` ISO9660 (2048-byte data sectors, 2352-byte raw sectors with 16/24 byte user offsets), including nested asset folders, and **read original resources lazily** without unpacking or rewriting the disc image.
- Independent original format decoders [`native_assets.hpp`](include/rloft_native/native_assets.hpp), [`native_assets.cpp`](src/native_assets.cpp): RES offset tables/entry lookup and original `COLORS` 6-bit DAC palette banks. Report the presence, byte size, archive entry count and structural validity of 19 key assets.
- Windows desktop entry [`win32_native_assets.cpp`](src/win32_native_assets.cpp): **independent `rloft_native.exe`**, original-install folder picker, read-only inventory and original COLORS palette12 display. Optional headless `--verify-assets <owned install>` for actual original-media validation.
- Existing native, renderer-neutral indexed-frame, VFS mod mount/precedence and proof-contract foundations remain independent of the original runtime. No original game media or private saves are included in distributed outputs.
- **No full unpack/repack is required at runtime.** A disposable, versioned derived-resource cache can be evaluated later if profiling justifies it. Do not repack the game.gog or change installed originals.

### Windows-first build

The next **manual-only** Windows build is [Native Engine Original Assets — Windows x64](https://github.com/Hyperbog/RLOFT/actions/workflows/native-engine-assets-windows.yml). It runs [`Build-NativeAssets-Windows.ps1`](Windows/Build-NativeAssets-Windows.ps1), builds the independent native reader, runs at least **nine** isolated native CTests (loose files, ISO sector variants, nested original directories, RES archive/COLORS decoding, native VFS/renderer contracts), and prepares an asset-free source-identified ZIP.

**Status:** this current independent asset engine source has been **committed but NOT yet compiled or executed under Windows MSVC**, and its real GOG asset validation is **NOT yet field-tested**. It must be built and checked against the user's original `game.gog` before acceptance. No Windows CI run has been dispatched for this revision. The previous Windows bootstrap's genuine **15/15 CTests** and the user's screenshot of its original static menu are real historical results, but that older bootstrap linked inherited decomp codecs and is **not proof that this new native engine loads assets**.

### Intended acceptance sequence

1. Compile and test the *independent* Windows `rloft_native.exe` on MSVC x64.
2. Open the user's original GOG/DOS install; validate actual `RES0`–`RES10`, `INIT`, `COLORS`, DAT and ANM assets and inspect original palette output. Keep the install read-only.
3. Add native, independently implemented RES EH sprite/texture/geometry, ANM cinematic, music/SFX and DAT world decoding; build a typed asset manager with resource-ID/version provenance and optional derived-data cache.
4. Implement original-source-faithful game semantics in **new** simulation code, original boot/world/input/save flows, and Windows rendering, comparing against evidence from DOS and the separate decomp demo without using either as a runtime dependency.
5. Establish gameplay fidelity/normal-world acceptance; then add renderer, modding and editor capabilities without changing original rules.

## Continuity and archival evidence

- [Current design and decision](ASSET_RUNTIME_ARCHITECTURE_20261011.md)
- [Native-engine developer instructions](AGENTS.md)
- [Windows native bootstrap build 15/15 (historical)](WINDOWS_VERIFIED_BUILD_20261011.md)
- [Real GOG installation static artwork test (historical bootstrap)](WINDOWS_GOG_FIELD_TEST_20261011.md)
- [Original CODE.1/CODE.2 decomp continuity](../Code1/DECOMPILATION_CONTINUITY.md) — **reference evidence, not native production dependency**
- [Native engine playable/runtime objective — Issue #9](https://github.com/Hyperbog/RLOFT/issues/9)
- [Native GPU comparison — Issue #10](https://github.com/Hyperbog/RLOFT/issues/10)

**Superseded:** `DEMO_BOOT_PIPELINE_REUSE_20261011.md` and `LIVE_ORIGINAL_RUNTIME_INTEGRATION_20261011.md` document an abandoned recompilation-based native integration and MUST NOT guide current implementation.

## Optional shared REA research adapter

[Optional shared REA native-binary analysis](REA_ANALYSIS.md) is available through the canonical `code1-decompilation:ResearchInfra/` checkout. It is local, read-only static analysis and **does not** install REA into this application's runtime, alter original game media, or certify original DOS/gameplay parity.
