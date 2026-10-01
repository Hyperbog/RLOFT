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

`DosCrtNext.staging.cpp` covers **880 functional instruction bytes** in `D23A..D5AC`; `D27D..D27F` is three zero bytes and excluded. It maps the attribute/status wrapper, find-first/find-next, DTA shuttles, variadic open front-end, open/create/truncate state machine, descriptor/IOCTL setup, open wrapper, and close wrapper. The large `D332..D4F1` routine remains structural staging until its descriptor-table representation is reconciled against `103C3..1042C`.

## E0D0..E3B2 runtime/interrupt continuation under reconciliation

`RuntimeGapE0D0.staging.cpp` adds **722 functional instruction bytes** between the integrated low-level/runtime block ending at `E0C7` and the integrated dispatcher block beginning at `E3B4`.

Direct binary boundaries are: `E0D0..E173` 164 bytes; `E174..E186` 19; `E190..E1F0` 97; `E1F4..E206` 19; `E208..E268` 97; `E26C..E27E` 19; `E280..E2C2` 67; `E2C3..E35F` 157; and `E360..E3B2` 83. Excluded are eight zeros at `E0C8..E0CF`, nine zeros at `E187..E18F`, alignment LEAs at `E1F1..E1F3` and `E269..E26B`, the single NOP at `E207`, and the zero byte at `E27F`.

The first routine copies an encoded variable-sized object after allocating `0x1B + dword[src+16h]`; three address-distinct wrappers call `7240`; two 97-byte constructor-like routines read a 16-bit length, allocate length+2 and invoke the `72D0/72E0/746C` chain. `E280` installs the INT 24h handler using the extender-specific AX=2506h/CL=24h form only for runtime modes 2..4. `E2C3` is an IRET trampoline that saves the full register/segment image, switches to the runtime stack, far-calls the saved callback, restores SS:ESP with LSS, and IRETs. `E360` is the 83-byte signed decimal parser using class-table bits 02h and 20h at `03B4`.

The interrupt trampoline remains explicitly machine-boundary staging; it will not be collapsed into an ordinary host callback.

## Active continuation

Harden independently provable staged subranges and fold them only after exact call-contract reconciliation and unified syntax checking. Current uncounted instruction-bounded staging totals **2,680 bytes** across startup, DOS-memory, mouse-pump, DOS/CRT, and E0D0..E3B2 runtime/interrupt work streams. Continue past isolated blockers toward the approximately 4 KB batch target without relaxing fidelity.

## Larger-batch policy

The working target remains approximately **4,000 newly verified bytes per substantial batch**, with fidelity controlling. Analysis continues past individual dependency edges rather than treating them as stopping points.

`CODE.1` remains controlling evidence whenever documentation and the actual bytes differ.
