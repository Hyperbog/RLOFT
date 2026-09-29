RAVENLOFT: STRAHD'S POSSESSION - PORT ALPHA 0.2.31
================================================

THIS IS A WORK-IN-PROGRESS PLAYABLE ALPHA HARNESS.
It is intended to let you directly test the systems that are currently
implemented. It is not yet the finished Forgotten Realms Forest vertical slice.

IMPORTANT
---------
No original Ravenloft game files are included.

The preferred data source is the GOG installation containing game.gog. The
engine reads game.gog directly, extracts the complete CD RLOFT data into its
own cache, and uses that as the authoritative source. You do NOT need to copy
RES9/RES10 or the cinematic files manually.

FIRST RUN
---------
1. Double-click BUILD_AND_RUN_ALPHA.bat.
2. CMake/Visual Studio builds the actual RavenloftAlpha.exe target.
3. RavenloftAlpha.exe launches automatically.
4. When prompted, select your GOG Ravenloft installation folder containing
   game.gog. A complete extracted CD RLOFT folder also works.
5. The selected source is remembered in ravenloft_data_path.txt beside the EXE.

To select a different installation later, delete ravenloft_data_path.txt.

WHY THE ZIP DOES NOT CONTAIN A PRECOMPILED EXE
-----------------------------------------------
The development environment used to create this package is Linux and does not
contain Microsoft's Windows compiler/SDK. The package therefore includes the
real Win32 alpha application source plus a one-click Windows build/launch
script. On your Windows PC with the Visual Studio C++ tools already installed,
BUILD_AND_RUN_ALPHA.bat creates and immediately launches RavenloftAlpha.exe.
This is different from the earlier developer snapshot: the project now has a
real GUI application target named RavenloftAlpha.

CURRENTLY TESTABLE
------------------
- Native Windows game window
- Direct GOG game.gog data-source support
- Original Quick Start state in Forgotten Realms Forest (level 22)
- Decomp-derived sky, perspective floor, palette/lighting, scenery rendering
- Live movement, turning, strafing and collision through recovered DOS logic
- Original movement input channel behavior
- Original player-hand combat path can kill the assassin through live attack input
- Ground-object pickup now starts from reconstructed screen hit-selection and the real runtime object index
- Original level state/position/heading shown in the alpha panel

CONTROLS
--------
W / Up / Numpad 8       Forward
S / Down / Numpad 2     Backward
A / Left / Numpad 4     Strafe left
D / Right / Numpad 6    Strafe right
Q / Home / Numpad 7     Turn left
E / PgUp / Numpad 9     Turn right
F5                      Reset original Quick Start state
Z / X                   Character 1 left/right hand attack
C / V                   Character 2 left/right hand attack
Left mouse button       Reconstructed ground-object screen hit / pickup path
Esc                     Quit

RECONSTRUCTION CHECKPOINT 0.2.19
--------------------------------
- Continues directly from the green 0.2.18 combat-dispatcher checkpoint.
- Reconstructs the primary CODE.2 0x1BDAC..0x1BEF2 NPC scheduler loop, the
  original live caller that sequences 0x1BBD8/0x1B760 movement/decision into
  0xD550 attacks and the original post-attack recovery state.
- Reconstructs CODE.2 0x14A60..0x14AB1 character-name lookup used by the
  recovered combat/status paths.
- Wires the shipped level-22 assassin into OpeningDemoSession through that
  scheduler while keeping 0x30700, 0x311E0, and 0x19CC4 as explicit original
  timing boundaries rather than substituting the 33 ms presentation timer.
- Corrects DS:C658 in the D550 context to the party-facing angle.
- Fixes the regression-data gate so the shipped root LEV022.DAT is genuinely
  exercised; the previous green report could skip this Quick Start block.
- Clean build, focused scheduler/name tests, live Quick Start scheduler tests,
  GOG image checks, and complete shipped cinematic decode are green.

VIEWPORT CHECKPOINT 0.2.31
--------------------------
- Mechanical demo remains locked at 100% from checkpoint 0.2.30.
- Viewport/presentation ledger advances to 31% verified.
- Corrects Level-22 scenery resource selection: runtime field14 direction bits
  3..5 are no longer misused as the animation-phase resource-group offset.
- Corrects NPC render category 0: assassin NPCS.DAT base 315 now resolves to
  the actual RES3 assassin frames rather than unrelated inventory sprites.
- Ordinary sprite drawing now uses the CODE.2 0x36CEC positive-scale gate and
  shipped SINTAB 127 scale-table clamp instead of the 0x21BF0 interaction
  threshold.
- Original autumn foliage and assassin art are now present in the reconstructed
  Level-22 render path.
- The original HUD/VGA composition is still incomplete and remains future
  viewport work; the current HUD compositor is diagnostic only.
- Original-data regression remains green.

RECONSTRUCTION CHECKPOINT 0.2.30
--------------------------------
- Verified mechanical-demo completion: 100% for the agreed Forgotten Realms
  Forest opening scope.
- Removes the final live forced-ready mist shortcut.
- Reconstructs the opening-relevant CODE.2 0x1E75C DS:CF4C world-clock service
  and 0x31F94 hour normalization/wrap behavior.
- Preserves the original 1080 gameplay-tick hour cadence and Quick Start hour
  state restored from SAVE.DAT.
- CODE.2 0x222C0..0x222E5 now waits on the real world-clock hour progression
  from 14 to 2 instead of receiving a forced boolean ready state.
- Preserves 0x21F70 gameplay-clock disable through 0x19CCC(0) and restore
  through 0x19CF4 after quest-object cleanup.
- Live SAVE.DAT snapshots carry current CF4C world-clock state.
- Clean build, CTest, full original-data regression, direct game.gog checks,
  and all shipped CINE00..CINE12 decode-to-end checks are green.
- This milestone closes the mechanical/behavioral demo. Final original
  viewport/HUD presentation remains deliberately deferred to the next phase.

RECONSTRUCTION CHECKPOINT 0.2.29
--------------------------------
- Verified mechanical-demo completion: 99%.
- Reconstructs the opening inventory click-selection chain at CODE.2 0x12E70 /
  0x12F9B..0x12FC8 through 0x11FB0..0x12029.
- Global mouse Y is divided into the original four 80-pixel character panels,
  then reduced to panel-local Y.
- Preserves inactive-character status bit 0x8000 gating, original X 297..396
  and local Y 5..74 inclusive bounds, click-bit-0 requirement, click-bit-1
  rejection, and DS:6F90 panel-mode rejection.
- Reproduces the original 3x4 slot mapping using 34-pixel column and 18-pixel
  row pitches, then calls the already reconstructed 0x117A0 exchange.
- Removes the live alpha's `I` first-free-slot development shortcut.
- A temporary 1:1 inventory proxy supplies original framebuffer coordinates;
  final Ravenloft HUD art/presentation remains deliberately deferred.
- End-to-end original-data regression now stows assassin quest drops through
  inventory-grid clicks rather than an automatic first-free-slot helper.

RECONSTRUCTION CHECKPOINT 0.2.28
--------------------------------
- Verified mechanical-demo completion: 98%.
- Reconstructs the opening ground-object front half of CODE.2 0x223F4.
- Builds interaction records from the exact 0x35798 projection, normal 0x2B2F4
  4/8-view selector, RES3 sprite bounds, and 0x21BF0 rectangle math.
- Preserves the object-list builder's literal +0x20 interaction Y bias.
- Scans previous-frame draw order from topmost/nearest to farthest and uses the
  original inclusive rectangle comparisons plus click-bit-0 gate.
- The selected runtime source index enters the already-converted
  0x223F4 -> 0x21704 pickup/ownership/mist chain.
- Removes the live alpha's 1-4 object-ID acquisition controls.
- Original-data regression acquires all four real assassin drops from screen
  coordinates without naming an object ID to the pickup routine.

CURRENT LIMITATIONS
-------------------
- Finished original Ravenloft adventure HUD is not yet composed.
- Final original inventory/equipment HUD art is not yet composed; the current mechanical proxy forwards exact original inventory coordinates into reconstructed 0x12E70/0x11FB0/0x117A0 logic.
- The game-layer normal hand-click attack/auto-target path is live through temporary keyboard bindings; final HUD mouse input/feedback is not yet wired.
- Visible mist effect is not yet finished; the recovered transition backend is.
- CINE00 is decoded by the engine but is not yet integrated into this launcher.
- This alpha ends at the mist transition; Svalich Woods is intentionally outside
  the current demo scope.

DATA / FIDELITY POLICY
----------------------
World placement, state, rendering behavior, movement and gameplay behavior are
derived from Ravenloft_Decomp_Final and the original shipped game data. The old
viewer reconstruction is not used as world-placement authority.

BUILD FIX IN 0.2.2
------------------
The Win32 presentation harness no longer calls std::min/std::max in its window
scaling/centering block. This avoids Windows min/max macro interference that
caused MSVC C2589 and the follow-on C2737 diagnostic in Alpha 0.2/0.2.1.
NOMINMAX remains defined as a second line of defense.

REGRESSION STATUS AT PACKAGING
------------------------------
PASS: core data, Quick Start opening flow, GOG CD-image loading, and shipped
cinematic decode validated against the original files.
