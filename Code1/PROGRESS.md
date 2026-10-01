# CODE.1 faithful decompilation progress

![Progress](https://img.shields.io/badge/verified%20conversion-7.77%25-red)

**Verified conversion:** 7.77%  
**Remaining:** 92.23%  
**Status color:** RED until verified conversion reaches exactly 100%; GREEN only at 100%.

## Measurement rule

The percentage is the conservative coverage floor: instruction-accounted translated bytes divided by the complete object-1 executable extent (`0x11302` bytes). Embedded data remains in the denominator until code/data separation is proven. Mapping, research, scaffolding, pseudocode, and unintegrated translations do not count.

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

## Current formatter continuation staging

`FormatterContinuation.staging.cpp` carries **227 newly instruction-accounted bytes** from the original binary. They remain excluded from the percentage until folded into `Code1.cpp`.

- `F206..F2D1`: **204 bytes** — fixed-point decimal materialization, default precision assignment, decimal-point emission, repeated multiply-by-ten digit generation, and the encoded carry/rounding walk across the generated string.
- `F2D2..F2E8`: **23 bytes** — exact three-argument indirect thunk through the function pointer stored at DGROUP `55C4h`.

## Materializer verification

`F2E9..F721` (**1,081 bytes**) has now been re-disassembled and recorded in `FormatterMaterializer.map.md` as an instruction-derived branch ledger. This map is deliberately not counted as translation. It records the exact entry state, conversion-family branch ranges, common accounting tail, and the memory-model constraints that must survive the C++ lift.

The hardened map specifically preserves these non-host semantics before code generation: selector+offset far pointers; in-memory mutation of the vararg cursor; EAX+DX offset/selector return from `F2D2`; the two-component far-null test; ES:EDI use in the common sign/accounting tail; and in-place mutation of width, precision, prefix and zero-fill fields.

This verification replaces the earlier unsafe temptation to flatten formatter addresses into host pointers. No additional materializer bytes are counted until the C++ representation satisfies these constraints.

## Active continuation

Lift `F2E9..F721` against the new selector/cursor ledger, then fold `F206..F721` into `Code1.cpp`. After the materializer is integrated, translate `EDE7..F006` against the proven helper set.

## Larger-batch policy

The working target remains approximately **4,000 newly verified bytes per substantial batch**, with fidelity controlling. Analysis continues past individual dependency edges rather than treating them as stopping points.

## Unresolved queue

Fold the verified 227-byte formatter continuation; lift `F2E9..F721` and then `EDE7..F006`; allocator bodies at `DAD9`/`DB5B`; candidate compressed-data routine `DB80`; transformation routine `DC90`; uncertain entry at `DE6F`; startup successor work; remaining address-named CRT helpers.

`CODE.1` remains controlling evidence whenever documentation and the actual bytes differ.
