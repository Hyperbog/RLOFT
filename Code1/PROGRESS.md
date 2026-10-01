# CODE.1 faithful decompilation progress

![Progress](https://img.shields.io/badge/verified%20conversion-0.75%25-red)

**Verified conversion:** 0.75%  
**Remaining:** 99.25%  
**Status color:** RED until verified conversion reaches exactly 100%; GREEN only at 100%.

## Measurement rule

The percentage is a conservative coverage floor: instruction-accounted translated bytes divided by the complete object-1 executable extent (`0x11302` bytes). Embedded data remains in the denominator until code/data separation is proven. Mapping, research, scaffolding, pseudocode, and unverified translations do not count.

Current verified translated bytes: **533**.  
Current conservative denominator: **70,402 (`0x11302`) bytes**.  
Raw coverage: **0.7571%**, displayed conservatively as **0.75%**.

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

## Verified translation

Previously verified startup regions remain **259 bytes** through `CD96`.

### Structured lift — D08C..D153 and D1E8..D239

An additional **274 bytes** are now instruction-accounted as ordinary structured C++, rather than extending the startup CPU-state scaffold:

- `D08C..D0B0`: 37-byte gated INT 33h wrapper, preserving AX=4 and original argument/register placement.
- `D0B4..D108`: 85-byte mouse-state query. Preserves INT 33h AX=3 and the exact stores at DGROUP `8A60`, `8A64`, and `8A66`, including the signed arithmetic shift of CX.
- `D10C..D12E`: 35-byte gated INT 33h AX=7 wrapper.
- `D130..D152`: 35-byte gated INT 33h AX=8 wrapper.
- `D1E8..D239`: 82-byte overlap-safe copy routine. Its forward/backward overlap tests and copy behavior establish the semantic contract of `memmove`, so the structured C++ uses `std::memmove` rather than simulated registers.

Open Watcom's DOS/4GW programming documentation explicitly documents protected-mode support for Microsoft-standard interrupt `33h`, corroborating the mouse boundary while CODE.1 remains the controlling evidence.

## Fidelity stop

The ~4,000-byte batch target is intentionally not a quota. This batch stopped after **274 new verified bytes** because the next nearby routines depend on calls at `84D0`, `8960`, `82A4`, and related targets whose semantics have not yet been established. They remain address-named boundaries rather than receiving guessed subsystem names.

This is also the first checkpoint implementing the agreed hybrid approach: low-level state remains only where needed for startup recovery, while independently proven routines are lifted directly into structured native C++.

Total verified translated bytes: **533**.

## Next checkpoint

Resolve the call graph around `84D0`/`8960` and the startup successor at `CD97`, then continue toward the ~4,000-byte batch target wherever control-flow and data semantics remain provable.

`CODE.1` remains controlling evidence whenever documentation and the actual bytes differ.
