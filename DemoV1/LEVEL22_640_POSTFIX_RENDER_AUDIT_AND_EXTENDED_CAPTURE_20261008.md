# User native capture #640 — first successful corrected ray-index visuals

**Date:** 2026-10-08 EDT / workflow run 2026-10-09 UTC  
**Original file:** `1791512367563.zip` (user-uploaded, not checked into Git)  
**Archive SHA-256:** `7ecd434e232e23e431f444fa825354a14d113c98504c3c5b3279673278c2091e`  
**Branch:** `demo-v1` · **Formal acceptance:** 1/5 gates (20%).

## Provenance verification

The archive contains **16** complete `rloft-native-indexed-capture-v2`
triplets (`.idx`, `.pal`, `.tsv`) with exactly 128,000
indexed pixels and 768 6-bit VGA DAC bytes per frame. Every TSV
declares the same checked-out source revision:

`47d5194a0c87109ff9c26df7794c960d492ccb99`.

GitHub Actions [manual Windows run #640](
https://github.com/Hyperbog/RLOFT/actions/runs/37872583683)
used **that exact source SHA**, compiled successfully, passed
**160/160 CTests** and successfully packaged the Windows artifact.
Linux tests were skipped by the manual workflow default.
Unlike prior #639 archives, this is definitively a post-sweep-cell-index
fix binary. The ZIP has **no `failure.tsv`**.

The normal 16-frame snapshot cap was reached, so lack of
`failure.tsv` proves no recorded frame-building exception in
this capture, **not** an exhaustive no-crash result for all future
poses, angles, or time after the final image.

## Camera/scene diagnostics

Sequence: start `(4,4)` at heading `270`; turn through
`271,275,281,288,298,308`, walk to `(5,5)`,
`(6,5)`, `(6,6)`, rotate to `307,303,297,290`,
then travel to `(7,6)` and `(8,6)` at heading
`290`. The first and last 16.16 camera origins are
`(294912,294912)` and `(531564,445210)`.

Across all 16 archived frames:

| Recorded original-source metric | Sum |
|---|---:|
| Successfully executed native RES3 draw calls | 2,127 |
| Fully occluded scene calls | 127 |
| RES3 planning / trace / raster failures | 0 |
| RES3 rasterized columns | 25,282 |
| RES3 indexed VGA writes | 521,921 |
| Individual-mode width-one source runs | 25,098 |
| Individual-mode width-two source runs | 92 |
| Source runs width three or greater | 0 |
| Palette variants | 1 |
| Distinct raw indexed images | 16 |

All recorded source runs are in original `C029=1`
individual-column mode and explicitly supported. No actual
scene run of width 8–255 occurred: this archive **does not**
certify the wider individual RES3 pathway against live assets.

The projected records contain exactly the expected original
catalog source classes: one type-9 NPC record per frame,
plus placed-object types 62/63/64. This confirms source
reconstruction/projection and native draw execution, **not**
pixel-for-pixel DOS match or original NPC behavioral timing.
All ordinary wall resources in the open Level 22 interior
remain the expected non-drawable `0x93`; this is not a
wall-bearing-level acceptance sample.

## Direct before/after indexed pixel comparison

Original pre-fix archive `1791508927564.zip`, SHA-256
`4cae065ba0c045664a446f0f46fb0671376ef90577d438c27c6b823b634656ba`,
contains **six exact matching** source camera A/B + heading
poses also present in the new post-fix archive (all at
`A=B=294912` / cell `(4,4)`).
Raw palette bytes are equal in all six paired frames.

| Facing | Exact indexed pixel mismatches / 128,000 | Native raster calls before → after |
|---:|---:|---:|
| 270 | 462 | 105 → 167 |
| 271 | 460 | 107 → 165 |
| 275 | 399 | 117 → 169 |
| 281 | 411 | 123 → 169 |
| 288 | 332 | 135 → 179 |
| 298 | 367 | 143 → 171 |

For facing 270, projected `scene_records` change from 62
to 88 and `scene_draw_calls` from 123 to 175, consistent
with the prior native traversal's wrong half-index
affecting visibility and selection. Exact camera pose and
palette hold fixed, but an independent DOS reference, game
clock/tick and NPC state were **not** captured: these are
observed native build deltas, not DOS equivalence.

This makes the cell-index fix **visible in raw pixels**
and increases native draw coverage while previously
unhandled ray guards are absent in these test views.
The official gate stays unpassed pending original DOS parity.

## Follow-on: capture beyond the early 16-frame ceiling

The same user archive reaches its original 16-frame cap at
`(8,6)`, before entering the source NPC's neighborhood at
`(13,7)`. The native capture was deliberately budgeted
to prevent unbounded disk writes; a default 16-frame limit
is still retained for manual command-line capture.

A follow-up **source-only** change adds
`--capture-frames=N` with strictly validated integer
`1..128`, valid only alongside `--capture-render`
and `--explore-level22`; the staged
`Capture-Level22.cmd` launcher opts into **64** frames.
The preview title shows actual `captures/current-limit`.
The already-existing `failure.tsv` exception sidecar
is unaffected by the image budget and still works past
the final image. The parser is a portable pure helper,
tested in `exploration_frame_capture_tests` for bounds,
invalid characters, overflow and normal values.
No original game assets, framebuffer rasterizer,
collision, script events or DOS gameplay were altered.

**IMPORTANT:** Windows run #640 verified the original
cell-index fix and existing capture mode. It **does
not verify the new configurable 64-frame capture source**,
which was committed only after #640 and needs another
manually initiated Windows build to become available.

## Next evidence-driven work

1. Manually build current `demo-v1` with Linux unchecked.
2. Use the new `Capture-Level22.cmd` and continue
   beyond (8,6) toward the NPC at (13,7) without
   exhausting the capture count. Archive at most 64
   successful changed poses plus an independent fatal
   sidecar when applicable.
3. Inspect source-indexed NPC/placed-object visibility,
   type-7 per-column drawing, and unresolved runs,
   prioritizing authentic original DOS indexed-frame
   references over screenshot-only judgments.
4. Exercise near-cardinal turns farther inside the map;
   any repeated frame-building failure now reports
   actual ray/pose/compiled revision in `failure.tsv`.

This is the first strongly documented post-fix native
sweep screenshot dataset. It does **not** mark a
formal demo acceptance gate complete.
