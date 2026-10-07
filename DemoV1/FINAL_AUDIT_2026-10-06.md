# Demo v1 final audit — boot through character creation

Date: 2026-10-06

## Scope

This audit covers the requested Demo v1 milestone:

```text
fresh launch
  -> installed-copy discovery / user-selected install
  -> CODE.2 cold-start path
  -> CINE00 / CINE12
  -> startup presentation and menu
  -> startup menu entry 2
  -> R.EXE status-10 dispatch
  -> CODE.1 opening presentation
  -> complete two-character creation workflow
  -> exact CHARS handoff serialization
  -> CODE.1 status-10 return to the CODE.2 boundary
```

The CHARS-present CODE.2 importer/game startup path and full behavior of startup-menu
entries 0 and 1 are intentionally outside this milestone.

## Audit result

No unresolved implementation discrepancy remains in the scoped path.

The cursor regression introduced at commit `50035d34` was only an initialization-order
compile error: `host.set_game_cursor(...)` preceded construction of `Host host`.
Commit `329d0f10` corrected the order without changing the reconstructed cursor identity
or compositor behavior.

The original cursor path is now:

- read from the selected installed copy, not bundled;
- resolved from RES0 using the CODE.2/CODE.1 INIT-relative cursor resource rule;
- composited into the indexed 320x400 presentation using the active Ravenloft palette;
- transparent-index aware;
- hidden during non-interactive cinematics/opening presentation;
- enabled for the startup menu and character controller;
- positioned through native-coordinate conversion rather than a Windows-system cursor.

## Rechecked implementation boundaries

- Installed-copy discovery, explicit user-selected install directory, and saved preference.
- Direct GOG `game.gog` ISO-9660 reads and loose-install reads.
- Runtime-file lookup for INIT, SOUND.CFG, CHARS, and writable CHARS output.
- CODE.2 cold-start state transitions and ESC skip behavior.
- CINE00/CINE12 ANM playback and timing path.
- COLORS palette selection/fades and RES0 startup presentation.
- Four-entry CODE.2 menu composition, hover selection, and entry-2 dispatch.
- VOC decoding/playback and RES5 XMIDI playback.
- CODE.1 SOUND.CFG relative-volume correction.
- CODE.2 -> supervisor -> CODE.1 status-10 transition.
- CODE.1 `45FC..4976` 70-frame opening presentation, palette reveal, music ramp, and SFX.
- Character controller states, hit testing, both actor slots, backtracking, and abort.
- Sex, race, class/multiclass, option, portrait, stats, final decision, and name entry.
- Duplicate-portrait restriction.
- Original stat generation/RNG/table behavior and stat editor.
- Existing-character keep/delete behavior inside the character-creation controller.
- `15B0`, `24D4`, `07E4`, controller-entry, stat-editor, final-decision, and name transitions.
- Portrait dim/hover behavior.
- CHARS exact 0x920-byte reconstruction.
- CODE.1 exit music fade and status-10 handoff back to the CODE.2 startup boundary.

## Automated verification

The repaired implementation passed both Windows and Linux CI at commit `329d0f10`.

Windows Release CTest result:

```text
1  boot_flow_tests                    PASS
2  game_source_tests                  PASS
3  presenter_tests                    PASS
4  voc_decoder_tests                  PASS
5  xmidi_tests                        PASS
6  character_flow_tests               PASS
7  character_stats_tests              PASS
8  character_transfer_tests           PASS
9  character_palette_remap_tests      PASS
10 sound_config_tests                 PASS

100% tests passed, 0 tests failed
```

The documentation-only synchronization commit `df2849f1` also passed CI.

The workflow now publishes the Release Windows executable as the
`Ravenloft-Demo-v1-Windows-x64` artifact.

## Original-media validation boundary

The repository intentionally does not contain the copyrighted Ravenloft resource binaries.
Accordingly, CI cannot perform a literal live presentation/audio smoke test using RES0,
COLORS, CINE00.ANM, CINE12.ANM, RES5/RES6, and the user's SOUND.CFG.

The runtime is designed to perform that final media-level validation against the user's
owned installed copy. Failure to locate or decode the expected original cursor/resource is
fatal rather than silently substituting a host asset.

## Milestone classification

**Implementation / reconstruction milestone: complete.**

A user-machine smoke run against an owned installation remains the final external
environment check, not an unresolved reconstruction item.
