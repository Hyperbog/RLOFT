# Ravenloft: Strahd's Possession — one game, mandatory acceptance contract

**Scope:** original DreamForge/SSI/TSR branding → CINE00/CINE12 introduction →
the authentic four-choice CODE.2 title menu → original CODE.1 character
creation → CHARS disk handoff → real CODE.2 Level 22 → assassin/relic ending →
authentic 22-frame mist transition. **No further campaign development may be
promoted until this one contiguous sequence works.**

## Locked owner milestone — required sequence and release freeze

**Owner-directed sequence:** modern API presentation → DreamForge, SSI and
TSR logos → original introduction → interactive four-choice title →
original CODE.1 character creation → faithful, fully playable Level 22 →
authentic 22-frame mist and one source-backed Level 23 transition.

**No expansion or release claim beyond this milestone** until its
full startup, all title selections, character choices, SAVE/Continue,
Level 22 environment/actors/interactions/sound/animation and original
transition are confirmed from a continuous player run. Neither compiling,
unit tests, static source-contract tests nor isolated screens satisfy
the gameplay/visual acceptance gate.

**Fidelity by default, enhancements optional:** the default Level 22
world is the source-raster scene (F2 modern polygon world must be opted
into); nearest-neighbor/source palette/filter settings are defaults.
Every graphical or input enhancement must be independently reversible
without affecting collision, script predicates, actor/item inventory,
puzzles, RNG, music content or the authoritative save data. The same
Windows OpenGL presentation path should drive logos, movies, title,
CODE.1 creation and gameplay, while a safe software path remains for
machines without a working GPU driver. The existing Win32 WGL backend
is a **compatibility-profile renderer**, not yet a modern OpenGL core
profile or DirectX 12 implementation. Modernizing the API must not
degrade original resource decoding or block gameplay fixes.

**Explicit boot-order mismatch needing original-game verification:** the
current Windows code plays `CINE00.ANM`, then `CINE12.ANM`, then the
three separately decoded splash/logo scenes. This does **not** match
the literal owner-specified logos-before-introduction order. Current
CODE.2 source research describes CINE events before logo steps, so
do not rearrange those assets on assumption alone: compare real
original CD-ROM playback (including the actual content of both movies)
to the expected owner sequence and resolve the discrepancy before
calling the front end accepted. The user-facing menu's Show
Introduction action must replay the correct original cinematic
without leaving the title unusable.

**Level 22 end-blocker:** `Level22MistTimeline` currently describes
original CODE.2 `21F70..223DE` operations, but the actual gameplay
loop still reports "mist transition pending" after the relic trigger.
Do not silently start Level 23 or substitute an invented cinematic.
Completion requires original effect startup, authored frame resource
lookup/draw, audio/timing, event lock, the exact frame-20 transition
and post-frame object removal, all tested in one live game session.

**Evidence required to close:** captured Win32 gameplay (logo
ordering, title menu actions, creation, Level 22 with the original
raster and optional 3D output), deterministic SAVE→quit→Continue
comparisons, original media and byte-source identity, audio inspection,
no flicker or bad perspective, and a cinematic record including the
single level transition. Each failure remains an open release gate.

## Startup-sequence discrepancy requiring original DOS visual validation

The requested on-screen sequence is **DreamForge → SSI → TSR logos,
then introduction cinematic, then title**. The current native `run()`
source executes original `CINE00.ANM` and `CINE12.ANM` event calls
first, then draws the three `RES0` logo resources (94,95,96),
then the title. Its CODE.2 `19888` comment identifies that ordering,
but the semantic contents of the ANM frames versus the three logo
resources have **not** been field-verified. Until original DOS startup
is captured, do not mark the user's requested display sequence as
achieved or silently reorder historical operations on assumption alone.
The original game's visible ordering must resolve the discrepancy.

## Technical and artistic invariants

- Keep the shipped original DOS game content/source code as the behavioral
  authority. The disassembled CODE.1/CODE.2 and SINTAB are *evidence*, not
  automatically usable native C++ game logic.
- Present the complete game through one Win32/OpenGL context when supported;
  keep direct GDI fallback for unsupported hardware. Draw original media as
  received: no invented logos, cinematics, text, characters, puzzle solutions,
  enemy actions or source map geometry.
- **Opt-in enhancements:** F2 experimental geometry; F3 smooth/mipmap
  filtering; F11 fullscreen; M/Ctrl+right-click modern mouse yaw/pitch; Shift
  run and WASD/QE are additional controls. The original DOS-derived world
  raster and nearest-neighbor pixels remain defaults. Never let an optional
  enhancement alter party state, original script predicates or collision.
- One real app: `Ravenloft.exe` and its optional convenience
  `Play-Ravenloft.cmd` must both execute the intro-to-Level22 product.
  Keep diagnostic explorers/capture tools source-only, not extra releases.
- Never overwrite the original user's `CHARS`, `SAVE.DAT`,
  `LEVnnn.DAT`, resource archives or original executables. Reconstructed
  native checkpoints are versioned and written under LOCALAPPDATA.

## Verified code milestones versus outstanding acceptance

| Gate | Implemented in source | Unmet quality/behavior gates |
|---|---|---|
| OpenGL from logos onward | `Host::begin_gpu_scene` before CINE00; persistent GL canvas for intro, menu, creation, world | Windows GPU screen capture comparing original logo order, palette, correct pixels/frame pacing |
| Show Introduction | Original CODE.2 title entry 0 runs genuine CINE00 replay | DOS image/video/audio timing comparison |
| Continue Game | Entry 1 reads guarded native slot and restores live party, map, NPCs, objects, inventory, event flags, RNG and visited levels | File-backed end-to-end restart/playtest; original DOS SAVE.DAT import; all transient timers and soundtrack restore |
| Create Characters | Entry 2 passes CODE.1 flow, builds exact 0x920 CHARS and rereads before CODE.2 | All original class/stat/portrait/party choices and input/audio visually accepted |
| Quit Game | Entry 3 routes through original boot exit state | Window close/audio/teardown field acceptance |
| Direct EXE startup | Original CHARS no longer silently skips logos; normal writes use private CHARS overlay | Launch from multiple genuine GOG/Steam/loose media layouts |
| Faithful Level22 world | Original source cell-map and RES4/RES8/RES3 raster software pipeline remains the default | Complete visual original DOS frame parity, scenery, distance clipping, sky, palettes, animation, sound and object interactions |
| Optional enhanced GPU world | Uses mutable real LevelState, NPCs, authentic RES4 mipmaps and RES3 rasters, same game's HUD | Real 3D sprite dimensions, world sky, scene occlusion and shading **not equivalent** to original yet |
| Camera direction | Original motion headings (0→+row, 90→−column) corrected; source C6CC=175 projection focal replaces hardcoded 75° vertical FOV | Actual field screenshot/pixel parity, near-plane and viewport edge cases |
| Level22 combat and ending | Source-backed assassin, drops, relic checks and original `Level22MistTimeline` exist | Finish playable combat/pickup and implement **live** effect, staging, 22 resource frames and frame-20 level handoff |

## Specific regressions identified, not papered over

1. Earlier OpenGL build incorrectly treated heading 0° as +X while CODE.2
   treats it as +Y. Corrected using an independently tested 0/90/180/270
   mapping.
2. It used a **75-degree vertical FOV guess**, while authentic SINTAB
   reports C6CC=175 with 256-pixel viewport. Horizontal FOV is 72.366°;
   at the preserved 4:3 canvas' physical first-person aspect **5:3** the
   correct vertical FOV is **47.389°**, derived, not guessed.
3. It drew OpenGL scenery *over the already-drawn software perspective*.
   The enhanced 3D viewport now clears that projection before drawing
   3D geometry. This still needs sky/RES8 compositing acceptance.
4. It activated unverified GPU geometry automatically. F2 is now opt-in;
   the authored software world remains the default even though **all**
   screens can be presented by OpenGL.
5. A direct executable was bypassing the original intro when stale DOS
   CHARS existed. An explicit `--resume-existing-party` is now required
   to take that compatibility route. Native CHARS transfers use an
   independent runtime overlay unless a temporary field-test sandbox
   explicitly requests the old writable copied game folder.
6. Continue Game and the in-game Save/Load originally did nothing or
   displayed an unimplemented message. They now use one bounded, hashed,
   atomic native save file at
   `%LOCALAPPDATA%\Hyperbog\Ravenloft\SavedGame-1.rls`.
   The original DOS SAVE.DAT remains untouched. This is a **native slot**,
   not yet DOS-compatible loading, and not yet an exact replay of every
   in-flight audio/effect event.

## Level22 ending: precise original machine-code boundary

The separate `AssassinRelicTransition` and `Level22MistTimeline` source
models confirm CODE.2 `21F70..223DE`: kill NPC type 9; four drops
`0x88,0x87,0xE9,0x74`; collect all four; original event prepares/camera
stages and starts effect `0x21` variant 7; exactly 22 original resource
frames sourced from the level's mutable resource base + `0x67+i`; at
frame **20**, invoke original `181B4(23)`; after all frames, remove relics
`0x87,0x88,0x74` and preserve parchment `0xE9`. This **timeline is
not yet a live cinematic renderer**. Do not replace it with a slideshow
or claim Level22 victory until it is integrated and filmed.

## Required release acceptance tests

1. Launch raw `Ravenloft.exe` and the provided CMD separately on owned
   media, without preexisting CHARS and with stale CHARS; confirm identical
   original logo/cinematic/title order and appropriate media discovery.
2. Exercise all four title entries, including replay from the menu, a
   missing-save Continue diagnostic, a valid saved Continue and clean Quit.
3. Create two original actors, confirm exact CHARS handoff, names, class,
   stats, inventory, sound and character portraits, then enter Level22.
4. Save after moving/turning, opening a source door and altering
   inventory/NPC HP; quit to Windows; Continue; compare party pose, map,
   event flags, RNG, inventory, placed/loose objects and live NPCs.
5. Side-by-side exact original DOS and native frames from original
   Level22 (4.5,4.5 facing 270), rotate four directions, walk and strafe,
   repeat after doors/combat. Capture both original software and optional
   OpenGL render; no empty/malformed scenery or mixed perspectives.
6. Verify sampled audio, MIDI, environmental ambience, 60Hz presentation,
   cinematic delays and actual animation source frames—not just frame
   counts or test pass rates.
7. Defeat the assassin, pick all original relics, trigger original effects
   and mist frames, execute exactly one level transition at frame 20,
   preserve parchment, remove three relics, and confirm post-transition
   state without inventing gameplay in Level23.

**Status:** code fixes and CI can pass while level visuals/audio remain
unaccepted. Treat any failed item above as a blocking bug. Do not describe
the port as game-faithful, feature-complete or past Level22 until every
applicable item has field evidence.
