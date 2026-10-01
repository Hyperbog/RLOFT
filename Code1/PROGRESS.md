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

`DispatcherTimer.staging.cpp` contains **1,250 instruction-accounted code bytes** awaiting fold-in to the single `Code1.cpp` reconstruction. They remain excluded from the percentage until integration.

- `E3B4..E4C6`: **275 bytes** — callback lookup, common dispatcher, INT 8 handler, PIC EOI path, and saved-vector chain behavior.
- `EA0B..EBF2`: **488 bytes** — descriptor/slot registration and clearing, events `64h`/`65h`, timer-backed event setup, pending-state release, and event `68h` dispatch.
- `EBF3..EDD2`: **480 bytes** — exactly 48 consecutive ten-byte wrappers, each encoded as `mov eax,<event>; jmp E3E0`.
- `EDE0..EDE6`: **7 bytes** — exact no-op-style frame routine.

The 48 wrapper event constants, in address order, are:

`78,79,86,7A,7B,85,7C,7D,7E,7F,80,81,82,83,84,96,97,98,99,9A,9B,9C,9D,9E,9F,AA,AB,AD,AE,AF,B0,B1,B2,B3,B4,B5,B6,B7,B9,BA,BB,BC,BD,BE,BF,C0,C1,C2` (hex).

`EDD3..EDDF` is **13 zero bytes** and is deliberately classified as data rather than code. It is not included in the 1,250-byte staging total.

## Formatted-output dependency trace

The formatted-output routine `EDE7..F006` is instruction-bounded at **544 bytes**. Its formerly blocking helper chain is now bounded through `F741`, adding **1,851 dependency bytes** of exact instruction coverage for analysis. These bytes are not yet counted as translated code.

Dependency boundaries established directly from CODE.1:

- `F007..F162`: **348 bytes** — conversion-specification parser. `F110..F162` is its flag parser for `-`, `#`, `+`, space, and `0`. `F007` parses width, `*`, precision, and modifier bytes `N`, `l`/`w`, `h`, `F`, and `L` into the conversion-state record.
- `F163..F18B`: **41 bytes** — selector:offset byte-string bounded-length scan; stops on NUL or supplied count.
- `F18C..F1B2`: **39 bytes** — selector:offset word-string bounded-length scan; advances two bytes per element and stops on zero word or count.
- `F1B3..F205`: **83 bytes** — hexadecimal conversion followed by left zero-padding to a requested field length.
- `F206..F2D1`: **204 bytes** — decimal/fixed conversion helper with precision handling and an encoded rounding/carry path. No floating-point semantic name is assigned yet.
- `F2D2..F2E8`: **23 bytes** — three-argument indirect thunk through dword `[55C4]`.
- `F2E9..F721`: **1,081 bytes** — main conversion materializer used by `EDE7`. It dispatches on conversion byte and length flags, consumes the argument cursor, calls the integer/radix helpers and the indirect thunk, handles `c`, `s`/`S`, signed/unsigned decimal, octal, hexadecimal, pointer-style forms, and the `E/F/G` family, then normalizes sign/prefix/precision fields in the conversion-state record.
- `F722..F741`: **32 bytes** — in-place uppercase pass using `D991` until NUL.

Total newly bounded helper chain `F007..F741`: **1,851 bytes**. Together with `EDE7..F006`, the formatter subsystem currently has **2,395 bytes** of exact bounded code under active lifting, in addition to the separate 1,250-byte dispatcher/timer staging unit.

Important fidelity points:

- `F007` consumes `*` width and precision by incrementing the argument cursor by exactly four bytes and reading the just-consumed dword.
- Negative `*` width is negated and sets flag `08h`; negative `*` precision is replaced with `FFFFFFFFh`.
- `F163` and `F18C` preserve explicit selectors and therefore must not be collapsed into ordinary host `strlen`/wide-string calls.
- `F2E9` contains multiple far-pointer/selector paths and conversion-specific argument widths; lifting must preserve those rather than imposing a modern ABI.
- `F206` has nontrivial byte/word aliasing and rounding behavior, so it remains address-named until the exact numeric representation is proven.
- `F2D2` is deliberately kept as an indirect boundary until the target stored at `[55C4]` is established.

## Active continuation

Lift the now-bounded `F007..F741` helper chain into instruction-accountable C++, then lift `EDE7..F006` against those helpers. The separate 1,250 staged dispatcher/timer bytes remain queued for fold-in to `Code1.cpp`; neither group enters the official percentage until integration and verification.

## Larger-batch policy

The working target remains approximately **4,000 newly verified bytes per substantial batch**, with fidelity controlling. Analysis continues past individual dependency edges rather than treating them as stopping points.

## Unresolved queue

Fold the 1,250 staged bytes into `Code1.cpp`; lift `F007..F741` and `EDE7..F006`; identify the `[55C4]` indirect target contract; allocator bodies at `DAD9`/`DB5B`; candidate compressed-data routine `DB80`; transformation routine `DC90`; uncertain entry at `DE6F`; startup successor work; remaining address-named CRT helpers.

`CODE.1` remains controlling evidence whenever documentation and the actual bytes differ.
