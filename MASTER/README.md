# D&D Classics — Original Collection Workspace

**STATUS: inventories committed; original game payloads NOT YET committed to this branch.**

This directory contains one isolated folder per game. Each `SOURCE_INVENTORY.csv` records original relative paths, source ZIP identity, file size, CRC-32, and SHA-256, based on the independently validated uploaded collection. The inventories are references, not substitutes for the original files.

| Game | Original files | Source archive |
| --- | ---: | --- |
| [Curse of the Azure Bonds](<Curse%20of%20the%20Azure%20Bonds/>) | 138 | `D&D Games 1.zip` |
| [Dark Sun](<Dark%20Sun/>) | 88 | `D&D Games 1.zip` |
| [Dark Sun 2](<Dark%20Sun%202/>) | 266 | `D&D Games 1.zip` |
| [Dungeon Hack](<Dungeon%20Hack/>) | 54 | `D&D Games 1.zip` |
| [Gateway to the Savage Frontier](<Gateway%20to%20the%20Savage%20Frontier/>) | 115 | `D&D Games 1.zip` |
| [Hillsfar](<Hillsfar/>) | 90 | `D&D Games 1.zip` |
| [Menzoberranzan](<Menzoberranzan/>) | 83 | `Menzoberranzan.zip` |
| [Pool of Radiance](<Pool%20of%20Radiance/>) | 188 | `D&D Games 1.zip` |
| [Pools of Darkness](<Pools%20of%20Darkness/>) | 101 | `D&D Games 2.zip` |
| [Ravenloft - Stone Prophet](<Ravenloft%20-%20Stone%20Prophet/>) | 83 | `Ravenloft - Stone Prophet.zip.001–.003` |
| [Secret of the Silver Blades](<Secret%20of%20the%20Silver%20Blades/>) | 85 | `D&D Games 2.zip` |
| [Treasures of the Savage Frontier](<Treasures%20of%20the%20Savage%20Frontier/>) | 101 | `D&D Games 2.zip` |
| [Unlimited Adventures](<Unlimited%20Adventures/>) | 141 | `D&D Games 2.zip` |

**Total original regular files: 1,533 (13 games).** The existing `README.md` in each game directory is a research scaffold, not an original game file.

To import the exact game content into these folders, use [Tools/CollectionImport](../Tools/CollectionImport/README.md). That importer verifies the six original source hashes, keeps filenames/paths segregated, uses Git LFS for four oversized files, and batches 80 files per commit. The importer requires running from an authenticated Git clone with the source ZIP archives present on disk.

Do not substitute Strahd's Possession reference binaries for any file in the Menzo or Stone source inventories. Source provenance must be game-specific.

Distribution/redistribution of these commercial game files is subject to the rights held by their owners; keep the repository private unless authorized.
