# Ravenloft: Strahd's Possession — Preservation Engineering Archive

## 21 — Final Contradiction, UNKNOWN, and Evidence Certification

**Status:** ACTIVE / FINAL AUDIT FOUNDATION — link integrity and primary-evidence boundaries checked; semantic contradiction/cross-chapter certification IN PROGRESS  
**Working branch:** `code1-decompilation`  
**Frozen original-source baseline:** `373c7f10e6fc0f29c1e805125ef048150fcc7ba7`  
**Audit subject:** current Chapters 00–20, original CODE.1/CODE.2 and supplied DAT/SINTAB fixtures, plus the certified original Resources and Runtime audits.  
**No 100% completion claim until:** each remaining contradiction is resolved with original evidence or explicitly bounded, all canonical claims have consistent provenance, and verification scope is correctly labeled.

---

## 1. Audit standard: what “complete” means

Three independent levels must not be collapsed:

- **Source-identity/structure:** original SHA-256s, parser record widths, fixed and dynamic offsets, resource and executable boundaries; original source bytes are the authority.
- **Behavior:** CODE.1/CODE.2 instruction semantics, producer/consumer ownership, event and state transitions; exact machine comparisons prevail over intuitive interpretations.
- **Historical and execution environment:** game patch history, CD mastering, designer/editor source identifiers, multimedia driver callback rates and DOS hardware timing. These can remain bounded UNKNOWNs if evidence is absent.

A published chapter is eligible for static documentation closure if it accurately records the first two levels and represents unproved third-level claims as unknowns. It does **not** become a native implementation acceptance test or an authenticated older-release reconstruction by virtue of having been written.

## 2. First mechanical cross-chapter gate: source links and chapter presence

The repository tree and published Markdown contents were read at branch `code1-decompilation`, after the completed Chapter-20 source audit:

```text
Numbered PreservationArchive chapters present:  21 (00..20)
Missing numbered documents within range:          0
Relative Markdown file links inspected:           86
Relative destinations absent from branch tree:     0
Canonical source document Chapter 20 sections:    18
Locally available original files re-hashed:        9 / 9
Worked source-byte examples checked:              19 / 19
```

The relative-link check resolves `../` against each document's directory and checks file-path existence against the tracked GitHub tree. It does **not** verify remote web sites, commit-pinned external targets, section-anchor correctness, legal media provenance or semantic consistency of the text at those paths. The reported 9-file integrity and 19 worked-example tests are the documented local checks of the current original fixture set.

## 3. Apparent cross-component contradictions already reconciled

| Apparent conflict | Resolution supported by primary or certified source | Canonical owner |
|---|---|---|
| `CODE.2` says `Ver 1.2a`; DOS/4GW says `1.96`; SINSTALL says `4.2.6b` | These identify the game executable, bound extender and third-party sound installer respectively | [19 — provenance](19_RELEASE_MEDIA_BUILD_PROVENANCE_AND_DREAMFORGE_LINEAGE.md) |
| Thirteen RES archives, but indexed CODE load count eleven | RES11/12 are retained older composite generations; certified CODE.1/CODE.2 load indexed RES0..10 | [03 — resource manager](03_RESOURCE_ARCHIVES_CACHE_AND_RESOLUTION.md) |
| INFO.DAT has thirteen selector characters, while CODE loads eleven archives | All thirteen positions are read; only 0..10 have proved active indexed-loader consumers; remaining digits are preserved | [20 — programmer reference](20_MODDER_PROGRAMMER_REFERENCE.md) |
| `CODE.2` has a finale routine `0x23DF0` but no third live finale trigger | Shipped static code has an **unreferenced duplicate**; separate live story and party-wipe paths remain authoritative | [18 — quirk census](18_ORIGINAL_QUIRKS_LATENT_FEATURES_AND_LEGACY_DATA.md) |
| Level-22 standalone saved cache differs in forty cells from the CODE-generated cache | Retain fixture bytes for lossless importing, reproduce certified CODE writer for new live cache; exact older source provenance unknown | [06 — automap](06_AUTOMAP_NAVIGATION_AND_EXPORT.md) |
| OBJ QUIVER selector 62 but only 62 serialized state descriptors 0..61 | Genuine original incompatibility/identity mismatch; no proved automatic 62→61 correction | [10 — items](10_OBJECT_INVENTORY_EQUIPMENT_AND_CONTAINERS.md) |
| Character `status_word & 0x2000` and level `cell & 0x2000` | Different owners: former participates in action blocking, latter has no directly proved cell-specific consumer | [20 — status/field glossary](20_MODDER_PROGRAMMER_REFERENCE.md) |
| `SAVE.DAT` layout has “fixed” offsets but grows with transient effects | Only its fixed prefix/early party offsets are invariant; later fields shift by `0x16*effect_count` | [08 — save model](08_SAVE_PARTY_AND_PERSISTENCE_ARCHITECTURE.md) |
| 1995 patch archives and 2015 1.2a checksums seem to date one binary from 1994 | Contemporary patch/distribution dates are not original compilation or resource-mastering dates; provenance is per file | [19 — provenance](19_RELEASE_MEDIA_BUILD_PROVENANCE_AND_DREAMFORGE_LINEAGE.md) |
| Previous chapters reused correction labels such as `C-03` | Correction references require chapter qualification: `(chapter, C-label)`; do not silently rewrite historical headings | [18 — correction index](18_ORIGINAL_QUIRKS_LATENT_FEATURES_AND_LEGACY_DATA.md) |

These are examples of **reconciled explanation/ownership questions**, not confirmation that a full adversarial contradiction scan across every machine-verified claim is finished.

## 4. UNKNOWN register ownership and incomplete external evidence

Chapter [00](00_SCOPE_EVIDENCE_BASELINE_AND_OPEN_QUESTIONS.md) originally assigns **U-001..U-016**; Chapter [18](18_ORIGINAL_QUIRKS_LATENT_FEATURES_AND_LEGACY_DATA.md) classifies each of those sixteen against later proofs. Preserve those identifiers without casually renumbering them as new source evidence arrives.

The outstanding categories fall into:

- **Original authoring names and unused capacity:** NPC residual flags/bytes and directional fields; OBJECTS unused/legacy fields, pair-class labels and range units; RESOURCE type-5 payload labels; RES9 companion arrays; unused ANM repeat producer model.
- **Historical resource revisions:** LEV022 forty-cell cache provenance, original RES11/12 producer/executable, legacy family-C ID migration and RES1 reserved literal.
- **Third-party driver details:** ARIA GM1/GM2 inner BNK layout, optional hardware drivers, individual `AIL_SERVE_DRVR` callback rates and complete device-sensitive timing behavior.
- **Old-version media:** exact 1.0/1.1/1.2/1.2a patch deltas, authenticated alternative CD/diskette/regional builds, and reported `CINE13.ANM` older-version failure.

The prior recoveries of AIL/32 period units and 1.2a executable identity are **partial resolutions of some uncertainty**, not evidence that every historical query is answered. Before closing this audit, create an explicit disposition for each outstanding question: *PROVED, BOUNDED TO AVAILABLE SOURCE, or REQUIRES UNSUPPLIED HISTORICAL/RUNTIME EVIDENCE*.

## 5. Evidence gaps versus defects in the project reconstruction

An absent source-level name is not a gameplay bug. An editor preserving a reserved field has not recovered its intent. Conversely, a C++ reconstruction correction (wrong argument slot, register-width alias, signed branch or interpreter operand count) may change executable semantics despite **100% byte-accounting** of the machine object. The CODE.1 and CODE.2 final semantic audits, and Chapter 18's `(chapter, C-label)` rules, remain controlling for those repairs.

Every new proposed correction must identify: prior claim; exact primary byte/instruction or archival record; affected files/addresses; revised semantic contract; test performed; and commit introducing the change. Do not write “fixed game bug” when only this project's explanatory prose was corrected.

## 6. Remaining final-audit gates

1. **Cross-chapter semantic scan:** compare matching address/field/record claims across Chapters 01–20; mark current canonical version and retire or correct contradictions rather than maintaining two active readings.
2. **UNKNOWN disposition:** enumerate U-001..U-016 and any later external-evidence requirements, with status and evidence owner; do not imply that a bounded historical unknown prevents lossless parsing.
3. **Original-source verification:** independently rerun validators on all canonical resource and DAT files **when those bytes are available**; retain earlier certified results as historical audit evidence where they are not currently mounted.
4. **Runtime comparison:** test game-state, original timing, event scripting and save/restore behavior against original execution where instrumentation is available. Log unexecuted tests as pending.
5. **Final manifest and branch safety:** preserve immutable baseline and backup, and document that the historical documentation branch has not rewritten original binaries, resource payloads, demo work or their source hashes.
6. **Publication decision:** explicitly distinguish “100% of available source documented,” “historical source UNKNOWN,” and “native execution fully integrated”; none is a substitute for the others.

**Checkpoint:** This is the **start of the final contradiction/UNKNOWN phase**, with structural/path integrity checked and several previously apparent conflicts classified. **No final 100% archival certification has yet been issued.**
