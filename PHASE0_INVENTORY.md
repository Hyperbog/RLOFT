# Phase 0 — Original-media inventory and Strahd baseline

**Status: COMPLETE for all 624 existing preserved repository files and all 239 CD_Extracted files.** Their actual checkout bytes were independently SHA-256 hashed. The external GOG disc/container files remain absent, 20 source-archive text/configuration files differ, and one Stone Prophet resource pair (`RES9`) differs between installation and CD extraction. No gameplay code or original game bytes were changed or committed.

## Scope and provenance

- Repository: [Hyperbog/RLOFT](https://github.com/Hyperbog/RLOFT), preservation branch [`DnD-Classics/MASTER`](https://github.com/Hyperbog/RLOFT/tree/DnD-Classics/MASTER); analysis branch `Ravenloft-Enhanced`.
- Tree IDs: Strahd `5fe053a0bfd673694e81b859cf874f92f109f949`, Menzo `074eb6640d867f5ce1007f65a8f9ed38c6635d69`, Stone `ba743bf7cfd33eca38d38696452bcc0581592bb9`. All recursive Git-tree responses reported `truncated=false`.
- Every row below is an **existing Git-tree file path and Git-reported byte size**, including GOG wrappers, DOSBox support, CD extractions, duplicated installation files, reference documents and save data. This is not a claim that each Git blob matches its original-media source.
- Git blob SHA-1 is **not** SHA-256. The SHA-256 column is identified as **expected source** when taken from an owner's prior `SOURCE_INVENTORY.csv`, not a current hash of the Git blob. `UNKNOWN` means no source SHA-256 could be substantiated from inspected evidence. Files with source-size mismatch are explicitly marked.
- Role is a conservative filename-based *classification*, not a proven internal format. `UNKNOWN` means no reliable role from the name alone.

## Independent SHA-256 checkout audit — verified 2026-11-14

**Evidence:** [successful GitHub Actions run #43608850010](https://github.com/Hyperbog/RLOFT/actions/runs/43608850010), commit `80b80a73b2c6dc5cdd4c616dfec632612604622f`, on the `Ravenloft-Enhanced` audit workflow. The runner performed a read-only sparse checkout of all three game directories directly from `DnD-Classics`, hashed every file with Python `hashlib.sha256`, and uploaded only derived metadata (no commercial media) as [artifact #14118877810](https://github.com/Hyperbog/RLOFT/actions/runs/43608850010/artifacts/14118877810). Workflow file: `.github/workflows/phase0-media-audit.yml`. This verifies the actual preserved checkout bytes, **not** independent identity with an unavailable original disc-image container.

| Title | Files hashed | Bytes hashed | CD_Extracted files | Source-archive SHA-256 PASS | Source-archive SHA-256 DIFFERENT | Original disc containers absent |
| --- | ---: | ---: | ---: | ---: | ---: | ---: |
| Strahd | 286 | 386,183,551 | 69 | no source CSV | no source CSV | not assessed |
| Menzoberranzan | 156 | 419,849,025 | 72 | 74 | 8 | 1 |
| Stone Prophet | 182 | 513,238,081 | 98 | 69 | 12 | 2 |
| **Total** | **624** | **1,319,270,657** | **239** | **143** | **20** | **3** |

All 624 actual per-file SHA-256 hashes reside in the machine-readable `strahd.json`, `menzo.json`, and `stone.json` files within the metadata artifact. The original source CSVs cover **outer archive/installation files**, not every extracted CD member. Therefore `CD_Extracted` rows are verified as **hashed and retained** but cannot be called independently matched against an unavailable original CD image. Full-media inventory completeness refers to all currently preserved files, not byte-level reproduction of the omitted outer containers.

Reproducible **aggregate extracted-file manifest** SHA-256 (a checksum over sorted `filename + NUL + file-SHA256 + newline` entries, **not** the GOG/ISO image hash):

| Title | CD_Extracted manifest SHA-256 |
| --- | --- |
| Strahd | `a0f9aa6220b14d3a3f2e3edf2ec134f521c6c0d1384db9b2eb7974af4154c334` |
| Menzo | `7c3cfbb85cc7cf1e56fe2331926fd8a557975a7d76d4a0b6008f37e4f4bb31bd` |
| Stone | `203ee9bb7684c44fe8e988c35545c410b04569fa280890e79a85e67bbcfaad1e` |

### Cross-copy identity check

Comparison rule: for a path `CD_Extracted/X`, compare the directly corresponding installed path `X` (Menzo), `PROPHET/X` (Stone), or `RLOFT/X` (Strahd, where extracted paths start at `CD_Extracted/RLOFT/`). Only pairs that actually exist are compared.

| Title | Compared pairs | SHA-256 identical | SHA-256 different |
| --- | ---: | ---: | ---: |
| Strahd | 47 | 42 | 5 |
| Menzo | 2 | 2 | 0 |
| Stone | 11 | 9 | 2 |

The five differing Strahd pairs are `PATCHES.INI`, `README.BAT`, `README.TXT`, `RLOFT.BAT`, and `SOUND.BAT`. Stone's differing pairs are `README.TXT` and **`RES9`**. Other extracted and installed files without a corresponding path are not treated as matching.

**Stone Prophet RES9 discrepancy (VERIFIED SHA-256 difference, semantics UNKNOWN):** `CD_Extracted/RES9` is 3,070,563 bytes with SHA-256 `b7555fbd071eca3adfe3d349c343fc0bb4b44518281fc163bf624c6bc81440c1`. `PROPHET/RES9` is also 3,070,563 bytes but SHA-256 `e17f2a1a4868fe580f346c2f273312f6c72bb2eec33b3af1118eac5134efff76`. Both variants must remain available; Phase 1 must establish original loading precedence and meaning before choosing any authoritative variant.

**Original source-archive discrepancies:** Menzo's eight nonmatching files are `dosbox_menzo.conf`, `dosbox_menzo_single.conf`, `goggame-galaxyFileList.ini`, `goglog.ini`, `PATCHES.INI`, `README.TXT`, `SOUND.INI`, and `unins000.ini`. Stone's twelve are `dosbox_prophet.conf`, `dosbox_prophet_single.conf`, `goggame-galaxyFileList.ini`, `goglog.ini`, `PROPHET/PATCHES.INI`, `PROPHET/PROPHET.BAT`, `PROPHET/README.BAT`, `PROPHET/README.TXT`, `PROPHET/README_1.1.TXT`, `PROPHET/SOUND.BAT`, `PROPHET/SOUND.INI`, and `unins000.ini`. SHA-256 does not establish the cause of the differences; no newline-normalization explanation is asserted.

**Unchanged Strahd baselines:** all four analyzers passed with exit code 0 on the checked-out `RLOFT` game directory; stored in `strahd-baseline.json` in the artifact.

Exact audit entry points (executed by the workflow):

```sh
python Tools/phase0_inventory.py --title strahd --media 'preservation/MASTER/Ravenloft - Strahd'\''s Possession' --output audit/strahd.json
python Tools/phase0_inventory.py --title menzo --media 'preservation/MASTER/Menzoberranzan' --expected-csv 'preservation/MASTER/Menzoberranzan/SOURCE_INVENTORY.csv' --output audit/menzo.json
python Tools/phase0_inventory.py --title stone --media 'preservation/MASTER/Ravenloft - Stone Prophet' --expected-csv 'preservation/MASTER/Ravenloft - Stone Prophet/SOURCE_INVENTORY.csv' --output audit/stone.json
```

The workflow hashes original media read-only and uploads JSON results. It never commits the original game files into `Ravenloft-Enhanced`.

## RES archive availability, execution selection, and legacy data (pre-Phase-1)

**Status:** VERIFIED for preserved filenames, SHA-256 identity, all three original CODE.2 startup group-count instruction sequences, Strahd archive record composition, and Stone RES9's changed record. **Legacy historical interpretation:** HIGH CONFIDENCE, not a recovered older-runtime trace. Evidence: [read-only media audit run #43618044077](https://github.com/Hyperbog/RLOFT/actions/runs/43618044077), commit `a3ed5a88b437d873dd98046403068f4f26bd0dad`; [metadata-only run artifact](https://github.com/Hyperbog/RLOFT/actions/runs/43618044077/artifacts/14123385247).

### Original CODE.2 resource group-count witnesses

The CI job read the actual, SHA-256-hashed CODE.2 files from the `DnD-Classics` checkout, recovered their LE code pages (nested MZ origin `0x25D24`, little-endian uint32 page offset at `LE+0x80`), and matched the x86 instructions plus the known CALL target. The group count is encoded as `PUSH imm8 0x0B`, a **1-byte immediate, value 11**; group indices are **0 through 10**, not 0 through 12.

| Title | CODE.2 source | Logical group-count instruction | Resource initializer callee | Verified count |
| --- | --- | --- | --- | ---: |
| Strahd | `RLOFT/CODE.2` | `0x20444` | `0x2F2EC` | 11 |
| Menzo | `CODE.2` | `0x23943` | `0x3532C` | 11 |
| Stone | `PROPHET/CODE.2` | `0x2D46B` | `0x412D0` | 11 |

CODE.1 startup reconstructions independently pass the same group count **11** for Menzo and Stone (`Enhanced/src/original_resource_startup.cpp`) and Strahd (`DatFiles/INFO.DAT/FORMAT.md`, `Resources/RES/RES11_RES12_LINEAGE.md`). Those CODE.1 facts are prior executable-backed research and were **not** re-traced from executable bytes in this CI job. An initialized resource *group* is not proof that each individual asset within it is visited in gameplay.

### Actually present RES files

| Game | Installed-directory RES files | CD_Extracted RES files | CD-only RES files | Runtime group-count range | Legacy-only RES in known runtime |
| --- | --- | --- | --- | --- | --- |
| Strahd | RES0..RES8 (9) | RES0..RES12 (13) | RES9, RES10, RES11, RES12 | RES0..RES10 | RES11, RES12 |
| Menzo | RES0..RES8 (9) | RES3, RES6, RES9, RES10 (4) | RES9, RES10 | RES0..RES10 | none demonstrated |
| Stone | RES0..RES9 (10) | RES0..RES10 (11) | RES10 | RES0..RES10 | none demonstrated |

For Strahd, the earlier estimate of twelve CD RES and eight installed RES was off by one each. The **two genuine Strahd historical holdovers** are RES11 and RES12; RES9 and RES10 are active group indices routed to the CD. All Menzo/Stone RES names observed fall inside the eleven-group original resource bootstrap. Missing a local copy does **not** indicate an unused asset.

The nested archive format uses a **4-byte little-endian entry count at offset 0**, followed by **4-byte little-endian absolute offsets beginning at offset 4**. For example, Strahd `RES11` reports **5,114** records and `RES12` reports **667**. EH records begin with a two-byte `EH` marker and a 1-byte type tag at record-relative +2. Those archive counts and record groups were re-parsed independently from the owner's supplied `RES+OTHER.zip`.

Strahd RES11 has 4,128 type-7 scene-shape, 652 type-6 texture, 74 type-3 XMIDI, and 260 type-4 sound records. RES12 has 370 type-1 graphics, 37 raw wordcode, and 260 type-4 sounds. Record-level SHA-256 checks found **2,402/4,128** RES11 scene-shapes identical to active RES3, **609/652** textures identical to RES4, **255/370** RES12 images identical to RES9, and **260/260** sounds identical between RES11's and RES12's legacy sound banks. This favors an older composite format replaced by later split archives, rather than unrelated random files.

The unchanged repository analyzer also passed against the read-only CI originals:

```sh
python Resources/RES/analyze_legacy_archives.py \
  "preservation/MASTER/Ravenloft - Strahd's Possession/CD_Extracted/RLOFT" \
  --code2 "preservation/MASTER/Ravenloft - Strahd's Possession/RLOFT/CODE.2"
```

It confirmed **37/37** legacy RES12 wordcode programs decode to EOF, **1,079** instructions, **zero** bad control targets, and **16** occurrences of an older 141-word inline character-template encoding for opcode 05. Detailed historical lineage is retained in `Resources/RES/RES11_RES12_LINEAGE.md`. The exact old producer/consumer executable is **UNKNOWN**; neither the current CODE.1 nor CODE.2 uses those two archives through its proved indexed RES initialization.

### Stone Prophet RES9: one changed record, not an extra archive

Both `CD_Extracted/RES9` and `PROPHET/RES9` are **3,070,563 bytes**, **626** entries, with identical content for **625** entries. The single differing record is **zero-based index 574**, whose 4-byte little-endian offset-table slot is at physical file offset `0x8FC`. Each version of entry 574 is **634 bytes** and non-EH/raw. **Five bytes differ**; the first changed byte is at record-relative offset `+494` (physical offset `uint32_le(file[0x8FC:0x900]) + 494`). The remaining byte differences and record semantics are not guessed.

Hashes:
- CD `RES9`: `b7555fbd071eca3adfe3d349c343fc0bb4b44518281fc163bf624c6bc81440c1`.
- Installed `RES9`: `e17f2a1a4868fe580f346c2f273312f6c72bb2eec33b3af1118eac5134efff76`.

This is a **same-index resource revision**, not proof the entire CD archive is obsolete. Which copy the original game resolves in each installation mode, why record 574 changed, and whether that specific record executes remain **UNKNOWN**. Preserve both and determine loader precedence and semantic meaning during Phase 1. Do not silently substitute one file for the other.

### Pre-Phase-1 guidance

- The unified engine should treat the **proved initialized group set RES0..RES10** as its default original source range for all three supplied CODE.2 binaries. Do not add RES11/12 to Strahd's active runtime merely because the CD includes them.
- Keep Strahd RES11 and RES12 as **read-only optional historical research inputs**; do not delete, modify or redistribute them.
- Do not assume CD-only Menzo/Stone `DFA`, `ACT`, audio, cinematics, or installer files are leftovers. Confirm each consumer and semantic role before omitting gameplay media.
- The exact **unused status** of individual entries within an initialized RES0..RES10 archive cannot be certified from the archive startup call alone. That requires runtime references/call-graph analysis in Phase 1.

**Reproducer:** `.github/workflows/phase0-media-audit.yml` runs the actual CODE.2 startup instruction checks, file-name comparisons, archive-entry diff, and unchanged legacy analyzer, then publishes `res-usage.json`, `strahd-legacy-lineage.txt`, and `SUMMARY.md` in the metadata-only audit artifact. **User-performed tests:** none; **automated tests:** all passed.

## File counts and provenance status

| Title | Git-tree files | Tree bytes | Unique basenames | Repeated basenames | Expected source-inventory paths | Source SHA-256 PASS | Source SHA-256 MISMATCH | Missing outer source containers |
| --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| Ravenloft - Strahd's Possession | 286 | 386,183,551 | 128 | 66 | — | — | — | — |
| Menzoberranzan | 156 | 419,849,025 | 153 | 3 | 83 | 74 | 8 | 1 |
| Ravenloft - Stone Prophet | 182 | 513,238,081 | 170 | 11 | 83 | 69 | 12 | 2 |

**Source-CSV SHA-256 matching was performed on the actual checkout bytes in run #43608850010.** Menzo: 74 exact matches, eight mismatches, and the outer disc image `game.gog` absent. Stone: 69 exact matches, 12 mismatches, and outer containers `gog.game` and `gog.zip` absent. These three omitted outer source containers do **not** invalidate the separately inventoried and hashed `CD_Extracted` directories. Strahd has no comparable original-source CSV in its game directory; its preserved bytes are independently hashed but cannot be certified against an absent original-source hash list.

## Cross-title filename presence (all Git-tree files)

Basenames are compared **case-insensitively**, with duplicates collapsed *within each title*. This does not equate equal names with equal bytes, gameplay semantics or equivalent relative directories.

| In all three | In exactly two | In exactly one |
| ---: | ---: | ---: |
| 71 | 16 | 206 |

| Basename | Titles present |
| --- | --- |
| `A32ADLIB.DLL` | menzo, stone, strahd |
| `A32ALGDG.DLL` | menzo, stone, strahd |
| `A32ALGFM.DLL` | menzo, stone, strahd |
| `A32ARDG.DLL` | menzo, stone, strahd |
| `A32ARXM.DLL` | menzo, stone, strahd |
| `A32GFX.DLL` | menzo, stone |
| `A32MT32.DLL` | menzo, stone, strahd |
| `A32PASDG.DLL` | menzo, stone, strahd |
| `A32PASFM.DLL` | menzo, stone, strahd |
| `A32PASOP.DLL` | menzo, stone, strahd |
| `A32SBDG.DLL` | menzo, stone, strahd |
| `A32SBFM.DLL` | menzo, stone, strahd |
| `A32SBPDG.DLL` | menzo, stone, strahd |
| `A32SP1FM.DLL` | menzo, stone, strahd |
| `A32SP2FM.DLL` | menzo, stone, strahd |
| `A32SPKR.DLL` | menzo, stone, strahd |
| `A32TANDY.DLL` | menzo, stone, strahd |
| `AUTHORS.TXT` | menzo, stone, strahd |
| `AUTO011.MAP` | strahd |
| `AUTO050.MAP` | strahd |
| `AWAKE.ACT` | stone |
| `AWAKE.DFA` | stone |
| `BATTLE-1.ACT` | stone |
| `BATTLE-1.DFA` | stone |
| `BATTLE-2.ACT` | stone |
| `BATTLE-2.DFA` | stone |
| `BATTLE-3.ACT` | stone |
| `BATTLE-3.DFA` | stone |
| `BATTLE-4.ACT` | stone |
| `BATTLE-4.DFA` | stone |
| `BOOK ONE.PDF` | menzo |
| `BOOT.NFO` | menzo, stone |
| `C` | menzo |
| `CARD-M1.ACT` | stone |
| `CARD-M1.DFA` | stone |
| `CD_EXTRACTION_STATUS.MD` | strahd |
| `CGEN.ACT` | stone |
| `CGEN.DFA` | stone |
| `CGEN00.DFA` | menzo |
| `CGEN01.DFA` | menzo |
| `CGEN02.DFA` | menzo |
| `CGEN03.DFA` | menzo |
| `CGEN04.DFA` | menzo |
| `CGEN05.DFA` | menzo |
| `CGEN06.DFA` | menzo |
| `CGEN07.DFA` | menzo |
| `CGEN08.DFA` | menzo |
| `CGEN09.DFA` | menzo |
| `CGEN10.DFA` | menzo |
| `CGEN11.DFA` | menzo |
| `CGEN12.DFA` | menzo |
| `CGEN13.DFA` | menzo |
| `CGEN14.DFA` | menzo |
| `CGEN15.DFA` | menzo |
| `CGEN16.DFA` | menzo |
| `CGEN17.DFA` | menzo |
| `CGEN18.DFA` | menzo |
| `CGEN19.DFA` | menzo |
| `CGEN20.DFA` | menzo |
| `CGEN21.DFA` | menzo |
| `CGEN22.DFA` | menzo |
| `CGEN23.DFA` | menzo |
| `CGEN24.DFA` | menzo |
| `CGEN25.DFA` | menzo |
| `CGEN26.DFA` | menzo |
| `CGEN27.DFA` | menzo |
| `CGEN28.DFA` | menzo |
| `CGEN29.DFA` | menzo |
| `CGEN30.DFA` | menzo |
| `CGEN31.DFA` | menzo |
| `CGEN32.DFA` | menzo |
| `CGEN33.DFA` | menzo |
| `CGEN34.DFA` | menzo |
| `CGEN35.DFA` | menzo |
| `CGEN36.DFA` | menzo |
| `CGEN37.DFA` | menzo |
| `CGEN38.DFA` | menzo |
| `CGEN39.DFA` | menzo |
| `CGEN40.DFA` | menzo |
| `CGEN41.DFA` | menzo |
| `CGEN50.DFA` | menzo |
| `CGEN51.DFA` | menzo |
| `CGEN52.DFA` | menzo |
| `CGEN53.DFA` | menzo |
| `CHARS` | strahd |
| `CINE00.ANM` | strahd |
| `CINE01.ANM` | strahd |
| `CINE02.ANM` | strahd |
| `CINE03.ANM` | strahd |
| `CINE04.ANM` | strahd |
| `CINE05.ANM` | strahd |
| `CINE06.ANM` | strahd |
| `CINE07.ANM` | strahd |
| `CINE08.ANM` | strahd |
| `CINE09.ANM` | strahd |
| `CINE10.ANM` | strahd |
| `CINE11.ANM` | strahd |
| `CINE12.ANM` | strahd |
| `CINEM10.DFA` | menzo |
| `CINEM13.DFA` | menzo |
| `CINEM3.DFA` | menzo |
| `CINEM4.DFA` | menzo |
| `CINEM5.DFA` | menzo |
| `CINEM6.DFA` | menzo |
| `CINEM8.DFA` | menzo |
| `CINEM9.DFA` | menzo |
| `CLUEBOOK.PDF` | menzo, stone, strahd |
| `CODE.1` | menzo, stone, strahd |
| `CODE.2` | menzo, stone, strahd |
| `COLORS` | menzo, stone, strahd |
| `COPYING.TXT` | menzo, stone, strahd |
| `DATA.A00` | menzo, stone |
| `DATA.A01` | menzo |
| `DBOVERLAY_DEL_CHARS` | strahd |
| `DEARJ.EXE` | menzo, stone |
| `DEATH.ACT` | stone |
| `DEATH.DFA` | menzo, stone |
| `DOSBOX 0.74-2 MANUAL.TXT` | menzo, stone, strahd |
| `DOSBOX_MENZO_SINGLE.CONF` | menzo |
| `DOSBOX_MENZO.CONF` | menzo |
| `DOSBOX_PROPHET_SINGLE.CONF` | stone |
| `DOSBOX_PROPHET.CONF` | stone |
| `DOSBOX_STRAHD_SINGLE.CONF` | strahd |
| `DOSBOX_STRAHD.CONF` | strahd |
| `DOSBOX-0.74-2.1.TAR.GZ` | menzo, stone, strahd |
| `DOSBOX.EXE` | menzo, stone, strahd |
| `DOSDIS-CANDIDATE-ANALYSIS.JSON` | stone, strahd |
| `END-A.DFA` | menzo |
| `END-B.DFA` | menzo |
| `FAIL-1.ACT` | stone |
| `FAIL-1.DFA` | stone |
| `FAIL-2.ACT` | stone |
| `FAIL-2.DFA` | stone |
| `FAIL-3.ACT` | stone |
| `FAIL-3.DFA` | stone |
| `FAIL-4.ACT` | stone |
| `FAIL-4.DFA` | stone |
| `FALCON-D.ACT` | stone |
| `FALCON-D.DFA` | stone |
| `FILESPEC` | stone |
| `GAME.OPT` | strahd |
| `GM1.BNK` | menzo, stone, strahd |
| `GM2.BNK` | menzo, stone, strahd |
| `GOG.ICO` | menzo, stone, strahd |
| `GOGDOSCONFIG.EXE` | menzo, stone, strahd |
| `GOGGAME-1432578339.HASHDB` | menzo |
| `GOGGAME-1432578339.ICO` | menzo |
| `GOGGAME-1432578339.INFO` | menzo |
| `GOGGAME-1432578339.SCRIPT` | menzo |
| `GOGGAME-1432905053.HASHDB` | strahd |
| `GOGGAME-1432905053.ICO` | strahd |
| `GOGGAME-1432905053.INFO` | strahd |
| `GOGGAME-1432905053.SCRIPT` | strahd |
| `GOGGAME-1432906190.HASHDB` | stone |
| `GOGGAME-1432906190.ICO` | stone |
| `GOGGAME-1432906190.ID` | stone |
| `GOGGAME-1432906190.INFO` | stone |
| `GOGGAME-1432906190.SCRIPT` | stone |
| `GOGGAME-GALAXYFILELIST.INI` | menzo, stone, strahd |
| `GOGLOG.INI` | menzo, stone, strahd |
| `GUSDIG32.DLL` | menzo, stone, strahd |
| `GUSMID32.DLL` | menzo, stone, strahd |
| `HELP.TXT` | stone |
| `HISTORY.ACT` | stone |
| `HISTORY.DFA` | stone |
| `INFO.DAT` | menzo, strahd |
| `INIT` | menzo, stone, strahd |
| `INSTALL.EXE` | menzo, stone, strahd |
| `INSTALL.NFO` | menzo, stone |
| `INSTALL.TXT` | menzo, stone, strahd |
| `INSTALL1.BAK` | stone |
| `LAUNCH MENZOBERRANZAN.LNK` | menzo |
| `LAUNCH RAVENLOFT - STONE PROPHET.LNK` | stone |
| `LAUNCH RAVENLOFT - STRAHD'S POSSESSION.LNK` | strahd |
| `LEV001.DAT` | strahd |
| `LEV005.DAT` | strahd |
| `LEV007.DAT` | strahd |
| `LEV010.DAT` | strahd |
| `LEV011.DAT` | strahd |
| `LEV012.DAT` | strahd |
| `LEV018.DAT` | strahd |
| `LEV019.DAT` | strahd |
| `LEV020.DAT` | strahd |
| `LEV021.DAT` | strahd |
| `LEV022.DAT` | strahd |
| `LEV023.DAT` | strahd |
| `LEV024.DAT` | strahd |
| `LEV025.DAT` | strahd |
| `LEV026.DAT` | strahd |
| `LEV027.DAT` | strahd |
| `LEV030.DAT` | strahd |
| `LEV031.DAT` | strahd |
| `LEVELS.DAT` | menzo, stone, strahd |
| `LEVINFO` | stone |
| `LOADPATS.EXE` | menzo, stone, strahd |
| `LOGOS.ACT` | stone |
| `LOGOS.DFA` | menzo, stone |
| `M.EXE` | menzo |
| `MAKSAV.BAT` | strahd |
| `MANUAL.PDF` | menzo, stone, strahd |
| `NEWS.TXT` | menzo, stone, strahd |
| `NPCS.DAT` | menzo, stone, strahd |
| `OBJECTS.DAT` | menzo, stone, strahd |
| `OPEN-A.DFA` | menzo |
| `OPEN-B.DFA` | menzo |
| `OPEN-C.DFA` | menzo |
| `OPEN.ACT` | stone |
| `OPEN.DFA` | stone |
| `PATCHES.INI` | menzo, stone, strahd |
| `PROPHET.BAT` | stone |
| `PROPHET.ZIP` | stone |
| `R.EXE` | strahd |
| `RAYS.ACT` | stone |
| `RAYS.DFA` | stone |
| `README_1.1.TXT` | stone |
| `README.BAT` | stone, strahd |
| `README.MD` | menzo, stone |
| `README.TXT` | menzo, stone, strahd |
| `RES0` | menzo, stone, strahd |
| `RES1` | menzo, stone, strahd |
| `RES10` | menzo, stone, strahd |
| `RES11` | strahd |
| `RES12` | strahd |
| `RES2` | menzo, stone, strahd |
| `RES3` | menzo, stone, strahd |
| `RES4` | menzo, stone, strahd |
| `RES5` | menzo, stone, strahd |
| `RES6` | menzo, stone, strahd |
| `RES7` | menzo, stone, strahd |
| `RES8` | menzo, stone, strahd |
| `RES9` | menzo, stone, strahd |
| `RLOFT.BAT` | strahd |
| `SAMPLE.AD` | menzo, stone, strahd |
| `SAVE.DAT` | menzo, stone, strahd |
| `SDL_NET.DLL` | menzo, stone, strahd |
| `SDL.DLL` | menzo, stone, strahd |
| `SHIP.ACT` | stone |
| `SHIP1.ACT` | stone |
| `SHIP1.DFA` | stone |
| `SHIP2.ACT` | stone |
| `SHIP2.DFA` | stone |
| `SINSTALL.EXE` | strahd |
| `SINTAB` | menzo, stone, strahd |
| `SLAB-D.ACT` | stone |
| `SLAB-D.DFA` | stone |
| `SLAB-N.ACT` | stone |
| `SLAB-N.DFA` | stone |
| `SOUND_PM.EXE` | menzo, stone |
| `SOUND.BAT` | stone, strahd |
| `SOUND.CFG` | menzo, stone, strahd |
| `SOUND.INI` | menzo, stone |
| `SOURCE_INVENTORY.CSV` | menzo, stone |
| `SP.EXE` | stone |
| `STATUS.ME` | stone, strahd |
| `STDERR.TXT` | strahd |
| `STDOUT.TXT` | strahd |
| `SUPPORT.ICO` | menzo, stone, strahd |
| `T` | menzo, stone, strahd |
| `TABLES` | stone |
| `TEARS.ACT` | stone |
| `TEARS.DFA` | stone |
| `THANKS.TXT` | menzo, stone, strahd |
| `UNINS000.DAT` | menzo, stone, strahd |
| `UNINS000.EXE` | menzo, stone, strahd |
| `UNINS000.INI` | menzo, stone, strahd |
| `UNINS000.MSG` | menzo, stone, strahd |
| `URCHSPK.DFA` | stone |
| `VIDEO INSTRUCTIONS.TXT` | menzo, stone, strahd |
| `WATER-D.ACT` | stone |
| `WATER-D.DFA` | stone |
| `WATER-N.ACT` | stone |
| `WATER-N.DFA` | stone |
| `WIN-1.ACT` | stone |
| `WIN-1.DFA` | stone |
| `WIN-1B.ACT` | stone |
| `WIN-1B.DFA` | stone |
| `WIN-2.ACT` | stone |
| `WIN-2.DFA` | stone |
| `WIN-2B.ACT` | stone |
| `WIN-2B.DFA` | stone |
| `WIN-3.ACT` | stone |
| `WIN-3.DFA` | stone |
| `WIN-3B.ACT` | stone |
| `WIN-3B.DFA` | stone |
| `WIN-4.ACT` | stone |
| `WIN-4.DFA` | stone |
| `WIN-4B.ACT` | stone |
| `WIN-4B.DFA` | stone |
| `WINCAP1.DFA` | stone |
| `WINCAP2.DFA` | stone |
| `WINCAP3.DFA` | stone |
| `ZMBV.DLL` | menzo, stone, strahd |
| `ZMBV.INF` | menzo, stone, strahd |

## Independently hashed supplied Strahd files

These eight files were independently hashed from the user-provided attachments and cross-checked against the CI checkout metadata. Seven have byte-identical corresponding extracted/installed game files where present. The uploaded `INFO.DAT` is **17 bytes**, SHA-256 `4ffa6ac5d5b5afd103bd67d84d3a372b401b3b19407b0c68c44bb0dfd9e32c30`; the installed Strahd `RLOFT/INFO.DAT` is **23 bytes**, SHA-256 `5856c56521ecdd543e4bcfb12e939ab8d39698630b1513b27bd49702a7e05efc`. They are **not interchangeable**. Menzo's `INFO.DAT` happens to match the 17-byte attachment; that does not prove the files have the same provenance.

| Local filename | Bytes | Locally computed SHA-256 |
| --- | ---: | --- |
| `CODE.1` | 277787 | `fd9b709ceee190e6a70870f9012ebea3bc437cae6bc74e0150d6f922fa750fed` |
| `CODE.2` | 558689 | `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314` |
| `INFO.DAT` | 17 | `4ffa6ac5d5b5afd103bd67d84d3a372b401b3b19407b0c68c44bb0dfd9e32c30` |
| `LEV022.DAT` | 6028 | `f88b033ff4cdafdd89620e0fac51ff2fb81bce103a6d31a26b91464532c9f51e` |
| `LEVELS.DAT` | 499878 | `50d0b1d13f81733a1ac55296d00fcba9aef002202bc00341f1062c37fd017d91` |
| `NPCS.DAT` | 5731 | `f2dd1175bf42e8291b95952282dfc0cfb84455aaa532987cdd7380094c8eb30b` |
| `OBJECTS.DAT` | 21844 | `f23f0e7c1856904aa0156b0bf174cab794a41a327352b6ca00c3591ae5f2eed0` |
| `SAVE.DAT` | 9288 | `f898cb92fca6447954f5e0b2bf6f8ad2435e4a64ae3b9de74f8bf75e0ed10d5d` |

Command: `sha256sum /mnt/data/{CODE.1,CODE.2,INFO.DAT,LEV022.DAT,LEVELS.DAT,NPCS.DAT,OBJECTS.DAT,SAVE.DAT}`. Scope: only these exact uploaded files. **Confidence: VERIFIED** for these checksums.

## Strahd existing-analyzer baseline

Four original analyzers were fetched from `Ravenloft-Enhanced/DatFiles`, copied without edits into a temporary test checkout, and validated by matching Git blob SHA-1 hashes before execution. The new driver launches them through subprocess without importing or changing any of their format logic.

| Unchanged analyzer | Git blob SHA-1 | Observed result |
| --- | --- | --- |
| `DatFiles/INFO.DAT/inspect_info.py` | `d4051723ff09fbf9b3bc1f8e558ef3f3086af277` | PASS; 17-byte INFO; 13 selectors; path length 2; byte-identical re-encode |
| `DatFiles/LEVELS.DAT/analyze_event_scripts.py` | `f68137d703ffd38551418f74ddcd1b7e539486b7` | PASS; 32 levels; 1,600 physical script slots; 394 active slots; 555 active commands |
| `DatFiles/NPCS.DAT/analyze_behavior_flags.py` | `885cf1b63d9cbe607b387d2bccf30a3bcdea0345` | PASS; 83 records of 0x45 bytes |
| `DatFiles/OBJECTS.DAT/analyze_attack_profile.py` | `ddbf8b9615ab219819475c4c554ae6a38a0f5613` | PASS; 336 records of 0x41 bytes; known class-2 name-set check passed |

The first four bytes of each `LEVELS.DAT`, `NPCS.DAT` and `OBJECTS.DAT` represent a **uint32 little-endian record count**, sampled respectively as 32, 83, and 336; these results follow the unmodified analyzers. `INFO.DAT` offset `0x0D..0x0E` is a **signed int16 little-endian resource-path length**, decoded as 2. **Confidence: LIKELY for this supplied Strahd build**, not a cross-game structural verification.

Actual baseline command (with temporary working tree containing the unchanged analyzers and symlinks to the four supplied input files):

```sh
python Tools/phase0_inventory.py --title strahd --media /mnt/data/phase0/strahd-sample --baseline --output /mnt/data/phase0/baseline.json
```

To reproduce from a real repository checkout, point `--media` at a *single* Strahd game-data directory with one instance of each required file, for example:

```sh
python Tools/phase0_inventory.py --title strahd --media '/path/to/RLOFT' --baseline --output strahd.json
python Tools/phase0_inventory.py --title menzo --media '/path/to/Menzoberranzan' --output menzo.json
python Tools/phase0_inventory.py --title stone --media '/path/to/PROPHET' --output stone.json
python Tools/phase0_inventory.py --title strahd --media '/path/to/RLOFT' --compare-other menzo.json --compare-other stone.json --output comparison.json
```

The driver accepts a directory or ZIP, hashes full file contents incrementally, classifies roles, writes machine-readable JSON, and compares basename presence using two other saved manifests. ZIP inputs support inventory but not the unchanged analyzers; 7z inputs require extracting to a separate read-only working directory. Do **not** point `--media` at a mixed parent folder if duplicate basename ambiguity would make analyzer input selection unsafe.

## Full Git-tree file inventory

Below, `source SHA-256` is the expected hash from the original source-archive `SOURCE_INVENTORY.csv`. **PASS** means CI independently hashed the actual checked-out bytes and they matched; **MISMATCH** means the checked-out bytes differ; **HASHED; NO SOURCE REFERENCE** means the actual checkout hash is available in the JSON artifact but no independent source hash exists for comparison. Git blob SHA-1 records repository object identity only. All 624 actual per-file hashes are in the linked CI artifact.

| Title | Repository-relative media filename | Git bytes | Classified role | Source SHA-256 (unverified to Git) | SHA-256 audit status | Git blob SHA-1 |
| --- | --- | ---: | --- | --- | --- | --- |
| strahd | `CD_Extracted/RLOFT/A32ADLIB.DLL` | 19968 | executable | UNKNOWN | HASHED; NO SOURCE REFERENCE | `8e41fec4502ad9107afe298c9a063a5f79a6cc5e` |
| strahd | `CD_Extracted/RLOFT/A32ALGDG.DLL` | 10752 | executable | UNKNOWN | HASHED; NO SOURCE REFERENCE | `bce10f8e48e4468aa18065ddeb9081b39d7fc327` |
| strahd | `CD_Extracted/RLOFT/A32ALGFM.DLL` | 24576 | executable | UNKNOWN | HASHED; NO SOURCE REFERENCE | `6aaa2c5ac6561ac2f22002093d5b81cbd6f4d19d` |
| strahd | `CD_Extracted/RLOFT/A32ARDG.DLL` | 16384 | executable | UNKNOWN | HASHED; NO SOURCE REFERENCE | `812c1604898f49d4b8301ff2f1ff07fd397d2c27` |
| strahd | `CD_Extracted/RLOFT/A32ARXM.DLL` | 53760 | executable | UNKNOWN | HASHED; NO SOURCE REFERENCE | `a799ea0b185072b231a910bfe730d08bb6660bf9` |
| strahd | `CD_Extracted/RLOFT/A32MT32.DLL` | 14336 | executable | UNKNOWN | HASHED; NO SOURCE REFERENCE | `15c56e38da8d2ae724540e1f7d89cf1a19a47b3e` |
| strahd | `CD_Extracted/RLOFT/A32PASDG.DLL` | 10752 | executable | UNKNOWN | HASHED; NO SOURCE REFERENCE | `f7bd1aa4154964029cc4ef536de65b477fc0f559` |
| strahd | `CD_Extracted/RLOFT/A32PASFM.DLL` | 19968 | executable | UNKNOWN | HASHED; NO SOURCE REFERENCE | `8923737f84fe42ee0519a5aa86a334942b172a84` |
| strahd | `CD_Extracted/RLOFT/A32PASOP.DLL` | 24576 | executable | UNKNOWN | HASHED; NO SOURCE REFERENCE | `dfc67837198693a878eeec08037e212f6e810c11` |
| strahd | `CD_Extracted/RLOFT/A32SBDG.DLL` | 10752 | executable | UNKNOWN | HASHED; NO SOURCE REFERENCE | `d19e4886dae560233c3ef93791350ec4899d3f73` |
| strahd | `CD_Extracted/RLOFT/A32SBFM.DLL` | 19968 | executable | UNKNOWN | HASHED; NO SOURCE REFERENCE | `4d40c5e034721b6ab7d208eb24a13840a5aeb103` |
| strahd | `CD_Extracted/RLOFT/A32SBPDG.DLL` | 10752 | executable | UNKNOWN | HASHED; NO SOURCE REFERENCE | `3453cb1b9751668d607aec44dc02fce412e9cf5c` |
| strahd | `CD_Extracted/RLOFT/A32SP1FM.DLL` | 19968 | executable | UNKNOWN | HASHED; NO SOURCE REFERENCE | `245be4ba908962aacbd7669b025ae744d989b492` |
| strahd | `CD_Extracted/RLOFT/A32SP2FM.DLL` | 24576 | executable | UNKNOWN | HASHED; NO SOURCE REFERENCE | `4c4d28af777ccba3e63b40c5532e08a59e367682` |
| strahd | `CD_Extracted/RLOFT/A32SPKR.DLL` | 14848 | executable | UNKNOWN | HASHED; NO SOURCE REFERENCE | `31a7c22f8a88562ef19273e01998102c1f739845` |
| strahd | `CD_Extracted/RLOFT/A32TANDY.DLL` | 14848 | executable | UNKNOWN | HASHED; NO SOURCE REFERENCE | `038971cb960f912df4f9e50f09e870d83923bae8` |
| strahd | `CD_Extracted/RLOFT/CINE00.ANM` | 13781936 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `e7616d72b69416a0202766b3f1ff247074a4d0e5` |
| strahd | `CD_Extracted/RLOFT/CINE01.ANM` | 16009364 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `78be7463f073edbbaed6ef63845ca84ac408bd32` |
| strahd | `CD_Extracted/RLOFT/CINE02.ANM` | 7498347 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `57e568e60aac14b7dc2c34405166657e1eb24829` |
| strahd | `CD_Extracted/RLOFT/CINE03.ANM` | 7498347 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `559d499b89851ee80d652901e32ba0757d68207b` |
| strahd | `CD_Extracted/RLOFT/CINE04.ANM` | 7498347 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `04402b69e2b011745ec591952ad81d58bb11121a` |
| strahd | `CD_Extracted/RLOFT/CINE05.ANM` | 7498347 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `0274ab1a866a7123b8b9e81e2a4e76eb16dffe4c` |
| strahd | `CD_Extracted/RLOFT/CINE06.ANM` | 7339821 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `bf5eca487cee55eb9a591fb442ff63dfed6d93c9` |
| strahd | `CD_Extracted/RLOFT/CINE07.ANM` | 34569939 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `4db0a5d7033ee89b73f5c78a298ffa96f053313d` |
| strahd | `CD_Extracted/RLOFT/CINE08.ANM` | 9255472 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `54a8e4535e099a06c4abd5cd2ed34a0465b3c901` |
| strahd | `CD_Extracted/RLOFT/CINE09.ANM` | 5485203 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `de809fb13e40ef4df9432c949f6e1a4b4b5b8759` |
| strahd | `CD_Extracted/RLOFT/CINE10.ANM` | 56273227 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `e19991bc259a1501304cb34f2c4fd0d23904b26a` |
| strahd | `CD_Extracted/RLOFT/CINE11.ANM` | 57092507 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `28e4eec90b1c4f70eaef5f9de7653d1d48c265cb` |
| strahd | `CD_Extracted/RLOFT/CINE12.ANM` | 1446666 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `081be687909ed78828d1761180121a0f5291bdc6` |
| strahd | `CD_Extracted/RLOFT/CODE.1` | 277787 | executable | UNKNOWN | HASHED; NO SOURCE REFERENCE | `430407e93e55ee7873f6e6ece68ced309389334e` |
| strahd | `CD_Extracted/RLOFT/CODE.2` | 558689 | executable | UNKNOWN | HASHED; NO SOURCE REFERENCE | `70d8b2e27a38c481153b6937493575797054bb58` |
| strahd | `CD_Extracted/RLOFT/COLORS` | 11521 | resource archive | UNKNOWN | HASHED; NO SOURCE REFERENCE | `452733d3d755e26a6a62baefbb100bdaa4ba37da` |
| strahd | `CD_Extracted/RLOFT/GM1.BNK` | 23907 | audio | UNKNOWN | HASHED; NO SOURCE REFERENCE | `0b1037e22a21be99811b605081e254c2a9681f43` |
| strahd | `CD_Extracted/RLOFT/GM2.BNK` | 24273 | audio | UNKNOWN | HASHED; NO SOURCE REFERENCE | `aa4eb15b776535805fceddc11d81514c96974272` |
| strahd | `CD_Extracted/RLOFT/GUSDIG32.DLL` | 28959 | executable | UNKNOWN | HASHED; NO SOURCE REFERENCE | `2c2b2799675e53402062325878cbbac5ab0a1c9d` |
| strahd | `CD_Extracted/RLOFT/GUSMID32.DLL` | 59865 | executable | UNKNOWN | HASHED; NO SOURCE REFERENCE | `ee0eb08179f80e99dda7e6fc4a10b3d0e1dab010` |
| strahd | `CD_Extracted/RLOFT/INIT` | 1879 | config | UNKNOWN | HASHED; NO SOURCE REFERENCE | `aeb2e1575879047d64d6cc961c5ea59043b555a4` |
| strahd | `CD_Extracted/RLOFT/INSTALL.EXE` | 26400 | executable | UNKNOWN | HASHED; NO SOURCE REFERENCE | `db47f0dee6876687fbece4663e71835a256ef677` |
| strahd | `CD_Extracted/RLOFT/LEV022.DAT` | 6028 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `2ac76db12beecb9479a21aa663534ee8fb9f5981` |
| strahd | `CD_Extracted/RLOFT/LEVELS.DAT` | 499878 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `a26401dfa462f324afbc02ec3b0151629ce6479c` |
| strahd | `CD_Extracted/RLOFT/LOADPATS.EXE` | 24705 | executable | UNKNOWN | HASHED; NO SOURCE REFERENCE | `55537c435d49520cb0b11370fb7f0a3230ce51d4` |
| strahd | `CD_Extracted/RLOFT/MAKSAV.BAT` | 123 | executable | UNKNOWN | HASHED; NO SOURCE REFERENCE | `596f48da4eeb0ac671818e59a78c551280afb1a5` |
| strahd | `CD_Extracted/RLOFT/NPCS.DAT` | 5731 | resource archive | UNKNOWN | HASHED; NO SOURCE REFERENCE | `f8d0e85a0f3146c227c50398c07586c6254f780f` |
| strahd | `CD_Extracted/RLOFT/OBJECTS.DAT` | 21844 | resource archive | UNKNOWN | HASHED; NO SOURCE REFERENCE | `e52b52aa716ce2024404be65e752e7698e32248f` |
| strahd | `CD_Extracted/RLOFT/PATCHES.INI` | 6952 | config | UNKNOWN | HASHED; NO SOURCE REFERENCE | `3d9b15de08ad25217618f7dd0046fdbb13f64c1b` |
| strahd | `CD_Extracted/RLOFT/R.EXE` | 12368 | executable | UNKNOWN | HASHED; NO SOURCE REFERENCE | `16eca25db273fc4f8eb0b4a76123a53cc5d92257` |
| strahd | `CD_Extracted/RLOFT/README.BAT` | 33 | executable | UNKNOWN | HASHED; NO SOURCE REFERENCE | `4dec73bb2d5803b7293d05d38f0f8b2056da24a1` |
| strahd | `CD_Extracted/RLOFT/README.TXT` | 6295 | text | UNKNOWN | HASHED; NO SOURCE REFERENCE | `a942e7f3cd177a6175cc4202ed7a4a4b9d57cd32` |
| strahd | `CD_Extracted/RLOFT/RES0` | 2186991 | resource archive | UNKNOWN | HASHED; NO SOURCE REFERENCE | `23e279f49dab8a2bd7824715f0f417fc5fe2afb7` |
| strahd | `CD_Extracted/RLOFT/RES1` | 354310 | resource archive | UNKNOWN | HASHED; NO SOURCE REFERENCE | `1ae6ed5af76f8efb3e0e26ff05d5582c7ce1dab4` |
| strahd | `CD_Extracted/RLOFT/RES10` | 46060354 | resource archive | UNKNOWN | HASHED; NO SOURCE REFERENCE | `18ca392b73687d038e681680040b64a47028f973` |
| strahd | `CD_Extracted/RLOFT/RES11` | 15589639 | resource archive | UNKNOWN | HASHED; NO SOURCE REFERENCE | `38a09fd9a1acdcfe81c68b026a2d3482656842a4` |
| strahd | `CD_Extracted/RLOFT/RES12` | 12150551 | resource archive | UNKNOWN | HASHED; NO SOURCE REFERENCE | `27ce45061bc0716599f80bcaef78bf9c68973945` |
| strahd | `CD_Extracted/RLOFT/RES2` | 63736 | resource archive | UNKNOWN | HASHED; NO SOURCE REFERENCE | `f2834dcb8f282d949e07f38ba3a642a2986cc52f` |
| strahd | `CD_Extracted/RLOFT/RES3` | 5228638 | resource archive | UNKNOWN | HASHED; NO SOURCE REFERENCE | `8bcae74169185139b08138a16814558879f6370d` |
| strahd | `CD_Extracted/RLOFT/RES4` | 1975542 | resource archive | UNKNOWN | HASHED; NO SOURCE REFERENCE | `30d1b3377ddddd0e9ae44f6dc2fa9ca1d7f0fad0` |
| strahd | `CD_Extracted/RLOFT/RES5` | 1510960 | resource archive | UNKNOWN | HASHED; NO SOURCE REFERENCE | `3b3c48c971a58b3cadd072d3fa83291e127753be` |
| strahd | `CD_Extracted/RLOFT/RES6` | 6210899 | resource archive | UNKNOWN | HASHED; NO SOURCE REFERENCE | `f6aaaaa85f8273dc2cf534237d934b0fea2f5119` |
| strahd | `CD_Extracted/RLOFT/RES7` | 211133 | resource archive | UNKNOWN | HASHED; NO SOURCE REFERENCE | `91bdf1aba59a4022e5b8e81c9570cb5215c7fcd6` |
| strahd | `CD_Extracted/RLOFT/RES8` | 58772 | resource archive | UNKNOWN | HASHED; NO SOURCE REFERENCE | `43a247cea108803a38741ab1aa48587972aba460` |
| strahd | `CD_Extracted/RLOFT/RES9` | 8061481 | resource archive | UNKNOWN | HASHED; NO SOURCE REFERENCE | `4a0b748ae33835015fe2941c4329c99f697df9b8` |
| strahd | `CD_Extracted/RLOFT/RLOFT.BAT` | 2632 | executable | UNKNOWN | HASHED; NO SOURCE REFERENCE | `51a1da2200d837da2b03b4b6443823f7192cb1d4` |
| strahd | `CD_Extracted/RLOFT/SAMPLE.AD` | 4662 | audio | UNKNOWN | HASHED; NO SOURCE REFERENCE | `bfccc365b141d38f2f31bd528c918dea396a6bc0` |
| strahd | `CD_Extracted/RLOFT/SAVE.DAT` | 9288 | save | UNKNOWN | HASHED; NO SOURCE REFERENCE | `e2783e4316bc94281491277c90527cc2c5454ab7` |
| strahd | `CD_Extracted/RLOFT/SINSTALL.EXE` | 381660 | executable | UNKNOWN | HASHED; NO SOURCE REFERENCE | `e3236fdb760020bd6a171a046907f57b951b1e31` |
| strahd | `CD_Extracted/RLOFT/SINTAB` | 333757 | resource archive | UNKNOWN | HASHED; NO SOURCE REFERENCE | `91ce7412034e716383d74bd11768fe26487b2f18` |
| strahd | `CD_Extracted/RLOFT/SOUND.BAT` | 2662 | executable | UNKNOWN | HASHED; NO SOURCE REFERENCE | `1c6235f9a608ac4aa551483b2a11a9321c5a935e` |
| strahd | `CD_Extracted/RLOFT/T` | 133584 | resource archive | UNKNOWN | HASHED; NO SOURCE REFERENCE | `68cde745e55de1def1b6ce828f1455d38c6f483b` |
| strahd | `CD_Extracted/STATUS.ME` | 972 | UNKNOWN | UNKNOWN | HASHED; NO SOURCE REFERENCE | `cf167dfab18d71b979fe0d3ea11062abdcd0da5d` |
| strahd | `CD_EXTRACTION_STATUS.md` | 3028 | text | UNKNOWN | HASHED; NO SOURCE REFERENCE | `6db535b13668b05b865834a2068444da55885e08` |
| strahd | `cloud_saves/RLOFT/AUTOMAPS/AUTO011.MAP` | 19509 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `079e98542b220d8e7b04c67a29c924b314bac266` |
| strahd | `cloud_saves/RLOFT/AUTOMAPS/AUTO050.MAP` | 4798 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `f4fe573bfe2b254771920b7315e5bf6f8fc6f7ae` |
| strahd | `cloud_saves/RLOFT/DBOVERLAY_DEL_CHARS` | 5 | UNKNOWN | UNKNOWN | HASHED; NO SOURCE REFERENCE | `7b4d68d70fcae134d5348f5e118f5e9c9d3f05f6` |
| strahd | `cloud_saves/RLOFT/GAME.OPT` | 14 | config | UNKNOWN | HASHED; NO SOURCE REFERENCE | `eaf6788f1c555ed364bd03740d455878e1063d43` |
| strahd | `cloud_saves/RLOFT/SAVE00/LEV001.DAT` | 7723 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `2016d31dc4c86b7eb302ca199d6ceec004047278` |
| strahd | `cloud_saves/RLOFT/SAVE00/LEV005.DAT` | 22278 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `b218b30b495470b9c1e76a7c86297323f699fe61` |
| strahd | `cloud_saves/RLOFT/SAVE00/LEV010.DAT` | 65052 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `534449ce3d830aeb3ef9b5aedaf61609c5979c9a` |
| strahd | `cloud_saves/RLOFT/SAVE00/LEV011.DAT` | 58696 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `a1523cff7e41bbefb2096971dea29331788c949a` |
| strahd | `cloud_saves/RLOFT/SAVE00/LEV012.DAT` | 19848 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `66ff788b63ab672d6eff3537ddef2eebe09a9bb0` |
| strahd | `cloud_saves/RLOFT/SAVE00/LEV018.DAT` | 19722 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `edaf2868092d35476115f3cb410c76928b3432a8` |
| strahd | `cloud_saves/RLOFT/SAVE00/LEV019.DAT` | 2996 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `2769f224f2ca0d3457c6080e8cf2ce1d53b56840` |
| strahd | `cloud_saves/RLOFT/SAVE00/LEV020.DAT` | 7548 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `b479735131d7bc38f04de5eeb16220f90a603a29` |
| strahd | `cloud_saves/RLOFT/SAVE00/LEV021.DAT` | 10740 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `cdffb77df5f34b22aa31e2214ac40f195ffddaba` |
| strahd | `cloud_saves/RLOFT/SAVE00/LEV022.DAT` | 6028 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `1055db8cbb2d142684115586860c1f5918ee9faa` |
| strahd | `cloud_saves/RLOFT/SAVE00/LEV023.DAT` | 20842 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `cf965670d3dc82adc41ca89d720e415528243cee` |
| strahd | `cloud_saves/RLOFT/SAVE00/LEV024.DAT` | 19071 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `5c5ecd02b5a19e5091550124d6b143ebacca0ae5` |
| strahd | `cloud_saves/RLOFT/SAVE00/LEV025.DAT` | 15864 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `9841155303c81c898fd5b190596f90d48205f812` |
| strahd | `cloud_saves/RLOFT/SAVE00/LEV026.DAT` | 20438 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `2248dd89e7f5c3a925347e5550c6de9e625f78a1` |
| strahd | `cloud_saves/RLOFT/SAVE00/LEV027.DAT` | 2834 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `51b349d6d0eabb2f647e2573b3cd5d13f43a7f97` |
| strahd | `cloud_saves/RLOFT/SAVE00/LEV030.DAT` | 4246 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `62138caf501e287e82bed54be487e2e1286d420c` |
| strahd | `cloud_saves/RLOFT/SAVE00/LEV031.DAT` | 14322 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `4086f1435cab91da1face650401a60e839ffff20` |
| strahd | `cloud_saves/RLOFT/SAVE00/SAVE.DAT` | 9288 | save | UNKNOWN | HASHED; NO SOURCE REFERENCE | `be801c87b629257a5ff9e814869c649e534ff4da` |
| strahd | `cloud_saves/RLOFT/SAVE01/LEV019.DAT` | 2952 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `46bd05666898e28944a3c87277083d525a302bb6` |
| strahd | `cloud_saves/RLOFT/SAVE01/LEV022.DAT` | 6138 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `8f25790726d3547c25c0877cac2748381b19c6bd` |
| strahd | `cloud_saves/RLOFT/SAVE01/LEV023.DAT` | 21018 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `34d0b8c37ea8055a62ec3ceed1f4976d8e74186e` |
| strahd | `cloud_saves/RLOFT/SAVE01/SAVE.DAT` | 9288 | save | UNKNOWN | HASHED; NO SOURCE REFERENCE | `ee48b2d6410f98c7ccc1e8d0bd0d6f394cbceadd` |
| strahd | `cloud_saves/RLOFT/SAVE02/LEV007.DAT` | 13326 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `8172ec5846a13a06becefa53c835aaab098edc0b` |
| strahd | `cloud_saves/RLOFT/SAVE02/LEV010.DAT` | 65052 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `24aebdc5bc7b067c19f94b3315ca5c25b5c16947` |
| strahd | `cloud_saves/RLOFT/SAVE02/LEV011.DAT` | 58756 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `090f879e09b515a52bf3cc38a4fc887a9d2d9266` |
| strahd | `cloud_saves/RLOFT/SAVE02/LEV019.DAT` | 2952 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `49289682df75647f537d066c37181b595294c930` |
| strahd | `cloud_saves/RLOFT/SAVE02/LEV020.DAT` | 7548 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `64aad9a65851d548f83a9a2ad7749c3be9f03ae9` |
| strahd | `cloud_saves/RLOFT/SAVE02/LEV021.DAT` | 10608 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `c26127acaa1f0687a5ed50393daa812d23df852f` |
| strahd | `cloud_saves/RLOFT/SAVE02/LEV022.DAT` | 6028 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `70888f7c5652f6f1476ba95e10fb3f1686eb6d15` |
| strahd | `cloud_saves/RLOFT/SAVE02/LEV023.DAT` | 20710 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `6e500906ac907b2b0554922752ed5d8a99605931` |
| strahd | `cloud_saves/RLOFT/SAVE02/LEV025.DAT` | 15798 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `785c9e14c4ae6b3ddae825a227174e1edf1ecdfc` |
| strahd | `cloud_saves/RLOFT/SAVE02/LEV026.DAT` | 20438 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `c793f09b8ebaf305a7684cd796f0b7059924e7eb` |
| strahd | `cloud_saves/RLOFT/SAVE02/SAVE.DAT` | 9288 | save | UNKNOWN | HASHED; NO SOURCE REFERENCE | `b0ef36357e3eb4b3a3cdafa3aae8da3f9bd6cede` |
| strahd | `cloud_saves/RLOFT/SAVE03/LEV001.DAT` | 7723 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `2016d31dc4c86b7eb302ca199d6ceec004047278` |
| strahd | `cloud_saves/RLOFT/SAVE03/LEV005.DAT` | 22372 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `cf84928c6f1b47d58b8035e11141258a616f7fae` |
| strahd | `cloud_saves/RLOFT/SAVE03/LEV010.DAT` | 65052 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `534449ce3d830aeb3ef9b5aedaf61609c5979c9a` |
| strahd | `cloud_saves/RLOFT/SAVE03/LEV011.DAT` | 58696 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `a1523cff7e41bbefb2096971dea29331788c949a` |
| strahd | `cloud_saves/RLOFT/SAVE03/LEV012.DAT` | 19848 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `66ff788b63ab672d6eff3537ddef2eebe09a9bb0` |
| strahd | `cloud_saves/RLOFT/SAVE03/LEV018.DAT` | 19722 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `edaf2868092d35476115f3cb410c76928b3432a8` |
| strahd | `cloud_saves/RLOFT/SAVE03/LEV019.DAT` | 2996 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `2769f224f2ca0d3457c6080e8cf2ce1d53b56840` |
| strahd | `cloud_saves/RLOFT/SAVE03/LEV020.DAT` | 7548 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `b479735131d7bc38f04de5eeb16220f90a603a29` |
| strahd | `cloud_saves/RLOFT/SAVE03/LEV021.DAT` | 10740 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `cdffb77df5f34b22aa31e2214ac40f195ffddaba` |
| strahd | `cloud_saves/RLOFT/SAVE03/LEV022.DAT` | 6028 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `1055db8cbb2d142684115586860c1f5918ee9faa` |
| strahd | `cloud_saves/RLOFT/SAVE03/LEV023.DAT` | 20842 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `cf965670d3dc82adc41ca89d720e415528243cee` |
| strahd | `cloud_saves/RLOFT/SAVE03/LEV024.DAT` | 19071 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `5c5ecd02b5a19e5091550124d6b143ebacca0ae5` |
| strahd | `cloud_saves/RLOFT/SAVE03/LEV025.DAT` | 15864 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `9841155303c81c898fd5b190596f90d48205f812` |
| strahd | `cloud_saves/RLOFT/SAVE03/LEV026.DAT` | 20438 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `2248dd89e7f5c3a925347e5550c6de9e625f78a1` |
| strahd | `cloud_saves/RLOFT/SAVE03/LEV027.DAT` | 2834 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `51b349d6d0eabb2f647e2573b3cd5d13f43a7f97` |
| strahd | `cloud_saves/RLOFT/SAVE03/LEV030.DAT` | 4246 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `62138caf501e287e82bed54be487e2e1286d420c` |
| strahd | `cloud_saves/RLOFT/SAVE03/LEV031.DAT` | 14322 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `4086f1435cab91da1face650401a60e839ffff20` |
| strahd | `cloud_saves/RLOFT/SAVE03/SAVE.DAT` | 9310 | save | UNKNOWN | HASHED; NO SOURCE REFERENCE | `e938db76efcaac772502f98ea646eceb62f482fe` |
| strahd | `cloud_saves/RLOFT/SAVE04/LEV001.DAT` | 7723 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `2016d31dc4c86b7eb302ca199d6ceec004047278` |
| strahd | `cloud_saves/RLOFT/SAVE04/LEV005.DAT` | 22007 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `ac09ee0e59314e53d3dd00586ba530232604f51b` |
| strahd | `cloud_saves/RLOFT/SAVE04/LEV010.DAT` | 65052 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `534449ce3d830aeb3ef9b5aedaf61609c5979c9a` |
| strahd | `cloud_saves/RLOFT/SAVE04/LEV011.DAT` | 58696 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `a1523cff7e41bbefb2096971dea29331788c949a` |
| strahd | `cloud_saves/RLOFT/SAVE04/LEV012.DAT` | 19848 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `66ff788b63ab672d6eff3537ddef2eebe09a9bb0` |
| strahd | `cloud_saves/RLOFT/SAVE04/LEV018.DAT` | 19722 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `edaf2868092d35476115f3cb410c76928b3432a8` |
| strahd | `cloud_saves/RLOFT/SAVE04/LEV019.DAT` | 2996 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `2769f224f2ca0d3457c6080e8cf2ce1d53b56840` |
| strahd | `cloud_saves/RLOFT/SAVE04/LEV020.DAT` | 7548 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `b479735131d7bc38f04de5eeb16220f90a603a29` |
| strahd | `cloud_saves/RLOFT/SAVE04/LEV021.DAT` | 10740 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `cdffb77df5f34b22aa31e2214ac40f195ffddaba` |
| strahd | `cloud_saves/RLOFT/SAVE04/LEV022.DAT` | 6028 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `1055db8cbb2d142684115586860c1f5918ee9faa` |
| strahd | `cloud_saves/RLOFT/SAVE04/LEV023.DAT` | 20842 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `cf965670d3dc82adc41ca89d720e415528243cee` |
| strahd | `cloud_saves/RLOFT/SAVE04/LEV024.DAT` | 19071 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `5c5ecd02b5a19e5091550124d6b143ebacca0ae5` |
| strahd | `cloud_saves/RLOFT/SAVE04/LEV025.DAT` | 15864 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `9841155303c81c898fd5b190596f90d48205f812` |
| strahd | `cloud_saves/RLOFT/SAVE04/LEV026.DAT` | 20438 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `2248dd89e7f5c3a925347e5550c6de9e625f78a1` |
| strahd | `cloud_saves/RLOFT/SAVE04/LEV027.DAT` | 2834 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `51b349d6d0eabb2f647e2573b3cd5d13f43a7f97` |
| strahd | `cloud_saves/RLOFT/SAVE04/LEV030.DAT` | 4246 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `62138caf501e287e82bed54be487e2e1286d420c` |
| strahd | `cloud_saves/RLOFT/SAVE04/LEV031.DAT` | 14322 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `4086f1435cab91da1face650401a60e839ffff20` |
| strahd | `cloud_saves/RLOFT/SAVE04/SAVE.DAT` | 9288 | save | UNKNOWN | HASHED; NO SOURCE REFERENCE | `a8d698b4f3eca2d9da35789e0ea73258d7a9b003` |
| strahd | `cloud_saves/RLOFT/SAVE08/LEV001.DAT` | 7723 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `2016d31dc4c86b7eb302ca199d6ceec004047278` |
| strahd | `cloud_saves/RLOFT/SAVE08/LEV005.DAT` | 21955 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `78c2e738cb3ab8636c5288ff521415a4b6c41a68` |
| strahd | `cloud_saves/RLOFT/SAVE08/LEV010.DAT` | 65052 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `534449ce3d830aeb3ef9b5aedaf61609c5979c9a` |
| strahd | `cloud_saves/RLOFT/SAVE08/LEV011.DAT` | 58696 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `2ed15b76364354049791bf990a3c8e138a499ae1` |
| strahd | `cloud_saves/RLOFT/SAVE08/LEV012.DAT` | 19848 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `66ff788b63ab672d6eff3537ddef2eebe09a9bb0` |
| strahd | `cloud_saves/RLOFT/SAVE08/LEV018.DAT` | 19722 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `edaf2868092d35476115f3cb410c76928b3432a8` |
| strahd | `cloud_saves/RLOFT/SAVE08/LEV019.DAT` | 2996 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `2769f224f2ca0d3457c6080e8cf2ce1d53b56840` |
| strahd | `cloud_saves/RLOFT/SAVE08/LEV020.DAT` | 7548 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `67b4e41144f28fa37e6a79be36073fa17630211b` |
| strahd | `cloud_saves/RLOFT/SAVE08/LEV021.DAT` | 10740 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `cdffb77df5f34b22aa31e2214ac40f195ffddaba` |
| strahd | `cloud_saves/RLOFT/SAVE08/LEV022.DAT` | 6028 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `1055db8cbb2d142684115586860c1f5918ee9faa` |
| strahd | `cloud_saves/RLOFT/SAVE08/LEV023.DAT` | 20842 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `cf965670d3dc82adc41ca89d720e415528243cee` |
| strahd | `cloud_saves/RLOFT/SAVE08/LEV024.DAT` | 19335 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `85e5cd30884fcc4e0bb7b4e24cebc4f7c337ae51` |
| strahd | `cloud_saves/RLOFT/SAVE08/LEV025.DAT` | 15864 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `9841155303c81c898fd5b190596f90d48205f812` |
| strahd | `cloud_saves/RLOFT/SAVE08/LEV026.DAT` | 20438 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `2248dd89e7f5c3a925347e5550c6de9e625f78a1` |
| strahd | `cloud_saves/RLOFT/SAVE08/LEV027.DAT` | 2834 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `51b349d6d0eabb2f647e2573b3cd5d13f43a7f97` |
| strahd | `cloud_saves/RLOFT/SAVE08/LEV030.DAT` | 4246 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `62138caf501e287e82bed54be487e2e1286d420c` |
| strahd | `cloud_saves/RLOFT/SAVE08/LEV031.DAT` | 14322 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `4086f1435cab91da1face650401a60e839ffff20` |
| strahd | `cloud_saves/RLOFT/SAVE08/SAVE.DAT` | 9288 | save | UNKNOWN | HASHED; NO SOURCE REFERENCE | `d7b1d75027b4bf09dde6137e9cd40b9e05e9bc75` |
| strahd | `cloud_saves/RLOFT/SAVE09/LEV001.DAT` | 7723 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `2016d31dc4c86b7eb302ca199d6ceec004047278` |
| strahd | `cloud_saves/RLOFT/SAVE09/LEV005.DAT` | 21911 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `17eb65b6cd96ebb36c26e601c3b3b03d74c4c8af` |
| strahd | `cloud_saves/RLOFT/SAVE09/LEV010.DAT` | 65052 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `534449ce3d830aeb3ef9b5aedaf61609c5979c9a` |
| strahd | `cloud_saves/RLOFT/SAVE09/LEV011.DAT` | 58696 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `2ed15b76364354049791bf990a3c8e138a499ae1` |
| strahd | `cloud_saves/RLOFT/SAVE09/LEV012.DAT` | 19848 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `66ff788b63ab672d6eff3537ddef2eebe09a9bb0` |
| strahd | `cloud_saves/RLOFT/SAVE09/LEV018.DAT` | 19722 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `edaf2868092d35476115f3cb410c76928b3432a8` |
| strahd | `cloud_saves/RLOFT/SAVE09/LEV019.DAT` | 2996 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `2769f224f2ca0d3457c6080e8cf2ce1d53b56840` |
| strahd | `cloud_saves/RLOFT/SAVE09/LEV020.DAT` | 7548 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `67b4e41144f28fa37e6a79be36073fa17630211b` |
| strahd | `cloud_saves/RLOFT/SAVE09/LEV021.DAT` | 10740 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `cdffb77df5f34b22aa31e2214ac40f195ffddaba` |
| strahd | `cloud_saves/RLOFT/SAVE09/LEV022.DAT` | 6028 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `1055db8cbb2d142684115586860c1f5918ee9faa` |
| strahd | `cloud_saves/RLOFT/SAVE09/LEV023.DAT` | 20842 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `cf965670d3dc82adc41ca89d720e415528243cee` |
| strahd | `cloud_saves/RLOFT/SAVE09/LEV024.DAT` | 19335 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `85e5cd30884fcc4e0bb7b4e24cebc4f7c337ae51` |
| strahd | `cloud_saves/RLOFT/SAVE09/LEV025.DAT` | 15864 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `9841155303c81c898fd5b190596f90d48205f812` |
| strahd | `cloud_saves/RLOFT/SAVE09/LEV026.DAT` | 20438 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `2248dd89e7f5c3a925347e5550c6de9e625f78a1` |
| strahd | `cloud_saves/RLOFT/SAVE09/LEV027.DAT` | 2834 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `51b349d6d0eabb2f647e2573b3cd5d13f43a7f97` |
| strahd | `cloud_saves/RLOFT/SAVE09/LEV030.DAT` | 4246 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `62138caf501e287e82bed54be487e2e1286d420c` |
| strahd | `cloud_saves/RLOFT/SAVE09/LEV031.DAT` | 14322 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `4086f1435cab91da1face650401a60e839ffff20` |
| strahd | `cloud_saves/RLOFT/SAVE09/SAVE.DAT` | 9310 | save | UNKNOWN | HASHED; NO SOURCE REFERENCE | `2f050250f01311695b7330b9be694ede0f33a181` |
| strahd | `cloud_saves/RLOFT/SAVE10/LEV001.DAT` | 7723 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `2016d31dc4c86b7eb302ca199d6ceec004047278` |
| strahd | `cloud_saves/RLOFT/SAVE10/LEV005.DAT` | 22278 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `18159c9edefddba90763cd06d1a898ee72831530` |
| strahd | `cloud_saves/RLOFT/SAVE10/LEV010.DAT` | 65052 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `534449ce3d830aeb3ef9b5aedaf61609c5979c9a` |
| strahd | `cloud_saves/RLOFT/SAVE10/LEV011.DAT` | 58696 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `a1523cff7e41bbefb2096971dea29331788c949a` |
| strahd | `cloud_saves/RLOFT/SAVE10/LEV012.DAT` | 19848 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `66ff788b63ab672d6eff3537ddef2eebe09a9bb0` |
| strahd | `cloud_saves/RLOFT/SAVE10/LEV018.DAT` | 19722 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `edaf2868092d35476115f3cb410c76928b3432a8` |
| strahd | `cloud_saves/RLOFT/SAVE10/LEV019.DAT` | 2996 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `2769f224f2ca0d3457c6080e8cf2ce1d53b56840` |
| strahd | `cloud_saves/RLOFT/SAVE10/LEV020.DAT` | 7548 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `b479735131d7bc38f04de5eeb16220f90a603a29` |
| strahd | `cloud_saves/RLOFT/SAVE10/LEV021.DAT` | 10740 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `cdffb77df5f34b22aa31e2214ac40f195ffddaba` |
| strahd | `cloud_saves/RLOFT/SAVE10/LEV022.DAT` | 6028 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `1055db8cbb2d142684115586860c1f5918ee9faa` |
| strahd | `cloud_saves/RLOFT/SAVE10/LEV023.DAT` | 20842 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `cf965670d3dc82adc41ca89d720e415528243cee` |
| strahd | `cloud_saves/RLOFT/SAVE10/LEV024.DAT` | 19071 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `5c5ecd02b5a19e5091550124d6b143ebacca0ae5` |
| strahd | `cloud_saves/RLOFT/SAVE10/LEV025.DAT` | 15864 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `9841155303c81c898fd5b190596f90d48205f812` |
| strahd | `cloud_saves/RLOFT/SAVE10/LEV026.DAT` | 20438 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `2248dd89e7f5c3a925347e5550c6de9e625f78a1` |
| strahd | `cloud_saves/RLOFT/SAVE10/LEV027.DAT` | 2834 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `51b349d6d0eabb2f647e2573b3cd5d13f43a7f97` |
| strahd | `cloud_saves/RLOFT/SAVE10/LEV030.DAT` | 4246 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `62138caf501e287e82bed54be487e2e1286d420c` |
| strahd | `cloud_saves/RLOFT/SAVE10/LEV031.DAT` | 14322 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `4086f1435cab91da1face650401a60e839ffff20` |
| strahd | `cloud_saves/RLOFT/SAVE10/SAVE.DAT` | 9288 | save | UNKNOWN | HASHED; NO SOURCE REFERENCE | `2b5bf001c92c6ec851e73cf9b1c43be38a7a4670` |
| strahd | `cloud_saves/RLOFT/SOUND.CFG` | 59 | config | UNKNOWN | HASHED; NO SOURCE REFERENCE | `2ac705668f0f73ba16451dab4fadd981cbdce4a1` |
| strahd | `Cluebook.pdf` | 2554235 | text | UNKNOWN | HASHED; NO SOURCE REFERENCE | `34b763484e27b80df51f2df4402b69355c4fbefd` |
| strahd | `dosbox_strahd_single.conf` | 964 | config | UNKNOWN | HASHED; NO SOURCE REFERENCE | `8fe173502befb91dd0e4dc4fd4df0654596315f2` |
| strahd | `dosbox_strahd.conf` | 11444 | config | UNKNOWN | HASHED; NO SOURCE REFERENCE | `9047894447e7e69369bdee4d3f384de2b4fc84b4` |
| strahd | `DOSBOX/Documentation/AUTHORS.txt` | 264 | text | UNKNOWN | HASHED; NO SOURCE REFERENCE | `eb365e5d6d4ad7b7b6358733e2981f1d8ea16cd7` |
| strahd | `DOSBOX/Documentation/COPYING.txt` | 17992 | text | UNKNOWN | HASHED; NO SOURCE REFERENCE | `d60c31a97a544b53039088d14fe9114583c0efc3` |
| strahd | `DOSBOX/Documentation/INSTALL.txt` | 3833 | text | UNKNOWN | HASHED; NO SOURCE REFERENCE | `cc48f8e25894151a678f24d6307aa29999a65f47` |
| strahd | `DOSBOX/Documentation/NEWS.txt` | 31918 | text | UNKNOWN | HASHED; NO SOURCE REFERENCE | `bf6b7a131434123d08525bcc22eb22865fa38eaa` |
| strahd | `DOSBOX/Documentation/README.txt` | 62740 | text | UNKNOWN | HASHED; NO SOURCE REFERENCE | `8bf8f63e57fdf1dfa3dee4355f467245b6d8d92d` |
| strahd | `DOSBOX/Documentation/THANKS.txt` | 972 | text | UNKNOWN | HASHED; NO SOURCE REFERENCE | `8b0e5d96fb83ddb0b8a9095bf8835b639e7a465c` |
| strahd | `DOSBOX/DOSBox 0.74-2 Manual.txt` | 62740 | text | UNKNOWN | HASHED; NO SOURCE REFERENCE | `8bf8f63e57fdf1dfa3dee4355f467245b6d8d92d` |
| strahd | `DOSBOX/dosbox-0.74-2.1.tar.gz` | 1334686 | UNKNOWN | UNKNOWN | HASHED; NO SOURCE REFERENCE | `993d5f215a68b175b016260be47ee80a63c39f5f` |
| strahd | `DOSBOX/DOSBox.exe` | 3802624 | executable | UNKNOWN | HASHED; NO SOURCE REFERENCE | `712e82a9406de38588fda627cebb2a23d39d72f1` |
| strahd | `DOSBOX/GOGDOSConfig.exe` | 2989648 | executable | UNKNOWN | HASHED; NO SOURCE REFERENCE | `6310a3f3eac941b43327f734be6ce917dd960965` |
| strahd | `DOSBOX/SDL_net.dll` | 13312 | executable | UNKNOWN | HASHED; NO SOURCE REFERENCE | `924e65403298ac3dc314dfec1ddc5d65831a27a8` |
| strahd | `DOSBOX/SDL.dll` | 448231 | executable | UNKNOWN | HASHED; NO SOURCE REFERENCE | `37d31517d044a891485220f91ee2be2151c5069e` |
| strahd | `DOSBOX/stderr.txt` | 0 | text | UNKNOWN | HASHED; NO SOURCE REFERENCE | `e69de29bb2d1d6434b8b29ae775ad8c2e48c5391` |
| strahd | `DOSBOX/stdout.txt` | 0 | text | UNKNOWN | HASHED; NO SOURCE REFERENCE | `e69de29bb2d1d6434b8b29ae775ad8c2e48c5391` |
| strahd | `DOSBOX/Video Codec/Video Instructions.txt` | 1573 | text | UNKNOWN | HASHED; NO SOURCE REFERENCE | `1d15b4f9c51e8920b3a37621784d2ac0d91e8860` |
| strahd | `DOSBOX/Video Codec/zmbv.dll` | 94208 | executable | UNKNOWN | HASHED; NO SOURCE REFERENCE | `c6c0eb63b7be968cab3d283e8413d39f115dd5c3` |
| strahd | `DOSBOX/Video Codec/zmbv.inf` | 3938 | UNKNOWN | UNKNOWN | HASHED; NO SOURCE REFERENCE | `a98ea01677054579ac9d901c41b8540c4d0de215` |
| strahd | `gog.ico` | 69248 | UNKNOWN | UNKNOWN | HASHED; NO SOURCE REFERENCE | `5a9ec11e053e7f83467b2e3fff9090cf5a207dec` |
| strahd | `goggame-1432905053.hashdb` | 1783 | UNKNOWN | UNKNOWN | HASHED; NO SOURCE REFERENCE | `13c5574fc2209fd20bb9122ab17d394c611c29a2` |
| strahd | `goggame-1432905053.ico` | 212340 | UNKNOWN | UNKNOWN | HASHED; NO SOURCE REFERENCE | `e1c7a33f63f715d4e085753ceb5b60dc3734e6d9` |
| strahd | `goggame-1432905053.info` | 1733 | UNKNOWN | UNKNOWN | HASHED; NO SOURCE REFERENCE | `32c21f9a062250690732e1267316435614dec1a3` |
| strahd | `goggame-1432905053.script` | 1839 | UNKNOWN | UNKNOWN | HASHED; NO SOURCE REFERENCE | `3fabfdd21bbbc0f1f9ac0f079cd99b6899d9643e` |
| strahd | `goggame-galaxyFileList.ini` | 2493 | config | UNKNOWN | HASHED; NO SOURCE REFERENCE | `3dcab5810ad2d9a2e1cdaefeb5488d0589b58493` |
| strahd | `goglog.ini` | 1012 | config | UNKNOWN | HASHED; NO SOURCE REFERENCE | `2acb9986d51350c0d501ebc6dc7f517020596179` |
| strahd | `Launch Ravenloft - Strahd's Possession.lnk` | 1304 | UNKNOWN | UNKNOWN | HASHED; NO SOURCE REFERENCE | `cbfaa952849a88f2ce3875f511d5b11300fa86e3` |
| strahd | `Manual.pdf` | 4044415 | text | UNKNOWN | HASHED; NO SOURCE REFERENCE | `eb0f8ee67daaff634419fb8bdb829b70bb98bd71` |
| strahd | `Readme.TXT` | 2693 | text | UNKNOWN | HASHED; NO SOURCE REFERENCE | `6b487141e9b14f03d34795fb4a29526087240cf4` |
| strahd | `RLOFT/A32ADLIB.DLL` | 19968 | executable | UNKNOWN | HASHED; NO SOURCE REFERENCE | `8e41fec4502ad9107afe298c9a063a5f79a6cc5e` |
| strahd | `RLOFT/A32ALGDG.DLL` | 10752 | executable | UNKNOWN | HASHED; NO SOURCE REFERENCE | `bce10f8e48e4468aa18065ddeb9081b39d7fc327` |
| strahd | `RLOFT/A32ALGFM.DLL` | 24576 | executable | UNKNOWN | HASHED; NO SOURCE REFERENCE | `6aaa2c5ac6561ac2f22002093d5b81cbd6f4d19d` |
| strahd | `RLOFT/A32ARDG.DLL` | 16384 | executable | UNKNOWN | HASHED; NO SOURCE REFERENCE | `812c1604898f49d4b8301ff2f1ff07fd397d2c27` |
| strahd | `RLOFT/A32ARXM.DLL` | 53760 | executable | UNKNOWN | HASHED; NO SOURCE REFERENCE | `a799ea0b185072b231a910bfe730d08bb6660bf9` |
| strahd | `RLOFT/A32MT32.DLL` | 14336 | executable | UNKNOWN | HASHED; NO SOURCE REFERENCE | `15c56e38da8d2ae724540e1f7d89cf1a19a47b3e` |
| strahd | `RLOFT/A32PASDG.DLL` | 10752 | executable | UNKNOWN | HASHED; NO SOURCE REFERENCE | `f7bd1aa4154964029cc4ef536de65b477fc0f559` |
| strahd | `RLOFT/A32PASFM.DLL` | 19968 | executable | UNKNOWN | HASHED; NO SOURCE REFERENCE | `8923737f84fe42ee0519a5aa86a334942b172a84` |
| strahd | `RLOFT/A32PASOP.DLL` | 24576 | executable | UNKNOWN | HASHED; NO SOURCE REFERENCE | `dfc67837198693a878eeec08037e212f6e810c11` |
| strahd | `RLOFT/A32SBDG.DLL` | 10752 | executable | UNKNOWN | HASHED; NO SOURCE REFERENCE | `d19e4886dae560233c3ef93791350ec4899d3f73` |
| strahd | `RLOFT/A32SBFM.DLL` | 19968 | executable | UNKNOWN | HASHED; NO SOURCE REFERENCE | `4d40c5e034721b6ab7d208eb24a13840a5aeb103` |
| strahd | `RLOFT/A32SBPDG.DLL` | 10752 | executable | UNKNOWN | HASHED; NO SOURCE REFERENCE | `3453cb1b9751668d607aec44dc02fce412e9cf5c` |
| strahd | `RLOFT/A32SP1FM.DLL` | 19968 | executable | UNKNOWN | HASHED; NO SOURCE REFERENCE | `245be4ba908962aacbd7669b025ae744d989b492` |
| strahd | `RLOFT/A32SP2FM.DLL` | 24576 | executable | UNKNOWN | HASHED; NO SOURCE REFERENCE | `4c4d28af777ccba3e63b40c5532e08a59e367682` |
| strahd | `RLOFT/A32SPKR.DLL` | 14848 | executable | UNKNOWN | HASHED; NO SOURCE REFERENCE | `31a7c22f8a88562ef19273e01998102c1f739845` |
| strahd | `RLOFT/A32TANDY.DLL` | 14848 | executable | UNKNOWN | HASHED; NO SOURCE REFERENCE | `038971cb960f912df4f9e50f09e870d83923bae8` |
| strahd | `RLOFT/CHARS` | 2336 | save | UNKNOWN | HASHED; NO SOURCE REFERENCE | `44299671acfb26a9304e23bb7a28106a507f2ef6` |
| strahd | `RLOFT/CODE.1` | 277787 | executable | UNKNOWN | HASHED; NO SOURCE REFERENCE | `430407e93e55ee7873f6e6ece68ced309389334e` |
| strahd | `RLOFT/CODE.2` | 558689 | executable | UNKNOWN | HASHED; NO SOURCE REFERENCE | `70d8b2e27a38c481153b6937493575797054bb58` |
| strahd | `RLOFT/COLORS` | 11521 | resource archive | UNKNOWN | HASHED; NO SOURCE REFERENCE | `452733d3d755e26a6a62baefbb100bdaa4ba37da` |
| strahd | `RLOFT/dosdis-candidate-analysis.json` | 11148814 | UNKNOWN | UNKNOWN | HASHED; NO SOURCE REFERENCE | `6ebe0d8a69602101b81d9b77a036c1a0c84243eb` |
| strahd | `RLOFT/GM1.BNK` | 23907 | audio | UNKNOWN | HASHED; NO SOURCE REFERENCE | `0b1037e22a21be99811b605081e254c2a9681f43` |
| strahd | `RLOFT/GM2.BNK` | 24273 | audio | UNKNOWN | HASHED; NO SOURCE REFERENCE | `aa4eb15b776535805fceddc11d81514c96974272` |
| strahd | `RLOFT/GUSDIG32.DLL` | 28959 | executable | UNKNOWN | HASHED; NO SOURCE REFERENCE | `2c2b2799675e53402062325878cbbac5ab0a1c9d` |
| strahd | `RLOFT/GUSMID32.DLL` | 59865 | executable | UNKNOWN | HASHED; NO SOURCE REFERENCE | `ee0eb08179f80e99dda7e6fc4a10b3d0e1dab010` |
| strahd | `RLOFT/INFO.DAT` | 23 | config | UNKNOWN | HASHED; NO SOURCE REFERENCE | `717f46572a804c64af40420496c75480d9ca8918` |
| strahd | `RLOFT/INIT` | 1879 | config | UNKNOWN | HASHED; NO SOURCE REFERENCE | `aeb2e1575879047d64d6cc961c5ea59043b555a4` |
| strahd | `RLOFT/LEVELS.DAT` | 499878 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `a26401dfa462f324afbc02ec3b0151629ce6479c` |
| strahd | `RLOFT/LOADPATS.EXE` | 24705 | executable | UNKNOWN | HASHED; NO SOURCE REFERENCE | `55537c435d49520cb0b11370fb7f0a3230ce51d4` |
| strahd | `RLOFT/NPCS.DAT` | 5731 | resource archive | UNKNOWN | HASHED; NO SOURCE REFERENCE | `f8d0e85a0f3146c227c50398c07586c6254f780f` |
| strahd | `RLOFT/OBJECTS.DAT` | 21844 | resource archive | UNKNOWN | HASHED; NO SOURCE REFERENCE | `e52b52aa716ce2024404be65e752e7698e32248f` |
| strahd | `RLOFT/PATCHES.INI` | 7157 | config | UNKNOWN | HASHED; NO SOURCE REFERENCE | `e13972d2609f0d60ca0e6479287bbe0493f1be83` |
| strahd | `RLOFT/R.EXE` | 12368 | executable | UNKNOWN | HASHED; NO SOURCE REFERENCE | `16eca25db273fc4f8eb0b4a76123a53cc5d92257` |
| strahd | `RLOFT/README.BAT` | 35 | executable | UNKNOWN | HASHED; NO SOURCE REFERENCE | `c2bf4b86fc4d357992293c288471ee8504a06c9f` |
| strahd | `RLOFT/README.TXT` | 6466 | text | UNKNOWN | HASHED; NO SOURCE REFERENCE | `270e774867d654fa8e2fa4e8376428f008dae968` |
| strahd | `RLOFT/RES0` | 2186991 | resource archive | UNKNOWN | HASHED; NO SOURCE REFERENCE | `23e279f49dab8a2bd7824715f0f417fc5fe2afb7` |
| strahd | `RLOFT/RES1` | 354310 | resource archive | UNKNOWN | HASHED; NO SOURCE REFERENCE | `1ae6ed5af76f8efb3e0e26ff05d5582c7ce1dab4` |
| strahd | `RLOFT/RES2` | 63736 | resource archive | UNKNOWN | HASHED; NO SOURCE REFERENCE | `f2834dcb8f282d949e07f38ba3a642a2986cc52f` |
| strahd | `RLOFT/RES3` | 5228638 | resource archive | UNKNOWN | HASHED; NO SOURCE REFERENCE | `8bcae74169185139b08138a16814558879f6370d` |
| strahd | `RLOFT/RES4` | 1975542 | resource archive | UNKNOWN | HASHED; NO SOURCE REFERENCE | `30d1b3377ddddd0e9ae44f6dc2fa9ca1d7f0fad0` |
| strahd | `RLOFT/RES5` | 1510960 | resource archive | UNKNOWN | HASHED; NO SOURCE REFERENCE | `3b3c48c971a58b3cadd072d3fa83291e127753be` |
| strahd | `RLOFT/RES6` | 6210899 | resource archive | UNKNOWN | HASHED; NO SOURCE REFERENCE | `f6aaaaa85f8273dc2cf534237d934b0fea2f5119` |
| strahd | `RLOFT/RES7` | 211133 | resource archive | UNKNOWN | HASHED; NO SOURCE REFERENCE | `91bdf1aba59a4022e5b8e81c9570cb5215c7fcd6` |
| strahd | `RLOFT/RES8` | 58772 | resource archive | UNKNOWN | HASHED; NO SOURCE REFERENCE | `43a247cea108803a38741ab1aa48587972aba460` |
| strahd | `RLOFT/RLOFT.BAT` | 2712 | executable | UNKNOWN | HASHED; NO SOURCE REFERENCE | `facdb7068f84deb5ba60a7db98482735f87478e2` |
| strahd | `RLOFT/SAMPLE.AD` | 4662 | audio | UNKNOWN | HASHED; NO SOURCE REFERENCE | `bfccc365b141d38f2f31bd528c918dea396a6bc0` |
| strahd | `RLOFT/SAVE01/LEV022.DAT` | 6028 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `2ac76db12beecb9479a21aa663534ee8fb9f5981` |
| strahd | `RLOFT/SAVE01/SAVE.DAT` | 9288 | save | UNKNOWN | HASHED; NO SOURCE REFERENCE | `e2783e4316bc94281491277c90527cc2c5454ab7` |
| strahd | `RLOFT/SINSTALL.EXE` | 381660 | executable | UNKNOWN | HASHED; NO SOURCE REFERENCE | `e3236fdb760020bd6a171a046907f57b951b1e31` |
| strahd | `RLOFT/SINTAB` | 333757 | resource archive | UNKNOWN | HASHED; NO SOURCE REFERENCE | `91ce7412034e716383d74bd11768fe26487b2f18` |
| strahd | `RLOFT/SOUND.BAT` | 2738 | executable | UNKNOWN | HASHED; NO SOURCE REFERENCE | `2af25a77de508ff962901d21eaf518a29e16855e` |
| strahd | `RLOFT/SOUND.CFG` | 59 | config | UNKNOWN | HASHED; NO SOURCE REFERENCE | `e27a0b38ec70f534e773e4d905d6cc67d3bd0c43` |
| strahd | `RLOFT/T` | 133584 | resource archive | UNKNOWN | HASHED; NO SOURCE REFERENCE | `68cde745e55de1def1b6ce828f1455d38c6f483b` |
| strahd | `support.ico` | 62895 | UNKNOWN | UNKNOWN | HASHED; NO SOURCE REFERENCE | `e1d26f4fa2e1df83ba4be87e8a356148acd5d0d6` |
| strahd | `unins000.dat` | 1327747 | resource archive | UNKNOWN | HASHED; NO SOURCE REFERENCE | `ce50349e20c4206ca5cadb767761a77996d4b05e` |
| strahd | `unins000.exe` | 1343048 | executable | UNKNOWN | HASHED; NO SOURCE REFERENCE | `43dfb90dda07d2fa0c3500ff1ee5af10ab6504c3` |
| strahd | `unins000.ini` | 41 | config | UNKNOWN | HASHED; NO SOURCE REFERENCE | `ff7c3cf1e965ac5f4a0a45c597867e1d0456a0c4` |
| strahd | `unins000.msg` | 23147 | UNKNOWN | UNKNOWN | HASHED; NO SOURCE REFERENCE | `aa742051830bda5f367d343b3ed5bff183f60d7d` |
| menzo | `A32ADLIB.DLL` | 19968 | executable | `520db726d034394e7ea7f62b9bfa90767898990836522fc09f89931f2c388a7e` | SHA-256 PASS (CI #1) | `8e41fec4502ad9107afe298c9a063a5f79a6cc5e` |
| menzo | `A32ALGDG.DLL` | 10752 | executable | `4e735e261934ff30bf2899d6ebe36d0d0a1452da455710142848071967306272` | SHA-256 PASS (CI #1) | `bce10f8e48e4468aa18065ddeb9081b39d7fc327` |
| menzo | `A32ALGFM.DLL` | 24576 | executable | `b45484c60bbd353a99f2d4b38c0d187af66941c3b3f771df331790bb52818f32` | SHA-256 PASS (CI #1) | `6aaa2c5ac6561ac2f22002093d5b81cbd6f4d19d` |
| menzo | `A32ARDG.DLL` | 16384 | executable | `6b0dcd7ea52e891d23f9f8b3af56e3774092d3e5a6a0dbcc1d4046ba39ae111b` | SHA-256 PASS (CI #1) | `812c1604898f49d4b8301ff2f1ff07fd397d2c27` |
| menzo | `A32ARXM.DLL` | 53760 | executable | `5fa2815cb37d35e59c68801d1382c2bf6c8549d2b1cf25658c701434e1f4c25f` | SHA-256 PASS (CI #1) | `a799ea0b185072b231a910bfe730d08bb6660bf9` |
| menzo | `A32GFX.DLL` | 45056 | executable | `986bd320b4dbf34968a6363311c0b9ada63d922f35f68c226f4de655c409992a` | SHA-256 PASS (CI #1) | `a337f66b6111d3d2bf8c71687e93b212a2fecc71` |
| menzo | `A32MT32.DLL` | 14336 | executable | `c783b293f1664c248505c6bb0591acb9874cbdfaa55e9086a6abdc313820daf0` | SHA-256 PASS (CI #1) | `15c56e38da8d2ae724540e1f7d89cf1a19a47b3e` |
| menzo | `A32PASDG.DLL` | 10752 | executable | `ca37ed704f4004c59ab546b959c689243b9ee8c600d595a9ef88f589a8775a89` | SHA-256 PASS (CI #1) | `f7bd1aa4154964029cc4ef536de65b477fc0f559` |
| menzo | `A32PASFM.DLL` | 19968 | executable | `0309fecd637c3ae7dfb4c5096cad2c354c2acf267c0dda22fcfcdfc2a78d6b1d` | SHA-256 PASS (CI #1) | `8923737f84fe42ee0519a5aa86a334942b172a84` |
| menzo | `A32PASOP.DLL` | 24576 | executable | `2fad5776664ab0e4027dfdcfd233b3bd35f946631904ae77b8400da802f455f4` | SHA-256 PASS (CI #1) | `dfc67837198693a878eeec08037e212f6e810c11` |
| menzo | `A32SBDG.DLL` | 10752 | executable | `a6fa522cdfa9a65c0a85f142c1377b87023d41739a38d9792339dae959133588` | SHA-256 PASS (CI #1) | `d19e4886dae560233c3ef93791350ec4899d3f73` |
| menzo | `A32SBFM.DLL` | 19968 | executable | `2919f0080c6d374094c5520d8394aa998ede2d5a2d07e9e32017e1f985d70622` | SHA-256 PASS (CI #1) | `4d40c5e034721b6ab7d208eb24a13840a5aeb103` |
| menzo | `A32SBPDG.DLL` | 10752 | executable | `d1f0de508c92c2b8fc73be5f036c77c60dbb61a66651a26112eed1a39a396093` | SHA-256 PASS (CI #1) | `3453cb1b9751668d607aec44dc02fce412e9cf5c` |
| menzo | `A32SP1FM.DLL` | 19968 | executable | `5a309bd1aaaa024b1553086ab610d671e8b53155f89f3b43c4eaf96b0cadbae5` | SHA-256 PASS (CI #1) | `245be4ba908962aacbd7669b025ae744d989b492` |
| menzo | `A32SP2FM.DLL` | 24576 | executable | `2aaf3579c7d7a4012f23aea08da35d6a726dadee533a29b545d8ce0ce375e4df` | SHA-256 PASS (CI #1) | `4c4d28af777ccba3e63b40c5532e08a59e367682` |
| menzo | `A32SPKR.DLL` | 14848 | executable | `da2b8348f435070e59b1d91eac806eb0bfb68690d96a02799d2a7828b8ca96ff` | SHA-256 PASS (CI #1) | `31a7c22f8a88562ef19273e01998102c1f739845` |
| menzo | `A32TANDY.DLL` | 14848 | executable | `e6fc7c5dff1cffd1032a6a6e93f4d052361c3db027de047eb07dcc5a23d0b4d2` | SHA-256 PASS (CI #1) | `038971cb960f912df4f9e50f09e870d83923bae8` |
| menzo | `Book One.pdf` | 18756603 | text | `de8e07efc39390ee6c99abc378dcc21e6d6499e217244a61a8940e6303e16ead` | SHA-256 PASS (CI #1) | `82d370aa7ff15c0fc9baecd47e7dd147a9d33635` |
| menzo | `CD_Extracted/BOOT.NFO` | 193 | text | UNKNOWN | HASHED; NO SOURCE REFERENCE | `06e827bac833bdfb132e1d1d9c9923d9b2a32387` |
| menzo | `CD_Extracted/C` | 2670 | UNKNOWN | UNKNOWN | HASHED; NO SOURCE REFERENCE | `559073e5aa89d960c92f8b8e3738712ff68a044e` |
| menzo | `CD_Extracted/CGEN/CGEN00.DFA` | 3696646 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `19a0d0055d5b32f30c5ac932c3dd55d234391fd4` |
| menzo | `CD_Extracted/CGEN/CGEN01.DFA` | 1095214 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `dc000bcda1d75db663a979ae44a86d211c5aaa59` |
| menzo | `CD_Extracted/CGEN/CGEN02.DFA` | 6130218 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `ef6eeebbf27d26ccaf170f158586ad8314e2570b` |
| menzo | `CD_Extracted/CGEN/CGEN03.DFA` | 1137494 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `4c714bb19446196db139bac1d392934ba465f988` |
| menzo | `CD_Extracted/CGEN/CGEN04.DFA` | 9871266 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `653c69cb666789294a4c2a0f7646c0e5264b1236` |
| menzo | `CD_Extracted/CGEN/CGEN05.DFA` | 89174 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `66f2cdcbd209e998fed14893f6147c5610bcc644` |
| menzo | `CD_Extracted/CGEN/CGEN06.DFA` | 4103352 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `7e037e3875c178006b2a1854fa29ae2e1f00888e` |
| menzo | `CD_Extracted/CGEN/CGEN07.DFA` | 1245110 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `d3ee3fd015d1a748ce63981ff845db19ba5a8492` |
| menzo | `CD_Extracted/CGEN/CGEN08.DFA` | 1222794 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `24bf3ff9993fb7151e1af10948ed05d4968a3275` |
| menzo | `CD_Extracted/CGEN/CGEN09.DFA` | 1210812 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `6c750c69d317ab61c0771d087818154b75dd9718` |
| menzo | `CD_Extracted/CGEN/CGEN10.DFA` | 1241204 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `9fa7715f8ea113bb8e3101104e4c64f439283ff4` |
| menzo | `CD_Extracted/CGEN/CGEN11.DFA` | 1130360 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `69ddbb613008be53a42202cf521b62f5c8e08a16` |
| menzo | `CD_Extracted/CGEN/CGEN12.DFA` | 1221092 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `499f54b324995434970412777bd82d7afde20062` |
| menzo | `CD_Extracted/CGEN/CGEN13.DFA` | 811530 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `dde4cf6a866623d95f7a4cfc9b49c76da1c97d8b` |
| menzo | `CD_Extracted/CGEN/CGEN14.DFA` | 879616 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `d6019ae97e1aa1f93cb48e22298fbf2aa802a899` |
| menzo | `CD_Extracted/CGEN/CGEN15.DFA` | 810772 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `1321035a6356347a5527ab65600c5c7167fcb9a0` |
| menzo | `CD_Extracted/CGEN/CGEN16.DFA` | 878338 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `6af4711107056405f1825287b1c30d41e2ec01c9` |
| menzo | `CD_Extracted/CGEN/CGEN17.DFA` | 817120 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `074644d373aca60031d8a115ba2e347a135439a8` |
| menzo | `CD_Extracted/CGEN/CGEN18.DFA` | 811528 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `80ed4ca5f17003de90bfce69c84ecba848082ee8` |
| menzo | `CD_Extracted/CGEN/CGEN19.DFA` | 4279560 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `474508ad35871e2033f865f2e6f9337d1a2551ef` |
| menzo | `CD_Extracted/CGEN/CGEN20.DFA` | 310184 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `0b0d75a41ea39041a400d773e32f89b8dffe4698` |
| menzo | `CD_Extracted/CGEN/CGEN21.DFA` | 57066 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `1cbdf002a72a768e3f15e0f5d7be7d9ca3369d97` |
| menzo | `CD_Extracted/CGEN/CGEN22.DFA` | 560326 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `fdea96343cdf3024c6ee65ae9e889407639f7c5e` |
| menzo | `CD_Extracted/CGEN/CGEN23.DFA` | 59348 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `bee773e322cc21a6e76b76673cae7348e0172dde` |
| menzo | `CD_Extracted/CGEN/CGEN24.DFA` | 628262 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `e199ae402a0f61a04e50b89662cc761e4ac8d044` |
| menzo | `CD_Extracted/CGEN/CGEN25.DFA` | 58758 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `b2651309dfbcf93bfe038c678cf2e4ac885c144c` |
| menzo | `CD_Extracted/CGEN/CGEN26.DFA` | 204012 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `61e2b24102ab662a525ab68c6e1009b2cff675cc` |
| menzo | `CD_Extracted/CGEN/CGEN27.DFA` | 300820 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `906eb12cfc9159b1bee79e65c151b7354f3032f0` |
| menzo | `CD_Extracted/CGEN/CGEN28.DFA` | 58106 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `24e32d52d532a1a755ca9898d4564aaa4f88a2f8` |
| menzo | `CD_Extracted/CGEN/CGEN29.DFA` | 560108 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `2382589ca9c7cd06de6eb69163d6ee833f066e22` |
| menzo | `CD_Extracted/CGEN/CGEN30.DFA` | 58152 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `4d18fa3e45b2c9c3ba5d78fd49b1c8ea38329329` |
| menzo | `CD_Extracted/CGEN/CGEN31.DFA` | 554512 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `e61233bc987019cea67c60056bcfe0985a381334` |
| menzo | `CD_Extracted/CGEN/CGEN32.DFA` | 57736 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `ffdbc278025b26fabb201d9025f679b4b673d0f1` |
| menzo | `CD_Extracted/CGEN/CGEN33.DFA` | 250868 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `4bc599e017d28bafdd23d370613ab60e0ab7355c` |
| menzo | `CD_Extracted/CGEN/CGEN34.DFA` | 1774318 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `cbe97412dad0fa6310b1e9434f6e3dab52ce7813` |
| menzo | `CD_Extracted/CGEN/CGEN35.DFA` | 1514534 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `a42c3311c803f069e45ec591dc072454f1502c09` |
| menzo | `CD_Extracted/CGEN/CGEN36.DFA` | 43750 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `d7eafba2e2562317c7d52655bc04804f9435fc90` |
| menzo | `CD_Extracted/CGEN/CGEN37.DFA` | 976522 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `42919622abe4f7708ccf03451e97e48dc29bda06` |
| menzo | `CD_Extracted/CGEN/CGEN38.DFA` | 74502 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `dc9bef2c8b8b232751d3b5e5414933ad0555ed6b` |
| menzo | `CD_Extracted/CGEN/CGEN39.DFA` | 312042 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `f17205681fb79383e8196506ceab604fbc3549cf` |
| menzo | `CD_Extracted/CGEN/CGEN40.DFA` | 54284 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `97d15bfa11ce1c5f94700ab7630cf90bb2ce77f9` |
| menzo | `CD_Extracted/CGEN/CGEN41.DFA` | 4442262 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `5d8087466cbfb81587cb488c2b7854d4d758be09` |
| menzo | `CD_Extracted/CGEN/CGEN50.DFA` | 3742754 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `cb41db453af6e8ce7584d532e20c4022e3c23326` |
| menzo | `CD_Extracted/CGEN/CGEN51.DFA` | 1913482 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `02988ff6020719b6a08c47a6002472890ab35e1e` |
| menzo | `CD_Extracted/CGEN/CGEN52.DFA` | 2182614 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `28dc0185f68411f742902bbd35560d0a56d88bf4` |
| menzo | `CD_Extracted/CGEN/CGEN53.DFA` | 1126810 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `39bb919c3bb554082a21330a095d031c913adc9e` |
| menzo | `CD_Extracted/CINE/CINEM10.DFA` | 4683702 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `ad666189165515a38d584dfad63e7e982de1cb5e` |
| menzo | `CD_Extracted/CINE/CINEM13.DFA` | 19839372 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `655d72b285da4ce1e182184bf1fb30836458c8b5` |
| menzo | `CD_Extracted/CINE/CINEM3.DFA` | 16067326 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `0240b93f49895bdeb747dc9ed2d2af963a81b4f6` |
| menzo | `CD_Extracted/CINE/CINEM4.DFA` | 11925846 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `321ed8e0c75292d6d94e7bdca23ba4eba84be2ee` |
| menzo | `CD_Extracted/CINE/CINEM5.DFA` | 14520126 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `ebc717b2c4ca71f8e5d99784025f32265df46bd9` |
| menzo | `CD_Extracted/CINE/CINEM6.DFA` | 9303934 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `589966526fc705224fccb88dbf4ecb04abbae80f` |
| menzo | `CD_Extracted/CINE/CINEM8.DFA` | 8306940 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `a66a56f1017a0a736a127d8df7d7f488b855c50a` |
| menzo | `CD_Extracted/CINE/CINEM9.DFA` | 19954678 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `d18fabedbe758d73b451f00453459496473dfaba` |
| menzo | `CD_Extracted/CINE/DEATH.DFA` | 3205906 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `9ecd19f4e6dd621d5fb837239b69aafc9dab1743` |
| menzo | `CD_Extracted/CINE/END-A.DFA` | 11061080 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `b3302485306797e6b78f074a5a122344b43a0ab2` |
| menzo | `CD_Extracted/CINE/END-B.DFA` | 10701008 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `d5e87f5d23042fa949ebe76b86f54822df52f015` |
| menzo | `CD_Extracted/CINE/LOGOS.DFA` | 2834310 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `4849942f92ac70190004239e277414b94cd8ba41` |
| menzo | `CD_Extracted/CINE/OPEN-A.DFA` | 20557676 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `98161eee494320501e0c6338642b2455ef92e8a9` |
| menzo | `CD_Extracted/CINE/OPEN-B.DFA` | 5349852 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `e800fe98d48fb7c6194a9858bfc4fc84ddd6640d` |
| menzo | `CD_Extracted/CINE/OPEN-C.DFA` | 11490658 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `d2a12bab342482cab648e2625fc4ce5a815d787c` |
| menzo | `CD_Extracted/DATA.A00` | 9133899 | UNKNOWN | UNKNOWN | HASHED; NO SOURCE REFERENCE | `17532a7cfc83433c07594108d404805bff85562c` |
| menzo | `CD_Extracted/DATA.A01` | 19743424 | UNKNOWN | UNKNOWN | HASHED; NO SOURCE REFERENCE | `2378164f7e6ad4cecbccf6778113c0255b5642a2` |
| menzo | `CD_Extracted/DEARJ.EXE` | 39441 | executable | UNKNOWN | HASHED; NO SOURCE REFERENCE | `6e4ef2c366f132bac5682fe780899107b43725a2` |
| menzo | `CD_Extracted/INSTALL.EXE` | 49934 | executable | UNKNOWN | HASHED; NO SOURCE REFERENCE | `089b8310c7ba6b4cb8c44b05b8f3ba51efb7c20c` |
| menzo | `CD_Extracted/INSTALL.NFO` | 420 | text | UNKNOWN | HASHED; NO SOURCE REFERENCE | `4907121b0e2109ad305610cacf4720bea5495579` |
| menzo | `CD_Extracted/RES10` | 55264696 | resource archive | UNKNOWN | HASHED; NO SOURCE REFERENCE | `f06c7784266492aac8cc7c6b1b0596cce8268e58` |
| menzo | `CD_Extracted/RES3` | 5337760 | resource archive | UNKNOWN | HASHED; NO SOURCE REFERENCE | `f5129e44bf32262b6d1a1602e11410ca2a43c7f2` |
| menzo | `CD_Extracted/RES6` | 14405518 | resource archive | UNKNOWN | HASHED; NO SOURCE REFERENCE | `998cd350764da95cf74d09acbac2e2ada1630d04` |
| menzo | `CD_Extracted/RES9` | 2119297 | resource archive | UNKNOWN | HASHED; NO SOURCE REFERENCE | `118a2e8239707cad6974bbbc9b1d458a90188d68` |
| menzo | `Cluebook.pdf` | 13208947 | text | `fccf40ea50cee2d6ee166781b3da4e60c65dc1d3304315bb23a67d7fc28ea2c8` | SHA-256 PASS (CI #1) | `a34a2cd35ea062b390887f56ce509b9d2f861da0` |
| menzo | `CODE.1` | 294203 | executable | `905590c145176f93cef1651eb6b34a2253758aea94e1a621205402f0b7e2bda9` | SHA-256 PASS (CI #1) | `ecf3c2164975b28b8266d9842fdaf09bc87c9534` |
| menzo | `CODE.2` | 650385 | executable | `4a4fa725dddea00cb62ff44f44c48999cf6ed4c899b16c21826e8141ec7fda96` | SHA-256 PASS (CI #1) | `9263a19aad4ab04747645b7118d95fcab3333830` |
| menzo | `COLORS` | 31489 | resource archive | `8f1f094b0dda0510a66c582dfb9f814fdfd66cbd5180d85163ae4c51a9908256` | SHA-256 PASS (CI #1) | `bd76a64d2095154c264f6e7daeccf551ffaeb627` |
| menzo | `dosbox_menzo_single.conf` | 948 | config | `7acbba0f34386a30b3057631470b1cfbc40c685f66abc2451f5c28f3beeceb76` | SHA-256 MISMATCH (CI #1) | `6204dddce6057bd77c32ce2b60e87a151b93d9a9` |
| menzo | `dosbox_menzo.conf` | 11206 | config | `24b696263d74fa160703c6d2b9d9cfc66d42c75e139e2d57ded664189de20551` | SHA-256 MISMATCH (CI #1) | `6607662888a060b4e90839091de66b0737c0596b` |
| menzo | `DOSBOX/Documentation/AUTHORS.txt` | 264 | text | `f6481008f772b0783585a8b583dd5dfa3ee986830aa71e19eed6300d16dc0203` | SHA-256 PASS (CI #1) | `eb365e5d6d4ad7b7b6358733e2981f1d8ea16cd7` |
| menzo | `DOSBOX/Documentation/COPYING.txt` | 17992 | text | `32b1062f7da84967e7019d01ab805935caa7ab7321a7ced0e30ebe75e5df1670` | SHA-256 PASS (CI #1) | `d60c31a97a544b53039088d14fe9114583c0efc3` |
| menzo | `DOSBOX/Documentation/INSTALL.txt` | 3833 | text | `c6679b562301614212c4bc0fd81798ed19324be8ccfd2e807856a41b39f7adff` | SHA-256 PASS (CI #1) | `cc48f8e25894151a678f24d6307aa29999a65f47` |
| menzo | `DOSBOX/Documentation/NEWS.txt` | 31918 | text | `1e648756aa6c0d0010c26d3c884b4698f51003bdfea0329dac1d02706b501924` | SHA-256 PASS (CI #1) | `bf6b7a131434123d08525bcc22eb22865fa38eaa` |
| menzo | `DOSBOX/Documentation/README.txt` | 62740 | text | `4a7b09b4a9b114221db38ae8258f30e430a37b6ac96ecb242d9350382beaf6a5` | SHA-256 PASS (CI #1) | `8bf8f63e57fdf1dfa3dee4355f467245b6d8d92d` |
| menzo | `DOSBOX/Documentation/THANKS.txt` | 972 | text | `f356d80820412b8c7f956562dad5103e3704506bd15a831078e1a2feee361526` | SHA-256 PASS (CI #1) | `8b0e5d96fb83ddb0b8a9095bf8835b639e7a465c` |
| menzo | `DOSBOX/DOSBox 0.74-2 Manual.txt` | 62740 | text | `4a7b09b4a9b114221db38ae8258f30e430a37b6ac96ecb242d9350382beaf6a5` | SHA-256 PASS (CI #1) | `8bf8f63e57fdf1dfa3dee4355f467245b6d8d92d` |
| menzo | `DOSBOX/dosbox-0.74-2.1.tar.gz` | 1334686 | UNKNOWN | `1b6c865340e9d6119529c4ae61e2f12661424a38f07f8818202c5357a6b97ba2` | SHA-256 PASS (CI #1) | `993d5f215a68b175b016260be47ee80a63c39f5f` |
| menzo | `DOSBOX/DOSBox.exe` | 3802624 | executable | `8a7a7fedd222bf985b51191000557f9ee2ffb033be02c6c59594dbbec013cd41` | SHA-256 PASS (CI #1) | `712e82a9406de38588fda627cebb2a23d39d72f1` |
| menzo | `DOSBOX/GOGDOSConfig.exe` | 2989648 | executable | `0465cedb523cbaf56c7aa8f144f1ebdddb7e1ade23575ecf0a0ba2c934455c54` | SHA-256 PASS (CI #1) | `6310a3f3eac941b43327f734be6ce917dd960965` |
| menzo | `DOSBOX/SDL_net.dll` | 13312 | executable | `2f39dc04acbecf47efa45034891602b6ea7bf6fd2f27b5c0a5ca8d7fb155c929` | SHA-256 PASS (CI #1) | `924e65403298ac3dc314dfec1ddc5d65831a27a8` |
| menzo | `DOSBOX/SDL.dll` | 448231 | executable | `69037ebc43755296c0cc292d57d560028d7f2265f7b86ca84e714835c19bbd58` | SHA-256 PASS (CI #1) | `37d31517d044a891485220f91ee2be2151c5069e` |
| menzo | `DOSBOX/Video Codec/Video Instructions.txt` | 1573 | text | `e478355e52d4be075d059441d3de1936bc43241ff17bc6270680e32f0881b5b1` | SHA-256 PASS (CI #1) | `1d15b4f9c51e8920b3a37621784d2ac0d91e8860` |
| menzo | `DOSBOX/Video Codec/zmbv.dll` | 94208 | executable | `be72d291901b4a67741732da3d6dfd87fb3e74c8d03d56d40c58dd4d9744a478` | SHA-256 PASS (CI #1) | `c6c0eb63b7be968cab3d283e8413d39f115dd5c3` |
| menzo | `DOSBOX/Video Codec/zmbv.inf` | 3938 | UNKNOWN | `f343e3b4693ae29cb5440382f3fee06572ae8fc6bce46cb673b6890bc735a026` | SHA-256 PASS (CI #1) | `a98ea01677054579ac9d901c41b8540c4d0de215` |
| menzo | `GM1.BNK` | 23907 | audio | `843f012e7980e0a58722aa5c7a6e88b652f2f8d404834638cd994ca0869df5a8` | SHA-256 PASS (CI #1) | `0b1037e22a21be99811b605081e254c2a9681f43` |
| menzo | `GM2.BNK` | 24273 | audio | `b4af656059a6afba5db2da972c3506ca08d899ca04c65389e0bec7aaf6536339` | SHA-256 PASS (CI #1) | `aa4eb15b776535805fceddc11d81514c96974272` |
| menzo | `gog.ico` | 69248 | UNKNOWN | `f6a71321521ba89713f0bd38b21f809e87e8a789cb172e8e4693f9479e30b1e4` | SHA-256 PASS (CI #1) | `5a9ec11e053e7f83467b2e3fff9090cf5a207dec` |
| menzo | `goggame-1432578339.hashdb` | 1766 | UNKNOWN | `811f9f627645f2daa4127d24b74a0187050f8cbe140d3b4df6d5adc034067bdf` | SHA-256 PASS (CI #1) | `0f73331f4ca86caa662740c978482a3e72566064` |
| menzo | `goggame-1432578339.ico` | 143367 | UNKNOWN | `fb898bd3c30aaf6a9150a649e3f953c61d30ef1d856251c7a2d79e639a75827a` | SHA-256 PASS (CI #1) | `9fc5b5ff2962837d87edb4483c4037ef15a79613` |
| menzo | `goggame-1432578339.info` | 1712 | UNKNOWN | `f67a07c58e56e616cc0915eabe358287d331501aea0e620ab54f54518cbfbe55` | SHA-256 PASS (CI #1) | `f3bb3eef838d89449ae4ce9b155d49da019060f1` |
| menzo | `goggame-1432578339.script` | 1838 | UNKNOWN | `a66bfc84dcda6fdfb00ec00156b092ca17288e0aee919067c9878a899205d918` | SHA-256 PASS (CI #1) | `5972a28bc5d6a2429d447486a2db9e49ad0a0c51` |
| menzo | `goggame-galaxyFileList.ini` | 1908 | config | `75e509b20739d1472d18bde57d9a416852020ce9d0fa006ef6f3a30e3f17e9f5` | SHA-256 MISMATCH (CI #1) | `40cd74d05897bf9d011b03e3b17ef0175429721b` |
| menzo | `goglog.ini` | 934 | config | `8d323156ffe5187434dedc25d22d3b59ecfc1d26112c0928bc8a88ad8841dd38` | SHA-256 MISMATCH (CI #1) | `2d0946413b72f7da28fdc89d54f26a8c9aa65eb1` |
| menzo | `GUSDIG32.DLL` | 28959 | executable | `9fc52cbc9bd95484a898defc1fbff9af90a443a74e073de5b80e8e11fc7eee41` | SHA-256 PASS (CI #1) | `2c2b2799675e53402062325878cbbac5ab0a1c9d` |
| menzo | `GUSMID32.DLL` | 59865 | executable | `13f3b30e4d1d218502b38bd6bd40ee984c502dfcd4d9393a1fc21bae2a461e99` | SHA-256 PASS (CI #1) | `ee0eb08179f80e99dda7e6fc4a10b3d0e1dab010` |
| menzo | `INFO.DAT` | 17 | config | `4ffa6ac5d5b5afd103bd67d84d3a372b401b3b19407b0c68c44bb0dfd9e32c30` | SHA-256 PASS (CI #1) | `0f1b979b00fda1b91d676a1b0e29fc1c5b0b8439` |
| menzo | `INIT` | 2830 | config | `15f638cc8cad49f6e6a371d8e560d9437c00a319a86433306a07205160dc1b28` | SHA-256 PASS (CI #1) | `3acae549329b534e6180803d73730e69a12e9746` |
| menzo | `Launch Menzoberranzan.lnk` | 2308 | UNKNOWN | `4d5677708bfdfc8a3b299f7a4f31f32f2363f39217bce74c305c48d6a30b2326` | SHA-256 PASS (CI #1) | `a027fbcd4f780423fa274a327d0902fcc103813e` |
| menzo | `LEVELS.DAT` | 481247 | level | `d3509c9288cc5b1e14b6a9e6c4e2d496e336b3dcaca05555a65e5ed4e564d8aa` | SHA-256 PASS (CI #1) | `210c2501722338aace7d86fea14715555c2fd89a` |
| menzo | `LOADPATS.EXE` | 24705 | executable | `17f40351cf53e445dbb62a041ff90b18dd755f6e1f3a216354e14aaeaa50f8ea` | SHA-256 PASS (CI #1) | `55537c435d49520cb0b11370fb7f0a3230ce51d4` |
| menzo | `M.EXE` | 12448 | executable | `45ac5a511b5c645d92cd674459ad033a0c4d05565f85829a64bf2d9c78a9c4a9` | SHA-256 PASS (CI #1) | `5b36ac66d593a129e8a993665610e5feb99630a9` |
| menzo | `Manual.pdf` | 5754143 | text | `1fd67b120dd276c83e755daffb3ad889126929d6ab15ab038650184c48cd0fe7` | SHA-256 PASS (CI #1) | `bfa7e89b50b6d8affce8015b45b301b43e3d6f36` |
| menzo | `NPCS.DAT` | 8628 | resource archive | `ad365e5da571e95306cf20f226399088563a7cc5d371d53db971e8c1e8449e39` | SHA-256 PASS (CI #1) | `9f2039465bf8fa04bc5cebb1cfa25cc7808eebb5` |
| menzo | `OBJECTS.DAT` | 33999 | resource archive | `cd812f233e155c1db8d94349ff27a2b9ef074a3c76ccf8f1db46edb1aa7a1578` | SHA-256 PASS (CI #1) | `ea8127b28136d6bc7ed070711b13cea727294892` |
| menzo | `PATCHES.INI` | 6952 | config | `3075d1660a15ff7ecb2f143ff2c12b2593d337b61ae06ab60a4f9cdd32e8d93b` | SHA-256 MISMATCH (CI #1) | `3d9b15de08ad25217618f7dd0046fdbb13f64c1b` |
| menzo | `README.md` | 444 | text | UNKNOWN | HASHED; NO SOURCE REFERENCE | `e802252ac5af95887be73a0cc4bd90e3392972a0` |
| menzo | `README.TXT` | 9007 | text | `c14b711d7863759ad9925257ad88e3f348fee50a4e43d14eb2f37f601fe22030` | SHA-256 MISMATCH (CI #1) | `0fc9a523e6951da38494eab562fb9fb6567f6151` |
| menzo | `RES0` | 580712 | resource archive | `7ffa8cc0544f15a27b9a935c2164e54e5600c7366882d99321956253bc7f6aba` | SHA-256 PASS (CI #1) | `0829fdf92f74f1b09ea29cb48de634c96e9c1703` |
| menzo | `RES1` | 602621 | resource archive | `46e6938d656d19e038d2edbe00d70d2ea9505f32bf572173e6a3d28249595acc` | SHA-256 PASS (CI #1) | `ccf721cca0b0727ec7e73170427baef72f81b793` |
| menzo | `RES2` | 420180 | resource archive | `348f1f06af4b3b1ec32d5545a5bc4294fb798bae3c36bc9c83898eb1d367e06b` | SHA-256 PASS (CI #1) | `05f5d6c430f03bd7e034e543369c8398a1e99802` |
| menzo | `RES3` | 5337760 | resource archive | `7b31032e76bb8c7635736b168f9eb2052bb859576c125d1b7de20a932b57a7e6` | SHA-256 PASS (CI #1) | `f5129e44bf32262b6d1a1602e11410ca2a43c7f2` |
| menzo | `RES4` | 4304691 | resource archive | `62b09c60881322c035567b58bc09c755e9f905ee0b620e5f3a3f49580ea4eafd` | SHA-256 PASS (CI #1) | `a54c06aec643e23ee349e698689d3e880905067d` |
| menzo | `RES5` | 1385584 | resource archive | `d8f49c291df98b0f352c9d802445fe549c1aaa2af2346abc0505bd578706050d` | SHA-256 PASS (CI #1) | `a2b18c0aa0c717f3fc54b1f4bbf09b64807e5d6d` |
| menzo | `RES6` | 14405518 | resource archive | `a054a95e73577d2571f96f7db220f4324fdabb03e3f61c8ca8cc09fb9ca00e36` | SHA-256 PASS (CI #1) | `998cd350764da95cf74d09acbac2e2ada1630d04` |
| menzo | `RES7` | 13167 | resource archive | `22d0ec5a3dc7ded9ba8037d1fd627fef7bf44e407ab8903e77f90d605e7d2c97` | SHA-256 PASS (CI #1) | `c6ecb8b6844ca378a9ac8fd9be5a3ef70d3dda4c` |
| menzo | `RES8` | 26284 | resource archive | `67487ffe4cc20df71deacb67ec566341491e0d1ef84407cfbf116bde1b746d84` | SHA-256 PASS (CI #1) | `bf5a772b85378bf6176713cbe4bf043bc41f5d01` |
| menzo | `SAMPLE.AD` | 4662 | audio | `79ee9f4ad3134be906ce493072ce0c3b7169c9013d3c25947ac385aaf3f9633b` | SHA-256 PASS (CI #1) | `bfccc365b141d38f2f31bd528c918dea396a6bc0` |
| menzo | `SAVE01/SAVE.DAT` | 27959 | save | `4ea34670dd1b89224704b6cab6158d8df6590fd36358908f13b911759b5dd400` | SHA-256 PASS (CI #1) | `f41adc9cb4783dd5477ab18763586f4a7919b721` |
| menzo | `SINTAB` | 8472 | resource archive | `7de88a782a1674bdf50a5e70141e0fb24ad8ecdff800b0e4ef9eb7c862d33f34` | SHA-256 PASS (CI #1) | `fc90835b95c932219c28e56dfd0bb664da82e8e7` |
| menzo | `SOUND_PM.EXE` | 481869 | executable | `a91a45b3aadf1fb5208ea594668c26baf990ac3db0448000fafe784f54c19bba` | SHA-256 PASS (CI #1) | `046289c289fc66eb6519765f0bbcc465c7bc56e0` |
| menzo | `SOUND.CFG` | 59 | config | `e892b9fd7e96ca949a149a649e016a6de088bf69fbd5e0726178386a51d0b839` | SHA-256 PASS (CI #1) | `1617af372c09321a35c26948e1019b55d481ebb4` |
| menzo | `SOUND.INI` | 29269 | config | `7509c30c3170b55c416aa7bebc9b05db66e3574fe6124adf83daad1ce1040e7a` | SHA-256 MISMATCH (CI #1) | `0e0162967bd6dee3112709e80a59dcecfe1762fb` |
| menzo | `SOURCE_INVENTORY.csv` | 9510 | text | UNKNOWN | HASHED; NO SOURCE REFERENCE | `fa896cb11b8fd954dc19544f7d0cd23c4bf796c8` |
| menzo | `support.ico` | 62895 | UNKNOWN | `05fe749ef47d1ec862d6c55be78e66d1011226bf1f78409acf57cb79cec5eb20` | SHA-256 PASS (CI #1) | `e1d26f4fa2e1df83ba4be87e8a356148acd5d0d6` |
| menzo | `T` | 122309 | resource archive | `46e773947cceefe9220f1e295c29b30563c41aea8eec6705706ea2f36e0968ae` | SHA-256 PASS (CI #1) | `8d3d57b77b356743308cbe1d096fa5a98606808c` |
| menzo | `unins000.dat` | 1327467 | resource archive | `55c68129d5a170076fc3c7d37c2d7151d9383cf26f149ef50547a3192dd34ed2` | SHA-256 PASS (CI #1) | `bb3fb7f3720687d7f0a0261028d955ad4f587ae1` |
| menzo | `unins000.exe` | 1343048 | executable | `5761e7789d813626cd68ee1e62429cfeb92bdd814cd29ef12fc4ae9ec1dbaff3` | SHA-256 PASS (CI #1) | `43dfb90dda07d2fa0c3500ff1ee5af10ab6504c3` |
| menzo | `unins000.ini` | 39 | config | `931c9c0e0bbd792b5726ef21bd95806591c6efadf748ff16e9bf5fdb5cc0e418` | SHA-256 MISMATCH (CI #1) | `1c760ab582f7cd076660441c8e33e472c5deb9ab` |
| menzo | `unins000.msg` | 23147 | UNKNOWN | `e43d06ec2b3dbe3d81bcd6b7880d28d074dac54b38646a605cfc5c809939da16` | SHA-256 PASS (CI #1) | `aa742051830bda5f367d343b3ed5bff183f60d7d` |
| stone | `CD_Extracted/BOOT.NFO` | 199 | text | UNKNOWN | HASHED; NO SOURCE REFERENCE | `bb0c50633564bc8ba4ea19c7fbe3df202843f028` |
| stone | `CD_Extracted/CINE/AWAKE.ACT` | 474 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `91bde606bd94bae63d8de053d4675223dc8c3db3` |
| stone | `CD_Extracted/CINE/AWAKE.DFA` | 8901077 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `8c62ef83144e7646169f505d27f603bd1ff2d347` |
| stone | `CD_Extracted/CINE/BATTLE-1.ACT` | 304 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `b2d8eb3f4e74eb124092b3a28b2d946b9af3767d` |
| stone | `CD_Extracted/CINE/BATTLE-1.DFA` | 4368197 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `87f4dbaff934a29620684d24432049f559e1d69b` |
| stone | `CD_Extracted/CINE/BATTLE-2.ACT` | 318 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `b6fd61623a985ce27bcbf9f1e6a20be790ad82fc` |
| stone | `CD_Extracted/CINE/BATTLE-2.DFA` | 6051378 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `2621f20f461a330f0ba53a170bf3cecd815c0d69` |
| stone | `CD_Extracted/CINE/BATTLE-3.ACT` | 150 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `04f81608f4b21847125ea746bbbf5661a277902b` |
| stone | `CD_Extracted/CINE/BATTLE-3.DFA` | 3011041 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `d2b5d50a4769bdb7694f32d6e0dfae97b0828b8c` |
| stone | `CD_Extracted/CINE/BATTLE-4.ACT` | 242 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `4039f7d0cd6ba1b126168d894df34ee422b16f31` |
| stone | `CD_Extracted/CINE/BATTLE-4.DFA` | 3644603 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `07b3811d1f31db10077026621d8be6842ae11985` |
| stone | `CD_Extracted/CINE/CARD-M1.ACT` | 148 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `e8e97d351367ae7d7d002c6fd6c674f6b6bc8ab0` |
| stone | `CD_Extracted/CINE/CARD-M1.DFA` | 1135948 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `28aeb87a63c085969a707ba84d56aa13b192d9b9` |
| stone | `CD_Extracted/CINE/CGEN.ACT` | 870 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `c8782e05825fbb66724a6809d67c092f4182d401` |
| stone | `CD_Extracted/CINE/CGEN.DFA` | 12858538 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `298a007861c570557103798e4d85e0bfda52b460` |
| stone | `CD_Extracted/CINE/DEATH.ACT` | 112 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `ab541441aa6db5011fd38ae2b62456f4d859aa40` |
| stone | `CD_Extracted/CINE/DEATH.DFA` | 2519727 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `d51686e53cf7b41181dd0e221f497f81e0273018` |
| stone | `CD_Extracted/CINE/FAIL-1.ACT` | 630 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `15d20046348d6c5006aba632340cb7cef8e7fdb6` |
| stone | `CD_Extracted/CINE/FAIL-1.DFA` | 8205914 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `0610d10b62dd2eabe4a038bf4c249f0143c4d158` |
| stone | `CD_Extracted/CINE/FAIL-2.ACT` | 580 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `23337c4b9ed47e5584c9972b6917888019decada` |
| stone | `CD_Extracted/CINE/FAIL-2.DFA` | 7061192 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `98b980531622a033f701929f9ed11c653a0e1137` |
| stone | `CD_Extracted/CINE/FAIL-3.ACT` | 582 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `fe4d1a761742192499941be868c149402f5443c0` |
| stone | `CD_Extracted/CINE/FAIL-3.DFA` | 7331344 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `918873dcc450559cfe6105ae98a4a02585afe345` |
| stone | `CD_Extracted/CINE/FAIL-4.ACT` | 630 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `968f67313ee3a28ff64e8e867a863d767817c8cb` |
| stone | `CD_Extracted/CINE/FAIL-4.DFA` | 8054732 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `4ada70e025a39bf24ee6d18f05aa0971d9add92c` |
| stone | `CD_Extracted/CINE/FALCON-D.ACT` | 722 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `e9157445c229b204294750796b4962b7e8d5ff13` |
| stone | `CD_Extracted/CINE/FALCON-D.DFA` | 14015167 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `c292c332d8431f5d83a0938a85320aa5297a720a` |
| stone | `CD_Extracted/CINE/HISTORY.ACT` | 904 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `9dd89ae6bc38b6dbfe55d8588ebc7b49d232989b` |
| stone | `CD_Extracted/CINE/HISTORY.DFA` | 15362982 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `b487e88579eac6a01ac824d820e68b5be89e2e7c` |
| stone | `CD_Extracted/CINE/LOGOS.ACT` | 1008 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `2d15beef20a1915d9d3abf5e1a8c48c07fb6d09c` |
| stone | `CD_Extracted/CINE/LOGOS.DFA` | 2224585 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `333944a7dc11723efe5d9e740026928c62595b29` |
| stone | `CD_Extracted/CINE/OPEN.ACT` | 3324 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `dac463afc92c8d0517e40a773856e5667fdcd364` |
| stone | `CD_Extracted/CINE/OPEN.DFA` | 25419233 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `68702d82f8a6944d8664de3c4cd5eade4c5ff0bb` |
| stone | `CD_Extracted/CINE/RAYS.ACT` | 228 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `c44b93cf53f20e9171254a35473f067668452991` |
| stone | `CD_Extracted/CINE/RAYS.DFA` | 4577160 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `81d3e8d04703cc6fa84b43d50adecaeecc605b76` |
| stone | `CD_Extracted/CINE/SHIP.ACT` | 1008 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `2d15beef20a1915d9d3abf5e1a8c48c07fb6d09c` |
| stone | `CD_Extracted/CINE/SHIP1.ACT` | 2248 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `ea159d53ddcfbcd25d393111b4e1248553606048` |
| stone | `CD_Extracted/CINE/SHIP1.DFA` | 31734826 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `5e085c346627f4818f249ee4afaddabcfe265c9a` |
| stone | `CD_Extracted/CINE/SHIP2.ACT` | 2882 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `09b19ee0b4419d3052fb4bf3dc1f9715776cd76e` |
| stone | `CD_Extracted/CINE/SHIP2.DFA` | 38775289 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `d9da91141d6f9463d44ffb6952f959d5b879cd84` |
| stone | `CD_Extracted/CINE/SLAB-D.ACT` | 466 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `9762244de4c3bdf2b9f5bf5fd14df09badfc9bba` |
| stone | `CD_Extracted/CINE/SLAB-D.DFA` | 4999131 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `44c653c0461811e541316f99d472b0198b7d5927` |
| stone | `CD_Extracted/CINE/SLAB-N.ACT` | 530 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `08a49ddf46542a1ca484110d1d31bac6d3514a01` |
| stone | `CD_Extracted/CINE/SLAB-N.DFA` | 4836359 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `8c9920ca81273f279883d34aaddd98931bc61837` |
| stone | `CD_Extracted/CINE/TEARS.ACT` | 274 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `849971c749bc5857e5d06fb52a3a6ba94fe0b7cc` |
| stone | `CD_Extracted/CINE/TEARS.DFA` | 4449599 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `e6088194b433b30e43e6e4851dc740c4641252f4` |
| stone | `CD_Extracted/CINE/URCHSPK.DFA` | 99462 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `16566dd87924d0d66aa03d61c26fd9e3f0f36a16` |
| stone | `CD_Extracted/CINE/WATER-D.ACT` | 664 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `2ed74b28857a62a6f4f155a789bf90fdc13bdc16` |
| stone | `CD_Extracted/CINE/WATER-D.DFA` | 12074345 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `e229067227e8f608c3cf7dd2465172780e2bc5cb` |
| stone | `CD_Extracted/CINE/WATER-N.ACT` | 668 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `4c977112c752d438fd327ab350ce72f7548342e6` |
| stone | `CD_Extracted/CINE/WATER-N.DFA` | 13257006 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `a6059f566804f82e12601145e918e46636d9766c` |
| stone | `CD_Extracted/CINE/WIN-1.ACT` | 386 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `d45aef90ac207afd573369594ec3bcb488d5fa0d` |
| stone | `CD_Extracted/CINE/WIN-1.DFA` | 5613978 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `414b9cdcdb918775276c76f07796108914b88f3a` |
| stone | `CD_Extracted/CINE/WIN-1B.ACT` | 424 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `8b00b0926eb6256055fcfeda9dccb27d29570443` |
| stone | `CD_Extracted/CINE/WIN-1B.DFA` | 9392636 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `5f955d64c793a6565663832bbd5891e94b821e47` |
| stone | `CD_Extracted/CINE/WIN-2.ACT` | 386 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `87279ed4f2e1a811964fcda5d01d67acf45694e0` |
| stone | `CD_Extracted/CINE/WIN-2.DFA` | 5950132 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `d4d9dfe982e25836393b6382e5b4948fd283315b` |
| stone | `CD_Extracted/CINE/WIN-2B.ACT` | 422 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `8628ec1311217799a087fc2c4ab37a4c95005b33` |
| stone | `CD_Extracted/CINE/WIN-2B.DFA` | 9507218 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `00487973154f08ca07f8ddd66d3fd66428108082` |
| stone | `CD_Extracted/CINE/WIN-3.ACT` | 386 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `87279ed4f2e1a811964fcda5d01d67acf45694e0` |
| stone | `CD_Extracted/CINE/WIN-3.DFA` | 6035232 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `81467c76e7a3cb39df3b5648666762e97bee10fc` |
| stone | `CD_Extracted/CINE/WIN-3B.ACT` | 424 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `43f26e59cfe389735247d0e72224d64b4fada19f` |
| stone | `CD_Extracted/CINE/WIN-3B.DFA` | 9572616 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `a26877738b4f217be291fe8d2569b97293410df5` |
| stone | `CD_Extracted/CINE/WIN-4.ACT` | 386 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `87279ed4f2e1a811964fcda5d01d67acf45694e0` |
| stone | `CD_Extracted/CINE/WIN-4.DFA` | 5656042 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `d663a9b31d1a64d8164a208e9866348a01b8d1c9` |
| stone | `CD_Extracted/CINE/WIN-4B.ACT` | 422 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `94755e23279d09734d6a75c643278bb3c4904f3c` |
| stone | `CD_Extracted/CINE/WIN-4B.DFA` | 9464462 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `d6694f3bac623d3b8c2fe6743bd83e732bc9ed0c` |
| stone | `CD_Extracted/CINE/WINCAP1.DFA` | 936208 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `ae5815006199b2729c1d3407a920c7a201a24ae0` |
| stone | `CD_Extracted/CINE/WINCAP2.DFA` | 935358 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `51a861a96cbb84873702f1aa56fdea89185e1d16` |
| stone | `CD_Extracted/CINE/WINCAP3.DFA` | 945978 | video | UNKNOWN | HASHED; NO SOURCE REFERENCE | `e9e9e3f3493750112a42e4a22f7546ea88bf8f4e` |
| stone | `CD_Extracted/COLORS` | 72193 | resource archive | UNKNOWN | HASHED; NO SOURCE REFERENCE | `71903d5904424d0e790f8e591407ee8fa0c8f592` |
| stone | `CD_Extracted/DATA.A00` | 1020578 | UNKNOWN | UNKNOWN | HASHED; NO SOURCE REFERENCE | `eddc026b6f6060b55c5087c96f4d7a14e4ff0aa5` |
| stone | `CD_Extracted/DEARJ.EXE` | 39441 | executable | UNKNOWN | HASHED; NO SOURCE REFERENCE | `6e4ef2c366f132bac5682fe780899107b43725a2` |
| stone | `CD_Extracted/FILESPEC` | 328 | UNKNOWN | UNKNOWN | HASHED; NO SOURCE REFERENCE | `7c10c5664c43fd2b930468b71f3281c9b7bbf5d7` |
| stone | `CD_Extracted/HELP.TXT` | 8254 | text | UNKNOWN | HASHED; NO SOURCE REFERENCE | `3957a35a0b0c9d99c2e302b9eae9368d2095ee61` |
| stone | `CD_Extracted/INIT` | 2970 | config | UNKNOWN | HASHED; NO SOURCE REFERENCE | `7ae42ad9bc10c0ec4fee2981147f131b23476594` |
| stone | `CD_Extracted/INSTALL.EXE` | 102448 | executable | UNKNOWN | HASHED; NO SOURCE REFERENCE | `7f2d6c3ab9c2659a7a8bab75704f6eb77260e045` |
| stone | `CD_Extracted/INSTALL.NFO` | 410 | text | UNKNOWN | HASHED; NO SOURCE REFERENCE | `4145a0d711602d1c778f2f11a18c7c88540ee49f` |
| stone | `CD_Extracted/INSTALL1.BAK` | 410 | UNKNOWN | UNKNOWN | HASHED; NO SOURCE REFERENCE | `c0ddf67875ef565a4de8fea76d7ed3414d205167` |
| stone | `CD_Extracted/LEVELS.DAT` | 577226 | level | UNKNOWN | HASHED; NO SOURCE REFERENCE | `55b6428293201a5877bb4967cac25135547253e9` |
| stone | `CD_Extracted/LEVINFO` | 32784 | UNKNOWN | UNKNOWN | HASHED; NO SOURCE REFERENCE | `ea5965cb1af0ded940baf3812ce68a679f4d3ee6` |
| stone | `CD_Extracted/NPCS.DAT` | 9684 | resource archive | UNKNOWN | HASHED; NO SOURCE REFERENCE | `52ceafeefd8fade3d21e89f3e57d7c032e87d3eb` |
| stone | `CD_Extracted/OBJECTS.DAT` | 33544 | resource archive | UNKNOWN | HASHED; NO SOURCE REFERENCE | `52a54958a637cf9273d174efc187d836f3ef225a` |
| stone | `CD_Extracted/README.TXT` | 6932 | text | UNKNOWN | HASHED; NO SOURCE REFERENCE | `e0144f94b7f02ce4879149682814c4866ee30142` |
| stone | `CD_Extracted/RES0` | 497722 | resource archive | UNKNOWN | HASHED; NO SOURCE REFERENCE | `1888e1cff792b9cbba87b61f8844cb040459789f` |
| stone | `CD_Extracted/RES1` | 491638 | resource archive | UNKNOWN | HASHED; NO SOURCE REFERENCE | `479e9420652387e0e6b9f4338633333bf6d1ff28` |
| stone | `CD_Extracted/RES10` | 96017909 | resource archive | UNKNOWN | HASHED; NO SOURCE REFERENCE | `e55b035ec2c9b699834acefb7a6b239fe6ee14fe` |
| stone | `CD_Extracted/RES2` | 497894 | resource archive | UNKNOWN | HASHED; NO SOURCE REFERENCE | `33caa5cad27b4cbe7ff748bf55dceab588105781` |
| stone | `CD_Extracted/RES3` | 4453100 | resource archive | UNKNOWN | HASHED; NO SOURCE REFERENCE | `0f5fc3b9dec01c1383821dacbcba415f25dd6e5c` |
| stone | `CD_Extracted/RES4` | 6356579 | resource archive | UNKNOWN | HASHED; NO SOURCE REFERENCE | `df1f18f0a2595ee289a61a21f40052e43370ae49` |
| stone | `CD_Extracted/RES5` | 1479162 | resource archive | UNKNOWN | HASHED; NO SOURCE REFERENCE | `d1b6802cd00e07ca7a2c207e6a1c07abd78eb5be` |
| stone | `CD_Extracted/RES6` | 11193632 | resource archive | UNKNOWN | HASHED; NO SOURCE REFERENCE | `37fe831b3dc647772124ece289e62e98cd6521db` |
| stone | `CD_Extracted/RES7` | 219578 | resource archive | UNKNOWN | HASHED; NO SOURCE REFERENCE | `9d228baff076c33238caf0c83635ebff351035eb` |
| stone | `CD_Extracted/RES8` | 28083 | resource archive | UNKNOWN | HASHED; NO SOURCE REFERENCE | `3c241f74c246631bc16b660b5bc3442153af0bc5` |
| stone | `CD_Extracted/RES9` | 3070563 | resource archive | UNKNOWN | HASHED; NO SOURCE REFERENCE | `5c2c18d4e494c6dec9119505442680e578810c97` |
| stone | `CD_Extracted/SINTAB` | 8472 | resource archive | UNKNOWN | HASHED; NO SOURCE REFERENCE | `fc90835b95c932219c28e56dfd0bb664da82e8e7` |
| stone | `CD_Extracted/STATUS.ME` | 966 | UNKNOWN | UNKNOWN | HASHED; NO SOURCE REFERENCE | `0eedfdfdeef49e4b650962f2920cddb05a049fc2` |
| stone | `CD_Extracted/TABLES` | 1121280 | UNKNOWN | UNKNOWN | HASHED; NO SOURCE REFERENCE | `d1f74704af92107bc7bc5a5347eacfb37bc14f38` |
| stone | `Cluebook.pdf` | 1697672 | text | `59a0453f8c6a610c5205aaff6b931a710c8fce677b8d260469f7909a7101fb2d` | SHA-256 PASS (CI #1) | `153920d6696a0de7b40601ac6533083a0f16dc5d` |
| stone | `dosbox_prophet_single.conf` | 926 | config | `f736efbeb9713365a27400f1fbf55e23452c16e08ab405d2c26494088bf71612` | SHA-256 MISMATCH (CI #1) | `050bf29e9fd57f1837080111990d9ea8d9fc80cc` |
| stone | `dosbox_prophet.conf` | 11207 | config | `45a8d9684756dfcdaada08a4b796f52b8e541b5fd977f5efe0ee61090c96708c` | SHA-256 MISMATCH (CI #1) | `c7bba4751e6413185b634f06cf2651186e068e4c` |
| stone | `DOSBOX/Documentation/AUTHORS.txt` | 264 | text | `f6481008f772b0783585a8b583dd5dfa3ee986830aa71e19eed6300d16dc0203` | SHA-256 PASS (CI #1) | `eb365e5d6d4ad7b7b6358733e2981f1d8ea16cd7` |
| stone | `DOSBOX/Documentation/COPYING.txt` | 17992 | text | `32b1062f7da84967e7019d01ab805935caa7ab7321a7ced0e30ebe75e5df1670` | SHA-256 PASS (CI #1) | `d60c31a97a544b53039088d14fe9114583c0efc3` |
| stone | `DOSBOX/Documentation/INSTALL.txt` | 3833 | text | `c6679b562301614212c4bc0fd81798ed19324be8ccfd2e807856a41b39f7adff` | SHA-256 PASS (CI #1) | `cc48f8e25894151a678f24d6307aa29999a65f47` |
| stone | `DOSBOX/Documentation/NEWS.txt` | 31918 | text | `1e648756aa6c0d0010c26d3c884b4698f51003bdfea0329dac1d02706b501924` | SHA-256 PASS (CI #1) | `bf6b7a131434123d08525bcc22eb22865fa38eaa` |
| stone | `DOSBOX/Documentation/README.txt` | 62740 | text | `4a7b09b4a9b114221db38ae8258f30e430a37b6ac96ecb242d9350382beaf6a5` | SHA-256 PASS (CI #1) | `8bf8f63e57fdf1dfa3dee4355f467245b6d8d92d` |
| stone | `DOSBOX/Documentation/THANKS.txt` | 972 | text | `f356d80820412b8c7f956562dad5103e3704506bd15a831078e1a2feee361526` | SHA-256 PASS (CI #1) | `8b0e5d96fb83ddb0b8a9095bf8835b639e7a465c` |
| stone | `DOSBOX/DOSBox 0.74-2 Manual.txt` | 62740 | text | `4a7b09b4a9b114221db38ae8258f30e430a37b6ac96ecb242d9350382beaf6a5` | SHA-256 PASS (CI #1) | `8bf8f63e57fdf1dfa3dee4355f467245b6d8d92d` |
| stone | `DOSBOX/dosbox-0.74-2.1.tar.gz` | 1334686 | UNKNOWN | `1b6c865340e9d6119529c4ae61e2f12661424a38f07f8818202c5357a6b97ba2` | SHA-256 PASS (CI #1) | `993d5f215a68b175b016260be47ee80a63c39f5f` |
| stone | `DOSBOX/DOSBox.exe` | 3802624 | executable | `8a7a7fedd222bf985b51191000557f9ee2ffb033be02c6c59594dbbec013cd41` | SHA-256 PASS (CI #1) | `712e82a9406de38588fda627cebb2a23d39d72f1` |
| stone | `DOSBOX/GOGDOSConfig.exe` | 2989648 | executable | `0465cedb523cbaf56c7aa8f144f1ebdddb7e1ade23575ecf0a0ba2c934455c54` | SHA-256 PASS (CI #1) | `6310a3f3eac941b43327f734be6ce917dd960965` |
| stone | `DOSBOX/SDL_net.dll` | 13312 | executable | `2f39dc04acbecf47efa45034891602b6ea7bf6fd2f27b5c0a5ca8d7fb155c929` | SHA-256 PASS (CI #1) | `924e65403298ac3dc314dfec1ddc5d65831a27a8` |
| stone | `DOSBOX/SDL.dll` | 448231 | executable | `69037ebc43755296c0cc292d57d560028d7f2265f7b86ca84e714835c19bbd58` | SHA-256 PASS (CI #1) | `37d31517d044a891485220f91ee2be2151c5069e` |
| stone | `DOSBOX/Video Codec/Video Instructions.txt` | 1573 | text | `e478355e52d4be075d059441d3de1936bc43241ff17bc6270680e32f0881b5b1` | SHA-256 PASS (CI #1) | `1d15b4f9c51e8920b3a37621784d2ac0d91e8860` |
| stone | `DOSBOX/Video Codec/zmbv.dll` | 94208 | executable | `be72d291901b4a67741732da3d6dfd87fb3e74c8d03d56d40c58dd4d9744a478` | SHA-256 PASS (CI #1) | `c6c0eb63b7be968cab3d283e8413d39f115dd5c3` |
| stone | `DOSBOX/Video Codec/zmbv.inf` | 3938 | UNKNOWN | `f343e3b4693ae29cb5440382f3fee06572ae8fc6bce46cb673b6890bc735a026` | SHA-256 PASS (CI #1) | `a98ea01677054579ac9d901c41b8540c4d0de215` |
| stone | `gog.ico` | 69248 | UNKNOWN | `f6a71321521ba89713f0bd38b21f809e87e8a789cb172e8e4693f9479e30b1e4` | SHA-256 PASS (CI #1) | `5a9ec11e053e7f83467b2e3fff9090cf5a207dec` |
| stone | `goggame-1432906190.hashdb` | 1717 | UNKNOWN | `bf0335d95c3ed84cededa8f8d004632cf116ffca0f22a8a63d903895feef293e` | SHA-256 PASS (CI #1) | `82ebadb5e5a4c7a2ba3eac7b200d6ab55ceeea77` |
| stone | `goggame-1432906190.ico` | 247101 | UNKNOWN | `da46acd26f131328cc43ee6b38dfc56611a706d9dd642c6fb67ed77fa02f3edd` | SHA-256 PASS (CI #1) | `504ad6342021a0bcf363fa899f1b9066f7177c7e` |
| stone | `goggame-1432906190.id` | 38 | UNKNOWN | `1c7117a244e904775fa06376c62bc46e14660efaef5a8c51117b63cc1dff7251` | SHA-256 PASS (CI #1) | `46851aef7e2e22f9244b94b9e9e1cc38d41a5ba5` |
| stone | `goggame-1432906190.info` | 1687 | UNKNOWN | `d70da590b18c5fa396be2d4deb988d1eb0deb1d289ba666699a111325e52e7dd` | SHA-256 PASS (CI #1) | `25af5cc971535dc2641446a5cf498a827d92ff2a` |
| stone | `goggame-1432906190.script` | 1840 | UNKNOWN | `36b70584af2a6475a13da087a46e21907049a3358e33dd223f3f1db8d00dff1a` | SHA-256 PASS (CI #1) | `8202e1c88681cb5eb8e01876132573cee769779d` |
| stone | `goggame-galaxyFileList.ini` | 2387 | config | `b1525fbfc3c4c936afab6770f6bcdb176a7f5ded9c02f8a0bfb79c6cc52e81d5` | SHA-256 MISMATCH (CI #1) | `a0d22443f28aead4ec14f8b4cd6dba476d88ab54` |
| stone | `goglog.ini` | 869 | config | `85f3c9e7179004efdc506569ae16b44ea8759e90703f49af61659aa657c54aa0` | SHA-256 MISMATCH (CI #1) | `cea7c6bf0ba05d0cf0d34c21f76daddfef27477d` |
| stone | `Launch Ravenloft - Stone Prophet.lnk` | 1260 | UNKNOWN | `038ced3f082df3881181ab94d71368132263d9784ae0c6da132397ecdd809a7c` | SHA-256 PASS (CI #1) | `608f618b5f547f84b36e7a7f7047df9e6a67c8da` |
| stone | `Manual.pdf` | 1862772 | text | `5250fd483fed5cf0839c142ac488dba2abee9763d8b1ed1124c0fa3ee29b79cd` | SHA-256 PASS (CI #1) | `78457b4881c247c5d4f643d65e722316de6f661d` |
| stone | `PROPHET.zip` | 21539155 | resource archive | `9b9584919315b24f7e7b22c540411225ddbc6bcf199a90e38157dff2d869e173` | SHA-256 PASS (CI #1) | `d9dc95947c7cfe5273adf7b8cdf2720f742d95b0` |
| stone | `PROPHET/A32ADLIB.DLL` | 19968 | executable | `520db726d034394e7ea7f62b9bfa90767898990836522fc09f89931f2c388a7e` | SHA-256 PASS (CI #1) | `8e41fec4502ad9107afe298c9a063a5f79a6cc5e` |
| stone | `PROPHET/A32ALGDG.DLL` | 10752 | executable | `4e735e261934ff30bf2899d6ebe36d0d0a1452da455710142848071967306272` | SHA-256 PASS (CI #1) | `bce10f8e48e4468aa18065ddeb9081b39d7fc327` |
| stone | `PROPHET/A32ALGFM.DLL` | 24576 | executable | `b45484c60bbd353a99f2d4b38c0d187af66941c3b3f771df331790bb52818f32` | SHA-256 PASS (CI #1) | `6aaa2c5ac6561ac2f22002093d5b81cbd6f4d19d` |
| stone | `PROPHET/A32ARDG.DLL` | 16384 | executable | `6b0dcd7ea52e891d23f9f8b3af56e3774092d3e5a6a0dbcc1d4046ba39ae111b` | SHA-256 PASS (CI #1) | `812c1604898f49d4b8301ff2f1ff07fd397d2c27` |
| stone | `PROPHET/A32ARXM.DLL` | 53760 | executable | `5fa2815cb37d35e59c68801d1382c2bf6c8549d2b1cf25658c701434e1f4c25f` | SHA-256 PASS (CI #1) | `a799ea0b185072b231a910bfe730d08bb6660bf9` |
| stone | `PROPHET/A32GFX.DLL` | 45056 | executable | `986bd320b4dbf34968a6363311c0b9ada63d922f35f68c226f4de655c409992a` | SHA-256 PASS (CI #1) | `a337f66b6111d3d2bf8c71687e93b212a2fecc71` |
| stone | `PROPHET/A32MT32.DLL` | 14336 | executable | `c783b293f1664c248505c6bb0591acb9874cbdfaa55e9086a6abdc313820daf0` | SHA-256 PASS (CI #1) | `15c56e38da8d2ae724540e1f7d89cf1a19a47b3e` |
| stone | `PROPHET/A32PASDG.DLL` | 10752 | executable | `ca37ed704f4004c59ab546b959c689243b9ee8c600d595a9ef88f589a8775a89` | SHA-256 PASS (CI #1) | `f7bd1aa4154964029cc4ef536de65b477fc0f559` |
| stone | `PROPHET/A32PASFM.DLL` | 19968 | executable | `0309fecd637c3ae7dfb4c5096cad2c354c2acf267c0dda22fcfcdfc2a78d6b1d` | SHA-256 PASS (CI #1) | `8923737f84fe42ee0519a5aa86a334942b172a84` |
| stone | `PROPHET/A32PASOP.DLL` | 24576 | executable | `2fad5776664ab0e4027dfdcfd233b3bd35f946631904ae77b8400da802f455f4` | SHA-256 PASS (CI #1) | `dfc67837198693a878eeec08037e212f6e810c11` |
| stone | `PROPHET/A32SBDG.DLL` | 10752 | executable | `a6fa522cdfa9a65c0a85f142c1377b87023d41739a38d9792339dae959133588` | SHA-256 PASS (CI #1) | `d19e4886dae560233c3ef93791350ec4899d3f73` |
| stone | `PROPHET/A32SBFM.DLL` | 19968 | executable | `2919f0080c6d374094c5520d8394aa998ede2d5a2d07e9e32017e1f985d70622` | SHA-256 PASS (CI #1) | `4d40c5e034721b6ab7d208eb24a13840a5aeb103` |
| stone | `PROPHET/A32SBPDG.DLL` | 10752 | executable | `d1f0de508c92c2b8fc73be5f036c77c60dbb61a66651a26112eed1a39a396093` | SHA-256 PASS (CI #1) | `3453cb1b9751668d607aec44dc02fce412e9cf5c` |
| stone | `PROPHET/A32SP1FM.DLL` | 19968 | executable | `5a309bd1aaaa024b1553086ab610d671e8b53155f89f3b43c4eaf96b0cadbae5` | SHA-256 PASS (CI #1) | `245be4ba908962aacbd7669b025ae744d989b492` |
| stone | `PROPHET/A32SP2FM.DLL` | 24576 | executable | `2aaf3579c7d7a4012f23aea08da35d6a726dadee533a29b545d8ce0ce375e4df` | SHA-256 PASS (CI #1) | `4c4d28af777ccba3e63b40c5532e08a59e367682` |
| stone | `PROPHET/A32SPKR.DLL` | 14848 | executable | `da2b8348f435070e59b1d91eac806eb0bfb68690d96a02799d2a7828b8ca96ff` | SHA-256 PASS (CI #1) | `31a7c22f8a88562ef19273e01998102c1f739845` |
| stone | `PROPHET/A32TANDY.DLL` | 14848 | executable | `e6fc7c5dff1cffd1032a6a6e93f4d052361c3db027de047eb07dcc5a23d0b4d2` | SHA-256 PASS (CI #1) | `038971cb960f912df4f9e50f09e870d83923bae8` |
| stone | `PROPHET/CODE.1` | 313819 | executable | `004d042a70c620187ebbf3686d63bdbf7e8d2ffc35e1f20ffbac2f7d6126224a` | SHA-256 PASS (CI #1) | `a61640f2976d490f100469288ffef42feae6424d` |
| stone | `PROPHET/CODE.2` | 820497 | executable | `00e225ec8f141a8fdb72ff0547fe93b2ea3aabdb4ec3b59d9261a5268de9203e` | SHA-256 PASS (CI #1) | `91bf47f13ac558495b252d92239d51e8f02314a0` |
| stone | `PROPHET/dosdis-candidate-analysis.json` | 9102240 | UNKNOWN | UNKNOWN | HASHED; NO SOURCE REFERENCE | `0a255fbe168e1c80ed0ddd99581d9ee23099f766` |
| stone | `PROPHET/GM1.BNK` | 23907 | audio | `843f012e7980e0a58722aa5c7a6e88b652f2f8d404834638cd994ca0869df5a8` | SHA-256 PASS (CI #1) | `0b1037e22a21be99811b605081e254c2a9681f43` |
| stone | `PROPHET/GM2.BNK` | 24273 | audio | `b4af656059a6afba5db2da972c3506ca08d899ca04c65389e0bec7aaf6536339` | SHA-256 PASS (CI #1) | `aa4eb15b776535805fceddc11d81514c96974272` |
| stone | `PROPHET/GUSDIG32.DLL` | 28959 | executable | `9fc52cbc9bd95484a898defc1fbff9af90a443a74e073de5b80e8e11fc7eee41` | SHA-256 PASS (CI #1) | `2c2b2799675e53402062325878cbbac5ab0a1c9d` |
| stone | `PROPHET/GUSMID32.DLL` | 59865 | executable | `13f3b30e4d1d218502b38bd6bd40ee984c502dfcd4d9393a1fc21bae2a461e99` | SHA-256 PASS (CI #1) | `ee0eb08179f80e99dda7e6fc4a10b3d0e1dab010` |
| stone | `PROPHET/LOADPATS.EXE` | 24705 | executable | `17f40351cf53e445dbb62a041ff90b18dd755f6e1f3a216354e14aaeaa50f8ea` | SHA-256 PASS (CI #1) | `55537c435d49520cb0b11370fb7f0a3230ce51d4` |
| stone | `PROPHET/PATCHES.INI` | 6952 | config | `3075d1660a15ff7ecb2f143ff2c12b2593d337b61ae06ab60a4f9cdd32e8d93b` | SHA-256 MISMATCH (CI #1) | `3d9b15de08ad25217618f7dd0046fdbb13f64c1b` |
| stone | `PROPHET/PROPHET.BAT` | 2617 | executable | `1c06935f1f28c2cfaf5cb19eac2ab81a0f671be0f3816db0b201109f41e7b1c3` | SHA-256 MISMATCH (CI #1) | `63dfaccb639845ae8c404e8ea8e583b8fdae03b6` |
| stone | `PROPHET/README_1.1.TXT` | 1300 | text | `7d76ca647e1156fa147a24e67a9787649dceec8c69fe6c49e0e7e52b5997012c` | SHA-256 MISMATCH (CI #1) | `892805095b34520105d4a89634c19887134f2bb0` |
| stone | `PROPHET/README.BAT` | 37 | executable | `2bf7a18f998c2b4dd3188ce4988db42b06417a399116a13d152904e71c2a61be` | SHA-256 MISMATCH (CI #1) | `e9688f7f3a0b2e31158e0ac32e086cb65b9d7f2f` |
| stone | `PROPHET/README.TXT` | 4179 | text | `bec69bb2de80d07718d0ef0e8f8051d443288673b6b22e13dc91ac063ec94962` | SHA-256 MISMATCH (CI #1) | `e0517151c8ff40c016dd5b4b18f09438a58ae39e` |
| stone | `PROPHET/RES0` | 497722 | resource archive | `f6529845c0aab9e26bb951bfe8f2619d661b8cd0e35f8c788e5314cd220bb306` | SHA-256 PASS (CI #1) | `1888e1cff792b9cbba87b61f8844cb040459789f` |
| stone | `PROPHET/RES1` | 491638 | resource archive | `44c81061b93be8820d4fca8f9146c651d9aff02e952e434bdd9c5e8af5dae4f9` | SHA-256 PASS (CI #1) | `479e9420652387e0e6b9f4338633333bf6d1ff28` |
| stone | `PROPHET/RES2` | 497894 | resource archive | `0d47a3cf5e3aaa3f743b107d1430fe5976702aed77bd30c42bc894ef803a7b53` | SHA-256 PASS (CI #1) | `33caa5cad27b4cbe7ff748bf55dceab588105781` |
| stone | `PROPHET/RES3` | 4453100 | resource archive | `09440cb00e4ca741a3abc99bae810a6c600d10a474378efa8a8362fedf6b0534` | SHA-256 PASS (CI #1) | `0f5fc3b9dec01c1383821dacbcba415f25dd6e5c` |
| stone | `PROPHET/RES4` | 6356579 | resource archive | `17a2b3569806af98a030f3d653b79736e623b4b703531306b457d0c5d0a1c7e4` | SHA-256 PASS (CI #1) | `df1f18f0a2595ee289a61a21f40052e43370ae49` |
| stone | `PROPHET/RES5` | 1479162 | resource archive | `1b7f69c587994fe3216c62cfa404a6ba1cfacf61f9608ed45783603410eede9c` | SHA-256 PASS (CI #1) | `d1b6802cd00e07ca7a2c207e6a1c07abd78eb5be` |
| stone | `PROPHET/RES6` | 11193632 | resource archive | `7651c017d5fbe280cf40e5b57c1f2eb7bfa178a52fe1b4999cad8e05116ff46f` | SHA-256 PASS (CI #1) | `37fe831b3dc647772124ece289e62e98cd6521db` |
| stone | `PROPHET/RES7` | 219578 | resource archive | `f8b3cd644164a9b3398727de28ece7cc47f3deb2bf371b447a358addf09424e5` | SHA-256 PASS (CI #1) | `9d228baff076c33238caf0c83635ebff351035eb` |
| stone | `PROPHET/RES8` | 28083 | resource archive | `f38b281e87e046b076a09f58ddea13a3d067593f25cf25450aa65ac862b74f02` | SHA-256 PASS (CI #1) | `3c241f74c246631bc16b660b5bc3442153af0bc5` |
| stone | `PROPHET/RES9` | 3070563 | resource archive | `e17f2a1a4868fe580f346c2f273312f6c72bb2eec33b3af1118eac5134efff76` | SHA-256 PASS (CI #1) | `b44d4187264f1a0d6e302fdae145dd57742f72f9` |
| stone | `PROPHET/SAMPLE.AD` | 4662 | audio | `79ee9f4ad3134be906ce493072ce0c3b7169c9013d3c25947ac385aaf3f9633b` | SHA-256 PASS (CI #1) | `bfccc365b141d38f2f31bd528c918dea396a6bc0` |
| stone | `PROPHET/SAVE01/SAVE.DAT` | 39002 | save | `d7dc62a5181082df83e376ea9e061f10d15af3f865fa9d6e60feb514a4dd56f2` | SHA-256 PASS (CI #1) | `5f512cf4f3177e20f8c85c43c32e21df7f58b0a6` |
| stone | `PROPHET/SOUND_PM.EXE` | 481869 | executable | `a91a45b3aadf1fb5208ea594668c26baf990ac3db0448000fafe784f54c19bba` | SHA-256 PASS (CI #1) | `046289c289fc66eb6519765f0bbcc465c7bc56e0` |
| stone | `PROPHET/SOUND.BAT` | 2641 | executable | `df8404510a71e3ce2daf18b987dec6bbfd85b0d9782a4ed097f2d2ae258fea6e` | SHA-256 MISMATCH (CI #1) | `8760051784b17538970222876c42fe761d28441c` |
| stone | `PROPHET/SOUND.CFG` | 59 | config | `ca99ba88707b0d4c8a67246cfe19bb412bac5093e22393dc04d882e6dc8f3f3b` | SHA-256 PASS (CI #1) | `389ce222f72aa2c7ea9dd20da2d678615e6c8c66` |
| stone | `PROPHET/SOUND.INI` | 27181 | config | `d06c1894bdbb5d64c8a17990c3879722cfb81a4b3f016ab9a3c3bf6d95b66b61` | SHA-256 MISMATCH (CI #1) | `7ce713fbb5aa30bbef6b2c8c8427ba6d4baec10e` |
| stone | `PROPHET/SP.EXE` | 12736 | executable | `abd21528c111cf1bdbcf718d8ed8d0f28851f986f5b9a5f0faa6d8bbbe6db2c1` | SHA-256 PASS (CI #1) | `eea1542857300e80b626d7fced13aa3a0ce24bc1` |
| stone | `PROPHET/T` | 192023 | resource archive | `0091d1a165c48b5bff73c75311e6272af7915d2cf328ec7cbc4123dcdb0c5f01` | SHA-256 PASS (CI #1) | `5419ea127d54ae2a8d86d13e6c24269f092d2ee8` |
| stone | `README.md` | 455 | text | UNKNOWN | HASHED; NO SOURCE REFERENCE | `1abeb637a3eb4861f0a9e623c0e4375ceb20207b` |
| stone | `SOURCE_INVENTORY.csv` | 11254 | text | UNKNOWN | HASHED; NO SOURCE REFERENCE | `984ffb0c6698a450c8e16736fb92273aea345010` |
| stone | `support.ico` | 62895 | UNKNOWN | `05fe749ef47d1ec862d6c55be78e66d1011226bf1f78409acf57cb79cec5eb20` | SHA-256 PASS (CI #1) | `e1d26f4fa2e1df83ba4be87e8a356148acd5d0d6` |
| stone | `unins000.dat` | 1327495 | resource archive | `e8a077277999ebaf055fba7b6ba23ef12b939ed8679adb5bd5e59ab100996ca8` | SHA-256 PASS (CI #1) | `5ef7f9bbc45261a448a82948cbb3c5a001bfc5c6` |
| stone | `unins000.exe` | 1343048 | executable | `5761e7789d813626cd68ee1e62429cfeb92bdd814cd29ef12fc4ae9ec1dbaff3` | SHA-256 PASS (CI #1) | `43dfb90dda07d2fa0c3500ff1ee5af10ab6504c3` |
| stone | `unins000.ini` | 39 | config | `97d6079c739190d9a24afe3f8e42f086ebd6cdd8e8ca483d645ec31155e64812` | SHA-256 MISMATCH (CI #1) | `b74cbe8288641b1c1e34fedd284e71b03a07efd6` |
| stone | `unins000.msg` | 23147 | UNKNOWN | `e43d06ec2b3dbe3d81bcd6b7880d28d074dac54b38646a605cfc5c809939da16` | SHA-256 PASS (CI #1) | `aa742051830bda5f367d343b3ed5bff183f60d7d` |

## Phase 0 disposition and remaining caveats

- **Complete for the available preservation checkout:** 624 of 624 existing files independently SHA-256 hashed; 239 of 239 extracted CD files included. A machine-readable audit artifact was uploaded and four unchanged Strahd analyzers passed in CI. **Confidence: VERIFIED** for checkout integrity and measured counts.
- **External disc containers:** Menzo `game.gog`; Stone `gog.game` and `gog.zip` are absent as outer archive containers, while their extracted directories are present and hashed. Their original container hashes remain **UNKNOWN** and cannot be inferred from extracted-file hashes. These containers are **not required** for the unified engine's file-based data loaders.
- **Twenty source-archive manifest mismatches:** independently verified for installation, launcher, text or config files, with causes not established. Do not claim they are newline-only without testing.
- **Stone RES9 copy divergence:** `CD_Extracted/RES9` and `PROPHET/RES9` have different SHA-256 hashes despite matching sizes. This is a **Phase 1 format/loader provenance question**, not a reason to discard either file.
- **Automated tests:** read-only three-title full-checkout hashing, source-manifest comparison, cross-copy comparison from the audit artifact, and four unchanged Strahd analyzers. **User-performed tests:** none. No Windows gameplay session was run or claimed.

**Next step:** review the Phase 0 inventory and authorize Phase 1 cross-title format analysis. Do not write unified-engine gameplay code before Phase 2's design review.
