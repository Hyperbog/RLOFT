# CODE.1 faithful decompilation progress

![Progress](https://img.shields.io/badge/verified%20conversion-0.10%25-red)

**Verified conversion:** 0.10%  
**Remaining:** 99.90%  
**Status color:** RED until verified conversion reaches exactly 100%; GREEN only at 100%.

## Measurement rule

The percentage is a conservative coverage floor: instruction-accounted translated bytes divided by the complete object-1 executable extent (`0x11302` bytes). This deliberately keeps embedded data in the denominator until code/data separation is proven, so discovery cannot inflate progress. Executable mapping, research, scaffolding, speculative pseudocode, and unverified translations do **not** count.

A region counts only after its original byte range and decoded x86 instructions are established and the corresponding C++ accounts for their observable behavior. If evidence is insufficient, the item remains unresolved instead of being guessed.

Current verified translated bytes: **74**.  
Current conservative denominator: **70,402 (`0x11302`) bytes**.  
Raw coverage: **0.1051%**, displayed as **0.10%**.

## Immutable source identity

- File: `CODE.1`
- Size: `277,787` bytes (`0x43D1B`)
- SHA-256: `fd9b709ceee190e6a70870f9012ebea3bc437cae6bc74e0150d6f922fa750fed`

## Verified executable mapping

- Outer DOS MZ image begins at file offset `0x00000000`.
- Bound Rational/Tenberry `BW` image begins at `0x0000F444`; embedded name is `4GWPRO.EXP`.
- The BW `next_header_offset` is `0x25D24`, exactly locating the protected-mode application's embedded MZ.
- The protected-mode application's embedded MZ begins at `0x00025D24`.
- Its LE header begins at `0x00027BDC`.
- LE target CPU field is `2` (80386); the module has 24 pages and 2 objects.
- LE data pages begin at absolute CODE.1 offset `0x0002C724`.
- Object 1 is the executable/readable object: virtual size `0x11302`, base `0x10000`, flags `0x2005`, pages 1-18.
- Object 2 is writable data/stack: virtual size `0x9E90`, base `0x30000`, flags `0x2003`, pages 19-24.
- Entry point is object 1 offset `0xCC1C`, mapping exactly to file offset `0x00039340`.
- Initial stack is object 2 offset `0x9E90`; automatic data object is object 2.
- Entry bytes are `EB 78`, jumping from `CC1C` to `CC96` over embedded Watcom run-time identification/data.
- The LE page-data extent, including the final `0x5F7`-byte partial page, ends exactly at CODE.1 EOF (`0x43D1B`).

## Verified C++ translation

### Region 1 — entry/startup prefix

- Entry jump: object-1 `CC1C..CC1D`, file `39340..39341` — **2 bytes**.
- Startup block: object-1 `CC96..CCDD`, file `393BA..39401` — **72 bytes**.
- Total counted: **74 bytes**.
- `CC1E..CC95` is deliberately excluded: entry control flow jumps over it and it contains the Watcom run-time identification/data block.
- The translation preserves register-width operations, stack alignment, DGROUP stores, `STI`, the DOS `INT 21h` boundary, returned DOS-version bytes, and the exact `AX == 0x4458` branch decision after `EAX >> 16`.
- DOS interrupt behavior is injected through the machine boundary rather than replaced by host-side assumptions.

## Next checkpoint

Continue both successors of the first verified branch:

1. fall-through at object-1 `0xCCDE`, and
2. branch target object-1 `0xCD17`.

They will not count until both machine-state effects and their rejoin/control-flow behavior are established.

## Primary technical references

- Open Watcom source/documentation for DOS/4G and LE executable structures.
- Rational/Tenberry DOS/4G documentation preserved with Open Watcom.
- Intel x86 architectural documentation for instruction semantics.
- DOS interrupt documentation only where it can be tied to the actual interrupt/function encoded by CODE.1.

`CODE.1` itself remains the controlling evidence whenever documentation and the actual bytes differ.
