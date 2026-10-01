# CODE.1 faithful decompilation progress

![Progress](https://img.shields.io/badge/verified%20conversion-4.08%25-red)

**Verified conversion:** 4.08%  
**Remaining:** 95.92%  
**Status color:** RED until verified conversion reaches exactly 100%; GREEN only at 100%.

## Measurement rule

The percentage is the conservative coverage floor: instruction-accounted translated bytes divided by the complete object-1 executable extent (`0x11302` bytes). Embedded data remains in the denominator until code/data separation is proven. Mapping, research, scaffolding, pseudocode, and unverified translations do not count.

Current verified translated bytes: **2,879**.  
Current conservative denominator: **70,402 (`0x11302`) bytes**.  
Raw coverage: **4.0894%**, displayed conservatively as **4.08%**.

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

Total verified translated bytes: **2,879**.

## Current pass: E4C7..E6E9

This pass translates another **547 bytes** directly from CODE.1:

- `E4C7..E513` (77): initialize the 17-entry timer state arrays and the `51FE` sentinel while preserving the caller's interrupt-enable state.
- `E514..E562` (79): save the existing INT 8 vector, record the original-handler trampoline at `E4A7`, and install the `E3F2` handler.
- `E563..E59A` (56): mark the active timer index invalid and restore the saved INT 8 vector.
- `E59B..E5C8` (46): program PIT channel 0 with control byte `36h`, store the reload value at `5212`, and emit its low/high bytes to port `40h`.
- `E5C9..E5F9` (49): convert the supplied period with the original unsigned multiply/divide constants (`2710h` / `20BCh`) when below `D68Dh`, otherwise program a zero reload.
- `E5FA..E67F` (134): scan active timer slots for the unsigned minimum period, reprogram the PIT when that minimum changes, and clear the 17 accumulated counters.
- `E680..E6E9` (106): preserve the data selector and initialize the 16-entry callback/state arrays at `5216`, `5256`, and `5296`.

The original PUSHF/CLI/conditional-STI/POPF sequences are represented as explicit save/disable/restore operations; they are not replaced by host timer APIs. The DOS INT 8 vector operations remain explicit low-level boundaries.

## Larger-batch policy

The working target remains approximately **4,000 newly verified bytes per substantial batch**, with fidelity controlling. Timer routines after `E6E9` are being traced through their callback and slot-management dependencies rather than counted early.

## Unresolved queue

The timer callback/slot-management continuation after `E6E9`, the INT 8 handler body `E3F2..E4C6`, allocator bodies at `DAD9`/`DB5B`, the candidate compressed-data routine at `DB80`, the transformation routine at `DC90`, the uncertain entry at `DE6F`, startup successor work, and remaining address-named CRT helpers remain queued.

`CODE.1` remains controlling evidence whenever documentation and the actual bytes differ.
