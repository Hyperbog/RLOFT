# Second Level 22 user capture — eastbound v2 diagnostics and uncapped crash sidecar

**Date:** 2026-10-08 EDT (upload after build #639). **Branch:**
`demo-v1`. **Status:** uploaded native-frame evidence independently
inspected; crash-forensic source and regression committed;
post-change Windows CI **not yet run**. GitHub Actions is manual-only.

## User archive identity and provenance

- Uploaded file: `1791510540377.zip`.
- SHA-256: `51492c25792af6e1bd7a4af4761e029e2fc9313b1f279108800784f00a846a9c`.
- Contains **16** complete `rloft-native-indexed-capture-v2` frames;
  each has 128,000 raw indexed bytes, a 768-byte raw VGA-6-bit palette,
  and a TSV. All indexed frames differ; all palettes match byte-for-byte.
- Source camera path: level 22, heading **270**, cell **(4,4)**
  to cell **(19,4)**, moving across successive X cells. Last
  captured source coordinate `A=1263210`, `B=294912`.
- All 16 TSVs have `scene_individual_columns_mode=1`
  (the original CODE.2 C029 individual-column mode).
- This archive has **no source-revision field** because it was
  generated from the prior capture writer, and no
  `failure.tsv`. The most recent GitHub Actions Windows
  run was still **#639** at
  `1561d634859e2afaf948e97f672a15a6d06203f3`,
  before the committed ray-cell indexing correction.

**Critical limitation:** these are the first 16 **successful**
changed-pose frames. The capture budget is 16; the camera/ray
at a later thrown exception is NOT recorded. Do not claim that
the error occurred at (19,4), only that (19,4) is the last
successfully captured pose.

## Cross-frame findings

- **4,080** logged main wall coalesced runs, all original non-drawable
  resource ID **147 = 0x93**. No ordinary/main wall was rasterized
  and none failed in these original outdoor open-field views.
- **1,157** successfully rasterized native RES3 scene calls;
  **369** fully occluded source calls. Every captured frame
  reports **zero** RES3 planning, trace, or raster errors.
- **One catalog type-9 NPC** appears in the source scene
  record stream in some views; original placed-object catalog
  types **62,63,64** also appear. This proves *native projection
  and dispatch*, not authentic DOS NPC/placed-object pixel parity.
- Visible RES3 source run width distributions are
  **18,082 width-1**, **928 width-2**, **202 width-3**
  and **34 width-4**. **No width >= 8** occurs in this archive;
  it does not exercise the new wide C029 loop.
- 16 unique native indexed images were captured, with changed
  pixel counts between successive frames ranging from
  17,508 to 41,077 of 128,000 positions; the sprites
  are responsive to movement. No original DOS reference
  images, matching animation tick, or live DGROUP bytes
  were supplied.

## Why the repeated exception could not yet certify the fix

The source fix for `ViewSweepTraversalModel::trace_ray` removes
the extra left/right index halving; see
[original-code analysis](LEVEL22_SWEEP_CELL_INDEX_CORRECTION_20261008.md).
A new `ravenloft_demo_v1.exe` **built after** that change is
required to assess it. Build #639 compiled the old binary.
Neither a different ZIP filename nor continued playtesting of
that old executable can demonstrate whether the source fix
eliminates the sweep guard.

To make *future* reports self-describing, the Windows
CMake/workflow now embeds checked-out `github.sha` in both
the preview title and exception dialog, and adds
`build-source-sha.txt` to the artifact. This checkpoint
adds the same `source_revision` to every TSV frame row.

## New opt-in fatal-frame capture

Previously the native `--capture-render` mode stopped recording
after 16 distinct poses, and a thrown exception during
`runtime.build_frame()` left no failing frame/pose in the ZIP.
This produced a misleading last-successful-pose ambiguity.

Now a thrown `std::exception` from `build_frame()` writes
`failure.tsv` to **the same session directory**, even
after all 16 image captures have been used. It records:

- Native-only provenance + `source_revision`.
- Original runtime level ID and full raw 16.16 A/B camera
  coordinates; derived integer cell, facing, level dimensions.
- Number of successful image snapshots written before failure.
- Exact thrown exception text, including the source-backed
  ray/state/candidate-cell values introduced in the previous
  sweep diagnosis. Tabs/newlines are escaped into single-line
  metadata cells.

It does **not** draw a fictional frame, alter original ray
traversal, modify GAME data, suppress the exception,
or write an automatic normal-gameplay log. If disk
capture itself fails, the native error is still rethrown.

The existing `exploration_frame_capture_tests` regression
now checks frame `source_revision`, explicitly stamped
revision on the second frame, separately written
16-frame-budget fatal metadata, exact camera identity,
escape-safe error text, absence of `frame_0017.idx`,
and Win32-safe file-stream cleanup.

## Manual build and next evidence

The last completed Actions run observed when this analysis
began is #639 at pre-fix SHA. The current-HEAD changes
have **not yet compiled or passed 160/160 Windows CTests**.
Do not claim a green newly built EXE.

Start a **new manual** `demo-v1` build, Linux unchecked;
download the new artifact into a *different* folder.
Verify the executable title's first 12 revision digits
against the artifact's `build-source-sha.txt`.
If the sweep guard repeats, upload the fresh ZIP **including
`failure.tsv`** and the complete exception screenshot.
The failure sidecar identifies the exact ray and camera
that failed rather than stopping at the old 16-frame cap.

Formal demo acceptance remains **1/5 gates (20%)**.
