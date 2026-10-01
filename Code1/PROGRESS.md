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

`FormatterContinuation.staging.cpp` contains **227 verified instruction-accounted bytes**, still excluded from the official percentage until fold-in:

- `F206..F2D1`: **204 bytes** — fixed-point decimal materialization and encoded rounding walk.
- `F2D2..F2E8`: **23 bytes** — indirect thunk through DGROUP `55C4h`.

`F2E9..F721` (**1,081 bytes**) remains a structural C++ lift under verification and is explicitly **uncounted**.

## Materializer correction pass

The immutable CODE.1 bytes for the complete `F2E9..F721` region were disassembled again before editing the structural lift. The staging model now preserves several register-level behaviors that a conventional formatter abstraction would lose:

- `F2D2` is represented as an **EAX:DX far return pair** (`offset`, `selector`) rather than an EAX-only callback result.
- The unsigned pre-read at `F30D..F36D` is limited to `X/o/u/x`; `d/i` consume signed arguments later at `F4BA`.
- `F/f` with flag `10h` is the path that invokes `F206`; the other floating conversions use the `F2D2` callback path.
- `d/i` preserve the encoded distinction between `E098` for the `20h`/`10h` length forms and `DFFB` for the default form.
- The `S` path now carries the selector-qualified character value in live EAX into the common tail, rather than inventing a string length. Its returned source offset is advanced by one or two bytes exactly as encoded.
- Common-tail precision comparisons are modeled as signed `CMP/Jcc` relationships where the original uses signed conditions.
- Guest offsets/selectors remain guest addresses; none are reinterpreted as host pointers.

A stale `DispatcherWrappers.staging.cpp` duplicate was also removed because `EBF3..EDE6` is already integrated and counted in `Code1.cpp`; leaving it present risked future double-accounting.

The materializer is still withheld from the verified count. The octal alternate-form/zero-precision path around `F477..F4A4` and the fixed-point helper's retained-word arithmetic are the next byte-level checks before promotion.

## Active continuation

Finish the remaining `F2E9..F721` byte-level checks, fold verified `F206..F721` into `Code1.cpp`, then translate `EDE7..F006` against the proven formatter helpers.

## Larger-batch policy

The working target remains approximately **4,000 newly verified bytes per substantial batch**, with fidelity controlling. Analysis continues past individual dependency edges rather than treating them as stopping points.

## Unresolved queue

Fold the verified 227-byte formatter continuation; complete and verify `F2E9..F721`; translate `EDE7..F006`; allocator bodies at `DAD9`/`DB5B`; candidate compressed-data routine `DB80`; transformation routine `DC90`; uncertain entry at `DE6F`; startup successor work; remaining address-named CRT helpers.

`CODE.1` remains controlling evidence whenever documentation and the actual bytes differ.
