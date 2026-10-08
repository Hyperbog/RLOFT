# Level 22 native frame and wide-run capture — 2026-10-08

**Branch:** `demo-v1` · **Scope:** opt-in diagnostic instrumentation.
No original DOS behavior is claimed by this capture. No GitHub Actions
build has yet verified the changes at this checkpoint.

## Usage

After manually running the Windows Demo v1 workflow on `demo-v1` and
extracting the complete Windows artifact, double-click
`Capture-Level22.cmd` alongside `ravenloft_demo_v1.exe`.
Alternatively, launch:

```bat
ravenloft_demo_v1.exe --explore-level22 --capture-render
```

Original assets are located through the existing installed-game
discovery flow. Each run creates one directory under:

`%TEMP%\RLOFT-Demo-Captures\<session-timestamp>\`

The preview title shows `Captures N/16` and `Capture error` if the
optional export fails. Normal startup and `Explore-Level22.cmd` do not
write captures. The new launcher is included in the workflow artifact
without changing the manual-only dispatch policy.

## Captured evidence and format

For each *changed* (level, integer cell X, integer cell Y, facing),
the first 16 distinct sequential observations save:

- `frame_0001.idx` — **128,000 raw bytes** in top-to-bottom
  row-major 320×400, 8-bit palette-index order. This is the exact
  native `ExplorationPresenter` output before transient hover/menu
  chrome; it is **not** a DOS framebuffer image.
- `frame_0001.pal` — **768 raw bytes**, index-ordered 256 RGB
  triplets from the native palette's 6-bit DAC representation.
- `frame_0001.tsv` — versioned capture metadata, level, cell,
  direction, viewport VGA page, main/prepass rendered/pending/failed
  counts; every original coalesced wall run's pass, first ray,
  `C6F4` width, phase, `((ray & 3)<<3)+width` index,
  depth, RES4 resource/column, shade decrement, and recorded
  raster failures. The three mask bytes are read from the
  **immutable shipped CODE.2 object-2 file image** using this
  index, and explicitly labelled as *not* live DOS memory.

Failure reasons are flattened to single TSV cells for later triage.
Pixel data and palette are binary, lossless and independent of
Windows DPI or screenshot color conversion.

## Controlling original-code evidence

The supplied `CODE.2` SHA-256 is
`853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
The original `337AD` increments `DWORD[C6F4]` without a seven-column
clamp; `3271C..3272D` computes the raw mask index, and original
`3285E/32902/32929` read `C668/C688/C6A8 + index` directly.
Those reads can reach adjacent DGROUP data for sufficiently wide
runs. **A captured static mask byte is not the same as the original
live mutable byte.** No DOS runtime memory snapshot exists here.

## Test and acceptance boundary

New assertion-enabled `exploration_frame_capture_tests` covers
lossless pixel/palette bytes, bounded output, long-run mask indices,
provenance labelling, structured failure diagnostics and invalid
CODE.2 rejection. The workflow stages the launcher in the Windows
artifact alongside the EXE. The new test has not been observed
passing on a current-HEAD Windows runner.

Follow-up: run manual Windows CI; collect a capture of a visually
incomplete Level 22 view; separately collect matching **real DOS**
frame/palette and `C6F4`/mask memory traces in an emulator. Only
then compare indexed pixels and infer live wide-run behavior.
Never fabricate DOS reference frames. Formal demo acceptance remains
**1 of 5 gates (20%)**.
