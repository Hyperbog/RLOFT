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

## Current startup continuation staging

`StartupNext.staging.cpp` now structurally covers **750 instruction-bounded candidate code bytes**, all explicitly **uncounted** pending instruction-level reconciliation:

- `CD97..CEBF`: **297 bytes** — continuation of startup state capture, PSP command-tail copying, environment scan including the case-insensitive `no87=` test, executable-path copy, DGROUP clearing, runtime initializer calls, and encoded DOS fatal/exit path.
- `CEC0..CEC7`: **8 bytes** — restore-DS helper using the saved word at `CS:CC90`.
- `CEC8..CEE8`: **33 bytes** — register-adapter wrapper around `FEFC`, returning the dword addressed by its third stack argument.
- `CEF0..D08B`: **412 bytes** — descriptor/service setup followed by mouse/runtime initialization and the no-mouse fallback/shutdown path.

`CEE9..CEEF` is **7 zero bytes** and is currently treated as an alignment/data candidate rather than code.

The new staging is not yet eligible for the official percentage. The next hardening pass must reconcile the exact `84A0`, `FEFC`, `FCE8`, `FC98`, `FD2B`, `6F10`, `6F74`, and terminal `CBF0` call contracts; PSP/environment selector provenance; the `CE21` ESP capture; and the exact far/selector representation. No higher-level replacement is permitted where the encoded register/selector behavior matters.

## Recent integrated runtime batch

`DAD9..DDD7` is now physically integrated into `Code1.cpp` and counted. The final `DC90` correction models `REPNE SCASW` literally: compare at the initial EDI, advance EDI and decrement ECX per examined word, then apply the encoded `SUB EDI,2` to select the matched/final examined entry.

## Active continuation

Harden `CD97..D08B` instruction-by-instruction against fresh CODE.1 disassembly, clearing independently provable subranges rather than stopping on the first unresolved runtime call. Fold only fully reconciled code into `Code1.cpp`. Continue through the remaining startup/CRT gaps toward the existing `D08C` mouse routines and later untranslated regions.

## Larger-batch policy

The working target remains approximately **4,000 newly verified bytes per substantial batch**, with fidelity controlling. Analysis continues past individual dependency edges rather than treating them as stopping points.

## Continuity

Future conversations must read `Code1/DECOMPILATION_CONTINUITY.md` before resuming. It records the immutable-source rules, extraction/disassembly procedure, verification standard, GitHub integration process, primary documentation policy, and CODE.2 transition method.

`CODE.1` remains controlling evidence whenever documentation and the actual bytes differ.
