# NativeEngine — Active developer/agent instructions

## CURRENT A5 — original RES1 item icons in first-party F7 inventory (2026-10-13)

[Original object UI EH1 RES1:168..335 decoding and GL-backed native inventory art](ALPHA_A5_ORIGINAL_RES1_INVENTORY_ART_20261013.md). **F7 now presents true original type-1 sprite art** from each occupied actor CHARS item via the exact `OBJECTS.DAT +0x2D` and `INIT[+0x02]` RES1 mapping, with source-authentic index transparency and Level22 COLORS world bank4. No fake replacement for intentionally empty source slots. Uses OpenGL1-compatible power-of-two padding; no source pixels resampled. New native `native_party_item_icon_tests` expects **43 Windows CTests** and a whole-168-slot **real original media audit** checking actual CODE.1 ten starting-item image links. **This remains a frame-0, context-palette inspection preview**, not recovered CODE.2 inventory-modal frame/DAC parity, item manipulation or gameplay. Exact-source Windows CI acceptance is pending [PR #22](https://github.com/Hyperbog/RLOFT/pull/22). Pre-Alpha.

## CURRENT A4 — original CODE.1 party equipment inspection (2026-10-13)

[21 authentic CHARS slots per actor, original 336-record OBJECTS.DAT provenance and F7 party inventory inspection](ALPHA_A4_SOURCE_PARTY_INVENTORY_INSPECTION_20261013.md). The native first-party Level22 GPU session now decodes two generated actors' **real 0x248 CHARS equipment item IDs at +0x40**, `+0x12B` known bits, 336 immutable `OBJECTS.DAT` names/categories and original RES1 icon IDs. **F7** toggles a read-only inventory panel showing 21 slots for both actual actor names. Native original-media startup audit independently verifies original object6 QUIVER and object12 LEATHER ARMOR with source IDs and icons, **and source-generation of two genuine CODE.1 starter rows (5+5=10 items) through the complete 0x920 CHARS ABI into the native inventory decoder**. New 42nd native CTest and Windows packaging gate. **No actual inventory use, moving, pickup, AI, combat or source sprite frame parity; A3 checkpoint stores original CHARS only.** Exact-source [Windows CI #109](https://github.com/Hyperbog/RLOFT/actions/runs/38475876241) **MSVC x64 42/42 CTests PASS**, original CHARS/OBJECTS 10-item source audit and unchanged original owner media PASS; [latest media-free A4 Windows package](https://github.com/Hyperbog/RLOFT/actions/runs/38475876241/artifacts/11854505539). QA PR #21 merged, earlier audit-fixture failure resolved. Actual user Windows GPU/GUI field-play, sprite-frame parity and gameplay item interactions are **still unverified**.

## CURRENT A3 — source-bound Level22 quicksave and cross-launch continuation (2026-10-13)

[Native `RNATSV01` profile format, F6/F9, launcher Continue and `--resume` CLI, validation boundaries, and live QA status](ALPHA_A3_LEVEL22_NATIVE_QUICKSAVE_RESUME_20261013.md). First-party Level22 now serializes **unaltered CODE.1 CHARS + 32-level immutable source-world SHA + real host camera + discovered cells** to a native-only `%LOCALAPPDATA%\RLOFT-Native\Level22-Quicksave.rnp`, written atomically outside any DOS asset directory. **F6 saves, F9 restores same party; launcher Continue Native Level22 Quicksave and `--resume <install>` can restore after reopening the executable**. Decode rejects bad SHA, wrong original world/CHARS, invalid camera, blocked terrain and malformed bytes *before* mutation. The separate portable `native_session_profile_tests` makes 41 expected native CTests. **Not a complete original DOS SAVE.DAT/LEVnnn schema, nor persisted NPC/script/inventory/quest/combat state, nor a playable Alpha.** Windows CI #89, #94 and #95 confirmed MSVC native builds and **41/41 passing CTests** including the CODE.1 original-party cross-launch native save regression and original-owner source-media audits. [Latest media-free Windows build artifact](https://github.com/Hyperbog/RLOFT/actions/runs/38454664776/artifacts/11841884517). Actual interactive GUI save/exit/relaunch/Continue field validation is **still unverified**.

## LATEST A2 — original RES3 EH7 base-shape probe; first Windows 40/40 (2026-10-13)

[Exact original Assassin RES3:315 source, actual Windows CTest evidence and unresolved live sprite-state gates](ALPHA_A2_ORIGINAL_RES3_BASE_PROBE_AND_WINDOWS_CI_20261013.md). Original first-party Level22 **F** inspection now resolves genuine INIT/NPCS/OBJECTS-authored base frame allocation and decodes its original compressed EH7 geometry, original/decoded fingerprints, width/clip bounds and SINTAB descriptor count, displayed legibly. This is a base diagnostic, **not actual sprite drawing or CODE.2 frame selection**. Original source type9 ASSASSIN is RES3 ID **315**, EH7 64w x15..51 y27..128 and NPCS stride/cycle/span **8/4/3**. GitHub Actions PR #16 run **38442392623** confirmed **Windows MSVC build and 40/40 CTests PASS**, original-media checks and media-free package; newer original Assassin RES3:315 proof also passed **Windows CI #83** (run **38443286600**), **40/40 native CTests**, original source audits and a fresh media-free executable artifact. Still **Pre-Alpha**; no GUI field acceptance, actor AI/event VM, pickups/combat, profile persistence or DOS image parity.

## LATEST A2 — Level 22 source-entity targeting, without fabricated interactions (2026-10-13)

[Original NPC9 16.16 record targeting and bounded host diagnostics](ALPHA_A2_LEVEL22_SOURCE_ENTITY_TARGETING_20261013.md). Source-authored Level22 placements now support **F: inspect nearest original record** using real fixed-point positions, bounded range/cone and ordinary-cell collision/LOS. The inspection HUD reports original NPC/placed type, RES3 base and compact-record ordinal, but never grants a made-up pickup, hostile AI, scripted dialogue or DOS-targeting parity. Portable `native_scene_target_tests` and authentic original LEVELS.DAT NPC9 targeting assertions are added to CTest/Windows original-media CI. NPC9 raw source record `09000d00007c0700007802` confirms (13.484375,7.46875), facing 2. **Exact-source Windows build/GUI testing, sprite frames, encounter VM, inventory, and saves remain open. Pre-Alpha.**

## LATEST A2 — real Level 22 authored placement ledger in party view (2026-10-13)

[Original Level22 source positions, typed scene bases, diagnostic-map binding, new regressions and open gameplay gates](ALPHA_A2_FIRST_LEVEL_AUTHORED_PLACEMENTS_20261013.md). Normal CODE.1-generated party now loads a source-authored **111-record Level22 directory** through original `LEVELS.DAT`/RES3/NPCS/INIT/OBJECTS: **NPC type 9 ×1, placed type62 ×52, type63 ×42, type64 ×16**. An optional **F5 diagnostic automap marker layer** shows discovered source-exact 16.16 positions, distinct from fabricated game sprites or original visibility semantics. Source provenance and mask filtering are verified only when exploration updates (not per GPU frame). Real original-media startup audit and Windows CI now gate the original counts and RES3 shape-base validity. **These dots are not sprite rendering, script interaction, AI, inventory or playable Alpha**. Exact-HEAD Windows build/test and owner GPU field acceptance are still **unverified**.

## LATEST A1 — first Level 22 source audit and build/UI repair (2026-10-13)

[Current original Level22 record audit and exact build limitations](ALPHA_A1_FIRST_WORLD_SOURCE_QA_20261013.md). Fixed C++ namespace qualification in native startup auditor; added original BootFlow-to-CHARS import regression, native title Up/Down/Enter and CODE.1 statistics keypad keys, transactional failed CODE.1 menu entry, and re-openable (fresh, not saved) original-party GPU session. Source LEVELS.DAT level22 (30×15) has exactly **10 kind-0 controls, 0 kind-1 controllers, 50 empty map-event slots, 11-byte source record families 1/110/0** and valid first cell (4,4). Do not synthesize a kind-1 door or map event. Isolated g++/Clang C++17 party-parser tests and independent Python real-file assertions passed; **full exact-HEAD Windows/MSVC CI, original-media Actions run, field test and playable Alpha are still unverified**. First-level event scripting is code-driven and needs source CODE.2 reconstruction.

## LATEST A1 — validated source CHARS party → Level 22 native entry (2026-10-13)

[Source-only acceptance, new party importer and first-world route](ALPHA_A1_SOURCE_PARTY_LEVEL22_ENTRY_20261013.md). Native CODE.1 completion now validates the original 0x920-byte `CHARS` through independent `NativePartyStart`, then enters the existing original-data GPU level at index 22, original start (4.5,4.5), heading 270°, with two source actors retained and the diagnostic level-cycle/noclip disabled. Media selection opens original cinematic/title/character startup by default; `--play <original-media>` is available, old inspection CLI retained. The first-world GPU view is **not** a completed CODE.2 gameplay session: interactions/events/placed actors/inventory, audio, profile saves and DOS parity remain missing. **Current-HEAD Windows CI compilation and owner visual acceptance remain unverified**; do not call this a first playable Alpha. Next obtain Windows CTest/runtime evidence and implement actual first-level gameplay from source records.

## LATEST A1 — native source CODE.1 stats/name and 0x920 CHARS in memory (2026-10-13)

[Exact original-media source audit and remaining acceptance gates](ALPHA_A1_CODE1_CHARS_IN_MEMORY_20261013.md). The existing DemoV1-derived CODE.1 character presenter, original statistics/roll/editor, name-input/font rules, two-actor completion and 0x920 CHARS serialization are now owned by neutral `Shared/StartupPresentation` and called by the independent native startup window via original RES1/RES7/RES8/CODE.1/OBJECTS.DAT/T assets. Native builds a **memory-only CHARS image** then stops at CODE.2 startup; there is still **no actual world/party runtime import or first playable Alpha**. Portrait hover and name blink are implemented; many original animated transitions/audio are not. Windows/Linux current-HEAD build/CTest and GUI acceptance are not yet confirmed. Do not describe this as fully accepted until exact new CI run evidence exists. Default startup is still the asset inspector, not canonical user game boot.

## CURRENT A1 — CODE.1 character opening and original actor/choice graphics (source checkpoint)

[Implemented integration and unaccepted Windows validation](ALPHA_A1_SHARED_CODE1_CHARACTER_PRESENTER_20261013.md). Neutral `Shared/StartupPresentation` now owns original CODE.1 presenter/palette/stats source once, consumed by DemoV1 and independent NativeEngine. Native CODE.2 title option routes into actual CODE.1 70-frame opening and source first actor/sex/race/class/portrait screens, using only original indexed RES1/7/8/T/CODE.1 resources through `NativeMedia`. Original stats/RNG, name input, 0x920 CHARS transfer and party/Level22 entry **remain unfinished**. New Windows source build/38-suite/original-media acceptance is **not verified yet**; never claim Alpha or DOS parity. The next task is CI/real-original acceptance of the current SHA, then original stats and CHARS. Avoid duplicate presenter implementations or a second native branch.

## CURRENT A1 character presenter resource handoff — source only

[Read-only CODE.1 visual source contract and open validation](ALPHA_A1_CODE1_CHARACTER_MEDIA_SEAM_20261013.md). The neutral `CharacterVisualSource` now underlies DemoV1's original character presenter/stats as well as `NativeStartupVisualSource`. Native media decodes source RES0/1/7/8 EH1 and bounded CODE.1 DGROUP on demand; the native source index is lifetime safe. New fixture regressions are authored but **not yet current-HEAD CI confirmed**. The interactive CODE.1 presenter still lives in DemoV1; move that original implementation and its source stats/remaps to Shared next, then integrate presentation/input and source CHARS, not a duplicate renderer. NO playable Alpha claim.

## CURRENT controlling handoff — Alpha A1 original CINE00/CINE12 movie integration

[Real-original CD ANM acceptance and exact x64 SHA](ALPHA_A1_ORIGINAL_CINE00_CINE12_20261013.md). Windows native startup now plays source-indexed `CINE00.ANM` (405 frames) and `CINE12.ANM` (91 frames), plus prior authentic logo/title panels; `DemoV1/src/anm_cinematic.cpp` was extracted into neutral `Shared/StartupPresentation` and forwards its former demo header, with no additional native loader. `NativeStartupVisualSource` implements original EH2 RES0 fonts and obfuscated T subtitles through `NativeMedia` and native LZSS. Original ANM byte-packet colors/frame deltas/timing preserved from shared Demo implementation. [Win64 #38346464375](https://github.com/Hyperbog/RLOFT/actions/runs/38346464375) **38/38 native tests**, *all 496 shipped CD-source frames* and original-media hash preservation PASS. ANM **13 sound commands decoded but no native VOC/XMIDI sound playback yet**. Audio, original fade parity, CODE.1 character UI and 0x920 CHARS transfer/party/world launch remain open A1; do NOT claim playable Alpha. Next implement sound bridge and CODE.1 presenter/CHARS. Never duplicate DemoV1 gameplay or make another native branch.

## CURRENT controlling A1: shared original CODE.2 rendered title window — 2026-10-13

[Implementation, limits and tests](ALPHA_A1_RES0_TITLE_MENU_20261013.md). The existing `DemoV1` `StartupPresenter` and indexed framebuffer were relocated to neutral `Shared/StartupPresentation` and used by **both** independent executables. DemoV1 now forwards the old header and links the shared source; NativeEngine uses its own original `NativeMedia` and EH1 decoder to provide RES0 pixels and COLORS DAC6. Native Win32 main window now offers a source-art CODE.2 startup button with 94/95/96 logos, 97 title and 98–105 original 14-step/menu graphics, source supervisor menu selection; character creation still correctly halts at the CODE.1 presenter boundary. [Win64 CI #38345217385](https://github.com/Hyperbog/RLOFT/actions/runs/38345217385) **38/38 tests**, 12 true original RES0 images and 18 original menu frames passed in read-only DnD-Classics game checkout; source RES0/COLORS/INIT SHA unchanged. **DO NOT describe this as full intro/playable Alpha**. Next: source-neutral original `AnmCinematic` plus RES0 EH2 font/T subtitle support, CINE00/CINE12 from CD media; CODE.1 `CharacterCreationPresenter` with original 0x920 CHARS transfer; native world/party session. No fork or additional native branch.

## CURRENT A1 checkpoint — original startup shared-source integration

[2026-10-12 source-identical extraction and dual-consumer build evidence](ALPHA_A1_SHARED_ORIGINAL_STARTUP_20261012.md). Neutral `Shared/BootFlow` and `Shared/CharacterCreation` now own the original DemoV1 state machines byte-for-byte. DemoV1 and the independent NativeEngine CMake both link these neutral libraries; no `../DemoV1/src` code is compiled into `rloft_native.exe`. Native media selection now runs the original CODE.2 `BootFlow` state and displays which path original `CHARS` presence would take. [Native CI 35/35](https://github.com/Hyperbog/RLOFT/actions/runs/38343564098) and [shared DemoV1 compatibility CI 2/2](https://github.com/Hyperbog/RLOFT/actions/runs/38343815405) PASS. The active separate `demo-v1` branch is unchanged. Next work: reuse **actual** DemoV1 `StartupPresenter`, `AnmCinematic`, `CharacterCreationPresenter` via a `NativeMedia`-compatible neutral resource boundary; do not fabricate character creation, ship DOS assets, or mark A1 complete until native user-facing launch and CODE.1→CHARS→CODE.2 works. Windows/MSVC compilation of this exact revision has not yet been verified.

## Controlling product roadmap

See [README](README.md) for the Alpha, Beta, and Final exit gates. Current GPU versions are technical previews, not a first playable Alpha. Next work is shared original startup, actual authored scenery and door behavior, party/world interaction, and safe profile saves. Keep `native-engine` as the sole native development branch. The A/D input handedness fix is in the shared portable movement function and both Win32 viewers. A related GPU mouse-look blocker was also found: collision validation rejected vertical pitches above 0.9 while GPU free-look supports up to 1.42 radians; the bound is now consistent with GPU input and covered by regression. These are native-source/portable CI findings, not yet a new Windows release.

## CONTROLLING GPU Alpha 0.12 — native-to-4K offscreen FBO and clean native branches

[Full 2026-10-12 v0.7 source comparison, Windows evidence, exact release SHA and deleted-branch audit](ALPHA012_V07_INTERNAL_RESOLUTION_CONTROLS_AND_BRANCH_CLEANUP_20261012.md). The owner explicitly corrected that F2 must cycle actual **internal rendering resolutions**, never window sizes. The WGL render-target source now implements exact old v0.7 five modes (native/auto; 1280×720; 1920×1080; 2560×1440; 3840×2160), real offscreen color+depth targets, aspect letterboxing, separate F11 fullscreen and optional 0/2/4× MSAA. Native GPU UI adds F1 controls, Shift sprint, M optional mouse with flipped horizontal yaw per owner's feedback, F3 3 texture filters, F8 150/210/300 panel sizes, F12 screenshot BMP, Tab minimap. The GPU error now shows actual C++ exceptions; the independent source audit checks **444 real RES4 GPU-use texture IDs on all 32 original maps**. Owner authorized and CI verified removal of **11 temporary native acceptance branches**; only `native-engine` remains among native branches. Preserve default `code1-decompilation`, original `DnD-Classics`, DemoV1 and disassembler/extractor branches. [Win64 run #38340993133](https://github.com/Hyperbog/RLOFT/actions/runs/38340993133) **34/34 native CTests**, source SHA provenance and original file preservation PASS. GUI/OpenGL physical user test remains OPEN, original CODE.2 VGA camera projection remains unfinished, v0.7 editor/GLSL crisp/per-object scenery still unported, first-playable milestone still OPEN.

## CONTROLLING Native GPU Alpha 0.11 — source-backed first-person geometry

[Source, 32-map real game audit, scene limitations and Win64 binary provenance](ALPHA011_ORIGINAL_GPU_WORLD_EXPLORATION_ACCEPTANCE_20261012.md). Primary GPU view is **NOT** the old fake-scaled-ray VGA view: real world-space wall and floor/ceiling geometry is built from original LEVELS source x/y banks 0/1 and 2/3, upper cell flag 0x0200, source floor/ceiling IDs, exact no-draw wall 147 (level22 outdoors), and original RES4+COLORS source bytes. OpenGL via native Win32 WGL; old CODE.2/VGA reference button remains. Default original-like WASD+Q/E, optional M mouse, F2 variable framebuffer resolution, N source texel display filtering, Tab session map; optimized tick/movement and map visibility. Windows original-media acceptance [#38336947761](https://github.com/Hyperbog/RLOFT/actions/runs/38336947761): **34/34 CTests PASS**, **32/32 source maps**, 38,419 lower/8,936 upper faces, 87,983 floor/76,761 ceiling faces, source files untouched and original-media-free x64 artifact. Exact build SHA `7994b45de282a3a981e3b10185af37e8b4e61128`, EXE SHA256 `6acef5b86ae684be66146b417672c7a83a1c771a70b5cd4156d865a66ea7b551`. NEXT **real scene fidelity** integration is INIT/RES3 placed scenery/paired trees including level22, original CODE.2 doors and stateful geometry, source sprite selection, original gameplay systems and DOS captured-frame parity. Real player GPU GUI field test is open. Do not label game complete or delete CODE.2 VGA reference engine.

## CONTROLLING Alpha 0.10: user screenshot wall material/collision and v0.7 input migration (2026-10-12)

[Original file witness, user-provided old v0.7 viewer comparison, source changes and accepted CI](LEGACY_VIEWER_V07_NATIVE_VIEWPORT_MIGRATION_20261012.md). Canonical engine source `native-engine:93cd779754e8d091b3ac20fb2f75949c17907cd0`, one-off Windows compiled `bbe2d8be4931ec2a7677d48a111a081096f3ef71`, [Windows #38335193771](https://github.com/Hyperbog/RLOFT/actions/runs/38335193771) **33/33 CTests / authentic 32/32 LEVELS views PASS**, 2,029 actual upper tier walls, 160 unique RES4 textures, media SHA unchanged. Correct static wall layout uses base+upper source axis bank 0/1 for x-normal and 2/3 for y-normal, cell upper flag 0x0200; original screenshot level1 cell14,10 has [497,62,499,62] and upper source rows96. Optional mouse-look + continuous W/A/S/D/Q/E now sampled on WM_TIMER, no event-per-redraw recentering; explicit top-down crop instead of GDI input-subrect source, Tab session automap, +/- zoom, N display filtering. The old Python/ModernGL v0.7 code and bundled licensed original archives were inspected locally ONLY, never copied to production. **Recommendation:** future SDL3+SDL_GPU or bgfx high-resolution mesh backend behind the still-required exact CPU CODE.2 VGA reference path. Input feel and original 34A74 floor/sprite geometry parity are NOT user-confirmed. Do not call Alpha 0.10 DOS faithful/first playable or create unrelated reconstruction tests.

## CONTROLLING Alpha 0.9 visual regression correction — 2026-10-12

[Exact CODE.2 1F317/1F388 original upper/lower RES4 selectors, host perspective and mouse-look, and 32-map Win64 acceptance](ALPHA09_AUTHORED_SURFACES_PERSPECTIVE_MOUSE_ACCEPTANCE_20261012.md). Canonical native C++ source `f006532b78639ae7fcf9eaa23326767a03b133f2`, isolated compiled Win64 checkout `ab3a0fc52cd625b86b52c08ea6ff852678927323` (one-time workflow trigger only), [Windows #38330375936](https://github.com/Hyperbog/RLOFT/actions/runs/38330375936) **33/33 PASS**, real original **32/32 levels** using **987,256 floor +842,872 upper authentic narrow EH6 source texels**, 137 unique decoded surface/wall textures, 7 original files unchanged, ZIP/executable hashes independently verified. Fixed wrong 320x400 square-host-pixel stretch by DOS 2× horizontal display aspect and zoomed original 256×256 view; replaced `55/d` with host projection focal/d; restored authored header +5/+7 original resource IDs with background-only for negative upper selectors; replaced WM_INPUT with optional focus-bound centered WM_MOUSEMOVE (M/right-click toggle OFF by default, Q/E keyboard turning). **Do not misreport original CODE.2 34A74 polygon/UV, original full frame or user GUI mouse feel as verified**. Existing first-playable milestone stays open; no extra unrelated reconstruction suites or copyrighted media in output; normal Windows release manual-only.

## CONTROLLING Alpha 0.8: host collision, real CODE.2 environment + optional mouse — 2026-10-12

[Source, bug repro, exact original FE94/header background, code and Windows validation](ALPHA08_COLLISION_BACKDROP_MOUSE_CONTROLS_20261012.md): source `native-engine:2b54a176c3a60b0458a06fb11cbd9f6b35357522`, isolated verified x64 binary `f80fcc2202ff33cc88c1feedb6c8b8dbba7c79dc`, [Windows Actions #38328986042](https://github.com/Hyperbog/RLOFT/actions/runs/38328986042) **33/33 tests PASS**, authentic original **32/32 levels**, **97** unique source textures and original seven-file hash preservation. Alpha 0.7's wall stretch was host point-radius approach + 400/d perspective saturation; new swept radius 0.26/axis slide, bounded 55/d host perspective, freeze on fully blocked movement, and exact CODE.2 33E28/33DE0 sky/floor fills mapped to first-person x32..287/y4..259. User controls now W/S walk, A/D strafe, Q/E turn, M toggle *optional* Win32 raw yaw/pitch, arrows no longer turn, default pitch fixed. **Wall dual-word/face chooser, DOS camera/frames, sprites, actor/gameplay and live mouse UI field acceptance are OPEN.** Normal Windows release workflow stays manual-only; no proprietary source media enters Git or distro. Do not add unrelated reconstruction tests; next targeted subsystem is correct original CODE.2 live scene wall selection.

## CONTROLLING Alpha 0.7 source-backed original VGA window — 2026-10-12

[Verified CODE.2 original byte ranges + Win64 real-media acceptance](ALPHA_CODE2_QUADRANT_VGA_WINDOWS_ACCEPTANCE_20261012.md). Canonical source checkpoint `d818b6424f9c5f30ed79072f2d72fd539e2a6938`, Windows isolated compiled SHA `2f5937c9c6a5d60f6264e52ca17e4662b1c84ecc`, [Windows #38327641005](https://github.com/Hyperbog/RLOFT/actions/runs/38327641005): **33/33 CTests PASS**, authentic **32/32** original maps textured using CODE.2 `338C8` fixed-point sampler, source quadrant signed-step/mirror decisions, 320×400 original four-plane VGA + DAC6 palette. Exact source-backed 10,097 wall columns / 1,167,450 texels; original six-file fixture unchanged, no owner media packaged. New live Windows preview image uses **host** camera angles/DDA, estimated wall face/depth/background and no actor or complete game; **never state original DOS screenshot parity or playability**. Original 32954 ray traversal and actual wall/sprite scheduling remain next dependencies. Windows release workflow on `native-engine` remains manual-only.

## CONTROLLING Alpha 0.6 original-textured Windows acceptance — 2026-10-12

[Original RES4/COLORS/LEVELS textured Windows acceptance](ALPHA_ORIGINAL_RES4_TEXTURED_PREVIEW_ACCEPTANCE_20261012.md): original-file-backed independent Win64 `rloft_native.exe` now opens a keyboard-navigable first-person host preview, drawing real original RES4 EH6 wall pixels using level-appropriate original COLORS palette banks. [Owner-original Windows CI #38325198679](https://github.com/Hyperbog/RLOFT/actions/runs/38325198679) verified exact compiled source `e031a2ad875e1073bd5cc20b78844451967bc47e`, **32/32 CTests PASS**, real original **32/32 levels**, **5,048** authentic wall columns, **360,182** original source texels, **95** EH6 texture IDs, and source media SHA preservation. Original media not packaged; user downloadable ZIP available in the Actions artifact. **The view is still provisional host DDA and guessed wall face/height, NOT an authentic DOS scene or playable game.** Original camera/scene scheduler integration and user's live Windows field input/video acceptance are still open. NativeEngine stays separate from DemoV1; normal Windows workflow remains manual.

## Native Alpha 0.5: Windows original LEVELS.DAT preview bridge — 2026-10-12

[Source and acceptance checklist](ALPHA_WORLD_PREVIEW_WINDOWS_20261012.md): `rloft_native.exe` now has an explicit original-file-backed **Explore LEVELS.DAT (preview)** entry after the user selects owned GOG/DOS media. The new Win32 window consumes the independent source-validated world runtime, 32 maps, and proven collision categories; W/A/S/D/arrows navigate the existing **provisional host** raycast. Its screen **must display** that the camera, synthetic colors and projection are not original CODE.2 rendering. Original file writes and player save changes are prohibited. This is a pragmatic Alpha 0.5 shell for original camera/draw-scheduler integration, **not** the gameplay engine or a rebranded Level22-only demo. No additional reconstruction-only tests were authored. An exact-source Windows x64 MSVC build has now passed **32/32 CTests** in [isolated authorized Windows #38323574522](https://github.com/Hyperbog/RLOFT/actions/runs/38323574522); the compiled preview is downloadable. New GUI user GOG field check remains **pending**; do not cite earlier 29/29 Linux or previous GUI Windows CI as covering this new integration.

## CONTROLLING CODE.2 400-record ordering and non-NPC scene-stage checkpoint — 2026-10-12

[Original class-one/class-two stage and source-byte audit](ORIGINAL_CODE2_NON_NPC_SCENE_STAGING_20261012.md):
new native C++ staging mirrors original `2BBF4..2BCC2` and
`2BE48..2BF17` by binding original definition frame base/stride,
the source packed 3-bit view, source `35798` projection, and `C02D`
visibility threshold without guessing the later animation, callback
or final draw ordering. Original `OBJECTS.DAT` has been checked:
**336×8=2,688** authentic-data frame cases with zero mismatches;
**131,072** original-word synthetic source/view combinations passed.
Original scene ordering at `2C3F1` is capacity-corrected to
**400** (`0x2260` bytes), not the 200 records initially cleared
by `2C2AF`. Both isolated source tests passed CMake and sanitizers.
The full current-head NativeEngine CI suite is not independently
verified; the documented **29/29** remains the CI baseline. No original
DOS frame or playable-game equivalence is claimed.

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

## NEW controlling milestone — original WHOLE-CONTENT CERTIFIED, not native gameplay complete (2026-10-11)

Read [WHOLE_CONTENT_WINDOWS_CERTIFICATION_20261011.md](WHOLE_CONTENT_WINDOWS_CERTIFICATION_20261011.md). **Windows MSVC source SHA `44cc2ccc950aa686d6563b9eda6cd804b1b08cf8`**, [actual Actions run #38174139897](https://github.com/Hyperbog/RLOFT/actions/runs/38174139897), **16/16 CTests PASS**, all owned `DnD-Classics` original RES0–RES12 **12,562/12,562 typed/empty/raw records PASS**, all 8,362 EH7 source geometry/display-list records pass, all 1,319 EH6 textures and all 1,005 VOC containers PASS, all 32 LEVELS.DAT levels and 127 SINTAB raster-code blocks structurally accepted on real game bytes. Original world totals **115,584 cells, 1,659 controls, 394 active event slots / 555 commands**. This supersedes older small-step 9/9–14/14 build notes without invalidating their narrower historical evidence. Native content system has *three* independent Windows executables now: GUI, native whole-bank audit, and native whole-world+SINTAB audit; output ZIP is original-asset-free and independently SHA256 verified.

**DO NOT CONFUSE WITH GAMEPLAY:** the new C++ parsers handle original full source data but do not execute SINTAB x86 code, simulate NPCs/events or draw original 3D scenes end-to-end. **Next priority: integrate existing typed all-bank + all-level + SINTAB readers into a persistent native scene/world simulation and renderer**, including texture shading, CODE.2 geometry semantics, VGA palette context, event VM, ANM/audio/save. Avoid recurring one-EH-record preview releases; the owner explicitly requested large, integrated engineering strides. The separate `demo-v1` recomp remains untouched and must never become the new native engine's gameplay runtime dependency. Documentation-only HEAD after the above compiled SHA is not an executable build.


## RES archive contents: do not treat all records as EH1 raster images

The owner field-tested native RES3/4/5/6/10 and found "no additional EH type1", which is **correct for the old EH1-only viewer** but not a broken install. [RES_ARCHIVE_TYPES_AND_EH6_WINDOWS_20261011.md](RES_ARCHIVE_TYPES_AND_EH6_WINDOWS_20261011.md) records exact source types: RES3 4,234 EH7 scene shape/display-list records; RES4 667 EH6 world textures; RES5 EH3 XMIDI music; RES6 and RES10 EH4 VOC audio. RES9 contains 517 EH1 graphics and 38 script records. New native engine source includes a reusable original LZSS4K, standalone EH6 32x128/64x128 column-major texture decoder/preview, RES9 browsing and informative type-specific non-image errors. Real Windows build [#38172310617](https://github.com/Hyperbog/RLOFT/actions/runs/38172310617) compiled **source `744036693e84619d136d69dab157e9de25a2086e`**, passed **14/14 CTests** and uploaded checked asset-free ZIP. Owner actual GOG *EH6 pixel appearance* remains pending; do not mark visual parity passed. Next: compare texture orientation and original CODE.2 runtime shading, and reconstruct independent SINTAB/EH7 scene renderer. Treat audio archives as audio, not corrupted graphics. Keep DemoV1 recomp separate.


## Proven palette context is NOT per RES archive (user field finding, 2026-10-11)

The owner found multiple incompatible palettes *inside* the same RES file, matching existing original CODE.2 palette-routing research. [ORIGINAL_PALETTE_CONTEXT_ROUTING_20261011.md](ORIGINAL_PALETTE_CONTEXT_ROUTING_20261011.md) is now the controlling checkpoint for native asset display. Current Windows source has `native_palette_context`: **exact callsite evidence, optional active-world-level palette, canonical PREVIEW ONLY, explicitly UNRESOLVED**. The 32 original level display banks and mixed RES0 palette IDs are tested, and Windows GUI defaults to `AUTO (source context)` with manual 0–14 override. Never assign one COLORS palette to a whole RES archive or assume each raster has a permanent correct bank; the eventual native renderer must carry mutable VGA DAC/level/modal/fade context. A fixed bank for a resource is evidence only for the original callsite that was observed.

Real Windows [run #38170601091](https://github.com/Hyperbog/RLOFT/actions/runs/38170601091) compiled source SHA `b1892e8448734f3ad8018a0a3911fe9ed59d582c`, **13/13 CTests PASS**, verified asset-free ZIP. **New AUTO palette appearance on owned GOG is NOT yet field-accepted**. Previous GOG ISO indexing 19/19 remains valid. No DemoV1 runtime linkage or original files packaged. Documentation HEAD after this source SHA is not what was compiled.


## Wrong-looking native RES colors — corrected source and MSVC verified, field comparison pending

[WINDOWS_PALETTE_CONTEXT_CORRECTION_20261011.md](WINDOWS_PALETTE_CONTEXT_CORRECTION_20261011.md) records the owner's observation: native and separate decomp-demo renderers can show inverted-looking sprite colors. The confirmed native cause is a **single hardcoded COLORS bank 12 for every original RES resource** and omitted CODE.2 menu cursor **19-index remap**. Real game uses multiple bank contexts: menu 12, character creation 8, opening logos 14/9, etc. Correct independent colorizer in `src/native_raster_colors.cpp` takes the decoded palette explicitly and performs original DAC6 RGB→Windows BGRA without blind channel swapping/inverting. Windows asset UI has 0–14 bank selection, a Title cursor mode and raw RES browsing.

[Windows MSVC run #38169819411](https://github.com/Hyperbog/RLOFT/actions/runs/38169819411) compiled exact Git SHA `1c9440303a5c83ff6834a3f87cfdf9afc8dbfa19`, passed **12/12 actual CTests**, source-identified archive hashes and no original game assets. **The corrected visuals have not been user-field-verified** on the owned GOG install. Await a screenshot with the archive ID, record number and active bank before declaring DOS palette parity. The separate DemoV1 recomp project was NOT modified. Later docs-only HEADs are not the compiled revision.


## Verified independently built original RES browser (2026-10-11)

[WINDOWS_NATIVE_RES_BROWSER_VERIFIED_20261011.md](WINDOWS_NATIVE_RES_BROWSER_VERIFIED_20261011.md) records actual verified Windows MSVC x64 execution: separate EH1 decoder build [run #38168263623](https://github.com/Hyperbog/RLOFT/actions/runs/38168263623) initially found a malformed test lambda capture, then passed **10/10** CTests after fix; full original RES image browser [run #38168898735](https://github.com/Hyperbog/RLOFT/actions/runs/38168898735) compiled source SHA `d29914503fb6d6bb3a65954257d0ac145f0ceaf0`, passed **11/11**, and produced asset-free AMD64 GUI EXE, with exact ZIP and executable SHA-256 independently verified. The new browser selects RES archive/record and navigates type1 images without flattening/repacking the install; other EH types are explicitly not decoded yet. **New raster output against user-owned GOG has NOT YET been field verified**; earlier 19/19 real GOG resource inventory belongs to a previous compiler SHA. Do not call documentation-only later HEAD compiled. Keep DemoV1 full decomp/recomp separate and do not resurrect gameplay source linking. Next resource work should inspect real images, then typed EH/ANM/DAT formats.


## Real original GOG asset loading accepted; EH1 raster next unbuilt gate

The owner provided a **Windows screenshot from the independent `rloft_native.exe` asset-loader** opening their original GOG `game.gog`. [Field ledger](WINDOWS_REAL_GOG_ASSET_FIELD_TEST_20261011.md) records **ISO2048, 69 indexed files, 19/19 known resources structurally validated, 0 missing, 0 invalid and original palette12 displayed**. This closes the *real source-media indexing and structural inventory* gate of the earlier compiled `fd314b89` binary. It does **not** establish deep decoding or gameplay.

[New native EH1 decoder and cursor visual proof](NATIVE_EH1_RASTER_MILESTONE_20261011.md) is source-committed: `src/native_raster.cpp` independently parses raw/LZSS EH type-1 RES graphics, original `INIT[0..1]+0x2F` cursor resource, transparent key, bounded raster sizes; native GUI now optionally displays its actual sprite under palette12, and `--verify-raster` has strict original-media acceptance. New `native_raster_tests` is an expected **10th Windows CTest**. **No MSVC run or user-owned GOG field test has been performed for this new revision.** Preserve demo-independent build and exact Git source-revision claims; the earlier 9/9 Windows build does not cover new code. No new Windows workflow run without user authorization.


## First verified independent Windows asset build — new accepted state

[WINDOWS_NATIVE_ASSETS_VERIFIED_BUILD_20261011.md](WINDOWS_NATIVE_ASSETS_VERIFIED_BUILD_20261011.md) is the exact execution evidence. **MSVC x64 compiled the independent `rloft_native.exe` from SHA `fd314b89e96db07c739176d25f1fc9be94ecaa19`**, **9/9 native Windows tests passed**, source-identified no-assets ZIP uploaded and its actual bytes downloaded/verified. [Windows Actions run #38164976411](https://github.com/Hyperbog/RLOFT/actions/runs/38164976411). This replaces earlier **no native asset Windows compilation** claims; those were historical. The **real user-owned GOG installation has NOT yet been opened by this independent asset engine**, so field compatibility is still pending. Historical decomp bootstrap's original-menu screenshot does not verify this parser. Do not call any documentation-only later HEAD compiled. No new automated push triggers on main development branches; one authorized isolated build was used.


**Source of truth:** [ASSET_RUNTIME_ARCHITECTURE_20261011.md](ASSET_RUNTIME_ARCHITECTURE_20261011.md). Owner explicitly corrected architectural confusion on October 11, 2026.

## Two different products — DO NOT MERGE

- **`demo-v1` branch:** complete source-traced **decomp/recomp** workstream. Its intro cinematics, menu, character creation, DOS timing/animations and game controllers remain its own executable/project. It is allowed as **research and DOS-fidelity evidence**, not as a production dependency of `NativeEngine`.
- **`native-engine` branch:** wholly new, standalone replacement engine akin to **OpenMW/KEX**. Its executable must load original user-owned file data and independently implement rendering, gameplay, physics/world, audio, scripting and saves. **Do not link `DemoV1/src/win32_main.cpp`, 176-file game core, BootFlow/CharacterCreationFlow or any original DOS module** into `rloft_native.exe`. Do not copy/rename a decomp demo EXE or construct another old-code wrapper.

**A previous attempt to compile the decomp demo as native was mistaken and retired.** The modified DemoV1 CMake/host/media files were restored to previously verified blobs. Old docs `DEMO_BOOT_PIPELINE_REUSE_20261011.md` and `LIVE_ORIGINAL_RUNTIME_INTEGRATION_20261011.md` are historical, **superseded**. The previous Windows bootstrap (15/15 actual CTests; original GOG screenshot) is valid archival evidence but used decomp resource parsers; **its results do not certify the new independent native asset reader**.

## Active platform and file I/O decisions

- **Windows x64/MSVC first**. Do not spend new effort on Linux porting, Linux CI or Linux packages. Keep C++17 renderer/gameplay boundaries portable by design only where that does not delay Windows.
- **Read original media directly, lazily, read-only.** Mount original DOS loose files or GOG `game.gog` ISO9660; never modify installed files, never bundle game assets, never run CODE.1/CODE.2 executables or game.gog under DOSBox. Full installation unpack/repack is NOT a runtime requirement.
- **Native original asset path:** `native_media.hpp/.cpp` (ISO sector indexing/loose media/unique DOS paths), `native_assets.hpp/.cpp` (RES index/entry and COLORS 6-bit palette), `win32_native_assets.cpp` (independent Windows UI and user-owned GOG validation). `NativeEngine/CMakeLists.txt` contains **no DemoV1 source paths** and builds `rloft_native.exe` from these NEW sources.
- **Future asset cache** (optional): separate user-owned profile, disposable, keyed by source-byte hash + independent codec revision + resource ID; never replace the original ISO and never commit derived copyrighted resources. Mod VFS must layer above the immutable base, no silent save-file fallback to purchased installation.
- **Type/format validation**: bounded reads, case-insensitive DOS names, no path traversal, duplicate resolution fails closed. Tests must use synthetic copyrighted-free ISO2048 and raw 2352/offset16/24 as portability baselines, and a **REAL original owned GOG installation on Windows** for acceptance. Tests that only run old decomp source do not count toward native asset validation.
- Decode original assets as independent typed data: RES graphics/texture/scene/audio, `COLORS` palette, ANM video, DAT world/objects/NPC, music; add original gameplay semantics in new runtime once data is proven. Do not fake data, convert original game code into game loop, or overwrite retail saves.

## Builds, progress and release honesty

Current **only active Windows native-engine workflow**: [native-engine-assets-windows.yml](https://github.com/Hyperbog/RLOFT/actions/workflows/native-engine-assets-windows.yml), **manual-only** on default and native-engine branches. [Build-NativeAssets-Windows.ps1](Windows/Build-NativeAssets-Windows.ps1) builds with VS2022/MSVC x64, requires **at least 9** source-independent native CTests, verifies ZIP and EXE SHA256 and packages only four engine-owned files. Source output marks `engine_stage=NATIVE_ASSET_LOADING`, `gameplay_verified=false`.

**Current validation state:** independent source implementation and tests committed; **NO new Windows/MSVC compile or user-owned original installation field test has run for this new engine.** The old 15/15 Windows result is for the retired bootstrap. Do not increment gameplay completion, represent original ANM/DAT decode as implemented, or reuse old binary/artifact. Windows CI runs require an explicit new user instruction; `continue` does not automatically authorize a new run.

For any substantial task: inspect live HEAD/branch differences, original Code1/Code2 archaeology, applicable continuity, and source data; use separate self-contained native source/CTest improvements, commit with `[skip ci]`, report actual evidence and remaining blockers; preserve decomp/demo changes on its own branch.

**Next exact gate:** build native asset engine under Windows; have owner select real `game.gog`, validate 19 named original sources/RES entry tables/palette12; then implement genuine independent EH sprite/ANM/DAT decoders and rendering. Focus on original-data fidelity before graphical modernization or mod extensions.
