# CODE.1 faithful decompilation progress

![Progress](https://img.shields.io/badge/verified%20conversion-7.77%25-red)

**Verified conversion:** 7.77%  
**Remaining:** 92.23%  
**Status color:** RED until verified conversion reaches exactly 100%; GREEN only at 100%.

## Measurement rule

The percentage is the conservative coverage floor: instruction-accounted translated bytes divided by the complete object-1 executable extent (`0x11302` bytes). Embedded data remains in the denominator until code/data separation is proven. Mapping, research, scaffolding, pseudocode, draft lifts, and unintegrated translations do not count.

Current verified translated bytes: **5,473**.  
Current conservative denominator: **70,402 (`0x11302`) bytes**.  
Raw coverage: **7.7739%**, displayed conservatively as **7.77%**.

## Immutable source identity

- File: `CODE.1`
- Size: `277,787` bytes (`0x43D1B`)
- SHA-256: `fd9b709ceee190e6a70870f9012ebea3bc437cae6bc74e0150d6f922fa750fed`

## Integrated verified translation inventory

- Startup/recovery regions through `CD96`: **259 bytes**.
- Structured mouse-facing routines `D08C..D153` plus overlap-safe copy `D1E8..D239`: **274 bytes**.
- File read/write/seek/delete `D5AD..D8CD`: **801 bytes**.
- DOS/CRT directory and utility routines `D8D0..DA1B`: **332 bytes**.
- Low-level/runtime regions through `E0C7`: **666 bytes**.
- Dispatcher and INT 8 paths `E3B4..E4C6`: **275 bytes**.
- Timer/PIT setup and state management `E4C7..E6E9`: **547 bytes**.
- Timer slot/control continuation `E6EA..EA0A`: **801 bytes**.
- Registration/event subsystem `EA0B..EBF2`: **488 bytes**.
- 48 exact dispatcher wrappers `EBF3..EDD2`: **480 bytes**.
- Frame no-op `EDE0..EDE6`: **7 bytes**.
- Formatter helper subset `F007..F205` and `F722..F741`: **543 bytes**.

Total integrated verified translated bytes: **5,473**.

`EDD3..EDDF` is **13 zero bytes** classified as data and is not counted as translated code.

## Current formatter continuation

`FormatterContinuation.staging.cpp` contains the verified **227-byte** helper continuation plus a first structural lift of the complete **1,081-byte `F2E9..F721` conversion materializer**.

Verified but not yet integrated:

- `F206..F2D1`: **204 bytes** — fixed-point decimal materialization and encoded rounding walk.
- `F2D2..F2E8`: **23 bytes** — exact three-argument indirect thunk through DGROUP `55C4h`.

Newly traced/lifted, but deliberately **not yet counted**:

- `F2E9..F721`: **1,081 bytes** — complete conversion switch and common tail, including integer argument-width handling, precision/pad interaction, floating-helper dispatch, octal/hex prefixes, selector-qualified `s`/`S`, near/far pointer rendering, `c`, unknown-conversion fallback, sign-prefix extraction, and final width/precision accounting.

The entire `F2E9..F721` range was re-disassembled directly from the immutable CODE.1 object bytes in this pass. Its C++ is still marked as a structural lift because the exact emulated argument-pointer and selector-memory representation must be reconciled with `Code1.cpp` before it can be called instruction-accounted and integrated. This prevents the percentage from being inflated by a convenient but unproven host-pointer model.

## Active continuation

Next work is to harden `F2E9..F721` against the exact instruction stream, replace host-pointer shortcuts with the same explicit memory model used by the integrated reconstruction, syntax-check it, and fold `F206..F721` into `Code1.cpp`. Once that dependency is integrated, `EDE7..F006` can be lifted against proven formatter helpers.

## Larger-batch policy

The working target remains approximately **4,000 newly verified bytes per substantial batch**, with fidelity controlling. Analysis continues past individual dependency edges rather than treating them as stopping points.

## Unresolved queue

Harden and integrate `F206..F721`; lift `EDE7..F006`; allocator bodies at `DAD9`/`DB5B`; candidate compressed-data routine `DB80`; transformation routine `DC90`; uncertain entry at `DE6F`; startup successor work; remaining address-named CRT helpers.

`CODE.1` remains controlling evidence whenever documentation and the actual bytes differ.
