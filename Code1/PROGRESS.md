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

## Runtime staging cleared for integration

`RuntimeNext.staging.cpp` contains **764 verified instruction-accounted bytes**, now cleared for physical fold into `Code1.cpp` but still excluded from the official percentage until that fold occurs.

- `DAD9..DB7F`: **167 bytes** — allocator wrapper/retry path and `DB5B` free-core wrapper. The retry flag exactly models the encoded one-time `10AA9` attempt followed by `10C46` gating; successful or terminal exits clear byte `8E78h`.
- `DB80..DC8C`: **269 bytes** — 4096-byte `FEh`-initialized ring-dictionary decoder. Direct inspection of `7360` confirms it returns the input byte zero-extended in EAX, validating the staged 12-bit back-reference construction and flag refill semantics.
- `DC90..DDD7`: **328 bytes** — header-gated permutation/transformation routine. The final discrepancy was the `REPNE SCASW` model: the instruction compares the word at the initial EDI before advancing it, decrements ECX for every examined word, and leaves EDI one word past the match/final examined element. The following encoded `SUB EDI,2` therefore selects exactly that examined table entry. Staging now reproduces this state transition explicitly.

The three ranges total **764 bytes**. After physical integration the verified total will become **8,089 / 70,402 = 11.4897%**, displayed conservatively as **11.48%**.

`DE6F` remains unclassified; no function boundary is inferred from an invalid-looking entry byte sequence.

## Formatter fully integrated

The contiguous formatter region `EDE7..F741` accounts for **2,395 verified bytes**, including the corrected post-`%` parser pointer, `%n` near/far stores, callback-owned output count, selector-qualified payload reads, signed 16.16 conversion, and the original shared `F001` epilogue relationship.

## Active continuation

Physically fold the cleared `DAD9..DDD7` runtime staging into `Code1.cpp`, syntax-check the unified translation unit, remove the staging file, and update integrated coverage to 11.48%. Then continue into independently provable startup successors and remaining address-named CRT helpers.

## Larger-batch policy

The working target remains approximately **4,000 newly verified bytes per substantial batch**, with fidelity controlling. Analysis continues past individual dependency edges rather than treating them as stopping points.

`CODE.1` remains controlling evidence whenever documentation and the actual bytes differ.
