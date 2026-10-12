# Original-authored RES3 scene sources — compiled NativeEngine and independent 6,204-instance oracle

**Date:** 2026-10-11 Eastern / 2026-10-12 CI UTC.
**Status:** Verified **source-instance translation and definition-backed base IDs**, NOT EH7 output pixels, source subframe choice, original DOS raycasting or native playable-game parity.

## Exact compiled/research SHA and CI

| Check | Result |
|---|---|
| Actual compiled NativeEngine source | \`5bffd3a3e53a06842d27fbe5c63d574acbc9babd\` |
| [Native Linux source-only Actions #38191784044](https://github.com/Hyperbog/RLOFT/actions/runs/38191784044) | **20/20 CTests PASS** |
| Independent canonical SharedDecomp source | \`47cc267de4ca5ea250c8cc9082f1a0dc189811e8\` |
| [Research source-only Actions #38192139467](https://github.com/Hyperbog/RLOFT/actions/runs/38192139467) | **139/139 Python tests PASS** (39 ResearchInfra + 100 SharedDecomp) |
| Executable independent original scene-source audit | **32/32 per-map resolved SHA-256 digests MATCH** |
| Actual instance identities validated | **6,204/6,204** original compact source instances |
| Independent full source-base digest | \`aeb7e334ba23bc114d5444617451495c4bda238d772343f597a9e3b1755d348c\` |

The C++ source-only pipeline builds the **real typed EH7 scene-shape
decoder**, verifies original INIT source-class bank rules, resolves
NPCS/OBJECTS/INIT type-definition bases, and turns all three 11-byte
compact LEVELS families into original-source-authenticated scene
placements with 16.16 fixed-point positions. The renderer code is not
linked to the demo-v1 game runtime.

## Original binaries actually checked

The final *compiled* Linux binary \`rloft_native_scene_instances_audit\`
was downloaded from GitHub Actions and run **locally** with the following
original media, not uploaded to CI:

\`\`\`text
LEVELS.DAT 499878 bytes
  SHA256 50d0b1d13f81733a1ac55296d00fcba9aef002202bc00341f1062c37fd017d91
INIT 1879 bytes
  SHA256 cc48df4eb4c0710d45e7f5557e440129df44c5efb1af3ca3426ea9f9e3798276
  Git blob aeb2e1575879047d64d6cc961c5ea59043b555a4 from DnD-Classics/MASTER
NPCS.DAT 5731 bytes
  SHA256 f2dd1175bf42e8291b95952282dfc0cfb84455aaa532987cdd7380094c8eb30b
OBJECTS.DAT 21844 bytes
  SHA256 f23f0e7c1856904aa0156b0bf174cab794a41a327352b6ca00c3591ae5f2eed0
\`\`\`

The copied original \`INIT\` was verified by recalculating the Git
binary-blob SHA-1 with \`blob <size>\\0<data>\` before use. Temporary
empty local RES0 and T files existed only to satisfy
\`NativeMedia::open()\` folder recognition; neither served as authentic
content. No original files were committed or distributed.

Both actual compiled C++ and a completely separate local Python parser
agreed on the 32 level-resolution SHA-256 fingerprints, the entire
source-derived resolved ledger and the raw original family-byte hash.

## Source namespaces, original instances, and exact original values

| CODE.2 source class | Definition | Source RES3 ID interval | Archived instances |
|---|---|---:|---:|
| 0 | NPCS.DAT 83 NPC definitions | 0–2168 | 1,258 |
| 1 | INIT 175 placed-object descriptors | 2169–3315 | 4,324 |
| 2 | OBJECTS.DAT 336 item definitions | 3316–4157 | 622 |
| 3 | INIT 12 transient moving-effect descriptors | 4158–4233 | None in these three authored archive families |
| | | **4,234 known original RES3 entries** | **6,204** |

The original INIT directly authenticates the four bank starts
\`[0,2169,3316,4158]\`; family B's 175 descriptors close their
1,147-entry source bank, while the 12 transient descriptors close
their 76-entry bank. Original NPC local shapes top out at 2149 within
the 2169-entry bank. Original OBJECTS base-plus-stride reaches 842
exactly, closing its item bank.

Original compact authored world record data is preserved without
normalizing unrelated bytes. Family A retains original NPC facing
\`compact[10]&7\`. The one authenticated CODE.2 compatibility alias
maps family-C original OBJECTS type \`0x54\` to runtime type
\`0x58\`, with both the archived and runtime IDs represented in
the typed scene-instance ledger.

Every compact world instance produces an independently reconstructable
typed source fingerprint from:

\`\`\`text
source class, authored ordinal, original and runtime type,
fixed-point X/Y, original map cell, compact byte +10, proved NPC facing,
absolute RES3 definition shape-base ID
\`\`\`

This verifies **source base IDs**, not original original subframe,
animation stride selection, projection or sprite pixels.

## Proved and unproved boundary

The original \`RES3\` Git blob is present in
\`DnD-Classics/MASTER\` (Git SHA-1
\`8bcae74169185139b08138a16814558879f6370d\`, size 5,228,638).
However **it has not been copied into the local original verification
directory or passed to this compiled EH7 decoder**. The present local
\`rloft_native_scene_instances_audit\` therefore explicitly reports
\`original_res3_archive_present=false\` and
\`res3_archive_count_source=known_retail_directory_cardinality\`.
Original RES3 C++ corpus decoding/shape descriptor parity is an
OPEN acceptance gate; source-bank ID translation alone is not a
substitute.

Original DOS SINTAB x86 rasterizer execution, actual frame
color/geometry parity, exact frame/heading choice, first-person
occlusion/raycast parity, actor movement/AI, scripts, SAVE.DAT /
LEVnnn persistence, current-source Windows/MSVC desktop binary and a
user-tested playable native game are **NOT verified**.

## Reproducible research gates and controls

- Canonical \`SharedDecomp/verify_native_scene_instances.py\` reads
  those four originals itself, pins each original SHA, and compares
  the actual native report field by field, including 32 per-level
  SHA-256 values. It rejects duplicate JSON keys, unsupported
  gameplay/parity claims, missing or modified source fingerprints,
  out-of-bounds source definitions, invalid source map directory and
  malformed variant allocation.
- Synthetic original-free regressions validate data corruption,
  altered OBJ definition base with unchanged level bytes, false
  attestation and native per-level digest mismatch.
- C++ source-only tests verify decoded EH7 column descriptors,
  unaligned/shared pointers, compressed-stream exhaustion,
  the four original source class boundaries, NPC/OBJECTS type
  definitions, signed original fixed-point positions, legacy
  item alias, source-bound mutable visibility filtering and
  deterministic resolved-base fingerprints.
- Windows build actions remain owner-manual; no unapproved Windows
  release workflow was triggered.

**Next code milestone:** materialize and locally native-decode all 4,234
original RES3 EH7 streams from user-owned install/ISO; join selected
scene instances and source geometry to original-code traced angle,
view-subframe and SINTAB scale behavior, then EH6 indexed
texture/palette rendering. Maintain separate frame-oracle proof
before claiming visual fidelity.
