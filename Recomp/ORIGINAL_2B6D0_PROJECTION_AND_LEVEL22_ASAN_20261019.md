# Part 26 — original Level 23 2B6D0 rejection and instrumented Level 22 crash

Branch: `Recomp`. Source authority: retail CODE.2 SHA-256
`853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.

## Genuine Level 23 source projection

From preserved owner cloud `SAVE01` on active Level 23:
- Original `07938 -> 181B4 -> 334C4 -> 32954`: **34** primary records,
  **3** actor cells marked ray-visible by source `0x0040`.
- Original `2B6D0` flag-`0x02` exclusion: all three pass.
- Instrumented original `35798` callback: **9** calls across actor/transient
  source passes, with **3** calls matching the three eligible primary actors.
- All nine are rejected by `35798`'s exact signed depth test:
  `depth < 0x5800`. Observed total depth range:
  **-2,815,566 .. -246,671**; the three matched primary actor depths also
  occupy **-2,815,566 .. -246,671**.
- `2C294` consequently creates **0** original sprite draw requests:
  no `21BF0` sprite-extent calls, no `36CEC` calls, no original-scene
  `369D8` indexed pixel writes.

The original projection computes
`ax=x-[255EC]`, `bx=y-[255F4]`,
`depth=35680(ax,[255F8])-35680(bx,[255FC])-0x2000`.
It returns zero output scale for negative/near-plane depth. The diagnostic
records the pre-return depth and emitted guest output and does not change
actor coordinates, camera, ray flags, resource data or projection parameters.

**Acceptance boundary:** this is a confirmed 35798 near-plane rejection,
not proof that the standalone recomp's camera-transform state is identical
to a running DOS game. Avoid forcing objects into view. Verify transform
initialization and original `32954` visibility semantics next.

## Level 22 instrumented crash and bounded protection

Authentic preserved `RLOFT/SAVE01` / `LEV022.DAT` source probe previously
exited 139. Two ASan runs with the original read-only media locate:

```text
code2::store_le16 (Code2/Code2.cpp:43)
code2::function_31f94 (Code2/Code2.cpp:24622)
SavedGame::update_daypart (Recomp/src/code2_original_save_game.cpp)
code2::function_181b4 (Code2/Code2.cpp:11552)
SavedGame::load_saved_level
app10_load_tail_06cad
code2::function_07938
```

`31F94` uses `view=[258A8]` and reads a descriptor through
`[ [2B764] + 4*view ]` when phase `(high16([CF4C]) % 24)`
differs from `[F9CC]` and the view is not `0xFF`.
The original game requires initialized scene-view descriptors on this path;
our isolated source harness has not bootstrapped that bank.

The safety guard now exactly preserves the original phase-early-return
branch and checks only guest-memory address bounds for its table and record.
It does not manufacture heap descriptors or modify the saved view index.

Final ASan run: **no segfault**; exits status 1 with
`CODE2_31F94_ORIGINAL_VIEW_DESCRIPTOR_PREREQUISITE view=1
scene_heap_initialized=NO source_level_load_rejected=YES`.
Level 23's original source census and Level 5 scene probe still pass.
The genuine cross-level census deliberately remains **red** because
Level 22's full source prerequisites are not yet satisfied.

## Verification and continuity

- [Genuine Level 23 source projection](https://github.com/Hyperbog/RLOFT/actions/runs/39194320760)
- [Final Level 23 recheck](https://github.com/Hyperbog/RLOFT/actions/runs/39194768067)
- [Original Level 22 ASan caller stack](https://github.com/Hyperbog/RLOFT/actions/runs/39194264546)
- [Level 22 guarded ASan run](https://github.com/Hyperbog/RLOFT/actions/runs/39194768008)
- [Cross-level census: 23 and 21 succeed; 22 explicitly rejected](https://github.com/Hyperbog/RLOFT/actions/runs/39194768035)
- [Linux C++ regressions](https://github.com/Hyperbog/RLOFT/actions/runs/39194768119)
- [Windows regression run](https://github.com/Hyperbog/RLOFT/actions/runs/39194865650)

**Next step:** initialize `2B764` through the original INIT/scene-definition
startup in Level 22 rather than bypass `31F94`. In parallel, compare the
Level 23 original camera ray/projection basis from `334C4` against the
full game boot so genuine screen-visible actors can be accepted.
Independent DOS VGA planes and RGB6 palette are still **not captured**.
