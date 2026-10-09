# DOSBox original screenshots — fresh-party fidelity acceptance reference

**Date:** 2026-10-08 · **Branch:** `demo-v1` · **Authority:** original DOS screenshots supplied directly by the player, original `CODE.1`/`CODE.2` executable code and original game data.

## Critical provenance correction from user

**The nine reference screenshots were taken after the player created a party in the original DOS game. They do NOT depict a game restored from the supplied quick-start `SAVE.DAT`.**

Therefore do not:
- import the provided `SAVE.DAT` active-party records as replacements for freshly created characters;
- declare an authentic fresh-start location proven merely because `SAVE.DAT` contains Level 22, A/B `0x00048000`, heading 270, and time 14:00;
- invent NPC positions from the static `LEVELS.DAT` record without preserving the post-startup live simulation state;
- use an opt-in `--explore-level22` synthetic-party shortcut as evidence of normal game-flow parity;
- claim the character portraits, starting items, or camera match the screenshots until a paired source state has been captured.

`--explore-level22` is a diagnostic shortcut. The **default** acceptance path is:

```text
Original CODE.2 cold start / intro / menu
  -> original CODE.1 character creation
  -> both completed actors (stats, classes, names, race, sex, portraits)
  -> CODE.1 5F4C/4EF0 and original starting equipment
  -> exactly 0x920 bytes of CHARS (four 0x248-byte records)
  -> R.EXE status-10 continuation / CODE.2 startup and CHARS import
  -> original fresh Level 22 world + clock + runtime NPC initialization
  -> normal first-person movement, inventory, map, encounter, UI
```

The Win32 source *already has a modeled handoff* in
`win32_main.cpp`: it serializes the two completed character drafts through
`build_character_transfer`, writes `CHARS`, calls `flow.finish_character_creation`,
`flow.supervisor_dispatch`, `flow.finish_code2_startup`, then directly
starts `run_exploration` using the transfer image. This is an implementation
route, **not proof of reference-DOS equivalence**. The original
`SAVE.DAT` test fixture is reserved for future explicit *saved-game*
loading, not substituted at this handoff.

The just-added `SAVE.DAT` fallback in the opt-in preview was withdrawn
immediately after the player's clarification. Preview mode now uses
actual `CHARS` when available, otherwise explicitly *synthetic*
EXPLORER/GUIDE diagnostic characters only. No default fresh-party path
imports saved-party bytes. A strengthened Release-enabled
`exploration_session_tests` asserts both actors, all `0x920` bytes,
portrait selectors, equipment IDs, starting-level fields and facing
survive the CODE.1->CODE.2 runtime entry unchanged.

## Supplied original reference scenes

1. First-person Level 22 forest immediately after game creation:
   orange sky, distant blue/green ridge, trees with varied shapes/foilage,
   foreground green/red textured grass, full decorative red/stone side frame,
   lower combat/movement HUD, two original created portraits.
2. Automap in "FORGOTTEN REALMS FOREST" with original legend and player
   marker. Use for starting map-cell, explored-state and terrain mask
   comparison after aligning actual party position—not a coordinate
   inference from an uncalibrated screenshot.
3. Four-column inventory modal: two fully created actors, body silhouettes,
   starting gear, inventory slots and item thumbnails. Empty remaining
   columns are rendered with original stone texture.
4. Multiple forest viewpoints as the party approaches an Assassin/NPC,
   followed by the large near-field sprite, combat overlays and damage
   presentation.
5. Original first-person weapon/hand animation frames, including skeletal
   hand/weapon art, overlaid on a distinct environment/ground scene.

These images provide an **important human-visible authenticity baseline**.
They do not provide synchronized 320x400 raw palette-index buffers,
original real-time input tick traces, or byte-for-byte matching initial
actor/world state: those are required before declaring pixel parity.

## Confirmed demo deficiencies and correction status

| Domain | Proven issue | 2026-10-08 response | Acceptance |
| --- | --- | --- | --- |
| Simulation pacing | Host called CODE.2 movement integrator at each ~70Hz VGA refresh; DOS main loop anchors a two-game-IRQ-tick wait at 201A7..201F7 | Source splits host present rate from movement cadence; uses documented BIOS tick fallback 54,925us * 2 = 109,850us **provisionally** | Await fresh manual build AND measured DOS timing |
| Static/cell collision | Native 2D5D0-based model checks cell words/record geometry but previously skipped original entity checks | Source adds original 2C800 live NPC and 2CC88 class-1 placed-object footprint checks, after cell geometry | Await build plus authentic outdoor blocked movement and side effects |
| Fresh party and equipment | Direct shortcut uses synthetic two-actor image; newly generated party must retain CODE.1 starting items | Withdrawn SAVE substitution; default original CHARS handoff preserved; byte-for-byte two-actor test expanded | Await live creation-to-first-person original screenshot match |
| Starting location / heading | Code uses fresh-party constants currently tied to documented CODE.2 initialization; player reports first-person camera mismatch | Keep source constants pending original dynamic start-state trace | **Unresolved, do not tweak by eye** |
| Assassin NPC | Static Level 22 world NPC records do not automatically simulate time, AI motion, orientation, combat frames | Preserve original live record identity; no guessed spawn displacement | **Unresolved** |
| Trees | Source RES3 scene dispatch executes but view-angle, scene order, depth masks/foreground and draw phase do not yet match original screenshots | Audit first actual DOS-aligned images and original CODE.2 instructions | **Unresolved** |
| Forest floor | Native presenter always draws CODE.2 flat `33DE0` lower fill; original `1F3CA` conditionally selects textured `34A74` when original C026 indicates it | Opt-in capture now labels `original_C026_initial` and `native_lower_surface=flat-fill-only;34A74-pending` | **Unimplemented original textured raster** |
| HUD/inventory/hand animation | Native models some original resource/layout paths, not seamless CODE.2 modal/combat/weapon sequence parity | Reference pictures now explicitly tracked; do not use saved-party substitution to conceal inventory gap | **Unresolved** |

**Do not count the initial 160/160 CTests, the previous 1/5 formal acceptance gates, or Level 22 source-record totals as visual fidelity closure.** These are separate kinds of evidence.

## Follow-up implementation gates

- First obtain a repeatable original DOS **new-game** recording from character selection through first exploration, with original chosen character details, exact camera 16.16 coordinates or game debugger trace, original palette indices, and sound/IRQ timestamps when practical.
- Compare the final CODE.1 two character records against the native `CHARS` image and the resulting CODE.2 party memory before any inventory UI tests.
- Validate starting Level 22 cell, facing, time, world NPC location and animation state from the **actual freshly created game**, not the supplied quick-start `SAVE.DAT`.
- Implement `34A74` textured lower ground (original texture source, row/column selection, shade, VGA masks), not a fabricated procedural forest.
- Compare reference DOS and native output at the same orientation/coordinates, separating exact palette-index mismatches from simulation-state differences. Enforce original sprite/tree layering and occlusion.
- Complete original NPC/placed interaction side effects and actual combat/weapon animation order. Verify inventory and automap independently.
- Build new source on **manually triggered** GitHub Actions; keep Linux optional/off. No automatic builds or unsupported 100% claims.

**Formal accepted demo gates remain 1/5 (20%).** This score reflects official acceptance only; it does not represent the total game engineering percentage.


## Subsequent original-binary proof and collision hardening

[See the source and gameplay checkpoint](ORIGINAL_FRESH_START_TIMING_COLLISION_AND_GROUND_AUDIT_20261008.md).
Actual `CODE.2` bytes `2060C..2076D` establish the
fresh-party default cell (4.5,4.5), level22 and heading270
*independently of the unrelated SAVE.DAT*. The original
entity collision model now respects the 0x54 object
level/state gate and nonblocking fallthrough, rather
than treating any overlap as an action. Pending manual
Windows build, actual DOS IRQ timing and full 34A74
textured-floor raster.
