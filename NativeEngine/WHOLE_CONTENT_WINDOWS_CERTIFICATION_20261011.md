# Independent native engine — whole original content baseline

**Date:** 2026-10-11. **Product:** independent native replacement-engine architecture (OpenMW/KEX-style), **not** the `demo-v1` decompilation/recompilation.

## Major milestone achieved

The native engine has moved from the single-EH1-image inspector to a **typed, complete original content-audit system**. Original copyrighted files are loaded READ-ONLY, never bundled into the executable or user ZIP. Both the active original RES banks and the two preserved legacy banks are handled.

**[Actual Windows full-content build and real-original-data certification #38174139897](https://github.com/Hyperbog/RLOFT/actions/runs/38174139897)** — **SUCCESS**. Exact **compiled** native Git revision:

`44cc2ccc950aa686d6563b9eda6cd804b1b08cf8`

- Windows Server 2022 / VS2022 x64 MSVC Release compiled **all three standalone tools**.
- **16/16 Windows CTests passed** (including dedicated native whole-bank and world/SINTAB regression suites).
- The CI workflow checked out ONLY the owned game media from `DnD-Classics` as a temporary private sparse-checkout test fixture. It did **NOT** incorporate original data into source, build binaries, or packaged artifact.
- Ran `rloft_native_bank_audit.exe` against the actual 13 RES files in `MASTER/Ravenloft - Strahd's Possession/CD_Extracted/RLOFT`.
- Ran `rloft_native_world_audit.exe` against the **actual** `LEVELS.DAT` and `SINTAB` in the same owned media.
- Both separate fail-closed Python corpus-certification steps accepted the exact reconstructed source counts and native parser output.
- User ZIP uploaded as [Actions artifact #11714947352](https://github.com/Hyperbog/RLOFT/actions/runs/38174139897/artifacts/11714947352), downloaded and **independently checksum-verified**.

### Whole-original RES corpus: 12,562 / 12,562

| Decoded/validated record family | Supplied count | Result |
|---|---:|---|
| Original RES archive files | 13 | **All passed** |
| All RES records | **12,562** | **All passed** |
| EH-typed records | 12,464 | **All recognized** |
| EH1 original indexed raster records | 1,591 | All accounted for: 1,586 drawable + 5 inert legacy records |
| EH2 fonts | 15 | Strict glyph envelope/raster decode |
| EH3 XMIDI/XDIR music containers | 172 | Payload size/signature checked |
| EH4 Creative VOC audio containers | 1,005 | Container/signature checked |
| EH6 first-person 32x128 / 64x128 textures | 1,319 | LZSS + dimensions/column layout verified |
| EH7 compiled geometry/display lists | **8,362** | LZSS + geometry/column tables/selector lists verified |
| Non-EH serialized script records | 75 | 38 active + 37 legacy |
| Inactive raw RES1 sentinel records | 2 | Preserved |
| Zero-length RES1 slots | 21 | Preserved |
| Historical EH4 VOC length anomalies | 2 | **Preserved, not repaired** |

Original RES12:74,153,154,156,157 contain **genuine inactive zero-pixel EH1 descriptors**. The native validator now classifies them explicitly as `InertLegacyRaster`, rather than attempting to render them or inventing missing image pixels. None of the 11 active banks has an invalid resource. The independently reconstructed common 4KiB LZSS parser validates **exact source stream consumption** for EH1/EH2/EH6/EH7.

The new `NativeEngine/src/native_bank_system.cpp` owns complete record dispatch and metadata reports; every reported failure identifies archive and original record ID. It **does not** execute original DOS game code or SINTAB x86 microcode. A separate original-format verifier `tools/verify_whole_res_banks.py` rejects any mismatch in total, family distribution, original anomalies, or missing source banks.

### Whole original world and SINTAB — both passed

| Source-backed world/renderer data | Original count | Result |
|---|---:|---|
| LEVELS.DAT archive records | 32 | **All decoded and section-validated** |
| Packed map cells | 115,584 | **All structurally accounted for** |
| Serialized 10-byte controls | 1,659 | **Validated** |
| Physical event slots | 1,600 | 394 active |
| Active event script commands | 555 | 0..12 CODE.2 opcode family structurally checked |
| SINTAB original scale blocks | 127 | **All validated** |
| Generated scale-code payload bytes | 324,989 | **Accounted for** |
| SINTAB per-scale selector/step entries | 129 + 129 | Checked in all 127 blocks |

`src/native_world_system.cpp` independently validates `LEVELS.DAT` archive/record offsets, dimensions, all packed cell words, 10-byte control lists, three compact world/actor families, every 26-byte event slot, automap tail and annotation span. It also consumes all SINTAB fixed prefix tables, scale block lengths, monotonic offset tables, 129 scale-step words per block, x86 code bounds/final RET and 512-byte original zero trailer. The SINTAB x86 code is **data**, never executed. Native semantics/scene simulation still need independent implementation.

The CI verifier `tools/verify_original_world_system.py` checked all 32 records, 115,584 cells, 1,659 controls, 394 active slots/555 commands, 127 scale blocks and 324,989 generated code bytes against the supplied canonical source files.

## Compiled/verified release bytes

| Exact downloaded item | SHA-256 |
|---|---|
| GitHub outer workflow artifact ZIP | `cc16f44294e707a43a19c52def3d4a8fca8588c1ac7db8e6c13793a98580723b` |
| Inner user distribution ZIP | `78e702134b130424466f259be746d6f2653c451100950db0dca673126aacd30e` |
| `rloft_native.exe` (Windows GUI) | `6f6e76ec2cfc30d26cdad30926a240fd44186e8eeb357563be79ccc3a6b53506` |
| `rloft_native_bank_audit.exe` | `6db4b7177649c35a25a5b42ef2a617c7d36a4b7cd080367e654bd59a6f1d86d3` |
| `rloft_native_world_audit.exe` | `feca14ace7ed67ba462ba01ac1bcc8811145cbe866d4c7fe261ee2fda42934d5` |

All three executables have the expected **Windows AMD64 PE machine 0x8664** signature. The six-entry inner ZIP contains only these EXEs, a README, SHA256SUMS and source provenance. The manifest states `original_assets_included=false`, `full_extraction_required=false`, `gameplay_verified=false`, and stage `ALL_RES_BANKS_AND_WORLD_CONTENT`.

**First full-bank run:** discovered five original zero-size legacy RES12 records. Corrected the native distinction and preserved them as inert data. Two subsequent whole-bank corpus verifications were initially blocked by Python verifier formatting/key mistakes, not an original-file parse failure. The final full-content run confirmed all source banks **and** the native world/SINTAB systems.

## What this milestone DOES NOT claim

- **Not yet a playable native engine.** Validating 8,362 EH7 display-list records does NOT mean the new renderer draws all 8,362 faithfully. The old SINTAB instructions are deliberately NOT executed. The next engine milestone is an independent replacement **scene projector and rasterizer**, backed by this decoded geometry/scale dataset.
- **Not yet an active event VM**. The 555 original event commands have been structurally accepted; opcode semantics/story execution, dynamic world objects and NPC AI must be implemented as native runtime systems.
- **Not yet native audio/cinematic playback**. Music/SFX are recognized and structurally validated; mixing, timing, codecs, ANM playback and native device presentation are separate systems.
- Not a DOS recomp or a renamed decomp demo. `NativeEngine/CMakeLists.txt` still compiles no `../DemoV1` runtime implementation.
- Documentation commits after exact compiled SHA `44cc2ccc...` are not part of the already verified executables.

## Whole-system next engineering stage

Stop producing one resource preview per milestone. Integrate the certified **resource bank + world + SINTAB** datasets into a persistent native runtime with engine-owned mutable state and renderer-neutral scene snapshots: level cell geometry, decoded EH7 projected shapes, EH6 texturing/shading, native CODE.2 display-list interpretation without x86 execution, authoritative per-draw palette context, gameplay input/ticking/events, native audio/ANM sequences and isolated save-state persistence. Add end-to-end Windows source-data tests across a complete original level/scene rather than isolated EH record fixtures.

The previous user-verified GOG ISO reader, palette and EH1/EH6 asset previews remain accepted at their own historical compiled SHAs; this whole-content build additionally validated **the immutable canonical original Git branch** end-to-end without requiring more user screenshots.
