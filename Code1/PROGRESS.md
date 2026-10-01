# CODE.1 faithful decompilation progress

![Progress](https://img.shields.io/badge/verified%20conversion-3.31%25-red)

**Verified conversion:** 3.31%  
**Remaining:** 96.69%  
**Status color:** RED until verified conversion reaches exactly 100%; GREEN only at 100%.

## Measurement rule

The percentage is the conservative coverage floor: instruction-accounted translated bytes divided by the complete object-1 executable extent (`0x11302` bytes). Embedded data remains in the denominator until code/data separation is proven. Mapping, research, scaffolding, pseudocode, and unverified translations do not count.

Current verified translated bytes: **2,332**.  
Current conservative denominator: **70,402 (`0x11302`) bytes**.  
Raw coverage: **3.3124%**, displayed conservatively as **3.31%**.

## Immutable source identity

- File: `CODE.1`
- Size: `277,787` bytes (`0x43D1B`)
- SHA-256: `fd9b709ceee190e6a70870f9012ebea3bc437cae6bc74e0150d6f922fa750fed`

## Verified translation inventory

- Startup/recovery regions through `CD96`: **259 bytes**.
- Structured mouse-facing routines `D08C..D153` plus overlap-safe copy `D1E8..D239`: **274 bytes**.
- File read/write/seek/delete `D5AD..D8CD`: **801 bytes**.
- DOS/CRT directory and utility routines `D8D0..DA1B`: **332 bytes**.
- Newly verified low-level/runtime regions: **666 bytes**.

Total verified translated bytes: **2,332**.

## Current pass: 666 newly verified bytes

This pass resolved and translated several independent regions rather than stopping at the allocator dependency:

- `10384..10396` (19): DOS carry/error normalizer used by memory-service wrappers.
- `DA48..DA6B` (36), `DA6C..DA7D` (18), `DA7E..DAA5` (40): DOS `AH=48h/49h/4Ah` allocation/free/resize wrappers, including the failure-side maximum-paragraph return from `BX`.
- `DAA6..DAC8` (35): allocator-list traversal returning the largest observed block size minus four bytes.
- `DDE0..DDE6` (7), `DDE7..DDF1` (11), `DDF2..DDFD` (12): no-op and byte port-I/O wrappers.
- `DDFE..DE34` (55), `DE35..DE6E` (58): interrupt-vector set/get wrappers, retaining the distinct DOS-extender modes `2..8` rather than collapsing them into legacy DOS semantics.
- `DEA7..DF12` (108): optimized dword-pattern fill helper. `DE6F..DEA0` remains deliberately uncounted because its true entry boundary is not yet proven.
- `DF13..DF72` (96) and `DF73..DF8D` (27): PIT channel-2 / PC-speaker programming and disable path.
- `DFB3..DFFA` (72), `DFFB..E02A` (48), `E050..E097` (72), `E098..E0C7` (48): two independently encoded unsigned radix conversion routines and their signed decimal wrappers. The intervening digit alphabets are classified as data and are not counted as translated code.

The signed conversion wrappers preserve two's-complement magnitude behavior for the most-negative 32-bit value rather than introducing host-language signed-negation overflow.

## Larger-batch policy

The working target remains approximately **4,000 newly verified bytes per substantial batch**, but fidelity is controlling. This checkpoint is below that target because these 666 bytes are the regions completed and verified during this continuation; work continues across other independent regions rather than inflating coverage with unresolved code.

## Unresolved queue

The allocator bodies at `DAD9`/`DB5B`, the probable compressed-data routine at `DB80`, the transformation routine at `DC90`, the uncertain entry at `DE6F`, startup successor work, and remaining address-named CRT helpers stay queued until their dependencies and boundaries are proven.

`CODE.1` remains controlling evidence whenever documentation and the actual bytes differ.
