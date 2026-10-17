> **2026-10-17 organization:** canonical recompilation sources and tests are now maintained on the [`recomp` branch](https://github.com/Hyperbog/RLOFT/tree/recomp/Recomp); the default branch is for decompilation and shared research. See [Decomp/README.md](../Decomp/README.md). Historical recompilation links below target the dedicated branch.

# RLOFT project documentation index

**Start here for cross-workstream navigation.** **Owner update, 2026-10-13:** the player-facing deliverable is an enhanced native PC port; original DOS fidelity audits are optional research, not release blockers.  This index is intentionally short. **Every project's technical documents stay with their owning source directory/branch**; this is a directory of links, not a replacement for local evidence or a request to move it.

- [Current cross-workstream status](CURRENT_STATUS.md) — current verification boundary, not a chronological changelog.
- [Branch ownership and lifecycle](BRANCH_OWNERSHIP.md) — current active/review branches and recovery policy.
- [2026-10-11 branch cleanup audit](BRANCH_CLEANUP_AUDIT_20261011.md) — fourteen archived branch tips and obsolete workflow retirement.
- [Shared-code synchronization register](SHARED_SOURCE_REGISTER.md) — exact shared blobs, intentional drift and source-proof gate.
- [Original CODE.2 arena allocator, free/reuse and raw-size ABI](https://github.com/Hyperbog/RLOFT/blob/recomp/Recomp/CODE2_ORIGINAL_ARENA_CLOSURE_20261011.md) — 7/7 compiled source tests and exact original-byte proof.
- [Four CODE.2 original initializers through the game-entry boundary](https://github.com/Hyperbog/RLOFT/blob/recomp/Recomp/INITIALIZER_20608_INTEGRATION_20261011.md) — canonical source execution, original-byte witnesses and DOS allocator limitations.
- [Original CODE.2 initializer/shutdown indirect callback candidates](../ResearchInfra/CODE2_INDIRECT_CALLBACK_TABLES_20261011.md) — four initializer targets and partially file-backed `3A228` termination target.
- [Optional cross-project REA/Ghidra static analysis](../ResearchInfra/REA.md) — SHA-bound local inspection plans for all owning branches, authenticated LE raw-x86 windows, and private evidence handling (not DOS parity).
- [Original-machine CFG and AI work-packet integration](../ResearchInfra/ORIGINAL_MACHINE_CFG_20261011.md) — optional authenticated CODE originals, independent x86 blocks and source-call overlays.
- [Bulk original LE survey and DOSDis evidence cross-check](../ResearchInfra/BULK_CFG_DOSDIS_BRIDGE_20261011.md) — bounded candidate source-address coverage and fail-closed DOSDis schema-v1 bridge.
- [CODE.1 B01A overlapping-original-instruction correction](../ResearchInfra/CODE1_B01A_OVERLAP_CORRECTION_20261011.md) — authenticated alternate-entry STOSB, compiled regression and shared-source synchronization.
- [Documentation maintenance policy](DOCUMENTATION_POLICY.md) — retain original addresses, research and history in context.

## Project-specific authoritative documentation

| Workstream | Owns its documentation | Start with |
| --- | --- | --- |
| Native game engine | `demo-v1:DemoV1/` | [Native README](https://github.com/Hyperbog/RLOFT/blob/demo-v1/DemoV1/README.md), **[active enhanced PC port charter](https://github.com/Hyperbog/RLOFT/blob/demo-v1/DemoV1/ENHANCED_PC_PORT_CHARTER.md)**, [new release gates](https://github.com/Hyperbog/RLOFT/blob/demo-v1/DemoV1/RELEASE_GATES.json), [historical original charter](https://github.com/Hyperbog/RLOFT/blob/demo-v1/DemoV1/ORIGINAL_EXECUTION_FIDELITY_CHARTER.md) |
| Shared research infrastructure | `code1-decompilation:ResearchInfra/` | [Research index CLI, project registry and evidence rules](../ResearchInfra/README.md) |
| Canonical source recompilation | `recomp:Recomp/` | [Executable reconstruction and original LE loader](https://github.com/Hyperbog/RLOFT/blob/recomp/Recomp/README.md) |
| Decompilation / CODE.1 & CODE.2 reconstruction | `code1-decompilation:Decomp/`, `Code1/`, `Code2/` | [CODE.1](../Code1/README.md), [CODE.2](../Code2/README.md), [CODE.2 continuity](../Code2/CODE2_CONTINUITY_ADDENDUM.md) |
| Disassembler | `DosDis:Tools/Disassembler/` | [DOSDis README](https://github.com/Hyperbog/RLOFT/blob/DosDis/Tools/Disassembler/README.md), [independent validation policy](https://github.com/Hyperbog/RLOFT/blob/DosDis/Tools/Disassembler/docs/INDEPENDENT_VALIDATION_POLICY.md) |
| Asset extractors | `Ravenloft-Asset-Extractor:Tools/`; shared extraction code also on default | [Extractor project](https://github.com/Hyperbog/RLOFT/blob/Ravenloft-Asset-Extractor/Tools/anm/README.md), [default tools entry](../Tools/README.md) |
| Source media, immutable originals | `DnD-Classics:MASTER/` | [Preservation game tree](https://github.com/Hyperbog/RLOFT/tree/DnD-Classics/MASTER) |
| Original DAT/RES research | Directory-specific `DatFiles/`, `Resources/`, `Runtime/` | [DAT](../DatFiles/README.md), [Resources](../Resources/README.md), [Runtime](../Runtime/README.md) |
| Long-form preservation reports | Existing `PreservationArchive/` and historical records | [PreservationArchive directory](../PreservationArchive/) |

## Interpretation rules

1. **An original-byte analysis is evidence, not automatically a live gameplay feature**. Preserve original-file SHA, offsets, verification windows and unresolved edges.
2. **Historical certification/accounting is not semantic or DOS acceptance**. View original release provenance separately from the latest executable build.
3. **No wholesale cross-branch merge for housekeeping**. Sync only evidence-proven shared files by exact blob; do not rewrite game assets or subsystem archives.
4. **No new GitHub Windows job** is authorized merely by a documentation commit. The native build stays owner-authorized per run.
