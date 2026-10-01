# CODE.1 faithful decompilation progress

![Progress](https://img.shields.io/badge/verified%20conversion-1.89%25-red)

**Verified conversion:** 1.89%  
**Remaining:** 98.11%  
**Status color:** RED until verified conversion reaches exactly 100%; GREEN only at 100%.

## Measurement rule

The percentage remains the conservative coverage floor: instruction-accounted translated bytes divided by the complete object-1 executable extent (`0x11302` bytes). Embedded data remains in the denominator until code/data separation is proven. Mapping, research, scaffolding, pseudocode, and unverified translations do not count.

Current verified translated bytes: **1,334**.  
Current conservative denominator: **70,402 (`0x11302`) bytes**.  
Raw coverage: **1.8948%**, displayed conservatively as **1.89%**.

## Immutable source identity

- File: `CODE.1`
- Size: `277,787` bytes (`0x43D1B`)
- SHA-256: `fd9b709ceee190e6a70870f9012ebea3bc437cae6bc74e0150d6f922fa750fed`

## Verified translation inventory

- Startup/recovery regions through `CD96`: **259 bytes**.
- Structured mouse-facing routines `D08C..D153` plus overlap-safe copy `D1E8..D239`: **274 bytes**.
- Structured file read `D5AD..D685`: **217 bytes**.
- Structured file write `D686..D85E`: **473 bytes**.
- Structured file seek `D85F..D8A9`: **75 bytes**.
- Structured file delete `D8AA..D8CD`: **36 bytes**.

Total verified translated bytes: **1,334**.

## Larger-batch policy

The working target is approximately **4,000 newly verified bytes per batch**, but an obstacle in one region no longer terminates the batch. Unresolved call edges are recorded and translation continues in other independently provable regions. Fidelity remains the hard constraint: no byte is counted merely to satisfy the target.

This checkpoint adds **801 verified bytes** beyond the previous 533-byte total. The newly lifted region is particularly useful because it is ordinary structured C++ rather than CPU-state emulation: the original DOS file read/write/seek/delete behavior is represented behind explicit native boundaries, while still-unproven Watcom runtime helpers remain address-named (`10314`, `103B7`, `103C3`, `1042D`, `1046F`).

### File-read fidelity (`D5AD..D685`)

The reconstruction preserves the original handle-flag tests, direct/binary read path, text-mode CR removal, `1Ah` termination behavior, repeated reads, and runtime error boundaries.

### File-write fidelity (`D686..D85E`)

The reconstruction preserves append seeking, direct/binary writes, text-mode LF-to-CR/LF expansion, the original `0x80`/`0x200` temporary-buffer selection, short-write error handling, and the original return-accounting behavior.

### Seek/delete (`D85F..D8CD`)

The DOS `AH=42h` seek and `AH=41h` delete routines are lifted behind explicit file-system boundaries, retaining the original runtime error-conversion boundary rather than inventing host errno behavior.

## Unresolved queue

Nearby unresolved dependencies such as `84D0`, `8960`, `82A4`, and the address-named CRT helpers remain queued. They do **not** prevent work on independent regions.

## Next checkpoint

Continue the same batch strategy toward approximately 4,000 additional verified bytes, moving among independently provable regions rather than stopping at the first unresolved edge.

`CODE.1` remains controlling evidence whenever documentation and the actual bytes differ.
