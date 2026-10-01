# CODE.1 faithful decompilation progress

![Progress](https://img.shields.io/badge/verified%20conversion-0%25-red)

**Verified conversion:** 0%  
**Remaining:** 100%  
**Status color:** RED until verified conversion reaches exactly 100%; GREEN only at 100%.

## Measurement rule

The percentage measures instruction-accountable conversion of the Ravenloft application machine code recovered from the original `CODE.1`. Loader/extender discovery, executable mapping, research, speculative pseudocode, and unverified translations do **not** count as converted code.

A region counts only after its original byte range and decoded x86 instructions are established and the corresponding C++ block accounts for their observable behavior. If evidence is insufficient, the item remains unresolved instead of being guessed.

## Immutable source identity

- File: `CODE.1`
- Size: `277,787` bytes
- SHA-256: `fd9b709ceee190e6a70870f9012ebea3bc437cae6bc74e0150d6f922fa750fed`

## Verified observations

- File begins with an `MZ` DOS executable header.
- The bound executable contains Rational Systems DOS/4G / DOS/16M material.
- A `BW` structure is present at file offset `0x0000F444`; nearby embedded name is `4GWPRO.EXP`.
- The binary contains the literal runtime identification `WATCOM C/C++32 Run-Time system. (c) Copyright by WATCOM International Corp. 1988-1993. All rights reserved.` at file offset `0x00039342` (the preceding byte at `0x39341` is not part of the text).
- Game-specific strings, including `Ravenloft`, occur later in the image (for example `Ravenloft` begins at file offset `0x0003E7AC`).

These observations establish toolchain/runtime evidence, but they do not by themselves prove the original source-language form of every game routine.

## Current phase

1. Map the bound DOS/4G/DOS16M executable and isolate the protected-mode application image.
2. Establish application entry point, segments/objects, relocations, and file-to-runtime address mapping.
3. Disassemble application code with original offsets retained.
4. Translate verified blocks into `Code1.cpp` without invented semantics.
5. Continue until all application instructions are accounted for.

## Primary technical references

Research may use original/official Watcom/Open Watcom, Rational/Tenberry DOS/4G documentation preserved with Open Watcom, and processor documentation. `CODE.1` itself remains the controlling evidence.