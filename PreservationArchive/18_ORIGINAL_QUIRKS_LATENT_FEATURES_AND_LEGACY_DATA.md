# Ravenloft: Strahd's Possession — Preservation Engineering Archive

## 18 — Original Quirks, Dormant Features, and Legacy Data

**Status:** CROSS-SUBSYSTEM Q/D/L/C/U CENSUS COMPLETE TO CURRENT PRIMARY EVIDENCE — runtime implementation tests pending  
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

**Current pass:** cross-subsystem census recorded. The canonical UNKNOWN inventory and compatibility acceptance criteria follow; any new historical build evidence must be incorporated through the correction policy.


## 7. Expanded Q census — original executable behavior

**Q-05 — Automap digit permutation.** CODE.2 `02A34` writes the hundreds digit, *ones* digit and *tens* digit into the three placeholders of `AUTO???`. Thus map ID `12` maps to `AUTO021`, not `AUTO012`; ID `22` deceptively yields ordinary-looking `AUTO022`. Preserve the exact conversion on legacy export and when locating existing generated automaps. [Automap chapter §41](06_AUTOMAP_NAVIGATION_AND_EXPORT.md).

**Q-06 — Category-11 weapon signedness.** Both [creature behavior §53](11_NPC_CREATURE_AI_AND_LIVE_ACTOR_MODEL.md) and [combat §26](12_COMBAT_ATTACK_DAMAGE_SAVES_AND_PROGRESSION.md) trace a synthetic category 11 for enchanted weapons. CODE.2 zero-extends the enchantment byte but compares it by signed branch against sign-extended `-2`; with supplied nonnegative enchantments, NPC flag `0x00020000` forces the one-damage floor, while flag `0x00040000` does not independently reduce damage. Store the flags as distinct numerical bits; never normalize them into an invented magical immunity model.

**Q-07 — Five-byte overwrite of a 50-byte save label.** For serializer argument `-1/0xFFFFFFFF`, the special `066E8` save path copies exactly five bytes from `TEMP` into an **existing** 50-byte label buffer, then writes the full 50-byte record. The remaining 45 bytes are not overwritten by that specific copy. Preserve the source-byte result in a compatibility serializer; do not label it a general-purpose safe `strncpy`. The special caller's historical intention remains unknown. [SAVE chapter §40](08_SAVE_PARTY_AND_PERSISTENCE_ARCHITECTURE.md).

**Q-08 — Event-11 reserved selector values.** LEVELS event opcode `0x0B` interprets argument `0x0B`, `0x0C`, or `0x0D` as three fixed story routines (final Strahd, possession, Irmgarde). Other arguments enter the numbered external-interaction path. The selector is neither a uniform RES9 wordcode index nor an unconditional cinematic filename ID. [Event chapter §102](14_EVENT_WORDCODE_DIALOGUE_AND_STORY_STATE.md).

**Q-09 — Short-write versus cancellation semantics.** SAVE writer `066E8` closes the file and reports failure on a short write, whereas the original menu's cancel paths can leave selected handles open. Preserve original failure/return behavior, but permit native host-handle cleanup when not game-observable. [SAVE chapter §§39–41](08_SAVE_PARTY_AND_PERSISTENCE_ARCHITECTURE.md).

## 8. Expanded dormant/legacy census

**D-07 — Five unexercised active wordcode instructions.** The active RES9 bank does not use opcodes `03` (set choice target), `04` (set choice text), `0B` (clear persistent flag), `0C` (toggle persistent flag), or `1C` (direct SFX), although the shipped VM dispatches all five. A faithful modder-compatible interpreter must implement these opcodes even though a replay of existing scripts would not cover them. [Wordcode chapter §50](14_EVENT_WORDCODE_DIALOGUE_AND_STORY_STATE.md).

**L-06 — Physical VOC length disagreement.** `RES11:4877` and `RES12:430` contain identical Creative Voice data but declare `0x11669` bytes when only `0x01669` are physically present, a precise `0x10000` difference. They are the only two mismatches among 1,005 surveyed type-4 resources. Parsing tools must report the mismatch rather than quietly fixing the archive length. [Audio chapter §14](15_AUDIO_MUSIC_SFX_AND_DRIVER_INTERFACE.md).

**L-07 — Legacy family-C object migration.** Compact family-C serialized ID `0x54` is remapped by the level loader to `0x58`, the current `ARROW OF SLAY UNDEAD`. The current OBJECTS definition at `0x54` is `STONE GIANT GAUNTLETS`. This is evidence of an authoring/resource-table revision, not license to substitute `0x54` globally; migration occurs at the precise legacy-load boundary. [Item chapter §64](10_OBJECT_INVENTORY_EQUIPMENT_AND_CONTAINERS.md).

**L-08 — Legacy music bank is a distinct revision.** The original RES11 bank has **74** type-3 sequences in two 37-entry banks. Final RES5 has **98** in two 49-entry banks, with no complete legacy sequence byte-identical to a final entry. These are distinct serialized generations; the absence of exact byte matches does not prove their musical compositions differ. [Audio chapter §18](15_AUDIO_MUSIC_SFX_AND_DRIVER_INTERFACE.md).

**L-09 — NPC behavior residuals are not padding.** All eight bits of NPC definition byte `+0x39` occur in the supplied NPC corpus; CODE.2 directly proves mask `0xE1` and leaves populated `0x1E` outside the currently named consumer set. The two main behavior bits `0x00020000` and `0x00040000` have the machine behavior described in Q-06 but lack trustworthy original design labels. Preserve both raw and interpreted views. [NPC chapter §§62–65](11_NPC_CREATURE_AI_AND_LIVE_ACTOR_MODEL.md).

## 9. Reconstruction correction census and identifier disambiguation

The `C-` labels in previous subsystem documents are **not a globally unique primary key**. Chapter 02's local `C-03` (CODE.1 extender identification), `C-04` (extender code-9 identity), `C-05` (Watcom enum incompatibility), and `C-06` (DPMI ownership header) collide numerically with later preservation-wide labels: Chapter 04 `C-03` (VGA dimensions), Chapter 05 `C-04` (LEV file form), and Chapter 06 `C-05/C-06` (automap model and export implementation).

**Correction policy CP-18.1:** identify an earlier correction by **`(chapter number, C-label)`**, or by chapter/section title, never by `C-nn` alone. Do not renumber legacy headings silently: external commit references may already point to them. Chapter 17's `C-39` appears inside §20 as a correction paragraph rather than as a separate numbered heading; index it by `(17, §20, C-39)`.

A corrected C++ reimplementation is not a discovery of a gameplay bug. Examples with exact audit provenance include CODE.1 `07E4` live-state handling, four-plane VGA mask rotation, and CODE.2 `000B8..001F9` removal of an invented call. The [CODE.1 audit](../Code1/FINAL_BINARY_AUDIT.md) and [CODE.2 semantic audit](../Code2/FINAL_SEMANTIC_AUDIT.md) establish those repairs; Chapter 18 records only their classification and traceability.

## 10. Direct native compatibility tests to prioritize

| Fixture / trigger | Observable or archival invariant |
|---|---|
| Automap ID 12 | Filename is `AUTO021`, not conventional `AUTO012` |
| NPC flag `0x00020000` / `0x00040000` | Preserve category-11 branch order and original integer signedness |
| Save label with special `-1` argument | Only the first five bytes overwritten before complete 50-byte write |
| LEVELS event `0x0B`, args 11/12/13/other | Three fixed calls versus external numbered path |
| RES9 VM unused instructions 03/04/0B/0C/1C | Dispatch all five independently; do not require shipped script coverage |
| RES type-4 mismatches | Preserve declared/physical sizes and original bytes |
| Family-C ID `0x54` | Remap only in the proved legacy loader |
| RES11 and RES5 music | Keep independent banks and original indices, without deduplicating |
| NPC `+0x39` and high flags | Preserve unknown populated bits byte-for-byte |

This continuation rechecked **existing audited source documents and GitHub commit state**. It has **not rerun** every original executable, file decoder or gameplay test. Requirements in this table are pending executable/native regression tests, not falsely certified as such.

**Next:** complete cross-component anomaly traceability for process shutdown, save errors, object IDs and audio/render cases; build a stable index for all bounded UNKNOWNs; then close Chapter 18 with a contradiction check before proceeding to release/build provenance.


## 11. Bounded UNKNOWN inventory reconciled with Chapter 00

The canonical initial [scope/UNKNOWN ledger](00_SCOPE_EVIDENCE_BASELINE_AND_OPEN_QUESTIONS.md) contains **16** independently numbered entries. Their original IDs remain stable; the following table supplies the Chapter-18 preservation action, **not** a claim that every historical source name can be recovered.

| ID | Remaining unresolved fact | Evidence-preserving disposition |
|---|---|---|
| U-001 | Original labels for six NPC main-behavior bits and four residual `+0x39` bits | Preserve packed bits and confirmed masks; keep Q-06's two category-11 branch semantics separately |
| U-002 | NPC `+0x3B/+0x3C`, zero in all 83 definitions | Keep both bytes as raw, not mandatory padding |
| U-003 | NPC directional-vector, cell-influence and animation-state original names | Keep reconstructed behavioral names and exact field geometry |
| U-004 | Original hand-pair class 0/1 names | Preserve numeric classes; class 2 is the proved paired-hand-exclusive group |
| U-005 | OBJECTS `+0x2C` meaning, zero in 336 records | Preserve byte raw; no proved CODE.2 consumer |
| U-006 | OBJECTS range units, scene-accumulator terminology, duplicate-definition provenance | Preserve units as encoded and exact object-ID identities |
| U-007 | LEVELS/LEV authoring provenance, object-ID `0x54 -> 0x58`, cell bit `0x2000`, LEV022 cache | Preserve original state, document loader transforms, do not invent revision history |
| U-008 | SAVE status low bits `0x0001..0x0020`, character `+0x1BA`, residual packed auxiliaries | Keep all unproved bits/dwords in byte-exact character records |
| U-009 | Purpose of the unused companion array in RES9 wordcode | Preserve the second `word_count` array verbatim |
| U-010 | Meaning of reserved RES1 face-slot literal `0x00000060` | Retain zero-length and four-byte sentinel entries |
| U-011 | Exact legacy producer/editor/executable for RES11/RES12 | Preserve separate generations and proven active-versus-legacy grammar |
| U-012 | Historical meaning of absent EH resource type 5 | Retain executable loader contract; no fabricated shipped instances |
| U-013 | Historical authoring meaning of unused ANM type-4 control | Preserve machine seek/repeat behavior and no-corpus-usage classification |
| U-014 | Inner grammar of ARIA `GM1.BNK/GM2.BNK` | Keep intact; defer third-party-driver archaeology |
| U-015 | Unnamed SOUND.CFG numeric fields and active audio-driver completion/cadence details | AIL/32 `svc_rate` units now known; **individual driver rates/config word labels remain open** |
| U-016 | Full optional LX/OS2 audio-driver internals | Separate historical archaeology; do not pretend full driver decompilation is necessary for base gameplay |

Relevant canonical sources: [NPC final audit](../DatFiles/NPCS.DAT/FINAL_AUDIT_2026-10-06.md), [OBJECTS final audit](../DatFiles/OBJECTS.DAT/FINAL_AUDIT_2026-10-06.md), [LEVELS final audit](../DatFiles/LEVELS.DAT/FINAL_AUDIT_2026-10-06.md), [SAVE final audit](../DatFiles/SAVE.DAT/FINAL_AUDIT_2026-10-06.md), [Resources final audit](../Resources/FINAL_RESOURCE_AUDIT.md), [audio ABI](15_AUDIO_MUSIC_SFX_AND_DRIVER_INTERFACE.md), and [timing ABI](17_TIMING_INTERRUPTS_AND_FRAME_CADENCE.md).

## 12. Compatibility implementation priority

**Release-blocking for byte-compatible data handling:** all unmodified original record and archive bytes, unknown occupied bits, per-level mutable cache, 62 serialized container descriptors, raw interaction companion arrays, and original save timebases. These obligations do not depend on recovering historical developer terminology.

**Release-blocking for gameplay behavior:** original item selection and inventory quirks; signed enchantment comparison; priest level-loss normalization; story opcode special dispatch; timing-domain independence; save/load success and failure logic; actual multimedia packet dispatch.

**Preservation-only until new original evidence appears:** undocumented type-5 payload names, unused ANM chunk-4 authoring model, dormant source paths without identified runtime producers, exact RES11/12 build lineage, and ARIA bank grammar. None of these should be reclassified as game-reachable merely for feature completeness.

**Controlled modernization:** safe native file-handle cleanup and host audio/graphics abstractions are acceptable only if visible game-state transitions and original bytes remain stable. Separate such changes in the future *native implementation* archive from this *original technology* archive.

### Chapter-18 audit gate

- Q-01..Q-09: **nine distinct original behavioral entries**, including the partial overlap Q-04/Q-09 on save cancellation; the overlap is intentional as ownership vs failure-path details, not two unrelated bugs.
- D-01..D-07: **seven dormant/unsupported activation entries** with explicitly bounded evidence of absence.
- L-01..L-09: **nine legacy, reserved or anomalous data classes**, not all defects.
- U-001..U-016: **all sixteen original scope UNKNOWN entries retained**; some narrower semantics have since been proved without proving historical intent.
- C corrections: source-local labels must be chapter-qualified because labels were reused. Original executable behavior is not conflated with C++ translation fixes.

This census is **documentation-level** and grounded in the previously certified source audits. It does **not** constitute a fresh whole-corpus test run, a native behavior regression certificate, or recovery of missing historical builds.

**Next archival component:** Chapter 19, release/media/build provenance and DreamForge lineage. Revisit any Chapter-18 entry if that investigation yields earlier original executable or editor evidence. Final project-wide contradiction checking remains a separate future chapter.
