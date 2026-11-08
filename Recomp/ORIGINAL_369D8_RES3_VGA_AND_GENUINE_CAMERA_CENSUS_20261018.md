# Part 23 — original 369D8/36B14 RES3 raster and saved-camera census

Branch: `Recomp`. Original retail CODE.2 SHA-256:
`853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
All source addresses are original CODE.2 object-local logical offsets.

## Source-backed implementation

`src/original_code2_369d8_raster.hpp` reconstructs the standalone
`36B14 -> 369D8 -> 369C0` raster-execution stage. It uses the existing
`original_sintab_raster.hpp` generated-code interpreter and actual
`OriginalVgaHost`, including CRTC page-one VGA aperture writes.

Direct original-byte disassembly confirms:
- `36A5B..36A8E`: read six-byte EH7 descriptors; decode stop/entry
  selectors and signed source adjustment; temporarily patch stop opcode to
  `C3`, restoring it at `36B04`.
- `36A18..36A47`: select three mask bytes from the original
  `C668/C688/C6A8` tables using `(destination_x&3)*8 + run_width`.
  All 28 width-one-through-seven / phase combinations were independently
  compared with the owner-provided retail CODE.2 data and match.
- `36A91..36AFF`: execute one required and two conditional plane passes
  with destination page, 80-byte row stride, row base and signed column
  byte position.
- `36B1A..36B40`: grouped run; switch packed AL/AH shade lookup after
  execution. `36B42..36B95`: individual pixel mode, parity-select
  the shade lookup, then restore the original lookup.
- Genuine original RES3:1829 has a 4,219-byte decompressed EH7 body,
  43 display-list columns and 75 descriptors. One signed adjustment is
  `-2` relative to the decoded-body pointer, referring to the real
  original resource's two-byte length prefix. The executor accepts
  separate allocation-start and body pointers to preserve that behavior.

The new host execution stages guest-shadow/VGA writes and temporary code
patches before commit. Unsupported selectors, out-of-range source accesses,
invalid destination addresses and bad x86 instructions fail closed. This
transactionality is an intentional host-safety behavior, not a claim of
original DOS rollback.

`src/test_original_code2_369d8_raster.cpp` provides native regressions
for actual planar indexed output, page-one CRTC scanout, masks, grouped and
individual packed shade state, private RET restoration, and transactional
rejection. `CMakeLists.txt` registers
`canonical_code2_original_369d8_res3_vga_plane_raster`.

## Verification

- [Windows run #125](https://github.com/Hyperbog/RLOFT/actions/runs/39168863685):
  MSVC and **74/74 CTests passed**, including the new raster test.
- [Subsequent Windows run](https://github.com/Hyperbog/RLOFT/actions/runs/39169190995):
  passed with saved-camera check changes.
- [Linux C++](https://github.com/Hyperbog/RLOFT/actions/runs/39169190927),
  [source-link](https://github.com/Hyperbog/RLOFT/actions/runs/39169190966),
  [world](https://github.com/Hyperbog/RLOFT/actions/runs/39169190980) and
  [saved-Level-5 baseline](https://github.com/Hyperbog/RLOFT/actions/runs/39169190937):
  passed.
- The original `Recomp` branch rename broke six old lowercase `recomp`
  CI triggers; all six trigger/checkout cases have been repaired.
- No original commercial media, original game pixels, or DOS capture data
  were committed to the repository.

## Genuine saved-camera validation — explicitly incomplete

`src/code2_original_world_frame.cpp` no longer assumes that every saved
camera has SAVE10's heading=252 and light state. It cross-checks the actual
guest camera against the immutable serialized `SAVE.DAT` and reads the
original `32954` ray-visibility flag `0x0040` for each original primary
actor's cell. No ray flag or actor position is fabricated.

The preserved source SAVE10/Level 5 pose remains valid and has **zero
ray-visible primary actor cells out of 90**. Four additional untouched
Level 5 saved poses from `DnD-Classics` were examined by the read-only
workflow. They cannot yet establish visibility because the existing full
world harness still terminates before the new actor-visibility metric:

```text
SOURCE_CAMERA_RESULT save=03 status=1 visibility_unresolved emissions_unresolved
SOURCE_CAMERA_RESULT save=04 status=1 visibility_unresolved emissions_unresolved
SOURCE_CAMERA_RESULT save=08 status=1 visibility_unresolved emissions_unresolved
SOURCE_CAMERA_RESULT save=09 status=1 visibility_unresolved emissions_unresolved
```

The public workflow currently deletes its diagnostic `probe.log`
without printing the original `CODE2_AUTHENTIC_WORLD_RES4_FRAME_FAIL`
exception. This is a diagnostic-design limitation, **not** evidence that
those cameras have no visible actors. After two bounded camera-probe
iterations no further speculative retry was made.

## Next independently verifiable step

1. Expose *only* the original exception/failing assertion in the saved-camera
   probe's public log, retaining private raw media/frame cleanup; distinguish
   source-save deserialization errors from hardcoded SAVE10-only world-scene
   assertions.
2. Obtain at least one real `32954` ray-visible actor camera without
   modifying any original `SAVE.DAT`, `LEV005.DAT`, ray flags or actor
   coordinates. Confirm an actual `2C294 -> 36CEC` call.
3. Bind the still-missing `36CEC` center-out occlusion traversal and
   `36C10/36C44/36C90/36CBC` vertical generated-code setup to the
   new `369D8` raster executor. The new stage is **tested but not yet
   invoked by a source-visible scene**.
4. Privately inspect the complete indexed BMP and independently capture
   original DOS VGA planes, CRTC presentation state and all 256 DAC RGB6
   values. Only then assert indexed/DAC parity.

Original DOS parity, authentic ray-visible sprite pixels, the complete
original-game boot and R04 acceptance remain open.
