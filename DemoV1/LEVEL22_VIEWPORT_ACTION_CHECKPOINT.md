# Level 22 viewport-action integration checkpoint

Status: input-facing action methods committed to `demo-v1`; not yet a complete playable Assassin combat/relic transition.

## Controlling original behavior

- CODE.2 `223F4`: world-scene hit selection proceeds in descending render-rank order.
- CODE.2 `224EF..224F4`: source-class-2 loose items are eligible only when action-flags bit zero is set.
- Original source-class zero selects live family-A NPC records, not scene-record indices.
- Level 22 Assassin damage, four original death drops, ordinary loose-item pickup, and relic-set completion are handled by the existing `Level22EncounterModel`.

## New integration

`Level22ViewportTargetModel` rejects nonmatching classes, wrong NPC type IDs, and invalidated native array indices.

`ExplorationRuntime::damage_level22_assassin_at(frame,row,column,damage,reaction_mode,probe)` passes flags zero into the already faithful RES3 hit selection. This lets an NPC remain attackable through a visually higher-ranked pickup item without turning the item into a combat target. Only a live Level 22 Assassin is dispatched to `damage_level22_assassin`.

`ExplorationRuntime::pickup_level22_loose_item_at(frame,row,column)` passes flags one into the hit selection and dispatches source class two to `pickup_level22_loose_item`, preserving the original highest-rank selection order.

Both functions accept a **specific prior `ExplorationRuntimeFrame`** rather than a free-floating vector of scene records. The frame captures its origin world revision, level ID, exact 16.16 camera coordinates, and heading. Action methods reject frames with mismatched state before accessing a live family record. World revision is invalidated before running NPC damage or pickup mutations, so even a failure after partial mutation cannot silently reuse an old frame.

These methods preserve original game mechanics; they do not synthesize attack rolls, hit probability, damage values, animation timing, or automatic item transfers. Damage and reaction parameters are supplied by the higher-level combat system.

## Test coverage

- `level22_viewport_target_tests` asserts source class routing, non-Assassin exclusion, correct native indices, and rejection of invalidated NPC/item references.
- The same test runs the original rectangle hit selector against overlapping Assassin and loose-item scene records, with both action-flags values.
- `exploration_runtime_tests` verifies the source pose and revision stored on newly built exploration frames, including the pose after motion.
- `type7_scene_pixel_executor_tests` now verifies that an invalid **second** descriptor cannot alter any caller VGA plane values written by an earlier valid descriptor.

## Still required before declaring complete

1. Wire the host's actual mouse/keyboard action processing to these typed methods using the original input-state rules.
2. Supply combat hit/damage/reaction values from the original attack controller, rather than inventing damage when an Assassin rectangle is clicked.
3. Rebuild the scene frame after damage or item pickup; do not reuse snapshots after family-C array compaction.
4. End-to-end test Assassin death, original drop positions and objects, pickup eligibility, and the mist/Level 23 transition.
5. Complete C++ compilation and in-game visual verification, independent of the unavailable GitHub Actions jobs.

No original game assets are committed to the repository.


## Native host pickup binding (October 8, 2026)

The Windows exploration host now retains the exact last presented `ExplorationRuntimeFrame`, and uses an otherwise unclaimed **primary** viewport click to call `pickup_level22_loose_item_at(frame,row,column)`. This invokes the previously reconstructed original CODE.2 `223F4` selection rectangles, render-rank precedence and class-2 action-flag gating. Clicks on the top menu or active actor panel are handled by their existing modal paths first, and are never interpreted as world pickups.

The host refuses item pickup while the original cursor is already holding an item. This **temporary loss-prevention guard** is not a reconstructed replacement for CODE.2's still-unimplemented held-item swap/inventory modal. A successful pickup clears the cached frame immediately because family-C record indices compact on removal. Stale snapshots are discarded rather than applying the click to a different item; the next frame is built normally. No arbitrary mouse click is converted into an Assassin damage roll.

The original completion predicate is preserved; a pickup that reports `level22_mist_sequence=true` is surfaced as *transition pending* in the host title. **The 22-frame mist cinematic, Level 23 load, relic removal, and held-item inventory transfer UI are not yet wired**, so this is an interaction checkpoint, not the complete story sequence.

CI verified the previous unmodified source baseline in GitHub Actions run [#572](https://github.com/Hyperbog/RLOFT/actions/runs/37710634731): Linux 99/99 tests, Windows full Build/Test and artifact publication all passed. Linux automatic builds were then deliberately paused at the user's request to conserve Actions minutes; Windows remains enabled.
