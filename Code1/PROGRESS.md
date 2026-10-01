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

`StartupNext.staging.cpp` contains **747 instruction-bounded candidate code bytes** from `CD97..D08B`, excluding the seven zero bytes at `CEE9..CEEF` and the three-byte alignment NOP at `CF41..CF43`. They remain uncounted pending final reconciliation.

The PSP command-tail `REPZ SCASB` mismatch behavior, environment executable-path `ESI += 3` transition, FEFC/FF33 adapter mapping, `FC98` non-returning launch path, no-mouse unconditional dereference, and `CBF0 -> CE8B` termination stack relationship have all been reconciled directly against CODE.1. `StartupExit.reconciled.cpp` retains the detailed exit-path evidence.

## DOS/runtime-memory continuation under reconciliation

`DosMemoryNext.staging.cpp` contains **186 functional instruction-bounded bytes** from `DA1C..DAD8`; `DA45..DA47` is three zero bytes and excluded.

A direct register-flow audit corrected the AH=48/AH=4A failure outputs. In `DA48`, DOS receives the request in BX, then `XCHG EDX,EBX` after INT 21h transfers the DOS-returned BX to EDX while restoring the output pointer to EBX. Therefore on carry the output is DOS's returned BX (largest available block), not the original request. `DA7E` likewise executes `MOV EDX,EBX` immediately after AH=4A and stores that returned BX through arg3 on carry. `10384` consumes the still-live AX+CF state.

## Mouse/event-pump continuation under reconciliation

`MousePumpNext.staging.cpp` adds **145 instruction-bounded functional bytes** between the already integrated `D08C..D153` and `D1E8..D239` regions. `D179..D17B` is the three-byte `LEA EAX,[EAX+0]` alignment NOP and is excluded.

- `D154..D178`: **37 bytes** — if `84D0` is active, repeatedly obtains the pointer from `D0B4`, calls `8960`, and tests the preserved pointer's first word until it becomes zero.
- `D17C..D1E7`: **108 bytes** — drains the same loop, then coordinates repeated `82A4`, `D0B4`, and `8960` calls. A nonzero final `82A4` invokes `82EC` then `8304`; the inactive `84D0` path calls `82C0`.

## D23A..D5AC DOS/CRT continuation under reconciliation

`DosCrtNext.staging.cpp` now covers the complete address gap from the end of the integrated overlap-copy helper to the beginning of the integrated file read/write/seek/delete block. The raw span is **883 bytes**; `D27D..D27F` is three zero bytes, leaving **880 instruction-bounded functional bytes** staged and uncounted.

Direct disassembly established these boundaries: `D23A..D27C` attribute/status wrapper; `D280..D2AC` find-first; `D2AD..D2CC` find-next; `D2CD..D312` mode-9 DTA shuttles; `D313..D331` variadic open front-end; `D332..D4F1` open/create/truncate/descriptor-state machine; `D4F2..D55D` descriptor/IOCTL mode setup; `D55E..D572` open wrapper; `D573..D5AC` close wrapper.

Important verified details include the RCL/ROR carry-to-sign encoding used after DOS calls, the explicit AX+CF contract of `10384`, the create-on-error-2 path, the zero-length AH=40 truncation path, descriptor state assembly through `103C3/10397/10415`, and the close failure behavior that stores runtime errno 4 rather than passing DOS AX through `10314`.

The large `D332..D4F1` routine remains structural staging rather than counted translation until its descriptor-table representation is reconciled against `103C3..1042C`; no host `open()`/`fopen()` semantics have been substituted.

## Active continuation

Harden the now-mapped `D23A..D5AC` block against the exact descriptor helpers at `10397..1042C`, then fold independently proven subranges instead of waiting for the entire 448-byte open state machine if it remains blocked. In parallel continue startup, mouse-pump, and DOS-memory reconciliation. Current uncounted instruction-bounded staging totals 1,958 bytes across these four work streams; none affects the official percentage until physically integrated and syntax checked.

## Larger-batch policy

The working target remains approximately **4,000 newly verified bytes per substantial batch**, with fidelity controlling. Analysis continues past individual dependency edges rather than treating them as stopping points.

`CODE.1` remains controlling evidence whenever documentation and the actual bytes differ.
