# CODE.1 faithful decompilation progress

![Progress](https://img.shields.io/badge/verified%20conversion-10.40%25-red)

**Verified conversion:** 10.40%  
**Remaining:** 89.60%  
**Status color:** RED until verified conversion reaches exactly 100%; GREEN only at 100%.

## Measurement rule

The percentage is the conservative coverage floor: instruction-accounted translated bytes divided by the complete object-1 executable extent (`0x11302` bytes). Embedded data remains in the denominator until code/data separation is proven. Mapping, research, scaffolding, pseudocode, and unintegrated translations do not count.

Current verified translated bytes: **7,325**.  
Current conservative denominator: **70,402 (`0x11302`) bytes**.  
Raw coverage: **10.4045%**, displayed conservatively as **10.40%**.

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
- Complete formatter driver/parser/materializer `EDE7..F741`: **2,395 bytes**.

Total integrated verified translated bytes: **7,325**.

`EDD3..EDDF` is **13 zero bytes** classified as data and is not counted as translated code.

## Next runtime staging batch

`RuntimeNext.staging.cpp` now carries **764 instruction-bounded bytes** regenerated directly from the immutable object-1 bytes. These remain deliberately excluded from the verified percentage until final instruction-level reconciliation and fold-in.

- `DAD9..DB7F`: **167 bytes** — allocator wrapper/retry path and the `DB5B` free-core wrapper. Calls to `104E0`, `10AA9`, `10C46`, and `10590` remain address-named until their surrounding runtime contracts are fully integrated.
- `DB80..DC8C`: **269 bytes** — 4096-byte `FEh`-initialized ring-dictionary decoder. The staging lift preserves the `FEEh` write index, 9-bit flag refill behavior, literal path, 12-bit back-reference construction, `(low nibble)+3` copy length, and the encoded `89CEh` termination checks.
- `DC90..DDD7`: **328 bytes** — header-gated transformation/permutation routine using the shared `30E8h` workspace. The lift retains the permutation-table construction, word search/swap, quotient/remainder-by-four source addressing, byte-span swaps/copies, and final header rewrite.

The `DC90` search loop is still under exact REPNE-SCAS reconciliation, so none of this 764-byte batch is counted early. `DE6F` remains unclassified; disassembly from that byte does not establish a valid routine entry, while the coherent fill path begins immediately afterward.

## Formatter fully integrated

The contiguous formatter region `EDE7..F741` accounts for **2,395 verified bytes**, including the corrected post-`%` parser pointer, `%n` near/far stores, callback-owned output count, selector-qualified payload reads, signed 16.16 conversion, and the original shared `F001` epilogue relationship.

## Active continuation

Reconcile the staged `DAD9..DDD7` ranges instruction-by-instruction, especially `DC90`'s `REPNE SCASW` state and allocator retry contracts; fold only proven ranges into `Code1.cpp`. Continue past any isolated dependency into startup successors and remaining address-named CRT helpers to approach the larger-batch target without relaxing fidelity.

## Larger-batch policy

The working target remains approximately **4,000 newly verified bytes per substantial batch**, with fidelity controlling. Analysis continues past individual dependency edges rather than treating them as stopping points.

`CODE.1` remains controlling evidence whenever documentation and the actual bytes differ.
