# RLOFT project documentation index

**Start here for cross-workstream navigation.** This index is intentionally short. **Every project's technical documents stay with their owning source directory/branch**; this is a directory of links, not a replacement for local evidence or a request to move it.

- [Current cross-workstream status](CURRENT_STATUS.md) — current verification boundary, not a chronological changelog.
- [Branch ownership and lifecycle](BRANCH_OWNERSHIP.md) — active, preservation, and review snapshots.
- [Shared-code synchronization register](SHARED_SOURCE_REGISTER.md) — exact shared blobs, intentional drift and source-proof gate.
- [Documentation maintenance policy](DOCUMENTATION_POLICY.md) — retain original addresses, research and history in context.

## Project-specific authoritative documentation

| Workstream | Owns its documentation | Start with |
| --- | --- | --- |
| Native game engine | `demo-v1:DemoV1/` | [Native README](https://github.com/Hyperbog/RLOFT/blob/demo-v1/DemoV1/README.md), [original-game charter](https://github.com/Hyperbog/RLOFT/blob/demo-v1/DemoV1/ORIGINAL_EXECUTION_FIDELITY_CHARTER.md), [integration contract](https://github.com/Hyperbog/RLOFT/blob/demo-v1/DemoV1/CONTINUOUS_GAME_INTEGRATION_CONTRACT_20261009.md) |
| CODE.1 / CODE.2 reconstruction | `code1-decompilation:Code1/`, `Code2/` | [CODE.1](../Code1/README.md), [CODE.2](../Code2/README.md), [CODE.2 continuity](../Code2/CODE2_CONTINUITY_ADDENDUM.md) |
| Disassembler | `disassembler-foundation:Tools/Disassembler/` | [DOSDis README](https://github.com/Hyperbog/RLOFT/blob/disassembler-foundation/Tools/Disassembler/README.md), [independent validation policy](https://github.com/Hyperbog/RLOFT/blob/disassembler-foundation/Tools/Disassembler/docs/INDEPENDENT_VALIDATION_POLICY.md) |
| Asset extractors | `extractor-wpf-fluent:Tools/`; shared extraction code also on default | [Extractor project](https://github.com/Hyperbog/RLOFT/blob/extractor-wpf-fluent/Tools/anm/README.md), [default tools entry](../Tools/README.md) |
| Source media, immutable originals | `DnD-Classics:MASTER/` | [Preservation game tree](https://github.com/Hyperbog/RLOFT/tree/DnD-Classics/MASTER) |
| Original DAT/RES research | Directory-specific `DatFiles/`, `Resources/`, `Runtime/` | [DAT](../DatFiles/README.md), [Resources](../Resources/README.md), [Runtime](../Runtime/README.md) |
| Long-form preservation reports | Existing `PreservationArchive/` and historical records | [PreservationArchive directory](../PreservationArchive/) |

## Interpretation rules

1. **An original-byte analysis is evidence, not automatically a live gameplay feature**. Preserve original-file SHA, offsets, verification windows and unresolved edges.
2. **Historical certification/accounting is not semantic or DOS acceptance**. View original release provenance separately from the latest executable build.
3. **No wholesale cross-branch merge for housekeeping**. Sync only evidence-proven shared files by exact blob; do not rewrite game assets or subsystem archives.
4. **No new GitHub Windows job** is authorized merely by a documentation commit. The native build stays owner-authorized per run.
