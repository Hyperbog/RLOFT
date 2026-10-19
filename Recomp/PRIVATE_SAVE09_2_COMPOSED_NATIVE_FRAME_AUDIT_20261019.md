# Private SAVE09(2) Level 5 — independently audited native frame

Date: 2026-10-19; branch `Recomp`; native owner run from
[Windows Milestone #165](https://github.com/Hyperbog/RLOFT/actions/runs/39213588061)
at source `fbb3b866bbd5b1d0062da32a2f6e4f7dbae7f96f`.

**Executive result: source native composed-frame execution verified YES;
visual two-skeleton/hud completeness verified NO; independent DOS VGA/DAC
pixel parity NOT VERIFIED. Do not promote R08/R09/R12 DOS acceptance.**

## Exact owner-private inputs (hash attestations only)

User supplied these files directly for this audit:
`SAVE09(2).zip`, `native_source_full_frame.log`,
`private_validation_receipt.txt`, and
`two_skeleton_complete_native.bmp`.

The uploaded ZIP SHA256 is
`be4f45c5f5eab9853705fa540cfd85c085ecf7f7c1235fd5dc76daa7ed3d3133`.
Read-only ZIP inspection confirms the required entries and bytes:

- `SAVE09/SAVE.DAT`: SHA256
  `28a4b28473997d2682d0a4671e104b13fbad078eea49b0369205ed8ed3669542`.
- `SAVE09/LEV005.DAT`: SHA256
  `b058964ce39d931e89417ef7f36f93640e48c57cbfdc9d5b363457fba238a672`.

Both match the owner's original-provenance hashes and local private
receipt, confirming that this is the intended TWO-SKELETON Level 5 save,
not the repository's unrelated archived SAVE09 camera.

## Native source execution evidence

Owner log:
- Reconstructed original `07938 -> 181B4` saved level 5,
  x16=1419090, y16=1436586, heading=198, party=4.
- 2 original source-visible primary actors and 2 projected callbacks;
  2 original `36CEC` source sprite callbacks.
- 78 visible raster runs, 287 `369D8` mask passes,
  5,515 generated indexed raster CPU writes.
- 4,499 of the 5,515 source CPU writes were recorded outside the
  A0000-AFFFF VGA aperture. Hence only **1,016 generated CPU
  writes were inside that aperture**, and even those are not
  necessarily unique, scanout-visible sprite pixels.
- RES4 outdoor, wall, quad dispatch passed; 12 RES0 HUD resource
  lookups/blits executed; HUD operation reported 656 changed
  visible indices. These counters verify source execution, not
  display-equivalent visual content.
- Source runner reports
  `CODE2_GENUINE_SAVED_SOURCE_FULL_FRAME_PASS`, indexed frame
  length 128,000 and palette RGB6 length 768, with original DOS
  parity explicitly `NOT_VERIFIED`.

Independent uploaded BMP examination:
- Indexed 320x400 BMP, 129,078 bytes with palette.
- Decoded BMP 128,000 top-down palette indices SHA256
  `626463ab4937ea02632353dd7bc1d1978fac9662d8371abaa0369592410565a5`,
  **identical** to receipt `NATIVE_FRAME_SHA256`.
- Its palette entries, converted to 6-bit RGB by `rgb8 >> 2`,
  SHA256
  `1f60d7831cd7b034ae67c894dc91662f15d4341fbf28e2a954b1b7e3e780f64f`,
  **identical** to receipt `NATIVE_PALETTE_SHA256`.
- Exactly 65,979 nonzero indexed pixels and 79 used indices.
- Rows 260 through 344 (inclusive) are **entirely index zero**.
  Only 933 nonzero pixels occur below row 259, all confined to
  x=57..90, displaying one partial figure at lower-left.
  The figure reaches the bottom edge; a second complete
  skeleton and finished game HUD are **not visually established**.
- The current BMP thus reveals serious frame-composition/render
  placement work remaining despite source-callback and hash success.

These measurements were made directly from owner uploads; the
original resources, private save, screenshot and full palette bytes
are **not included in Git**.

## Engineering distinction and next work

The runtime can now safely complete a private native source-composed
frame, which was previously blocked by host acceptance of off-aperture
CPU writes. The earlier B0000 CPU write handling is a bounded host
policy, **not** independent VGA hardware parity.

Next investigate the disconnected scene/HUD presentation:
1. Compare original DOSBox CRTC 0C/0D, line compare 18/07/09,
   repeat-scanline mode, display page and four VGA planes during
   precisely this saved camera and animation phase.
2. Trace original HUD **base** display initialization and full
   lower-screen composition; `1EA04`'s 12 RES0 blits/656
   altered indices are not evidence of complete base UI.
3. Trace which of the two `36CEC` source sprite calls actually
   affect active CRTC scanout; record source sprite XY, vertical
   phase, page, effective CPU/planar addresses and occlusion,
   without synthesizing missing skeletons.
4. Generate an independent original-DOS 320x400 raw `.idx` and
   256xRGB6 `.pal`, then run `compare_dos_indexed_frames.py`.
   No user-facing independent DOS parity claim before these match.

**Acceptance matrix:**
- exact private archive and receipt provenance — PASS;
- original two source actor callbacks — PASS;
- generated native indexed frame/palette integrity — PASS;
- visible two-skeleton composition — OPEN / incorrect partial output;
- complete HUD display — OPEN / visibly missing;
- independent DOS VGA/DAC parity — OPEN;
- continuous original game boot — OPEN.
