# Ravenloft Demo Build v1

## Project-wide timing fidelity — mandatory

**Walking pace, turn response, animation frame dwell, card flips, combat
recovery, NPC reactions and story transitions must match the original game's
timekeeping**, not merely its final graphics or state. See the
[Timing Fidelity Contract](TIMING_FIDELITY_CONTRACT.md), grounded in
[Preservation Archive Chapter 17](../PreservationArchive/17_TIMING_INTERRUPTS_AND_FRAME_CADENCE.md).

The original main loop has an anchored two-game-tick target
(`201A7..2022B`, `2E608/2E614`). This must be reconciled with the native
host's nominal 70.0863-Hz **display** refresh; the two are not interchangeable.
No universal milliseconds-per-tick claim is made without measured original
runtime evidence. Every new gameplay subsystem requires explicit timing,
interruptibility, audio/animation ordering and regression acceptance.
Overall five-major-gate demo completion remains **20%**; cadence equality is
not yet certified.

## Private-repository development and build policy

**Automatic GitHub Actions Windows builds are disabled on `demo-v1`.**
`.github/workflows/demo-v1-build.yml` has **manual `workflow_dispatch`
only**, to conserve private-repository Actions minutes. Continue committing
faithful gameplay reconstruction, regression tests and archival research
without repeatedly building preview binaries. Re-enable push builds only
after explicitly deciding to do so when the repository becomes public.

The **last previously tested Windows executable** is from
[build #631](https://github.com/Hyperbog/RLOFT/actions/runs/37719712911)
(102/102 CTests), but it **predates** the recently verified correction
of the hand readiness offset and the new combat-entry/target-selection
source. Do not treat its tests as validation of current HEAD.

The current source-only work includes
[original attack/target preflight and offline verification](LEVEL22_COMBAT_ENTRY_CHECKPOINT.md).
An original-asset offline byte audit now covers **117 original CODE.2
machine-code anchors**. Nine new combat C++ test targets are registered,
but **have not been executed** since the private-repository build pause.
The [conditional 24478 hand-eligibility model](LEVEL22_HAND_PAIR_RAW_HEAP_MODEL_CHECKPOINT.md)
preserves the original requirement for observed DOS heap-adjacent
bytes; it is not yet connected to live attacks.
This deliberate pause does not alter the **overall faithful-demo
acceptance: 20% (1/5 release gates)**.


## Playable Level 22 walkaround preview (opt-in)

**Windows x64:** use the newly added `Explore-Level22.cmd` packaged next
to `ravenloft_demo_v1.exe` in the latest Windows Actions artifact. It
loads the actual Level 22 without requiring the intro or character-creation
sequence. If CHARS is present it is used read-only; otherwise the preview
uses temporary in-memory EXPLORER/GUIDE characters. Your installed game files
are not saved or modified by this mode.

Use arrow keys or the numeric keypad to move and turn; press Esc to exit.
Alternatively run `ravenloft_demo_v1.exe --explore-level22` (optionally
followed by a quoted installation path). The normal no-flag boot flow is
unchanged. The original world/environment is loaded, but incomplete
main-wall rasterization can leave some foliage/walls/scene pixels absent.
The preview is not a full gameplay demo.

[Walkaround instructions, implementation and acceptance checklist](EXPLORATION_WALKAROUND_CHECKPOINT.md).
**Initial walkaround checkpoint: 100% (5/5 bounded gates)**:
[Windows build #602](https://github.com/Hyperbog/RLOFT/actions/runs/37716513471)
passed 101/101 CTests and published the direct-entry EXE with a launcher.
User-provided Windows screenshots subsequently confirmed original-asset outdoor
scenery, HUD, expanded top menu and automap. These captures also show an
unresolved black-background character panel and questionable automap/modal
background isolation. Locomotion fidelity, wall rasterization, complete
gameplay and original DOS indexed-frame equality remain unverified.
Overall faithful-demo acceptance is still **20% (1/5 major release gates)**.


Goal: reproduce the original shipped behavior from boot through character creation while reading assets from a legally installed/owned copy of *Ravenloft: Strahd's Possession*.

## Original E5F0 target selection and attack timing — source checkpoint

[Attack target and timing checkpoint](LEVEL22_ATTACK_TARGET_AND_TIMING_CHECKPOINT.md)
now reconstructs the original explicit-target-vs-`DB94` preference
order, special rear-party `+0x35 & 3 == 2` restriction and action
recovery based on the **full original OBJECTS WORD +0x35 bits 2..5**.
The result is **9 × ((word >> 2) & 15)** gameplay maintenance ticks
for a regular item, or 18 for an empty/pseudo hand, optionally halved
by the original actor effect mask. On a successful hit the original
`E5F0` stores **A7E4 + half the interval** as the *hit-only*
hand-ready deadline and records the resulting damage. Its next call,
original CODE.2 logical **`24B48`**, presents the hand action and,
when the interval is nonzero, stores a **second, miss-inclusive**
action-busy deadline using the **later A7E4 reading + full interval**.
The two dwords and clock observations cannot be merged.

The new native routines are **side-effect-free**, consume the
**captured dispatched item ID** as original E5F0 does, and do not
change the character state, choose invented damage or run animations.
Timing may not be equated with VGA refresh or host milliseconds.
Both new C++ regression targets await a deliberate consolidated build;
21 additional original executable signatures passed local byte audit.

**Bounded E5F0 target/timing source checkpoint: 67% (4/6)**.
Overall faithful-demo acceptance remains **20% (1/5 release gates)**.

## Full 24478 pair decision — conditionally reconstructed

The [raw-heap-conditioned original hand decision model](LEVEL22_HAND_PAIR_RAW_HEAP_MODEL_CHECKPOINT.md)
now accounts for ordinary, pseudo, both-empty and mixed negative-item
`24478` paths **as a side-effect-free candidate calculation**.
A negative item reads raw bytes before the original allocator base;
the model demands the actual observed original category alone for
both-empty, or category/weight/qualifier for a mixed pair.
This does not make unarmed attacks playable: raw DOS heap values,
attack/use event ordering, hit/damage, audio and recovery timing are
unverified.

**Bounded original pair-decision source checkpoint: 67% (4/6 gates)**.
Overall faithful-demo release acceptance remains **20%**.

## Original 24478 empty/pseudo short circuits

The [empty/pseudo early-gate checkpoint](LEVEL22_HAND_PAIR_EARLY_GATES_CHECKPOINT.md)
models original `244CE..246E9` and distinguishes its first-actor
`status >= 0x4000` threshold from `240A8`'s **separate**
`status == 0x8000` branch. The `24478` pair evaluator now
supports **both hands empty**, but only with the original observed
OBJECTS pre-base category byte, or **both hands holding pseudo
spell items `0x7E..0x86`**; both shortcut directly to their distinct
action-state busy checks. Mixed **positive pseudo+ordinary** pairs follow the normal
qualifier/weight arbiter after pseudo overrides. A mixed `-1`
with any nonempty hand can also be **modeled** when its actual
original allocator-adjacent category, weight and qualifier bytes
are supplied explicitly; absent evidence is rejected.
[Original heap-conditional pair checkpoint](LEVEL22_HAND_PAIR_RAW_HEAP_MODEL_CHECKPOINT.md)
documents the exact three byte offsets. The authentic DOS heap values
and temporal combat execution remain outstanding. No attack is enabled and current source
has not been compiled since private CI was paused.

## Original 24478 ordinary two-hand arbitration

The [ordinary two-hand arbitration checkpoint](LEVEL22_HAND_PAIR_ARBITRATION_CHECKPOINT.md)
originally modeled CODE.2 `24478` for **two ordinary positive equipped items**; the later [conditional full pair model](LEVEL22_HAND_PAIR_RAW_HEAP_MODEL_CHECKPOINT.md) now extends that source to the original pseudo and `-1` branches,
using the original independent `240A8` eligibility/classification results,
literal OBJECTS `+0x35 & 3` and `+0x30` weight fields, character class
groups, `23EE4` pair exemptions, and each hand's `+0x10B/+0x10F`
action-state busy gate. The source/tests are registered for an eventual
manual build; **there is no newly verified Windows binary or playable attack**.

The original loader at `268C8` allocates OBJECTS through a wrapper
returning a pointer 10 bytes into an allocation. `240A8(-1)` reads the
pre-table byte `OBJECTS_POINTER-0x12`, *outside* the allocated object
record and even before the low-level allocation base. The helper can
model the empty-hand decision only when that actual raw byte is supplied.
The full pair model now handles `-1` and pseudo items **conditional
on all raw original bytes actually read by the selected path**.
Original heap behavior and game-time action pacing remain separate
research gates.

**Bounded ordinary pair checkpoint: 60% (3/5 gates)**. Overall faithful
demo release acceptance remains **20% (1/5 gates)**.

## Original 240A8 candidate and 24478 hand legality — source-only progress

The latest [single-hand eligibility checkpoint](LEVEL22_SINGLE_HAND_ELIGIBILITY_CHECKPOINT.md)
implements original `240A8` for **nonnegative object IDs** and permits the original
empty-hand (`-1`) branch **only with the true OBJECTS pre-base category byte**,
including its
`33FC4/34060` exception sets, signed class and level gates, two original
spell-ID intervals, party-mode rules and the literal OBJECTS `+0x35 & 3`
qualifier. Its regression target is registered but not yet compiled.

The source deliberately **does not invent a default `240A8(-1)` unarmed category**:
the original helper reads memory before the allocated OBJECTS buffer.
The original loader's pointer arithmetic has now been recovered, but
the allocator-adjacent **runtime byte value** is still unknown. Conditional
`24478` arbitration now supports `-1` and pseudo hands; **capturing the real
original heap values and connecting timed attack input** remain prerequisites
to live attacks.
The [offline original-byte audit](tools/verify_combat_entry_bytes.py)
registers **87 exact opcode anchors**, including the 15 newly audited `24478` empty/pseudo short-circuit instructions (both original party-status checks remain distinct) matched
against original CODE.2. This is source evidence, not a new game build.

**Bounded nonnegative `240A8` checkpoint: 75%** (compiled/original-runtime
comparison pending). **Overall faithful-demo release acceptance: 20%**.

## Level 22 live HUD item handling (original CODE.2 214B8 / 211D8)

The [live HUD hand and actor-panel checkpoint](LEVEL22_HAND_INTERACTION_CHECKPOINT.md)
extends ordinary full-inventory swaps into the playable world UI. Supported
normal-gameplay **right-click hand tiles** exchange their held items, preserving
item identity, the identified bit and the original selected-hand ready-deadline
reset. A **right-click on the actor inventory panel** places an ordinary
held item in the actor's first free ordinary slot without opening the modal.
An inventory-full result leaves the item safely on the cursor. These native
right-click bindings are provisional pending DOS event-flag trace confirmation.

The original `0F238` known-bit early return and `211D8` direct-stash
known-bit non-update are preserved rather than replaced by generic inventory
semantics. Special equipment, container and spell-hand branches remain
explicitly unsupported. The original `214B8` separate use/attack path is
not yet enabled: original target selection, damage, cooldown and feedback
must be reconstructed first.

**Hand interaction checkpoint: 60% (3/5 gates) on current source**,
pending a deferred manual CI rerun and installed-game playtest. Historical
[Windows CI run #631](https://github.com/Hyperbog/RLOFT/actions/runs/37719712911)
passed 102/102 for the *earlier binary*, but did not validate the later
`+0x19E` readiness offset correction. Overall faithful demo: **20%**.

## Level 22 functional gameplay integration (ordinary inventory)

The next [inventory interaction checkpoint](LEVEL22_INVENTORY_INTERACTION_CHECKPOINT.md)
adds **original CODE.2 12E70 ordinary 3x4 slot clicks** and the bounded
`117A0` held-item/identified-bit exchange to the existing four-party inventory
modal. Players can now take a previously picked-up loose item from the held
cursor into an empty normal inventory slot, freeing the cursor for another
pickup. The model tests also cover the full sequence of three Assassin relic
pickups/stashes followed by the fourth item completing the original Level 22
possession predicate.

Temporary spell-hand objects, occupied container insertion, equipment and
hand action rules remain gated. Combat and the 22-frame Level 23 transition
are not yet enabled. Automated Windows compilation and an original-asset
inventory interaction playtest remain separate acceptance gates.

**Inventory subcheckpoint: 83% (5/6 bounded gates)** after Windows 102/102 CTests; real gameplay interaction remains unverified.
**Overall faithful-demo release acceptance: 20% (1/5 gates)**.

## Level 22 combat accuracy — original hit test

The separate [CODE.2 C80C combat hit-check checkpoint](LEVEL22_COMBAT_HIT_CHECKPOINT.md)
implements the original ordered player-to-NPC threshold calculation as a
bounded, deterministic native primitive: held-target auto-hit, weapon and
Strength adjustments, burden, Slow, Bless/Aid, Stone Giant Gauntlets and
the original **modified-d20** automatic-hit/miss rules. Synthetic tests
are registered in CMake, but the input command, hit target, RNG, damage,
weapon category gate, NPC reaction and hand cooldown have **not** been
connected. Attacks are intentionally not dispatched by arbitrary clicks.

**Hit-check arithmetic subcheckpoint: 100% (3/3 bounded gates)** after
Windows 102/102 CTests; full combat is still unimplemented and overall
faithful-demo acceptance remains 20%.

## Extended integration milestone — Level 22

The demo's original boot/character-creation milestone has been extended to
**all systems integrated for the Level 22 Assassin/relic sequence**.
The earlier boot/controller documentation below remains valid but no longer
describes the full current scope.

The original SINTAB 127-block rasterizer interpreter, original RES3 type-7
pixel execution path, and whole-scene VGA trace integrity are documented in
[TYPE7_RENDERER_CHECKPOINT.md](TYPE7_RENDERER_CHECKPOINT.md).
The separate original CODE.2 surface-prepass investigation and its
signed/unsigned SINTAB underflow boundary are tracked in
[SURFACE_PREPASS_CHECKPOINT.md](SURFACE_PREPASS_CHECKPOINT.md).
A **transactional SINTAB pixel executor is now connected to supported
RES4 surface prepass runs**, with per-run diagnostics and pending coverage.
Ordinary SINTAB main-pass wall rendering and DOS-frame equivalence remain open.
Viewport-selected Assassin damage and loose-item pickup bindings, including
snapshot-invalidating world state, are documented in
[LEVEL22_VIEWPORT_ACTION_CHECKPOINT.md](LEVEL22_VIEWPORT_ACTION_CHECKPOINT.md).

**Do not label the demo finished yet.** Ordinary SINTAB wall rendering, final
original-frame equivalence, remaining host combat/inventory-input bindings,
and relic-transition integration remain open.
Development commits continue on `demo-v1`; the Windows CI workflow handles
native-source changes while Linux is manual opt-in.


## Certified preservation reference

The complete original-engine preservation documentation is mirrored as
[PreservationArchive/](../PreservationArchive/00_SCOPE_EVIDENCE_BASELINE_AND_OPEN_QUESTIONS.md)
from the pinned `code1-decompilation` source, including the 22 completed
static chapters and the separate 91-task future research register. See
[PRESERVATION_INTEGRATION_ROADMAP.md](PRESERVATION_INTEGRATION_ROADMAP.md)
for the original-code-to-native source map, open acceptance tests, and the
resolved two-routine CODE.2 reconstruction reconciliation across branches.
Both branches now share the original-byte-corrected `Code2/Code2.cpp` blob
`87f6638921c53a32bde59735515fbc175e2c96ee`; see
[the recorded CODE.2 errata](../Code2/POST_CERTIFICATION_ERRATA.md).
The historical 2026-10-05 source-certification hashes remain provenance
for the pre-correction revision, not the present source.

The native demo is a **separate** workstream. GitHub Actions run #572 passed
full Linux and Windows compilation/tests (99/99 Linux tests; Windows
executable published), but original-frame equivalence and the full Level 22
Assassin/relic/transition gameplay remain unverified. Automatic Linux jobs
are intentionally paused to save Actions minutes; Windows builds and tests
remain available for native changes.

## Asset policy

The demo does **not** bundle Ravenloft game data.

Normal use is:

1. locate an installed Ravenloft copy automatically;
2. identify the original media source;
3. read/extract only the files required by the native runtime;
4. preserve original resource IDs, palette indices, timing, and data semantics.

A pre-extracted resource folder remains supported for development and preservation testing, but it is not the intended end-user requirement.

## Installed-copy discovery

The native source layer currently supports:

- the GOG Ravenloft product registry entry (product ID `1432905053`) on Windows;
- `RLOFT_GAME_DIR` as an explicit override;
- common GOG Games / GOG Galaxy roots;
- common Steam roots;
- a directly supplied game installation directory;
- a directly supplied `game.gog` path;
- a loose directory containing Ravenloft `RES0`, `INIT`, and `T`.

Candidate installations are accepted only when Ravenloft-specific media anchors are present.

## game.gog

GOG's installed CD image can be consumed directly. The demo has a small read-only ISO-9660 reader and does not require 7-Zip or a manual extraction step.

Supported data-sector layouts:

- 2048-byte ISO sectors;
- 2352-byte raw Mode-1 sectors with 2048-byte user data;
- 2352-byte sectors whose ISO user data begins at offset 24.

ISO version suffixes such as `;1` are normalized during lookup. The indexer walks subdirectories so resource files do not need to be at the image root.

## Runtime-side files

For files that belong to the writable installed game rather than the CD resource image, lookup checks both:

- the installation root;
- the installation root's `RLOFT` directory.

This keeps the original distinction between installed runtime state and CD/resource media available to later compatibility work.

## Current boot milestone

The binary-proven transition implemented so far is:

```text
R.EXE supervisor
  -> CODE.2 startup
  -> CHARS-absent cold-start presentation
  -> CODE.2 four-entry startup menu
  -> menu index 2
  -> legacy result 10
  -> R.EXE launches CODE.1
  -> character-creation workflow
```

The native RES0/COLORS cold-start renderer and startup-menu presentation are implemented. The Windows host also composites the original Ravenloft cursor from the selected installation's RES0 data rather than using a host-system pointer.


## CODE.1 character-creation boundary

Direct re-disassembly of the certified CODE.1 object closes the first three
controller states:

```text
state 0  0x1CD0  choose one of the two original player-character slots
state 1  0x1DD8  sex selection
                  result 3 -> sex 0 / MALE
                  result 4 -> sex 1 / FEMALE
state 2  0x1E64  race selection
                  results 1..6 -> race codes 0..5
```

The state-0 presentation is now rendered from the installed game data.
CODE.1 loads COLORS palette 8 before the controller. Its first screen uses
RES7 graphics, including the two vertically emitted background strips at
RES7:24/25, the fixed RES7:30..38 control family, RES7:42, and the two
state-0 selector graphics.

The observable CODE.1 opening presentation at `0x45FC..0x4976` is implemented,
including its 70 displayed frames, palette reveal, opening music ramp, and
RES6 one-shot sound cues. The reconstructed controller then continues through
the character-creation workflow, its transition animations, stat generation
and editing, portrait selection, naming, final acceptance, CHARS serialization,
and the status-10 supervisor handoff back to CODE.2.

For Demo v1's requested cold-start-to-character-creation slice, the remaining
unimplemented CODE.2 behavior is outside the target path: the CHARS-present
startup/import branch and the full visible behavior of startup-menu entries 0
and 1.
