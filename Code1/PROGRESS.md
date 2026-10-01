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

- `F206..F2D1`: **204 bytes** — signed 16.16 fixed-point decimal materialization and encoded rounding/carry walk.
- `F2D2..F2E8`: **23 bytes** — indirect thunk through DGROUP `55C4h`, preserving the EAX:DX far-return contract.

`F2E9..F721` (**1,081 bytes**) remains a structural C++ lift under verification and is explicitly **uncounted**.

## Formatter register-alias verification pass

A fresh disassembly of the complete `F206..F721` region was checked directly against the immutable CODE.1 bytes. This pass resolved two aliasing cases where a conventional source-level formatter reconstruction gives the wrong result:

- `F206` stores the absolute input as one dword at `[EBP-4]`, passes **WORD `[EBP-2]`** to `DFFB`, then repeatedly zeroes that same high word before `IMUL value,10`. The helper is therefore operating on signed **16.16 fixed-point** state. Fractional digits are the successive high-word values after multiplying the retained low 16-bit fraction by ten. `TEST BYTE [EBP-3],80h` is exactly a test of bit 15 of the final retained fraction for rounding. The carry-through-front path at `F284..F2B4` is now represented explicitly rather than replaced by host numeric formatting.
- `F477..F4A4` deliberately aliases two output cursors. ESI advances past an alternate-form octal `0`, while EDI remains at the original scratch start. The zero-precision test examines `[ESI]`, but the suppressing NUL is written through `ES:EDI`. Thus alternate-form octal zero clears the original prefix byte; the staging model now preserves that encoded behavior.
- `F4FC..F548` was rechecked for null far strings: `{offset=0,selector=0}` falls back to the caller-supplied null string **and resets the active selector to DS** at `F545`. A non-null far pointer retains its supplied selector.
- The previously corrected EAX:DX callback return, `S` one-character/two-byte-character path, signed precision comparisons, and `AND F9h` flag masks remain consistent with the new disassembly.

No libc `printf`, host floating-point conversion, or flat-pointer substitution is used in these paths.

## Active continuation

Continue the remaining instruction-level review of `F2E9..F721`, with particular attention to the pointer path and common-tail live-register values. Once all branches are reconciled, fold `F206..F721` into `Code1.cpp`; then translate `EDE7..F006` against the proven formatter helpers.

## Larger-batch policy

The working target remains approximately **4,000 newly verified bytes per substantial batch**, with fidelity controlling. Analysis continues past individual dependency edges rather than treating them as stopping points.

## Unresolved queue

Fold the verified 227-byte formatter continuation; complete and verify `F2E9..F721`; translate `EDE7..F006`; allocator bodies at `DAD9`/`DB5B`; candidate compressed-data routine `DB80`; transformation routine `DC90`; uncertain entry at `DE6F`; startup successor work; remaining address-named CRT helpers.

`CODE.1` remains controlling evidence whenever documentation and the actual bytes differ.
