# Second independent DOSBox-X VGA capture — temporal separation and COLORS bank3 DAC reconciliation

Date: 2026-10-19. Branch `Recomp`. Input: owner-uploaded exact
`image.png` and `1.sav` from running original DOSBox-X 2026.10.01 SDL2.
The original user-supplied save, captured media, decoded indexed frame
and DAC bytes must remain **private** and must NOT be committed.

## Input provenance and original registers

- Second DOSBox-X `1.sav` SHA256
  `7607207004fb14b414fe886e1d3c8b2d844bdc325c1c6f91548d49ed349332f4`.
- Screenshot `image.png` SHA256
  `72e97fcdd6e788d0c98d761398f0685aae570d6a843ea99a8e9c0fa148155007`.
- State timestamp `2026-10-19 19:29`, program CODE, VGA component
  2,643,024 bytes, ZIP version `DOSBox-X 2026.10.01 (SDL2)`.
  Neither input equals the earlier 19:03 original capture.
- Original CRTC start `0x8000`, CRTC offset `0x28`,
  CRTC line compare `0x1FE`; original 6-bit DAC with PEL mask 255.
- Original RGB6 DAC SHA256
  `0af8ea393f1f266596af59c1a2013350b51000b7456d377ab284ebbc43db0812`,
  EXACTLY matching the earlier DOSBox save-state palette SHA.
- This second save selected-page indexed-frame SHA256
  `109ad5f45c82dc184500496d9784c8b2004a444301f6b7798cded71f84de5375`.
  It is a different animated picture from the screenshot and native
  saved-world renderer, so raw differences are not parity verdicts.

## Pair temporal alignment

The supplied `642x452` image has visible window title/toolbar chrome.
A carefully verified crop samples source x=1,3,...,639 and y=51..450
to obtain a 320x400 RGB image; all 128,000 RGB pixels belong to the
independently recovered original DAC, authenticating palette provenance.

Original CRTC-selected VGA page from the new SAV compared to its own
uploaded screenshot:
- **116,214 of 128,000 RGB pixels match** (90.792%),
  **11,786 differ**. The skeleton animation poses visibly diverge.
- Interior first-person world x32..287,y0..255 matches
  **54,154/65,536 (82.63%)**.
- Lower HUD y260..399 matches **44,484/44,800 (99.295%)**.
- Top 50 rows exactly match **16,000/16,000**.
- The central animation mismatch is greatest around skeleton arms,
  weapons and faces; old/new original capture screenshot and state
  must NOT be treated as an exact single frame despite high HUD agreement.
- This image was not a native DOSBox-X Capture-menu screenshot;
  window title and DOSBox host toolbar are present. Do not ask owner
  to run additional tools while we still have independent hardware
  state and can address deterministic source DAC differences.

## Important new provenance: all eight palette mismatches are COLORS bank 3

The independently saved original RGB6 DAC differs from the existing
source-native bank0 RGB6 palette only at **indices 246..253**, 24
channel bytes; 248/256 entries already agree.

I fetched the ORIGINAL retail `COLORS` resource read-only from
`DnD-Classics/MASTER/Ravenloft - Strahd's Possession/RLOFT/COLORS`,
Git blob SHA `452733d3d755e26a6a62baefbb100bdaa4ba37da`. The entire
resource is 11,521 bytes: 15 banks of 768 RGB6 bytes plus the 1-byte
count header. At all eight relevant indices (246 through 253),
original **bank 3** matches the original hardware VGA DAC **exactly**,
while the previous native frame used those entries from bank 0. The
remaining 248 hardware entries equal native bank 0 exactly.

Consequently the source-asset-backed mixture:
```
256-entry RGB6 frame = original COLORS bank 0
indices 246..253 only = original COLORS bank 3 (8 RGB6 triples)
```
matches the exact ORIGINAL independent DAC palette SHA256
`0af8ea393f1f266596af59c1a2013350b51000b7456d377ab284ebbc43db0812`.

This is a precise independent VIDEO-OUTPUT and GAME-RESOURCE match.
The original retail `325B0` can load palettes from the original
resource's row/bank; original source `323BC/324F8` supports palette
slices/rotation. **We have not yet located the exact original gameplay
caller that installs precisely this bank3 slice**, so do not falsely
claim complete instruction-by-instruction DOS palette-state parity.
Do not replace all 256 entries with bank3.

## Implementation/test

- `Recomp/src/code2_compiled_graphics_frame.cpp` now has an
  *optional* `original_dac_observed_slice` parameter when loading
  read-only COLORS. It loads original bank0, selectively overwrites
  eight RGB triples from original bank3, then passes the original
  result through the already-recompiled CODE.2 `32150` brightness
  and `29CA0` DAC output pipeline.
- `Recomp/src/code2_original_world_frame.cpp` enables this path
  only on genuine composed saved-world frame and original-resource
  synthetic status fixture. All generic graphics tools remain
  unchanged. A machine-readable log explicitly labels the retail
  palette caller sequence `NOT_VERIFIED`.
- `.github/workflows/recomp-saved-world-linux.yml` has a strict
  original resource/archived save integration gate checking the entire
  768-byte palette output SHA256 against the independent original
  hardware capture SHA, without committing or shipping palette bytes.

### What the screenshot comparison can and cannot show

Native Build185 RGB6 palette rendered over the static accepted
native 320x400 indexed frame agrees with the second ORIGINAL
screenshot at 76,587/128,000 RGB pixels. Replacing **only** 8
palette triples by observed original bank3 triples raises that
to **86,756**, an improvement of **10,169 matching pixels**.
The source-original 8 palette entries are used in **10,671 native
pixel positions**; the other native indices remain unchanged.

Original savestate selected-page indexed-frame versus static
native saved-world index comparison differs in 42,992 positions,
but the two skeleton poses and some status-frame phase remain
different. NO DOS indexed parity acceptance may be inferred.

## Next engineering gate and owner actions

No owner action is needed for the source-DAC correction.
After green Windows/Linux CI, distribute the matched native
build if a refreshed native screenshot/palette is desired. A
controlled screenshot+state pair with zero temporal mismatch
would be valuable eventually, but not a prerequisite for
finishing the verified original source palette update.

For true original indexed-frame parity, synchronize the original
sprite animation tick and status effect overlays with native
scene source, or capture an exact original frame phase at a CPU
draw+pageflip breakpoint. Do not report temporal mismatch pixels
as erroneous VGA rendering.
