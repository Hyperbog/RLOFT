# Missing original D&D Classics source files

**Verified:** 2026-10-08 (America/New_York)  
**Branch:** `DnD-Classics`  
**Source snapshot:** `5bc398ad7b74` (after `dbcfaf0a78c5`, the `DS2` import)  
**Scope:** the 13-game collection's `SOURCE_INVENTORY.csv` files. Ravenloft: Strahd's Possession is maintained separately.

All 13 game directories exist. **1,529 of 1,533 original inventory file paths are present; exactly four are missing.** These counts reflect presence in Git trees, **not SHA-256 validation**.

## Files to request from the original archives

All relative paths below begin at the repository's `MASTER/` directory.

| Game | File path within game directory | Required bytes | Original SHA-256 | Original source archive |
| --- | --- | ---: | --- | --- |
| Dark Sun 2 | `game.gog` | 109,132,800 | `855332c33db90aa2b524ccd00d630180c95cdf455abb3b00038e830fe9e72866` | `D&D Games 1.zip` |
| Menzoberranzan | `game.gog` | 341,192,704 | `218d60d0fd992cb360198da1b7214e2345b03af87ee2be3f646fa3efbf307f65` | `Menzoberranzan.zip` |
| Ravenloft - Stone Prophet | `gog.game` | 437,256,192 | `701bd803d4b3a54e9200d15c1b4af1626a793cec1449e7dfca14d8a6f96e3629` | `Ravenloft - Stone Prophet.zip.001`–`.003` |
| Ravenloft - Stone Prophet | `gog.zip` | 292,931,966 | `0c1ba4f9d58f8c53cf1dea0d3749ba68112625b0e8443b19ed905e7e8a6a6862` | `Ravenloft - Stone Prophet.zip.001`–`.003` |

**Total missing payload:** 1,180,513,662 bytes (before Git LFS overhead). All four files exceed the 100 MiB regular Git file limit and should be imported through Git LFS, preserving original bytes and validating SHA-256 after transfer. The Stone Prophet source archive was provided as three consecutively split ZIP segments; **all three segments must be available together** to extract the two originals.

**When needed:** The user can supply any individual missing file extracted from the original archive, preserving the exact bytes, or supply its full source archive. Check size and SHA-256 against this table before importing; do not replace one game's file with another game's identically named asset. If transferring via an authenticated local Git clone, use Git LFS. Existing tools do not presently offer a direct Git LFS binary-upload endpoint through the GitHub connector.

## Separate integrity-verification backlog (files present, not missing)

Besides the four missing paths, **104 present original-file paths have a byte-size mismatch** versus their source inventories:

| Game | Present files with source-size mismatch |
| --- | ---: |
| Curse of the Azure Bonds | 7 |
| Dark Sun | 11 |
| Dark Sun 2 | 10 |
| Dungeon Hack | 8 |
| Gateway to the Savage Frontier | 7 |
| Hillsfar | 7 |
| Menzoberranzan | 8 |
| Pool of Radiance | 6 |
| Pools of Darkness | 8 |
| Ravenloft - Stone Prophet | 12 |
| Secret of the Silver Blades | 7 |
| Treasures of the Savage Frontier | 6 |
| Unlimited Adventures | 7 |
| **Total** | **104** |

These 104 are **not missing**. They must be byte-audited and, if altered, restored from authenticated source ZIPs. The cause has not been independently established; do not treat them as original/verified merely because they are present. Likewise, files of matching size still need SHA-256 checking before claiming full byte-for-byte preservation.

**References:** each `MASTER/<game>/SOURCE_INVENTORY.csv`, `MASTER/IMPORT_STATUS.md`, and `Tools/CollectionImport/README.md`.

## Extracted disc folders added on 2026-10-08

The `game.gog;gog.game` commit (`a9fb6ab091af967415fb2dc2840aa5bf88174e5e`) added **extracted CD filesystem contents** as directories, under the **correct games**:

| Game | Current extracted-directory path | Number of files | Sum of committed file sizes | Classification |
| --- | --- | ---: | ---: | --- |
| Dark Sun 2 | `MASTER/Dark Sun 2/game.gog/` | 264 | 93,526,520 bytes | Extracted disc files; NOT the original `game.gog` image |
| Menzoberranzan | `MASTER/Menzoberranzan/game.gog/` | 72 | 340,458,998 bytes | Extracted disc files; NOT the original `game.gog` image |
| Ravenloft - Stone Prophet | `MASTER/Ravenloft - Stone Prophet/gog.game/` | 98 | 436,345,876 bytes | Extracted disc files; NOT the original `gog.game` image |

Spot-checked distinctive contents: Dark Sun 2 has `DSUN.EXE`, `RESOURCE.GFF`, cinematics and speech; Menzoberranzan has `DATA.A00`, `RES10`, `CGEN/` and `CINE/`; Stone Prophet has `LEVELS.DAT`, `OBJECTS.DAT`, `RES10`, and cinematics. These paths are appropriately associated with their game, and this classification does **not** assert each extracted file is byte-identical to original CD sectors or ZIP contents.

**Naming issue:** Git cannot store both a directory named `game.gog/` and a blob named `game.gog` at the same path. Likewise for Stone Prophet's `gog.game/`. Before importing the actual large disc images via Git LFS, rename the extracted directories to `CD_Extracted/` (or another clearly distinct name) in their respective game folders, retaining their relative internal structure. Do not mistake these extracted directories for the original `.gog` or `.game` byte sequences. **No rename was performed by this audit.**

The user confirms that **Stone Prophet's `gog.zip` is a ZIP containing `gog.game`**, not a second distinct disc filesystem. It remains a separate exact-original source-inventory entry for byte-preservation purposes. A newly recompressed ZIP is not guaranteed to reproduce the source archive's SHA-256. The four original file paths listed above therefore remain pending for exact archival completeness, even though the exposed disc contents are now present as directories.
