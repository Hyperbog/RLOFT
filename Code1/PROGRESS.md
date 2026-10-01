# CODE.1 faithful decompilation progress

![Progress](https://img.shields.io/badge/verified%20conversion-5.22%25-red)

**Verified conversion:** 5.22%  
**Remaining:** 94.78%  
**Status color:** RED until verified conversion reaches exactly 100%; GREEN only at 100%.

## Measurement rule

The percentage is the conservative coverage floor: instruction-accounted translated bytes divided by the complete object-1 executable extent (`0x11302` bytes). Embedded data remains in the denominator until code/data separation is proven. Mapping, research, scaffolding, pseudocode, and unverified translations do not count.

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

Total verified translated bytes: **3,680**.

## Current verified checkpoint: E6EA..EA0A

This checkpoint adds **801 bytes** translated directly from CODE.1 after checking the complete instruction ranges against the extracted object-1 disassembly.

- `E6EA..E758` (111): iterate all 16 callback slots under CLI, release assigned timer handles, invoke the still-address-named `EBB6` boundary, and perform all-slot timer cleanup.
- `E759..E808` (176): allocate one of 16 timer slots, initialize its state/value/period entries, and execute the first-allocation initialization sequence that installs INT 8 and initializes special slot 16.
- `E809..E855` (77): release one timer slot and, when the active count reaches zero, program PIT reload zero and restore the saved INT 8 vector.
- `E856..E87A` (37): release slots 15 down through 0.
- `E87B..E8AA` (48), `E8AB..E8CF` (37): transition state `1 -> 2` for one slot or all 16 slots.
- `E8D0..E8FF` (48), `E900..E924` (37): transition state `2 -> 1` for one slot or all 16 slots.
- `E925..E979` (85): temporarily force a slot active, replace its period, clear its accumulated counter, recalculate the global minimum, then restore the original state word.
- `E97A..E9AC` (51): compute unsigned `1,000,000 / divisor` and apply it through `E925`; the original division-by-zero behavior remains unguarded.
- `E9AD..E9F1` (69): use `D68Dh` for zero, otherwise compute unsigned `(2710h * value) / 2E9Ch`, then apply it through `E925`.
- `E9F2..EA0A` (25): return the stored PIT reload value at `5212` while preserving interrupt-enable state.

The C++ retains the original data-group offsets, unsigned arithmetic, slot ordering, sentinel values, and interrupt save/CLI/restore behavior. `E6EA`'s call into `EBB6` remains an explicit address-named boundary rather than assigning an unproven higher-level meaning.

A previously misleading local name in `D686` was also corrected: helper `1042D` returns a stack-distance value, not a version number. This correction changes no verified-byte count.

## Next analyzed continuation

`EA0B..EBF2` is already instruction-bounded as **488 candidate bytes**. It remains outside the verified percentage until its C++ lifting is checked. The common callback dispatcher/INT 8 path at `E3B4..E4C6` is also queued because it resolves dependencies used by this timer subsystem.

## Larger-batch policy

The working target remains approximately **4,000 newly verified bytes per substantial batch**, with fidelity controlling. Analysis continues past individual dependency edges rather than treating them as stopping points.

## Unresolved queue

`EA0B..EBF2`, common dispatcher/INT 8 routines `E3B4..E4C6`, allocator bodies at `DAD9`/`DB5B`, candidate compressed-data routine `DB80`, transformation routine `DC90`, uncertain entry at `DE6F`, startup successor work, and remaining address-named CRT helpers remain queued.

`CODE.1` remains controlling evidence whenever documentation and the actual bytes differ.
