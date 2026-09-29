# Development checkpoint 0.2.19

## Continuity base

This checkpoint continues directly from `Ravenloft_Alpha_0.2.18-dev-incremental.zip`; no earlier branch was substituted. The 0.2.18 archive remains the preserved green base for the fully verified `0x1C328` / `0xD550` conversion.

- 0.2.18 SHA-256: `decc1236fe298c543be37ca2ae48150b8ee1047ad4a7362cf6b220c0e1093033`
- `Ravenloft_Decomp_Final.zip` SHA-256: `331c85756a36ba359c1f0823f612bd24e759965b361f73dbfc5910546f177729`
- original `game.gog` SHA-256: `a915b600a59b7512cc3894d154a6225a312a74526cc9c5965542107eddc37e6a`

## Newly assembly-mapped components

- `CODE.2 0x1BDAC..0x1BEF2` primary NPC-array loop -> `NpcScheduler1BDAC::execute`
- `CODE.2 0x14A60..0x14AB1` -> `CharacterNameLookup14A60::find`

The primary NPC scheduler preserves the BF38 global gate, runtime suppression and +0x10 countdown, four-state action jump table, `0x1BBD8/0x1B760` movement/decision handoff, `0xD550` attack promotion, definition cooldown, state-bit tripling rule, fixed seven-tick post-attack recovery, and normal animation/behavior branches. The later loops in the original parent routine that operate other runtime arrays are intentionally not mislabeled as part of this converted primary loop.

`0x14A60` preserves the four-record `strcmp` scan and the original low-byte-only +0x10 bit-0x80 eligibility test. The currently proven combat callers pass the object-2 literals `VLADISLAV` (`0x584`) and `IRMGARDE` (`0x578`).

## Live opening integration

`OpeningDemoSession` now owns the shipped Quick Start party records and level-22 NPC definition database and can drive the opening assassin through the reconstructed scheduler. The integration deliberately keeps the original timing sources as explicit boundaries:

- `0x30700` -> DS:F58C timer source
- `0x311E0` -> BIOS tick source
- `0x19CC4` -> DS:A7E4 gameplay/combat clock

They are not replaced with the Win32 harness's 33 ms presentation cadence.

DS:C658 in the D550 context is corrected to the party-facing angle, matching the independently reconstructed player collision path and the D550 divide-by-45 use.

## Regression-gate correction

The original-data test previously required `SAVE01/LEV022.DAT`, while the shipped CD image exposes the Quick Start work level as root `LEV022.DAT`. That could cause the large Quick Start block to be silently skipped. The gate now accepts the same root fallback used by production `OpeningDemoSession`, and the resulting newly executed scheduler expectations were corrected against the actual shipped mutable work-level actor state rather than the pristine level placement.

The shipped Quick Start assassin begins at X=18.484375 in the mutable work level. Its first active scheduler decision leaves position unchanged and schedules action state 1; the following pass promotes to state 2 / enters D550, which rejects the attack while the assassin remains outside the fixed-point reach threshold; the next pass installs the original seven-tick recovery.

## Verification gate

From a fresh Release build directory:

- project configure/build: PASS
- CTest: 1/1 PASS
- full regression with `RAVENLOFT_DATA_DIR` pointed at the read-only original-data overlay: PASS
- direct `game.gog` ISO checks: PASS
- all shipped CINE00..CINE12 decode-to-end checks: PASS
- live Quick Start scheduler countdown/action/recovery integration: PASS
- `0x14A60` shipped and synthetic qualification cases: PASS

The source archive excludes transient build directories; no original Ravenloft game data is included.
