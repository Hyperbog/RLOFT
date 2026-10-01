# CODE.1 faithful decompilation progress

![Progress](https://img.shields.io/badge/verified%20conversion-0%25-red)

**Verified conversion:** 0%  
**Remaining:** 100%  
**Status color:** RED until verified conversion reaches exactly 100%; GREEN only at 100%.

## Measurement rule

The percentage measures instruction-accountable C++ conversion of the protected-mode application code recovered from the original `CODE.1`. Executable mapping, disassembly, research, scaffolding, speculative pseudocode, and unverified translations do **not** count as converted code.

A region counts only after its original byte range and decoded x86 instructions are established and the corresponding C++ accounts for their observable behavior. If evidence is insufficient, the item remains unresolved instead of being guessed.

## Immutable source identity

- File: `CODE.1`
- Size: `277,787` bytes (`0x43D1B`)
- SHA-256: `fd9b709ceee190e6a70870f9012ebea3bc437cae6bc74e0150d6f922fa750fed`

## Verified executable mapping

- Outer DOS MZ image begins at file offset `0x00000000`.
- Bound Rational/Tenberry `BW` image begins at `0x0000F444`; embedded name is `4GWPRO.EXP`.
- The protected-mode application's embedded MZ begins at `0x00025D24`.
- Its LE header begins at `0x00027BDC`.
- LE target CPU field is `2` (80386); the module has 24 pages and 2 objects.
- LE data pages begin at absolute CODE.1 offset `0x0002C724`.
- Object 1 is the code object: virtual size `0x11302`, base `0x10000`, flags `0x2005`, pages 1-18.
- Object 2 is data/stack: virtual size `0x9E90`, base `0x30000`, flags `0x2003`, pages 19-24.
- Entry point is object 1 offset `0xCC1C`, mapping exactly to file offset `0x00039340`.
- Initial stack is object 2 offset `0x9E90`; automatic data object is object 2.
- Entry bytes are `EB 78`, jumping from `CC1C` to `CC96` over the embedded Watcom run-time identification/data block.
- The LE page-data extent, including the final `0x5F7`-byte partial page, ends exactly at CODE.1 EOF (`0x43D1B`). This independently validates the recovered application image base.

## Current checkpoint

Executable/container mapping is sufficiently established to begin instruction-accountable conversion. The first live entry/startup block has been disassembled, but it is **not counted yet** because its DOS INT 21h effects, segment-register state, and Watcom start-up contract still need to be represented faithfully in C++.

Next conversion target: object 1 entry/startup beginning at logical `0xCC1C` / file `0x39340`.

## Primary technical references

- Open Watcom Programmer's Guide: DOS/4GW uses the LE linear executable format and a 32-bit 386 flat-address-space runtime.
- IBM OS/2 16/32-bit Object Module Format (preserved by Open Watcom): LE/LX header, object, page, entry, and fixup definitions.
- Watcom/Open Watcom linker documentation: `SYSTEM dos4g` selects a 32-bit DOS/4GW executable target.
- Rational/Tenberry DOS/4G documentation preserved with Open Watcom for DOS extender behavior.
- Intel x86 architectural documentation for instruction semantics.

`CODE.1` itself remains the controlling evidence whenever documentation and the actual bytes differ.