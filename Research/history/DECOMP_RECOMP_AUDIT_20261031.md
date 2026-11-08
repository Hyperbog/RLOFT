# Recomp / Decomp cross-examination — 2026-10-31

**Baseline:** `Decomp` `41e14238` (437 tracked blobs) versus `Recomp` `e698b374` (720 tracked blobs): **416 identical paths, 14 divergent paths, 7 Decomp-only, 290 Recomp-only**. Identical files already share Git content objects; deleting shared research to eliminate copies would break consumers without reducing storage.

## Source reconciled

- `Code1/Code1.cpp` is byte-identical: retain shared stable path.
- `Code2/Code2.cpp` differed in original graphics wrapper ABI, plane-row raster width and cell indexing. The tested Recomp Git blob `1cf86c9eca12bb9613289eff261c96cbbc01fd04` is promoted unchanged to Decomp. Original `CODE.2` SHA-256 `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314` and independently disassembled `29838`/`298F0` support the corrected six-argument wrappers.
- `Code2/graphics/original_levels_grid.hpp` is synchronized to Recomp blob `44ad92da6a8c51788e7cf935b9e081da1e61da39`, including bounded direct-save `LEVnnn.DAT` decoding.

**Cleanup:** retire only four Decomp workflow definitions whose owners are no longer on this branch: demo-v1, native-engine, disassembler-core and the old recomp Windows milestone. The default branch's status generator is repaired for five actual branches, its regression tests updated, and canonical C++17 syntax checking added to the Decomp Linux check. All original bytes, source-address research, continuity records and Git history are preserved.

## Progress (distinct denominators)

| Scope | Measured status | Limitation |
| --- | --- | --- |
| CODE.1 original object-1 decomp accounting | 100% documented | Does not prove full-program equivalence |
| CODE.2 original object-1 decomp accounting | 239,698 / 239,698 bytes = 100% | Semantics can still be wrong |
| Recomp coarse engineering acceptance | **3/12 accepted; 9 open** | NOT a playable-completion percentage |
| Player-visible complete game | NOT accepted | Original boot, creation, UI, full campaign/save and ending open |

## Options and direction

**A. Original-source first (recommended):** make one native executable traverse authentic R.EXE CODE.2 → CODE.1 character-creation → CODE.2 world flow, then complete shared DOS services and core mechanics; require an actual user-visible session for acceptance.

**B. Recover historical demo:** retrieve old demo from Git tags/archived history or artifacts and use its functional intro/creation as the baseline; swap in original-source components with explicit fidelity checks.

**C. Parallel enhanced port and recomp:** share only verified decoders/algorithms; offers feature speed but more maintenance and semantic drift risk.

Preserve original media under `DnD-Classics` and treat Green CI, decomp byte accounting and physical gameplay verification as three different forms of evidence.
