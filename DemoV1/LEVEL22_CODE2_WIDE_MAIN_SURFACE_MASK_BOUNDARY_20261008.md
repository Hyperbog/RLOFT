# CODE.2 335DC/336F7 / 3263C — unsplit eight-column surface boundary

**Date:** 2026-10-08 · **Branch:** `demo-v1` · **Scope:** original
main-pass FCD0=0 only, not a release acceptance milestone.

## Controlling sources

Immutable supplied original `CODE.2` is 558,689 bytes,
SHA-256 `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
Object-1 file base `0x3AF24`; object-2 DGROUP image base `0x75F24`.
Original `SINTAB` is 333,757 bytes,
SHA-256 `19b6bc15f1be616bb8ff49dc9029995a1a0413fae06addc9fd41ddef761d0e87`.

Independent original-byte disassembly:

- `3370B` initializes `DWORD[C6F4]=1`, `33715` initializes
  `DWORD[C6F8]=0` for the FCD0=0 ordinary all-record wall pass.
- `33754..33761` compares next/previous depth and source-column
  selectors. `33763` calls `3263C` on a change; `337AD`
  **increments C6F4 without a width=7 clamp or chunk split**.
  `337BF` dispatches the final coalesced run.
- `3271C..3272D` in `3263C` uses
  `((C6F8 & 3) << 3) + C6F4` as the direct lookup index.
  `3285E`, `32902`, `32929` read three distinct DGROUP
  table bytes at `C668 + index`, `C688 + index`,
  `C6A8 + index` without masking the index to 0..31.
- `328EE..3294A` calls the same SINTAB entry on each active
  VGA mask pass. The first call is unconditional; subsequent calls
  execute only for nonzero mask bytes.

## Source-backed bounded extension

The previous native renderer supported coalesced widths 1..7 only.
For **exact width 8**, source table indices and immutable bytes are:

| Phase | Index | Plane masks | Selected VGA pixel offsets |
| --- | ---: | --- | --- |
| 0 | 8 | `0F/0F/00` | `0..7` |
| 1 | 16 | `0E/0F/01` | `1..8` |
| 2 | 24 | `0C/0F/03` | `2..9` |
| 3 | 32 | `08/0F/FF` | `3..11` after high-nibble truncation |

Phases 0..2 select exactly eight pixels from three valid,
**source-authenticated**, 32-byte mask arrays. Phase 3 touches byte
`C6C8`, which lies *after* the third array, and on the original
initial data selects nine rather than eight physical pixels. That
is not a validated eight-column draw; it stays explicitly unsupported.

Widths greater than eight likewise must not be converted into made-up
seven-column chunks. The original code does not perform such a split,
and wider raw DGROUP reads can alias other state and exceed the known
mask-table domain. Recover actual run counts and runtime DGROUP captures
before enabling any additional lengths.

## Code and regression

- `Type7VgaPlaneMaskModel::for_surface_main_run`: isolated original
  wall-only bounded mask accessor. Ordinary sprite `for_run` retains
  its independent 1..7 contract.
- `SurfaceSintabMainExecutor`: validates width up to eight, uses
  the new accessor, and continues its transactional four-plane render.
- Both mask and main-wall CTests now check exact original phase masks,
  contiguous pixel placement for 8-wide phase 0/1/2, shade parity,
  source/non-source planes, and rejected phase-3/9-wide behavior.
- Eight-wide masks are verified on wall use, not during global
  mask-loader initialization; synthetic 1..7 sprite fixtures remain
  backward compatible.
- Level 22 `--explore-level22` preview title now reports
  `SINTAB walls rendered/total | Wall errors`, outside the
  original 320×400 indexed framebuffer. This diagnostic is not
  part of original DOS graphics and does not affect pixel fidelity.

## Acceptance and next step

This is **source committed, not yet freshly built**. It cannot
establish a complete wall scene, original DOS framebuffer parity,
accurate wide-run runtime memory, or passing MSVC CI. Current full
Windows baseline run #636 previously passed 157/157 at an earlier SHA;
the new test is part of an expected 158-test consolidated suite.

Next: manual build on `demo-v1`; inspect the preview caption
and full-resolution Level 22 comparison against original DOS, then
instrument original C6F4 and raw DGROUP mask reads for width 9+ and
phase-3 width 8. Formal game-demo acceptance remains **1/5 gates**.
