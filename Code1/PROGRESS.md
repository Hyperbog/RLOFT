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

`DispatcherTimer.staging.cpp` now contains **763 instruction-accounted bytes** awaiting fold-in to the single `Code1.cpp` reconstruction. They are deliberately excluded from the percentage until integration.

- `E3B4..E3DF` (44): callback-table lookup for one of 16 slots.
- `E3E0..E3F1` (18): common event lookup and indirect tail-dispatch.
- `E3F2..E4C6` (213): INT 8 handler, including reentrancy handling, 17 timer states, accumulator/period update, callback invocation, local PIC EOI path, and saved-vector chain path.
- `EA0B..EA99` (143): free-slot scan, literal `79706F43h` (`Copy`) descriptor check, slot installation, EAC8 call, and unsigned returned-record bound check against `52E8h`.
- `EA9A..EAC7` (46): slot-table clear for indices below 16.
- `EAC8..EAE6` (31): event `64h` dispatch with original `{slot,E9F2h}` stack arguments.
- `EAE7..EAF0` (10): event `65h` tail-dispatch wrapper.
- `EAF1..EBB5` (197): slot setup/control, event `67h` timer callback lookup, timer allocation/rate setup, event `66h` five-argument dispatch, pending-state set, and timer enable.
- `EBB6..EBF2` (61): pending-state exchange/clear, timer release, and event `68h` tail-dispatch.

The staging representation remains address-named where semantics are not proven. In particular, the INT 8 saved-vector path is represented as a chain result rather than being modernized into a host timer.

## Newly bounded continuation

`EBF3..EDD2` is a contiguous **480-byte** block of exactly **48 ten-byte tail-dispatch wrappers**. Each is `mov eax,<event>; jmp E3E0`. The encoded event constants, in address order, are:

`78,79,86,7A,7B,85,7C,7D,7E,7F,80,81,82,83,84,96,97,98,99,9A,9B,9C,9D,9E,9F,AA,AB,AD,AE,AF,B0,B1,B2,B3,B4,B5,B6,B7,B9,BA,BB,BC,BD,BE,BF,C0,C1,C2` (hex).

`EDD3..EDDF` is **13 zero bytes** and is therefore not being treated as executable code. `EDE0..EDE6` is a separate 7-byte no-op-style routine (`push ebp; mov ebp,esp; mov esp,ebp; pop ebp; ret`). `EDE7` begins a substantially larger formatted-output/runtime routine and is being analyzed separately rather than guessed from its apparent behavior.

## Larger-batch policy

The working target remains approximately **4,000 newly verified bytes per substantial batch**, with fidelity controlling. Analysis continues past individual dependency edges rather than treating them as stopping points.

## Unresolved queue

Fold the 763 staged bytes into `Code1.cpp`; translate/account the 480-byte `EBF3..EDD2` wrapper block; analyze the `EDE7` runtime routine and its `F007`/`F2E9` dependencies; allocator bodies at `DAD9`/`DB5B`; candidate compressed-data routine `DB80`; transformation routine `DC90`; uncertain entry at `DE6F`; startup successor work; remaining address-named CRT helpers.

`CODE.1` remains controlling evidence whenever documentation and the actual bytes differ.
