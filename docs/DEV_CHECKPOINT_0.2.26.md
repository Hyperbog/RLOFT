# Development checkpoint 0.2.26

## Continuity base

This checkpoint continues directly from `Ravenloft_Alpha_0.2.25-dev-incremental.zip`. No renderer/viewer rollback or alternate source snapshot was used.

- 0.2.25 SHA-256: `e6e23ac9f103da3abaec055209aad6387c4cb5dfe940f75507c4ee1b0438dd6f`
- `Ravenloft_Decomp_Final.zip` SHA-256: `331c85756a36ba359c1f0823f612bd24e759965b361f73dbfc5910546f177729`
- original `game.gog` SHA-256: `a915b600a59b7512cc3894d154a6225a312a74526cc9c5965542107eddc37e6a`

## Mechanical-demo milestone

Verified mechanical-demo completion: **96%**.

## Live production combat closure

The alpha no longer requires the F6 development death shortcut to complete the assassin combat portion. The platform bridge now routes temporary keyboard hand bindings into the converted production click path:

`0x214B8 -> 0x24478 -> 0x26018 -> 0xE5F0 -> 0xC80C/0x1C328/0xC910 -> 0xE284 -> 0x2195C`

Bindings are intentionally presentation-layer placeholders until the original HUD is reconstructed: Z/X = character 1 left/right hand, C/V = character 2 left/right hand. They do not alter gameplay semantics.

## New regression

A shipped Quick Start end-to-end regression approaches and faces the original type-9 assassin through reconstructed movement, attacks only through `attackOpeningHand(..., clickFlags=1)`, advances the original gameplay clock for cooldown expiry, and kills the assassin without calling `resolveAssassinDeath()`.

The deterministic seed-1 sequence is:

- hit for 3 (HP 15 -> 12)
- miss
- hit for 9 (HP 12 -> 3)
- miss
- hit for 3 (HP 3 -> 0)

The regression then proves BF44=1, dead/suppressed actor state, and runtime drops 136/135/233/116.

## Verification gate

This checkpoint passes:

- fresh Release configure/build (build resumed after an external command timeout; no compiler failure occurred);
- CTest;
- live production-click assassin kill regression;
- full read-only original-data regression;
- direct original `game.gog` validation;
- all shipped cinematic decode checks;
- all prior movement, combat, death, drop, pickup, ownership, mist and timing regressions.

The Linux environment cannot compile the Win32 presentation target itself because the Microsoft Windows SDK/toolchain is not installed; the platform binding is a thin source-only change over the fully tested game-layer path.
