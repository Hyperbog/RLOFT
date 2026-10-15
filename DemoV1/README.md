# Ravenloft: Strahd's Possession — enhanced native PC port

**Active development branch:** `demo-v1`  
**Goal:** create a **complete enhanced native Windows PC port** of Ravenloft. Preserve all authored game content and established cinematics/audio/character creation, but use modern display/input/timing/audio and safe port-native saves. **Exact original DOS execution or DOSBox parity is no longer a release requirement.** [Active port charter](ENHANCED_PC_PORT_CHARTER.md) supersedes the former original-fidelity mandate. The [mod-forward profile](MOD_FORWARD_PROFILE.md) applies the [universal methodology](../MOD_FORWARD_METHODOLOGY.md) without claiming a shipped mod loader. The priority remains one working intro → creation → genuinely rendered playable Level 22 → entire campaign.

This is the active project entry point. Historical development updates have been preserved, without deletion, in [the original full development log](DEVELOPMENT_LOG_ARCHIVE_20261010.md). That archive contains old checkpoint statuses and must **not** be read as a current test or release report.

## Owner field milestone — continuous Windows startup reaches Level 22 (October 13, 2026)

The owner has tested the [Windows continuous-Alpha build #819](https://github.com/Hyperbog/RLOFT/actions/runs/38494095524) and confirmed it **runs all the way to Level 22**. This is the first field-confirmed end-to-end startup checkpoint for the enhanced PC port, including the transition out of character creation. The owner also reports **remaining Level 22 issues**, so this is **not** acceptance of the world renderer, gameplay, audio quality, controls or the full Alpha.

Build source: `4dfa3eaa170a4622d46f3fa92989c6dbed726683`. Windows CI: **29/29 focused tests passed**, clean syntax parse, artifact published. The published ZIP was independently audited for integrity, a matching PE executable SHA-256, exact source revision, original-media auto-discovery, and absence of original copyrighted assets.

**Current release evidence: ALPHA-BUILD met; other nine Alpha gates remain open.** Next work: inspect the current Level 22 frame/video and repair its specific scene/rendering, NPC placement, animation/audio and gameplay defects without regressing the now-working continuous front end.

## Active policy — enhanced Windows PC port (October 13, 2026)

The project owner superseded the historical strict DOS recreation charter.
**Read [ENHANCED_PC_PORT_CHARTER.md](ENHANCED_PC_PORT_CHARTER.md) first**, and
use the [new Alpha/Beta/Final roadmap](RELEASE_MILESTONES_ALPHA_BETA_FINAL.md).
We no longer need instruction-perfect decompilation, DOSBox playback parity,
VGA planar/IRQ/BIOS timer cloning, or destructive original save quirks as
release gates. Retain and reuse the original DOS research **only where it
unlocks game content, rules, assets, scripts, gameplay or compatibility**.
A complete playable modern port takes precedence over theoretical exactness.

Current priority: preserve functioning splash/intro/audio/lady-cat/card
animation and character creation; fix the actual blank/flickering Level 22
world, missing environmental scenery and audio; then connect campaign-wide
mechanics, levels and robust port saves. Windows x64 is the first target.

## Current Level 22 UI/graphics checkpoint — October 14, 2026

The [Part 21 triage](PART21_LEVEL22_GRAPHICS_AUTOMAP_OPTIONS_STATUS_20261014.md)
records the four **separate** owner-reported Level 22 defect areas
(graphics, inventory, automap, top menu). Title-screen Continue is a
**separate** save/load feature, not another Level 22 defect.

The [Part 22 native modal-input checkpoint](PART22_LEVEL22_MODAL_INPUT_20261014.md)
implements automap arrow/Shift+arrow pan and isolates inventory/map
keyboard input from world movement. The Windows source-matched build
[`5c9e87b4`](https://github.com/Hyperbog/RLOFT/actions/runs/38630980570)
passed **37/37 focused tests** and published a verified Windows Alpha
artifact. The original opening, sound and character creation remain
preserved. **No new owner Level 22 graphics/inventory acceptance has yet
been recorded:** tree placement/colors, other inventory bugs, map visual
details, missing top-menu gameplay actions, and title Continue remain open.

## Level 22 native RES3 SINTAB color correction (October 15, 2026)

[Authenticated sprite shade-plan source, original SINTAB verification and
remaining field gates](LEVEL22_RES3_SINTAB_SHADE_CHECKPOINT_20261015.md).

Native RES3 scenery previously drew decoded palette indices without the
source-authored SINTAB packed palette subtraction. The enhanced native
compositor now applies per-column, per-row AL/AH shade values and low-byte
borrow with native underflow clamping. Original RES3 pixels, masks, geometry,
scene order and DOS-reference rasterization remain intact. A separate typed
shade plan decouples source lookup, screen-row phase and replacement artwork.
[Windows CI #892](https://github.com/Hyperbog/RLOFT/actions/runs/38825500152)
compiled the exact source, with **38/38 focused tests passing** and
**17/17 source-guard mutations rejected**. The Windows Alpha executable
was refreshed and published. The screenshot/field acceptance gate is
**still open**.

## Level 22 RES8 authored-width clipping (October 15, 2026)

[Source audit, enhanced rendering fix, regression evidence and outstanding
visual gates](LEVEL22_RES8_AUTHORED_WIDTH_CHECKPOINT_20261015.md).

Environment draw plans already carry authored widths (including 32 contiguous
eight-pixel randomized backdrop strips). The presenter had disregarded the
plan's width and used the complete RES8 image stride, allowing one strip to
overwrite its neighbor when a source raster is wider than the planned draw.
The native draw path now uses the bounded authored width, retaining source
transparency and the original resource pixels. The default historical
full-raster drawing API is unchanged. Explicit invalid widths are rejected;
large authored coordinates are accumulated without signed-int overflow.
[Windows CI #885](https://github.com/Hyperbog/RLOFT/actions/runs/38823907665)
compiled exact source `362d66a49c6f` successfully, with **38/38 focused
CTests** and all 16 native scene source-guard mutations rejected.
The source-matched Windows Alpha artifact was published. Actual Level 22
visual acceptance remains open.

## Level 22 enhanced RES4 ground shade-underflow protection (October 15, 2026)

[Native ground shade policy and original-source evidence](LEVEL22_NATIVE_GROUND_SHADE_CHECKPOINT_20261015.md).

The Windows enhanced renderer now clamps RES4 texels below the original
FE15 per-row shade decrement to palette zero, instead of wrapping into
unrelated high palette indices. The **standalone DOS-reference raster
remains byte-accurate** because its default policy retains 8-bit wrap.
Runtime field captures report potential wrap pixels separately.
[Windows CI #884](https://github.com/Hyperbog/RLOFT/actions/runs/38820363732) compiled the exact new source and passed **38/38 focused CTests**, with a refreshed continuous Alpha executable. Source checks and CTests remain separate from
**outstanding Level 22 visual acceptance**.

## Level 22 native RES3 sprite bounds and palette evidence (October 15, 2026)

[Exact Windows build, field-capture format, source evidence and remaining graphics gates](LEVEL22_RES3_NATIVE_BOUNDS_PALETTE_CHECKPOINT_20261015.md).

The native RES3 compositor now reports per-source *actual written pixel
bounds*, potential opaque pixels before viewport clipping, palette-index
range and zero-output placement, without changing the game's pixels or
authored scene geometry. Extremely offscreen modded sprite coordinates
no longer risk signed destination-Y overflow. Existing frame-capture TSVs
include `scene_native_draw` records for identifying trees/NPCs that are
projected outside the visible Level 22 window.

[Windows CI #877](https://github.com/Hyperbog/RLOFT/actions/runs/38722251801)
compiled exact source `c7099fe41a4cef96a0ad8d4fad976b70c790a972`;
**38/38 focused tests passed**, including compositor, capture and
frontend-preservation regressions. The verified Windows Alpha artifact
was published. **Visual sprite placement and palette correctness are
not yet field accepted.**

## Part 23 — Level 22 automap annotation clip repair (October 14, 2026)

[Source, tests, build provenance and remaining defects](PART23_LEVEL22_AUTOMAP_ANNOTATION_CLIP_20261014.md).

The enhanced automap text renderer now uses the **same exact 48×47-cell
clipping rectangle** as its terrain tiles and player marker, preserving
the right-hand buttons and bottom chrome. Labels overlapping the map
boundary are partially displayed rather than disappearing. A Windows
build of source `46d1733d6a145ee7dd7e0eb68f848cdc4216814f`
[passed 38/38 focused tests](https://github.com/Hyperbog/RLOFT/actions/runs/38654822818)
and published a continuous Alpha artifact; its ZIP, PE signature,
source stamp and executable SHA-256 were verified independently.

**Not yet owner-accepted:** Level 22 tree/texture placement and colors,
other inventory mechanics, remaining automap behaviors, and Rest/Pray/
Memorize/Load/Save top-menu actions. Title Continue is a **separate**
native save/load feature. Working intro, character creation and audio
have not been rewritten.

## Part 20 — Native Alpha build acceleration and Level 22 music

[Windows enhanced-port release/test workflow and authored XMIDI cue](PART20_ENHANCED_ALPHA_BUILD_AND_WORLD_MUSIC_20261013.md).

A normal `demo-v1` source push now compiles and tests the **native
enhanced Windows game**, front-end preservation and Level 22 motion/
rendering/audio regression set before publishing the continuous
`Ravenloft-Continuous-Alpha-Windows-x64` artifact. The canonical
original-DOS recompiler/source audits remain accessible via manual Full
CI, no longer automatic Alpha blockers. Every checked-in EXE refresh
includes the **actual compiled checkout revision** in
`DemoV1/Windows/build-source-sha.txt`.

At the genuine post-character-creation Level 22 load, native playback
now starts the authored original RES5 XMIDI cue identified by
`LevelState::music_sequence` (level 22 = **0**) with a 2500ms fade.
`EnhancedWorldMusicState` prevents repeated song restarts at 60 Hz,
updates on level/cue changes, and resumes music after full-screen
cinematic interruptions. The established CINE/character audio is
unchanged; remaining NPC/encounter sound effects are unfinished.

**Pre-Alpha:** the latest exact-source Windows build and audible/visual
owner field acceptance are still pending. Do not treat source-only
contract results as a successful playthrough.

## Part 19 — enhanced native Level 22 scene and pacing (October 13, 2026)

[Actual code changes and outstanding visual field test](PART19_ENHANCED_LEVEL22_NATIVE_RENDER_20261013.md).

The player-facing enhanced renderer now draws **original decoded RES3 type-7
tree/object/NPC pixels** via a masked native compositor, projected into
the existing source-authored depth-clipped scene columns. It no longer
requires successful DOS VGA plane-instruction tracing to show the scenery.
The verified original **RES4 textured floor** draws as a background behind
foreground wall surfaces rather than failing the old DOS all-no-wall gate.
Native exploration uses **60 Hz frame presentation**; the previously
working CINE, splash, music, lady/cat/card animation and character-creation
timings have not been rewritten. The original artwork, world placement,
palette and HUD pipeline remain in use.

A new standalone C++17 sprite compositor test, extended ground eligibility
tests and an eight-mutation native scene contract test are registered.
A local GCC 14 interface-equivalent compositor smoke test passed. **The
full exact-source Windows artifact and owner-visible first Level 22 scene
are still unverified.** Previous reports of an empty/flickering scene
remain open until a successful real-game playtest.

## Part 18 — ONE continuous original frontend → first Level22 Alpha

[Source, launch safety and player acceptance boundaries](PART18_CONTINUOUS_ALPHA_FRONTEND_TO_LEVEL22_20261013.md).

The primary Windows Alpha artifact is
**`Ravenloft-Continuous-Alpha-Windows-x64`**. Unzip it, copy your
*owned original DOS installation files* once into the adjacent `Game/`
directory, and double-click **`Play-Ravenloft-Alpha.cmd`**.
There is **no folder picker or gameplay mode selection**. The launcher
validates the binary and copies the original game files to a new disposable
workspace, removes `CHARS` *only inside that copy*, and launches the
**normal no-developer-flags** native path:

`CINE00 → CINE12 → logos → title → CODE.1 character creation → exact
CHARS on disk → CODE.2 reads CHARS → authored Level22 entry`.

The owner-approved ANM/MIDI/SFX/title/card-animation/character-flow
implementations remain source-locked against the established frontend
snapshot; the player route avoids opt-in `--dev-trace` frame overhead.
Existing `Play-Original-Intro.cmd` remains a separate **incomplete
showcase** and must not be mistaken for this continuous Alpha.

**Pre-Alpha**: merely reaching Level 22 does not establish that its ground, sky, trees, backgrounds or assassin are visible, or that sound, performance and controls work. Prior owner-observed blank/flickering scene and missing audio are P0 port issues. Bit-exact DOS VGA/frame parity is no longer required.

## Part 17 — original title Load Game source oracle (2026-10-13)

[Private CODE.2 07B94 selector verification and remaining full-save blocker](PART17_ALPHA_TITLE_LOAD_SLOT_ORACLE_20261013.md).
A second standalone, source-verified C++17 model now preserves
the original **WORK clear → parent-directory change → SAVE## copy →
shared loader / retry** result ordering, original non-atomic error
behavior, ten-handle cleanup and the mutable pathname-separator quirk.
It performs **no filesystem IO**, remains disconnected from the live
title, and has a separate registered native CTest. Original byte-oracle
coverage expanded from 14 to **24 exact opcode windows**. A locally
compiled GCC 14 semantic-equivalent harness and all ten new private
CODE.2 instruction windows passed; an exact-current-HEAD Windows
build, player field test and saved-world parity remain independent.

An additional source-only CTest now protects the original Load Game
slot geometry and prevents the unfinished native action from falsely
claiming a loaded campaign. A separate, intentionally unhooked C++17
selector helper and Release-active exhaustive CTest now preserve original
right-click cancellation and ten-slot hitboxes. Against the supplied
private SHA-pinned `CODE.2`, the original-byte oracle passed
**24 instruction windows, 8 source-relative calls, 7 failure/retry
branches and 7 fixed DGROUP strings**. The original
`SAVE01..SAVE10` load first copies a complete slot directory into
`WORK` before the shared global/level loader; `SAVE.DAT` alone is
not a sufficient saved game. This checkpoint does not implement the
modal or SAVE/LEV restoration, test Windows gameplay, or advance Alpha.

## Part 17 — exploration preload first-blocker trace (2026-10-13)

[Source and instrumentation boundary](PART17_ALPHA_EXPLORATION_PRELOAD_TRACE_20261013.md).
The separate opt-in exploration diagnostic now starts **before** the
original Level22 initial session/world load, rather than only after its
presenters were constructed. Source-safe stage IDs distinguish initial
load failure, world-state initialization failure and presenter readiness.
The early trace pointer is detached on exceptions and ordinary launches
without `--dev-trace` remain non-instrumented. A source-only regression
guards the ordering and ten deliberate negative mutations. The isolated
field runner reports only the last whitelisted initialization checkpoint
rather than printing potentially private raw input events. The same
opt-in trace now distinguishes **first gameplay frame build, raster, and
host submission** using one-shot stage IDs; `present.submitted` is not
proof of a correctly displayed frame. The Windows artifact now carries the built EXE's
SHA-256 manifest. The isolated Alpha runner checks that hash before
accessing owned media and refuses to label a missing or non-0x920-byte
copied CHARS file as an existing-party test. Its sandbox contract
rejects seventeen deliberate unsafe source mutations, including
wrong-process diagnostic trace attribution and filesystem-link roots. **This is not a verified
current-HEAD Windows field test or original DOS parity; the release
remains Pre-Alpha.**

## Alpha → Beta → Final: release acceptance (2026-10-12)

**Current classification: Pre-Alpha.** See the
[release milestones, criteria and blocker order](RELEASE_MILESTONES_ALPHA_BETA_FINAL.md)
and the [machine-readable gate ledger](RELEASE_GATES.json).
Under the **enhanced port charter**, Alpha is the continuous native intro, title and two-person character creation through **actually visible and playable Level 22** with sound and responsive controls. The full Level 22 quest, later levels and port-native save/load belong to Beta. Final includes complete game content, smooth modern rendering, scalable displays, reliable sound, input/options and safe persistence. Original DOS bit/IRQ/VGA parity and DOS-format saves are optional, not release gates. Historical reports below retain their old project terminology for research continuity.

Latest verified exact-checkout Windows Full:
[#676](https://github.com/Hyperbog/RLOFT/actions/runs/38346121349),
source `b17871c9ac2350afd0c4667de78a2aca059a5bad`,
**257/257 CTests passed**. This does not demonstrate the complete
normal original-game startup on Windows. Windows CI
passes do **not** establish native normal-play field acceptance or
original-DOS parity. The historical build/status paragraphs below
are retained as dated evidence, not automatically current.


## Isolated normal-game Alpha field test (2026-10-12)

The separate development-only CI artifact
`Ravenloft-Alpha-Field-Test-Windows-x64` provides
`Test-Alpha-Continuous-Game.cmd`. It selects a user's owned
original game installation and makes a unique **local temporary copy**
before running the ordinary `--dev-trace` path. The selected original
CHARS/SAVE.DAT files are never deleted or rewritten; a cold start may
delete CHARS **from the copy only**. A `--dev-trace` explicit temporary
installation also **does not change the saved Windows game-folder
preference**; normal non-diagnostic installation selection retains
its existing remembered-folder behavior. The copied data remains under
`%TEMP%\\RLOFT-Alpha-Field` until the tester elects to remove it.
Captured diagnostic ZIPs are local and may contain personal party names.
The opt-in `--dev-trace` field run now also creates a **separate startup
diagnostic ZIP** containing source-SHA-pinned stage IDs and native input
observations from before CINE00 through title/character creation.
**Raw key and scan codes are intentionally suppressed throughout CODE.1
character creation**, so entered actor names are not recoverable from
the diagnostic events.
This startup ZIP can exist even if the first Level22 runtime never
loads; a distinct exploration ZIP is created only after entering that
runtime. Neither is automatically uploaded or counts as DOS evidence.

**This is an opt-in Pre-Alpha field-test artifact, not an Alpha game
release and not DOS execution.** It does not restore the retired
Level22 warp or enter gameplay from the default read-only
`Play-Original-Intro.cmd` launcher. The main player-facing artifact
continues to contain only the read-only front-end showcase.

Source-only safety and packaging regressions are registered as
`alpha_sandbox_normal_launch_contract_tests`; Windows execution,
the first normal-game blocker, and original-DOS comparison remain
separate acceptance gates. See
[formal Alpha/Beta/Final criteria](RELEASE_MILESTONES_ALPHA_BETA_FINAL.md).

## Original intro / character-creation build (2026-10-11)

The owner has retired the standalone Level 22 demo as a player-facing
checkpoint. The existing code already contains the CINE00/CINE12 intros,
DreamForge/SSI sequence, title/menu animation and CODE.1 character creation.
The [verified #661 package](FRONTEND_RESTORATION_BUILD_661_20261011.md) includes `Play-Original-Intro.cmd` and uses owned original media,
forces the existing front end even when CHARS exists, and **does not modify
CHARS/SAVE.DAT or enter the incomplete Level 22 renderer**. The unfinished
title-menu options are explicitly reported as such. This is a native
front-end recreation, **not** the original DOS executable or a complete
playable game. Existing Level 22 game assets and reconstruction code are
preserved for eventual game continuity; old Level 22 demo launchers are no
longer distributed. Automatic Windows Full CI is authorized on public
`demo-v1` updates and publishes `Ravenloft-Original-Intro-Windows-x64`.

## Current evidence and limits (2026-10-11)

| Gate | Evidence / current limit |
| --- | --- |
| Original executable | `CODE.1` / `CODE.2` reconstructed and byte-accounted; **not exhaustively semantically verified**, and post-accounting corrections remain possible |
| Native Windows Full build | [#661](https://github.com/Hyperbog/RLOFT/actions/runs/38194351208) **246/246 CTests passed** on exact source `66046988`; [intro-and-character-creation artifact](FRONTEND_RESTORATION_BUILD_661_20261011.md) verified. This is not original DOS or end-to-end campaign parity. |
| Current source test catalog | Run the [read-only preflight](tools/research_preflight.py) for current CMake declarations; source registration and passing Windows CTest are separate gates. |
| Level 22 viewport | Original `RES4[655]` full-open ground confirmed in 64/64 native capture frames; source-accurate partial clipping and DOS framebuffer equivalence remain pending |
| Original DOS acceptance | No accepted full indexed-frame/palette/IRQ/RNG/complete-game parity; do not infer fidelity from synthetic tests or a successful native build |
| Working score | Provisional **49.8% engineering readiness** is **not** 49.8% original-game completeness; preserve independent DOS parity gates |

The latest native source includes a read-only, provenance-checked source `22220/FF50` field projection. It does **not** yet use partial-depth calculations for gameplay rendering. Original `F790` stack-local text pre-read and callback identity have been synchronized from the verified decompilation branch; this is a **source correction**, not a passing end-to-end game test.

**October 11 build follow-up:** [Windows Full #659](FULL_WINDOWS_BUILD_CHECKPOINT_20261011_RUN659.md)
compiled but failed 2 source-only documentation assertions (243/245);
the missing `42-way` source-audit phrase was repaired. The
[user-authorized #660 rerun](FULL_WINDOWS_BUILD_CHECKPOINT_20261011_RUN660.md)
then passed **245/245**, produced a new Windows artifact, and committed its
verified executable. This closes the Windows regression checkpoint, **not**
the M-PLAY-01 normal-play field test or original DOS equivalence.

## Player field observation: prototype versus original DOS runtime

**October 11:** the owner reported that the bundled normal-trace launcher
still opens the old Level 22 native demo. This is consistent with the
source: `ravenloft_demo_v1.exe` is an incomplete C++ reconstruction
using original asset data, **not the original DOS R.EXE or original
CODE.1/CODE.2 execution**. Existing `CHARS` immediately enters its
Level 22 native engine. The field milestone M-PLAY-01 is **not accepted**.

The NEXT authorized Windows build will update the diagnostic command to
`--dev-trace --choose-install` so the source installation is selected
explicitly (instead of silently reusing a cached one). This **only
clarifies provenance**; it does not make the complete game playable.
Build #660 remains the last Windows-verified executable. Do not
represent this source-only revision as built or DOS-faithful.

## Current player field-test checkpoint

**[M-PLAY-01 — First native boot-to-blocker playability assessment](MPLAY01_FIRST_NATIVE_BOOT_TO_BLOCKER_20261010.md)** is now tracked in [GitHub issue #7](https://github.com/Hyperbog/RLOFT/issues/7). Its purpose is to test an exact-source Windows build through the **ordinary intro/menu/character creation/Level 22 path**, determine the farthest genuinely playable action, and preserve a reproducible first blocker. The existing developer-only Level 22 launcher remains a **separate diagnostic**, not proof of normal game progression. The milestone is defined, not yet field-accepted; the Windows Full #660 compilation/test gate is accepted; independent DOS parity is unchanged. No build is dispatched by creating this checkpoint.

The verified #660 Windows artifact includes
`Trace-Normal-Launch.cmd`, which starts ordinary gameplay with opt-in
non-mutating native tracing instead of the Level 22 diagnostic shortcut.
It preserves native game exit status and distinguishes failure before
exploration from a recorded field session. Existing save files must be
protected using a disposable runtime copy. Build #660 is published; normal gameplay and DOS acceptance remain open.

The [M-PLAY-01 offline trace-integrity tool](tools/audit_mplay01_trace.py)
now verifies an existing diagnostic ZIP's original indexed-byte hashes,
palettes, capture events and build provenance without unzipping or modifying
gameplay. Its synthetic tests do not replace the unaccepted current Windows
Full build or normal-path field test.

## Mandatory development methodology

1. [Original execution fidelity charter](ORIGINAL_EXECUTION_FIDELITY_CHARTER.md) — binding goal, immutable-game source verification and **pre-commit** proof requirements.
2. [Active game integration contract](CONTINUOUS_GAME_INTEGRATION_CONTRACT_20261009.md) — up-to-date blockers, branch ownership, manual CI requirements and source-to-native gates.
3. [Subsystem readiness and acceptance](PROGRESS_SUBSYSTEM_REPORT_20261009.md) — tracked engineering readiness; keep it distinct from DOS equivalence.
4. [Preservation and integration roadmap](PRESERVATION_INTEGRATION_ROADMAP.md) — full-game scope and inter-system dependencies.
5. [Research iteration playbook](RESEARCH_ITERATION_PLAYBOOK.md) — repeatable preflight, faster original-byte oracles, safe concurrent commits and measurable process corrections.

**Source priority:** reuse original data and already-reconstructed `Code1/Code1.cpp`, `Code2/Code2.cpp`, and native `DemoV1` systems to preserve intended gameplay and content. Original-byte/DOS research is a tool for resolving real unknowns, **not** mandatory proof for every modern renderer, timer, input or safety improvement. Do not silently replace authored story/game mechanics with placeholders.

## Where to work

| Area | Description |
| --- | --- |
| `include/rloft/`, `src/` | Portable native engine and original behavior adapters |
| `tests/` | C++17 standalone behavioral regressions |
| `tools/` | Offline Python original-byte/source contract tests and field probes |
| `Windows/` | Windows executable host, supporting assets and launchers |
| `../Code1/`, `../Code2/` | Reconstructed original executable code (maintained across divergent branches) |
| [Level22 nearest-depth/FF50 projection](LEVEL22_NATIVE_GROUND_22220_FF50_FIELD_PROJECTION_20261010.md) | Recent source-backed native diagnostic and remaining cache/overlay limitations |
| [F790 decompilation proof](../Code2/verify_f790_stack_local_preread.py) | Original `T[0x3F]` read-order regression; 21 original E8 sites |

## Build and verification

The project uses CMake, C++17, Python regression tools, and a Windows host. Portable checks can be built locally with an available C++ compiler:

```sh
cmake -S DemoV1 -B build/native -DCMAKE_BUILD_TYPE=Release
cmake --build build/native
ctest --test-dir build/native --output-on-failure
```

Run these from the **repository root**. The Windows GUI executable, audiovisual quality, input responsiveness, save behavior and resource-backed gameplay need actual Windows playtests. Passing `ctest` alone is not proof of player-visible quality. Full DOS playback comparison is optional. Tests must retain active assertions under Release, as configured in the CMake suite.

**Full Windows builds now run automatically on relevant `demo-v1` pushes**, by owner authorization after the repository became public. Passing CI remains distinct from actual player gameplay acceptance. Original copyrighted game assets are required for game execution and should not be added to source commits.

## Research history

The [archived development log](DEVELOPMENT_LOG_ARCHIVE_20261010.md) preserves every prior entry from the former 126 KB README, including historical, superseded test counts and failures. Independent original-byte audit and subsystem documents remain in place for reproducibility. Do not retroactively reinterpret old `certified` headings as a guarantee of semantic parity.

## Optional shared REA research adapter

[Optional shared REA native-binary analysis](REA_ANALYSIS.md) is available through the canonical `code1-decompilation:ResearchInfra/` checkout. It is local, read-only static analysis and **does not** install REA into this application's runtime, alter original game media, or certify original DOS/gameplay parity.
