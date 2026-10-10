# Ravenloft: Strahd's Possession — source preservation and reconstruction

Faithful reconstruction of the original **Ravenloft: Strahd's Possession** game, including its executable code, data formats, and a native game engine. The goal is to recreate **the complete original game before modernization**. Current work is an in-progress reconstruction, **not** a semantically certified game or original-DOS-equivalent release.

## Start here

| Project area | Active documentation |
| --- | --- |
| Native executable and gameplay (`demo-v1`) | [Engine README](DemoV1/README.md), [original execution fidelity charter](DemoV1/ORIGINAL_EXECUTION_FIDELITY_CHARTER.md), [current integration contract](DemoV1/CONTINUOUS_GAME_INTEGRATION_CONTRACT_20261009.md) |
| Reconstructed executable code | [CODE.1](Code1/README.md) and [CODE.2](Code2/README.md) |
| Binary formats and data investigation | [DatFiles](DatFiles/) and [Tools](Tools/) |
| Development history | [Full preserved native development log](DemoV1/DEVELOPMENT_LOG_ARCHIVE_20261010.md) |
| Faithfulness and progress tracking | [Engineering subsystem report](DemoV1/PROGRESS_SUBSYSTEM_REPORT_20261009.md) |

**Repository branches are distinct workstreams.** `demo-v1` is the native game engine; `code1-decompilation` maintains executable reconstruction; `disassembler-foundation` maintains the disassembly tooling; `DnD-Classics` preserves original game data. Reconcile changes by original-byte evidence and narrow commits, **not by merging divergent branches wholesale**.

## Validation policy

- The original, version-identified `CODE.1`/`CODE.2` files and original DOS execution are authoritative. Reconstructed C++ may be **fully byte-accounted** while still containing semantic defects.
- Portable unit tests, compiler success, an executable launch, and native screen captures are necessary evidence, **not proofs of DOS behavior or pixel parity**.
- Native Windows Full CI workflow dispatch is **manual and user-authorized per run**. A source commit does not authorize CI.
- Do not check proprietary original game assets into development source, guess undocumented game behavior, or start modernization until the faithful recreation is complete.

Consult the [engine README](DemoV1/README.md) for the latest acceptance boundary, build instructions and research links. Historical `certified` terminology in earlier reports denotes an archival milestone, **not complete semantic or runtime verification**.
