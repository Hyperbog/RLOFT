# Part 23 — owner two-skeleton complete-frame rejection and isolated differential

**2026-10-19. Branch:** `Recomp`. **Do not claim the user's full
two-skeleton frame or independent DOS pixel parity accepted.**

## Original private run

The owner executed `run_two_skeleton_complete_frame.ps1` using the
untouched `SAVE09(2).zip`, authenticated game data and Windows
Build #153. It stopped inside the full-frame source sprite path:

```
CODE2_SOURCE_36CEC_RASTER_FAILURE: 369D8 unsupported SINTAB opcode
CODE2_SOURCE_36CEC_RASTER_FAILURE: 36CEC unloaded RES3 body
CODE2_AUTHENTIC_WORLD_RES4_FRAME_FAIL: CODE2 graphics: !failure && !gfx.stream_error && !gfx.unsupported
```

Windows PowerShell 5.1 with `$ErrorActionPreference="Stop"` also turned
stderr from the native executable into a terminating
`NativeCommandError`, preventing the original runner from printing its
intended private full diagnostic receipt.

The earlier isolated owner sprite probe with this exact save had
verified original Level 5 heading 198 degrees, type-4 actors #38/#70,
two source `36CEC` calls and 5,515 `369D8` indexed pixel writes.
The new full-frame failure does **not** revoke that earlier isolated
result; it identifies an additional composition boundary.

## Source-verified diagnostic correction

Original `original_sintab_raster.hpp` returned `ok=false` on
*multiple distinct conditions*: unsupported instruction, source read
outside decoded EH7 allocation, invalid generated-code entry/jump,
truncated instruction, VGA write outside its aperture, and budget
exhaustion. The outer `execute_original_369d8` mapped all of these
into the *same* misleading `unsupported SINTAB opcode` message.

Changes on Recomp:
- `a20bf05`: classify every bounded SINTAB interpreter rejection
  while recording failing instruction offset, opcode, ModR/M and
  effective address; preserve fail-closed checks.
- `579c09e`: propagate those scalar diagnostics to `36CEC` instead
  of replacing the error with an unsupported-opcode guess.
- `de616e3`: include sprite RES3 ID/projection and report whether
  any generated SINTAB code bytes differed from the original source
  after the wall pass, **without restoring or modifying** those bytes.
  Avoid the false second `unloaded RES3 body` failure after earlier
  `s.failure`.
- `32532c8`: handle Windows PowerShell 5.1's native stderr without
  discarding the return status or owner-local diagnostic log.
- `2664ee4`: regression-test distinct out-of-range source reads and
  unknown generated SINTAB opcodes.
- `4672c63`: after a failed composed frame, automatically re-run the
  **same unchanged save** via the isolated sprite command and write
  `native_isolated_sprite_check.log` and
  `native_failure_differential.txt`.

### CI state and constraints

[Windows build #157](https://github.com/Hyperbog/RLOFT/actions/runs/39210891381)
passed MSVC construction and CTests at exact source
`2664ee49bab9c7be45f35bd502741a52dc976b8d`. Download
`RLOFT-Recomp-Windows-Milestone-157`; its latest source executable
includes the diagnostic code. The PowerShell script is pulled directly
from current `Recomp` head (`4672c63`).

Other Linux regression/source-link gates on the diagnostic change
passed. Archive-based Level 5 full-frame CI remained green; it is a
*different saved camera* and cannot certify private `SAVE09(2).zip`.

The post-world SINTAB code-byte comparison observed
`changed_code_bytes=0` on the archive CI fixtures; do not conclude it
is zero for the owner's private save until re-run.

### Required next private evidence

Rerun **new Build #157 and current PowerShell runner** with the exact
same `SAVE09(2).zip` and original game assets, with a *new* empty
output directory. Upload only:
`native_source_full_frame.log`,
`native_isolated_sprite_check.log`, and
`native_failure_differential.txt`.
These files contain execution counters and failure addresses, not
proprietary resource payloads. Keep raw saves/BMPs owner-local.

**Root cause of the composed-frame SINTAB rejection remains
undetermined until exact execution-failure classification is observed
on the private save.** Do not introduce opcode support, artificial ray
flags, rewritten sprite bytes or SINTAB resets to force a frame.

No independent DOSBox VGA/CRTC/DAC reference exists yet; DOS/native
pixel parity remains unverified. Formal R08/R09/R12 gates remain open.
