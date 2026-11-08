# Part 23 — Private SAVE09(2): original 1F494 four-party frame visually renders; fix false two-status gate

Date: 2026-10-19. Branch: `Recomp`.
Source experiment: Windows Milestone **#183** owner-original private `SAVE09(2).zip`.
Checkpoint/fix: `6d4d1c448e8097423190c83527e2d685242503e5` corrected owner PowerShell acceptance; `b310c9637899d19407703015decab7f2393b12ab` corrected a stale diagnostic claiming portraits were missing even when original 1F494 had run.

## Cause: successful native compositor but false PowerShell rejection

The owner uploaded exactly the native source log and complete BMP from `CHATGPT_Full_Frame_6`, plus the PowerShell exception. The native log terminates with:

```
CODE2_GENUINE_SAVED_SOURCE_FULL_FRAME_PASS level=5 source_36cec_calls=2 raster_indexed_writes=5515 off_aperture_cpu_writes=0 frame_indices=128000 palette_rgb6=768 DOS_VGA_PARITY=NOT_VERIFIED
```

The 1F494 diagnostic, source-backed from this actual private save:
```
active_original_characters=4 source_state_lut_nonzero=1015 resource_calls=25 masked_298f0_2a584=15 direct_2975c=6 fills_29aa4=16 borders_2e374=1 hand_slots_1ea90=8 decimal_2e6f0=1 centered_30508=1 glyph_303c8=1 unsupported_text=0 callback_failure=0
```

The original caller **does not draw a fixed two status numerals**.
The prior mandatory `$decimalText -lt 2` PowerShell condition was
borrowed from an *archived game-save test with deliberately injected
two synthetic status fields* and was never a source-derived requirement
for this private original SAVE09(2).zip. The owner-original file has
one active status number; the image visibly shows that numeric overlay.
The correct owner acceptance checks `decimalText>=1`,
`decimalText==centeredText`, `glyphDraws>=centeredText`, and
`unsupportedText==panelFailure==0`. It does **not** add/modify a status
field to make the previous test pass.

The original native renderer completed successfully. The failing exit
was raised **afterwards**, by the runner at line 225 and before it wrote
`private_validation_receipt.txt`. A missing receipt from this run is
not an executable or frame failure. The 76-CTest build #185 ships
matching corrected runner and executable and will write the receipt
for a rerun of the same unchanged private save.

## Independent inspection of owner-uploaded native BMP

Uploaded `two_skeleton_complete_native.bmp` is original palette-indexed
BMP (320×400), visually inspected at 3× nearest-neighbor **without
altering underlying pixels**. Four different character faces, equipment
slot images, HUD controls and one red numbered status overlay are
visible below two genuine skeletons in the textured outdoor scene.
Source sprite positions remain from the previous accepted camera:
`RES3:132 bbox=(55,105,106,253)`, `RES3:164 bbox=(139,87,246,260)`.
Original 369D8 source indexed CPU stores remain **5,515** with
**0 out-of-aperture** writes.

Independent byte examination using Pillow:
- BMP file SHA256: `dee27c6ca3f047bef1914a2f989b8c7aeebeb1613226d34b8f27cac2351c603c`.
- Decoded exactly 128,000 indexed bytes; SHA256:
  `bdd0d5090c50802ae27b4b951af028d69c6981220cedb37f5fc9559567da9f10`.
- 768 RGB6 palette bytes derived as `RGB8 >> 2`; SHA256:
  `1f60d7831cd7b034ae67c894dc91662f15d4341fbf28e2a954b1b7e3e780f64f`.
- 119,130 nonzero indexed pixels; 210 distinct indices in the 320×400
  frame; no entirely zero rows in rows 260–399.
- Portrait-panel lower-region pixel evidence (bounds x=[0,53),
  [53,107), [211,264), [264,320), y=[300,399)):
  3,523; 3,863; 3,851; 3,504 nonzero pixels respectively.
  This is positive frame completeness evidence, NOT retail DOS
  pixel-position verification.

No original artwork, screenshot, private SAVE, palette or IDX is
committed to Git; only scalar hashes, counts and provenance labels.

## Verified GitHub gates for corrected acceptance

- [Windows MSVC Milestone #185](https://github.com/Hyperbog/RLOFT/actions/runs/39225595722), source `b310c9637899d19407703015decab7f2393b12ab`,
  **76/76 CTests passed**, runner PowerShell parser passed; artifact:
  [Milestone #185](https://github.com/Hyperbog/RLOFT/actions/runs/39225595722/artifacts/12173601567),
  GitHub archive SHA256 `6a5af1747d577638c01027a875273a421d6b072d75c4aee6d3f4691733f8c74c`.
- [Linux regressions](https://github.com/Hyperbog/RLOFT/actions/runs/39225595858): success.
- [Authenticated archived saved-world](https://github.com/Hyperbog/RLOFT/actions/runs/39225595908): success.
- [Source link](https://github.com/Hyperbog/RLOFT/actions/runs/39225595738): success.
- [Original RES3 actor discovery](https://github.com/Hyperbog/RLOFT/actions/runs/39225595848): success.
- [Source-authentic RES4](https://github.com/Hyperbog/RLOFT/actions/runs/39225595687): success.

## Acceptance boundary

**Proven:** native source compositor completed on exact owner
two-skeleton save, original 1F494/1EA90 portraits/hands/HP/status
callbacks execute without error, visual image contains all four
portrait panels, both skeletons, lower HUD; malformed fixed
two-status runner gate now corrected and built/tested.

**Not proven:** independent DOSBox original indexed-frame equality,
VGA split/CRTC/viewport boundary at y256..260, original DAC readback,
pixel-perfect portrait/font placement, continual original CODE1 boot
or all alternative/status-menu HUD behaviors. Formal R08/R09/R12
DOS parity/continuous-game acceptance remains open.
