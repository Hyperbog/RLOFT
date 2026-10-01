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

`StartupNext.staging.cpp` contains **747 instruction-bounded candidate code bytes** from the startup gap immediately preceding the already integrated `D08C` routines. They remain uncounted pending final reconciliation.

- `CD97..CEBF`: **297 bytes** — PSP command-tail/environment processing, `no87=` detection, runtime initialization and the common process-exit/fatal continuation.
- `CEC0..CEC7`: **8 bytes** — CS-relative saved-DS restore helper.
- `CEC8..CEE8`: **33 bytes** — FEFC register adapter; its four stack arguments map to ESI/EDI/EDX/EBX and it returns dword `[third_argument]` after FEFC.
- `CEF0..CF40`: **81 bytes** — descriptor/register-image setup for the CEC8 service-31h adapter.
- `CF44..CF97`: **84 bytes** — 8A28 structure setup and status test.
- `CF98..D08B`: **244 bytes** — mouse initialization plus encoded no-mouse terminal fallback.

Boundary correction from direct CODE.1 disassembly: `CEE9..CEEF` is **7 zero bytes**, and `CF41..CF43` is a three-byte `LEA EAX,[EAX+0]` alignment NOP. Neither is counted as functional translated code.

### Newly reconciled startup details

Direct re-disassembly found and fixed another environment-tail fidelity error. At `CE11`, ESI points at the second NUL terminating the environment block. `CE16 LODSB` consumes that NUL and `CE17/CE18` increment ESI twice more, so the executable-path source begins **three bytes** beyond the second NUL. The earlier staging expression advanced only two bytes despite its comment saying +3. `StartupExit.reconciled.cpp` records the corrected `ESI += 3` behavior and the exact terminating-NUL copy semantics of `CE19..CE1D`.

The `CBF0 -> CE8B` termination relationship is now resolved from the bytes. Callers including `FCE2`, `D07C`, `97E0`, `5085`, and `50A4` push an exit value and JMP to `CBF0`. `CBF0` runs the callback chain, re-pushes `[EBP+4]`, creates its second frame, runs the second callback chain, pushes the same exit value again, and JMPs to `CE8B`. Thus `CE8B POP EAX` retrieves the original exit value exactly. It preserves that value around `FD2B`, executes `MOV AH,4Ch`, and terminates through INT 21h with AL as the status.

The alternate fatal entry at `CE8E` is also reconciled. `10C4D` proves its stack order with `PUSH 1; PUSH 390h; JMP CE8E`: the top value is the message pointer and the next value is the exit status. `CE8E` opens `CC92` (`"con"`) with AX=3D01h, scans the supplied NUL-terminated message, writes exactly `strlen(message)` bytes using AH=40h, restores the exit value, and falls into the common `CEB3..CEBC` cleanup/termination tail. `10479` reaches the same fatal entry through the stack-check trampoline.

`FC98` remains correctly classified as non-returning in the normal startup path: it computes runtime stack placement, initializes runtime state, calls `508C`, pushes EAX and tail-jumps to `CBF0`.

## Active continuation

Fold the newly proved environment-tail and exit-path corrections back into the main startup staging, then finish the remaining `CEF0..D08B` guest-stack/descriptor and mouse-runtime call-contract reconciliation. `CEC0..CEC7`, `CEC8..CEE8`, and the termination stack relationship now have direct cross-call evidence. Continue into independent CRT/runtime gaps if a remaining startup edge blocks; do not relax the instruction-level standard.

## Larger-batch policy

The working target remains approximately **4,000 newly verified bytes per substantial batch**, with fidelity controlling. Analysis continues past individual dependency edges rather than treating them as stopping points.

`CODE.1` remains controlling evidence whenever documentation and the actual bytes differ.
