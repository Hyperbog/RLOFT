# Ravenloft: Strahd's Possession — Preservation Engineering Archive

## 00 — Scope, Evidence Standard, Repository Baseline, and Open Historical Questions

**Status:** ACTIVE — fresh reconstruction pass  
**Repository:** `Hyperbog/RLOFT`  
**Working branch:** `code1-decompilation`  
**Frozen starting commit:** `373c7f10e6fc0f29c1e805125ef048150fcc7ba7`  
**Frozen commit message:** `code1: correct raw 07E4 expansion numerator`  
**Commit tree:** `b71f48f19e60607de20b546794b3b2f85f8075fa`  
**Repository-local preservation snapshot:** `preservation-backup-code1-2026-10-07`, created at the exact starting commit above and designated read-only by project policy.

Primary repository baseline: [commit 373c7f10](https://github.com/Hyperbog/RLOFT/commit/373c7f10e6fc0f29c1e805125ef048150fcc7ba7).

---

## 1. Purpose

This archive is a new, from-scratch technical reconstruction of the shipped technology in *Ravenloft: Strahd's Possession*. Its intended audience is simultaneously:

1. historians preserving the DreamForge/SSI DOS implementation;
2. programmers implementing a faithful native port;
3. reverse engineers validating the reconstructed code and data formats;
4. modders who need stable, precise descriptions of file formats, runtime structures, renderer behavior, scripting, audio, AI, combat, UI, save state, and resource ownership.

The earlier audits are evidence, not the narrative structure of this archive. Their conclusions will be re-checked, cross-linked, corrected where necessary, and expanded into subsystem-level explanations. The previous `TechnicalArchive/` material is likewise retained as historical project work, but it is **not** treated as authoritative merely because it already exists.

The controlling rule remains the one established by the certified CODE reconstructions: immutable shipped bytes outrank names, comments, hypotheses, compiler conventions, secondary documentation, and prior project prose. CODE.1's final audit explicitly distinguishes complete byte accounting from semantic correctness, while CODE.2's certification states that the immutable CODE.2 binary remains controlling evidence. Sources: [CODE.1 final binary audit](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code1/FINAL_BINARY_AUDIT.md), [CODE.2 final certification](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code2/FINAL_CERTIFICATION.md).

---

## 2. Preservation scope

### 2.1 Included as primary technical evidence

The fresh documentation pass covers the shipped-game reconstruction and preservation material in:

- `Code1/`
- `Code2/`
- `DatFiles/`
- `Resources/`
- `Runtime/`

The repository's audit workflows under `.github/` are secondary provenance where they encode verification equations, expected hashes, or historical audit gates.

The existing `TechnicalArchive/` is preserved and may be used as a lead/index, but every claim reused from it must be independently re-established from stronger evidence.

The repository tree at the frozen baseline contains 294 tracked files. At that revision, the primary evidence directories contain 8 `Code1` files, 7 `Code2` files, 95 `DatFiles` files, 82 `Resources` files, and 27 `Runtime` files. Source: [frozen repository tree at 373c7f10](https://github.com/Hyperbog/RLOFT/tree/373c7f10e6fc0f29c1e805125ef048150fcc7ba7).

### 2.2 Asset extraction suite: preserved, not a documentation target

The entire `Tools/` directory, including the Ravenloft Asset Extraction Suite under `Tools/anm/`, is part of the repository and **must be preserved unchanged by the historical baseline**. It is not deleted, omitted from backup, or treated as disposable generated material.

The extraction suite is excluded only from the principal subject matter of this archive: this archive documents the **original game technology**, not the modern extraction application's architecture. Tool code may nevertheless be cited when it performs a reproducible validation of a shipped format, provided the underlying claim is also traceable to the original bytes or certified format evidence. The tool's own README states that its decoders follow the certified `Resources/` documentation and preserve unresolved semantics rather than invent them. Source: [Tools/README.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Tools/README.md).

### 2.3 Explicitly outside this archive

The WIP `demo-v1` branch is outside the scope of this historical reconstruction. Modern-port behavior must never be used as proof of original behavior unless it is explicitly being compared against a captured original build and the original evidence is separately cited.

---

## 3. Evidence hierarchy

Every material claim must be assigned an evidence status. The following hierarchy is stricter than ordinary software documentation because the objective is historical preservation.

### A — BYTE-PROVEN

Directly demonstrated by immutable original bytes: executable instructions, serialized file structure, resource payload, table contents, string/data islands, headers, relocation records, or other shipped data.

Required citation: immutable artifact identity plus address/offset/range.

### B — CODE-PROVEN

Semantics established by an instruction-accounted reconstruction that has been checked against the immutable bytes.

Required citation: original logical/physical address and the corresponding certified reconstruction/audit location.

### C — CROSS-CORROBORATED

A mechanism is established by multiple independent primary artifacts—for example, a DAT field plus its CODE.2 consumer plus the resource it indexes.

Required citation: every primary evidence leg needed for the conclusion.

### D — OFFICIAL-DOCUMENTED

A fact comes from contemporary or official documentation: SSI manual/data card, compiler documentation, DOS/DPMI documentation, Miles/Audio Interface Library material, hardware vendor documentation, or another directly relevant period technical source.

Official documentation may explain terminology or intended behavior, but it cannot override contradictory shipped bytes.

### E — DEVELOPER TESTIMONY

A claim comes from a DreamForge/SSI developer, composer, artist, tester, or other participant in an interview, postmortem, correspondence, or recorded statement.

Such testimony is valuable historical evidence, but if it is not concretely corroborated it must remain labeled testimony rather than being merged into machine-proven architecture.

### F — COMMUNITY-CORROBORATED

Community reverse engineering, technical articles, walkthrough research, emulator observations, preservation notes, or other third-party analysis that has been checked against project evidence.

Community work must be credited. A correct community discovery does not become anonymous simply because this project later proves it independently.

### G — INFERRED

A strong architectural interpretation that fits the evidence but is not directly encoded or independently documented.

### H — UNKNOWN

Evidence is insufficient, contradictory, absent from the supplied build, or recoverable only from missing historical material. UNKNOWN is a valid preservation result.

The final archive must never silently promote D–G evidence into A–C.

---

## 4. Citation standard

Every substantive technical section must include citations at the point of use.

For repository evidence, citations must be pinned to a commit whenever practical and include the relevant path plus executable address, file offset, structure offset, resource ID, or record range. A mutable branch URL alone is insufficient for a preservation-grade claim.

For external sources, record:

- author/organization;
- title;
- publication/version/date when known;
- stable URL or archival identifier;
- whether the source is official, developer testimony, or community research;
- exactly which claim it supports;
- corroboration status.

For developer interviews or recollections, quote sparingly and preserve the distinction between what the participant remembered and what the binaries prove.

For community work, preserve attribution even after independent verification.

---

## 5. Certified executable baseline

### 5.1 CODE.1

The frozen repository records CODE.1 object accounting at 70,402/70,402 address bytes, comprising 69,361 translated instruction bytes and 1,041 proven non-code/data bytes, with no unresolved object bytes after the final audit. The post-closure `07E4` correction is significant to this new project because it demonstrates why “100% audited” cannot be treated as “historically infallible”: live comparison against the original exposed a semantic normalization error inside an already-accounted range. Source: [Code1/FINAL_BINARY_AUDIT.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code1/FINAL_BINARY_AUDIT.md).

Therefore every subsystem chapter must permit corrections to previously certified prose or reconstructed semantics when stronger direct evidence appears.

### 5.2 CODE.2

CODE.2 object 1 is certified as 239,698 bytes: 235,022 translated instruction bytes plus 4,676 proven non-code/data bytes, with 100% semantic-inspection coverage and zero unresolved object-1 bytes. Its certified reconstruction is `Code2/Code2.cpp`; the certification records original CODE.2 SHA-256 `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`, object-1 SHA-256 `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`, object-1 file start `0x3AF24`, virtual base `0x10000`, and entry logical offset `0x353A0`. Source: [Code2/FINAL_CERTIFICATION.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code2/FINAL_CERTIFICATION.md).

“Zero unresolved object bytes” means byte ownership and reconstructed execution semantics are closed to that audit boundary. It does **not** mean all historical source names, editor meanings, content-authoring semantics, or middleware internals are known.

---

## 6. Initial historical-unknown ledger

The following questions survive the prior audit program and are reopened as explicit preservation targets. They are not assumed unresolvable merely because they did not block faithful execution.

### U-001 — NPC main behavior flag source semantics

Six populated/serialized main behavior bits still lack proved source-level names; category-11 bits `0x20000/0x40000` have exact executable weapon-gate behavior but unresolved intended labels; four populated `+0x39` bits likewise lack proved direct semantic branches. Source: [NPCS.DAT final audit](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/NPCS.DAT/FINAL_AUDIT_2026-10-06.md).

### U-002 — NPC bytes `+0x3B/+0x3C`

Both bytes are zero in all 83 supplied NPC records and have no proved consumer in the certified build. They are preserved as raw fields, not declared padding. Another historical executable/data revision may be required to determine whether they had an authoring meaning. Source: [NPCS.DAT final audit](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/NPCS.DAT/FINAL_AUDIT_2026-10-06.md).

### U-003 — NPC author/editor terminology

The original names for the four-word directional-offset vector, scene-cell influence quantity, and finer story-NPC animation states remain unknown even though their physical/arithmetic behavior is bounded. Source: [NPCS.DAT final audit](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/NPCS.DAT/FINAL_AUDIT_2026-10-06.md).

### U-004 — OBJECTS hand-pair classes 0 and 1

The executable proves four numeric compatibility classes and strongly establishes class 2 as paired-hand-exclusive, but the original enum/source names for classes 0 and 1 are not recovered. Source: [OBJECTS.DAT final audit](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/OBJECTS.DAT/FINAL_AUDIT_2026-10-06.md).

### U-005 — OBJECTS byte `+0x2C`

All 336 supplied records store zero and no OBJECTS-relative CODE.2 consumer has been proved, including aliasing checks. It must remain raw until contrary primary evidence is found. Source: [OBJECTS.DAT final audit](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/OBJECTS.DAT/FINAL_AUDIT_2026-10-06.md).

### U-006 — OBJECTS range units and original terminology

The three range bytes have runtime behavior but their physical-world units are not established; the original name for the scene-cell influence accumulator and historical provenance of duplicated/legacy object definitions remain enrichment targets. Source: [OBJECTS.DAT final audit](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/OBJECTS.DAT/FINAL_AUDIT_2026-10-06.md).

### U-007 — LEVELS/LEV legacy and cache provenance

The supplied LEVELS/LEV corpus is structurally closed for preservation, but the prior work deliberately retains provenance/source-name questions, including the legacy object-table revision behind the `0x54 -> 0x58` Arrow alias, the original editor/source name of raw cell bit `0x2000`, and a 40-cell LEV022 automap/cache provenance discrepancy. Source: [LEVELS.DAT final audit](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/LEVELS.DAT/FINAL_AUDIT_2026-10-06.md), [LEVELS semantics](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/LEVELS.DAT/SEMANTICS.md).

### U-008 — SAVE character/status residual semantics

The SAVE audit preserves several bounded unknowns: character status low bits `0x0001..0x0020` with no proved gameplay ownership, serialized character dword `+0x1BA` without a direct CODE.2 field reference, exact source-level names for three paired timed slots/auxiliary quantities, and unnamed packed bits/enum labels in transient-effect state. Source: [SAVE.DAT final audit](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/SAVE.DAT/FINAL_AUDIT_2026-10-06.md).

### U-009 — RES9 companion bank historical purpose

Certified CODE.2 consumes the first RES9 word array but not the equal-length serialized companion array. Its historical producer/editor purpose remains unknown and it must be preserved verbatim. Source: [Resources final audit](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/FINAL_RESOURCE_AUDIT.md).

### U-010 — RES1 terminal `0x00000060` records

The terminal reserved face slots are bounded structurally and are not valid EH graphics, but the literal producer-side meaning of the two inactive `0x00000060` records remains unknown. Source: [Resources final audit](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/FINAL_RESOURCE_AUDIT.md).

### U-011 — RES11/RES12 exact historical producer/consumer build

The resource re-audit classifies RES11/RES12 as a legacy/pre-split composite generation retained on the CD and reconstructs the active-versus-legacy wordcode relationship. The exact older executable/editor/build that produced or consumed that generation remains unknown. Sources: [Resources full re-audit](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/FULL_REAUDIT_2026-10-06.md), [Resources final audit](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/FINAL_RESOURCE_AUDIT.md).

### U-012 — Uninstantiated resource type 5

CODE contains resource-type-5 handling, but no supplied Ravenloft resource instantiates the type. Its historical semantic field names therefore remain unproved from the supplied corpus. Source: [Resources full re-audit](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/FULL_REAUDIT_2026-10-06.md).

### U-013 — ANM chunk type 4 producer semantics

The executable control behavior can be documented, but the higher-level producer/authoring semantics of unused ANM chunk type 4 remain a historical question. Source: [Resources full re-audit](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/FULL_REAUDIT_2026-10-06.md).

### U-014 — ARIA GM1/GM2 inner bank grammar

The resource audit identifies `GM1.BNK`/`GM2.BNK` with the ARIA driver path rather than the AdLib path, but the inner bank grammar remains pending specification or shipped-driver reconstruction. Sources: [Resources final audit](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/FINAL_RESOURCE_AUDIT.md), [Resources full re-audit](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/FULL_REAUDIT_2026-10-06.md).

### U-015 — Core-facing audio configuration semantics

The runtime audio dependency audit identifies one remaining core-facing task: semantic naming of numeric configuration fields and determination of any game-visible audio timing/status dependency. It explicitly points to tracing the two CODE initialization callees and/or comparing additional differently configured `SOUND.CFG` samples. Source: [Runtime audio-interface dependency audit](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Runtime/AUDIO_INTERFACE_DEPENDENCY_AUDIT.md).

### U-016 — Optional driver internals

The optional driver triage identifies 18 LX/OS2-style audio DLLs for period hardware backends and correctly defers full driver decompilation unless a core-facing ABI question requires it. In this preservation project, those internals remain candidates for end-stage historical archaeology rather than being dismissed permanently. Source: [Optional executable/driver triage](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Runtime/OPTIONAL_EXECUTABLE_DRIVER_TRIAGE.md).

---

## 7. Correction policy

When cross-referencing exposes an error, the archive will distinguish three cases:

1. **Documentation correction only** — terminology or explanation was too strong, while code/data reconstruction remains byte-faithful.
2. **Semantic reconstruction correction** — reconstructed C++ behavior differs from immutable machine behavior. This requires a minimal code correction, address-based justification, revalidation, and a permanent correction record.
3. **Tooling correction** — parser/analyzer/serializer behavior or labels are inconsistent with primary evidence. Tools may be corrected, but the immutable source artifacts and preservation snapshot are never rewritten.

The CODE.1 `07E4` expansion correction is the model for case 2: it corrected semantics inside a range whose byte accounting had already been closed. Source: [CODE.1 post-closure checkpoint 10p](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code1/FINAL_BINARY_AUDIT.md).

Any correction made during this project must record:

- previous claim/behavior;
- corrected claim/behavior;
- exact evidence;
- affected addresses/offsets/files;
- whether serialized compatibility changes;
- validation performed;
- commit that introduced the correction.

---

## 8. Documentation build order

The archive will be built from low-level certainty upward:

1. **Repository/source provenance and immutable identities**
2. **Executable/container/runtime architecture**
3. **Process startup, CODE.1/CODE.2 relationship, lifecycle, shutdown**
4. **Memory model, selectors, DPMI, DOS interfaces, allocator/runtime**
5. **Resource manager and archive formats**
6. **Graphics pipeline, VGA, compiled shapes, scale rasterizer, palettes**
7. **World/level representation, coordinates, movement, collision, doors**
8. **Automap and map-export architecture**
9. **Input, event queues, mouse/keyboard, top-edge/menu UI**
10. **Party and character model**
11. **Items, inventory, equipment, containers, charges**
12. **Creatures/NPCs, AI, perception, movement, animation**
13. **Combat, damage, saving throws, effects, conditions**
14. **Spells/magic and effect scheduling**
15. **Event-script/wordcode systems and dialogue/text dispatch**
16. **Save/load and persistent/transient state**
17. **Audio architecture: game API, AIL/Miles boundary, music, SFX, hardware drivers**
18. **Cinematics/ANM**
19. **Timing, PIT/interrupt scheduling, frame/update cadence**
20. **Original bugs, quirks, latent features, abandoned/legacy data**
21. **Release/media/build provenance and DreamForge lineage**
22. **Modder/programmer reference: stable IDs, structures, extension constraints**
23. **Final contradiction/UNKNOWN audit**

Subsystem chapters must explain both **what the code does** and **how the pieces connect**. A field dictionary without producer/consumer call chains is insufficient.

---

## 9. Preservation invariant

The archive is not complete merely when the supplied build can be reproduced.

The historical target is stronger:

> Every recoverable behavior, data relationship, architectural boundary, implementation quirk, unused-but-shipped capacity, and historically meaningful uncertainty should be documented to the strongest evidence level currently obtainable.

Where original source/editor terminology cannot be recovered, the archive will use explicit reconstructed names and preserve the unknown rather than manufacture a period name.

Where official documentation or developer testimony supplies information absent from the binaries, it will be cited and labeled by evidence class.

Where community researchers discovered something first, they will be credited and their result will be independently checked before promotion.

The next document begins with the executable/runtime foundation and reconstructs the startup/container relationship without importing assumptions from the previous `TechnicalArchive/` narrative.
