# Original Ravenloft native recreation — Alpha / Beta / Final gates

**Owner branch:** `demo-v1`. **Current classification:** **Pre-Alpha**.
**Controlling charter:** [original fidelity policy](ORIGINAL_EXECUTION_FIDELITY_CHARTER.md).
**Machine-readable gate register:** [RELEASE_GATES.json](RELEASE_GATES.json).
All release milestones are for **one continuous complete original game**.
The retired standalone Level22 developer preview and separate
`native-engine` Alpha-0.6 technology preview cannot qualify the DemoV1
original-game Alpha. Modernization remains prohibited until Final fidelity
acceptance. No target calendar date is promised or inferred from test count.

## Source/build evidence baseline (2026-10-12 ET)

- Most recent verified exact-checkout Full Windows:
  [#667](https://github.com/Hyperbog/RLOFT/actions/runs/38340609115),
  source `11d1df416ea41a46b40ed55cb40f89e7c95ac636`,
  **250/250 CTests passed**. Subsequent committed EXE refresh
  `fabca2bf` has no gameplay source change. This is an engineering
  regression achievement, NOT original-DOS or normal-gameplay acceptance.
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

## ALPHA — internal continuous Level22 acceptance, NOT standalone release

The complete normal path must function: original cinematics/title
and all supported menu actions → CODE.1 or existing-party CHARS
→ CODE.2 world → authentic initial Level22 viewpoint and HUD →
movement/collision/input/events → original combat/NPC and four
Assassin drops → inventory/relic/puzzles → 22-frame mist ending
→ original `181B4(23)` frame-20 destination entry, preserving state.

**Pass criteria:** every ten `ALPHA-*` gate in the manifest,
a reproducible full Windows candidate build, player/owner
field trace on a protected original-install copy and independent
original-DOS differential checks for the path, with no developer
warps, fabricated SAVE party, arbitrary level IDs or source
unknowns disguised as finished gameplay. Source analysis of
the Level23 *entry boundary* is permitted for Level22's ending;
independent Level23 campaign feature work starts only after
Alpha's Level22 completion gate.

**Deliverable after acceptance:** versioned internal
`Ravenloft-Alpha` candidate with manifest, protected-install
instructions and identified original-media version. Do not
rename the intro showcase or old Level22 preview “Alpha.”

## BETA — complete original campaign functional

The same runtime follows **every gameplay-required original
level/quest route** from the real title to the actual original
ending without fake story transitions, developer keys or per-level
mock programs. All original game systems needed for the
campaign operate across levels: complete UI/menus, original
combat/enemy AI, magic and spell preparation, object/equipment
rules, levels/doors/portals/events, authored cinematics, sound,
clock/IRQ and compatible full-world SAVE/LOAD and reentry.

**Pass criteria:** all ten `BETA-*` gates and all Alpha
gates remain accepted at the Beta candidate source revision.
Whole-game normal-play field session(s), sampled independent
DOS comparison and no known campaign-blocking or
save-corrupting defects. Remaining non-blocking DOS fidelity
differences must have explicit repro, source address, impact
and tracked ownership. No game-facing modernization.

**Deliverable after acceptance:** Beta build with all-game
reproduction instructions, regression package and honest
known-differences report.

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

## Immediate execution order (no arbitrary calendar dates)

1. Finish original-DOS and Win32 normal-launch/CHARS/first Level22
   frame field comparison; protect saved CHARS and SAVE.DAT by using
   a separate installation copy. Preserve actual first blocker.
2. Source-verify and integrate VGA `ES:EDI` selector/page side
   effects and cached `22220→15618→FF50` producer/callback
   state; do not use the read-only FF50 model as live clipping
   without original reference frames.
3. Resolve normal Win32 scan stream → F590/F572 DOS IRQ path,
   movement and gameplay cadence using original DOS clocks.
4. Complete real Level22 event/combat/ordinary inventory/relic
   pipeline and original mist transition; accept Alpha route.
5. Only then extend ordinary original campaign gameplay in authored
   order, with complete world SAVE/LOAD and source-proven systems
   through the ending; accept Beta.
6. Close all remaining original-DOS video/state/clock/audio
   equivalence and patch/version scope; accept Final.

## Reporting discipline

At each build note current `demo-v1` HEAD, **CI checked-out
source SHA**, CTest results, real Win32 field coverage, compared
DOS version/captures and open blockers independently.
`tools/check_release_gates.py` is intentionally read-only.
`--self-test` tests the release ledger against seven deliberate
invalid mutations. A green CTest for that tool means the
manifest is *honest*, not that Alpha was achieved.
