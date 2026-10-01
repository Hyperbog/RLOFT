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

`DispatcherTimer.staging.cpp` now contains **1,250 instruction-accounted code bytes** awaiting fold-in to the single `Code1.cpp` reconstruction. They remain excluded from the percentage until integration.

- `E3B4..E4C6`: **275 bytes** — callback lookup, common dispatcher, INT 8 handler, PIC EOI path, and saved-vector chain behavior.
- `EA0B..EBF2`: **488 bytes** — descriptor/slot registration and clearing, events `64h`/`65h`, timer-backed event setup, pending-state release, and event `68h` dispatch.
- `EBF3..EDD2`: **480 bytes** — exactly 48 consecutive ten-byte wrappers, each encoded as `mov eax,<event>; jmp E3E0`.
- `EDE0..EDE6`: **7 bytes** — exact no-op-style frame routine.

The 48 wrapper event constants, in address order, are:

`78,79,86,7A,7B,85,7C,7D,7E,7F,80,81,82,83,84,96,97,98,99,9A,9B,9C,9D,9E,9F,AA,AB,AD,AE,AF,B0,B1,B2,B3,B4,B5,B6,B7,B9,BA,BB,BC,BD,BE,BF,C0,C1,C2` (hex).

`EDD3..EDDF` is **13 zero bytes** and is deliberately classified as data rather than code. It is not included in the 1,250-byte staging total.

The staging representation remains address-named where semantics are not proven. The INT 8 saved-vector path is represented as a chain result rather than modernized into a host timer, and the 48 wrappers preserve their encoded event ordering without assigning speculative event names.

## Active continuation

The next active routine starts at `EDE7`. Its larger formatted-output/runtime control flow and calls into `F007`/`F2E9` are being treated as separate dependency work. The immediate integration task is also to fold the 1,250 staged bytes into `Code1.cpp`, after which they can enter the official coverage count and the temporary staging file can be removed.

## Larger-batch policy

The working target remains approximately **4,000 newly verified bytes per substantial batch**, with fidelity controlling. Analysis continues past individual dependency edges rather than treating them as stopping points.

## Unresolved queue

Fold the 1,250 staged bytes into `Code1.cpp`; analyze `EDE7` and its `F007`/`F2E9` dependencies; allocator bodies at `DAD9`/`DB5B`; candidate compressed-data routine `DB80`; transformation routine `DC90`; uncertain entry at `DE6F`; startup successor work; remaining address-named CRT helpers.

`CODE.1` remains controlling evidence whenever documentation and the actual bytes differ.
