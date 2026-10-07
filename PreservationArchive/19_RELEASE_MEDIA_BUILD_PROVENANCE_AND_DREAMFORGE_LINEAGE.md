# Ravenloft: Strahd's Possession — Preservation Engineering Archive

## 19 — Release, Media, Build Provenance, and DreamForge Lineage

**Status:** SUPPLIED-BUILD STATIC PROVENANCE AUDITED — historical patch deltas and exact media mastering remain OPEN  
**Working branch:** `code1-decompilation`  
**Frozen repository evidence baseline:** `373c7f10e6fc0f29c1e805125ef048150fcc7ba7`  
**Evidence hierarchy:** immutable supplied binaries and certified format audits first; dated storefront/publisher records and documented contemporary media second; historical community testimony third. Never infer executable equality solely from matching game title, file name or platform.

---

## 1. Which historical object are we preserving?

The subject is the **supplied DOS CODE.1/CODE.2 and resource corpus**: a specific game executable revision accompanied by a collection of shipped and older serialized resource generations. It is not a generic, edition-independent "1994 game" and not a verified bit-identical image of every CD, diskette, regional edition, or storefront installer.

Every file should carry its own **hash, version evidence and source/media provenance**. Different timestamp generations on a single mastered disc are historically meaningful; so are installation-specific records that naturally change when a game is installed.

## 2. Direct identification of the supplied executable pair: version 1.2a

Original `CODE.2`, file byte offset **`0x7E1E4`**, stores the NUL-terminated literal **`Ver 1.2a`**. This is **BYTE-PROVEN executable version text**, not a later DOSBox or GOG installer label.

| Supplied file | Size | SHA-256 | MD5 |
|---|---:|---|---|
| `CODE.1` | 277,787 | `fd9b709ceee190e6a70870f9012ebea3bc437cae6bc74e0150d6f922fa750fed` | `b6916a96f85c65f318663413ac1269f6` |
| `CODE.2` | 558,689 | `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314` | `ef764911c84b9c55bfcee1e5300399fd` |

Those **two MD5 values match exactly** the 1.2a-patch executable checksums posted on **2015-10-29** by community patch researcher *micartu* in the [GOG Ravenloft patch discussion](https://www.gog.com/forum/dungeons_dragons_ravenloft_series/strahds_possession_need_help_with_bugsglitches/post10). This secondary match corroborates the independent primary executable string; it is not the sole basis for version assignment. GOG's [2015-11-03 release notes](https://www.gogdb.org/product/1432905053/releasenotes) separately say its English edition was updated to **1.2a**.

**Classification:** supplied **executable pair = 1.2a (high confidence / direct string plus checksum match)**. Whether every data file on the associated disc originated in the same packaging/mastering event is a separate question.

## 3. INFO.DAT mismatch is not proof of a mixed executable patch

The same 2015 post gives patch reference MD5 `e64b6744671a557bd34f24d122752815` for `INFO.DAT`. The supplied `INFO.DAT` instead has:

```text
size        17 bytes
MD5         e376e8e0e8918ff1dfbf8e4fbe8a6f55
SHA-256     4ffa6ac5d5b5afd103bd67d84d3a372b401b3b19407b0c68c44bb0dfd9e32c30
raw hex     30 30 30 30 30 30 30 30 30 31 31 31 31 02 00 44 3A
```

The last two bytes encode ASCII `D:`; the preceding bytes include ASCII digit/selector data. These are file bytes, not a claim that `D:` was the drive of an original retail master. [RES11/RES12 lineage](../Resources/RES/RES11_RES12_LINEAGE.md) already notes that `INFO.DAT` selector/path content is installation-specific. A different INFO hash is therefore **a file-level mismatch requiring field-level comparison**, not evidence that CODE.1 and CODE.2 have different executable patch levels.

Neither the published 1.2a `R.EXE` MD5 nor a complete original 1.2a media-image manifest has yet been cross-checked with independently supplied matching bytes here. **Do not declare this complete installation an untouched Masterpiece Collection disc.**

## 4. Release/platform chronology — external historical corroboration

| Date / period | Historical record | Caution |
|---|---|---|
| **1994** | DreamForge Intertainment DOS release, US publisher **Strategic Simulations (SSI)**, TSR license; German publication by **Softgold** | Database year is not proof of an exact day or of this project's specific executable bytes |
| **April 1995** | Japanese **FM Towns** release via **Ving** | Separate platform build; do not assume DOS code/resource binary compatibility |
| **June 23, 1995** | Japanese **PC-98** release via **Ving** | Separate platform build |
| **Mid-1990s** | Later compilation distribution, including **AD&D Masterpiece Collection** | Edition/repackaging name does not on its own identify resource file hashes |
| **1997** | Documented US SSI rerelease with PDF hint book | Packaging fact, not an established CODE.2 recompile |
| **2015-11-03** | GOG English update to **1.2a**; German release described there as **1.2** | Storefront wrapper chronology, not original 1994 source-build date |
| **2022-03-29** | Steam listing under SNEG's **Dungeons & Dragons: Ravenloft Series** | Modern distribution date, not date of game's original creation |

Historical sources: [MobyGames releases](https://www.mobygames.com/game/3315/ravenloft-strahds-possession/releases/), [GameFAQs historical releases](https://gamefaqs.gamespot.com/pc/564797-ravenloft-strahds-possession/data), [GOG 2015 release notes](https://www.gogdb.org/product/1432905053/releasenotes), [Steam 2022 listing](https://store.steampowered.com/app/1904570/).

The GOG note says 1.2a *should resolve* a then-reported missing `cine13.anm` issue. **Do not translate this into a fabricated `CINE13.ANM` corpus member**: the supplied independently audited ANM set contains thirteen files **CINE00..CINE12**. The historical report can reflect an executable/resource mismatch or superseded path; its exact repair mechanism requires comparing the older executable and media images.

## 5. Binary/toolchain layers — different version numbers

| Layer | Controlling evidence | Correct interpretation |
|---|---|---|
| 16-bit launcher `R.EXE` | 12,368-byte MZ image and embedded Borland runtime identification | DOS/Borland process supervisor, not 32-bit gameplay object |
| `CODE.1`, `CODE.2` | DOS/4GW Professional-bound nested LE images | Protected-mode applications; common bound loader up to `0x25D23` |
| Bound DOS/4GW runtime | `Version 1.96`, `Feb 17 1994 11:54:25`, Rational Systems copyright | **Middleware** version/build identification, **not** the game patch version or CD mastering date |
| Watcom C/runtime | Object-1 code and matched surviving Watcom runtime startup | Vendor runtime lineage identified; exact full original compiler command line not recovered |
| `SINSTALL.EXE` | `SSI Sound Flat Model Installer for Ravenloft 4.2.6b` | Sound configuration/installer identifier, **not** `Ravenloft 4.2.6b` game version |
| AIL/32 / drivers | Miles AIL/32 original timer-service ABI; A32/GUS LX-style driver images | Third-party driver versions/behavior independent of the game's `Ver 1.2a` string |

Source ledger: [R.EXE structural reconstruction](../Runtime/R_EXE/01_STRUCTURAL_RECONSTRUCTION.md), [bound application map](../Runtime/CODE_OBJECT_MAP.md), [Watcom/DOS startup](02_PROTECTED_MODE_STARTUP_AND_MEMORY_RUNTIME.md), [installer/driver triage](../Runtime/OPTIONAL_EXECUTABLE_DRIVER_TRIAGE.md), and [AIL/32 timing](17_TIMING_INTERRUPTS_AND_FRAME_CADENCE.md).

## 6. Disc-resource chronology versus executable revision

Supplied CD-derived timestamps recorded in the [legacy resource lineage audit](../Resources/RES/RES11_RES12_LINEAGE.md) place **RES11/RES12 on March 4, 1994**; active successors were stamped during **March 9–13, 1994**. Cross-generation record comparisons identify the older composite archives as precursors to the split RES3/4/5/6/9 structure, while the supplied CODE.1/CODE.2 initialize only indexed **RES0..RES10**.

This supports **resource-content lineage and partial ordering**, not an exact "version 1.0/1.1/1.2" mapping. Preserved timestamps may originate from authoring, copying, mastering or archive repackaging. Retained obsolete archives are evidence about production history, not automatically a second running game.

## 7. DreamForge/SSI authorship and adjacent-engine lineage

The 1994 DOS credits identify **DreamForge Intertainment** as developer and **SSI** as publisher. [MobyGames credits](https://www.mobygames.com/game/3315/ravenloft-strahds-possession/credits/dos/) attribute lead programming to **Don Wuenschell**, additional programming to **Thomas J. Holmes**, game design to **Holmes and Christopher L. Straka**, music/SFX to **Jamie McMenamy**, and interaction/rulebook text to **Scot Noel**. These are credit attributions, not a recovered original source-control commit history.

DreamForge's earlier corporate name was **Event Horizon Software**; the company's historical record places its rename in **1993**. [Developer record](https://www.mobygames.com/company/159/dreamforge-intertainment-inc/). Contemporary-title references identify **Menzoberranzan** (1994) and **Ravenloft: Stone Prophet** (1995) as related-engine games: [Menzoberranzan](https://www.mobygames.com/game/1375/menzoberranzan/), [Stone Prophet](https://www.mobygames.com/game/3314/ravenloft-stone-prophet/).

**Preservation constraint:** shared engine lineage makes those releases useful for **comparative archaeological research**, not direct proof that similarly named files, packed records, script opcodes or executable offsets are interchangeable. Compare immutable bytes, format geometries and caller contracts before borrowing any interpretation.

## 8. Historical unknowns and next evidence gate

Still unproved: the exact retail mastering source/media image for this supplied installation; field-by-field INFO.DAT variation across installations; full historical patch manifests for 1.0/1.1/1.2/1.2a; a direct earlier-executable comparison for `CINE13`; exact RES11/12 source/editor build; cross-platform FM Towns/PC-98 binary layouts; and the DreamForge original compiler/linker command line.

Next: establish a **per-file provenance matrix** (hash, native/legacy status, CD timestamp, consumer, recorded distribution), search archived patch/manifests where lawful and available, then turn the remaining verified release history into a cross-version compatibility boundary. Leave missing documentary evidence explicit.



## 9. Per-file original evidence matrix: directly available binaries

A file's **SHA-256** is its byte identity. Version and media attribution are separate columns of evidence: the verified local source corpus contains eight originally supplied runtime/data files plus **SINTAB**, the latter also appearing in the independently certified 33-file Resources manifest. An original file present in the local evidence set does **not** establish its original DOS CD drive or directory.

| File | Size (bytes) | SHA-256 | Interpretation |
|---|---:|---|---|
| `CODE.1` | 277,787 | `fd9b709ceee190e6a70870f9012ebea3bc437cae6bc74e0150d6f922fa750fed` | Executable; 1.2a independently matched |
| `CODE.2` | 558,689 | `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314` | Executable; embedded `Ver 1.2a` at file `0x7E1E4` |
| `INFO.DAT` | 17 | `4ffa6ac5d5b5afd103bd67d84d3a372b401b3b19407b0c68c44bb0dfd9e32c30` | Installation/resource-location selection; **not** a fixed CD-wide revision marker |
| `LEVELS.DAT` | 499,878 | `50d0b1d13f81733a1ac55296d00fcba9aef002202bc00341f1062c37fd017d91` | Packed world baseline; no patch-to-patch hash comparison yet |
| `LEV022.DAT` | 6,028 | `f88b033ff4cdafdd89620e0fac51ff2fb81bce103a6d31a26b91464532c9f51e` | Mutable level-22 save fixture, **not** an original immutable level baseline |
| `OBJECTS.DAT` | 21,844 | `f23f0e7c1856904aa0156b0bf174cab794a41a327352b6ca00c3591ae5f2eed0` | Original static item-definition database |
| `NPCS.DAT` | 5,731 | `f2dd1175bf42e8291b95952282dfc0cfb84455aaa532987cdd7380094c8eb30b` | Original static NPC-definition database |
| `SAVE.DAT` | 9,288 | `f898cb92fca6447954f5e0b2bf6f8ad2435e4a64ae3b9de74f8bf75e0ed10d5d` | Quick-start/mutable-state fixture; not fixed per retail revision |
| `SINTAB` | 333,757 | `19b6bc15f1be616bb8ff49dc9029995a1a0413fae06addc9fd41ddef761d0e87` | Executable rasterizer support; independently matches certified resource-manifest identity |

**Audit correction P-19.1:** the previous §3 accidentally transcribed the INFO.DAT MD5 as `e376e8e0e8918ff1dfbf8e4a6f55`. The original 17-byte file actually hashes to **`e376e8e0e8918ff1dfbf8e4fbe8a6f55`**. The published 2015 patch reference MD5 `e64b6744671a557bd34f24d122752815` is unchanged; the discrepancy is genuine. This correction changes **documentation only** and affects no serialized format/behavior. MD5 is retained solely to compare the published historical patch post; use SHA-256 as the project's controlling file identity.

## 10. Complete 33-file resources provenance manifest

The table below is transcribed **mechanically** from [Resources/FINAL_RESOURCE_AUDIT.md](../Resources/FINAL_RESOURCE_AUDIT.md), where the 33 source identities were certified against the original resource corpus. Only `SINTAB` was independently re-hashed from currently mounted resource bytes in this continuation. The remaining 32 are **previously certified repo evidence**, not files newly re-downloaded or rehashed here.

| File | Size (bytes) | SHA-256 | Source role |
|---|---:|---|---|
| `CINE00.ANM` | 13,781,936 | `6627ebb7e91cc62c56871ad5140a5df038fde198b079e94b010467433ce9eccb` | ANM corpus |
| `CINE01.ANM` | 16,009,364 | `47a1e618f687351ebdb3e9ed07961f0feb3a42969f4cf984e48884c548153362` | ANM corpus |
| `CINE02.ANM` | 7,498,347 | `62a06c3ecf78c3f7907279592cc042f652cbcafeb8694027ce72f5d6995d9199` | ANM corpus |
| `CINE03.ANM` | 7,498,347 | `7da29505c4aac4f3449eb8feed9f119f9f80101fb0c91ffdb46938956e3b6ce6` | ANM corpus |
| `CINE04.ANM` | 7,498,347 | `78f352452abfeb7c9f4f3edb34f30c4c44f30752b88e6e783a04d1a3c5d66c2f` | ANM corpus |
| `CINE05.ANM` | 7,498,347 | `6f991c789a1a99c2d182bae55426b6e4b00e5ceb210871874518afd0a5aefbe6` | ANM corpus |
| `CINE06.ANM` | 7,339,821 | `7149f7a9020a1d718970ce2fa610cfbfab76b5f89d95caae17b78e119d9e3925` | ANM corpus |
| `CINE07.ANM` | 34,569,939 | `70cefedf8d2cb68525e6ebdcb8bf9e4be07e4619d12610b537e2f3c9841685ef` | ANM corpus |
| `CINE08.ANM` | 9,255,472 | `cfbc8b5835906ceedcf74e8c202430cd0b5b4e4d4c7c00054d42b934def817f3` | ANM corpus |
| `CINE09.ANM` | 5,485,203 | `287c3752048c5545c78f51d09035635bcd5422849770482918179a0d89f9387c` | ANM corpus |
| `CINE10.ANM` | 56,273,227 | `e7d6404057d1ea472f0ee1ec870b9098fff9adb86020af08d2e531eae6961bfd` | ANM corpus |
| `CINE11.ANM` | 57,092,507 | `9ee3f32320e0ae975b496ec69d34691ff45cc9e48fbf54a5de4a3f2fb5ef067e` | ANM corpus |
| `CINE12.ANM` | 1,446,666 | `a39bdb4965ba27f9fd1b1f4a8fb2ac291a48da831070cdf226faab229b132291` | ANM corpus |
| `RES0` | 2,186,991 | `5e30584947c9627e31b5dd2573ad75c2d3cc9fe796ac669e0ca5d02d6db7e61d` | Indexed RES 0–10 |
| `RES1` | 354,310 | `c13b8ae42f7d3b65c06d8413627f40a2a155a7404da44abfc158b38a4e5efd19` | Indexed RES 0–10 |
| `RES2` | 63,736 | `bd46022e716fb1014ce0d0be98fa2c8a1cad7970df6943b1b4d54d017581a8f8` | Indexed RES 0–10 |
| `RES3` | 5,228,638 | `44955ebbed0a8ed8d874fae07b01710317044882ab3c7c95d9be54e883df9a36` | Indexed RES 0–10 |
| `RES4` | 1,975,542 | `d8989c50fa58e317ff33465e7baec16e970f40586417d790fd8a1b9005c60f46` | Indexed RES 0–10 |
| `RES5` | 1,510,960 | `f943c933cfe593bb2912bc25e10b72f1ecafe10f684526f38489616cedbe91b6` | Indexed RES 0–10 |
| `RES6` | 6,210,899 | `697aa0803b2219dcb094bfae769ffb0c60cd7d89f7a7ba06c12ed6a4553f736d` | Indexed RES 0–10 |
| `RES7` | 211,133 | `78b46977d1d3232824392ec1e4852519a6d2e20277adb0e78a67c7605ee9c4e1` | Indexed RES 0–10 |
| `RES8` | 58,772 | `bfadd70c836260143f7e28b34128a0a16e7ca1743379a00176d2adfa525b28b4` | Indexed RES 0–10 |
| `RES9` | 8,061,481 | `c3021a8bb16bf1d8b9234ac47a670e4a41b8a1c078cba34be34f0f89a5135543` | Indexed RES 0–10 |
| `RES10` | 46,060,354 | `dc0c6b25a521f54d82bc564168859c74aa4b464413338d4b58d519b166e8e6f9` | Indexed RES 0–10 |
| `RES11` | 15,589,639 | `5c231d0e117def4f465f9b8d24ea6a315f610919e5fdd0910b9c2446d435d903` | Legacy/pre-split RES |
| `RES12` | 12,150,551 | `d03808be3f96ae48eab9f991433e8f7c4af989a6cf76ae9cb5a14f6964e40855` | Legacy/pre-split RES |
| `GM1.BNK` | 23,907 | `843f012e7980e0a58722aa5c7a6e88b652f2f8d404834638cd994ca0869df5a8` | Third-party ARIA bank |
| `GM2.BNK` | 24,273 | `b4af656059a6afba5db2da972c3506ca08d899ca04c65389e0bec7aaf6536339` | Third-party ARIA bank |
| `COLORS` | 11,521 | `e366986bf701ba832aa769bf99f0db90a76af658300ce3d02e7039a970a9baf5` | Palette support |
| `INIT` | 1,879 | `cc48df4eb4c0710d45e7f5557e440129df44c5efb1af3ca3426ea9f9e3798276` | Resource index/bases |
| `SAMPLE.AD` | 4,662 | `79ee9f4ad3134be906ce493072ce0c3b7169c9013d3c25947ac385aaf3f9633b` | AIL instrument support |
| `SINTAB` | 333,757 | `19b6bc15f1be616bb8ff49dc9029995a1a0413fae06addc9fd41ddef761d0e87` | Rasterizer support |
| `T` | 133,584 | `a3f1e19e40f226c8cf150a43aacd9c378ee35a1294d2c9dd72870bfdf1e16315` | Text index |

The provenance matrix deliberately separates the *resource corpus presence* of `RES11`/`RES12` from the *executable indexed-loader selection* of **RES0–RES10**. It similarly identifies `CINE00.ANM` through `CINE12.ANM` as the **thirteen** supplied cinematic sources without inventing `CINE13.ANM` as a fourteenth file. Full size/hash identities provide comparisons for future CD-ROM image, GOG `game.gog`, Steam payload, diskette, regional and compilation dumps **without implying equality before actual matching**.

## 11. INFO.DAT installation routing and historical selector boundary

The [certified INFO.DAT parser](../DatFiles/INFO.DAT/README.md) establishes an exact **17-byte** physical structure, not an opaque revision marker:

```text
offset 0x00  ASCII decimal resource_location_digits[13] = "0000000001111"
offset 0x0D  LE uint16 resource_path_length = 2
offset 0x0F  resource_path bytes = "D:"
```

CODE.2 `2E768..2E7EE` reads and converts all thirteen ASCII digits, but its indexed archive initialization supplies a **count of 11**. Within that count, zero selects the working-directory archive and a nonzero selector prepends the shared path. For the supplied configuration, `RES0..RES8` resolve locally while **`RES9` and `RES10` resolve through `D:\RES9` and `D:\RES10`**. The same two-byte `D:` prefix is also used for numbered external interactive resources (e.g. `D:\07`).

Selectors **11** and **12** are physically present and parsed but lack proved active consumers in this executable pair. They must be preserved **without claiming the 1-valued digits directly enable RES11 or RES12**. This is an important practical reason an `INFO.DAT` checksum may differ between valid installations with identical game executable bytes.

## 12. Evidence of 1995 patches — but not recovered binary deltas

The contemporary [9 May 1995 patch-list posting](https://groups.google.com/g/comp.sys.ibm.pc.games.adventure/c/c8ZPgcpO8Tw) independently records distinct CD and diskette patch distributions:

| Listed version | CD-ROM update | Diskette update |
|---|---|---|
| 1.1 | `RLFTCD11.ZIP` | `RLFTDK11.ZIP` |
| 1.2 | `RLFTCD12.ZIP` | `RLFTDK12.ZIP` |

A preserved [1.1 CD updater notice](https://www.oocities.org/timessquare/battlefield/7222/strahd.html) separately describes extracting `RLFTCD11.EXE` and running `PATCH`, while explicitly excluding the diskette version from that updater. **ZIP/archive naming and installer-stage EXE naming describe different packaging layers**; they are not contradictory claims about a single filename.

Those contemporary notices prove that media-specific updates were distributed by 1995. They do **not** supply authenticated complete contents, hashes, byte-level before/after deltas or patch scripts in the current evidence set. The 2015 community claim that 1.2a came through the Masterpiece Collection, with MD5s for `CODE.1`, `CODE.2`, `INFO.DAT` and `R.EXE`, is useful corroboration but not a vendor patch manifest. Its published `R.EXE` MD5 remains **UNVERIFIED** for this project because the present local file evidence does not include original `R.EXE` bytes.

Version nomenclature and changes should therefore be tracked as three different things: **original executable self-identification**, **external updater/patch label**, and **storefront package label**.

## 13. GOG installer history and the CINE13 boundary

[GOG's 2015 release history](https://www.gogdb.org/product/1432905053/releasenotes) explicitly identifies an English **1.2 → 1.2a** transition on **2015-11-03**, with German **1.2** remaining separate at that time. An archived [contemporaneous download-change log](https://www.gog.com/forum/general/the_what_did_just_update_thread/post10833) records distinct installer revisions for Windows, Mac and Linux. These are **GOG wrapper/distribution revisions** and do not establish that each platform contains a different 1994 DOS executable.

GOG's fix note uses the string `cine13.anm`. The current source inventory and Chapter 16 establish only `CINE00..CINE12`; matching 1.2a executables likewise cannot establish exactly which call/site was altered between 1.2 and 1.2a without an original 1.2 CODE image. Preserve this as a **patch-reason/alternate-executable comparison target**, not evidence of a missing source archive file that should be fabricated.

A community [2015 patch discussion](https://www.gog.com/forum/dungeons_dragons_ravenloft_series/strahds_possession_need_help_with_bugsglitches/post10) reported changing `game.gog` as well as installed CODE/configuration/launcher scripts. This is an account of a user's patch package, **not proof that GOG's official patch wrote exactly those files**. A future cross-distributor audit must independently hash the mounted CD content and installed resource trees.

## 14. Minimum acceptable cross-version comparison

A real 1.1/1.2/1.2a regression diff should compare:

1. Whole-file SHA-256 and lengths for `CODE.1`, `CODE.2`, `R.EXE`, `INFO.DAT` and each source resource; do not rely on user-facing version labels alone.
2. For CODE, isolate the shared DOS/4GW bound loader first and compare nested **LE object pages, relocations, import/name tables, and game-owned strings** independently; an installer rebuild of the extender is not necessarily a gameplay change.
3. For RES and ANM, compare exact index tables, entry hashes, wordcode grammar, active namespace indices and content rather than mere file timestamps.
4. For `INFO.DAT`, normalize only the **semantic location selectors and path** for comparison; retain the raw 17-byte source independently.
5. For mutable `SAVE.DAT` and `LEVnnn.DAT`, identify their save-state provenance separately from immutable CD authoring data. Never infer patch origin solely from their hashes.
6. For 1.2's reported `CINE13` failure, prove the actual filename constructor and callers from the **older executable** before assigning cause.
7. For cross-platform/regional builds, test encodings, drivers, offsets and serialization contracts from their own originals rather than importing DOS 1.2a identities.

**Completion boundary:** the present supplied-build provenance can be certified, but the patch delta matrix is **OPEN** until source patches or older edition binaries are acquired and independently hashed. No amount of static interpretation of 1.2a alone can prove which exact bytes changed in 1.2.


## 15. Final static provenance audit — executable and installation evidence

**Source-level decision: CLOSED TO SUPPLIED ORIGINALS; alternate media/patch delta reconstruction remains OPEN.** The 33-entry RES/ANM/support manifest in §10 is copied from the independently certified original resource audit; the per-file DATA and CODE identifiers in §9 are compared against **currently supplied original byte files**. These two evidence populations must not be confused.

A fresh local verification of `CODE.1`, `CODE.2`, `INFO.DAT`, `LEVELS.DAT`, `LEV022.DAT`, `OBJECTS.DAT`, `NPCS.DAT`, `SAVE.DAT`, and `SINTAB` produced **15/15 successful tests**:

- nine independent **file-size and SHA-256** identities;
- exact **INFO.DAT** physical selector/path bytes and length;
- `CODE.2` literal **`Ver 1.2a`** at file offset `0x7E1E4`;
- OBJECTS count `336` × record length `0x41`, NPCS count `83` × `0x45`;
- LEVELS count `32` and first record offset `0x84`;
- SAVE tick/time samples `1899`, `0x000E0000` and `1735`.

The locally provided **SINTAB** SHA-256 matches the corresponding independently certified row of the resources archive. The other 32 source resources in §10 were audited by the earlier `Resources/FINAL_RESOURCE_AUDIT.md` investigation, **not re-present in this workspace's mounted original files**. The CODE.1/CODE.2 MD5 matches and embedded version string establish 1.2a executable provenance; the original R.EXE remains represented here by its certified separate binary-audit report.

## 16. Earlier contemporary patch attestation and chronology correction

A publicly preserved contemporary [patch listing dated **March 3, 1995**](https://groups.google.com/g/comp.sys.ibm.pc.games.adventure/c/fDBFpSLIaus) names **all four** `rlftcd11.zip`, `rlftcd12.zip`, `rlftdk11.zip`, and `rlftdk12.zip`; the previously cited [May 9, 1995 list](https://groups.google.com/g/comp.sys.ibm.pc.games.adventure/c/c8ZPgcpO8Tw) corroborates the same entries, as does an [August 1995 follow-up](https://groups.google.com/g/comp.sys.ibm.pc.games.adventure/c/_miPm_DHjAg).

**Correction P-19.2:** the May 1995 listing is **not the earliest established surviving contemporary attestation** in the presently checked sources. The four patch filenames are documented **by March 3, 1995**. That does **not** date the creation/release of each ZIP, nor verify its binary contents. The separate 1995 archive index gives approximate rounded sizes; present-day archival indexes may give precise stored byte counts, but neither constitutes a recovered, authenticated patch archive.

## 17. Preservation sign-off and cross-version gate

| Question | Status |
|---|---|
| Supplied executable pair byte identity and `Ver 1.2a` | **VERIFIED** |
| Resource/core-DAT archive identities and 1.2a installation selectors | **VERIFIED for supplied original files; manifest verified against prior certified corpus** |
| Historical existence of CD/diskette version 1.1 and 1.2 patch names | **CONTEMPORARY DOCUMENTARY EVIDENCE** |
| One-to-one mapping of 1994 archive timestamps onto retail code versions | **UNPROVED** |
| Exact 1.0/1.1/1.2/1.2a code/object/resource deltas | **UNPROVED without older originals or authenticated patches** |
| `CINE13.ANM` report's exact failing older call and repair | **UNPROVED; do not invent a resource** |
| Original source compiler/linker command lines, DreamForge editor version | **UNKNOWN** |
| FM Towns/PC-98 and German-release byte/format equivalence | **UNPROVED** |

Chapter 19 is therefore **documentation-complete to the currently available and independently verified evidence**, with precise external-comparison gates retained. Future discoveries must extend the per-file manifest, record how new evidence was acquired and hashed, and amend the claims rather than revising source bytes or inferring gameplay changes from filenames alone.

**Next:** Chapter 20 — stable IDs, serialized record layouts, cross-resource pointer namespaces, validators, preservation editor rules and implementation compatibility contracts. The complete archive still requires its separate final contradiction/UNKNOWN audit.
