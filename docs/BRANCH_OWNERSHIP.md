# Branch ownership, status and preservation boundaries

**Checked 2026-10-10.** Every branch has independent source, tests and build acceptance. This defines where changes belong; it does not authorize deletion, moving a project-specific document, rewriting original assets or merging long-diverged branches.

| Branch | Workstream / owner area | Treatment |
| --- | --- | --- |
| `code1-decompilation` | Default branch; source-owned `Code1/`, `Code2/`, `DatFiles/`, `Resources/`, preservation evidence | **Canonical decompilation**; cross-project documentation entry lives here |
| `demo-v1` | `DemoV1/` faithful native engine, tests and Win32 host | **Active native development**; [original-game charter](https://github.com/Hyperbog/RLOFT/blob/demo-v1/DemoV1/ORIGINAL_EXECUTION_FIDELITY_CHARTER.md) is binding |
| `disassembler-foundation` | `Tools/Disassembler/`: DOSDis .NET, WPF and external validation | **Active disassembler**; [its local docs](https://github.com/Hyperbog/RLOFT/blob/disassembler-foundation/Tools/Disassembler/README.md) remain authoritative |
| `extractor-wpf-fluent` | `Tools/`: WPF extractor, Python/ANM underlying formats | **Specialist implementation**; local docs stay in place |
| `DnD-Classics` | `MASTER/` original media | **Media-preservation branch**; do not rewrite game files, identities or history during housekeeping |
| `disassembler-g2-build-20261010` | Predeclared G2 candidate build tree | **Validation snapshot** with independent tool changes; keep exact evidence until promotion |
| `disassembler-g2-validation-20261010` | G2 workflow-validation variant | **Validation snapshot**; check existing CI and PR provenance before retirement |
| `preservation-backup-code1-2026-10-07` | Historical code/preservation snapshot | **Historical backup**, intentionally preserving older decomp source |

At the audited checkpoint, all eight branches reported `protected=false`. Review bot-commit workflows and collaboration permissions before changing that.

## Cross-branch correction protocol

1. Check exact live HEAD and target Git blob SHA, original-file version, relevant project-specific methodology and affected caller/consumer.
2. Synchronize **bounded source-verified shared changes** such as `Code1.cpp` or `Code2.cpp` into active live consumers. Verify matching Git blobs. Do not merge separate engine/disassembler/extractor trees wholesale.
3. For original media, historical backups or accuracy-validation snapshots, mark drift **intentional/pinned**, never rewrite a proven build checkout to equalize source tree hashes.
4. Record any shared source delta in [the source synchronization register](SHARED_SOURCE_REGISTER.md), including owner, witness, active consumers and remaining adoption.
5. Project-specific contracts, instruction witnesses, reports and tests **stay under their existing owning directory/branch**. The central [project index](INDEX.md) links to them rather than duplicating them.
6. No CI run, release rewrite, gameplay change, DOS parity claim, or branch deletion is authorized by docs synchronization alone.
7. Before retiring a branch, require active-owner approval, exact compare/merge review, and a recovery tag/ref.

**Promotion checklist:** original proof and version → corrected owning source → local regression → active-consumer inventory → exact blob comparison → targeted file sync → fresh HEAD check → record status → separate platform and original-DOS acceptance.
