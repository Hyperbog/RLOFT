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

## Formatter continuation cleared for integration

`FormatterContinuation.staging.cpp` contains **1,308 verified instruction-accounted bytes** covering `F206..F721`. They remain excluded from the official percentage only until folded into `Code1.cpp`.

- `F206..F2D1`: **204 bytes** — signed 16.16 fixed-point decimal materialization and exact rounding/carry walk.
- `F2D2..F2E8`: **23 bytes** — indirect thunk through DGROUP `55C4h`, preserving the EAX:DX far-return contract.
- `F2E9..F721`: **1,081 bytes** — complete conversion materializer, cleared after instruction-level review.

Once folded, the integrated verified total becomes **6,781 bytes**, raw coverage **9.6318%** (conservative display **9.63%**).

## Formatter driver structural lift

`FormatterDriver.staging.cpp` now carries the complete **544-byte `EDE7..F006`** formatter driver as a structural, deliberately uncounted lift. The range was regenerated directly from the immutable CODE.1 object bytes before translation.

The lift preserves the stack-resident formatter-state layout, literal-character callback path, `%n` word/dword and near/far stores, the encoded but operationally redundant `40h` tests, the call into `F2E9`, both right-alignment padding phases, prefix emission, zero fill, selector-qualified payload reads, the two-byte source stride for `s/S` with flag `20h`, left-alignment trailing spaces, and the returned count at `F001`.

The original `F2E9` routine jumps into this driver's `F001` epilogue rather than returning through an independent epilogue. The staging source documents that shared-tail relationship explicitly and models it as a normal materializer return so the two verified regions can later be folded into one coherent C++ representation.

`EDE7..F006` is not yet included in the verified byte count. The next pass is an instruction-by-instruction reconciliation of the lift, especially the callback-owned count at state `+10h`, post-decrement loop state, and `%n` guest-memory writes.

## Active continuation

Integrate the cleared `F206..F721` materializer into `Code1.cpp`; simultaneously harden `EDE7..F006` against its exact instruction stream. Once both are integrated, the formatter dependency region `EDE7..F741` will be contiguous except for already classified `EDD3..EDDF` data immediately before it.

## Larger-batch policy

The working target remains approximately **4,000 newly verified bytes per substantial batch**, with fidelity controlling. Analysis continues past individual dependency edges rather than treating them as stopping points.

## Unresolved queue

Integrate `F206..F721`; verify/integrate `EDE7..F006`; allocator bodies at `DAD9`/`DB5B`; candidate compressed-data routine `DB80`; transformation routine `DC90`; uncertain entry at `DE6F`; startup successor work; remaining address-named CRT helpers.

`CODE.1` remains controlling evidence whenever documentation and the actual bytes differ.
