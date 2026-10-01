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

## Newly traced continuation: EDE7..F006

The complete next routine is now instruction-bounded at **544 bytes** (`EDE7..F006`). It is not yet counted as translated code.

The routine is a formatted-output control core: it keeps a 0x50-byte local frame, walks a format string, sends literal bytes through an indirect output callback, invokes `F007` when `%` is encountered to parse conversion state, handles `%n` storage in near/far and word/dword forms according to flag bits, invokes `F2E9` to materialize a conversion into the local buffer, emits left/right padding, and has separate byte-step and two-byte-step output paths for `s`/`S`. The return value comes from the local counter at `[EBP-18h]`.

Important fidelity points established directly from CODE.1:

- `%n` storage distinguishes flag `20h`, `10h`, `80h`, and `40h`; far-pointer forms use `LES` and ES-qualified stores.
- The `%n` paths consume either four or eight bytes from the argument cursor exactly as encoded.
- Literal output and padding call the function pointer supplied in the routine's fifth incoming argument.
- Conversion formatting depends on `F2E9`; conversion-state parsing depends on `F007`. Those dependencies must be lifted before `EDE7..F006` can be counted without replacing original behavior with a guessed libc formatter.
- The `s`/`S` branch advances the formatted source by one byte normally and two bytes when flag `20h` is set.

## Active continuation

Next dependency work is `F007` followed by `F2E9`, then lifting `EDE7..F006` against those proven helpers. In parallel, the 1,250 staged bytes remain queued for fold-in to `Code1.cpp`; they will enter the official percentage only after that integration and verification.

## Larger-batch policy

The working target remains approximately **4,000 newly verified bytes per substantial batch**, with fidelity controlling. Analysis continues past individual dependency edges rather than treating them as stopping points.

## Unresolved queue

Fold the 1,250 staged bytes into `Code1.cpp`; lift `F007`, `F2E9`, and then `EDE7..F006`; allocator bodies at `DAD9`/`DB5B`; candidate compressed-data routine `DB80`; transformation routine `DC90`; uncertain entry at `DE6F`; startup successor work; remaining address-named CRT helpers.

`CODE.1` remains controlling evidence whenever documentation and the actual bytes differ.
