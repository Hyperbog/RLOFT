# Ravenloft: Strahd's Possession — source research and reconstruction

**Goal:** faithfully reconstruct the original game and make its engine, data and behavior understandable, portable and moddable. A completed player-facing game is **not** yet verified.

## Current workstreams (GitHub branches verified 2026-10-31)

| Branch | Purpose |
| --- | --- |
| [`Decomp`](https://github.com/Hyperbog/RLOFT/tree/Decomp) | CODE.1 / CODE.2 decompilation and research evidence (this checkout) |
| [`Recomp`](https://github.com/Hyperbog/RLOFT/tree/Recomp) | Native original-source runtime and Windows player package |
| [`DosDis`](https://github.com/Hyperbog/RLOFT/tree/DosDis) | General-purpose game disassembler |
| [`Ravenloft-Asset-Extractor`](https://github.com/Hyperbog/RLOFT/tree/Ravenloft-Asset-Extractor) | Original game asset extraction |
| [`DnD-Classics`](https://github.com/Hyperbog/RLOFT/tree/DnD-Classics/MASTER) | Original commercial media; preserve unmodified |

Earlier references to `demo-v1`, `native-engine`, `code1-decompilation` and lowercase `recomp` refer to historical workstreams or obsolete branch names, not currently listed branches. Recovery of earlier demo material requires inspecting actual historical commit/tag or Actions artifact records.

## Start here

- [Decompilation guide](Decomp/README.md) and canonical [CODE.1](Code1/README.md) / [CODE.2](Code2/README.md) reconstructed sources.
- [Active Recomp source and verification](https://github.com/Hyperbog/RLOFT/blob/Recomp/Recomp/README.md), [acceptance ledger](https://github.com/Hyperbog/RLOFT/blob/Recomp/Recomp/ACCEPTANCE_GATES.json), and [native Windows build workflows](https://github.com/Hyperbog/RLOFT/tree/Recomp/.github/workflows).
- [Decomp / Recomp cross-examination](docs/RECOMP_DECOMP_AUDIT_20261031.md), [current branch ownership](docs/BRANCH_OWNERSHIP.md), [source identity register](docs/SHARED_SOURCE_REGISTER.md), and [documentation index](docs/INDEX.md).
- [Read-only shared source-parity checker](ResearchInfra/check_shared_source.py): compare critical Git blobs after fetching both `Decomp` and `Recomp` refs.
- [Original game resource format research](DatFiles/README.md) and [mod-forward methodology](MOD_FORWARD_METHODOLOGY.md).

## Evidence and acceptance

CODE.1 and CODE.2 reconstructed executable-byte accounting has been recorded as 100%. This is **not** a claim of complete semantic equivalence or a finished game. The Recomp checklist records **3 of 12 engineering gates** accepted; the full player-visible game remains unaccepted. A compiled Windows executable or passing CTests establishes only its tested engineering conditions.

Recompilation development must preserve exact original-source function behavior, keep the original media in its source-preservation branch, and verify game-visible startup, character creation, save/load, world rendering, input, combat, scripts, audio, and campaign progression in one runnable session. Do not distribute proprietary original game files with the reconstructed-source application.

## Local verification

From a checkout containing both branch refs:

```sh
python3 ResearchInfra/check_shared_source.py --self-test
python3 ResearchInfra/check_shared_source.py --decomp-ref origin/Decomp --recomp-ref origin/Recomp
g++ -std=c++17 -fsyntax-only Code1/Code1.cpp
g++ -std=c++17 -fsyntax-only Code2/Code2.cpp
```

For active Windows player packaging and full CTests, see the Recomp branch. A successful syntax check is not a gameplay test.
