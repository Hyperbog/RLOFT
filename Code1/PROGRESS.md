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

## Current verified checkpoint: E4C7..E6E9

This checkpoint translates **547 bytes** directly from CODE.1: timer-array initialization, INT 8 vector save/install/restore, PIT channel-0 programming, period conversion, minimum-period recalculation, and initialization of the 16-entry callback/state arrays. The original interrupt-enable preservation remains explicit.

## Continuation analysis after E6E9

The next contiguous timer/control region has now been instruction-bounded through `EA0A`. These bytes are deliberately **not** included in the verified percentage until their C++ lifting and instruction check are complete.

Established routine boundaries and sizes:

- `E6EA..E758` (111): iterate 16 callback slots under CLI; release any assigned timer handle through `E809`, dispatch through `EBB6`, then call the all-slot cleanup at `E856`.
- `E759..E808` (176): allocate one of 16 timer slots. It scans the word state table at `5154`, installs the supplied callback at `5110`, initializes `51BA`, increments the active count at `510C`, and on the first allocation performs the timer initialization / INT 8 installation / slot-16 setup sequence.
- `E809..E855` (77): release one timer slot; when the active count reaches zero it programs PIT reload zero and restores the saved INT 8 vector.
- `E856..E87A` (37): release slots 15 down through 0.
- `E87B..E8AA` (48) and `E8AB..E8CF` (37): change state `1 -> 2` for one slot or all 16 slots.
- `E8D0..E8FF` (48) and `E900..E924` (37): change state `2 -> 1` for one slot or all 16 slots.
- `E925..E979` (85): temporarily force a slot active, replace its period at `51BA`, clear its accumulated count at `5176`, recalculate the global minimum via `E5FA`, then restore the prior slot state.
- `E97A..E9AC` (51): convert `1,000,000 / argument` and pass the result to `E925`.
- `E9AD..E9F1` (69): alternate period conversion; zero selects `D68Dh`, otherwise computes `(2710h * argument) / 2E9Ch`, then calls `E925`.
- `E9F2..EA0A` (25): return the current PIT reload value stored at `5212` while preserving the interrupt-enable state.

These boundaries account for **801 candidate code bytes**. They are not counted yet because the fidelity rule requires the lifted C++ to be checked before coverage changes.

The dependency trace also now reaches `EA0B..EBF2`. In particular, `EBB6..EBF2` atomically clears the per-slot pending flag at `5296`, releases an assigned timer handle from `5256` when present, and dispatches event `68h` through the common `E3E0` dispatcher. This resolves the main dependency used by `E6EA` without assigning higher-level game semantics that the bytes do not prove.

## Larger-batch policy

The working target remains approximately **4,000 newly verified bytes per substantial batch**, with fidelity controlling. Analysis continues past individual dependency edges rather than treating them as stopping points.

## Unresolved queue

The C++ lifting/check of `E6EA..EA0A`, common dispatcher routines `E3B4..E4C6`, continuation `EA0B..EBF2`, allocator bodies at `DAD9`/`DB5B`, the candidate compressed-data routine at `DB80`, transformation routine at `DC90`, uncertain entry at `DE6F`, startup successor work, and remaining address-named CRT helpers remain queued.

`CODE.1` remains controlling evidence whenever documentation and the actual bytes differ.
