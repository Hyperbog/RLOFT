# Development checkpoint 0.2.25

## Continuity base

This checkpoint continues directly from `Ravenloft_Alpha_0.2.24-dev-incremental.zip`. No visual/viewer branch or earlier source snapshot was substituted.

- 0.2.24 SHA-256: `48500637a048629629b8d75bfb43b4a0800ff5de3aed8eaaa80c223296c5c2dc`
- `Ravenloft_Decomp_Final.zip` SHA-256: `331c85756a36ba359c1f0823f612bd24e759965b361f73dbfc5910546f177729`
- original `game.gog` SHA-256: `a915b600a59b7512cc3894d154a6225a312a74526cc9c5965542107eddc37e6a`

## Mechanical-demo milestone

Verified mechanical-demo completion: **95%**.

## Newly assembly-mapped gameplay clock

- CODE.2 `0x30700` -> DS:F58C timer read
- CODE.2 `0x30708` -> IRQ0 timer pulse
- CODE.2 `0x30738` -> DS:F58C timer set
- CODE.2 `0x19C80..0x19CC2` -> gameplay-clock service
- CODE.2 `0x19CC4` -> DS:A7E4 read
- CODE.2 `0x19CCC` / `0x19CF4` -> A7E8 enable push/pop stack
- CODE.2 `0x311E0` BIOS tick read remains an explicit machine-clock boundary and is now carried by the same runtime clock object.

`OriginalGameplayClock19C80` preserves the original 54,925-us PIT/IRQ0 cadence, F58C increment, A7E4 conditional increment, fourteen-entry BYTE enable stack, and the low-byte-only restore quirk. The `0x19C80` timed-message display arm is deliberately left as a presentation boundary because it does not alter the reconstructed gameplay-clock state.

## Shipped save state recovered

The Quick Start save now exposes both original gameplay timing counters rather than fabricating them at runtime:

- DS:F58C / SAVE.DAT +0x6C = **1899**
- DS:A7E4 / saved dynamic-state clock = **1735**

The save writer evidence directly calls `0x30700` before serializing the F58C value and later serializes A7E4 after the dynamic runtime records.

## Live opening integration

`OpeningDemoSession` now owns the recovered clock state. Production opening attacks can consume its real `0x19CC4` value without test-supplied tick lambdas. One reconstructed gameplay-frame service preserves the proven original ordering:

`0x1E4A0 attack timer service -> 0x1F1C8 -> 0x1BDAC NPC scheduler`

The Win32 alpha keeps its 33-ms host/presentation timer separate. Elapsed real time is accumulated into independent 54,925-us `0x30708` pulses, and the BIOS-equivalent tick is seeded from local time-of-day instead of from the renderer cadence.

## Verification gate

This checkpoint passes:

- fresh Release configure/build;
- CTest;
- exact F58C/A7E4 Quick Start save assertions;
- `0x30708` enabled/disabled clock advancement tests;
- `0x19CCC/0x19CF4` low-byte stack quirk and fourteen-entry capacity tests;
- live session-owned attack deadline regression;
- frame-order regression proving `0x1E4A0` expires attack state before the NPC scheduler frame pass;
- full read-only original-data regression;
- direct original `game.gog` validation;
- all shipped cinematic decode checks;
- all prior combat/death/drop/pickup/mist regressions.

The source archive excludes transient build directories and original Ravenloft game data.
