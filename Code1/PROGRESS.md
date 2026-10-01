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

`StartupNext.staging.cpp` contains **747 instruction-bounded candidate code bytes** immediately preceding the already integrated `D08C` routines. They remain uncounted pending final reconciliation.

- `CD97..CEBF`: **297 bytes** — PSP command-tail/environment processing, `no87=` detection, runtime initialization and common process-exit/fatal continuation.
- `CEC0..CEC7`: **8 bytes** — CS-relative saved-DS restore helper.
- `CEC8..CEE8`: **33 bytes** — FEFC register adapter; four dword arguments map to ESI/EDI/EDX/EBX and the wrapper returns dword `[third_argument]` after FEFC.
- `CEF0..CF40`: **81 bytes** — service-31h register-image/far-cell setup.
- `CF44..CF97`: **84 bytes** — 8A28 structure setup and status test.
- `CF98..D08B`: **244 bytes** — mouse initialization plus encoded no-mouse terminal fallback.

`CEE9..CEEF` is **7 zero bytes**. `CF41..CF43` is `LEA EAX,[EAX+0]`, a three-byte alignment NOP. Neither is counted as functional candidate code.

### Reconciliation findings

The PSP command-tail lift was corrected after literal reconciliation of `CDB2..CDE3`. `REPZ SCASB` consumes the first non-space byte as well as the preceding spaces: EDI advances and ECX decrements for that comparison. The following `LEA ESI,[EDI-1]` points back to that consumed byte, and `INC ECX` restores it to the subsequent `REP MOVSB` count. The earlier structural lift failed to advance over the mismatch and was non-faithful.

`CEC8` was cross-checked against `FEFC..FF56` and independent call sites at `526E`, `7173`, and `DA3B`. Its argument-to-register mapping is confirmed as ESI/EDI/EDX/EBX, and the post-FEFC return load is from `[EDX]` in the restored data selector. The staging boundary now carries the actual DS selector instead of using selector zero.

`CEF0` was rechecked byte-for-byte. `84A0` clears a 12-byte local far cell; the register image is seeded with EAX=`0300h`, EBX=`33h`, ECX=0, and the caller pointer at image offset `14h`. The far cell receives the current DS selector. CEC8 is then invoked with service `31h`, EDI=EDX=register-image address and EBX=far-cell address. The prior staging path that supplied selector zero to CEF0 was corrected.

`FC98` remains a non-returning runtime-launch path: it computes runtime stack placement, calls `10D1C`, invokes `508C`, then tail-jumps to `CBF0`. The `CF98` no-mouse branch likewise preserves its encoded unconditional `[2AA0]+8` dereference and terminal `8424; PUSH 1; JMP CBF0` path.

## Active continuation

Finish the DOS `CE8B..CEBD` exit/fatal boundary and the exact `CBF0 -> CE8B` stack relationship, then determine whether the reconciled startup subranges can be folded independently rather than waiting on the entire 747-byte block. Continue through independent remaining CRT/runtime gaps in parallel.

## Larger-batch policy

The working target remains approximately **4,000 newly verified bytes per substantial batch**, with fidelity controlling. Analysis continues past individual dependency edges rather than treating them as stopping points.

`CODE.1` remains controlling evidence whenever documentation and the actual bytes differ.
