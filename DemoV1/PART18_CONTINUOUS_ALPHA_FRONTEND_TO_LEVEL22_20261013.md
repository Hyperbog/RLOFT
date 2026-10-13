# Part 18 — one continuous original-front-end → first Level22 Alpha

**Branch:** `demo-v1`. **Status:** Pre-Alpha, owner Windows visual/audio acceptance pending.

## Required player-facing order

```text
Original CINE00 → original CINE12 with encoded audio/frame waits
 → DreamForge / SSI / TSR splash presentation
 → animated title menu
 → CODE.1 two-person character creation
 → original CHARS on writable disk, exact 0x920 bytes
 → R.EXE status 10 semantics / CODE.2 reads CHARS back
 → original authored Level 22 world + native exploration presenter
```

This is already the normal **CHARS-absent** native source path in
`DemoV1/src/win32_main.cpp`; it does not require recreating the
cinematics, music, sound effects, lady/cat animation or card flipping.
The original DOS binary is *not* executed.

### Why earlier test packages felt like regressions

- `Play-Original-Intro.cmd` explicitly passes `--front-end-only`.
  Its CHARS transfer is a **read-only ending**, not normal exploration.
- A writable installation with a preexisting `CHARS` enters CODE.2's
  normal existing-party shortcut and bypasses all intro/title steps.
- `Test-Alpha-Continuous-Game.cmd` invoked a folder browser and
  separate cold-vs-existing-party selection solely for diagnostics.
- The `--explore-level22` developer shortcut has a synthetic party
  and must never stand in for actual character creation.

None of those are suitable for this **single-route player Alpha**.

## New single launcher

`DemoV1/Windows/Play-Ravenloft-Alpha.cmd` invokes its paired
`.ps1`. The Windows build uploads a dedicated
`Ravenloft-Continuous-Alpha-Windows-x64` artifact.

1. Once, put **your owned original DOS installation files** inside
   the release's `Game/` directory, optionally preserving a nested
   `Game/RLOFT/` directory. Loose originals (`RES0`, `INIT`, `T`)
   work in either root; the launcher passes the correct media directory
   to `GameSource::open`. A `game.gog` image in either directory is
   also accepted through the existing ISO9660 reader. No original
   proprietary assets are shipped.
2. Double-click **Play-Ravenloft-Alpha.cmd**. No directory picker, yes/no
   startup selection, source variant, splash selector or developer menu.
3. The launcher verifies the full executable SHA-256 and exact compiled
   source SHA, rejects filesystem links, clones `Game/` to a fresh
   `%TEMP%/RLOFT-Continuous-Alpha/<unique>` directory, and removes
   preexisting `CHARS` **only inside that disposable copy**.
4. It then invokes the **unmodified default native game** with exactly
   one argument: the copied installation path. It does not pass
   `--front-end-only`, `--explore-level22`, `--dev-trace`, or a
   special gameplay flag. CODE.1 writes CHARS in the disposable copy
   and the original-source-mapped CODE.2 handoff reopens those bytes.
5. The game proceeds directly to the partial native Level22 runtime.
   The launcher neither writes nor deletes the owner’s original
   `Game/CHARS`, `SAVE.DAT` or `SAVE##/LEVnnn.DAT`.

The optional diagnostic launcher from Part 17 is **not** the new
player-facing route.

## Frontend preservation fence

`DemoV1/tools/verify_alpha_frontend_preservation.py` pins six C++
Git blobs against the established `ff263c3` front-end snapshot:

- `anm_cinematic.cpp` — ANM decoder/audio command timing
- `win32_audio.cpp` — sampled audio/voice output
- `win32_midi.cpp` — sequence and MIDI output
- `startup_presenter.cpp` — splash and title rendering
- `character_creation_presenter.cpp` — lady/cat/cards animation
- `character_creation_flow.cpp` — original character workflow

The same file also checks the unbroken source order and handoff in
`win32_main.cpp`; this host controller differs from that snapshot
only by opt-in first-frame diagnostic markers and is not pinned as a
whole. Rework of these frontend files is deliberately forbidden without
new source/behavioral evidence and a reviewed regression update.

`DemoV1/tools/verify_continuous_alpha_launch.py` checks the
launcher/media guards, exact default argument, CMake and workflow
staging and the original CODE.1-to-CODE.2 handoff with 11
source-mutation rejection cases. Its full Windows test result is
separate from this source-only repository checkpoint.

## What is NOT accepted

The first-viewport `exploration_presenter.cpp` currently includes
original RES8 sky/environment commands, original-ground eligible-path
checks, SINTAB surface rasterization and original RES3 type-7 world
scene plans. The party/assassin placement is in the authored Level22
runtime, but **the owner previously observed an empty/flashing level**.
Those original rendering and timing gaps are real P0 issues; mere
availability of drawing code does **not** prove that original grass,
trees/backgrounds and assassin are visibly correct on Windows.

We have **not** independently executed the resulting exact GitHub
Windows artifact in this environment. Release remains Pre-Alpha until
the next owner normal start-to-first-Level22 Windows capture verifies:
CINE/voice/music, splash order, CODE.1 lady/cat and card flip parity,
disk handoff, first terrain+sky+trees+assassin pixels, smooth cadence,
and usable movement. Any first-frame fallback, blank scene, bad palette
or missing audio must be fixed before calling this Alpha accepted.

The Level22 diagnostic's source stage markers differentiate
`exploration.load`, `world`, `presenters`, `frame.build`,
`frame.raster`, and `frame.present.submitted`. Host submission
does not prove original pixels. Preserve original CODE.2 scene
semantics; do not insert fabricated scenery to hide unfinished
rasterization.
