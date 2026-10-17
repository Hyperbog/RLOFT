# RLOFT branch ownership and lifecycle

**Live branch inventory reviewed 2026-10-17.** Original preserved work and decompilation/recompilation are separate workstreams. The [prior 2026-10-11 cleanup audit](BRANCH_CLEANUP_AUDIT_20261011.md) records fourteen historically retired branch refs and their immutable recovery tags; it is not a current branch count.

## Primary branches

| Branch | Owner / purpose | Policy |
| --- | --- | --- |
| `code1-decompilation` | **Default:** `Decomp/`, reconstructed `Code1/` and `Code2/`, `SharedDecomp/`, original DAT/RES/runtime research and `ResearchInfra/` | Current decompilation and source research; **no recompilation build files** |
| `recomp` | **Dedicated:** `Recomp/`, independent canonical source-link/tests and build workflow; retains `Code1/` and `Code2/` as build inputs | Recompilation changes only, separate CI |
| `demo-v1` | `DemoV1/` native Windows player game | Active enhanced port; do not wholesale merge reconstruction branches |
| `native-engine` | `NativeEngine/` standalone original-asset-backed GPU prototype | Independent experimental renderer/runtime |
| `disassembler-foundation` | `Tools/Disassembler/` DOSDis | Active disassembler tooling |
| `extractor-wpf-fluent` | `Tools/` extraction GUI/source | Active extractor tooling |
| `DnD-Classics` | Immutable original `MASTER/` game files | Do not rewrite, redistribute or clean |
| `port/asset-cache` | Asset cache integration experiments | Review before any retirement; may contain unmerged changes |

The default branch name is retained to avoid breaking existing links. On the decompilation branch, `Decomp/README.md` is the entry point; the original source paths remain unchanged to protect include paths and research tools. `Recomp/` now lives on the `recomp` branch. Original DOS source hashes, offsets and evidence remain unchanged.

## Temporary CI branches pending retirement review

Eight refs currently begin with `ci/native-` and date to 2026-10-13. They are **not** source-of-truth branches, but cannot safely be assumed redundant from their names. Review each head/unique diff, open PRs, and workflow provenance before retirement. No automatic branch deletions are implied by this document.

## Cleanup and future branch rules

1. Don't delete or rewrite `DnD-Classics`, `demo-v1`, `recomp`, `code1-decompilation` or other owner branches. Preserve player saves and original owned media.
2. Use `Decomp/` and `Recomp/` entry points and branch-local tests. Avoid a second copy of 1.3 MB `Code2.cpp` inside nested directory trees; `recomp` retains its source dependency at the original relative path.
3. Reconcile shared CODE.1/CODE.2 changes by bounded, tested cross-branch commits, referencing `docs/SHARED_SOURCE_REGISTER.md`; never wholesale merge competing products.
4. Before deleting any temporary branch, record exact SHA, inspect ahead/behind unique changes and PR references, and create/read a recovery tag. An unmerged change is a hard stop.
5. Ignore/cache generated disassembly and binary outputs locally. No copyrighted originals or full original-byte JSON reports in Git. Windows Actions artifacts remain separate from tracked sources.
6. Documentation-only cleanup uses `[skip ci]`. Build results, independent DOS evidence and player-accepted gameplay remain different statements.

Historical retired ref details and their recovery tags remain in [the 2026-10-11 audit](BRANCH_CLEANUP_AUDIT_20261011.md).
