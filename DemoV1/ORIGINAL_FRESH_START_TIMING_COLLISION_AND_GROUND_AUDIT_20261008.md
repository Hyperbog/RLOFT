# Original fresh-party gameplay fidelity: binary audit and unbuilt corrections

**Checkpoint date:** 2026-10-08 (source-only), branch `demo-v1`.
**Original authorities:** user-supplied DOSBox screenshots of an ACTUALLY
NEW CODE.1-CREATED PARTY, original `CODE.1`, original `CODE.2`,
`NPCS.DAT`, `INIT`, `LEVELS.DAT`, and original `SINTAB`.

**DO NOT build automatically.** GitHub Actions remains workflow_dispatch
only with Linux tests disabled by default. Latest observed Windows CI:
manual #640, 160/160 tests, at an OLDER source revision. The changes
below have not yet passed a current-HEAD MSVC build or DOS comparison.

## Immediate priority from original screenshots

The supplied reference shows the original orange sky/distant hills,
spatially varied autumn trees, textured green/red ground, authentic
portraits and starting inventory, original automap, advancing Assassin
encounter, damage feedback, and weapon/hand attack artwork.

Native static scenery and frame construction are insufficient acceptance:
movement, collision, fresh-party inventory, NPC timing, texture fill and
combat state must be reproduced in sequence.

## Fresh original CODE.1 -> CODE.2 path; NOT quick SAVE.DAT

The screenshots were taken after original character creation.
`SAVE.DAT` is not the provenance of either actor in the screenshots.
The default Win32 flow already uses:

CODE.1 character editor -> `build_character_transfer` ->
complete 0x920-byte `CHARS` -> R.EXE status-10 continuation ->
CODE.2 CHARS-present startup -> `ExplorationRuntime::load_fresh`.

The incorrectly introduced optional preview SAVE.DAT substitution was
withdrawn; `--explore-level22` is only an opt-in diagnostic shortcut,
using actual CHARS when available and synthetic EXPLORER/GUIDE if absent.
It is NOT the final acceptance path.

### Original CODE.2 bootstrap verified directly, not inferred from a save

Original `CODE.2`, SHA-256
`853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`,
object-1 code `0x20608..0x20773`:

- `0x2060C: MOV EBP,0x16` => fresh level 22.
- `0x20616: MOV ESI,0x4`, `0x2061B: MOV EBX,0x10E` =>
  initial cell component 4, facing 270.
- `0x2074C`/`0x20755`: low coordinate words `0x8000`
  at mutable `0x255D0`/`0x255D4`.
- `0x2075E`/`0x20766`: high coordinate words from SI/DI,
  then `0x2076D: MOV [C658],EBX` heading 270.
- On the cold-fresh branch, DI receives SI after the setup
  call at `0x20625`; hence default fixed coordinates
  **`0x00048000 / 0x00048000`** (4.5,4.5).

The native constants match this source. Do not change starting
coordinates based only on screenshot layout; first verify camera
projection, original palette/horizon/ground raster, NPC live timing,
and saved-vs-fresh world initialization. Other dynamic startup
paths may override these values; inspect them separately.

The strengthened `exploration_session_tests` checks lossless
transfer of both full 0x248-byte actor records, equipment selectors,
portrait selectors, fresh coordinates and facing.

## Critical movement scheduling correction (unbuilt)

The live host had called `input.tick`/CODE.2 `2E120` movement
every VGA refresh (~70 Hz), independent of original game clock.
The DOS main game loop `201A7..201F7` waits for two game ticks
between movement/update cycles. `ExplorationGameplayCadence`
now gates only motion updates to a provisional
`2*54925us = 109850us` period while retaining VGA display
refresh separately. It prevents unbounded catch-up steps after
host stalls, and allows first-frame movement promptly. A
new assertion-enabled `exploration_gameplay_cadence_tests`
covers cadence, next-deadline, late frames, reset and separation
from intermediate VGA frames.

**Important fidelity boundary:** 54,925 microseconds is
the DOS BIOS timer fallback, not a measured guarantee of the
game's PIT/IRQ rate with every DOSBox configuration or installed
sound driver. Verify real input timing against an original DOS
run before classifying walking/turning parity as complete.

## Original NPC and placed-object collision is now connected

`2D5D0` performs party cell/record geometry checks and then
the original `2C800` NPC and `2CC88` class-1 object overlap
checks. The native movement path previously checked only cells
and accepted steps through living NPC/tree footprints.

- `2C800` source disassembly `0x2C800..0x2C8E1`:
  iterates `[0x258A2]` NPC count, skips dead/inactive
  byte `+0x0B & 0x02`, loads original NPCS.DAT
  collision half-extent `+0x16`, adds `0x3E8`,
  then tests both unsigned axes with inclusive JB/JBE
  bounds and returns blocking on overlap.
- `2CC88` `0x2CC88..0x2CEB5`: iterates class-1
  objects, loads original INIT 7-byte descriptor extent
  `+0x02`, skips `extent<=1`, then checks inclusive
  A/B source-coordinate overlap. Normal source types
  return one (blocked) on overlap.
- Type `0x54` is NOT an unconditional block or
  transition. `0x2CD76..0x2CE39` tests the live
  placed-object `packed_runtime_state & 0x1C0 == 0x40`
  AND level ID `C6C8 == 0x17` or `0x0A`.
  If either test fails, the original CONTINUES scanning
  other objects (returns no blocking for this one).
  The code now uses this exact predicate; no guessed
  transition is executed.
- Type `0x7A` separately calls special routine
  `0x3CAD4` and returns the special-action result.
  Missing scripted action remains explicitly suppressed;
  do not simulate an ordinary immovable object.

The live `ExplorationRuntime::tick_motion` connects that
collision callback through `ExplorationRuntimeMotionModel`
and `ExplorationLocomotionModel` after cell footprint geometry,
retaining original attempt order full -> Y-only -> X-only.
The original entity data loaded from NPCS/INIT and mutable
Level 22 world are used, not invented screen-space collisions.

New `original_entity_collision_tests` covers live/dead NPCs,
exact 0x3E8-inclusive footprint boundaries, type-62 trees,
nonblocking extent-one objects, 0x54 level/state predicate,
0x7A special-action preservation and locomotion's fallback
slide. Existing `exploration_runtime_motion_tests` checks
the callback reaches the integrator. **Both require CI
verification; no Windows test run since #640.**

Further original NPC AI, live entity movement, scripted transitions,
combat knockback and authored event side effects remain
unimplemented and must NOT be classified as completed collision.

## Forest graphics: proven original missing pipeline

The immutable CODE.2 object-2 initial `C026` byte at
file offset `0x81F4A` is `0x01`. Original CODE.2
`1F3CA` selects the textured lower-surface renderer
`0x34A74` when C026 is nonzero, rather than the flat
lower-fill `0x33DE0` used exclusively by today's
native presenter.

Direct disassembly of original `0x34A74..0x34BAC`
shows this is NOT a uniform grass color or simple image
copy. It uses mutable `C040/C044/C048`, temporaries
`FF40/FF44/FF48/FF4C/FF50`, an FF58 series of 6-byte
source steps, a FE94 lookup, and patches immediate operands
in its runtime-generated code. It emits up to 0x50
four-coordinate calls through `0x342E4`, with remaining
calls through `0x34954` when the source span exceeds 0x50.
These routines still need independent source-byte and
VGA-memory tracing before faithful native pixel output
can be implemented.

The source-backed v2 capture metadata labels
`original_C026_initial` and
`native_lower_surface=flat-fill-only;34A74-pending`.
Do NOT replace the forest floor with generated noise or
claim that successful RES3 tree draw calls prove
outdoor framebuffer parity. Additional verification
must cover tree silhouettes, depth order, sky/horizon,
character portraits/inventory and attack animations.

## Remaining acceptance work

1. Manually run current-HEAD Windows CI and fix any C++ or
   test failures before claiming the speed/collision/fresh-CHARS
   work is built; new tests should take total registered
   CTests from 160 to **162** in standard Python-enabled CI.
2. Observe original DOS fresh created-party input clock,
   compare party CHARS, first frame, camera and equipment
   without importing SAVE.DAT.
3. Trace and implement CODE.2 34A74 textured lower
   raster using original RES and source state; only then
   compare pixel-accurate DOS vs native screenshots.
4. Trace NPC motion/AI near Level22 Assassin at (13,7)
   and combat/hand animation state; prove encounter
   positions match comparable source ticks.
5. Wire original scripted collision effects, inventory
   actions, modal transitions and HUD state as tested.

**No newly accepted formal gate.** Formal demo score is
still **1/5 (20%)** and is not a whole-project
completion estimate.
