# Part 35 — Owner-live CODE.2 171FC/181B4 and 1FC14 continuation frontier

**Branch:** `Recomp`. All original game media remain in the owner's private installation.
**Status:** Source callback graph integrated behind an explicit experimental gate. A human-played Level 22 → 23 → 22 transition and a post-transition retail-equivalent framebuffer are **not accepted**.

## Changes in the retained CODE.2 Session

- A private `rloft::live_level_files::Overlay` stores original `WORK\\LEVnnn.DAT` snapshots with bounded, source-style DOS handles, seeks, read/write counts, and strict 16-byte guest pathname validation. **No file is written into the original game directory.**
- Original `181B4` opens the session's saved level when present, otherwise falls back to the owner's read-only `LEVELS.DAT`. On a level change the compiled original `171FC` serializes the departing live world. The original source heap, map pointer, moving actors, event records, scripts and owner guest are retained.
- Original `171FC` now invokes compiled `01558` to regenerate its source exploration map before serialization; its `1E8C8` and `1E930` map-record callbacks are connected. Regression fixtures check the original 01558 player/automap encoding.
- Original `181B4` runs the already-connected compiled `17A68` actor post-load callbacks, state-stack `19CCC/19CF4`, terminal-party check `0CCF4`, original `31EDC/31F38/31F94` view-phase helpers, and the unchanged-mode branch of compiled `1F8E0`.
- `28DCC`, `28EF0`, and `28E58` execute through the compiled source when the authentic AIL device handle is disabled (`EE78==0`). This preserves source music-bank-change semantics without claiming audio output; active original devices remain guarded.
- The original `2D3D8` movement callback can enter a guarded live `181B4` replacement. The retained compiled source call stack then returns toward `1FC14`; a separate scene-handoff witness validates the *next* original frame and reports its level. Merely finishing a world load is not counted as resumed gameplay.

## Explicit entry and fail-closed conditions

The live cross-level path requires `RLOFT_RECOMP_EXPERIMENTAL_LIVE_LEVEL_TRANSITIONS=1`. The caller must be executing an actual Level 22↔23 portal on the retained source guest. Before mutating the departing level, the bridge checks original world geometry, record and script pointers, bounded saved-level data, unmapped original WORK saves, ownership, original event-list termination, balanced state stack and source file handles.

**Graphics and dynamic actors are the primary remaining acceptance blockers.** The destination must use the *already-active graphics mode 3*, otherwise original `1F8E0 → 1F8FC` full mode-redraw callbacks are required and execution stops. Where original `20850` could reposition a moving NPC, its unresolved collision/relocation graph must also stop before deserialization. The active AIL driver path is unconnected.

The authenticated source `LEVELS.DAT` identifies Level 22 as 30×15 with one header moving record, and Level 23 as 64×64 with 34. These figures make NPC/collision auditing important; they are **not** proof that actual portal traversal succeeds. Source render/actor completion and retail DOS indexed-frame parity remain independent gates.

## Verification and acceptance rules

The Windows milestone and canonical Linux/source-link CTest suites compile these callbacks. The Windows owner-media gate already independently exercised the original `181B4 → 171FC → 181B4` Level 22→23→22 **memory-only** save/restore round-trip. New regressions cover the private filename/file interface, malformed DOS paths, compiled original `01558` map regeneration, and inactive-device original 28DCC/28EF0/28E58 music path.

**No new playable milestone, full cross-level acceptance, DOS frame-parity acceptance, or authentic audible AIL acceptance is claimed** until the real native window demonstrates a portal load, source 17A68 state, correct original RES display-bank changes if required, and a subsequent validated 1FC14 frame on the owner media.

## Next source dependencies

1. Complete original `20850` relocation/collision callbacks (including `2D5D0` / `207E8` / `0E0F8`) for moving actor worlds without changing their rules.
2. Connect and validate full original `1F8FC` scene-mode transitions for destination graphics and palette, including CODE.2-backed RES changes and viewport copies.
3. Exercise the original portal event from a real saved/owner guest in the Windows source host; verify the post-transition `1FC14` frame witness and direct saved-level return.
4. Preserve entirely separate retail DOS indexed-frame and audio acceptance gates.
