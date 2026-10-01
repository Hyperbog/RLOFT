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

## Formatter continuation cleared for integration

`FormatterContinuation.staging.cpp` now contains **1,308 verified instruction-accounted bytes** covering the complete `F206..F721` continuation. They remain excluded from the official percentage only because they have not yet been folded into `Code1.cpp`.

- `F206..F2D1`: **204 bytes** — signed 16.16 fixed-point decimal materialization and exact rounding/carry walk.
- `F2D2..F2E8`: **23 bytes** — indirect thunk through DGROUP `55C4h`, preserving the EAX:DX far-return contract.
- `F2E9..F721`: **1,081 bytes** — conversion materializer, now cleared after complete instruction-level review.

If folded without further changes, the integrated verified total becomes **6,781 bytes**, or raw coverage **9.6318%** (conservative display **9.63%**).

## Final materializer clearance pass

The complete `F2E9..F721` instruction stream was regenerated directly from the immutable CODE.1 object-1 bytes and reviewed from entry through the shared jump to `F001`. The final pass resolved two remaining translation defects:

- `F303` explicitly zeros state offset `+0Ch` on every entry. The structural lift now resets `zero_count` before dispatch instead of relying on caller state.
- `F69B` reads the first payload byte through `ES:EDI`. For locally generated numeric/fixed/pointer text, EDI denotes the local scratch buffer. The staging representation keeps that buffer host-local while separately carrying its guest offset, so the sign test must read `scratch[0]` for the local DS/scratch case. Routing that address through the external selector callback could miss a generated `'-'` and lose the encoded sign extraction. External/far payloads continue to use the selector-qualified boundary.

The following paths were rechecked and match the encoded control flow: unsigned pre-read for `X/o/u/x`; signed `d/i` late consumption; explicit-precision pad selection; `F/f` fixed-point versus indirect floating callback; octal alternate-form ESI/EDI aliasing; signed and unsigned zero-precision suppression; `s/S` near/far/null selector handling; one-byte/two-byte `S` advancement; `p/P` 8- versus 13-character pointer materialization; `c` and default conversions; sign/space/plus extraction; signed precision comparisons; `'*'` pad handling; prefix length, precision, zero-count and width accounting; and the final EAX=EDI / DX=selector state entering the shared `F001` epilogue.

No host `printf`, host floating-point conversion, flat-pointer substitution, or guessed source-level formatter behavior is used. `F206..F721` is therefore **cleared for integration**.

## Active continuation

Fold the verified 1,308-byte `F206..F721` continuation into `Code1.cpp`, update integrated coverage to 9.63%, then translate `EDE7..F006` against the now-proven materializer and helper contracts.

## Larger-batch policy

The working target remains approximately **4,000 newly verified bytes per substantial batch**, with fidelity controlling. Analysis continues past individual dependency edges rather than treating them as stopping points.

## Unresolved queue

Integrate `F206..F721`; translate `EDE7..F006`; allocator bodies at `DAD9`/`DB5B`; candidate compressed-data routine `DB80`; transformation routine `DC90`; uncertain entry at `DE6F`; startup successor work; remaining address-named CRT helpers.

`CODE.1` remains controlling evidence whenever documentation and the actual bytes differ.
