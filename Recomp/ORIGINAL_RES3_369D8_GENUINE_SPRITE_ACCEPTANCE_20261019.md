# Verified genuine CODE.2 36CEC → 369D8 sprite pixels from untouched DOS saves

## Status, 2026-10-19

**First genuine indexed RES3 sprite writes achieved.** The exact owner
`DnD-Classics` read-only preserved saves provide **five** original
`2C294 → 36CEC → 369D8` emitted source calls and **3,111** bounded,
actual native indexed palette-pixel writes across 10 files.
All ten original isolated actor+scene probes pass without override,
save/actor/camera changes, or artificial ray visibility. **DOS VGA
pixel parity remains unverified**; these are native reconstructed
indexed writes and are not a DOS emulator framebuffer oracle.

Source acceptance workflow:
https://github.com/Hyperbog/RLOFT/actions/runs/39206757031
(`CODE2_ORIGINAL_UNTOUCHED_RES3_INDEXED_SPRITE_ACCEPTANCE_PASS
calls=5 writes=3111 DOS_VGA_PARITY=NOT_VERIFIED`).
Previous first positive run:
https://github.com/Hyperbog/RLOFT/actions/runs/39206321268 .

| Preserved original save | Level | Heading | 36CEC calls | 369D8 indexed writes |
| --- | ---: | ---: | ---: | ---: |
| CD_Extracted/RLOFT/SAVE.DAT | 22 | 270 | 1 | 70 |
| RLOFT/SAVE01/SAVE.DAT | 22 | 270 | 1 | 70 |
| cloud_saves/RLOFT/SAVE00 | 5 | 252 | 0 | 0 |
| cloud_saves/RLOFT/SAVE01 | 23 | 50 | 1 | 95 |
| cloud_saves/RLOFT/SAVE02 | 21 | 216 | 1 | 2749 |
| cloud_saves/RLOFT/SAVE03 | 5 | 15 | 0 | 0 |
| cloud_saves/RLOFT/SAVE04 | 5 | 190 | 0 | 0 |
| cloud_saves/RLOFT/SAVE08 | 5 | 181 | 0 | 0 |
| cloud_saves/RLOFT/SAVE09 | 5 | 187 | 1 | 127 |
| cloud_saves/RLOFT/SAVE10 | 5 | 252 | 0 | 0 |

## Source-rooted root cause of the raster block

The prior `Original369d8Context` code rejected the
**intermediate** `source_body+signed_display_list_adjustment`
if it fell outside the decoded sprite bounds. That check was NOT
present in original DOS execution; the original CODE.2 object-1
disassembly (physical image starting at `0x3AF24`) proves:

- `36E94..36EA5`: `2F29C(res_id,3)` returns the genuine
  length-prefixed sprite; `+2` is passed through `[ebp-4]`.
- `36F5D..36F67`, `3700B..37015`, `37166..37171`, etc.:
  caller pushes `[ebp-4]` unchanged to `36B14`.
- `36B23..36B31` and `36B74..36B7D`: `36B14`
  pushes this same coordinate pointer to `369D8`.
- `369D8`: machine code adds SIGN-EXTENDED 16-bit display-list
  adjustment to the caller coordinate to obtain callback `ESI`.
- The SINTAB x86 routine subsequently reads `[ESI + disp32]`.
  Thus an ESI preceding the allocation is valid when the eventual
  effective read is inside the decoded sprite allocation.

Example original EH7 decompressed sprite descriptors: ID315
has signed adjustments from **−36** through +1880; ID1541
includes **−44**. These are genuine original resource values.
Clamping or rejecting the intermediate ESI incorrectly drops
real sprites and is not source-equivalent.

Fixes:
`Recomp/src/original_code2_369d8_raster.hpp` preserves the
authentic signed intermediate ESI and only checks guest pointer
range; `Recomp/src/original_sintab_raster.hpp` checks each
generated `[ESI + disp32]` effective read against the genuine
sprite allocation. The interpreter still rejects invalid reads,
unsupported x86, invalid selectors and unsafe VGA writes, while
preserving staged/transactional rendering.

Regression `Recomp/src/test_original_code2_369d8_raster.cpp`
covers negative ESI adjustment and a valid effective read as well
as an intentionally out-of-range final read that MUST fail.

## Earlier CODE.2 32954 DDA index correction

The original byte sequence at `32BF8..32C3B` (left DDA) and
`32D09..32D25` (right DDA) builds a **cell index**, doubles it
into a 16-bit-cell-array byte offset, halves it for the cell-count
comparison, and doubles it again when addressing the
`198E0` word array. Canonical C++ had dropped the initial doubling
but retained the later halving, incorrectly visiting unrelated
half-indexed map cells. The numerical reconstruction now uses the
cell index directly and multiplies by two when addressing memory.
This is semantically equivalent to the retail DDA instruction path,
not a forced map-cell visibility change.

The original Level11 wall fixture's authentic new front-material
frequency is `48:1, 56:7, 59:2, 409:2`; the outdated synthetic
`48/53/59` fixture assertion was updated to the new exact source
histogram. No actual walls were invented/changed.

- [Cross-level genuine ray source, 3/3](https://github.com/Hyperbog/RLOFT/actions/runs/39205179902)
- [Linux C++ regression, 74/74](https://github.com/Hyperbog/RLOFT/actions/runs/39206621718)
- [Windows C++ regression, 74/74](https://github.com/Hyperbog/RLOFT/actions/runs/39206621665)
- [Level11 source RES4 world gate](https://github.com/Hyperbog/RLOFT/actions/runs/39206621741)
- [Saved Level5 world](https://github.com/Hyperbog/RLOFT/actions/runs/39206621712)
- [Full CODE1+CODE2 native link](https://github.com/Hyperbog/RLOFT/actions/runs/39206621682)

## Immediate next acceptance — owner's two-skeleton DOSBox save

The user's separate, newly created original DOSBox
`SAVE09(2).zip` carries `CHATGPT`, Level 5, 198°, and two
onscreen type-4 skeleton actors. Unmodified inner `SAVE.DAT`
SHA256:
`28a4b28473997d2682d0a4671e104b13fbad078eea49b0369205ed8ed3669542`.
Its **old native** probe reported 0 ray-visible actors because it
predated corrected DDA indexing. The new Windows engineering package
from run 39206621665, artifact
`RLOFT-Recomp-Windows-Milestone-152`, contains both
`rloft_original_code2_save_game.exe` and
`rloft_original_code2_world_frame.exe` for local private source
validation. Do **not** place user's private save bytes in a public CI
artifact, git commit, issue, or comment.

Run existing `run_chatgpt_save09_native_probe.ps1`
with the unchanged `SAVE09(2).zip`, owner original game directory
and these updated Windows executables. Compare actor indices
#38/#70, ray-visible counts, `36CEC` calls, indexed writes.
Separate genuinely reconstructed indexed pixels from independent
original DOS planar VGA + DAC RGB6 reference. Source pixel production
does NOT imply DOS parity.
