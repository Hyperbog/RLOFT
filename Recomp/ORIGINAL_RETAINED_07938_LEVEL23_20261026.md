# Part 38 — retained original 07938 saved Level 23 integration

## Source / original-media provenance

- Branch: `Recomp`; owner original media remain read-only and never enter packages.
- Reconstructed original CODE.2 function `07938` is invoked on the SAME retained
  source worker, DGROUP and native Watcom heap as `353A0 → 20608 → 20440 → 1FC14`.
- Original `DnD-Classics/MASTER/Ravenloft - Strahd's Possession/cloud_saves/RLOFT/SAVE01`
  contains a real `SAVE.DAT` (9,288 bytes, level 23 at offset 0x52) and
  `LEV023.DAT`. The authentic source save position is x=738207, y=2926568
  (16.16 fixed point), heading=50. It is not a Level 22 portal or fake camera.
- Original archived Level 23 type-8 portal tiles to level 26 are (9,1) and
  (37,1); the archived Level 26 reciprocal portal records are (68,30) and
  (95,30). The saved player starts on tile (11,44): private diagnostic
  portal-record invocation does **not** prove physical walking into a portal.

## What the retained-host bridge does

- `original_code2_live_07938_restore.inc` opt-in code accepts a specified
  original slot with both `SAVE.DAT` and `LEV023.DAT`.
- It checks the source save header, decodes the preserved LEV record, then
  seeds the existing session-private `WORK\\LEV023.DAT` overlay.
- The original reconstructed `07938` controls party state, camera, inventory,
  clock, lighting, and the call into `181B4`; file callbacks present the
  owner's unchanged SAVE.DAT bytes as read-only guest data.
- The same session retains the source `1FC14` loop and indexed VGA.
  A private `FFFA` source-test command selects the restore, never a retail key.
- `--load-save23 DIR` enters the interactive Windows window after restoration;
  `--smoke-save23 DIR` additionally requires the source authenticated
  Level 23 → 26 → 23 archive portal pair and all three completed `1FC14`
  native RGB6/indexed frame audits. Nothing is written into the retail WORK dir.

## Validation boundaries

The isolated original `07938 → 181B4` preserved media harness passed on the
unaltered Level 23 save in the Part 37 Windows workflow. The prior
`--smoke-portal` diagnostic timed out while attempting the first native Level
23 scene; its nonblocking CI step can be red even when the overall engineering
milestone is green. A native destination `1FC14` scene and post-frame audit
are **not accepted** without explicit `07938_RETAINED_LEVEL23_RESTORE_PASS`,
`171FC_181B4_LIVE_SOURCE_RETURN` and
`1FC14_AFTER_181B4_FULL_CYCLE_SCANOUT_PASS level=26` witnesses in the same
run.

The save-backed probe uses a separate explicitly nonblocking Windows CI step.
Read its individual step logs, not the aggregate job's success icon, before
claiming native gameplay/frame certification. Even a successful native indexed
frame is not retail DOS synchronized frame parity or proof of user-controlled
movement onto the portal tile.


## Part 38 follow-up: verified original 07938 sentinel (strict Windows failure)

Commit `c81521206fbaf42d36a0f7aea68cb01bd6d0aec6` introduced a
**strict**, standalone, bounded Windows CI job that correctly fails when
native preserved-source restoration or its portal frame is missing.

- Strict workflow: https://github.com/Hyperbog/RLOFT/actions/runs/40265009326
- Result: **failure**, native acceptance OPEN; window MSVC compilation PASS.
- At the exact **compiled original** `07938 → 181B4(23)` callback:
  `requested=23`, `guest_prior=4294967295 (0xFFFFFFFF)`,
  `saved_world_overlay=1`, `save_reads=156`,
  `restore_active=1`, `save_level_callbacks=1`.
- The owner prior-level value is an original **no-departing-level sentinel**,
  not the Level 22/23 same-level case assumed in the previous guard.
- First unsupported callback:
  `LIVE_CODE2_SOURCE_STOP=ORIGINAL_181B4_LIVE_UNVERIFIED_LEVEL_PAIR`.
- No `07938_RETAINED_LEVEL23_RESTORE_PASS`, no
  `1FC14_AFTER_181B4_FULL_CYCLE_SCANOUT_PASS`, and no Level 26 frame.
- The isolated untouched Level 23 `07938 → 181B4` saved-game harness still
  passes. The ordinary Linux, source-link and Windows CTest regressions pass.

### Bounded next fix (not implemented in this report)

In `Recomp/src/original_code2_live_level_transition.inc`, support
`prior==0xFFFFFFFF` **only** when all of the original saved-game inputs
and existing retained-owner prerequisites are satisfied: active `07938`
restore, `requested==23`, one source level callback, authenticated
private `LEV023` overlay, and original save reads observed.

The original `181B4` should then own whether the no-departing-level path
skips `171FC`. Audit the original postload branches and make the
`level_overlay.has(prior)` postcondition conditional on an actual
departing level; do not synthesize, overwrite or remove real saved-world
data, and do not broaden general level transition permissions.

Rerun the dedicated strict workflow and require the actual
`07938_RETAINED_LEVEL23_RESTORE_PASS` marker **before** interpreting
the native `23 → 26` framebuffer test. A green aggregate milestone with
a nonblocking timed-out portal test is not certification. Native
movement-triggered portal entry and independent retail DOS parity remain
separate acceptance gates.
