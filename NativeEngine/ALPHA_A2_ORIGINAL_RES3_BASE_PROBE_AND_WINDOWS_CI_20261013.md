# Alpha A2 — Original RES3 EH7 base-shape evidence and first verified Windows acceptance

**Project:** `Hyperbog/RLOFT`; **branch:** `native-engine`  
**Integration checkpoint prior to this document:** `8d068583f6efc99be4ccdebf0293fba8f7738c46`  
**Date:** 2026-10-13  
**Gate:** Source-backed geometry inspection validated with real original assets in Windows CI at the earlier exact code SHA; still **Pre-Alpha**, not live sprite-frame selection.

## Native RES3/EH7 base-shape integration

Original source frame range implementation (`NativeEngine/src/native_scene_frame_ranges.cpp`) now has `probe_native_authored_base_eh7`:

- Accepts a source-authenticated 11-byte record's legal original `NativeAuthoredFrameRange`, its INIT-derived bank map and a real `NativeResArchive`.
- Requires the archive cardinality to agree with the original source banks and uses `select_native_authored_res3_frame(range,banks,0,0)`. This is the **authenticated base resource**, **not** a live camera/animation frame.
- Decodes the actual compressed EH type-7 source through the independent native LZSS+EH7 decoder, retaining the original compressed/decoded SHA-256 fingerprints, width, clipped x/y bounds, source raster descriptor count and decoded-body length. Rejects invalid classes, resource range and malformed shape data. No x86 bytes are executed.
- The ordinary CODE.1-generated party GPU session now loads original `RES3` once, retains the complete authored source frame-range directory, and lazily decodes only an inspected original RES3 base (cached by ID).
- `F` near a discovered source record displays original record identity, authenticated base RES3 ID, original EH7 geometry, descriptor count and legal slot count. The evidence HUD is legible on a dark-backed overlay above the controls. `F5` retains the separate source-position diagnostic map.
- The party runtime still **does not select actual frame variants**, project the EH7 shapes into the 3D camera, draw actual NPC pixels, animate or interact. It does not treat base shape as necessarily the live frame.

## Original level 22 Assassin source

Independently validated from owner-supplied original binaries:
- `LEVELS.DAT` SHA-256: `50d0b1d13f81733a1ac55296d00fcba9aef002202bc00341f1062c37fd017d91`.
- Only original Level 22 class-A NPC is **type 9 ASSASSIN**, compact bytes `09000d00007c0700007802`, fixed-point cell position **(13.484375, 7.46875)**, facing low bits 2.
- Original `NPCS.DAT` has 83 records (four-byte record-count header, 0x45 bytes each). The original type-9 record maps to **RES3 315**, with animation stride **8**, cycle divisor **4**, animation span **3**.
- Original `Resources/RES/TYPE7_NPC_INDEX.md` reports the type-9 EH7 base shape at `RES3:315`: 64w, x 15..51, y 27..128. The real-source startup auditor now decodes and verifies this exact original EH7 geometry and checks the definitions remain 8/4/3, plus all other unique Level 22 authored base shapes.
- Family-B source placement types remain **62×52**, **63×42**, **64×16**. There are 0 class-C loose placements, 0 kind-1 controls and 50 empty map-local event slots. Executable-driven actor behavior still needs CODE.2 reconstruction.

## Actual verification evidence

**Windows GitHub Actions PR #16 / run #78**:
- Run ID `38442392623`, source QA head `4d9c9f48a83abf800b09977e444c75196306d9f8` (based on native HEAD `029806206d902bfa0760e2df45470e699206c16f`).
- Windows 2022 x64 MSVC build succeeded.
- **40/40 native CTests passed; 0 failed**.
- Real original-owner CODE.1/CODE.2 startup and `LEVELS.DAT`/RES3/NPCS/OBJECTS audits passed against immutable SHA-protected source.
- CINE00/CINE12 source checks succeeded.
- CI produced media-free Windows package `RLOFT-Native-A1-Original-Startup-Windows-x64` (GitHub artifact ID `11837940388`, run URL `https://github.com/Hyperbog/RLOFT/actions/runs/38442392623`). The artifact contains no original licensed media.
- The PR's additional source-hash regression passed and was merged as `8d068583f6efc99be4ccdebf0293fba8f7738c46`.

**Updated-source acceptance — Windows GitHub Actions PR #17 / run #83:**
- Run ID `38443286600`, QA head `d31bd80d493b1832ec5e9df99fca236e8348af85` based on `native-engine` commit `8d068583f6efc99be4ccdebf0293fba8f7738c46`.
- **Completed SUCCESS** on Windows 2022 x64 MSVC. Job logs explicitly show **40/40 native CTests PASS, zero failures** and the original-media startup/equipment/Level22 source check succeeded, including the new `original_first_level_assassin_res3_315_base_verified` gate. The original CINE00/CINE12 checks succeeded as well.
- Authenticated media-free artifact `RLOFT-Native-A1-Original-Startup-Windows-x64`, ID `11837511757`, from `https://github.com/Hyperbog/RLOFT/actions/runs/38443286600/artifacts/11837511757`.
- Regression asserting EH7 x_max is less than its original source width was merged from PR #17 as `c6c000e2cda8854be0d0b7188832723818e507a8`.
- These source acceptance results cover the implemented baseline and Assassin audit through the exact PR head. Later documentation-only commits and merge SHAs are not independently new binary acceptance runs. Do **not** confuse accepted source with an exercised Windows GPU field playthrough.

**Not verified by either CI:** actual interactive native Windows startup through every GUI page, installed Windows GPU driver, original DOS 3D view parity, source runtime sprite frame selection, real Level22 NPC9 encounter/AI, inventory, combat, sound, or native profile save/reload.

## Next

1. The original-data Windows CI #83 is confirmed green (40/40). Next obtain actual owner GPU screenshots and an interactive normal launch → character creation → Level22 field run; use those findings to fix runtime defects.
2. Reconstruct live CODE.2 world-record `+0x14` selector and the view/animation subframe logic for `2B2F4`, NPC `2B77A..2BA4F`, and class-one `2BBA0..2BDF0` before live sprite draw. The existing `NativeCode2StagedNonNpc`, `NativeType7VgaDispatch` and safe SINTAB interpreter are source building blocks, not an excuse to assign guessed frames.
3. Implement real 3D NPC/placed entity rendering, first Level 22 scene/Assassin event VM, actor interaction/inventory, and separate profile save/load. Keep original files read-only. **No Level22→23 priority until a continuous normal-launch playable Alpha passes.**

**Product classification: Pre-Alpha.** An accepted native MSVC test build is not playable Alpha.
