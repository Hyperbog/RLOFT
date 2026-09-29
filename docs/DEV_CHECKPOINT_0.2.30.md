# Development checkpoint 0.2.30

## Continuity base

This checkpoint continues directly from `Ravenloft_Alpha_0.2.29-dev-incremental.zip`. No renderer/viewer rollback or alternate source snapshot was used.

- 0.2.29 SHA-256: `beda7a5b4cdf58c41f5d02b2bb2e9ea47447919acc2c823cf41358e1f782c1f5`
- `Ravenloft_Decomp_Final.zip` SHA-256: `331c85756a36ba359c1f0823f612bd24e759965b361f73dbfc5910546f177729`
- original `game.gog` SHA-256: `a915b600a59b7512cc3894d154a6225a312a74526cc9c5965542107eddc37e6a`

## Mechanical-demo milestone

Verified mechanical-demo completion: **100%** for the agreed Forgotten Realms Forest opening scope.

This closes the mechanical/behavioral demo milestone. Final original viewport/HUD composition remains a separate presentation phase and is not included in the 100% mechanical figure.

## `CODE.2 0x1E75C / 0x31F94` world-clock closure

The final forced-ready mist shortcut is removed. `OpeningDemoSession` now owns reconstructed `DS:CF4C` world-clock state and services it from the recovered gameplay clock rather than from presentation timing.

The reconstruction preserves:

- `CODE.2 0x1E75C` opening-relevant world-clock service;
- exact 1080 `DS:A7E4` gameplay-tick hour cadence;
- `DS:CF4C` hour in the high WORD while preserving the low WORD;
- opening-relevant `0x31F94` normalization, including `0xFFFF -> 23` and 23 -> 0 wrap;
- live SAVE.DAT snapshots carrying the current `CF4C` value.

## `CODE.2 0x21F70 / 0x222C0..0x222E5` mist wait closure

The opening mist transition no longer receives an unconditional ready flag.

- completing quest pickup disables gameplay time through reconstructed `0x19CCC(0)`;
- the transition starts from the shipped Quick Start world-clock hour 14;
- `0x222C0..0x222E5` advances/normalizes the real world-clock state until hour 2;
- quest objects 135, 136, and 116 are removed while 233 is retained;
- the level-23 destination state is loaded through the already reconstructed transition path;
- gameplay time is restored through reconstructed `0x19CF4` after completion.

## New regressions

- exact 1080-tick hour advancement;
- multi-hour advancement from a larger gameplay-clock delta;
- `0xFFFF -> 23` normalization and 23 -> 0 wrap;
- Quick Start hour 14 -> hour 2 mist-wait sequence;
- gameplay-clock disable/restore ordering around the transition;
- live SAVE.DAT snapshot of the mutated world-clock state;
- end-to-end opening quest pickup, mist wait, cleanup, and level transfer.

## Verification gate

This checkpoint passes:

- clean 0.2.30 configure/build;
- ordinary CTest;
- focused world-clock/mist regressions;
- full read-only original-data regression;
- direct original `game.gog` validation;
- all shipped CINE00..CINE12 decode-to-end checks;
- all prior movement, combat, death, drops, screen-coordinate pickup, inventory selection/exchange, ownership, transition, timing, and SAVE.DAT regressions.

The final explicit verification invocation used the original read-only regression view, the original `game.gog`, and the complete shipped cinematic set and exited 0 with:

`PASS: core data, Quick Start opening flow, and shipped cinematic decode validated.`
