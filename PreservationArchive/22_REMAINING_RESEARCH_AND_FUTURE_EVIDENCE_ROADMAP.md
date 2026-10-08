# Ravenloft: Strahd's Possession — Preservation Engineering Archive

## 22 — Every Remaining Research Strand: Master Evidence and Execution Backlog

**Status:** OPEN FORWARD-LOOKING REGISTER — historical static 1.2a certification remains COMPLETE.  
**Working branch:** `code1-decompilation` · **Immutable starting reference:** `373c7f10e6fc0f29c1e805125ef048150fcc7ba7`.  
**Coverage:** all *currently recorded* historical UNKNOWNs, deferred primary-source research, dormant/legacy archaeology, and unexecuted native/runtime validation obligations from Chapters 00–21 and the certified Resources/Runtime audits.

This chapter is intentionally separate from the [completed 1.2a static documentation certificate](FINAL_STATIC_DOCUMENTATION_CERTIFICATION.md). It does **not** revert that certification, imply these tasks are prerequisites to parsing the supplied original game, or call an unimplemented native engine “complete.” “Every last strand” means every remaining strand **identifiable in current project evidence**; new original discoveries can reveal new tasks.

## 1. Research classification and status model

**Priority:** P0 = acceptance work essential to a faithful *native port*; P1 = targeted research or reproducible methodology; P2 = optional historical archaeology, dependent on missing originals or testimony. P0 does not reopen the completed **static** archive.

**Evidence requirement:** H = new historical/media/authoring evidence; I = specific additional static investigation or tooling; D = original DOS/hardware/emulator observation; T = native and/or original-executable regression test. A task requiring H may properly remain **BOUNDED / SOURCE UNAVAILABLE**; do not invent period source names or fabricate unused shipped assets.

**Work status:** Every entry below begins **OPEN** or **EVIDENCE DEPENDENT**, even if its file layout is already documented. Mark PROVED only after a source-specific witness/test and repository commit. Keep newly identified unknowns as new R-IDs rather than renumbering older ones.

## 2. Work that is already finished, not part of this backlog

The 22 chapters 00–21 and the main [resource audit](../Resources/FINAL_RESOURCE_AUDIT.md) account for the shipped 1.2a CODE/DAT/RES/SINTAB evidence and bounded historical ambiguities. All 33 original resource/support SHA-256 identities, 13 RES and 13 ANM outer stream grammars, original ROM-like compiled render resources, and the prior **11,174** certified LZSS source streams have already been checked. The 1.1a(F) and 1.2(F) floppy updater executable replacement members were decompressed and SHA-256 verified; 1.1 `CODE.1` and `R.EXE` match the supplied 1.2a bytes. All 16 A32 driver descriptor/service export pairs were read and their nominal callback requests established. No new task should reclassify these **completed static findings** as mysteries merely because physical DOS gameplay has not been measured.

## 3. The 91 separately actionable research tasks

Each row is a distinct question or regression deliverable with a verifiable end state. Refer to its R-ID when assigning work or recording a new discovery.

## 4. Release archaeology and provenance

**Source:** [Ch.19](19_RELEASE_MEDIA_BUILD_PROVENANCE_AND_DREAMFORGE_LINEAGE.md)

| Task | Priority / evidence | Unfinished strand | Completion criterion |
|---|---|---|---|
| **R-001** | P2 / H | Authenticated 1.0 originals | Locate legally accessible unpatched original media; hash each CODE/data/resource file and record pressing and version strings. |
| **R-002** | P2 / H | CD-specific v1.1/v1.2 patches | Obtain `rlftcd11.zip` and `rlftcd12.zip` from authenticated sources; decode and verify their binaries and CD-only differences. |
| **R-003** | P1 / I | 1.1a(F)→1.2(F)→1.2a binary diff | Align verified CODE.2 LE objects, relocations and control-flow graphs; report byte, function and behavioral deltas. |
| **R-004** | P2 / H | GOG/Steam/Masterpiece checksums | Obtain authorized product payloads and compare CODE, image, resources and launch settings; never equate versions from labels alone. |
| **R-005** | P2 / H | CINE13 pre-1.2a incident | Reproduce the earlier-build invocation and prove its exact caller and 1.2a change, without inventing an absent CINE13 asset. |
| **R-006** | P2 / H | Retail mastering and archive timestamps | Correlate primary CD/diskette mastering evidence with RES11/12 legacy and active split resource timestamps. |
| **R-007** | P2 / H | Regional/FM Towns/PC-98 editions | Obtain authenticated other-platform editions and document per-file, encoding, rendering and script differences. |
| **R-008** | P2 / H | 1997 reprints and compilations | Map compilation pressings and installer revisions to exact executable/resource hashes and publisher release history. |
| **R-009** | P2 / H | DreamForge authoring software and sister titles | Find original editors/exporters, source or MAP data; only promote cross-game structural correspondences with byte evidence. |
| **R-010** | P2 / H | Publisher and developer archival records | Source SSI support notices, manuals, contemporary credits and developer testimony; label evidence class and reconcile with binaries. |

## 5. CODE startup, toolchain, supervisor and memory

**Source:** [Ch.01](01_EXECUTABLE_CONTAINER_AND_MODULE_CYCLE.md), [Ch.02](02_PROTECTED_MODE_STARTUP_AND_MEMORY_RUNTIME.md)

| Task | Priority / evidence | Unfinished strand | Completion criterion |
|---|---|---|---|
| **R-011** | P2 / H | Compiler/linker exact revision | Pin the application Watcom compiler/linker from matching library/runtime objects or original build materials, independent of DOS/4GW 1.96. |
| **R-012** | P2 / H | CODE.1 0x531F original compatibility symbol | Recover precise version-matched Watcom source designation while preserving known numeric behavior. |
| **R-013** | P2 / D | Non-Rational extender branches | Test dormant Phar Lap/Intel CodeBuilder/X32VM paths with faithful historical environments; document real failures and conditions. |
| **R-014** | P1 / D | Startup/teardown error branches | Record original selection, heap/handle allocation, missing-file, child-status and driver-failure behavior in a controlled DOS trace. |
| **R-015** | P0 / T | R.EXE/CODE.1/CODE.2/CHARS lifecycle | Verify return codes 10/12, CHARS 0x920 bytes, restart semantics and temporary-file ownership in original↔native state tests. |
| **R-016** | P1 / T | LE relocations, selectors and heap edge cases | Regression-test signed widths, selector conversion, low-memory arenas, pointer wrap and ordered frees. |
| **R-017** | P2 / H | Original C/C++ source module names | Locate authenticated DreamForge symbols, maps or developer notes; keep reconstructed C++ names clearly modern. |

## 6. RES, ANM, graphics and authoring formats

**Source:** [Ch.03](03_RESOURCE_ARCHIVES_CACHE_AND_RESOLUTION.md), [Ch.04](04_VGA_RENDERING_PIPELINE.md), [Resources audit](../Resources/FINAL_RESOURCE_AUDIT.md)

| Task | Priority / evidence | Unfinished strand | Completion criterion |
|---|---|---|---|
| **R-018** | P2 / H | RES11/12 original producer/editor | Find the authoring executable or earlier CODE revision that actually consumed pre-split composite resources. |
| **R-019** | P2 / H | RES9/12 second wordcode array | Identify authentic authoring producer/consumer for equal-length companion words; preserve otherwise. |
| **R-020** | P2 / H | RES1 raw 0x00000060 face marker | Prove meaning of two terminal non-EH sentinels and neighboring zero-length slots. |
| **R-021** | P2 / H | EH type-5 authoring semantics | Locate original specimen or source for never-shipped loader type 5's 28-byte header and tail. |
| **R-022** | P2 / H | Type-1 spare bytes/compression-1 producer | Recover true names for header bytes +0x05/+0x07 and authentic compressed method-1 source; synthetic tests remain synthetic. |
| **R-023** | P2 / H | ANM type-4 repeat author | Locate real repeat/seek packet or exporter; confirm intended production usage without claiming any existing ANM uses it. |
| **R-024** | P2 / H | SINTAB rasterizer code generator | Find source/tool used for 127 per-scale x86 programs, 129-selector tables and their generation options. |
| **R-025** | P2 / H | RES3/RES4 art and palette atlas | Identify real artwork and texture labels; map backdrops, art variants and palette conversions to authenticated source material. |
| **R-026** | P2 / H | Original graphics and compositor terminology | Identify source names for VGA configurations, type-7 descriptors, environment profiles and opcodes 0x10/0x11/0x23/0x25/0x26. |
| **R-027** | P1 / T | Specialized decoder regression harness | Re-run type-1/2/6/7 and corrected LZSS decompression, sign extension, 18 undefined dictionary bytes and 11,174 shipped streams in versioned CI. |
| **R-028** | P1 / T | Pixel-level original renderer parity | Capture reference VGA planes, integer transforms, clipping, animations, composite order, palettes and scaled microcode output. |

## 7. World, LEVELS, automap and environment state

**Source:** [Ch.05](05_WORLD_LEVEL_STATE_AND_EVENTS.md), [Ch.06](06_AUTOMAP_NAVIGATION_AND_EXPORT.md)

| Task | Priority / evidence | Unfinished strand | Completion criterion |
|---|---|---|---|
| **R-029** | P2 / H | LEVELS cell bit 0x2000 | Seek actual editor/other-version bit-specific producer or consumer; do not confuse with character status 0x2000. |
| **R-030** | P2 / H | LEV022 forty-cell cache difference | Trace an authenticated old save/editor recomputation that explains discrepancy without changing source cache bytes. |
| **R-031** | P2 / H | Family-C 0x54→0x58 legacy item migration | Recover object table history and editor/old-code origin of loader alias. |
| **R-032** | P2 / H | 10-byte geometry records and door mode 2 | Resolve original record-kind names, raw bytes and exact source meaning of alternate barrier mode. |
| **R-033** | P2 / H | World shading/prepass/scene class nomenclature | Determine editor terms for 0x0200, 0xC000, family A/B/C and four RES3 source classes. |
| **R-034** | P2 / H | Automap cache and special glyph names | Identify cache classes 1–5, hidden bit 0x0800, 0xE9/0xB0/0x09, 64-entry style table and constant 0xB5B1. |
| **R-035** | P2 / H | Automap annotations/export quirks | Research original browser/annotation globals and AUTO tens/ones digit sequencing intent. |
| **R-036** | P0 / T | World-state/automap live regression | Exercise 32 levels, doors, 10-byte records, world-family capacities, fog/visibility, notes, map export and cache persistence. |

## 8. OBJECTS, NPC, AI, inventory and combat

**Source:** [Ch.10](10_OBJECT_INVENTORY_EQUIPMENT_AND_CONTAINERS.md), [Ch.11](11_NPC_CREATURE_AI_AND_LIVE_ACTOR_MODEL.md), [Ch.12](12_COMBAT_ATTACK_DAMAGE_SAVES_AND_PROGRESSION.md)

| Task | Priority / evidence | Unfinished strand | Completion criterion |
|---|---|---|---|
| **R-037** | P2 / H | OBJECTS +0x2C reserved byte | Find authentic nonzero producer or alternate consumer; preserve all 336 existing bytes. |
| **R-038** | P2 / H | Object range/unit and state field names | Resolve scene_influence_step, range real-world units, item category taxonomy and container bitfield design names. |
| **R-039** | P2 / H | Duplicate object entries and QUIVER 62/61 | Trace editor/database revisions and intended quiver selector mismatch; never silently coerce selector. |
| **R-040** | P2 / H | Hand-pair class 0/1 original labels | Recover source taxonomy without changing known class-2 exclusivity. |
| **R-041** | P2 / H | NPC unknown main/residual bits | Find symbolic identities for six main behavior bits, category-11 0x20000/0x40000 and +0x39 bit quartet. |
| **R-042** | P2 / H | NPC +0x3B/3C and AI/direction labels | Seek old source/producer for unconsumed bytes, directional_offset, animation, scene step and path AI flags. |
| **R-043** | P2 / H | Special attack/combat table labels | Recover type-15 NPC attack branch name, saves family A/B, burden classes and progression table terminology. |
| **R-044** | P2 / D | Attack speed to physical-time unit | Measure attack duration units in original timed gameplay, not from nominal AIL callback rate. |
| **R-045** | P0 / T | NPC, combat and inventory traces | Compare spawning, pathing, resistance, attacks, signed rolls, equipment, containers, item throwing, charge counters and quiver edge cases. |

## 9. SAVE, characters, status and magic

**Source:** [Ch.08](08_SAVE_PARTY_AND_PERSISTENCE_ARCHITECTURE.md), [Ch.09](09_PARTY_CHARACTER_GAMEPLAY_MODEL.md), [Ch.13](13_SPELL_MAGIC_EFFECT_AND_PREPARATION_SYSTEM.md)

| Task | Priority / evidence | Unfinished strand | Completion criterion |
|---|---|---|---|
| **R-046** | P2 / H | Character low status bits and 0x2000 | Recover producers and player-facing status names for 0x0001..0x0020 and action-blocking 0x2000. |
| **R-047** | P2 / H | Character +0x1BA and unused 0x248 tail | Find alternate revision or source owner of residual dword/bytes while preserving original party image. |
| **R-048** | P2 / H | Three timed slots and SAVE prefix unknown fields | Name post-effect paired timed counters and SAVE+0x62..0x6B words from primary producer/consumer evidence. |
| **R-049** | P2 / H | Special TEMP slot==-1 save path | Establish purpose of five-byte TEMP label and whether other working-file classes were shipped. |
| **R-050** | P2 / H | Class-3 effect types 5/7 and packed state | Locate actual producer/author names for types 5/7, movement bits, context and special cleanup. |
| **R-051** | P2 / H | Original spell and priest action source enums | Recover names for two preparation matrices, detection counters, effect types, priest special actions and level-loss table rationale. |
| **R-052** | P2 / H | Other-version status/effect capacity | Seek real authored cases of unused branches; avoid treating synthetic test data as shipped spells. |
| **R-053** | P0 / T | Mutable save/load byte-compatibility | Test variable effect counts, SAVE/WORK/TEMP files, 10 roster slots, status flags, clock domains, cancellation, restart and raw-unknown preservation. |
| **R-054** | P0 / T | Spell/character state regression | Replay casting, durations, priest level loss, possession, healing, death, revival, scrolls/wands, temporary hands and class-3 cleanup. |

## 10. Events, dialogue, UI and cinemas

**Source:** [Ch.07](07_INPUT_UI_MENU_AND_INVENTORY_ARCHITECTURE.md), [Ch.14](14_EVENT_WORDCODE_DIALOGUE_AND_STORY_STATE.md), [Ch.16](16_ANM_CINEMATIC_ENGINE_AND_TIMELINE.md)

| Task | Priority / evidence | Unfinished strand | Completion criterion |
|---|---|---|---|
| **R-055** | P2 / H | External numbered interactive files | Recover legitimate numbered assets (e.g. D:\07) and certify their grammar separately from RES9 scripts. |
| **R-056** | P2 / H | Dialog predicates and suppression name map | Find original names for condition values, 0xFFFF/0x29 suppression operands and source compositor operations. |
| **R-057** | P2 / H | Original two-VM opcode names | Recover editor grammar for LEVELS event commands, active RES9 and RES12's older inline-template operand. |
| **R-058** | P2 / H | UI globals, pointer bitflags and analog path | Identify authentic input source nomenclature, command rectangle tables and inventory hit descriptors. |
| **R-059** | P2 / H | Menu capture/poll design rationale | Discover whether unusual focus and click behavior is intended or incidental without changing machine-proven event order. |
| **R-060** | P0 / T | Event-script/dialog complete replay | Test 50×26-byte slot semantics, 42 RES9 VM opcodes, choice targets, T, persistent/local flags and special event 11 handlers. |
| **R-061** | P0 / T | UI pointer/keyboard/HUD acceptance | Capture original hit rectangles, mouse/key queue, portraits, menu, inventory and reordered party state. |
| **R-062** | P0 / T | ANM playback/event and timing parity | Test packet order, abort status, subtitle/audio queue, CINE10/CINE11 predicates, waits and a clearly synthetic chunk-4 case. |
| **R-063** | P2 / H | Cinematic producer/story names | Recover original event, packet and scene authoring terminology only from original assets/editor evidence. |

## 11. Music, SFX, AIL and physical timing

**Source:** [Ch.15](15_AUDIO_MUSIC_SFX_AND_DRIVER_INTERFACE.md), [Ch.17](17_TIMING_INTERRUPTS_AND_FRAME_CADENCE.md), [optional driver triage](../Runtime/OPTIONAL_EXECUTABLE_DRIVER_TRIAGE.md)

| Task | Priority / evidence | Unfinished strand | Completion criterion |
|---|---|---|---|
| **R-064** | P1 / I | SINSTALL field/writer mapping | Recover exact names of SOUND.CFG numeric fields and trailer selectors by tracing the installer against MT32/SC55/SB16 samples. |
| **R-065** | P1 / D | Observed IRQ/PIT and cinematic cadence | Capture port-0x40 programming, 0xF58C/0xA7E4, chained IRQ8, AIL callbacks and real frame timestamps under specified hardware. |
| **R-066** | P1 / D | Driver initialization/failure status | Instrument original AIL_INIT_DRVR, 0x67 service, callback register/start/stop, error returns and exceptional frequency requests. |
| **R-067** | P2 / H | GUS drivers/PATCHES.INI/LOADPATS execution | Trace true Gravis loader call path and the two differently structured GUS vendor backends. |
| **R-068** | P2 / H | ARIA bank interior grammar | Derive GM1/GM2 structure via A32ARXM/authoritative ARIA source and prove record fields with binaries. |
| **R-069** | P2 / H | Optional A32/GUS hardware internals | Decompile only device ABI behavior not resolved at CODE boundary, including DMA/IRQ, card probing, FM/MPU/GUS synthesis. |
| **R-070** | P2 / H | RES5 alternate bank and original audio labels | Prove driver-dependent selection and named original MIDI/SFX API functions rather than assuming all music devices use one bank. |
| **R-071** | P1 / T | Music/SFX/voice scheduling parity | Reproduce XMIDI/VOC/SAMPLE.AD, NPC audio, interaction voices, priority, stop/start, audio device substitution and cinematic mix ordering. |

## 12. Faithful native-port integration and acceptance tests

**Source:** [Ch.20](20_MODDER_PROGRAMMER_REFERENCE.md), [Ch.18](18_ORIGINAL_QUIRKS_LATENT_FEATURES_AND_LEGACY_DATA.md)

| Task | Priority / evidence | Unfinished strand | Completion criterion |
|---|---|---|---|
| **R-072** | P0 / T | Compilable source-to-port traceability | Map original routines, offsets, data structure owners and associated tests to integrated native modules. |
| **R-073** | P0 / T | Representative game-state campaign replay | Capture deterministic original scenarios spanning character creation, quests, combat, saves, cinematic endings and restarts. |
| **R-074** | P0 / T | Safe lossless legacy asset import/export | Read DAT/RES/ANM/T/SINTAB byte-exactly, retaining unknowns, signed fields, offsets and x86 SINTAB as untrusted source. |
| **R-075** | P0 / T | Actor/AI/combat/spell acceptance | Run all canonical NPC/player and magic suites with original numeric quirks, not intuitive rulebook approximations. |
| **R-076** | P0 / T | World/raycast/automap graphics acceptance | Compare pixel planes, color, occlusion, projection, automap cache and doors across the original fixture set. |
| **R-077** | P0 / T | Module/SAVE/CHARS process compatibility | Prove R.EXE status handoff, transient effect counts, per-field save writes, load errors and lifecycle. |
| **R-078** | P0 / T | RES9/LEVELS/ANM story and cinematic integration | Verify scripts, dialogue, text, interactions, story flags, SFX and cinematic branch order. |
| **R-079** | P0 / T | Host input/audio/UI faithful behavior | Keep original logical controls/panel selection, event order and queued audio semantics despite a modern implementation. |
| **R-080** | P1 / T | Negative/error/edge-case regression suite | Exercise invalid indices, corrupt payloads, signed extrema, LZSS dictionary tail, clock overflow and allocation failures without altering originals. |
| **R-081** | P1 / T | Reproducible original-versus-native trace capture | Record state snapshots, visual hashes and scheduled audio events with fixed emulator config and input sequences. |
| **R-082** | P1 / T | Modder editor change/report pipeline | Implement explicit byte diffs, foreign ID validation, safe repairs and versioned native-only extension policy. |
| **R-083** | P1 / T | Freeze and certify faithful port before remaster | Publish separate port architecture/source-to-native test map and freeze tested revision before modernization. |

## 13. Verification, archival governance and new research intake

**Source:** [Ch.00](00_SCOPE_EVIDENCE_BASELINE_AND_OPEN_QUESTIONS.md), [Ch.21](21_FINAL_CONTRADICTION_UNKNOWN_AND_EVIDENCE_AUDIT.md), [certificate](FINAL_STATIC_DOCUMENTATION_CERTIFICATION.md)

| Task | Priority / evidence | Unfinished strand | Completion criterion |
|---|---|---|---|
| **R-084** | P1 / I | Automated original-manifest/format regression CI | Pin hashes, original archive provenance, parser versions, complete RES/ANM/DAT fixtures and re-executable test logs. |
| **R-085** | P1 / I | Correction and document link audit CI | Track (chapter,C-label), 16 U-IDs, section heading uniqueness, the Chapter-14 old-number migration and relative/anchor links. |
| **R-086** | P1 / I | Concurrent tool versus documentation branch isolation | Reconcile four Tools/anm changes present on code1-decompilation without undoing other work or changing the frozen baseline. |
| **R-087** | P1 / I | Authentic intake chain of custody | Record medium owner/permission, pressing, filesystem date, regional variant, hash and parser output for each later artifact. |
| **R-088** | P2 / H | Cross-check community provenance reports | Reconcile patch listings, collector anecdotes, GOG metadata and real on-disc hashes; mark reports as documentary until proven. |
| **R-089** | P2 / H | Original source names, artwork attribution and design history | Maintain source-name versus reconstructed-name distinctions and credit developers and community researchers with primary support. |
| **R-090** | P1 / I | Amendments and certificate versioning | For each new proof, preserve prior wording and commit history, publish offset/hash witness and scope impact; avoid retroactively claiming all runtime work complete. |
| **R-091** | P1 / I | Rights-aware artifact distribution | Publish hashes, machine analysis and legally distributable comparisons, not proprietary commercial binaries or unlicensed middleware images. |

## 14. Cross-index: all 16 original UNKNOWN records

Original U-IDs remain owned by [Ch.00](00_SCOPE_EVIDENCE_BASELINE_AND_OPEN_QUESTIONS.md) and [Ch.18](18_ORIGINAL_QUIRKS_LATENT_FEATURES_AND_LEGACY_DATA.md). Their *still-unknown aspects* now have explicit master task IDs:

| Original ID | Outstanding facet | R-ID(s) |
|---|---|---|
| **U-001** | NPC flag producer/intent | R-041 |
| **U-002** | NPC +0x3B/+0x3C | R-042 |
| **U-003** | NPC direction and scene semantics | R-042 |
| **U-004** | Hand-pair classes 0/1 | R-040 |
| **U-005** | OBJECTS +0x2C | R-037 |
| **U-006** | OBJECTS units and duplicate records | R-038, R-039 |
| **U-007** | World cell, ID alias, cache | R-029, R-030, R-031 |
| **U-008** | SAVE status bits and opaque character fields | R-046, R-047 |
| **U-009** | Interaction script companion words | R-019 |
| **U-010** | RES1 reserved literal | R-020 |
| **U-011** | RES11/12 historical editor | R-018 |
| **U-012** | EH type 5 | R-021 |
| **U-013** | ANM repeat/seek producer | R-023 |
| **U-014** | ARIA GM banks | R-068 |
| **U-015** | Sound config and actual driver cadence | R-064, R-065, R-066 |
| **U-016** | Optional driver internals | R-067, R-069 |

This is **16/16 UNKNOWN disposition coverage**, not a claim that the underlying historical producers have been discovered. Several IDs refer to one task because their evidence would be recovered from the same absent editor/binary.

## 15. What evidence would actually move the needle?

| Material | Unblocks | Current boundary |
|---|---|---|
| Authorized unpatched 1.0 media, authenticated CD patch 1.1/1.2, regional/compilation originals | R-001..R-010 and older code/editor hypotheses | **Missing; optional** |
| Original DreamForge/SSI source symbols, editor packages, format notes or attributed developer testimony | Source names, reserved fields, old authoring lineage | **Missing; do not guess** |
| Instrumented original DOS execution with exact hardware emulation settings and device logs | Timing, failure modes, pixel/audio output, real attack-duration units | **Not executed end-to-end** |
| Original numbered interaction files and authentic unused packet/type specimens | Extra loader grammar, dormant types, RES-era producers | **Not established for shipped corpus** |
| Original CODE/DAT/RES archives, verified floppy update CODE images, 18 DLLs, three SOUND.CFG profiles | Repeated static checks and tests for currently documented 1.2a | **Already supplied and hash-known; do not request again** |
| Instrumented faithful native executable and test harness | Most P0 runtime parity and port regression tasks | **Independent implementation workstream** |

## 16. Recommended sequence and dependencies

1. **Stabilize fixtures and CI**: exact hash manifest; original structure readers; binary comparison scripts; immutable inputs. Pin test versions; keep the frozen backup untouched.
2. **Prioritize P0 native behavior** using already proven data: CODE module lifecycle, SAVE/CHARS, world, graphics, NPC/combat/spells, two script grammars, ANM, sound and UI. Capture deterministic state transitions rather than relying only on casual gameplay.
3. **Instrument original DOS behavior where necessary**: IRQ/PIT divisor, video/audio packets, state/clock snapshots and device/driver error branches. Do not infer FPS from an A32 `svc_rate`.
4. **Compare alternate releases only after obtaining authentic media**: alignment uses physical object pages and relocations; later floppy CODE offsets are not interchangeable with current 1.2a offsets.
5. **Pursue optional historical intent last**: old designer labels, dormant assets, ARIA/GUS archaeology, regional history and release chronology. Record a bounded negative search result if originals are unavailable.
6. **Freeze and document the native faithful implementation** before adding explicit remaster extensions; that future implementation certificate does not belong to this historical static research certificate.

## 17. Reproducible task closure record

For each R-ID use this minimal record:

```text
Research ID / related U-ID:
Previously established original hash and file offsets:
New primary evidence and verified provenance:
Falsifiable claim and observed output:
Method: original static bytes | editor archive | DOS trace | port comparison
Byte-level or state-level regression fixtures and outcome:
Remaining uncertainty (if any):
Canonical chapter amendments and commit:
Status: OPEN / BLOCKED-ON-EVIDENCE / TEST-PENDING / BOUNDED / PROVED
```

**Do not** silently normalize unknown save bytes, misclassify old RES11/12 as active indexed resources, promote third-party anecdotes to binary proof, execute untrusted SINTAB code in a modern editor, or confuse the original-build certificate with game/native-port completion. If a future discovery contradicts a certified original claim, follow [Ch.00's correction policy](00_SCOPE_EVIDENCE_BASELINE_AND_OPEN_QUESTIONS.md).

## 18. Research register certification boundary

This register inventories **91 actionable strands** in **10 research areas**, including **16 P0 native acceptance**, **19 P1 targeted work**, and **56 P2 evidence-dependent history**. It cross-indexes all original 16 historical UNKNOWNs, and it distinguishes actual missing evidence from not-yet-performed tests. Its purpose is **traceability and honest scope**, not an unsupported universal claim that no future unknown could exist.

The existing certificate for Chapters 00–21 remains valid. Chapter 22 catalogs what follows after that static-documentation milestone and is a living research register, not a demand to retroactively reopen already certified file parsing.
