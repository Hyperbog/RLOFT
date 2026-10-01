# CODE.1 faithful decompilation progress

![Progress](https://img.shields.io/badge/verified%20conversion-5.22%25-red)

**Verified conversion:** 5.22%  
**Remaining:** 94.78%  
**Status color:** RED until verified conversion reaches exactly 100%; GREEN only at 100%.

## Measurement rule

The percentage is the conservative coverage floor: instruction-accounted translated bytes divided by the complete object-1 executable extent (`0x11302` bytes). Embedded data remains in the denominator until code/data separation is proven. Mapping, research, scaffolding, pseudocode, staging files, and unintegrated translations do not count.

Current verified translated bytes: **3,680**.  
Current conservative denominator: **70,402 (`0x11302`) bytes**.  
Raw coverage: **5.2271%**, displayed conservatively as **5.22%**.

## Immutable source identity

- File: `CODE.1`
- Size: `277,787` bytes (`0x43D1B`)
- SHA-256: `fd9b709ceee190e6a70870f9012ebea3bc437cae6bc74e0150d6f922fa750fed`

## Verified translation inventory

- Startup/recovery regions through `CD96`: **259 bytes**.
- Structured mouse-facing routines `D08C..D153` plus overlap-safe copy `D1E8..D239`: **274 bytes**.
- File read/write/seek/delete `D5AD..D8CD`: **801 bytes**.
- DOS/CRT directory and utility routines `D8D0..DA1B`: **332 bytes**.
- Low-level/runtime regions through `E0C7`: **666 bytes**.
- Timer/PIT setup and state management `E4C7..E6E9`: **547 bytes**.
- Timer slot/control continuation `E6EA..EA0A`: **801 bytes**.

Total integrated verified translated bytes: **3,680**.

## Current instruction-checked staging

`DispatcherTimer.staging.cpp` contains **1,250 instruction-accounted code bytes** awaiting fold-in to the single `Code1.cpp` reconstruction.

- `E3B4..E4C6`: **275 bytes** — callback lookup, common dispatcher, INT 8 handler, PIC EOI path, and saved-vector chain behavior.
- `EA0B..EBF2`: **488 bytes** — descriptor/slot registration and clearing, events `64h`/`65h`, timer-backed event setup, pending-state release, and event `68h` dispatch.
- `EBF3..EDD2`: **480 bytes** — 48 consecutive ten-byte dispatcher wrappers.
- `EDE0..EDE6`: **7 bytes** — exact no-op-style frame routine.

`FormatterHelpers.staging.cpp` now adds **543 instruction-accounted translated bytes**:

- `F007..F10F`: **265 bytes** — format flags, width, precision, `*` argument consumption, and encoded length-modifier parsing.
- `F110..F162`: **83 bytes** — `-`, `#`, `+`, space, and `0` flag scanner.
- `F163..F18B`: **41 bytes** — selector-qualified bounded byte-string scan.
- `F18C..F1B2`: **39 bytes** — selector-qualified bounded two-byte-string scan.
- `F1B3..F205`: **83 bytes** — hexadecimal conversion followed by left zero-padding.
- `F722..F741`: **32 bytes** — in-place uppercase helper through the already identified `D991` ASCII transform.

Total translated staging is now **1,793 bytes**. These remain excluded from the official percentage until folded into `Code1.cpp`.

`EDD3..EDDF` is **13 zero bytes** and remains classified as data rather than code.

## Formatter continuation

The complete formatter dependency region `F007..F741` remains bounded at **1,851 bytes**. Of that region, **543 bytes are now lifted into instruction-accounted C++ staging**. The remaining principal body is `F206..F721`, including the conversion materializer beginning at `F2E9`.

`EDE7..F006` remains a separately bounded **544-byte** formatted-output core. It will not be counted until its helper behavior is represented without substituting libc formatting semantics.

Important fidelity details retained in the new staging include signed handling of `*` width and precision, exact flag-bit mutations, one-dword argument-cursor consumption for each `*`, selector-qualified string reads, and the distinct byte/two-byte scan increments.

## Active continuation

Continue lifting `F206..F721`, especially `F2E9`, then lift `EDE7..F006` against those helpers. The dispatcher/timer and formatter staging will be folded into `Code1.cpp` only after the combined representation is rechecked.

## Larger-batch policy

The working target remains approximately **4,000 newly verified bytes per substantial batch**, with fidelity controlling. Analysis continues past individual dependency edges rather than treating them as stopping points.

## Unresolved queue

Fold the 1,793 staged bytes into `Code1.cpp`; lift `F206..F721` and then `EDE7..F006`; allocator bodies at `DAD9`/`DB5B`; candidate compressed-data routine `DB80`; transformation routine `DC90`; uncertain entry at `DE6F`; startup successor work; remaining address-named CRT helpers.

`CODE.1` remains controlling evidence whenever documentation and the actual bytes differ.
