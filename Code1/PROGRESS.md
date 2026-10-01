# CODE.1 faithful decompilation progress

![Progress](https://img.shields.io/badge/verified%20conversion-7.77%25-red)

**Verified conversion:** 7.77%  
**Remaining:** 92.23%  
**Status color:** RED until verified conversion reaches exactly 100%; GREEN only at 100%.

## Measurement rule

The percentage is the conservative coverage floor: instruction-accounted translated bytes divided by the complete object-1 executable extent (`0x11302` bytes). Embedded data remains in the denominator until code/data separation is proven. Mapping, research, scaffolding, pseudocode, and unintegrated translations do not count.

Current verified translated bytes: **5,473**.  
Current conservative denominator: **70,402 (`0x11302`) bytes**.  
Raw coverage: **7.7739%**, displayed conservatively as **7.77%**.

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
- Formatter helper subset `F007..F205` and `F722..F741`: **543 bytes**.

Total integrated verified translated bytes: **5,473**.

`EDD3..EDDF` is **13 zero bytes** classified as data and is not counted as translated code.

## Formatter staging cleared for integration

Both remaining formatter staging files are now instruction-accounted and cleared:

- `FormatterDriver.staging.cpp`, `EDE7..F006`: **544 verified bytes**.
- `FormatterContinuation.staging.cpp`, `F206..F721`: **1,308 verified bytes**.

The final driver reconciliation found one structural-lift defect before clearance: at `EE75`, EAX is incremented before the `%` branch reaches `EE10`, so `F007` receives the byte immediately **after** `%`. The staging source previously passed the `%` address itself. This is corrected. The `%n` near/far word/dword stores, callback-owned count, pre/post padding loops, prefix and zero emission, selector-qualified payload reads, two-byte `s/S` stride, and shared `F001` epilogue were then rechecked against the immutable instruction stream.

Together these staged ranges add **1,852 verified bytes**. Folding them into `Code1.cpp` will make the complete formatter region `EDE7..F741` contiguous at **2,395 verified bytes** and raise the integrated total to **7,325 / 70,402 = 10.4045%**, displayed conservatively as **10.40%**.

The official percentage remains 7.77% until the physical fold is committed; verified-but-staged bytes are never counted early.

## Active continuation

Physically fold the two cleared formatter staging files into `Code1.cpp`, update the unified `FormatState` layout and verified-byte constant, syntax-check the resulting C++17 translation unit, remove the staging files, and then advance to the next independent CODE.1 regions.

## Larger-batch policy

The working target remains approximately **4,000 newly verified bytes per substantial batch**, with fidelity controlling. Analysis continues past individual dependency edges rather than treating them as stopping points.

`CODE.1` remains controlling evidence whenever documentation and the actual bytes differ.
