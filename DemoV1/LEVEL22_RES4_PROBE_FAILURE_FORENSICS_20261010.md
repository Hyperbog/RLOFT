# Level22 RES4 failure: preserve original-media evidence before decode — 2026-10-10

**Branch:** `demo-v1` · **Workstream B:** faithful native reimplementation. **Classification:** proven diagnostic defect corrected; gameplay renderer unchanged. Windows and DOS fidelity are not accepted at this checkpoint.

## Field evidence and root diagnostic defect

The user-supplied F12 field session for authorized [Windows Full #654](https://github.com/Hyperbog/RLOFT/actions/runs/38079988991) produced twelve separate renderer captures whose common RES4 error was **`RES entry is not an EH type-1 raster`**, with `ground_original_res4_verified=0` and the native lower surface in the flat-fill fallback. This does **not** prove whether the wrong index, bank, original-media variant, malformed resource or loader decoding caused the mismatch.

A source review found a separate deterministic problem in `DemoV1/tools/rloft_res4_ground_probe.cpp`: it called `OriginalGroundRes4Audit::resolve` **before** `inspect`. When the original source-selected archive entry was not EH-1, `resolve` threw, and the probe exited before printing archive entry counts and other eligible 64×64 candidates. Thus the existing `Probe-Level22-Ground.cmd` could not deliver the evidence needed for this exact real-world failure.

## Evidence-backed correction

The source-only `OriginalGroundRes4ProbeSnapshot::capture` now:

1. Enumerates genuine original RES4 candidate metadata **before** attempting the selected decode.
2. Reads only the source-proved absolute ID `u16(INIT+0x0E) + s16(original_LevelState.header+7)` and retrieves the exact raw selected entry before calling the unchanged strict EH type-1 decoder.
3. Records original RES4 entry count, type-1 count, malformed count, eligible 64×64 candidates, original `INIT` base, signed relative level selector and selected absolute ID.
4. Captures the selected entry's **first 13 bytes in hexadecimal** and raw byte count. These bytes are diagnostic header metadata, not a copied graphic.
5. Writes **the full report even when decoding fails**; the command still returns nonzero so automation cannot mistake invalid original media for validated forest-ground artwork.
6. Reports each eligible candidate's unverified hypothetical base (`absolute - signed_selector`) separately from the actual `INIT` base. No visual similarity heuristic or replacement texture is allowed.

The revised Windows `rloft_res4_ground_probe.cpp` prints the snapshot first and only then reports a nonzero failed selection. Its unit regression extends `original_ground_res4_audit_tests` with four independent synthetic cases: a selected EH-7 entry plus otherwise valid alternative EH-1, a valid selected EH-1 64×64, an out-of-range absolute ID, and a truncated `INIT`. Every failed selection must still retain the candidate report and reason.

## Separate no-build executable-media check

To avoid requiring a fresh Windows build merely to collect source-media facts, `DemoV1/tools/probe_original_res4_selection.py` can inspect the user's original files directly with **Python 3**:

```sh
python DemoV1/tools/probe_original_res4_selection.py \
  --init "/path/to/RLOFT/INIT" \
  --res4 "/path/to/RLOFT/RES4" \
  --lev022 "/path/to/RLOFT/LEV022.DAT" \
  --json
```

When original `LEV022.DAT` is not present, pass `--selector 55` explicitly *only for the original fresh Level22 header*, rather than assuming `RES4[55]` is absolute. The script independently validates the RES index offset table, selects exactly `base + selector`, classifies raw EH resource types and original 64×64 descriptor candidates, and outputs SHA256 file fingerprints, first 13 selected header bytes and all candidate indexes. It never executes any original DOS binary, decodes picture pixels, writes original source files, downloads copyrighted media, or claims original-game raster parity.

`test_probe_original_res4_selection.py` adds a Python synthetic CLI+model regression registered in CMake as `original_res4_selection_probe_tests`. The catalog becomes **213 registered CTests with Python**, versus 212 before this batch. Neither the new C++ nor the new Python registration has run in authorized Windows CI yet.

## Independent immutable-source observations

Original supplied `CODE.2`, 558,689 bytes, SHA-256 `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`: direct object-1 bytes at `16E55..16E90` read `INIT+0x0E` into `258F8`; `1F388..1F398` adds signed `2561F` to the base and invokes bank 4; `2F29C` compares the loaded resource's type against **1**. The separately supplied original `LEV022.DAT` has signed header+7 = **+55** (physical file `0x1D+7`). These are actual source-code and source-level identities, not a guess at which of RES4's unprovided archive entries contains the floor.

The current session has valid original `CODE.2` and `LEV022.DAT`, but **does not have original `INIT` or `RES4` file bytes** mounted; the GitHub connector's UTF-8-only binary fetch cannot retrieve these resources despite their presence on `DnD-Classics`. Consequently the real source-selected absolute entry and its type are still unverified. Do not report the original RES4 bug itself as fixed.

## Acceptance boundaries and next steps

- Run the new no-build Python script on the **same installed GOG media as the failing Full #654 session**, or provide copies of just `INIT` and `RES4` for direct inspection. Compare the actual archive type distribution, selected raw header and numeric index with native field telemetry.
- If the selected entry is not EH-1, independently audit original CODE.2 loader state and bank selection against that media before changing game behavior.
- The corrected C++ probe needs a **separately authorized** Windows build before it appears in a new downloadable executable; previously built #654 artifacts contain the old premature-failing probe.
- Compare native indexed VGA frames, original DOS framebuffer, sprites and original IRQ/RNG timing after exact resource selection is resolved. The source authored CTests are **not** DOS acceptance.
- Manual Windows workflow remains unchanged; **no CI run was launched**. No Level22 acceptance percentages, overall completeness estimate, or standalone Level23 progress is increased.
