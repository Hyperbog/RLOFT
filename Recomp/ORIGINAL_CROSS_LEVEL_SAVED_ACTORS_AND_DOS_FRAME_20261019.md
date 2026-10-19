# Part 25 — genuine cross-level saved-camera and DOS VGA validation

**Branch:** `Recomp`. Work is source-validated but **DOS pixel parity remains open**.
Retail CODE.2 SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.

## What was reconstructed and executed

- `Recomp/src/code2_original_save_game.cpp` now accepts the matching unmodified
  `LEVnnn.DAT` named by each source `SAVE.DAT`, while retaining the
  compiled original `07938 -> 181B4` parser and strict checks that the
  source guest state matches the serialized camera and tile grid.
- New `--source-ray-census <private-save-directory> <original-CODE.2>`
  executes original `334C4 -> 32954` and records the original primary actor
  cells marked with ray flag `0x0040`; **no actors, ray flags or positions
  are moved or invented**.
- `.github/workflows/recomp-genuine-camera-census.yml` checks genuine
  media on the read-only `DnD-Classics` branch. The workflow copies
  original files to a temporary private directory and publishes numeric
  diagnostics only, not source saves or generated images.
- `Recomp/src/code2_original_world_frame.cpp` gained an isolated
  `--save-sprite-probe` that runs the original saved camera through
  `07938 -> 181B4 -> 334C4 -> 32954`, initializes the authentic
  `INIT/NPCS.DAT/OBJECTS.DAT` definitions and requests the original
  `2C294 -> 36CEC` scene path. It renders on a blank VGA framebuffer
  to isolate sprite output; **this is NOT the complete world frame**.

## Measured unmodified cameras

| Read-only source | Level | Facing | Primary actor records | Cells marked ray-visible | Actual original `36CEC` calls |
|---|---:|---:|---:|---:|---:|
| `cloud_saves/RLOFT/SAVE01` | 23 | 50° | 34 | **3** | **0** |
| `cloud_saves/RLOFT/SAVE02` | 21 | 216° | 2 | 0 | not run in sprite probe |
| `RLOFT/SAVE01` | 22 | 270° | unresolved | unresolved | unresolved |
| `cloud_saves/RLOFT/SAVE10` | 5 | 252° | 90 | 0 | 0 |

Level 23's three ray-marked actor records also pass the
original `2B6D0` flag-`0x02` exclusion test, verified against
authenticated original CODE.2 disassembly at `2B703` and `2B763`.
Nevertheless, the compiled source `2C294` produced
`projected_res3_ids=0 emitted_calls=0 369d8_indexed_writes=0`.
This is **not** evidence of successful genuine sprite rasterization.
Additional projection/scene-record conditions must be understood from
source data before an actual `36CEC` invocation can be certified.

Level 22 source save `RLOFT/SAVE01` exits with status 139 (segfault) in
the cross-level source probe. The first workflow failed the full
three-camera gate; a second bounded diagnostic run also failed, because
`gdb` was unavailable on the CI worker.
**Do not mark this source level tested, or suppress its CI failure.**
Next debug pass should use a deliberate ASan/UBSan or gdb-enabled
worker and report an exact crash location without original bytes.

## CI evidence

- [Genuine cross-level source census](https://github.com/Hyperbog/RLOFT/actions/runs/39192361976):
  Level 23 source visible=3; Level 21 visible=0; Level 22 segfault.
  Overall workflow **failed** by design.
- [Second census with crash diagnostic](https://github.com/Hyperbog/RLOFT/actions/runs/39193219763):
  same source results; `gdb_missing`, overall **failed**.
- [Isolated Level 23 source sprite probe](https://github.com/Hyperbog/RLOFT/actions/runs/39193057363):
  command completed, visible=3, initial flag-eligible=3,
  original source draw calls=0, sprite pixels=0.
- [Windows MSVC](https://github.com/Hyperbog/RLOFT/actions/runs/39193057400):
  **74/74 CTests passed**.
- [Linux GCC](https://github.com/Hyperbog/RLOFT/actions/runs/39193057390):
  **74/74 CTests passed**.
- [Canonical source-link](https://github.com/Hyperbog/RLOFT/actions/runs/39193057403):
  passed.

## Original DOS validation: blocked on independent observations

No independently run retail DOS frame or palette has been supplied
or produced. No DOS emulator capable of running the original protected-
mode game is installed in the current local container. Reconstructed
`.idx`, `.pal`, native BMP previews and synthetic CTests are **not**
original DOS capture evidence.

A valid comparison requires a recorded original game/launcher version,
identical save/camera, heading, RNG/time animation phase, CRTC page/start
and presentation geometry, four physical VGA planes or independently
decoded row-major **320x400 = 128,000** source-index bytes, and
**256x3 = 768** original DAC RGB6 bytes. Capture them under an
independent DOS execution on a private emulator/debugger or real DOS.
Then run the existing strict tool, for example:

```sh
python Recomp/tools/compare_dos_indexed_frames.py \
  --dos-index /private/original.idx \
  --native-index /private/reconstructed.idx \
  --dos-palette /private/original.pal \
  --native-palette /private/reconstructed.pal \
  --report /private/comparison.json
```

An indexed/DAC byte match still requires independently attested
reference provenance and the same actual source camera/frame.
The current checker explicitly labels provenance as unverified.

## Next verified boundary

1. Diagnose the original `2B6D0` scene projection chain for Level 23's
   three ray-marked eligible primary actors; do not manufacture draws.
2. Resolve the separate Level 22 source segfault with instrumented CI.
3. Obtain an authentic `2C294 -> 36CEC` draw from a genuine camera,
   validate indexed output, then inspect an actual complete native BMP.
4. Acquire independent original DOS VGA/DAC reference and compare the
   same scene/presentation. **Full frame parity and continuous DOS boot
   are not yet accepted.**
