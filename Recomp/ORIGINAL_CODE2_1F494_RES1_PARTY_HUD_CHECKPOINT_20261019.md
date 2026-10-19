# Part 23 — Original CODE.2 1F494/1EA90 four-character party HUD source integration

**2026-10-19 · Owning branch `Recomp`.**
**Source-level implementation and authenticated archived-save execution: PASS.**
**Exact owner two-skeleton frame after adding RES1: NOT YET CAPTURED.**
**Independent DOS/VGA/CRTC/DAC pixel parity: NOT VERIFIED.**
Private game media, screenshots, SAVEs and palettes are never committed.

## Original retail evidence

The immutable 558,689-byte user-supplied `CODE.2` has SHA256
`853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
Source functions in `Code2/Code2.cpp` are keyed to object-1 binary
addresses, not interpreted from game-specific fabricated fixtures.

- `1F494..1F7B0`: all four character panel positions are
  `(slot%2)*53 + (slot/2)*211`, yielding columns 0,53,211,264. It
  reads character records at `218F0 + slot*0x248` and party
  dirty flags `CF5A+slot` set originally by `1F8FC`.
- RES1 portrait ID:
  `INIT/258E4 base (336) + sex*23 + signed face_index +
  (status_word&0x0100 ? 46 : 0)`. Original `1F494` invokes
  `298F0` with RES1 slot 1 and state LUT `CF70 + state*256`.
- Each source panel calls `1EA90` for hand sides 0 and 1 (eight
  hand widgets), reads equipment indices `2193C + slot*0x248`,
  OBJECTS `+0x2D` resource selector, mode/disable status and
  real game timer. Equipment uses RES1 slot 1; empty-hand graphic
  uses RES0 +0x2D in slot 0.
- `0EF14` computes the original HP numerator from
  `218FC+slot*0x248`, `21ACA`, `219F3`. `1F494`
  computes original 80-pixel-width, 20-pixel critical threshold
  and colors 0x43/0x57, dispatching original rectangular fill
  via callback `29B40`.
- `1F494` also invokes original `2E374` borders and original
  `22F40` state-specific masked graphic overlay (where active).
  Timed effect text and alternate `23024` group lists remain
  explicitly fail-closed until their font and group callbacks are wired.

**Important canonical source correction:** retail x86 at
`298F0..29922` accepts **six** cdecl inputs:
`frame,row,column,resource_id,archive_slot,LUT_address`.
Its first two pushes pass slot+resource to original `2F29C`;
the five values forwarded to `2A584` are
`frame,row,column,decoded_image,LUT_address`.
The previous C++ wrapper incorrectly had five source arguments,
conflating a pixel coordinate with resource ID. The canonical C++
signature is corrected and now has a media-free ABI regression test.

## Actual implemented source paths

- `Recomp/src/original_code2_1f494_bridge.inc` supplies the
  original compiled `1F494` `Application44Boundary` and
  `Application43Boundary` host callbacks. This includes actual
  source `1EA90`, `0EF14`, `22F40`, `24478`, `19CC4`,
  `2975C`, `298F0`, `2A584`, `2E374`, `2E510`,
  `2E4A4`, `29AA4`, and `2A258`.
- Masked sprite rendering executes **compiled original** `2A584`
  over original decoded image frames and 256-byte state LUT, using
  the original VGA sequencer map mask and four-plane host aperture.
  It is not hand-drawn substitute portraits.
- `0xCF60..0xD770` guest LUT memory, all four untouched character
  records, group modes and game timer are copied from the original
  saved/source game machine. `1F8FC`'s original all-party-dirty
  step is reproduced before invoking `1F494`.
- The archived world fixture uses original user-preserved `RES1`
  input from `DnD-Classics`; the owner-local runner also stages
  `RES1` read-only. It is a new extra argument for
  `--save-complete-frame`, **after the existing output stem**.
  The isolated sprite-only mode keeps its legacy signature.
- All original resource lookups go through the reconstructed
  `2F29C` cache/loader. The host switches RES0 and RES1 based on
  original slot and restores RES0 before the existing `1EA04` HUD
  overlays. No original game bytes are committed or regenerated.

## Authenticated archived Level 5 integration

[Archived saved-world #39219889619](https://github.com/Hyperbog/RLOFT/actions/runs/39219889619)
passed using original archived SAVE09, real original RES1 and 2D
source CODE.2 game assets. It emitted:

```
CODE2_ORIGINAL_1F494_1EA90_PARTY_HUD
  active_original_characters=4
  source_state_lut_nonzero=1015
  resource_calls=22
  masked_298f0_2a584=16
  direct_2975c=4
  fills_29aa4=16
  borders_2e374=1
  hand_slots_1ea90=8
  unsupported_text=0
  callback_failure=0
  DOS_PARITY=NOT_VERIFIED
```

Strict authenticated saved-world regression
[39220352735](https://github.com/Hyperbog/RLOFT/actions/runs/39220352735)
passed the entire locked callback fingerprint. The test prints only
counts, no raw source media or native pixel data.

The media-free CTest
`canonical_code2_original_298f0_six_argument_res1_masked_abi`
locks original resource ID+archive slot and separately observes the
frame, row, column, translated table pointer and unloaded-image skip.
[Windows `39220164213`](https://github.com/Hyperbog/RLOFT/actions/runs/39220164213)
passed all **75/75 CTests** including this new test.
[Linux `39220216846`](https://github.com/Hyperbog/RLOFT/actions/runs/39220216846)
and source link
[39220216888](https://github.com/Hyperbog/RLOFT/actions/runs/39220216888)
succeeded on the 75-test source revision.

## Exact owner private case / final acceptance

The user's untouched `SAVE09(2).zip` has independently verified
original SAVE.DAT SHA256
`28a4b28473997d2682d0a4671e104b13fbad078eea49b0369205ed8ed3669542`
and LEV005.DAT SHA256
`b058964ce39d931e89417ef7f36f93640e48c57cbfdc9d5b363457fba238a672`.

**No rendered native output on this newer RES1-enabled executable
has been supplied yet.** The earlier private two-skeleton success
at Milestone #171 proved the world and RES0 UI base, but not the new
per-character RES1 composition.

The updated PowerShell runner requires:
- four source-active original character records and populated authentic
  256-byte translation tables;
- at least eight genuine `298F0 -> 2A584` source masked blits;
- eight `1EA90` hand slots;
- zero unsupported font or group-list callback calls;
- the two previously independently visible original RES3 skeleton
  footprints and 400 original gameplay-frame base blits;
- exactly 128000 indexed frame bytes and 768 RGB6 palette bytes.

Following a successful owner run, inspect the BMP's four portrait
panels and hand equipment explicitly. Do not infer visual correctness
only from callback counts. Remaining missing paths: status-effect
number/font rendering (`2E6F0/30508` if activated), alternative
party group menu (`23024`), full CODE1 game startup, independent
DOSBox original VGA/CRTC/sprite clipping capture and matching 256-entry
RGB6 DAC. R08/R09/R12 full DOS parity gates remain **OPEN**.
