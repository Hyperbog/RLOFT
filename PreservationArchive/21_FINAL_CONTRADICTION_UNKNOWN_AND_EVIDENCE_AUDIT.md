# Ravenloft: Strahd's Possession — Preservation Engineering Archive

## 21 — Final Contradiction, UNKNOWN, and Evidence Certification

**Status:** FINAL EVIDENCE AUDIT — 16 UNKNOWNs classified, heading collision corrected; final link/semantic gates remain OPEN  
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


## 7. Disposition of all 16 foundational UNKNOWNs

**State codes for this table:**

- **B — BOUNDED:** the supplied original binary/format proves the operative geometry or behavior; the missing name or historical motivation is not needed to read and faithfully preserve the data.
- **H — HISTORICAL/ALTERNATE ORIGINAL REQUIRED:** source/editor, old executable or old media needed to identify an earlier feature's intent, producer or revision.
- **D — DRIVER/DYNAMIC EVIDENCE REQUIRED:** middleware backend, additional configuration or instrumented real/emulated execution needed for specific behavior.
- **O — OPTIONAL THIRD-PARTY ARCHAEOLOGY:** original project scope defers this without declaring it valueless.

Codes describe the **remaining unproved aspect**, not the status of the entire format. A row marked B is not being claimed “solved” merely because a current-format consumer is known.

| Scope ID | Disposition | Proven current boundary | Specific missing evidence |
|---|---|---|---|
| **U-001** | B/H | NPC flags preserved; category-11 `0x20000/0x40000` weapon gate behavior measured statically | Original symbolic names and any alternate-build producer rules |
| **U-002** | B/H | NPC `+0x3B/+0x3C` zero in 83 records, no direct consumer proved | Original definition/editor or alternate revision with populated values |
| **U-003** | B/H | NPC directional-vector, animation and cell-influence arithmetic documented | Original design/editor nomenclature |
| **U-004** | B/H | Hand-pair classes 0/1/2 numeric behavior; class 2 paired-hand-exclusive | Source enum names for classes 0/1 |
| **U-005** | B/H | OBJECTS `+0x2C` zero across 336 records, no current consumer | Original authoring purpose / another revision |
| **U-006** | B/H | OBJECTS range bytes, scene influence and duplicate item mappings encoded and preserved | Physical unit names and object-table historical changes |
| **U-007** | B/H | LEVELS/LEV geometry, legacy `0x54→0x58`, cell `0x2000`, 40 saved-cache mismatches bounded | Editor/revision or runtime trace explaining legacy cache provenance |
| **U-008** | B/H | SAVE fixed/variable fields, status low bits, opaque character `+0x1BA` carried losslessly | Producer for residual status/auxiliary words or original source names |
| **U-009** | B/H | Second RES9 word bank persisted; supplied CODE.2 never consumes it | Editor/older executable producer or consumer |
| **U-010** | B/H | Two RES1 terminal raw `0x00000060` records and empty neighbors preserved | Historical portrait resource producer/significance |
| **U-011** | H | RES11/RES12 pre-split families and RES12→RES9 migration demonstrated | Exact producer/editor/older executable, hash-matched |
| **U-012** | B/H | Type-5 EH loader contract reconstructed; no shipped type-5 resource | Type-5 example or authoring source naming payload fields |
| **U-013** | B/H | ANM packet-4 seeking/repetition machine behavior reconstructed; zero shipped instances | Actual authored packet or producer format |
| **U-014** | O/D | GM1/GM2 identified as ARIA bank material, bytes retained | Driver/source or documented ARIA inner-bank specification |
| **U-015** | D | SOUND.CFG layout and AIL/32 `svc_rate` Hz/microseconds contract identified | Driver descriptor values, numeric config labels, real callback and error path traces |
| **U-016** | O | Optional installer/18 audio drivers separated from core game logic | Targeted driver decompilation only where a substantive original ABI question remains |

**Reconciliation:** all **16/16 IDs** from Chapter 00 are now explicitly disposed. The evidence improved since their first recording (especially U-001, U-007, U-011, U-015), but the residual historical/source-name portions have **not** been relabeled as proved. This is an accounting closure, **not** a claim of recovering every original author intent. Chapter [18](18_ORIGINAL_QUIRKS_LATENT_FEATURES_AND_LEGACY_DATA.md) remains the detailed quirk/source preservation ledger.

## 8. Finding A-14.1: duplicate section identity in the event/wordcode chapter

A current source-and-document scan identified a **real editorial conflict** in [Chapter 14](14_EVENT_WORDCODE_DIALOGUE_AND_STORY_STATE.md):

- §§1–75 originally progressed normally.
- A later **independent content block** restarted its section count at §51 and continued through §114.
- **25 pairs** of distinct topics were labeled §51–§75, making reference by numeric section ambiguous.

This is **not duplicate source data or a bug in CODE.2**. Neither content block was deleted. On 2026-10-07, correction **A-14.1** repaired the second block's section numbering by adding 25: later §51→current §76, …, later §114→current §139. The original first §§1–75 remain unchanged. Original correction labels `C-28..C-32`, original byte addresses, images and citations were left intact.

A cross-document citation in Chapter 18's **Q-08** formerly pointed to ambiguous Chapter-14 §77 and now points to **§102**, the section that describes the reserved LEVELS event-11 dispatcher. This is tracked under the Chapter-18 rule requiring chapter-qualified correction references, not silent rewrite of past commit history.

**Verification:** the corrected Chapter 14 has **139/139 sequential unique numbered sections**, and a scan of all **22** archive chapters (00–21, before this new text) found **no second chapter with duplicate or out-of-order numbered sections**. Existing intra-document prose references and external commit-era hyperlinks may still need citation-migration treatment if new ones surface; the explicit formula above remains available.

## 9. Fresh original-file witness check, 35/35 PASS

The project-mount originals were read independently from the documentation and tested against hashes and structure fields in Chapter 19:

```text
9/9 original file sizes and SHA-256:
  CODE.1 / CODE.2 / INFO.DAT / LEVELS.DAT / LEV022.DAT
  OBJECTS.DAT / NPCS.DAT / SAVE.DAT / SINTAB

CODE.2 embedded "Ver 1.2a" byte string
CODE.1 and CODE.2 published 1.2a reference MD5 matches
Corrected INFO.DAT MD5 and exact 13-character selector + 2-byte path
CODE.1 / CODE.2 common bound-loader prefix to 0x27BF0
OBJECTS 336 × 0x41 and quiver ID 329/selector 62
NPCS 83 × 0x45 and GHOUL LORD ID 18
LEVELS count 32 and strictly increasing in-file record offsets
SAVE 0x2448 fixture, level 22 and clock values 1899 / 14:00 / 1735
SAVE zero live transient records and four verified occupancy status words
SINTAB full zero trailer
SAVE 32-byte (256-bit) persistent event-flag block

TOTAL: 35/35 original-byte assertions passed
```

The checks establish these fields for **the nine available original binary inputs**. They **do not rerun** the independent 33-resource RES/ANM audit, driver hardware timing, native game integration or old-release diff tests. Time stamps on the mounted files are runtime upload metadata, **not** trusted 1994 build evidence.

## 10. Current contradiction disposition and next gates

Cross-chapter comparison has now examined: LE executable geometry and object origin, save/CHARS image sizes, fixed versus variable SAVE offsets, original status-word bit domains, script opcode/fixed-slot distinctions, RES bank-local IDs, runtime-vs-legacy archive selection, rendering microcode dependencies, game clock ownership, and release/middleware version boundaries.

**Confirmed new problem:** the Chapter-14 section-number collision, now repaired with a migration record. **No additional unresolved contradiction in those selected canonical field families has been established by this pass.** This is **not** proof that every sentence or example in Chapters 00–20 has been adversarially reverified instruction-by-instruction.

**Still required before a defensible final publication certificate:**

1. Verify the complete current relative-link manifest (including the new Chapter 21) and check dangling source-path/section references; treat public remote-link uptime as a separate matter.
2. Complete a field-level cross-reference of *all* previously recorded correction entries and the surviving historical UNKNOWN register; ensure original-bit identities never collide across unrelated owner types.
3. Produce a canonical file/edition integrity manifest that distinguishes currently-mounted originals from previously audited 33 resource files and unavailable third-party driver binaries.
4. Decide and document explicit gates for required native regressions and unsupplied original-media/runtime timing research, without pretending to have executed them.
5. Only then issue a scoped static documentation completeness decision. Do not equate it with 100% native game behavior testing or 100% historical provenance recovery.



## 11. Post-correction repository integrity: complete publication-path gate

A new tree/Markdown pass after correction A-14.1 read **all 22 numbered chapters (00–21)** and checked the currently published headings and all relative Markdown link targets:

```text
numbered chapter documents       22
missing numbered chapter IDs      0
chapters with duplicate/out-of-order section numbers   0
numbered Chapter-14 sections     139, strictly sequential and unique
relative Markdown links          100
broken relative target paths       0
Markdown relative #anchor URLs     0
```

The prior audit counted 86 relative links across Chapters 00–20; Chapter 21 adds fourteen links, accounting for the new full total **100**. Remote external/commit-pinned hyperlinks were **not network-probed one by one**. Markdown source paths resolving does not prove that the cited prose or external original data is correct; that remains an independent semantic/evidence check.

The correction preserved all Chapter-14 paragraphs and C-labels, altered **64** later `##` section numbers, and updated Chapter-18 Q-08 from §77 to the corrected §102. The historical migration note is embedded at the top of Chapter 14 and has a stable short label **A-14.1**.

## 12. Repository delta and frozen-source separation

An authenticated GitHub comparison of `373c7f10e6fc0f29c1e805125ef048150fcc7ba7...code1-decompilation`, captured immediately before the final documentation amendment, returned **89 commits ahead** and **43 changed tracked paths**. The commit-ahead count is a historical snapshot that increases with each subsequent documentation commit; the affected-path inventory below remains the comparison's result:

| Path group | Changed paths | What actually changed |
|---|---:|---|
| `PreservationArchive/` | 22 | New numbered documentary chapters `00..21` |
| `DatFiles/` | 12 | 11 previous Markdown audit/reference files modified; 1 new Markdown workflow note |
| `Runtime/` | 1 | `CODE_OBJECT_MAP.md` documentation correction |
| `TechnicalArchive/` | 8 | Former superseded docs removed from the active documentation branch |
| `Code1/`, `Code2/`, `Resources/`, `Tools/` | **0** | No tracked changes in these primary reconstruction/asset-extraction areas |

**Important nuance:** the original binary fixture inputs supplied in the present workspace are separate from repository Markdown. The file comparison excludes silent original-binary rewrites in the tracked Git tree (none are listed); the local 9/9 SHA-256 and 35/35 byte/structure checks independently defend the fixtures tested here. Removing obsolete `TechnicalArchive/` **from the active documentation branch** is an intentional prior branch-scope decision, not erasure of the frozen history.

The branch `preservation-backup-code1-2026-10-07` still points to exact frozen commit **`373c7f10e6fc0f29c1e805125ef048150fcc7ba7`**. The separate `demo-v1` branch points to **`18dbc63e867b04043c63b3723f45dcc6d22cbc3d`** at this audit checkpoint; no changes were made to either branch in this documentation pass.

## 13. Scope-limited publication state and unresolved test responsibility

The **structural evidence and document-navigation audits now pass**: 22/22 chapter IDs, all present section sequences, 100/100 internal relative file links, complete explicit dispositions for U-001..U-016, and the 35/35 available-file verification suite. Original system architecture, field dimensions and major cross-system ownership relations have been compared, with one new editorial contradiction repaired.

**Do not claim universal original-program verification from these figures.** Remaining distinct gates:

1. **Source-semantic review depth:** examine every chapter's specific disputed arithmetic/control-flow corrections and permanent record-field aliases against its controlling original instruction range where a new contradiction is suspected; the current scan checked major shared-field families, not literally every prose assertion.
2. **Full original resource replay:** current workspace lacks independently mounted bytes for the other 32 resource corpus files; prior certified decoder audits remain the valid recorded workstream evidence, not fresh checks in this audit.
3. **Hardware/driver observations:** actual sound-driver descriptor frequencies, PIT/IRQ timing, transient initialization failures and full runtime frame trace are still unmeasured. Chapter 17 describes the static contract and keeps this external gate open.
4. **Historical edition comparison:** exact pre-1.2a code differences, publisher CD mastering, alternate regional/platform source, and original DreamForge authoring formats require unsupplied originals.
5. **Faithful native implementation:** executable gameplay integration, graphics/audio/video equivalence, full save/restore replay and optional extension security tests belong to the separate port/demo workstream.

**Current decision: FINAL CERTIFICATION NOT YET ISSUED.** The archive has reached a strong *evidence-accounted and structurally consistent* checkpoint. The final documentation milestone must state exactly what is complete and what is bounded, not pretend that unavailable original historical media or hardware traces were produced.

**Next:** complete the last semantic correction/alias crosswalk and explicitly decide whether the remaining gaps are permitted as bounded historical/runtime UNKNOWNs under Chapter 00's archival completeness definition. If the check succeeds, issue a scoped documentation completion certificate; keep native-port completion a separate determination.
