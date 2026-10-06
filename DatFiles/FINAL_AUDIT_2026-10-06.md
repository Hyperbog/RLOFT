# Final DAT reconstruction audit — 2026-10-06

Status: **PART 3 DAT FILE RECONSTRUCTION — 100% COMPLETE TO THE SUPPLIED EVIDENCE BOUNDARY.**

This document is the final cross-format certification for the six supplied Ravenloft: Strahd's Possession DAT artifacts. It does not claim that every historical source/editor name has been recovered, nor that every format already has a standalone general-purpose authoring tool. It certifies that the supplied-build formats are structurally reconstructed, their active runtime semantics are bounded to the available executable/resource evidence, cross-file ownership is reconciled, and remaining unknowns are explicitly preserved rather than guessed.

## Canonical source identities

```text
OBJECTS.DAT   21,844 bytes
SHA-256       f23f0e7c1856904aa0156b0bf174cab794a41a327352b6ca00c3591ae5f2eed0

INFO.DAT          17 bytes
SHA-256       4ffa6ac5d5b5afd103bd67d84d3a372b401b3b19407b0c68c44bb0dfd9e32c30

NPCS.DAT       5,731 bytes
SHA-256       f2dd1175bf42e8291b95952282dfc0cfb84455aaa532987cdd7380094c8eb30b

LEV022.DAT     6,028 bytes
SHA-256       f88b033ff4cdafdd89620e0fac51ff2fb81bce103a6d31a26b91464532c9f51e

SAVE.DAT       9,288 bytes
SHA-256       f898cb92fca6447954f5e0b2bf6f8ad2435e4a64ae3b9de74f8bf75e0ed10d5d

LEVELS.DAT   499,878 bytes
SHA-256       50d0b1d13f81733a1ac55296d00fcba9aef002202bc00341f1062c37fd017d91
```

Controlling executable identities used across the reconstruction:

```text
CODE.1  277,787 bytes
SHA-256 fd9b709ceee190e6a70870f9012ebea3bc437cae6bc74e0150d6f922fa750fed

CODE.2  558,689 bytes
SHA-256 853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314
```

## Per-format final status

### OBJECTS.DAT

**ARCHIVAL-COMPLETE.**

- fixed 336 × 0x41 record geometry;
- every physical byte accounted for;
- RES3 class-2 scene-shape and RES1 UI namespaces reconciled;
- category, weight, equipment, combat, damage, ranges, mutable-state selectors, container state, charge state, magic visibility, and packed hand behavior reconstructed;
- shipped selector-62 QUIVER mismatch preserved exactly;
- latent `+0x2B` and unresolved `+0x2C` bounded;
- semantic decode→encode reproduces the canonical file byte-for-byte.

Certification: `OBJECTS.DAT/FINAL_AUDIT_2026-10-06.md`.

### NPCS.DAT

**ARCHIVAL-COMPLETE.**

- fixed 83 × 0x45 record geometry;
- every physical byte accounted for;
- LEVELS family-A namespace and RES3 class-0 scene-shape linkage reconciled;
- combat, XP, collision, renderer, sound, initial runtime state, effect/status aliases and weapon gates reconstructed;
- residual populated flag capacity explicitly bounded and preserved;
- 20 records with nonzero post-NUL name-field residue preserved exactly;
- semantic decode→encode reproduces the canonical file byte-for-byte.

Certification: `NPCS.DAT/FINAL_AUDIT_2026-10-06.md`.

### INFO.DAT

**ARCHIVAL-COMPLETE.**

- exact 13-selector + signed path-length + path layout;
- active RES0..RES10 path routing proved independently in CODE.1 and CODE.2;
- shared installation/resource path independently consumed by LEVELS event handling;
- selector positions 11/12 bounded as historical preserved slots with no active consumer proved in the certified executable pair;
- RES11/RES12 legacy lineage reconciled without falsely promoting those selector slots;
- semantic decode→encode reproduces the canonical file byte-for-byte.

Certification: `INFO.DAT/FINAL_AUDIT_2026-10-06.md`.

### SAVE.DAT

**ARCHIVAL-COMPLETE.**

- fixed prefix, four 0x248 active-party records, variable counted transient/moving-effect family, UI state, mutable container bank, renderer/environment controls, 15 charge counters, ten-record saved roster, and trailing UI contexts reconstructed;
- character record identity, status, race/class/alignment/age, HP/progression, ability pairs, inventory, spell matrices, effects/timers, known-item bits and equipment auxiliaries placed;
- variable-size rule from live-effect count proved;
- bounded opaque/reserved fields explicitly preserved;
- original compatibility anomalies retained.

Certification: `SAVE.DAT/FINAL_AUDIT_2026-10-06.md`.

### LEVELS.DAT / LEVnnn.DAT

**ARCHIVAL-COMPLETE TO THE SUPPLIED EVIDENCE BOUNDARY.**

- all 32 archive records reconcile structurally;
- packed cell layer, 10-byte cell-control records, family A/B/C records, event table, header/tail state and automap/cache model reconstructed;
- family A links directly to NPCS.DAT;
- family B/C link directly to OBJECTS.DAT;
- family-C legacy `0x54 -> 0x58` compatibility alias resolved as Arrow of Slay Undead;
- all opcode-11 uses classified;
- cell bit `0x2000` bounded as preserved authoring/legacy state with no direct runtime consumer;
- standalone LEV writer/loader model reconstructed.

Certification: `LEVELS.DAT/FINAL_AUDIT_2026-10-06.md`.

### LEV022.DAT

**CERTIFIED MUTABLE LEVEL-SAVE FIXTURE.**

- direct-save geometry and state ordering match the certified writer/loader model;
- exact Level-22 mutable delta is reconstructed;
- one 40-cell generated-cache discrepancy is deliberately retained as a save-provenance / executable-revision / unproved-production-path issue;
- the certified CODE.2 cache algorithm is not altered to force the artifact to match.

LEV022 is covered by the combined LEVELS final audit and its own directory documentation.

## Cross-format consistency audit

### LEVELS -> NPCS

Family A is the creature/NPC world-record family.

```text
NPCS definitions             83
family-A records          1,258
supplied family-A type IDs 0..82
```

All supplied family-A type references are inside the NPCS namespace. No alternate creature definition table is required.

### LEVELS -> OBJECTS

Families B and C both use the OBJECTS type namespace.

```text
OBJECTS definitions          336
family-B records           4,324
family-C records             622
family-B supplied IDs       0..174
family-C supplied IDs       0..335
```

All supplied type references are in range. The one legacy family-C ID migration is explicitly handled by the original loader rather than by rewriting the archive.

### OBJECTS -> SAVE mutable state

OBJECTS static definitions do not embed instance contents or current wand charges.

Category-1 containers select mutable container descriptors/state through `+0x31`.

The supplied object database contains:

```text
container definitions       62
distinct selectors          62
selector set                0..60 plus 62
```

SAVE serializes 62 descriptor-backed states, range 0..61. The resulting selector-62/state-61 mismatch is a shipped compatibility anomaly and is preserved.

Charged devices select a separate fifteen-byte SAVE-persistent charge vector. The supplied quick-start fixture contains the expected unique initialized `0x0C * 15` vector at SAVE `+0x0D59`.

Static definitions and mutable save state therefore remain cleanly separated.

### NPCS/OBJECTS -> RES3

The certified scene-source bank partition is:

```text
class 0  NPCS            RES3 0..2168
class 1  placed records  RES3 2169..3315
class 2  OBJECTS         RES3 3316..4157
class 3  transient FX    RES3 4158..4233
```

NPCS `+0x14` and OBJECTS `+0x28/+0x2A` store bank-local shape identities. No on-disk DAT field needs to be rewritten to an absolute RES3 ID.

### SAVE transient effects -> INIT/RES3 class 3

The counted 0x16 SAVE array is the same transient/moving-effect family consumed by the class-3 scene emitter. Its type field indexes the twelve INIT class-3 descriptors; it is not an NPCS or OBJECTS type ID.

This resolves a former cross-file namespace ambiguity.

### INFO -> resource system

INFO controls active archive *location*, not archive format or contents.

Selectors 0..10 route RES0..RES10 between local/current and prefixed paths. RES11/RES12 remain historical resource generations on the supplied CD and do not require changing INFO's active supplied-build semantics.

### LEVELS <-> LEV022

LEVELS record 22 and LEV022 independently validate the compact-archive versus expanded-direct-save distinction.

The mutable delta remains bounded to:

- visibility/exploration cell bits;
- one live ASSASSIN position/facing/state update;
- expanded family-B representation;
- generated cache state, subject to the explicitly retained 40-cell provenance discrepancy.

No second incompatible level schema is required.

## Bounded anomalies and unknowns

The final audit intentionally retains the following without treating them as reconstruction failure:

- LEVELS cell bit `0x2000`: preserved authoring/legacy state, no direct runtime consumer;
- LEV022 40-cell cache discrepancy: provenance/revision/production-path boundary;
- family-C `0x54 -> 0x58`: loader compatibility migration with unknown historical database revision;
- OBJECTS category-1 selector 62 versus persisted descriptor/state range 0..61;
- OBJECTS `+0x2C`: zero in supplied data, no proved consumer;
- NPCS residual behavior bits and zero `+0x3B/+0x3C`;
- SAVE bounded opaque/reserved character/status/effect fields;
- INFO selector positions 11/12;
- original source/editor names and historical build provenance where runtime behavior is already bounded.

These are preservation requirements and historical research targets, not unidentified structural regions.

## Tooling milestone boundary

Reconstruction completion and authoring-tool completion are separate milestones.

Already byte-exact semantic round-trip verified:

```text
OBJECTS.DAT
NPCS.DAT
INFO.DAT
```

SAVE.DAT and LEVELS/LEV reconstruction is complete to the supplied evidence boundary, but a fully independent general-purpose byte-exact serializer/editor remains a separate implementation milestone where not already demonstrated by dedicated tooling.

This distinction prevents "100% reconstruction" from being confused with "all future tooling finished."

## Final decision

The six supplied DAT artifacts have now all received final archival review, and their cross-file relationships reconcile without an unresolved structural contradiction.

Therefore:

> **Part 3: DAT File Reconstruction is 100% complete to the supplied evidence boundary.**

Any future DAT work is optional enrichment or implementation work:

- source/editor terminology recovery;
- older/newer build comparison;
- generic authoring/serializer tooling;
- compatibility-fix modes that are explicitly separate from faithful original behavior.

New evidence may refine names or historical provenance, but it should not silently rewrite the certified supplied-build model.
