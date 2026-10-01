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

## Current formatter continuation staging

`FormatterContinuation.staging.cpp` contains **227 verified instruction-accounted bytes**, still excluded from the official percentage until fold-in:

- `F206..F2D1`: **204 bytes** — signed 16.16 fixed-point decimal materialization and encoded rounding walk.
- `F2D2..F2E8`: **23 bytes** — indirect thunk through DGROUP `55C4h` preserving the EAX:DX offset/selector return pair.

`F2E9..F721` (**1,081 bytes**) remains a structural C++ lift under verification and is explicitly **uncounted**.

## Fixed-point verification completed this pass

`F206..F2D1` was re-disassembled directly from the immutable object bytes and its overlapping stack-word behavior is now represented exactly rather than approximated as an ordinary integer/fraction formatter:

- The input is a signed **16.16 fixed-point** value. After sign normalization, `MOVZX EAX,WORD PTR [EBP-2]` passes the high 16-bit integer portion to `DFFB`.
- Each fractional digit executes `MOV WORD PTR [EBP-2],0`, multiplies the remaining low-word fraction by ten, and emits byte `[EBP-2]` (bits 16..23 of the product) plus `'0'`.
- Rounding tests bit 15 of the retained fraction via `TEST BYTE PTR [EBP-3],80h`.
- The carry walk across trailing `9` digits, decimal point relocation, and leading carry insertion are retained explicitly.
- No host floating-point type or libc formatting operation is used.

The octal `F477..F4A4` path was also checked: EDI is initialized to the original scratch address at `F310`, while alternate-form octal may advance ESI before conversion. With zero precision and a generated zero digit, the ES:EDI store therefore clears the **original first scratch byte**, suppressing even the alternate-form prefix. The staging model now reflects this encoded behavior rather than standard printf expectations.

## Active continuation

Continue byte-level verification of `F2E9..F721`, then fold verified `F206..F721` into `Code1.cpp`. After that, translate `EDE7..F006` against the proven formatter helpers.

## Larger-batch policy

The working target remains approximately **4,000 newly verified bytes per substantial batch**, with fidelity controlling. Analysis continues past individual dependency edges rather than treating them as stopping points.

## Unresolved queue

Fold the verified 227-byte formatter continuation; complete and verify `F2E9..F721`; translate `EDE7..F006`; allocator bodies at `DAD9`/`DB5B`; candidate compressed-data routine `DB80`; transformation routine `DC90`; uncertain entry at `DE6F`; startup successor work; remaining address-named CRT helpers.

`CODE.1` remains controlling evidence whenever documentation and the actual bytes differ.
