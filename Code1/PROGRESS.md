# CODE.1 faithful decompilation progress

![Progress](https://img.shields.io/badge/verified%20conversion-0.27%25-red)

**Verified conversion:** 0.27%  
**Remaining:** 99.73%  
**Status color:** RED until verified conversion reaches exactly 100%; GREEN only at 100%.

## Measurement rule

The percentage is a conservative coverage floor: instruction-accounted translated bytes divided by the complete object-1 executable extent (`0x11302` bytes). Embedded data remains in the denominator until code/data separation is proven. Mapping, research, scaffolding, pseudocode, and unverified translations do not count.

Current verified translated bytes: **188**.  
Current conservative denominator: **70,402 (`0x11302`) bytes**.  
Raw coverage: **0.2670%**, displayed as **0.27%**.

## Immutable source identity

- File: `CODE.1`
- Size: `277,787` bytes (`0x43D1B`)
- SHA-256: `fd9b709ceee190e6a70870f9012ebea3bc437cae6bc74e0150d6f922fa750fed`

## Verified executable mapping

- Outer MZ: `0x00000000`.
- Bound Rational/Tenberry BW image: `0x0000F444`; embedded name `4GWPRO.EXP`.
- BW `next_header_offset = 0x25D24`, locating the application MZ.
- Application MZ: `0x00025D24`; LE header: `0x00027BDC`; LE page data: `0x0002C724`.
- Object 1: executable/readable, virtual size `0x11302`, base `0x10000`, flags `0x2005`, pages 1-18.
- Object 2: writable data/stack, virtual size `0x9E90`, base `0x30000`, flags `0x2003`, pages 19-24.
- Entry: object 1 `0xCC1C` / file `0x39340`; initial stack object 2 `0x9E90`.
- Entry `EB 78` jumps to `CC96`, over embedded Watcom runtime identification/data.
- LE page-data extent ends exactly at CODE.1 EOF (`0x43D1B`).

## Preventative architecture checkpoint

The early review concerns were addressed before expanding translation:

- `INT 21h` is no longer modeled as a callback whose name implies ordinary DOS semantics. It crosses an explicit protected-mode interrupt boundary carrying the machine state and interrupt vector. No DOS/4GW behavior is guessed inside the translated block.
- DGROUP-relative offsets and register-derived linear addresses are represented by distinct address types. Linear reads cross an explicit resolver boundary instead of silently indexing the Object-2 byte array.
- Existing translated DGROUP accesses were converted to the typed DGROUP boundary without changing their recovered offsets.
- Segment-register moves into 32-bit GPRs are treated conservatively for the declared 80386 target: the selector word is written without assuming later-processor upper-word zero extension.

This refactor adds **zero** progress by itself; only newly instruction-accounted CODE.1 bytes affect the percentage.

## Verified C++ translation

### Region 1 — entry/startup prefix
- `CC1C..CC1D`: 2-byte entry jump.
- `CC96..CCDD`: 72-byte startup block.
- Counted: **74 bytes**.

### Region 2 — CCDE fall-through path
- `CCDE..CD16`: **57 bytes**.
- Preserves partial-register behavior, stack-saved EAX, selector operations, 4-KiB rounding, DGROUP stores, and the protected-mode `INT 21h` boundary.

### Region 3 — CD17 alternate successor/equal path
- `CD17..CD4F`: **57 bytes**.
- `CMP AX,4243h` / `JNE` is decoded with target **CD50**.
- The equal path preserves `[3094]=EDX`, register-derived reads at `[ESI+1C]`, `[ESI+10]`, and `[EAX+2C]`, `AH=4Ah / INT 21h`, selector stores, `EDI`/`ESI` arithmetic, `AL=09h`, and the final jump to `CD97`.
- Register-derived memory reads use the new linear-address resolver boundary and are not assumed to be DGROUP offsets.

Total verified translated bytes: **188**.

## Correction caught before downstream expansion

An earlier checkpoint comment described the `CD1B` conditional branch as targeting `CD4E`. Direct decoding of the original bytes (`0F 85 2F 00 00 00`) establishes the target as **`CD50`**. `Code1.cpp` and this progress record now use the corrected edge before the target block is translated.

## Next checkpoint

Translate the `CD50` alternate path and establish its convergence at `CD97`. The new address/interrupt boundaries stay in place as invariants for subsequent reconstruction.

`CODE.1` remains controlling evidence whenever documentation and the actual bytes differ.