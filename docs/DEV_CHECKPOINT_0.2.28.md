# Development checkpoint 0.2.28

## Continuity base

This checkpoint continues directly from `Ravenloft_Alpha_0.2.27-dev-incremental.zip`. No renderer/viewer rollback or alternate source snapshot was used.

- 0.2.27 SHA-256: `eba498f203782d1e994d76b77788c1747f6d475fd1c9a750b4e9bd91372aa33f`
- `Ravenloft_Decomp_Final.zip` SHA-256: `331c85756a36ba359c1f0823f612bd24e759965b361f73dbfc5910546f177729`
- original `game.gog` SHA-256: `a915b600a59b7512cc3894d154a6225a312a74526cc9c5965542107eddc37e6a`

## Mechanical-demo milestone

Verified mechanical-demo completion: **98%**.

## `CODE.2 0x223F4` opening screen-hit closure

The live alpha no longer chooses the assassin quest drops by object ID. The opening category-2 front half of the original interaction dispatcher now reconstructs the rendered-object hit records used by `CODE.2 0x223F4` and resolves a cursor coordinate to the runtime object index passed into the already-converted `0x21704` pickup path.

The reconstruction preserves:

- `0x35798` world-to-screen projection;
- normal 4/8-view `0x2B2F4` resource selection for opening objects;
- decoded RES3 sprite bounds and exact `0x21BF0` rectangle setup;
- the object-list builder's literal `+0x20` adjustment to hit-record Y bounds at `0x2BFC1/0x2BFC9`;
- stable far-to-near scale ordering used by the render list;
- `0x223F4` reverse draw-order scan from nearest/topmost to farthest;
- inclusive left/right/top/bottom cursor comparisons;
- click bit 0 as the gate into the category-2 pickup branch;
- the selected runtime source index as the argument to `0x21704`.

Other interaction categories in `0x223F4` remain separate reconstruction targets; this checkpoint closes the ground-object branch required by the opening demo.

## Alpha input change

The `1-4` development controls that acquired quest objects by ID are removed from the Win32 alpha. A left click inside the 256x256 world viewport is translated back to original framebuffer coordinates and enters the reconstructed screen-hit/pickup path.

`I` remains a temporary inventory-UI selector: it chooses the first free carried slot, but the actual transfer is still the previously reconstructed `0x117A0` exchange path. The final HUD/inventory presentation remains later work.

## New regressions

- focused synthetic records prove reverse draw-order selection, inclusive edge comparisons, and click-bit gating;
- shipped Quick Start regression approaches the assassin area through reconstructed movement, creates the four real death drops, rotates until a drop occupies the live 256x256 interaction viewport, selects each drop only from screen coordinates, feeds the selected runtime index through `0x21704`, stows the first three through `0x117A0`, and proves the fourth selected pickup starts the real mist condition;
- the pickup call is never supplied object IDs 116/135/136/233; their identities are discovered from the selected runtime records.

## Verification gate

This checkpoint passes:

- clean Release configure/build of the 0.2.28 working tree;
- ordinary CTest;
- focused screen-hit ordering/gating regression;
- screen-coordinate acquisition of all four original assassin drops;
- full read-only original-data regression;
- direct original `game.gog` validation;
- all shipped CINE00..CINE12 decode checks;
- all prior movement, combat, death, drop, pickup, ownership, mist, timing and SAVE.DAT regressions.

The Linux environment does not compile the Win32 target because the Microsoft Windows SDK/toolchain is unavailable; the Win32 input bridge is source-only here, while the complete game-layer hit/pickup path is covered by the cross-platform regression executable.
