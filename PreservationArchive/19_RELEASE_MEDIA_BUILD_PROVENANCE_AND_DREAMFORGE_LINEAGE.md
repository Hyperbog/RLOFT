# Ravenloft: Strahd's Possession — Preservation Engineering Archive

## 19 — Release, Media, Build Provenance, and DreamForge Lineage

**Status:** FOUNDATION — supplied executable revision identified; historical media/build equivalence deliberately unresolved  
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
MD5         e376e8e0e8918ff1dfbf8e4a6f55
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

