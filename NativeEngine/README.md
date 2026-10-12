# Ravenloft: Strahd's Possession — Independent Native Engine

## CONTROLLING verified native scene view/resource-header milestone — 288/288 original sections (2026-10-11)

[Evidence chapter](ACTUAL_SCENE_HEADER_288_AND_PROBE_20261011.md).
Exact compiled Linux SHA \`0850d312a76a52715eec459063a47b393396382d\`;
[Actions #38190025508](https://github.com/Hyperbog/RLOFT/actions/runs/38190025508),
**18/18 CTests PASS**. New independent NativeEngine now retains **all
676 original header bytes per map** (five 64-entry resource mapping
tables), resolves conservative original-backed resource candidates,
and has a bounded camera movement / 320-column raycast **diagnostic
host probe**. The latter uses original level topology and proven
normal collision but **is not the original DOS traversal, projection
or renderer**; its artificial RGB debug image is *not game artwork*.

The actual CI-built native executable was executed locally against
the owner's authentic \`LEVELS.DAT\`. Independent original byte parser
and actual native SHA-256 output match **288/288 map section digests**
(32 maps × complete header, cells, 10-byte controls, events, automap,
annotations and each A/B/C record family). New header-only source
SHA-256 matched **32/32**. This required a real-world malformed JSON
separator fix; the fixed compiled executable now emits valid JSON.
The canonical comparator is
\`code1-decompilation:SharedDecomp/verify_native_world_fingerprints.py\`,
which now requires the header digest too; [research CI
#38190191412](https://github.com/Hyperbog/RLOFT/actions/runs/38190191412)
passed **129/129** synthetic source-only tests.

**Next controlling work:** replace host DDA with original CODE.2-proven
orientation/traversal, bind validated RES3/EH7 and SINTAB microcode
semantics safely to the renderer, EH6 textures/VGA palettes, then
integrate animation, true party motion, NPC/event VM and SAVE
compatibility. No original media is committed; current Windows/MSVC
build and field/gameplay/visual parity remain unverified, and Windows
Actions stay manual-only. Keep independent \`demo-v1\` game runtime
unlinked.

## CONTROLLING native persistent-world milestone — actual original 32-level runtime verified on Linux

[Compiled source and full 32-level normal-collision evidence](ACTUAL_ORIGINAL_RUNTIME_32_LEVELS_VERIFIED_20261011.md).
At exact native source `20543d8fd3a6c61aad7c886d6d1699f1fbf07005`,
[Linux CI #38186427291](https://github.com/Hyperbog/RLOFT/actions/runs/38186427291)
passed **17/17 portable CTests**. New `NativeWorldRuntime` keeps 32 level
states, mutable 16-bit cell/automap and original ten-byte control records,
source-hash-bound snapshots, evidence-backed ordinary/dynamic collision,
kind-1 animation tick and bounded renderer-input marked-cell inventories.
The **actual compiled GitHub Linux runtime auditor** was independently
checked against the owner's original `LEVELS.DAT`, and exact source-level
control/normal-collision outcomes match all **115584 cells** (87983
normally passable, 26857 ordinary blocked, 744 special blocked; 745 special
centers, 324 kind-1 dynamic records). Python independently reproduced the
**full retained-world source SHA-256**
`a8232782ff8654f9b6c6651300b3fbb2235e58d2fadf61fe2221e11dbbc8bdcd`,
including cells, controls, A/B/C records, all event slots, automap and
annotations. Canonical `SharedDecomp/verify_native_world_runtime.py`
adds reusable cross-language gates, with [research CI
#38186651456](https://github.com/Hyperbog/RLOFT/actions/runs/38186651456)
**124/124 source-only tests passing**.

**Completion boundary:** fully decoded original world + native
state/collision/animation primitives YES; **not** proven player traversal,
original raycaster, graphics projection/occlusion, event VM, actor AI, save
compatibility, Windows/MSVC current build, or playable full game. The
Windows release workflow remains manual-only. Keep separate `demo-v1`
out of independent native runtime linkage. Next priority: first-person
ray traversal/RES3-SINTAB/EH6 renderer and the authenticated original
event/movement behavior, not additional generic JSON audit tools.

## CONTROLLING SOURCE-ONLY MILESTONE — actual NativeEngine byte round-trip VERIFIED on Linux (2026-10-11)

**Exact compiled C++ source:** `4676ebf50668480512861c9fd981d6e36402da82`.
**GitHub Actions Linux source-only run [#38185185120](https://github.com/Hyperbog/RLOFT/actions/runs/38185185120): 16/16 CTests PASS**, including the native bank and new world-payload tests. The full portable engine sources compile. A prior Linux SIGILL was traced to an EH2 font validator falling off a `bool` function; the explicit non-inert return repaired genuine undefined behavior, without disabling the test. The manually authorized Windows/MSVC workflow remains unchanged and **has NOT** built this newer revision.

The two GitHub-built Linux auditors from **that exact source** were downloaded and executed locally on the owner's real `LEVELS.DAT` and `SINTAB`, each pinned to its SHA-256. NativeEngine produced a complete 32-level world report (115,584 cells, 1,659 controls, 394 event slots / 555 commands, 127 SINTAB scale blocks), and its native typed-world payload round-trip returned `decoded_section_roundtrip_equal=true`. A separately implemented Python original-byte oracle independently verified **256/256 source section SHA-256 hashes**: all 32 levels × packed cells, controls, event slots, automap, annotations and the A/B/C compact families. Full original `LEVELS.DAT` SHA also matched between the independent reader and native executable. This supersedes the prior unbuilt source-only parity note for **world data preservation**.

The local isolated source directory included copies of the authentic `LEVELS.DAT` and `SINTAB`, plus **empty synthetic INIT, RES0 and T signature files solely to satisfy NativeMedia directory recognition**. This is *not* a validated playable original installation; no game input was uploaded to CI, shipped in the executable ZIP, or committed. The verification evidence is retained as the local user-facing report `rloft_native_original_sections_256_verified.json`; source and binary SHA/provenance details are recorded in [the milestone documentation](ACTUAL_ORIGINAL_WORLD_256_LINUX_VERIFIED_20261011.md).

**Project completion boundary:** Compiled Linux content decoder and byte-exact source preservation YES; current Windows/MSVC executable validation, original DOS runtime execution, 3D renderer/texture parity, AI/NPC/event VM and playable independent native game NO. The independent `demo-v1` gameplay implementation must never be linked into NativeEngine.

## New typed-world SHA-256 round-trip gate — source only, not yet a Windows build

[Full report contract and implementation](WORLD_FINGERPRINT_COMPARISON_20261011.md):
\`native_world_fingerprints\` now reconstructs the **full 32-level typed
world source sections**, verifies each decoded section byte-for-byte
against the user-owned original image, and emits **256 section SHA-256
hashes** across cells, controls, all event-slot bytes, automap,
annotations, and the three eleven-byte families. The new
\`rloft_native_payload_audit\` is a fourth internal C++ executable
configured in CMake, not yet a certified/packaged Windows binary.
Existing \`native_world_payload_tests\` now include three standard SHA-256
vectors and structural mutation rejection. **Current native-engine
HEAD has not been compiled or CTest-certified on Windows.**

The independent canonical \`SharedDecomp/verify_native_world_fingerprints.py\`
compares these native hashes against independent original-file parsing.
On **actual owner-supplied original bytes**, an isolated locally compiled
C++17 SHA implementation and independent Python \`hashlib\` agreed on
**385/385 hashes** (256 per-level sections, 127 SINTAB blocks, two full
original input files); four separate local byte-mutation controls were
all detected. This validates SHA/corpus evidence, **not** the exact
\`native-engine\` source compile or native gameplay. Canonical research CI
passed **106/106 source-only tests** including native-shaped synthetic
fingerprint comparison. Windows Actions remain manual-only, requiring
explicit owner authorization. Next is compiling the exact native source
and independently comparing the real \`rloft_native_payload_audit\` output,
then integrating verified world state into first-person simulation/rendering.

## Typed original-world payload continuation — SOURCE ONLY, UNBUILT

[Original-world payload retention](WORLD_PAYLOAD_RETENTION_20261011.md) at native
commit `19ada509fe03c5718a49cc6ce08b2154f5c233e2` adds
`native_world_payload`: all 32 original packed cell grids, column-owned
10-byte controls, complete 11-byte world families, full event-slot data and
automap/annotations, plus isolated mutable cell/automap copies. The standalone
`native_world_payload_tests` is registered as the **17th CTest**. This is a
real implementation step toward native simulation, not full gameplay or DOS
execution parity. **No build/CTest of this newer source has been run**;
the last certified MSVC artifact remains pinned to original
`44cc2ccc950aa686d6563b9eda6cd804b1b08cf8`. Do not trigger native
Windows Actions without explicit owner approval. Independent source-byte
fingerprints are available under canonical
`code1-decompilation/SharedDecomp/original_world_oracle.py`; native decoded
payload bytes are not yet compared against those hashes.

## Whole original RES + LEVELS + SINTAB content system: WINDOWS VERIFIED

[**Canonical whole-content audit / MSVC certification**](WHOLE_CONTENT_WINDOWS_CERTIFICATION_20261011.md) supersedes individual resource-preview checkpoints. Real Windows x64 MSVC [Actions run #38174139897](https://github.com/Hyperbog/RLOFT/actions/runs/38174139897) compiled exact native source `44cc2ccc950aa686d6563b9eda6cd804b1b08cf8`, passed **16/16 Windows CTests**, then successfully decoded/validated **12,562/12,562 real source RES0..RES12 records**, all known EH families including 8,362 compiled shape display lists, all **32 original world records / 115,584 packed cells / 1,659 controls / 555 active event commands**, and all **127 SINTAB renderer scale packages** from `DnD-Classics` owned original media. New independent C++ all-bank/world systems and their fail-closed native command-line auditors are distributed with the GUI, not the decomp demo. The six-file asset-free ZIP and all **three Win64 executables** were downloaded and SHA256 verified; no copyrighted original files were shipped. **This is not yet native gameplay, scene rasterizer parity, active event VM or audio/cinematic playback.** Next work must be integrated first-person level simulation and rendering, not more one-record previews.


## Original RES archive categories and native world textures — 14/14 MSVC verified

[**Archive type classifications and EH6 Windows build**](RES_ARCHIVE_TYPES_AND_EH6_WINDOWS_20261011.md): The user observed no graphics in RES3/4/5/6/10 because the earlier browser was **EH1-only**; the original shipped corpus shows RES3=4,234 EH7 compiled shapes, RES4=667 EH6 first-person textures, RES5=98 EH3 XMIDI, RES6=266 EH4 VOC sound effects, RES10=219 EH4 VOC voices. **These archives were not broken or missing.** RES9 also holds 517 EH1 graphics but was omitted from the selector. Independent native update now decodes **RES4 EH6 32×128/64×128 LZSS/column-major textures**, browses RES9 EH1 graphics, and explains why RES3 geometry/SINTAB and sound archives cannot be displayed as EH1 sprites. [Windows MSVC run #38172310617](https://github.com/Hyperbog/RLOFT/actions/runs/38172310617) compiled SHA `744036693e84619d136d69dab157e9de25a2086e`, passed **14/14 CTests**, and the asset-free Windows ZIP/PE x64 EXE were independently SHA-verified. Real GOG visual texture browsing is pending; this does **not** implement DOS per-draw texture shading or EH7 scene rendering. No DemoV1 linkage.


## Original palette routing is per callsite and game state — Windows verified 2026-10-11

[**Contextual palette source/build audit**](ORIGINAL_PALETTE_CONTEXT_ROUTING_20261011.md): user-confirmed mixed original palettes within individual RES archives are **correct game design**. Per-record research in original CODE.2 and the existing project [palette router](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/Tools/anm/palette_router.py) establishes specific RES0 IDs using banks 0–4, 10, 11, 12 and 13, plus original world-level palette state and genuinely unknown context. Native `src/native_palette_context.cpp` independently models original **exact callsite / level context / preview-only / unresolved** decisions and the original 32-level bank table. The Windows RES inspector now defaults to `AUTO (source context)` and shows evidence labels while retaining manual 0–14 bank overrides; it does **not** claim every record has one immutable bank. Actual Windows MSVC [run #38170601091](https://github.com/Hyperbog/RLOFT/actions/runs/38170601091) compiled SHA `b1892e8448734f3ad8018a0a3911fe9ed59d582c`, passed **13/13 CTests** and uploaded an independently verified asset-free x64 Windows ZIP. Real original-GOG visual confirmation of this new AUTO routing is still pending; eventual native gameplay must model mutable VGA palette at each scene/render operation. The decomp demo remains a separate untouched project.


## Native indexed sprite color context — VERIFIED Windows code, GOG visual test pending

[**Original palette correction report (2026-10-11)**](WINDOWS_PALETTE_CONTEXT_CORRECTION_20261011.md): the prior RES browser applied **COLORS bank 12 to every sprite**, even though original gameplay screens use different banks (CODE.1 character creation bank 8, CODE.2 menu bank 12, initial logos 14/9). It also omitted CODE.2's original **19-index title cursor translation**. New independent `rloft_native_assets` code distinguishes raw RES indices from the original title cursor and exposes a **15-bank chooser**. Actual Windows x64 MSVC [run #38169819411](https://github.com/Hyperbog/RLOFT/actions/runs/38169819411) compiled source `1c9440303a5c83ff6834a3f87cfdf9afc8dbfa19`, passed **12/12 CTests** including asymmetric RGB/DAC6→BGRA color-channel and cursor remap tests, and uploaded a verified asset-free ZIP. This new visual fix is **not yet user-verified against original GOG sprite colors**; other resources can require other original palette semantics. Do not apply a universal R/B swap or inversion, and do not modify the separate demo.


## Verified independent native RES image browser — Windows 2026-10-11

**[MSVC / 11-of-11 Windows CTest acceptance](WINDOWS_NATIVE_RES_BROWSER_VERIFIED_20261011.md)**: the independent `rloft_native.exe` now has its own native EH1 raw/LZSS decoder, original INIT/RES0 cursor resolver, and **RES0–RES8/RES10 numeric image browser** with previous/next image navigation. Windows Actions [run #38168898735](https://github.com/Hyperbog/RLOFT/actions/runs/38168898735) compiled this specific source SHA `d29914503fb6d6bb3a65954257d0ac145f0ceaf0`, passed **11/11 CTests**, uploaded an asset-free ZIP, and its actual EXE/ZIP checksums and Windows AMD64 PE format were independently verified. Download [the archived Windows artifact](https://github.com/Hyperbog/RLOFT/actions/runs/38168898735/artifacts/11712864857). **The new raster browser is NOT YET USER-TESTED against the original GOG installation**, although earlier real original file indexing (19/19, 69 files, COLORS palette12) is verified in its own previous build. The decomp demo remains entirely separate; no original assets are distributed or extracted in bulk. Later documentation-only HEAD revisions are not this compiled source.


## First real GOG install verified; independent raster decoding underway

[**User's original GOG field test**](WINDOWS_REAL_GOG_ASSET_FIELD_TEST_20261011.md): the previously compiled independent native asset loader opened the actual original GOG `game.gog` as **ISO2048**, indexed **69 source files**, reported **19/19 recognized resources validated, zero missing/invalid**, and displayed the **original COLORS palette 12**. This is real installation-backed **structural validation**, not proof of full sprite/cinematic/world/gameplay decoding.

[**Native EH1 raster milestone**](NATIVE_EH1_RASTER_MILESTONE_20261011.md): the next independent source implementation now decodes raw and original LZSS-4K compressed RES EH type-1 images, resolves the original default cursor from INIT/RES0, displays a native GDI original-sprite preview when available, and adds `--verify-raster` with separate failure codes. A new native corruption/LZSS/selection test brings Windows **required registered CTests from 9 to 10**. **These latest source changes have not been MSVC compiled or tested against the owned GOG installation; the prior 9/9 verified build corresponds only to `fd314b89`.** No decomp demo code is linked or original assets distributed.


## VERIFIED Windows x64 independent native asset build — October 11

**[Download and verification evidence](WINDOWS_NATIVE_ASSETS_VERIFIED_BUILD_20261011.md):** `rloft_native.exe` was compiled on Windows Server 2022 using **MSVC 19.44**, all **9/9 Windows CTests passed**, and the asset-free ZIP was downloaded and independently checked. [Actual GitHub build #38164976411](https://github.com/Hyperbog/RLOFT/actions/runs/38164976411). **Exact compiled Git revision:** `fd314b89e96db07c739176d25f1fc9be94ecaa19` (later README/docs HEADs are not compiled revisions). Native engine is not the demo recomp, and this build does **not** implement gameplay. First **real GOG asset-loader field test remains pending**, although an older diagnostic bootstrap did display real GOG menu artwork. Primary output: `RLOFT-NativeEngine-Windows-x64-ASSET-PREVIEW.zip` (no original files). ZIP SHA-256: `5ee2cdb58f3aa0ea10c715a2841b6be3eb0f411abb957132c4aa1150a857713c`.


**Authoritative direction (October 11, 2026):** [Native asset runtime architecture](ASSET_RUNTIME_ARCHITECTURE_20261011.md).

This is a NEW Windows replacement game engine, conceptually like OpenMW/KEX. It reads the player's **original game assets** and will eventually implement its own graphics, audio, game-state, world and gameplay systems. It is **not** the original-source decompilation/recompilation and does **not** link or execute the `demo-v1` game runtime. The working decomp demo continues as an independent project and can supply research/evidence about original formats and behavior.

## Active implementation — asset loading

- Native, read-only original media backend [`native_media.hpp`](include/rloft_native/native_media.hpp), [`native_media.cpp`](src/native_media.cpp): index DOS loose files or GOG `game.gog` ISO9660 (2048-byte data sectors, 2352-byte raw sectors with 16/24 byte user offsets), including nested asset folders, and **read original resources lazily** without unpacking or rewriting the disc image.
- Independent original format decoders [`native_assets.hpp`](include/rloft_native/native_assets.hpp), [`native_assets.cpp`](src/native_assets.cpp): RES offset tables/entry lookup and original `COLORS` 6-bit DAC palette banks. Report the presence, byte size, archive entry count and structural validity of 19 key assets.
- Windows desktop entry [`win32_native_assets.cpp`](src/win32_native_assets.cpp): **independent `rloft_native.exe`**, original-install folder picker, read-only inventory and original COLORS palette12 display. Optional headless `--verify-assets <owned install>` for actual original-media validation.
- Existing native, renderer-neutral indexed-frame, VFS mod mount/precedence and proof-contract foundations remain independent of the original runtime. No original game media or private saves are included in distributed outputs.
- **No full unpack/repack is required at runtime.** A disposable, versioned derived-resource cache can be evaluated later if profiling justifies it. Do not repack the game.gog or change installed originals.

### Windows-first build

The next **manual-only** Windows build is [Native Engine Original Assets — Windows x64](https://github.com/Hyperbog/RLOFT/actions/workflows/native-engine-assets-windows.yml). It runs [`Build-NativeAssets-Windows.ps1`](Windows/Build-NativeAssets-Windows.ps1), builds the independent native reader, runs at least **nine** isolated native CTests (loose files, ISO sector variants, nested original directories, RES archive/COLORS decoding, native VFS/renderer contracts), and prepares an asset-free source-identified ZIP.

**Status:** this current independent asset engine source has been **committed but NOT yet compiled or executed under Windows MSVC**, and its real GOG asset validation is **NOT yet field-tested**. It must be built and checked against the user's original `game.gog` before acceptance. No Windows CI run has been dispatched for this revision. The previous Windows bootstrap's genuine **15/15 CTests** and the user's screenshot of its original static menu are real historical results, but that older bootstrap linked inherited decomp codecs and is **not proof that this new native engine loads assets**.

### Intended acceptance sequence

1. Compile and test the *independent* Windows `rloft_native.exe` on MSVC x64.
2. Open the user's original GOG/DOS install; validate actual `RES0`–`RES10`, `INIT`, `COLORS`, DAT and ANM assets and inspect original palette output. Keep the install read-only.
3. Add native, independently implemented RES EH sprite/texture/geometry, ANM cinematic, music/SFX and DAT world decoding; build a typed asset manager with resource-ID/version provenance and optional derived-data cache.
4. Implement original-source-faithful game semantics in **new** simulation code, original boot/world/input/save flows, and Windows rendering, comparing against evidence from DOS and the separate decomp demo without using either as a runtime dependency.
5. Establish gameplay fidelity/normal-world acceptance; then add renderer, modding and editor capabilities without changing original rules.

## Continuity and archival evidence

- [Current design and decision](ASSET_RUNTIME_ARCHITECTURE_20261011.md)
- [Native-engine developer instructions](AGENTS.md)
- [Windows native bootstrap build 15/15 (historical)](WINDOWS_VERIFIED_BUILD_20261011.md)
- [Real GOG installation static artwork test (historical bootstrap)](WINDOWS_GOG_FIELD_TEST_20261011.md)
- [Original CODE.1/CODE.2 decomp continuity](../Code1/DECOMPILATION_CONTINUITY.md) — **reference evidence, not native production dependency**
- [Native engine playable/runtime objective — Issue #9](https://github.com/Hyperbog/RLOFT/issues/9)
- [Native GPU comparison — Issue #10](https://github.com/Hyperbog/RLOFT/issues/10)

**Superseded:** `DEMO_BOOT_PIPELINE_REUSE_20261011.md` and `LIVE_ORIGINAL_RUNTIME_INTEGRATION_20261011.md` document an abandoned recompilation-based native integration and MUST NOT guide current implementation.
