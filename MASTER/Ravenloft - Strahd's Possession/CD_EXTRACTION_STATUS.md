# Strahd's Possession: extracted disc-content verification

**Reviewed:** 2026-10-08 (America/New_York)  
**Branch:** `DnD-Classics`  
**Upload source:** `a9fb6ab091af967415fb2dc2840aa5bf88174e5e` (commit `game.gog;gog.game`)

## Placement

The Strahd CD contents have been uploaded in the **correct game directory**:

```text
MASTER/
  Ravenloft - Strahd's Possession/
    RLOFT/                  # separately imported installed game content
    game.gog/               # extracted disc contents (directory, not disc image)
      STATUS.ME
      RLOFT/
        CINE00.ANM ... CINE12.ANM
        RES9 ... RES12
        ...
```

The `game.gog/` directory contains **69 ordinary files** whose Git-reported sizes sum to **333,687,767 bytes**. It contains Strahd-specific `RLOFT/` game resources, including thirteen `CINExx.ANM` cinematic files, supporting resource archives `RES9` through `RES12`, `LEVELS.DAT`, and save/install support files.

## Comparison against existing `RLOFT/`

Comparing Git blob SHAs of identically named files in the extracted `game.gog/RLOFT/` and the installed `RLOFT/` directories produced:

- **47 overlapping paths**, of which **42 have identical Git blob SHA-1 values** and matching sizes.
- **5 differ**: `PATCHES.INI` (6,952 vs. 7,157 bytes), `README.BAT` (33 vs. 35), `README.TXT` (6,295 vs. 6,466), `RLOFT.BAT` (2,632 vs. 2,712), and `SOUND.BAT` (2,662 vs. 2,738). Values are extracted-disc then installed-game sizes. Possible newline/encoding normalization has **not** been confirmed.
- **22 extracted-disc files do not have the same relative path in the existing installation**, including `STATUS.ME`, installation files and additional cinematic and resource data. Not all of these are necessarily absent from every game component; paths differ by distribution.
- Some installed-game-only files relate to a saved party, runtime configuration, or separately authored research; they are not proof that the disc extraction is incomplete.

These checks establish correct **game association and Git object equality** for overlaps only. They do **not** certify the entire image as an exact archival reconstruction.

## Preservation and naming

**Important:** `game.gog/` here is a **directory of extracted CD filesystem files**, not the original `game.gog` disc-image byte stream. A Git tree and a Git blob cannot occupy the same name, so it is recommended to rename the extracted folder to `CD_Extracted/` **before** an eventual LFS import of a true `game.gog` image.

The original Strahd disc-image byte length/SHA-256 have **not yet been independently recorded for this workspace**. Do not guess them from extracted-file sizes; raw CD images may include filesystem headers, padding and other sector-level information.

Strahd's Possession is **not** one of the 13 games counted by the separate `MASTER/MISSING_SOURCE_FILES.md` D&D collection manifest. This note does not alter the separate 1,529/1,533 original-file inventory status.

**No folders were renamed, and no game binaries were changed during this audit.**
