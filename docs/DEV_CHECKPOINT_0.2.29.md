# Development checkpoint 0.2.29

## Continuity base

This checkpoint continues directly from `Ravenloft_Alpha_0.2.28-dev-incremental.zip`. No renderer/viewer rollback or alternate source snapshot was used.

- 0.2.28 SHA-256: `81601b851b44c4fc18be4af6ad09c51b6e33c8a72e5a335a5ca619add3be354d`
- `Ravenloft_Decomp_Final.zip` SHA-256: `331c85756a36ba359c1f0823f612bd24e759965b361f73dbfc5910546f177729`
- original `game.gog` SHA-256: `a915b600a59b7512cc3894d154a6225a312a74526cc9c5965542107eddc37e6a`

## Mechanical-demo milestone

Verified mechanical-demo completion: **99%**.

## `CODE.2 0x12E70 / 0x11FB0` opening inventory selection closure

The temporary `I` first-free-slot selector is removed. Opening inventory stow now starts from reconstructed original interface coordinates and follows the original slot-selection chain into the already converted `0x117A0` held-object exchange.

The reconstruction preserves:

- `0x12E7B..0x12E9C` global Y division into four 80-pixel character panels and conversion to panel-local Y;
- inactive-character status bit `0x8000` gating;
- `0x12F9B..0x12FBA` original X 297..396 and local Y 5..74 inclusive inventory rectangle;
- click bit 0 as the inventory-grid activation gate;
- `0x11FB1..0x11FC9` click bit 1 rejection and nonzero DS:6F90 panel-mode rejection;
- `0x11FCA..0x12014` 34-pixel column and 18-pixel row pitch arithmetic for the 3x4 slot grid;
- `0x12012..0x12020` selected character/slot dispatch into the reconstructed `0x117A0` exchange.

The Win32 alpha currently draws a temporary 1:1 mechanical inventory proxy. It is not a replacement HUD; it exists only to supply the exact original coordinate space to the reconstructed selector until the final original interface presentation is composed.

## New regressions

- focused slot mapping verifies original bounds and grid arithmetic;
- missing click bit, click bit 1, and nonzero panel mode are rejected;
- a real held quest object is stowed into the clicked raw party slot through `0x117A0`;
- the end-to-end assassin-drop acquisition regression now chooses empty carried slots through actual inventory-grid clicks instead of the automatic first-free-slot helper;
- the fourth clicked pickup still triggers the original ownership/mist condition and the complete quest cleanup/level-23 transition remains green.

## Verification gate

This checkpoint passes:

- clean build of the 0.2.29 working tree;
- ordinary CTest;
- focused inventory selection/gating regression;
- end-to-end screen-coordinate drop pickup plus inventory-grid stow;
- full read-only original-data regression;
- direct original `game.gog` validation;
- all shipped CINE00..CINE12 decode checks;
- all prior movement, combat, death, drop, pickup, ownership, mist, timing and SAVE.DAT regressions.

The Linux environment does not compile the Win32 target because the Microsoft Windows SDK/toolchain is unavailable; the Win32 proxy/input bridge is source-only here, while the complete game-layer inventory selector/exchange path is covered by the cross-platform regression executable.
