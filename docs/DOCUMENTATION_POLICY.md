# Documentation stewardship — retain each project's technical docs in place

**Policy effective 2026-10-10.** The shared `docs/` folder is an index and cross-stream coordination area. It must **not** absorb or replace subsystem evidence, original-byte analyses, or historical research.

## Project-specific documentation that stays local

| Owns content | Retain in |
| --- | --- |
| Faithful native engine, original-game charter, Level22 evidence and continuity | `demo-v1:DemoV1/` |
| CODE.1/CODE.2 byte accounting, exact call witnesses and decomp errata | `code1-decompilation:Code1/` and `Code2/` |
| DOSDis architecture, command help, independent oracle/G2–G8 validations and .NET build evidence | `disassembler-foundation:Tools/Disassembler/` |
| Extractor source formats, WPF/Python user guide, application releases | `extractor-wpf-fluent:Tools/` |
| Resource data structures, runtime research and recovered archives | `DatFiles/`, `Resources/`, `Runtime/`, `PreservationArchive/` |
| Original untouched game data and naming | `DnD-Classics:MASTER/` |

**No mass move, auto-deduplication, destructive rewrite or wholesale renaming.** Any future relocation must first preserve existing URLs, relative links, source offsets, test references and other workstreams' citation paths. Dated original forensic evidence is valuable even when superseded: label the superseding conclusion and original version rather than erase history.

## One document for each cross-project role

| Purpose | Location | Maintenance |
| --- | --- | --- |
| Project landing | Repository root `README.md` on default branch | Brief scope and links; not a changelog |
| Workstream map | [`docs/INDEX.md`](INDEX.md) | Point to owner documents, not copy them |
| Overall evidence status | [`docs/CURRENT_STATUS.md`](CURRENT_STATUS.md) | Dated, concise, precise acceptance and no unverified completeness claims |
| Branch ownership | [`docs/BRANCH_OWNERSHIP.md`](BRANCH_OWNERSHIP.md) | Active, candidate, backup, preserved original data |
| Shared version drift | [`docs/SHARED_SOURCE_REGISTER.md`](SHARED_SOURCE_REGISTER.md) | Exact Git blobs and provenance |
| Workstream contracts and operating methods | **Existing local documents** | Must be read before relevant source changes |
| Detailed history | **Existing dated local reports and archived README** | Do not delete to make the README short |

For new technical findings, prefer the existing subsystem's evidence file or a distinct dated proof in the **same owner directory**. Update a short current-state entry instead of prepending historical status to a long charter or contract. Do not falsely convert authored/source tests to compiled Windows or original-DOS acceptance.

All housekeeping commits use `[skip ci]`. An automatically executed build, original-game-data redistribution, original DOS parity claim, Windows Full dispatch or branch deletion requires independent authorization and validation; documentation cleanup grants none.
