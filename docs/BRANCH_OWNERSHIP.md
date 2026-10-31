# RLOFT workstream ownership (verified 2026-10-31)

The GitHub branches API currently exposes **five** branch refs. Names are case-sensitive. Earlier documents mentioning `code1-decompilation`, `recomp`, `demo-v1`, `native-engine`, `disassembler-foundation`, or `extractor-wpf-fluent` describe historical branch names or no-longer-present refs, **not** current worktrees.

| Active branch | Owner and scope |
| --- | --- |
| [`Decomp`](https://github.com/Hyperbog/RLOFT/tree/Decomp) | Original-source CODE.1/CODE.2 reconstruction, byte witnesses, ResearchInfra and decompilation evidence |
| [`Recomp`](https://github.com/Hyperbog/RLOFT/tree/Recomp) | Host runtime, compilation and player-facing original-source execution; retains identical CODE.1/CODE.2 source paths needed for its builds |
| [`DosDis`](https://github.com/Hyperbog/RLOFT/tree/DosDis) | Cross-platform/Windows disassembler product development |
| [`Ravenloft-Asset-Extractor`](https://github.com/Hyperbog/RLOFT/tree/Ravenloft-Asset-Extractor) | Asset-extraction application |
| [`DnD-Classics`](https://github.com/Hyperbog/RLOFT/tree/DnD-Classics) | Immutable original commercial media and `MASTER/` game assets |

## Shared source policy

1. Keep `Code1/Code1.cpp`, `Code2/Code2.cpp` and shared parsers at stable paths on both Decomp and Recomp. Git stores identical content as shared blobs; removing one copy **does not** remove meaningful Git storage redundancy and breaks standalone checkouts.
2. For each source fix: verify original bytes, test the owning branch, promote **the identical Git blob** to the other branch, and check both builds. Do not merge all of `Recomp/` into Decomp or vice versa.
3. Recomp owns execution/host-specific `Recomp/` code. Decomp owns decompilation documentation and source-semantics verification. Keep cross-workstream status descriptions accurate and explicitly grade evidence as static accounting, tested execution, or playable original functionality.
4. Preserve complete Git history and historical research. Never delete commercial media, commit proprietary binaries to source branches, force-push, or delete a current branch as part of documentation cleanup.
5. Historical recovery tags under `archive/retired-20261011/` were recorded in earlier cleanup reports, but an absent active branch does not demonstrate whether its playable demo can be recovered. Verify a concrete historical ref or artifact before claiming recovery.

## Verified current cross-branch facts

The October 31 tree audit found that CODE.1, CODE.2 and the graphics/level parser source copies already have matching blob SHAs. The remaining differences are mainly metadata, workstream-specific docs and workflows. See [Decomp cross-examination](https://github.com/Hyperbog/RLOFT/blob/Decomp/docs/RECOMP_DECOMP_AUDIT_20261031.md) and [Recomp acceptance ledger](https://github.com/Hyperbog/RLOFT/blob/Recomp/Recomp/ACCEPTANCE_GATES.json).

**Completion disclaimer:** 100% reconstructed executable-byte accounting is not 100% semantic fidelity. Recomp's 3/12 accepted gates are an engineering checklist, not 25% player completion. The complete original game is not accepted as playable.
