# PRIVATE SAVE09(2) — original two-skeleton native frame visual acceptance checkpoint

Date: 2026-10-19. Branch: `Recomp`. **Owner-only media and image not committed.**
Evidence supplied directly in this project: `SAVE09(2).zip`,
`private_validation_receipt.txt`, `native_source_full_frame.log` and
`two_skeleton_complete_native.bmp`. This is a follow-up to
[`SOURCE_VGA_SPRITE_ROW_AND_HUD_BASE_CHECKPOINT_20261019.md`](SOURCE_VGA_SPRITE_ROW_AND_HUD_BASE_CHECKPOINT_20261019.md)
and supersedes the *visual defects* from the old
[`PRIVATE_SAVE09_2_COMPOSED_NATIVE_FRAME_AUDIT_20261019.md`](PRIVATE_SAVE09_2_COMPOSED_NATIVE_FRAME_AUDIT_20261019.md)
without superseding its old-build provenance.

## Acceptance from independent supplied-file inspection

**New #171 source-native frame successfully renders two visibly distinct skeletons
in the textured first-person scene, with restored original RES0 UI background
and movement-pad frame.** It does not yet include the full character HUD
(portraits, hand/equipment, live HP/status), nor is it DOS/VGA/DAC pixel
parity. Distinguish these acceptance scopes.

The private archive `SAVE09(2).zip` independently hashes to
`be4f45c5f5eab9853705fa540cfd85c085ecf7f7c1235fd5dc76daa7ed3d3133`.
Its exact members are:

- `SAVE09/SAVE.DAT`: SHA256
  `28a4b28473997d2682d0a4671e104b13fbad078eea49b0369205ed8ed3669542`.
- `SAVE09/LEV005.DAT`: SHA256
  `b058964ce39d931e89417ef7f36f93640e48c57cbfdc9d5b363457fba238a672`.

Both match the new owner's source receipt and the prior private-save
identity. This is **not** the different archive `DnD-Classics/SAVE09`.

The provided palettized 320×400 BMP independently decodes to exactly
128,000 palette indices, SHA256
`6216a0c7eb8f8715421d4fdb5f35f54be20dabecf853c387064b193ee28d8654`,
and 768 RGB6 bytes (after 8→6 component conversion), SHA256
`1f60d7831cd7b034ae67c894dc91662f15d4341fbf28e2a954b1b7e3e780f64f`.
Both exactly match the owner's receipt. The palette SHA equals the
previous owner build's palette, isolating this observed change primarily
to rendering/indexed-frame data rather than DAC changes.

## Original CODE.2 source and visibility

- Exact authentic Level 5 saved camera: x16=1419090, y16=1436586,
  heading=198, party=4.
- Two source `2B6D0` visible primary actors, two `2C294`
  projection callbacks and two genuine `36CEC` RES3 sprite draws.
- 78 visible raster runs, 287 `369D8` plane passes,
  **5,515 original indexed CPU raster writes**.
- **Zero off-aperture writes**. The old build had reported 4,499,
  caused by incorrectly using the SINTAB clip scalar as the actual
  `0x10450` VGA destination row. New source address differentiation
  is demonstrably consequential on the **exact private save**.
- Sprite #1 `RES3:132`, source_x=45, projected_size=411,
  visible bbox x=55..106, y=105..253. **2,276 active-page pixels
  changed**, all in source world rows before 256.
- Sprite #2 `RES3:164`, source_x=176, projected_size=775,
  visible bbox x=139..246, y=87..260. **6,968 active-page pixels
  changed**, **6,768** before physical row 256 and **200** after.
  The 200 pixels crossing y255 remain a specific independent retail
  viewport-clipping parity question, not evidence of a verified fault.

## HUD and compositor

- Original `1F8FC → 2E510 → 2F29C → 2E4A4 → 2939C → 2A380`
  source-frame base produces **two genuine RES0 backgrounds and
  400 frame blits**.
- `1EA04` adds 12 HUD image blits and reports 659 changed indices.
- Native frame: 122,020 nonzero indexed pixels, 110 distinct indices.
- Physical rows 260..399 contain 39,575 nonzero indexed pixels;
  **zero fully black rows**. Original movement control pad and ornate
  frame are now visibly populated.
- Compared with the older owner native frame `626463ab...`
  (65,979 nonzero pixels), **66,115 indexed pixels differ**;
  the new frame is not just the old one with a forged receipt.
- Visually: two recognizable skeletons in correct side-by-side
  first-person placement. Background/frame and movement pad appear.
  Empty portrait and equipment rectangles remain clearly visible.
  Those are *not* missing from the RES0 base: party data requires
  original `1F494` / masked `298F0 → 2A584` and hand/status
  routines.

## Strict acceptance matrix

| Verification | Status |
| --- | --- |
| Exact user's SAVE09(2) SHA and LEVEL data SHA | PASS |
| Original source emits 2 actors and 5,515 CPU stores | PASS |
| 2 separate active draw-page VGA footprints and visible skeleton shapes | PASS |
| Original RES0 frame background with 400 blits | PASS |
| No black lower HUD region | PASS |
| 320×400 BMP index and 256×RGB6 receipt SHA parity | PASS |
| Entire character HUD (four portraits, hands, HP, status) | OPEN |
| Retail viewport boundary / 200 pixels y>255 | OPEN |
| Independent DOSBox/retail VGA planes and indexed frame parity | OPEN |
| Independent original DAC register palette parity | OPEN |
| Continuous original game runtime boot and behavior parity | OPEN |

### Next original-source work

1. Wire original per-character `1F494` from real SAVE records,
   including RES1 portrait selection and original `298F0 → 2A584`
   mask semantics; then `1EA90` hand, HP, and status widgets.
2. Keep drawing-order and CRTC active vs visible page explicit and
   backed by the original source; compare the 200 changed sprite pixels
   at y256..260 with the independent original DOS visible framebuffer.
3. Obtain **independent** original DOSBox-X or hardware VGA four-plane
   snapshot, CRTC state, active DAC RGB6 and actual indexed scanout at
   this exact Level5/heading198 saved camera. Run
   `Recomp/tools/compare_dos_indexed_frames.py`.
4. Do not mark the formal continuous boot or R08/R09/R12 full VGA
   parity as accepted until an independent original reference agrees.

**Repository privacy:** only source-independent hashes, metrics,
provenance labels, and acceptance results are stored. Do not commit
original artwork, screenshots, raw indexed pixels, palettes, SAVEs,
game executables or RES resources.
