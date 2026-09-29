# Development checkpoint 0.2.18

## Continuity base

This checkpoint continues directly from the last persistent Pt. 5 green archive, `Ravenloft_Alpha_0.2.17-dev-incremental.zip`. The uncheckpointed end-of-Pt. 5 `0x1C328` / `0xD550` delta was reconstructed from the final decomp evidence rather than guessed or replaced with demo-only logic.

- 0.2.17 SHA-256: `329f558a39588c17e23250c700acb8682d5cecfa6e0e87f1662dd0e1b385b81c`
- `Ravenloft_Decomp_Final.zip` SHA-256: `331c85756a36ba359c1f0823f612bd24e759965b361f73dbfc5910546f177729`
- original `game.gog` SHA-256: `a915b600a59b7512cc3894d154a6225a312a74526cc9c5965542107eddc37e6a`

## Restored assembly-mapped components

- `CODE.2 0x1C328..0x1C365` -> `OriginalDice1C328::roll`
- `CODE.2 0xD550..0xDACB` -> `NpcAttackDispatcherD550::execute`

The dispatcher preserves the recovered three-argument original call signature, type-15 and type-8 special branches, DS:687E target ordering, D540 target-resolution jump table, timer reseed, target exclusion/cycling, `0xC9A0` hit test, `0x1C328` damage, NPC trait/status branches, `0xE284` protection effect, `0xCD44` character damage, audio/feedback boundaries, and the original redundant party-defeat check.

The `+0x1EE` timed Strength-drain expiry uses the assembly-confirmed `2d4 * 10,800` tick arithmetic. The `+0x3A & 0x20` protection flag is intentionally evaluated before the query-`0x584` threshold branch, matching instruction order.

## Verification gate

Focused regressions cover:

- exact `0x1C328` per-die/final-modifier and RNG behavior;
- ordinary hit and miss;
- fixed-point reach gate;
- randomized target-table resolution;
- four-target exclusion cycling and no-target fallback;
- type-8 special attack;
- type-15 attack;
- level drain;
- Strength drain and 10,800-tick multiplier;
- query-`0x584` 10-HP threshold;
- negative-plane protection consumption and `0xE284`;
- status-trait and paired-dirty-state mutations.

The full project regression is also run through a read-only filesystem view of the files extracted from the original `game.gog`; the original extracted bytes are not modified.
