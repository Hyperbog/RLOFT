# Ravenloft Demo Build v1

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
**Checkpoint acceptance: 80% (4/5 defined gates)**: [Windows build #602](https://github.com/Hyperbog/RLOFT/actions/runs/37716513471)
passed 101/101 CTests and published the direct-entry EXE plus one-click launcher.
A real installed-game walkaround smoke test remains outstanding. Overall demo
acceptance remains **20% (1/5 major release gates)**.


Goal: reproduce the original shipped behavior from boot through character creation while reading assets from a legally installed/owned copy of *Ravenloft: Strahd's Possession*.

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
