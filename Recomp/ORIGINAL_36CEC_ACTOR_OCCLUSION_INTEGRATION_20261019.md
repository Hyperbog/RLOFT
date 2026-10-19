# Part 24 — original CODE.2 RES3 source raster integration and real saved cameras

**Branch:** `Recomp` (capital R). **Retail byte authority:** owner-original CODE.2,
558,689 bytes, SHA-256
`853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.

## Integrated execution

Original `2C294` source rendering callback now invokes the bounded
`36CEC` traversal and `36B14 -> 369D8` renderer rather than stopping at
EH7 column preflight. The connected code:

1. Resolves actual RES3 archive slot 3 through compiled `2F29C`.
2. Uses source `18FE0` scale-program pointers, `258C4` lookup words,
   `C738` scale count, `C730` maximum lookup, `C6E0` row origin,
   `C029` individual-column mode, and `EF4D` VGA draw page.
3. Traverses the decoded EH7 source lists center-out as original
   `36EED..372C3` does. Horizontal widths derive from authentic
   generated SINTAB words, not arbitrary billboard sizing.
4. Clips projected horizontal runs against original `32954` ray depths
   in 256 source `0x1C`-byte records, matching the original
   first/last-ray branch logic and partial-run trimming.
5. Emulates original `36B9C/36BD8` boundary searches,
   `36C10/36C44` generated-code vertical patching, and
   `36C90/36CBC` restoration *privately*.
6. Calls the previously tested `369D8` raster executor for its
   authentic VGA plane masks, generated-code micro-operations and
   indexed destinations. The temporary four-plane VGA snapshot is
   heap-allocated to avoid the Windows 1 MiB thread-stack limit.
7. Commits VGA writes only after successful execution. This rollback
   property is a host safety measure, **not** evidence the DOS program
   offered transactional video writes.

Source `src/original_code2_36cec_scene.hpp` contains the bounded scene
execution. `src/code2_original_world_frame.cpp` wires it to compiled
original `2C294` and records source draw requests, visible runs and
actual indexed writes. The original SAVE10 baseline explicitly prints
`RES3_36CEC_SOURCE_DRAWS=0 RES3_369D8_INDEXED_WRITES=0` instead
of falsely describing the implemented module as unimplemented.

## Test evidence

`src/test_original_code2_369d8_raster.cpp` additionally exercises the
complete `36CEC -> 36B14 -> 369D8` path with clearly **synthetic**
source ray records: screen column 0 is depth-occluded and column 1
writes an authentic indexed VGA pixel using generated SINTAB x86.
It checks the stop-opcode restoration and depth-threshold equality.
These inputs are **not** a claimed original game actor or DOS frame.

At compiled SHA `7cdb692ed91c55951a184e19eadb0d44699ca8a7`:

- [Windows milestone #131](https://github.com/Hyperbog/RLOFT/actions/runs/39190675962):
  MSVC succeeded and **74/74 CTests passed**.
- [Windows original-source link](https://github.com/Hyperbog/RLOFT/actions/runs/39190675922):
  passed.
- [Linux recompilation](https://github.com/Hyperbog/RLOFT/actions/runs/39190675937):
  **74/74 CTests passed**.
- [Owner-original SAVE10 scene](https://github.com/Hyperbog/RLOFT/actions/runs/39190675889):
  passed; baseline original-indexed frame SHA256 remained
  `d4831707916391c12290afc488318e65b9325d6e178ebb35fce4c794e9cbb8b7`.
- The previous MSVC run of the newly expanded test was a **stack overflow**,
  not a semantic raster assert; moving nested VGA snapshots to the heap
  resolved it. Preserve this failure/correction distinction in history.

## Genuine saved-camera findings

Each snapshot came from the unchanged original
`DnD-Classics` cloud save and its matching Level 5 file. Original
compiled `07938/181B4/334C4/32954` produced these actor-ray results:

| Save | Heading | Source actor cells visible / 90 | Rendering |
| --- | ---: | ---: | --- |
| SAVE10 | 252° | 0 | Complete saved-world frame |
| SAVE03 | 15° | 0 | Complete saved-world frame |
| SAVE04 | 190° | 0 | Ray pass; historical zero-quad-scanline assertion still blocks complete frame |
| SAVE08 | 181° | 0 | Complete saved-world frame |
| SAVE09 | 187° | 0 | Complete saved-world frame |

The hardcoded SAVE10 header length, pose, light and transient-record
counts were removed from the source-verification harness. Genuine
9,310-byte saves and mutable source counts are admitted while source
`07938` remains the authoritative parser. Captured alternate-save
failures now log only reason strings, and raw originals/frames are deleted.

**No ray-visible original actor has been identified in those five saved
poses.** Therefore there is still no genuine full-scene sprite pixel
evidence, no verified native sprite occlusion on a real camera, no
complete actor-containing BMP accepted and no independent DOS VGA/DAC
pixel parity. The original 320×400 indexed output and 256 RGB6 values
must be captured separately from original DOS execution before claiming
pixel-exact equivalence.

## Next bounded engineering step

Generalize read-only original `07938 + 181B4 + 32954` camera probes
beyond saved Level 5, using each untouched source `SAVE.DAT` and matching
`LEVnnn.DAT`. Locate a source ray-visible actor without inserting or
moving an actor and without editing rays or saved camera fields. Then
execute original `2C294/36CEC`, verify authentic RES3 writes and occlusion
on the genuine scene, inspect its complete private BMP, and compare raw
320×400 VGA indices and DAC6 palette against independently captured DOS.

**Explicit outstanding:** real visible actor, original-DOS VGA trace,
source-execution sprite pixel acceptance, and full DOS/game boot.
