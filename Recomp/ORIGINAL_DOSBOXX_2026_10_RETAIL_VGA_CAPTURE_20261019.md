# Independent DOSBox-X 2026.10.01 retail VGA capture — first original-frame evidence

Date: 2026-10-19 · Branch `Recomp`.
Owner supplied a screenshot `image.png` of retail Ravenloft and
an emulator save `1.sav`. These are **original running DOSBox-X game state**,
not the source-native renderer. **Do not commit either upload or any
decoded raw VGA planes, palettes, screenshots, or frame index arrays.**

## Input provenance and reproducible extraction

`1.sav` SHA256:
`0ce395f1e3017673e2ae7d584fd2ab7dfef39198c99879ab7e84901fa0be23d6`

The save is a DOSBox-X ZIP with:
- `DOSBox-X_Version = DOSBox-X 2026.10.01 (SDL2)`
- `Program_Name = CODE`
- `Machine_Type = MCH_VGA`
- `Time_Stamp = 2026-10-19 19:03`
- 2,643,024-byte uncompressed `Vga` component, a separate `Render`
  component and independent `CPU` and `Memory` components.
- Screenshot `image.png` SHA256
  `805ddb48e78e5f65eab104b7e80e510ac329929f0ea665a76e2ab32289c4856c`,
  642x452 including SDL title/menu chrome.

Original DOSBox-X `src/hardware/vga.cpp` `SerializeVga::getBytes`
writes the original VGA register/DAC POD structs and
`POD_Save_VGA_Memory` copies `vga.mem.linear` raw.
The original `include/vga.h` confirms the `VGA_Crtc` and
`VGA_Dac` field layout. The matching version-specific guest archive
offsets in `Vga` were independently validated against both the
serialized hardware registers and raw screenshot pixel patterns:

| VGA component | Exact saved-state byte offset |
| --- | --- |
| CRTC register 00h | 0x82F8C |
| VGA_Dac struct | 0x82FB1 |
| DAC RGB[256] (768 6-bit channels) | 0x82FD1 |
| Start of `vga.mem.linear` | 0x84A40 |
| VGA linear memory region | 0x200000 bytes |

The independent, strict, standard-library-only parser at
`Recomp/tools/extract_dosboxx_2026_10_vga_state.py` is
**specific to this DOSBox-X serialized layout**, including platform,
version and length signature; do not generalize byte offsets to another
DOSBox-X version or a 32-bit emulator build without new evidence.

## Independently captured original VGA register state

- Exact CRTC index 0..18h:
  `5f 4f 50 82 54 80 bf 1f 00 00 00 00 80 00 00 00 9c 8e 8f 28 00 96 b9 e3 fe`
- CRTC start address: **0x8000** (the original second display page).
- CRTC register 13h display line offset: **0x28** (40);
  first-person mode uses 80 VGA plane-address bytes per row / 320 pixels.
- CRTC maximum scan-line/overflow + register 18h line compare:
  **0x1FE (510)**, outside the 0..399 output range of this capture;
  do not claim a split at y256 based only on source sprites.
- CRTC mode control 17h: **0xE3**.
- DAC channel precision: **6 bits**, PEL mask `0xFF`.
- Four original VGA planes exist as interleaved byte lanes
  `vga.mem.linear[4*plane_address + plane_index]`;
  save includes both display pages at CRTC addresses 0 and 0x8000.
- Original palette `RGB6` (768 bytes) SHA256:
  `0af8ea393f1f266596af59c1a2013350b51000b7456d377ab284ebbc43db0812`.
- Original saved CRTC-selected (page1) 320x400 raw array:
  SHA256 `ecdb65f31c8589e22fd4b666583067e63708fd16bec50675239b28ddf8e80445`.
  Page0 320x400 SHA256 `efd903027cf14914742890831f01a4123749a663ba69cc3c1d4c0d9ec447ceef`.
  Page0 and page1 differ at just 305 byte positions.

The saved selected-page memory is a **memory snapshot**, not proven to
be the scanout that produced the independently taken screenshot at
exactly the same scanline/tick.

## DOSBox-X screenshot validation and source comparison

A truly native 320x400 screenshot can be cropped, without resampling,
from desktop `image.png` by taking source pixels
`x=1..640 step 2, y=51..450`.
**Every one of the 128,000 resulting RGB pixels** matches an expanded
`(component6 << 2) | (component6 >> 4)` color in the **independently
extracted original DOSBox-X DAC**.

This *validates palette provenance* but does not uniquely reconstruct
indexed bytes from screenshot alone, because some VGA palette RGB
triples are duplicates. Use the real VGA memory dump instead.

The screenshot has the normal first-person upper frame; the actual
save-state VGA memory selected page contains the game's **top command
menu** (REST / PRAY / MEMORIZE / AUTOMAP / LOAD / SAVE / QUIT /
OPTIONS). The skeletons are also in different animation poses.
Therefore screenshot color-versus-saved-VGA matches only
95,486 / 128,000 RGB pixels, and the state and screenshot are **not
a synchronized pair**. This is not a native renderer failure.

Native `SAVE09(2)` source frame from accepted Milestone #185:
- Indexed SHA256
  `bdd0d5090c50802ae27b4b951af028d69c6981220cedb37f5fc9559567da9f10`
- RGB6 palette SHA256
  `1f60d7831cd7b034ae67c894dc91662f15d4341fbf28e2a954b1b7e3e780f64f`

**Confirmed original/native DAC divergence:** 8 RGB6 entries
**246..253** differ, 24 component bytes total. All other **248**
entries are byte-identical. The raw RGB6 triples are retained only in
the owner's private JSON audit and `.pal` extraction outputs, **not in
the public source checkpoint**.

At their different animation/UI instants, original screenshot vs
native source 320x400 frame:
- Native palette produces 76,579 matching RGB pixels;
  51,421 differ.
- Rendering the **unchanged** native indices with the extracted
  original DAC produces 86,753 matching RGB pixels;
  only 41,247 differ — 10,174 additional matching positions.
  This supports targeting the native DAC mismatch but is not
  exact-game-pose or frame parity.
- Comparing original saved page indices directly with native gives
  56,417 different byte positions. **Do not treat this as a valid
  synchronized original retail framebuffer comparison.**

## Next gate — unmodified retail synchronized capture

The original VGA state, DAC and frame extraction exist and are real.
Do not mark DOS parity passed. To establish a true pixel-equivalence
oracle, create a **frame-synchronized** original retail capture at
the exact same 320x400 screen draw/animation phase as the native
static saved-world frame (or reproduce that original frame phase
within the native renderer) without invoking the top game-menu UI
between framebuffer screenshot and VGA state capture. A debugger
pause/capture at the correct renderer/frame boundary is preferable
to independently pressing screenshot and Save State on different
emulator ticks.

Then run `Recomp/tools/compare_dos_indexed_frames.py` on the
independent original 128000-byte `.idx` and 768-byte `.pal` and
the accepted native `.idx` and `.pal`. Document CRTC address
and draw/show page, DAC mode, effects of top menu, and sprite
animation. **Keep any 200 pixels changing at y256..260 open until
the synchronized original reference proves their visible behavior.**

All original media remain owner-local and must not be committed.
