# Whole-image original DOS/4GW LE reconstruction — 2026-10-11

Canonical owner: `code1-decompilation:Recomp/`. This loader is not a native-game engine, simulator, or DOS emulator.

## Authenticated original evidence

The supplied original `CODE.1` is 277,787 bytes with SHA-256 `fd9b709ceee190e6a70870f9012ebea3bc437cae6bc74e0150d6f922fa750fed`. Original `CODE.2` is 558,689 bytes with SHA-256 `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`. Both local original files were hash-checked independently before testing. No original file is committed or distributed by this target.

| Property | CODE.1 | CODE.2 |
| --- | ---: | ---: |
| nested MZ module physical start | 0x25D24 | 0x25D24 |
| LE header physical start | 0x27BDC | 0x27BDC |
| total mapped pages | 24 | 78 |
| original object-1 code bytes | 70,402 | 239,698 |
| original object-2 virtual data bytes | 40,592 | 187,664 |
| original object-2 backed data bytes | 22,007 | 75,581 |
| object-2 implicit zero-filled bytes | 18,585 | 112,083 |
| 32-bit internal target relocations | 2,617 | 9,813 |
| signed backward-page fixups | 1 | 6 |
| entry object/offset | 1 / 0xCC1C | 1 / 0x353A0 |

Source: original nested application MZ and LE header, object page map, fixup page table, fixup records and complete original page data. Correct origin and object geometry independently match `Runtime/CODE_OBJECT_MAP.md`.

## Implemented executable loading

`src/original_le_image.cpp` parses each entire original LE image from its bound application module, materializes all four full virtual code/data objects (not slices), and zero-fills genuine file-unbacked BSS. For each object it preserves both `raw` (original DGROUP-relative operands for reconstructed source) and `relocated` (32-bit object relocation base plus target offset) representations. This distinction prevents corrupting the address model of the separately compiled canonical C++ functions.

All original fixup records are type `0x07` with flags `0x00` or `0x10`. Both modules were scanned exhaustively: 12,430/12,430 records are handled, source operand bytes agree with encoded targets, target offsets remain inside their target objects, and signed `-1/-2/-3` page-boundary records are resolved into the prior page. No unverified import, ordinal, compression, or alternate relocation semantics were silently substituted. Unsupported forms fail closed.

## Executed validation and build wiring

Local GCC C++17 `-Wall -Wextra -Wpedantic -Werror` compiled and executed `original_le_image.cpp` plus `original_image_gate.cpp`. Synthetic tests passed full object recovery, preserved raw bytes, relocations crossing a 4 KiB page and deliberate header/page/fixup corruption rejection. Supplying the two original game executables validated 2,617 CODE.1 plus 9,813 CODE.2 original fixups and both complete memory layouts. This is a local loader test, not a Windows Full acceptance claim.

The existing CMake project adds `rloft_original_image_gate` and a registered standalone CTest independent of copyrighted originals. It also links `original_le_image.cpp` into `rloft_recomp_link_gate`. Passing the exact original files as arguments to either executable enables opt-in original-file verification. The canonical recompilation link gate additionally invokes compiled original CODE.1/CODE.2 memory-getter functions against loaded, original DGROUP data objects; its default source-only invocation does not pretend this original-data test ran when files were absent. Windows CI must independently compile/execute the new revision before being counted as green.

## Remaining whole-game startup frontier

The LE loader does **not** execute DOS4GW code, establish selectors/interrupt services, initialize external runtime services, or execute the original CODE.2 startup continuation at `0x35418`. The existing `0x353A0` entry stub's callback observation remains explicitly limited to its tested boundary. Authentic object initialization/relocation is a prerequisite to real boot, not proof of boot or a faithful game runtime.

Next work: identify the source-backed service interface required at `35418`; construct its startup object/state in the canonical reconstruction with real loaded LE data, add DOS/DPMI boundary implementations verified against immutable shipped bytes, and run the first entry-to-startup execution gate. Do not transplant `demo-v1` or `native-engine` runtime implementation as a substitute.
