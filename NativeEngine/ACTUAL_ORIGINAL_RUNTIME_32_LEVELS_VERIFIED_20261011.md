# Native original world runtime — full 32-level compiled/source audit

**Date:** 2026-10-11 local (Linux CI logged 2026-10-12 UTC).

## Exact verified source revisions

- **NativeEngine compiled source**: \`20543d8fd3a6c61aad7c886d6d1699f1fbf07005\`.
- **Linux compiler / unit suite**: [GitHub Actions #38186427291](https://github.com/Hyperbog/RLOFT/actions/runs/38186427291), **17/17 CTests PASS**, includes new \`native_world_runtime_tests\` and all previously certified tests.
- **Independent research implementation**: commit \`36612940f4e8aa1650c9428c096be50907150ab7\` on \`code1-decompilation\`, with \`SharedDecomp/verify_native_world_runtime.py\`. [Research Actions #38186651456](https://github.com/Hyperbog/RLOFT/actions/runs/38186651456), **124/124 standard-library/source-only tests PASS** (37 research parser + 87 shared Decomp regression tests).

## Actual source input audit, beyond synthetic tests

The Linux runtime auditor from exactly native commit \`20543d8f\` was downloaded from GitHub's **source-only** build artifact and executed locally against the user-supplied 499,878-byte original \`LEVELS.DAT\`, SHA-256:

\`50d0b1d13f81733a1ac55296d00fcba9aef002202bc00341f1062c37fd017d91\`.

Only the authentic original level file was decoded. Empty local \`INIT\`, \`RES0\` and \`T\` placeholders satisfied \`NativeMedia::open\`'s folder recognition; **this was not a playable original installation**, and these placeholders were not considered original game data. No game bytes entered CI.

An independently written Python reader parsed all original 32 level records and derived the CODE.2 normal collision decision from their actual 16-bit words and 10-byte control records. It agreed with the **actual compiled C++ output**, without copying native counters into its expectations:

| Checked evidence | Independent original bytes | Compiled native |
|---|---:|---:|
| Maps read | 32 | 32 |
| Cell words | 115,584 | 115,584 |
| Original 10-byte controls | 1,659 | 1,659 (native world-audit crosscheck) |
| Kind-1 dynamic records | 324 | 324 |
| Special-center cells (0x0080) | 745 | 745 |
| Ordinary blocking cells | 26,857 | 26,857 |
| Special blocking cells | 744 | 744 |
| Passable under the normal collision gate | 87,983 | 87,983 |
| Baseline explored cells | 3 | 3 |
| Opaque authoring/legacy bit 0x2000 cells | 3 | 3 |

All 115,584 source cells were categorized without a missing or duplicated category:
\`26,857 + 744 + 87,983 = 115,584\`.

More than just matching counters, **both independently constructed source fingerprints matched**:

\`\`\`text
Original retained world-source fingerprint (Python, from raw LEVELS.DAT):
a8232782ff8654f9b6c6651300b3fbb2235e58d2fadf61fe2221e11dbbc8bdcd
Actual compiled NativeWorldRuntime::source_fingerprint():
a8232782ff8654f9b6c6651300b3fbb2235e58d2fadf61fe2221e11dbbc8bdcd
\`\`\`

The fingerprint includes all 32 levels' original cell words, 10-byte
controls with owning columns, 11-byte compact A/B/C family records, all
1,600 event slots, automap grids and annotation data. The original file
SHA provides a separate anchor to the exact supplied retail original.
Earlier byte-round-trip validation also passed **256/256** original-to-native
section fingerprints at native compiled commit \`4676ebf5\`.

## What source behavior is implemented

\`NativeWorldRuntime\` owns the immutable \`NativeWorldPayload\` and distinct
mutable cell words, automaps and 10-byte control records. It supports
32-level state residency, source-bound in-memory snapshot/restore,
CODE.2 instruction-derived collision query classes, byte-preserving
six-bit cell-code writes, OR/clear visibility/exploration bits, state
resource variant bits, and the source-proven +6/+7 dynamic kind-1
barrier animation tick. Debug-started transitions are explicitly **not**
a finished original player interaction path.

The output's \`scene_projection_verified=false\`,
\`event_vm_executed=false\`, \`original_dos_execution_verified=false\`,
and \`gameplay_parity_verified=false\` remain binding.

## Follow-on engineering and remaining blockers

The **read-only native world data → mutable world-state foundation** is
compiled, test-verified and audited against the user's actual level bytes.
Remaining game work:
- CODE.2-faithful player movement, footprints, entity occupancy and mode-specific collision;
- original line-of-sight/raycast traversal, scene-geometry and prepass ordering;
- SINTAB/EH7 scene-shape projection and rasterizer replacement with a safe
  native renderer (do **not** execute shipped x86);
- EH6 texture sampling, VGA DAC/palette selection, animation;
- original event VM, actor AI, items, audio, original LEVnnn/SAVE persistence;
- exact-current-source Windows/MSVC compilation, release artifact and
  user field gameplay/visual acceptance.

The Windows release workflow remains **manual-only** and has not been
triggered by this research. No original copyrighted payload bytes are
included in repository or compiled artifact.
