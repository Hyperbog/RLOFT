# Controlling native SINTAB raster interpreter milestone — original source and bounded VGA output verified

**Date:** 2026-10-11 Eastern / 2026-10-12 UTC.
**Exact compiled native source:** `a3bc4e24fb18d0c7988dbe1ea261276def6df42d`.
**Linux source-only build:** [GitHub Actions #38194123968](https://github.com/Hyperbog/RLOFT/actions/runs/38194123968), **21/21 CTests PASS** (includes `native_sintab_raster_tests`).
**Independent research branch source:** `0a7290fcf2832dfbf8693874b7e3663d1a4da908`.
**Research source-only CI:** [#38193878447](https://github.com/Hyperbog/RLOFT/actions/runs/38193878447), **159/159 Python tests PASS** (39 ResearchInfra + 120 SharedDecomp).

## Authentic original files and independent C++/Python agreement

The owner's `RES+OTHER.zip` and other original uploads provide the exact original `SINTAB` and `RES3`, used only locally:

```text
SINTAB SHA-256 19b6bc15f1be616bb8ff49dc9029995a1a0413fae06addc9fd41ddef761d0e87
RES3   SHA-256 44955ebbed0a8ed8d874fae07b01710317044882ab3c7c95d9be54e883df9a36
```

Exact compiled `rloft_native_sintab_raster_audit`, taken from the **successful source-only CI artifact**, was executed locally against both original files. The independently written Python source decoder, `rloft_sintab_original_reference.py`, decoded the same files *without taking expectations from the C++ report*. All **127 SINTAB scale programs' ten typed fields per block** and twelve aggregate fields matched: **1,282/1,282 values, zero differences**.

| Verified source property | Value |
|---|---:|
| SINTAB original scale blocks | 127 |
| Original scale payload bytes | 324,989 |
| Generated x86 bytes decoded as source | 259,457 |
| Read opcodes in original code | 11,587 |
| VGA destination-store opcodes | 23,910 |
| RES3 EH7 original referenced descriptors | 278,085 |
| All source descriptor-to-scale plans (scales 0,63,126) | 834,255 |
| Those with nonempty code span | 478,999 |
| Original selector-pair variants | 7,854 |

The independent reference hashes all 127 original blocks/code bodies and proves that every patch target is a complete decoded instruction boundary; source loads are within original 128-byte columns, and destination stores are 80-byte VGA row aligned. This does not require executing DOS x86 machine code.

### Original-data bounded interpreter test

A subsequent audit revision compiled at **`a3bc4e24`** executes an explicit, isolated diagnostic span, using original EH7 **shape 0**, first column, first descriptor, SINTAB **scale 126** and its actual source-adjustment/selector data. The testing call explicitly supplies **ECX=0, VGA destination byte=0, sequencer mask=15**. These are **diagnostic** operands, not asserted to be the original live game's register context or camera.

The real original span's selectors are entry **142** and stop **144**, with signed adjustment **15**. It performs exactly **one** original-code-derived source read and **four** isolated indexed VGA stores. The C++ safe interpreter's full four-plane output SHA-256 exactly matches a separate original-byte Python interpreter's output:

```text
e0b7360b0fd1615974122a5a451993b248978affed73333c000bf394ec613ddf
```

The host implementation decodes and handles the original opcode patterns `8A86`, `88C4`, `29C8`, `8887`, `88A7`, and `C3`; validates selector-to-code locations; enforces source and VGA-plane bounds; honors sequencer masks; and commits a raster span's writes **transactionally** only after its entire execution is valid. Tests detect corruption in SINTAB microcode, patch boundaries, source data, destination masks and incomplete spans.

The result of the locally executed C++-against-original and independent Python comparison is the metadata-only `rloft_original_SINTAB_127_native_safe_raster_verified.json`; the full source audit and native output are `rloft_original_sintab_safe_raster_reference.json` and `rloft_native_sintab_original_span_a3bc4e24.json`. The asset-free, compiled Linux audit binaries are in `RLOFT-native-SINTAB-EH7-original-sample-a3bc4e24.zip`. No original source game payload is in any report, archive, Git commit or GitHub Actions cache.

### Completion boundary

**VERIFIED:** Original RES3 4,234/4,234 type-7 source decode (earlier gate), all SINTAB 127 original code blocks' opcode grammar and offsets, 834,255 source selector plans, C++/Python block and plan parity, and **one real-original span** interpreted safely into deterministic four-plane output.

**NOT VERIFIED:** Full original `36B14`/`369D8` grouped scene calls, actual camera orientation/depth/occlusion, original ECX shade or source pointer call context, patch-jump modes, selected source animation/perspective frames, full EH7 compositor draw order, actual palette/EH6 texture compositing, DOS runtime pixel/frame agreement, live 3D gameplay, NPC/event VM, original SAVE/LEV persistence and current-source Windows/MSVC GUI build.

**Next engineering priority:** integrate CODE.2-authenticated scene traversal and original frame/scale selection into the safe native raster interpreter, including grouped/individual VGA mask dispatch and original texture/palette selection. Then compare complete native frames against captured DOS reference frames. Do not re-label an isolated four-store raster sample as finished game graphics. Windows release workflow remains manual-only; the independent NativeEngine must never link DemoV1 gameplay code.
