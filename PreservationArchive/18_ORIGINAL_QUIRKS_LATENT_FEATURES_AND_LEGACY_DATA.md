# Ravenloft: Strahd's Possession — Preservation Engineering Archive

## 18 — Original Quirks, Dormant Features, and Legacy Data

**Status:** FOUNDATION — cross-system anomaly classification begun; complete correction/quirk census pending  
**Branch:** `code1-decompilation`  
**Frozen original-evidence baseline:** `373c7f10e6fc0f29c1e805125ef048150fcc7ba7`  
**Controlling evidence:** original CODE.1/CODE.2 and shipped game resources, supplemented by certified executable, DAT and resource audits.

---

## 1. Definition and preservation discipline

The original code's unusual behavior is **not automatically a developer bug**, and an error once made in the modern decompilation is **not a shipped-game defect**. Every entry in this chapter has a type:

| Class | Meaning | Required treatment |
|---|---|---|
| **Q** | Original shipped behavior with a demonstrated non-obvious inconsistency or edge condition | Model faithfully; document any deliberate modernization |
| **D** | Implemented executable capability with no identified activating producer in the supplied corpus | Retain numeric/semantic capacity; do not invent gameplay activation |
| **L** | Serialized legacy/editor/reserved material or reproducible source-data anomaly | Preserve original bytes, boundaries, and provenance |
| **C** | Historical reconstruction *correction*: prior C++/documentation differed from original instructions | Keep corrected reconstruction; **never** attribute the defect to the original game |
| **U** | Original intent or alternate-release provenance not recoverable from supplied evidence | State a bounded UNKNOWN; do not fabricate meanings |

An observation may have more than one classification, but the nature of each claim must remain clear. The user's later native port can fix non-observable host implementation hazards while preserving observable game logic.

## 2. Original gameplay/serialization quirks (Q)

### Q-01 — Quiver container selector is outside the saved descriptor range

`OBJECTS.DAT` definition ID **329 (QUIVER)** has signed mutable-state selector **62** at record `+0x31`. CODE.2's runtime descriptor **61** is quiver-shaped but descriptor **62** is zero-filled; SAVE writes only descriptors **0..61**. The audited executable has **no** proved selector `62 -> 61` repair. This is a shipped identity mismatch, not a mandate to patch the source object database. Import/export must retain selector 62 and avoid silently remapping it. See [OBJECTS final audit](../DatFiles/OBJECTS.DAT/FINAL_AUDIT_2026-10-06.md).

### Q-02 — Priest level-loss normalizer uses a Mage-shaped table

The regular spell-preparation system has distinct priest and Mage matrix capacities, but level-loss normalization `3EB28` is used on the priest matrix with the **Mage-shaped** count table. This discrepancy is preserved by the certified gameplay logic and the SAVE final audit. It must be regression-tested as a separate level-loss path; correcting the table to priest capacities changes original behavior. See [Chapter 13 §14 and §86](13_SPELL_MAGIC_EFFECT_AND_PREPARATION_SYSTEM.md).

### Q-03 — Time deadlines have exact integer and branch semantics

Chapter [17](17_TIMING_INTERRUPTS_AND_FRAME_CADENCE.md) documents signed periodic-maintenance thresholds, an unsigned anchored-wait deadline, a 14-deep enable-state stack and separate 32-bit clocks. These are original edge semantics, **not** general invitations to introduce high-resolution host-time normalization. Neither a universal FPS nor behavior at every timer wrap has been dynamically certified.

### Q-04 — Save/load cancellation may leave original runtime handles open

The CODE.2 SAVE/LOAD selection loop contains right-click/cancel exit paths that omit an ordinary close of selected original runtime handles. This is an *original control-flow quirk*, not a requirement to leak host operating-system handles. A native port should match the observable cancel/return state while permitting safe internal cleanup, explicitly documenting that implementation difference. See [Chapter 08 §41](08_SAVE_PARTY_AND_PERSISTENCE_ARCHITECTURE.md).

## 3. Dormant executable features (D)

| ID | Executable facility | Evidence-bound restriction |
|---|---|---|
| **D-01** | CODE.2 `0x23DF0` repeats event-10 finale/status-12 sequence | Static application scan found no incoming call, branch, jump-table entry, relocation or known function pointer; **unreferenced duplicate**, not an extra live ending |
| **D-02** | ANM chunk 4 repetition/seek state machine `13488` | Present in code, **zero** instances in 13 shipped ANMs; original authoring intent unknown |
| **D-03** | RES `EH` resource type 5 loader `374E0` | Implemented dispatcher, **no** shipped resource of type 5 |
| **D-04** | RES image compression method 1 `2FC48` | Implemented signed-control RLE, unused by shipped type-1/2 resources |
| **D-05** | OBJECTS `+0x2B` scene influence `2B088` | Proven consumer, but zero for all **336** supplied object definitions |
| **D-06** | SAVE transient-effect type 5 and type 7 descriptors | Valid INIT/RES3 descriptors, but no audited direct producer identified in supplied paths |

Do not turn an absence of a **known** direct caller into proof of impossible execution under every hypothetical alternate build. See [finale reachability audit](../Runtime/STATUS12_23DF0_REACHABILITY_AUDIT.md), [ANM chapter](16_ANM_CINEMATIC_ENGINE_AND_TIMELINE.md), [resource executable-only formats](../Resources/RES/EXECUTABLE_ONLY_FORMATS.md), and [SAVE final audit](../DatFiles/SAVE.DAT/FINAL_AUDIT_2026-10-06.md).

## 4. Raw artifacts and older authoring generations (L)

**L-01 — Inactive interaction resources.** Shipped RES11/RES12 are a legacy/pre-split generation of the later active split archives. CODE.1/CODE.2 initialize indexed RES groups 0..10, not 11/12. The active RES9 wordcode records carry a second equally long word array (all zero in the supplied corpus); CODE.2 reads only the primary array. Preserve the companion words and the inactive archives byte-exactly; their historical editor/producer use is unknown.

**L-02 — Portrait sentinels.** RES1 terminal portrait slots 380/426 are zero-length and 381/427 hold the raw four bytes `60 00 00 00`. They are reserved, not malformed EH portraits to be synthesized or discarded. Literal producer meaning of `0x60` is unknown.

**L-03 — Level cell bit `0x2000`.** Only level 12/13/14 cell `(44,34)` carries this bit (word `0x2400`). The prior 76-reference CODE.2 cell-word audit found no direct bit-specific consumer. Classify as preserved legacy/authoring state, separate from the **unrelated character status bit `0x2000`**.

**L-04 — Standalone LEV022 visibility-cache discrepancy.** Forty cache cells in supplied LEV022 cannot be reproduced by the certified CODE.2 writer in the documented initial state; no hidden runtime bit is established. Retain source LEV cache bytes when importing rather than patching CODE.2 solely to match the fixture.

**L-05 — Noncanonical but original resource bytes.** Two RES type-4 Creative Voice resource length anomalies and nonzero bytes after the first NUL in **20** NPC names are real archival bytes. Likewise CODE.2 LZSS initialization leaves dictionary positions `0xFEE..0xFFF` undefined until written; the shipped 11,174 LZSS streams have **zero** observed undefined-history accesses according to the stored audit. Do not silently pad, normalize, or reinterpret these artifacts.

Primary cross-checks: [Resources final audit](../Resources/FINAL_RESOURCE_AUDIT.md), [LEVELS/LEV final audit](../DatFiles/LEVELS.DAT/FINAL_AUDIT_2026-10-06.md), [NPCS final audit](../DatFiles/NPCS.DAT/FINAL_AUDIT_2026-10-06.md).

## 5. Do not confuse decompiler repair with game bugs (C)

The post-accounting executable audits record genuine **reconstruction fixes**: CODE.1's `07E4` expanded live validation, CODE.1's four-plane VGA line progression rather than eight-plane rotation, and CODE.2's removal of an invented `1C124(0x3C)` call from `000B8..001F9`. Such entries are historical corrections to our understanding and code, not evidence the shipped executable contained those translation mistakes. The [CODE.1 final binary audit](../Code1/FINAL_BINARY_AUDIT.md) and [CODE.2 final semantic audit](../Code2/FINAL_SEMANTIC_AUDIT.md) remain the canonical correction history.

## 6. Regression and archival sign-off standard

- Keep **original bytes** for all supplied anomalies; do not rewrite binaries in order to satisfy new decoder assumptions.
- Distinguish a no-known-producer claim from a formal exhaustive reachability proof.
- Record field/offset, source-file identity, consumer and source-audit reference for each entry.
- For runtime-visible Q entries, test original-versus-native behavior, including cancellation, level-loss, inventory descriptors and timer thresholds.
- For D/L entries, test parse→encode **byte identity** and preserve dormant packet/field access without fabricating shipped usage.
- For C entries, link the exact before/after correction and validation; avoid perpetuating superseded explanations.

**Direct fixture identity cross-check in this continuation:** CODE.1, CODE.2, OBJECTS.DAT, NPCS.DAT, SAVE.DAT, LEVELS.DAT and LEV022.DAT sizes and SHA-256 hashes match the certified project records; SAVE stores raw clock values `1899`, `0x000E0000` and `1735`; raw OBJECTS record 329 holds selector 62. Prior large-corpus/decompiler audits were **read**, not rerun in this continuation.

**Next:** build the address-level Q/D/L/C/U census from all 17 subsystem chapters, add competing historical revision evidence where it exists, and verify each preservation recommendation against both the binary and source archive.
