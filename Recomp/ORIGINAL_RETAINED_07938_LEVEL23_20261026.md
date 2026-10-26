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


## 2026-10-26 restoration and renderer status — current acceptance

The earlier sentinel error is fixed in [`138a2cf2`](https://github.com/Hyperbog/RLOFT/commit/138a2cf22bf333ba61e918668c8e6e01a765e6f2). In strict [run 40271379104](https://github.com/Hyperbog/RLOFT/actions/runs/40271379104), original `07938` performed 156 source save reads, original `181B4(23)` performed 219 source level reads, and **zero** writes of a departing world occurred. The same retained CODE.2 guest emitted `07938_RETAINED_LEVEL23_RESTORE_PASS`. This is a saved-world restore, not a full frame return.

The [compiled original `2D5D0` integration](https://github.com/Hyperbog/RLOFT/commit/febdb63c4fc2a511250c1536a169503e6fd0012b) allowed the first Level23 `1BDAC` actor pass to return: three collision checks returned 1, 0, 1 without synthetic collision values.

Strict [run 40273739122](https://github.com/Hyperbog/RLOFT/actions/runs/40273739122) reached the authentic `2C294` scene-record clear, multiple `35798` projections, and `21BF0`, but did not return from original `335DC` wall rendering. Source material and flush tracing in [run 40273999130](https://github.com/Hyperbog/RLOFT/actions/runs/40273999130) showed authentic RES4 material loads and completed original `3263C` VGA flushes through cumulative count 1928 within a 90-second test; `335DC` itself did not return before timeout. A cumulative flush count does not establish an infinite loop. Original `335DC` has two source-authored scan passes over 256 records; see [source-only run 40274120863](https://github.com/Hyperbog/RLOFT/actions/runs/40274120863).

An exploratory Release-mode strict probe [run 40274297356](https://github.com/Hyperbog/RLOFT/actions/runs/40274297356) exited with Windows code `-1073740791` (`0xC0000409`), unlike Debug mode's bounded timeout. This is an **unresolved release-specific fast-fail**, not evidence that original render or DOS parity completed; the native acceptance workflow was returned to **Debug** until the cause is diagnosed.

**Unaccepted:** first Level23 original `1FC14` full page/HUD/audio/indexed-RGB6 scanout after restoration; Level23→26 genuine type-8 portal framebuffer; later Level26→23 return; physical original DOS behavior parity. No invented level, camera, frame, or collision state is used to claim success.

**Next bounded investigation:** instrument original `335DC` starting and ending ray-record indices and per-pass flush deltas (distinguish elapsed time from progress); isolate whether the reconstructed `3263C` bridge or source resource management dominates. Preserve original CODE.2 call order and the strict timeout while diagnosing the Release fast-fail separately.


## 2026-10-26 Part 32 continuation — completed Level 23 scene; nested Level 26 preflight

**The previous Level 23 ray-stall statement above is historical, not the present frontier.**
At [strict run #40279091361](https://github.com/Hyperbog/RLOFT/actions/runs/40279091361)
on `037209cd`, the same retained original guest completed both source
`335DC` scan passes (`ray_count=256`, cumulative `3263C` flushes
`2291`), original `2C294` scene/sprite returns, source page handoff,
post-scene HUD/audio/random queries, and re-entered source `30B40`.
**No Level 26 native frame or retail DOS-equivalent VGA frame was accepted.**

The subsequent first Level 23→26 transition encountered a host-only
`171FC` preflight wrongly requiring each of the three original 22-byte
world-record families to have at most 150 entries. Original `LEVELS.DAT`
contains Level 23 Family A/C/B counts `34/28/213`, and Level 26 counts
`47/11/183`. The original Family B placed-item array has room for 600
22-byte records between `0x22238` and `0x255D8`. [Commit
`3d57d759`](https://github.com/Hyperbog/RLOFT/commit/3d57d759d75943d23af751deb55d7832c9389574)
checks the three original families separately; [strict run
#40306828750](https://github.com/Hyperbog/RLOFT/actions/runs/40306828750)
advanced beyond that guard.

The next `1BDAC` host acceptance guard rejected **two** completed
original polygon-scan returns because it expected exactly one. The
nonmutating diagnostic at [`10464c0a`](https://github.com/Hyperbog/RLOFT/commit/10464c0a8bbc27de8587518271eae6d48ce97848)
and [strict run #40307645820](https://github.com/Hyperbog/RLOFT/actions/runs/40307645820)
recorded genuine source Level 26, mode 4, active transition, polygon
`scan_returns=2`, `quads=6`, `scanlines=162`, `cells=1320`,
`projections=55`. [Commit `6adf7e11`](https://github.com/Hyperbog/RLOFT/commit/6adf7e11635cabce1d108a86d80bffb36b04c5b6)
now accepts at least one actual prior `34DE8` scan without inserting
polygon callbacks or altering guest state.

**Current strict frontier:** [run #40308055341](https://github.com/Hyperbog/RLOFT/actions/runs/40308055341)
again completed the genuine restored Level 23 `1FC14` scene,
entered original destination-resource loading, passed the `1BDAC`
polygon preflight and stopped at
`LIVE_CODE2_SOURCE_STOP=ORIGINAL_2C294_ACTOR_PREDECESSOR_MISSING`.
The `2C294` host guard currently expects exactly one cumulative actor
update return, despite the nested `181B4 → 1F8FC` rendering progression.
The following `Concurrent host access to a running original guest`
message remains a secondary supervisor symptom until independently
isolated. Strict result: **failed**; `restored=True`,
`portal_full_cycle=False`, no genuine Level 26→23 return accepted.

**Next source-safe step:** instrument the `2C294` actor-return count,
the source guest level and subframe/transition boundaries. Then
replace only demonstrably stale per-frame host assertions with
monotonic or per-invocation source witnesses; do not zero live counters,
synthesize actor updates, omit original callbacks, or accept a portal
without the strict native full-cycle marker. This is a diagnostic
and acceptance-boundary issue, **not** evidence of original DOS parity.
