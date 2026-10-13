# Original Ravenloft native recreation — Alpha / Beta / Final gates

**Owner branch:** `demo-v1`. **Current classification:** **Pre-Alpha**.
**Controlling charter:** [original fidelity policy](ORIGINAL_EXECUTION_FIDELITY_CHARTER.md).
**Machine-readable gate register:** [RELEASE_GATES.json](RELEASE_GATES.json).
All release milestones are for **one continuous complete original game**.
The retired standalone Level22 developer preview and separate
`native-engine` Alpha-0.6 technology preview cannot qualify the DemoV1
original-game Alpha. Modernization remains prohibited until Final fidelity
acceptance. No target calendar date is promised or inferred from test count.

## Source/build evidence baseline (updated 2026-10-13 ET)

- Latest verified exact-checkout Full Windows:
  [#676](https://github.com/Hyperbog/RLOFT/actions/runs/38346121349),
  source `b17871c9ac2350afd0c4667de78a2aca059a5bad`,
  **257/257 CTests passed**. This is automated build evidence,
  not the original-game opening field acceptance.
- Player-facing artifact remains
  `Ravenloft-Original-Intro-Windows-x64` with
  `Play-Original-Intro.cmd` read-only startup showcase.
  It intentionally stops before writing CHARS or entering Level22.
- Unflagged ordinary host does contain CODE.1 → serialized CHARS →
  CODE.2 → Level22 startup, and its disk-read source/CTest were
  repaired in Part16. The entire path has NOT completed owner field
  acceptance or matched original DOS gameplay.
- Provisional **49.8% weighted engineering readiness** from an
  October 9 historical 20-system/122-subsystem analysis is NOT game
  completion, a release grade or justification to mark any gate met.
  The old historical “1/5 formal gates” scorecard is a separate
  pre-release evaluation, not the new Alpha/Beta/Final status.

## Evidence rules — independent, additive

| Evidence type | Meaning | Cannot substitute for |
| --- | --- | --- |
| `source_oracle` | Pinned original file+patch, original instruction/resource/data identity and audited translated behavior | Runtime correctness |
| `windows_ci` | Exact source SHA clean Full native Windows compile, all registered CTests and published artifact | Live gameplay / original DOS comparison |
| `native_field` | Protected-copy ordinary Windows playthrough trace with source SHA, initial party/state, steps, results and honest first blocker | DOS fidelity |
| `dos_comparison` | Matching original DOS scene/event/state/tick/RNG/VGA/audio observations with version, setup and reproducible differences | Other unsampled game branches |
| `package_audit` | EXE/artifact SHA, original-free files, selected owned-install launcher and clean-install provenance | Game progression |
| `owner_acceptance` | Recorded sign-off of the explicit milestone scope and remaining known differences | Source/oracle/test records |

Evidence records must pin the **full exact candidate source SHA** and
a concrete reviewable reference; an anonymous percentage or a log
reading “250/250” is not a DOS comparison. The manifest validator
checks **presence, structure and ordering**, not the authenticity
of external evidence: release acceptance requires manual review of
each record. Release state cannot be advanced by changing a label.

## ALPHA — the opening sequence and first playable Level 22 scene

**Alpha is NOT an entire level or the game ending.** The first internal
Alpha is the authentic cold-start pipeline and the **first controllable**
original level. Work in this source-order (the original starts with
CINE00/CINE12, then DreamForge/SSI/logo stages; do not reorder
presentation simply because splash and intro are discussed together):

1. Play real `CINE00.ANM` and `CINE12.ANM`, including source-audited
   timing, sound, skip/quit and return behavior.
2. Display all original splash/logo/title frames, original palettes,
   music, animation and fades in correct sequence.
3. Make the animated title screen and **all four original menu actions**
   work: replay intro (index 0), original Load Game (1), CODE.1
   character creation (2), Quit (3). Incomplete actions are displayed
   as unavailable, **not** silently ignored or faked.
4. Recreate complete two-actor CODE.1 creation UI, choices, stats,
   names, portraits, cancel/edit/confirm and music.
5. Write original CHARS only on normal creation, return through R.EXE
   CODE.1→CODE.2 supervisor status 10, and reread original 0x920
   bytes from disk before existing-party import.
6. Render the **first Level 22 scene** in normal gameplay using
   original textures, palette, HUD and characters; enable basic
   movement, rotation, collision, UI and initial interactions.

**Alpha acceptance:** all ten `ALPHA-*` gates in the
[ledger](RELEASE_GATES.json), a clean reproducible exact-source
Windows build, no developer warp/synthetic party, a protected-copy
user field test of the real opening→first game scene, and matched
original-DOS captures for the corresponding frames/inputs. Existing
CHARS legitimately skips the cold-start intro in source CODE.2;
test both fresh and returning party without falsifying that rule.

**Explicitly NOT an Alpha requirement:** the entire Level22 Assassin
fight, four relic pickups, complete puzzles/quest, full spell system,
22-frame mist scene, destination Level23, full world SAVE/LOAD and
the ending. Those belong to **Beta's whole-game campaign**, and
source-only audits of them are not Alpha acceptance evidence.
Prioritize boot/title/creation/first-scene defects over more
isolated Level23 or advanced VGA research.

**Deliverable after acceptance:** a versioned internal
`Ravenloft-Alpha` that starts the original game correctly and lets
a new party enter and operate in Level22. The read-only intro
showcase and old Level22 warp artifact are not Alpha.

## BETA — complete original campaign playable

Only after a working Alpha, complete the entire first level
(encounter, inventory/relic flow, scripted triggers, mist
transition and original Level23 entry), then every subsequent
authored level and required original story route to the real
ending. Preserve the real two-code-module startup and original
SAVE/LOAD behavior across the campaign. Complete combat/AI,
spell preparation and effects, equipment, world events and
audio/graphics in the same engine; never ship disconnected
per-level prototypes.

**Beta acceptance:** all ten `BETA-*` gates and the Alpha
opening-sequence gates remain accepted at the Beta source SHA.
A protected-copy campaign playthrough with independent
DOS comparison, full-world SAVE/LOAD and no campaign-blocking
or save-corrupting defects is required. Remaining non-blocking
DOS mismatches must be documented, reproducible and prioritized.

**Deliverable:** a complete original-campaign playable Beta
candidate, not a fidelity-final release.

## FINAL — full original-DOS fidelity and release acceptance

Resolve material Beta differences across **all original content**
and representative branches, then accept original game behavior
and presentation: indexed VGA/page masks, exact original data and
art, sprites and clipping including `22220→15618→FF50`,
palettes, UI, animation timeline, mouse/keyboard hardware semantics,
game RNG, interrupts/timing, sound/music events and
original SAVE/LOAD, ending variants and edge cases.

**Pass criteria:** all ten `FINAL-*` gates, Beta and Alpha
remain accepted on the candidate; independently review original
DOS-vs-native frame/state/clock/audio differential reports,
full game-campaign captures, original-file version identities,
Windows clean-build/packaging SHA and owner approval. No
copyrighted game media in public source or downloadable artifact.
Only **after** faithful Final acceptance may a separate branch
propose graphics/QoL/modding modernization.

**Deliverable:** `1.0`-class original-faithful release, with an
immutable tagged candidate SHA, installed-original-data chooser,
reproducible verification and retained original fidelity baseline.

## Pre-Alpha normal-route field runner (source checkpoint)

An opt-in **separate** `Ravenloft-Alpha-Field-Test-Windows-x64`
artifact is now built alongside the existing player-facing intro
package. `Test-Alpha-Continuous-Game.cmd` invokes PowerShell,
requires an explicit original media folder selection, copies its
complete directory tree to a fresh local
`%TEMP%\\RLOFT-Alpha-Field\\<unique-ID>` sandbox and launches
`ravenloft_demo_v1.exe --dev-trace "<SANDBOX>"`.
The user's original folder is **never the executable's argument**.
Cold-start mode removes CHARS only in that sandbox; existing-party
mode leaves CHARS as copied. SAVE.DAT is not altered in the source.
This is a host diagnostic tool with no reconstructed game logic
changes, and it does not advance any ALPHA-* gate automatically.

A bounded read-only CI contract checks copy-before-execute ordering,
original save-file safety, source provenance, source-only mutation
coverage and separate artifact staging. A genuine Win32 field session
and DOS original-versus-native acceptance still need independent
evidence. The sandbox must be manually removed after testing, and
locally created diagnostic reports can include character names.
The launcher rejects selecting the temporary root or an ancestor
and refuses directory reparse points to prevent recursive copying
or accidental access beyond the selected original media tree.

## Immediate execution order — startup first

1. Verify normal CHARS-absent boot into original CINE00/CINE12 and
   DreamForge/SSI/title splash presentation (frames, audio, palette
   and escape handling). Fix the first real player-facing failure.
2. Restore all original title actions and input selection, including
   CINE00 replay and authentic 7B94 Load Game without fake progress.
3. Complete CODE.1 character creation end to end, including original
   actor choices, stat edits/confirm, names and cancel flow.
4. Test the actual CODE.1→R.EXE→CODE.2 CHARS serialization/re-read.
   Test existing party separately; preserve source-authentic skip.
5. Make the **first** Level22 scene render correctly, then basic
   movement, collisions, HUD, controls and first interactions.
   Match native normal gameplay to original DOS before Alpha.
6. Only **after Alpha**, finish the complete first level's quest,
   Assassin encounter, relics and original mist/Level23 transition,
   and proceed through the campaign for Beta.
7. After Beta, close complete DOS framebuffer/state/audio/timing,
   menus, errors and patch-scope fidelity for Final.

## Reporting discipline

At each build note current `demo-v1` HEAD, **CI checked-out
source SHA**, CTest results, real Win32 field coverage, compared
DOS version/captures and open blockers independently.
`tools/check_release_gates.py` is intentionally read-only.
`--self-test` tests the release ledger against seven deliberate
invalid mutations. A green CTest for that tool means the
manifest is *honest*, not that Alpha was achieved.
