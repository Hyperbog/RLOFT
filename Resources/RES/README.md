# RES resource archives

This directory contains one analysis workspace per supplied `RES0` through `RES12` archive.

## Current reconstruction status

The outer count/absolute-offset container is fully reconstructed and byte-exact round-trippable for all 13 archives. CODE.2's inner `EH` dispatcher has been traced directly, and every supplied instance of types 1, 2, 3, 4, 6, and 7 has been structurally validated.

Type 6 is resolved as column-major first-person texture data: each source column is exactly 128 texels, and the supplied resources are 32x128 or 64x128. Type 7 is resolved as a compiled scene-shape format with a 10-byte geometry header, per-column relative-pointer table, and zero-terminated 6-byte display-list descriptors that drive scale-specific renderer microcode. `NPCS.DAT +0x14` is now directly tied to archive-3/type-7 base IDs for all 83 supplied NPC definitions. The 38 non-`EH` records at `RES9:517..554` are executable-proved interaction/display wordcode programs selected from `INIT`-derived archive bases.

See:

- `FORMAT.md` — executable-backed inner/outer format specification and evidence;
- `INVENTORY.md` — complete supplied-corpus counts, compression distribution, and anomalies;
- `TYPE6_RENDERER.md` — executable-backed 32x128 / 64x128 column-major texture layout;
- `TYPE7_RENDERER.md` — compiled scene-shape geometry, column-pointer table, and renderer-microcode display-list reconstruction;
- `TYPE7_NPC_INDEX.md` — all 83 `NPCS.DAT` definitions mapped to archive-3/type-7 base IDs and base geometry;
- `NON_EH.md` — archive-9 interaction/display wordcode container and interpreter evidence;
- `analyze_res.py` — lossless outer parser plus confirmed-family validator;
- `analyze_type7.py` — exhaustive type-7 geometry/table/list validator.

## Evidence rules

The original RES bytes and original CODE.2 machine code control all claims. Certified `Code2/Code2.cpp`, current DAT analysis, and CODE.1 are primary cross-reference evidence. Community tools such as `LoftRES` are useful leads only and do not override the shipped bytes or executable behavior.

Unknown or unsupported-in-corpus behavior remains explicit. In particular:

- executable type 5 has no supplied instance;
- type-1 compression method 1 exists in CODE.2 but no supplied resource uses it;
- the 38 `RES9` non-EH programs are interaction/display wordcode, while the ID-aligned `RES12` bank and two `RES1` sentinels still have unresolved runtime roles;
- type 6 is resolved as 32x128 / 64x128 column-major first-person surface texture data;
- type 7's on-disk compiled-shape structure is resolved, while the historical names/semantics of its three 6-byte rasterizer-descriptor fields and the external scale-rasterizer data remain under reconstruction.

No binary asset is silently repaired or normalized. The two duplicated type-4 resources whose declared dword length exceeds their stored archive entry by exactly `0x10000` are documented as shipped anomalies.

## Historical legacy archives

- RES11_RES12_LINEAGE.md — cross-generation proof that CD-shipped RES11/RES12 are legacy/pre-split composite resource generations retained alongside the later active RES0..RES10 layout.
- analyze_legacy_archives.py — reproducible RES11/RES12 -> RES3/4/5/6/9/10 and CODE.2 template-lineage validator.
- RES12_WORDCODE.md — recovered legacy VM grammar and opcode-05 inline character-template encoding.
