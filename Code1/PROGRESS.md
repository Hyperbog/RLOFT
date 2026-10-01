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

Remaining startup hardening is concentrated in the `CEF0..D08B` descriptor/mouse-runtime contracts before physical fold-in.

## DOS/runtime-memory continuation under reconciliation

`DosMemoryNext.staging.cpp` now closes the address gap immediately between the integrated `D8D0..DA1B` helpers and integrated `DAD9` allocator path. It contains **186 functional instruction-bounded bytes**; `DA45..DA47` is three zero bytes classified as data/alignment and is excluded.

- `DA1C..DA44`: **41 bytes** — initializes the 12-byte `104B1` adapter cell and calls the already cross-checked `CEC8` FEFC adapter.
- `DA48..DA6B`: **36 bytes** — DOS INT 21h/AH=48 allocation wrapper. Success writes returned AX through arg2 and returns zero; failure preserves the requested paragraph count for the output and enters `10384` with carry set.
- `DA6C..DA7D`: **18 bytes** — DOS AH=49 free wrapper followed by unconditional `10384` while the DOS carry state is live.
- `DA7E..DAA5`: **40 bytes** — DOS AH=4A resize wrapper; on carry only it writes the requested paragraph count through arg3, then calls `10384` with the unchanged carry state.
- `DAA6..DAC8`: **35 bytes** — unsigned maximum scan of the arena list rooted at `[30DC]`, ending at sentinel `30D4`, using `dword[node]-4` and `node=[node+8]` exactly.
- `DAC9..DAD8`: **16 bytes** — ordinary wrapper into the already integrated `DAD9` allocator.

Direct inspection of `10384` confirmed that it begins with `JAE`: the carry flag produced by the preceding DOS interrupt is an explicit input to that helper. The staging boundary therefore carries both AX and CF rather than reducing the helper to an AX-only error mapper.

## Active continuation

Continue hardening the 747-byte startup batch, while independently reconciling the new 186-byte `DA1C..DAD8` bridge. The latter is structurally contiguous with already integrated code on both sides and is a strong near-term integration candidate once `104B1` and the DOS carry/error boundary are fully reconciled. Continue through other independent CRT/runtime gaps instead of stopping on isolated dependencies.

## Larger-batch policy

The working target remains approximately **4,000 newly verified bytes per substantial batch**, with fidelity controlling. Analysis continues past individual dependency edges rather than treating them as stopping points.

`CODE.1` remains controlling evidence whenever documentation and the actual bytes differ.
