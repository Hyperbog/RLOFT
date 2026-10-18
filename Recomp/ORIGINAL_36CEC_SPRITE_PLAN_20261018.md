# Part 22 — original CODE.2 36CEC input and RES3 plan checkpoint

**Owning branch:** `recomp`. This is a bounded preparatory integration,
not successful sprite rasterization or DOS-frame parity.

## Original-byte audit

The original CODE.2 from the owner-supplied retail build is 558,689 bytes,
SHA256 `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
Original object-1 file offset `0x3AF24`; `36CEC` starts at file
offset `0x71C10`. Direct disassembly of `2C4A3..2C4E6` proves the
six cdecl arguments: screen X, **RES3 absolute ID**, projected extent,
packed lookup bias, vertical mode, and depth threshold. The old
`source_scene_sprite_emit` callback incorrectly named the screen X as
`frame` and checked it against RES3 archive count; this is corrected.

`36CEC..36DAD` computes projected scale, clamps to DGROUP C738 and
reads the 258C4 runtime lookup. `36E94` retrieves RES3 via original
`2F29C`; `36EED..372C3` walks the per-column display lists from
the center toward left and right. The C++ source preflight uses those
same source inputs; no made-up texture, pose, source visibility or
screen-space position is substituted.

## Committed implementation and validation

- `src/original_code2_36cec_columns.hpp`: typed six-argument source
  contract, SINTAB scale and 16-bit lookup behavior, bounded decoded
  EH7 geometry/list validation and original center-left/center-right
  traversal, including duplicate and odd list offsets.
- `src/code2_original_world_frame.cpp`: actual original 2C294
  `call_36cec` callback now resolves argument **two** as RES3 via
  original `2F29C` archive loading, reads the real DGROUP lookup,
  and records preflight column/descriptor counts **only if original
  ray-visible source scene invokes it**.
- `src/test_original_code2_36cec_columns.cpp`: media-free synthetic
  tests for cdecl identity, original scale-zero early return, truncation
  of 16-bit projected extent, index clamp, reverse/forward columns,
  shared odd list offsets, signed descriptor and malformed-data
  fail-closed behavior; registered as a CTest.

Local GCC C++17 `-Wall -Wextra -Wpedantic -Werror -O2` with UBSan
passed for the standalone test. A separate local authenticated
real-media decode of `RES3:1829` produced the expected 4,219-byte
EH7 body and the new native preflight accepted **43 columns, 75
descriptor invocations**. Original media are not committed.

## Explicit remaining blockers

- Original source 369D8/36B14 code patching, ray-depth clipping and
  SINTAB generated instructions still need to write the four authentic
  VGA planes. The new preflight `drawable` means structurally ready,
  **not** drawn pixels.
- The Part 21 SAVE10/Level 5 camera has zero ray-visible primary actor
  cells; it cannot validate sprite output. Other genuine saved poses
  exist on DnD-Classics but have not been shown ray-visible by original
  32954/2C294. Do not manufacture visibility.
- A full compiled Windows source/CTest run and any real-camera sprite
  callback observations are additional gates after this commit.
- Complete BMP visual inspection, independently captured original DOS
  VRAM/DAC data and exact indexed comparison remain outstanding.
