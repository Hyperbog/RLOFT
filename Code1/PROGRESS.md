# CODE.1 faithful decompilation progress

![Progress](https://img.shields.io/badge/verified%20conversion-0.36%25-red)

**Verified conversion:** 0.36%  
**Remaining:** 99.64%  
**Status color:** RED until verified conversion reaches exactly 100%; GREEN only at 100%.

## Measurement rule

The percentage is a conservative coverage floor: instruction-accounted translated bytes divided by the complete object-1 executable extent (`0x11302` bytes). Embedded data remains in the denominator until code/data separation is proven. Mapping, research, scaffolding, pseudocode, and unverified translations do not count.

Current verified translated bytes: **259**.  
Current conservative denominator: **70,402 (`0x11302`) bytes**.  
Raw coverage: **0.3679%**, displayed conservatively as **0.36%**.

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

- Protected-mode interrupts cross an explicit boundary; translated blocks do not silently assume ordinary DOS semantics.
- DGROUP offsets, register-derived linear addresses, and selector-relative addresses are distinct address spaces.
- Selector-relative reads now cross their own resolver boundary, required by the newly translated `ES:[2C]` and temporary-`DS:[2C]` accesses.
- Segment-register moves into 32-bit GPRs are treated conservatively for the declared 80386 target without assuming later-processor upper-word zero extension.

These architectural safeguards add zero progress by themselves.

## Verified C++ translation

### Region 1 — entry/startup prefix
- `CC1C..CC1D` plus `CC96..CCDD`: **74 bytes**.

### Region 2 — CCDE fall-through path
- `CCDE..CD16`: **57 bytes**.

### Region 3 — CD17 alternate successor/equal path
- `CD17..CD4F`: **57 bytes**.
- `CMP AX,4243h` / `JNE` target is verified as **CD50**.
- The equal path reaches the common successor at `CD97`.

### Region 4 — CD50 non-equal-signature path
- `CD50..CD96`: **71 bytes**.
- Preserves `DX=0078h`, `AX=FF00h`, the protected-mode `INT 21h`, and the subsequent branch on returned `AL`.
- The nonzero-AL path preserves the GS selector test/store, `AL=1`, DS/ES selector transfers, and `ES:[2C]` read.
- The zero-AL path preserves DS in DX, temporarily loads `DS=0024h`, reads `DS:[2C]`, restores DS, then sets `BX=0017h` and `AL=0`.
- Both paths converge at `CD97`.

Total verified translated bytes: **259**.

## Next checkpoint

Continue from the common startup successor at `CD97`. The low-level machine representation remains recovery scaffolding; proven application routines will be lifted into structured C++ rather than extending this into a general x86 emulator.

`CODE.1` remains controlling evidence whenever documentation and the actual bytes differ.
