# CODE.1 faithful decompilation progress

![Progress](https://img.shields.io/badge/verified%20conversion-10.40%25-red)

**Verified conversion:** 10.40%  
**Remaining:** 89.60%  
**Status color:** RED until verified conversion reaches exactly 100%; GREEN only at 100%.

## Measurement rule

The percentage is the conservative coverage floor: instruction-accounted translated bytes divided by the complete object-1 executable extent (`0x11302` bytes). Embedded data remains in the denominator until code/data separation is proven. Mapping, research, scaffolding, pseudocode, and unintegrated translations do not count.

Current verified translated bytes: **7,325**.  
Current conservative denominator: **70,402 (`0x11302`) bytes**.  
Raw coverage: **10.4045%**, displayed conservatively as **10.40%**.

## Immutable source identity

- File: `CODE.1`
- Size: `277,787` bytes (`0x43D1B`)
- SHA-256: `fd9b709ceee190e6a70870f9012ebea3bc437cae6bc74e0150d6f922fa750fed`

## Integrated verified translation inventory

- Startup/recovery regions through `CD96`: **259 bytes**.
- Structured mouse-facing routines `D08C..D153` plus overlap-safe copy `D1E8..D239`: **274 bytes**.
- File read/write/seek/delete `D5AD..D8CD`: **801 bytes**.
- DOS/CRT directory and utility routines `D8D0..DA1B`: **332 bytes**.
- Low-level/runtime regions through `E0C7`: **666 bytes**.
- Dispatcher and INT 8 paths `E3B4..E4C6`: **275 bytes**.
- Timer/PIT setup and state management `E4C7..E6E9`: **547 bytes**.
- Timer slot/control continuation `E6EA..EA0A`: **801 bytes**.
- Registration/event subsystem `EA0B..EBF2`: **488 bytes**.
- 48 exact dispatcher wrappers `EBF3..EDD2`: **480 bytes**.
- Frame no-op `EDE0..EDE6`: **7 bytes**.
- Complete formatter driver/parser/materializer `EDE7..F741`: **2,395 bytes**.

Total integrated verified translated bytes: **7,325**.

`EDD3..EDDF` is **13 zero bytes** classified as data and is not counted as translated code.

## Formatter fully integrated

Both cleared staging ranges are now physically folded into `Code1.cpp`: `EDE7..F006` (**544 bytes**) and `F206..F721` (**1,308 bytes**). With the previously integrated helpers, the contiguous formatter region `EDE7..F741` accounts for **2,395 verified bytes**.

The fold preserves the corrected post-`%` parser pointer, `%n` near/far word and dword stores, callback-owned output count, post-decrement padding/precision behavior, selector-qualified payload reads, two-byte `s/S` stride, signed 16.16 conversion, EAX:DX far-return contract, and the original shared `F001` epilogue relationship.

The staging files are removed after the syntax-checked fold so no verified bytes can be double-counted.

## Active continuation

Advance to the next independent CODE.1 regions: allocator bodies at `DAD9`/`DB5B`, candidate compressed-data routine `DB80`, transformation routine `DC90`, uncertain entry at `DE6F`, startup successors, and remaining address-named CRT helpers.

## Larger-batch policy

The working target remains approximately **4,000 newly verified bytes per substantial batch**, with fidelity controlling. Analysis continues past individual dependency edges rather than treating them as stopping points.

`CODE.1` remains controlling evidence whenever documentation and the actual bytes differ.
