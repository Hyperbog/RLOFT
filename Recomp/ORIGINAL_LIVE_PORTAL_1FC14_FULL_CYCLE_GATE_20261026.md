# Part 36 — Original 20850, 1F8FC, and genuine portal 1FC14 acceptance

**Branch:** `Recomp`. **Current acceptance:** the original actor and graphics callbacks build and the original Level 23 → 26 → 23 type-8 portal *save/restore* round-trip passes. A live GUI **destination frame is not yet witnessed**.

## Source-authentic callback graph

- `181B4` loading now calls compiled `20850` for destination actor records through source `2D5D0`, `2DF2C` and `0E0F8` with original `30CA2`, `30700`, `290D0`, and `1C328` callbacks. Guest source records, not host movement rules, determine the result.
- `181B4` routes source `1F8E0 → 1F8FC` graphics-mode switches through authentic RES archives, original video functions, and the owner's `COLORS` bank, not a static imitation.
- Source `171FC` writes `WORK\\LEVnnn.DAT` into bounded, private owner-session memory; native gameplay never changes the original game files. Saved-world restoration invokes the original `181B4` direct-save path.
- The source `2D3D8` portal's true record coordinates/heading are taken from the untouched `LEVELS.DAT` archive. The isolated preserved-media Windows gate verified the bidirectional original Level 23 ↔ 26 record pair, guest map/script persistence and 554 source saves / 219 source restoration reads.

## Genuine native-frame proof tightened

After an opt-in live portal callback returns, `original_pending_level_frame` stays outstanding across two independent acceptance boundaries:

1. Compiled `1FC14` must return through original scene-polygon and sprite rasterization and the `1A510` page handoff. A scene-only result logs `1FC14_AFTER_181B4_SCENE_RETURN`, **not success**.
2. On the next keyboard-loop re-entry, the original `1FC14` post-scene audit must independently verify page-return count, HUD callbacks, actor audio query and random callback; the retained VGA must expose exactly 320×400 indexed pixels and 256 RGB6 palette entries, with a valid source CRTC page. Only then may it log `1FC14_AFTER_181B4_FULL_CYCLE_SCANOUT_PASS` with private SHA256 frame/palette witnesses.

No native post-portal full-cycle success marker appeared in the passing Windows CI: its portal test deliberately does not execute a retail VGA frame. Therefore neither DOS frame parity nor completed playable transition is claimed.

## Authenticated portal topology matters

Inspection of the owner's unmodified 32-level `LEVELS.DAT` shows **no type-8 portal records in Level 22**. Two Level 23 portals lead to Level 26, and Level 26 has reciprocal portals back to 23. A transition triggered by host forcing Level 22 → 23 would **not** count as a genuine Level 22 portal.

The current Windows preview starts at Level 22. The first authentic GUI portal demonstration must therefore provide a **source-correct guest state in Level 23** (for example through verified original save-game restore) before walking into the archive's real type-8 portal tile. This is a **separate outstanding source startup/save acceptance gate**; do not introduce fake Level 22 gateways merely to satisfy an animation/screenshot milestone.

## Reproduction

1. Download the Windows milestone artifact from the successful `Recomp` workflow.
2. Extract the entire ZIP and run the owner-local launcher. Use `-ExperimentalPortals` in `Launch-RLOFT-Level22.ps1` only for explicit experimental traversal; it never automatically turns on the unsupported branches.
3. For a proof attempt, retain the source diagnostic output and original private frame observation. Require **both** `171FC_181B4_LIVE_SOURCE_RETURN` and `1FC14_AFTER_181B4_FULL_CYCLE_SCANOUT_PASS` for the destination level. Mere save-file round-trip success, a 1F8FC compositor return, or a single scene return is insufficient.

## Verified CI vs open work

All **92/92** Linux, canonical source-link and Windows CTests passed on commit `01d9d2b51913fb86cde9ba7f022d47af1d7aac36`. The Windows preserved-media workflow separately passed `CODE2_ORIGINAL_2D3D8_PORTAL_23_26_23_SOURCE_ROUNDTRIP_PASS`; it expressly labels retail VGA frame `NOT_EXECUTED` and original NPC postload `NOT_CERTIFIED`. Original Linux/Windows tests are not independent synchronized DOS capture.

**Next:** connect authenticated original save-game 07938 restoration into the retained GUI guest for a legal Level 23 starting state, test its actor and graphics world, walk a *real* Level 23 → 26 archive portal through the original 1FC14 loop, and capture a successful post-portal indexed framebuffer. Verify independent retail DOS parity later.
