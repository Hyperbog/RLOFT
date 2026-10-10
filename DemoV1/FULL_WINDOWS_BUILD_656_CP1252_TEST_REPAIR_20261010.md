# Windows Full #656 — EH6 source builds; one CP1252 console-output test failure (2026-10-10)

**Authorized run:** [Windows Full #656](https://github.com/Hyperbog/RLOFT/actions/runs/38083481440), source checkout `72a9e7843dbf995b0fe98493077fd608fd4d8609`. **Result: failure after successful compilation.** No executable/artifact was published from this run; the workflow was restored to its original manual-only byte-identical blob and its temporary marker removed.

## Exact Windows CI result

- Canonical CODE.2 C++17 syntax preflight: **PASS**.
- CMake configure and **all C++ targets**: **PASS**, including new EH6 native loader and its synthetic C++ tests.
- CTest: **213 of 214 passed**; the only failing registration was `original_res4_selection_probe_tests` (Test #7).
- Failure at `DemoV1/tools/test_probe_original_res4_selection.py:133` was in the **last print statement**, *after* all decoder, selection, malformed-resource and CLI assertions completed:

```text
UnicodeEncodeError: 'charmap' codec can't encode character '\u2192'
```

The Windows Python 3.14 GitHub Actions runner defaults to **CP1252** stdout, which does not encode the Unicode arrow in `PASS: independent CODE.2 EH6 disk→runtime type1/LZSS oracle`. This is **not** a failed source-tile decode or a native C++ runtime error.

## Narrow repair and independent evidence

Source-only fix [`3dc37d0fbc3a2a97461139ad3d3c0fcfc60d9f52`](https://github.com/Hyperbog/RLOFT/commit/3dc37d0fbc3a2a97461139ad3d3c0fcfc60d9f52) replaces that one success diagnostic with ASCII `disk-to-runtime`. It changes no assertion, test fixture, game logic, pixel data, RNG or DOS behavior.

Separately rechecked local immutable original `CODE.2` against SHA-256 `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`: exact 9 instruction windows, all 7 original near-E8 targets and the EH6 jump-table selection into `2EEC5` **PASS**. Original supplied `LEV022.DAT` signed header+7 is **+55**; with the user's #655 original-media probe base **600**, the actual entry is **RES4[655]**, the reported on-disk EH6 2,515-byte record. Independent original-resource decode in [media proof](LEVEL22_RES4_EH6_ORIGINAL_MEDIA_DECODE_20261010.md) covers the 4,096 original palette-index bytes.

## Acceptance and subsequent retry

[Windows Full #657](https://github.com/Hyperbog/RLOFT/actions/runs/38084050726) is a **separately started full CMake/CTest retry** covering the ASCII-only test fix. #656 must not be labeled green, and #657 results must be recorded separately from this checkpoint.

Even a green #657 will prove native build and synthetic/immutable-original contracts, not active textured-ground raster in actual installed game, not original DOS indexed framebuffer parity and not full Level22 completion. The field test still requires `Diagnose-Level22.cmd` plus original ground probe against the same installed GOG media; trace should show `ground_original_res4_verified=1`, source entry **655**, and texture-path eligibility, followed by paired original DOS pixels/ticks. The game's source-data access and game completion percentages are unchanged.
