# CODE.1 faithful decompilation progress

![Progress](https://img.shields.io/badge/verified%20conversion-0.18%25-red)

**Verified conversion:** 0.18%  
**Remaining:** 99.82%  
**Status color:** RED until verified conversion reaches exactly 100%; GREEN only at 100%.

## Measurement rule

The percentage is a conservative coverage floor: instruction-accounted translated bytes divided by the complete object-1 executable extent (`0x11302` bytes). Embedded data remains in the denominator until code/data separation is proven. Mapping, research, scaffolding, pseudocode, and unverified translations do not count.

Current verified translated bytes: **131**.  
Current conservative denominator: **70,402 (`0x11302`) bytes**.  
Raw coverage: **0.1861%**, displayed conservatively as **0.18%**.

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

## Verified C++ translation

### Region 1 — entry/startup prefix
- `CC1C..CC1D`: 2-byte entry jump.
- `CC96..CCDD`: 72-byte startup block.
- Counted: **74 bytes**.

### Region 2 — CCDE fall-through path
- `CCDE..CD16`: **57 bytes**.
- Preserves BL/AL partial-register operations and saved EAX behavior.
- Loads the selector stored at DGROUP `52FC`, reads the original `ES:[5C]` value through an explicit input boundary, rounds it upward to a 4-KiB boundary, stores the result at DGROUP `52F0`, and shifts it by 12 exactly as encoded.
- Reconstructs `ES=DS`, `AH=4Ah`, and the DOS `INT 21h` boundary without replacing DOS/4GW behavior with host assumptions.
- Restores EAX from the original push, sets `BX=DS`, `CX=002Ch`, then jumps to `CD4E`.

Total verified translated bytes: **131**.

## Documentation check

Open Watcom's DOS/4GW documentation states that protected-mode `INT 21h` calls are translated to the corresponding DOS register/pointer forms by DOS/4GW. It also specifically identifies DOS function `4Ah` as a kernel-mediated segment allocation/expansion operation. This supports preserving the interrupt as an explicit extender/DOS boundary instead of pretending it is an ordinary C++ allocator.

## Next checkpoint

Translate the alternate successor beginning at `CD17` and establish its join at `CD4E`. No bytes in that path are counted until the complete branch effects are represented.

`CODE.1` remains controlling evidence whenever documentation and the actual bytes differ.