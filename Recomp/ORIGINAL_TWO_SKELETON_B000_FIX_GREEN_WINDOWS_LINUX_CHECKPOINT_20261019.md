# Part 23 — corrected B000 sprite regression, green Windows/Linux and exact owner frame boundary

Date: 2026-10-19. Branch: `Recomp`.
**Correction commit:** [`f628b6b553632e974e48cbe2ef75bf96fe7d5f76`](https://github.com/Hyperbog/RLOFT/commit/f628b6b553632e974e48cbe2ef75bf96fe7d5f76).
**Package/runner commit:** [`fbb3b866bbd5b1d0062da32a2f6e4f7dbae7f96f`](https://github.com/Hyperbog/RLOFT/commit/fbb3b866bbd5b1d0062da32a2f6e4f7dbae7f96f).

## Corrected source-specific assertion

In `Recomp/src/test_original_code2_369d8_raster.cpp`,
the test fixture sets a synthetic SINTAB stop slot at
`code+0x210`. `execute_original_369d8` patches this temporary
stop instruction to `RET`. The initial B0000-shadow store is executed
in both VGA plane-mask passes, while the following +80-byte store is
stopped before execution. Correct fixture assertion:

```cpp
assert(guest[0xB0020u]==40u && guest[0xB0070u]==0u);
```

No original game binary, resource, save or actor position was edited.

## Validated regression results

- [Linux regression `39213486049`](https://github.com/Hyperbog/RLOFT/actions/runs/39213486049)
  at `f628b6b`: **passed, 74/74 tests** including
  `canonical_code2_original_369d8_res3_vga_plane_raster`.
- [Windows milestone `39213588061`](https://github.com/Hyperbog/RLOFT/actions/runs/39213588061)
  at `fbb3b86`: **passed, 74/74 MSVC tests** including the exact
  B0000-shadow regression; packaging and artifact upload passed.
  `RLOFT-Recomp-Windows-Milestone-165`, artifact
  `12168597844`, SHA256 digest of GitHub artifact archive
  `a5ab11ab090ae84c5e943064e7b88a32dcbb8bca986d41c7b3c6283fb9eb5841`.
- [Canonical CODE1/CODE2 source-link gate `39213486014`](https://github.com/Hyperbog/RLOFT/actions/runs/39213486014):
  **passed** at `f628b6b`.
- [Authenticated archived SAVE09 full-frame integration `39212657036`](https://github.com/Hyperbog/RLOFT/actions/runs/39212657036):
  **passed** at `5072da2`, same renderer source with the new
  off-aperture handling; archived SAVE09 is **not** private
  `SAVE09(2).zip`. The intervening latest commits corrected the
  synthetic assertion and packaging, not the actual renderer source.

Windows milestone #165 package includes, together,
`rloft_original_code2_world_frame.exe`,
`run_two_skeleton_complete_frame.ps1` and
`compare_dos_indexed_frames.py`. This prevents using the user's
old PowerShell script at `Downloads\run_two_skeleton_complete_frame.ps1`
which previously reported a terminating native stderr error.

## Owner's exact two-skeleton frame still requires owner-local run

The supplied private `native_source_full_frame.log` previously
authenticated level 5, heading 198 degrees, two matching skeleton
projection callbacks, a source `36CEC` sprite resource 132 and a
real generated x86 `MOV [EDI+disp32],AL` at `0xB0032`.
At that time, the old strict 64K VGA shadow boundary rejected this
CPU store. The corrected host now keeps bounded A0000-BFFFF CPU
writes transactional without wrapping B0032 to A0032. Off-aperture
write counts are reported separately.

**This is not a passed private-save frame.** The privately preserved
`SAVE09(2).zip` is not present in the current tool filesystem.
The uploaded local `SAVE.DAT` belongs to **level 22**, has SHA256
`f898cb92fca6447954f5e0b2bf6f8ad2435e4a64ae3b9de74f8bf75e0ed10d5d`,
and cannot replace the private two-skeleton Level 5 save SHA256
`28a4b28473997d2682d0a4671e104b13fbad078eea49b0369205ed8ed3669542`.
No native two-skeleton full-frame `.bmp`, `.idx` or `.pal` from
Build #165 has been supplied.

Next owner-local validation:
1. Unpack Windows milestone #165; use its packaged script and EXE.
2. Invoke `run_two_skeleton_complete_frame.ps1` with unchanged private
   `SAVE09(2).zip`, original RLOFT data, and a new empty output dir.
3. If successful, upload `native_source_full_frame.log`,
   `private_validation_receipt.txt`, and (if visual check is wanted)
   `two_skeleton_complete_native.bmp`. For a raw pixel validation,
   the associated `.idx` and `.pal` must also be supplied.
4. For original retail DOS equivalence separately supply independently
   sourced exact DOS indexed frame and RGB6 palette captures.

**Acceptance:** native compiled regression green. Exact two-skeleton
composite and independent original DOS VGA/DAC parity unverified;
do not update formal R08/R09/R12 acceptance flags.
