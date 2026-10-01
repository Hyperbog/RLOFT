# CODE.1 faithful decompilation progress

![Progress](https://img.shields.io/badge/verified%20conversion-2.36%25-red)

**Verified conversion:** 2.36%  
**Remaining:** 97.64%  
**Status color:** RED until verified conversion reaches exactly 100%; GREEN only at 100%.

## Measurement rule

The percentage is the conservative coverage floor: instruction-accounted translated bytes divided by the complete object-1 executable extent (`0x11302` bytes). Embedded data remains in the denominator until code/data separation is proven. Mapping, research, scaffolding, pseudocode, and unverified translations do not count.

Current verified translated bytes: **1,666**.  
Current conservative denominator: **70,402 (`0x11302`) bytes**.  
Raw coverage: **2.3664%**, displayed conservatively as **2.36%**.

## Immutable source identity

- File: `CODE.1`
- Size: `277,787` bytes (`0x43D1B`)
- SHA-256: `fd9b709ceee190e6a70870f9012ebea3bc437cae6bc74e0150d6f922fa750fed`

## Verified translation inventory

- Startup/recovery regions through `CD96`: **259 bytes**.
- Structured mouse-facing routines `D08C..D153` plus overlap-safe copy `D1E8..D239`: **274 bytes**.
- File read/write/seek/delete `D5AD..D8CD`: **801 bytes**.
- DOS/CRT directory and utility routines `D8D0..DA1B`: **332 bytes** of non-padding executable ranges.

Total verified translated bytes: **1,666**.

## Current pass: D8D0..DA1B

This pass continued past unresolved call edges and verified another **332 bytes** directly from CODE.1:

- `D8D0..D8E7` (24): query current DOS drive and store its one-based representation.
- `D8E8..D905` (30): select a drive after converting the one-based argument to DOS's zero-based drive number.
- `D906..D990` (139): obtain the current directory and construct the drive-qualified path, allocating storage only when the supplied destination is null.
- `D991..D9A5` (21): ASCII lowercase-to-uppercase conversion.
- `D9A6..D9C9` (36): DOS change-directory wrapper with the existing runtime error-conversion boundary.
- `D9CA..D9ED` (36): DOS make-directory wrapper with the same error boundary.
- `D9EE..DA17` (42): forward copy routine, represented as `memcpy`; unlike the previously recovered `D1E8` routine it contains no overlap-safe backward path.
- `DA18..DA19` and `DA1A..DA1B` (2 bytes each): CLI/STI wrappers.

The bytes `DA45..` were not automatically counted merely because they follow these routines; alignment/data ambiguity and subsequent runtime dependencies are being resolved before coverage is assigned.

Open Watcom's preserved C Library Reference independently identifies `chdir`, `getcwd`, and `mkdir` as directory-manipulation functions. That documentation is corroboration only; CODE.1's instructions and DOS interrupt behavior remain controlling evidence.

## Larger-batch policy

The working target remains approximately **4,000 newly verified bytes per substantial batch**, but fidelity is controlling. A blocked edge is queued and work moves to another independently provable region. A region is counted only after its boundaries and behavior are instruction-accounted.

## Unresolved queue

The existing `84D0`, `8960`, `82A4`, startup successor work, and address-named CRT helpers remain queued. The next nearby allocator/runtime region beginning around `DA45` is also being treated cautiously rather than receiving guessed host-allocation semantics.

`CODE.1` remains controlling evidence whenever documentation and the actual bytes differ.
