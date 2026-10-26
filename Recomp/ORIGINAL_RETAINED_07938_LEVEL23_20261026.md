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
