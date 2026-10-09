# D&D Classics: Import and Byte-Fidelity Audit

**Audit date:** 2026-10-08 (America/New_York)  
**Branch:** `DnD-Classics`  
**Scope:** Original 13-game collection under `MASTER/`. Strahd's Possession is a separate, additional workspace.

## Current coverage

| Game | Inventory entries | Source paths currently present | Paths missing | Present files with source-size mismatch |
| --- | ---: | ---: | ---: | ---: |
| Curse of the Azure Bonds | 138 | 138 | 0 | 7 |
| Dark Sun | 88 | 88 | 0 | 11 |
| Dark Sun 2 | 266 | 0 | 266 | 0 |
| Dungeon Hack | 54 | 54 | 0 | 8 |
| Gateway to the Savage Frontier | 115 | 115 | 0 | 7 |
| Hillsfar | 90 | 90 | 0 | 7 |
| Menzoberranzan | 83 | 2 | 81 | 0 |
| Pool of Radiance | 188 | 188 | 0 | 6 |
| Pools of Darkness | 101 | 101 | 0 | 8 |
| Ravenloft - Stone Prophet | 83 | 0 | 83 | 0 |
| Secret of the Silver Blades | 85 | 85 | 0 | 7 |
| Treasures of the Savage Frontier | 101 | 101 | 0 | 6 |
| Unlimited Adventures | 141 | 141 | 0 | 7 |
| **Total** | **1,533** | **1,103** | **430** | **74** |

Counts were obtained by comparing committed Git tree paths and byte sizes to each game's `SOURCE_INVENTORY.csv`. A file with the same size is **not yet certified SHA-256 identical**; all 1,533 files need original-byte verification. The 74 size mismatches are not independently proven to be newline normalization and must be recovered/verified from sources.

## Files requiring Git LFS (over GitHub's 100 MiB regular Git limit)

| Relative path under `MASTER/` | Bytes | Expected original SHA-256 |
| --- | ---: | --- |
| `Dark Sun 2/game.gog` | 109132800 | `855332c33db90aa2b524ccd00d630180c95cdf455abb3b00038e830fe9e72866` |
| `Menzoberranzan/game.gog` | 341192704 | `218d60d0fd992cb360198da1b7214e2345b03af87ee2be3f646fa3efbf307f65` |
| `Ravenloft - Stone Prophet/gog.game` | 437256192 | `701bd803d4b3a54e9200d15c1b4af1626a793cec1449e7dfca14d8a6f96e3629` |
| `Ravenloft - Stone Prophet/gog.zip` | 292931966 | `0c1ba4f9d58f8c53cf1dea0d3749ba68112625b0e8443b19ed905e7e8a6a6862` |

Total original large-file payload: 1,180,513,662 bytes. The existing importer in `Tools/CollectionImport/` already implements Git LFS tracking (default threshold: 90 MiB). LFS objects, unlike Git tree pointers, require an LFS-capable authorized client for original-byte retrieval.

## Source archive dependency

The original ZIP archives from a preceding chat are not accessible to the current conversation's working filesystem, project-file list, or Library listing. The existing importer expects these six original archive files, each with an audited SHA-256:

- `D&D Games 1.zip` — includes Dark Sun 2 and six of the games with mismatches.
- `D&D Games 2.zip` — needed to verify/repair the other four imported games.
- `Menzoberranzan.zip` — needed for 81 missing files.
- `Ravenloft - Stone Prophet.zip.001`
- `Ravenloft - Stone Prophet.zip.002`
- `Ravenloft - Stone Prophet.zip.003`

The last three parts must be available together to reconstruct the original Stone Prophet archive.

## Remaining work and precautions

1. Reattach the original six ZIP files or make them available to an authenticated import environment.
2. Compare all currently committed original files to their inventory SHA-256 values, not merely sizes.
3. Restore missing entries and any altered source bytes **without** text/newline conversion.
4. Use Git LFS for the four oversized originals; verify their LFS SHA-256 identifiers and file sizes.
5. Commit in bounded batches, then independently verify the Git tree, file bytes, and LFS availability before marking the collection complete.
6. Update the READMEs and per-game manifest records once the import is verified.

**Important:** The current `Tools/CollectionImport/upload_collection.py` is written for a first-time import and refuses to overwrite any already-present files; it is **not yet suitable for restoring the current partially populated `MASTER/` tree** without an audited resume/repair enhancement. Do not run `--apply` unchanged against this branch.

Original commercial files are only intended for privately authorized archival and research use; do not expose the repository publicly without appropriate rights.
