# CODE.1 faithful decompilation progress

![Progress](https://img.shields.io/badge/verified%20conversion-11.48%25-red)

**Verified conversion:** 11.48%  
**Remaining:** 88.52%  
**Status color:** RED until verified conversion reaches exactly 100%; GREEN only at 100%.

## Measurement rule

The percentage is the conservative coverage floor: instruction-accounted translated bytes divided by the complete object-1 executable extent (`0x11302` bytes). Embedded data remains in the denominator until code/data separation is proven. Mapping, research, scaffolding, pseudocode, and unintegrated translations do not count.

Current verified translated bytes: **8,089**.  
Current conservative denominator: **70,402 (`0x11302`) bytes**.  
Raw coverage: **11.4897%**, displayed conservatively as **11.48%**.

## Immutable source identity

- File: `CODE.1`
- Size: `277,787` bytes (`0x43D1B`)
- SHA-256: `fd9b709ceee190e6a70870f9012ebea3bc437cae6bc74e0150d6f922fa750fed`

## Integrated verified translation inventory

- Startup/recovery regions through `CD96`: **259 bytes**.
- Structured mouse-facing routines `D08C..D153` plus overlap-safe copy `D1E8..D239`: **274 bytes**.
- File read/write/seek/delete `D5AD..D8CD`: **801 bytes**.
- DOS/CRT directory and utility routines `D8D0..DA1B`: **332 bytes**.
- Runtime allocator/decoder/transform `DAD9..DDD7`: **764 bytes**.
- Low-level/runtime regions through `E0C7`: **666 bytes**.
- Dispatcher and INT 8 paths `E3B4..E4C6`: **275 bytes**.
- Timer/PIT setup and state management `E4C7..E6E9`: **547 bytes**.
- Timer slot/control continuation `E6EA..EA0A`: **801 bytes**.
- Registration/event subsystem `EA0B..EBF2`: **488 bytes**.
- 48 exact dispatcher wrappers `EBF3..EDD2`: **480 bytes**.
- Frame no-op `EDE0..EDE6`: **7 bytes**.
- Complete formatter driver/parser/materializer `EDE7..F741`: **2,395 bytes**.

Total integrated verified translated bytes: **8,089**.

`EDD3..EDDF` is **13 zero bytes** classified as data and is not counted as translated code.

## Startup continuation under reconciliation

`StartupNext.staging.cpp` now contains **747 instruction-bounded candidate code bytes** from the startup gap immediately preceding the already integrated `D08C` routines. They remain uncounted pending final reconciliation.

- `CD97..CEBF`: **297 bytes** — PSP command-tail/environment processing, `no87=` detection, runtime initialization and the common process-exit/fatal continuation.
- `CEC0..CEC7`: **8 bytes** — CS-relative saved-DS restore helper.
- `CEC8..CEE8`: **33 bytes** — FEFC register adapter; its four stack arguments map to ESI/EDI/EDX/EBX and it returns dword `[third_argument]` after FEFC.
- `CEF0..CF40`: **81 bytes** — descriptor/register-image setup for the CEC8 service-31h adapter.
- `CF44..CF97`: **84 bytes** — 8A28 structure setup and status test.
- `CF98..D08B`: **244 bytes** — mouse initialization plus encoded no-mouse terminal fallback.

Boundary correction from direct CODE.1 disassembly: `CEE9..CEEF` is **7 zero bytes**, and `CF41..CF43` is a three-byte `LEA EAX,[EAX+0]` alignment NOP. The earlier 750-byte staging count incorrectly included that alignment NOP as functional candidate code; the candidate count is therefore corrected to 747 without affecting official coverage.

The startup model was also corrected in two important ways. First, `FC98` is not treated as an ordinary returning initializer: it computes runtime stack placement, initializes runtime state, calls `508C`, then tail-jumps to `CBF0`, which eventually jumps into the common `CE8B` exit continuation. Second, the no-mouse `CF98` branch now preserves the encoded unconditional `[2AA0]+8` dereference instead of adding a non-existent null guard.

## Active continuation

Continue hardening `CD97..D08B`, especially the `FEFC/FF33` register-image contract, CEF0 guest-stack/far-cell layout, DOS exit/fatal path, and the terminal `CBF0 -> CE8B` relationship. Fold only after instruction-level reconciliation. Then continue through independent remaining startup/CRT gaps rather than stopping on an isolated runtime dependency.

## Larger-batch policy

The working target remains approximately **4,000 newly verified bytes per substantial batch**, with fidelity controlling. Analysis continues past individual dependency edges rather than treating them as stopping points.

`CODE.1` remains controlling evidence whenever documentation and the actual bytes differ.
