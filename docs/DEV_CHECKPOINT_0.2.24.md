# Development checkpoint 0.2.24

## Continuity base

This checkpoint continues directly from `Ravenloft_Alpha_0.2.23-dev-incremental.zip`. It preserves the verified opening combat, assassin death/drop, pickup, inventory transfer, ownership, mist cleanup, and transition paths while replacing the opening attack helper's hard-coded NPC target with the original interactive hand-click and auto-target route.

- 0.2.23 SHA-256: `f8cdd9a5c972c814cd20c1e062f24403b4b2e5157b1cf2952ef0ee5504e1919b`
- `Ravenloft_Decomp_Final.zip` SHA-256: `331c85756a36ba359c1f0823f612bd24e759965b361f73dbfc5910546f177729`
- original `game.gog` SHA-256: `a915b600a59b7512cc3894d154a6225a312a74526cc9c5965542107eddc37e6a`

## Mechanical-demo milestone

Verified mechanical-demo completion: **94%**.

## Newly assembly-mapped components

- `CODE.2 0xDACC..0xDB93` -> `PlayerTargetSelectorDACC::select`
- `CODE.2 0xDB94..0xDBD2` -> `PlayerAutoTargetDB94::select`
- `CODE.2 0x214B8..0x21701` normal click-1 attack arm -> `PlayerHandClick214B8::execute`

`PlayerNpcAttackE5F0` and `PlayerHandAttack26018` now preserve the original `-1` target contract instead of requiring an already-selected actor.

## Exact behavior preserved

The reconstructed target path now preserves:

1. `0x214B8` inactive-character high-bit gate;
2. click-bit `0x08` priority as a distinct item-exchange branch;
3. normal attack only for click bit `0x01`;
4. character-state mask `0xF240`;
5. `0x24478` hand-usability call before attack dispatch;
6. literal target `-1` passed to `0x26018`;
7. `0xE5F0` calling `0xDB94` before `0xE578` when target is `-1`;
8. `0xDB94` even/odd character screen-half preference and fallback;
9. `0xDACC` previous-render rank scan, NPC category gate, suppressed/dead actor gate, screen-half split, `0x7D00` biased-scale/range rule, and targetable-state bit;
10. selected runtime actor index feeding the existing `0xC80C -> 0x1C328 -> 0xC910 -> 0xE284` path.

At the shipped Quick Start camera state the assassin is not a valid DACC target, and the production click route correctly produces `NoTarget`; it no longer silently attacks actor index 0.

## Verification gate

This checkpoint passes:

- fresh configure/build;
- CTest;
- focused `0xDACC/0xDB94` branch tests;
- exact scale-threshold and dead/targetable-bit tests;
- `0x214B8` click/status/usability gating tests;
- synthetic `-1 -> DB94 -> selected NPC -> hit/damage` integration regression;
- live shipped Quick Start normal-click `NoTarget` regression;
- full read-only original-data regression;
- direct original `game.gog` validation;
- all shipped cinematic decode checks;
- all prior opening combat/death/drop/pickup/mist regressions.

The source archive excludes transient build directories and original Ravenloft game data.
