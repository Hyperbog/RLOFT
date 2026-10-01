# CODE.1 faithful decompilation progress

![Progress](https://img.shields.io/badge/verified%20conversion-11.77%25-red)

**Verified conversion:** 11.77%  
**Remaining:** 88.23%  
**Status color:** RED until verified conversion reaches exactly 100%; GREEN only at 100%.

## Measurement rule

The percentage is the conservative coverage floor: instruction-accounted translated bytes divided by the complete object-1 executable extent (`0x11302` bytes). Embedded data remains in the denominator until code/data separation is proven. Mapping, research, scaffolding, pseudocode, and unintegrated translations do not count.

Current verified translated bytes: **8,291**.  
Current conservative denominator: **70,402 (`0x11302`) bytes**.  
Raw coverage: **11.7766%**, displayed conservatively as **11.77%**.

## Immutable source identity

- File: `CODE.1`
- Size: `277,787` bytes (`0x43D1B`)
- SHA-256: `fd9b709ceee190e6a70870f9012ebea3bc437cae6bc74e0150d6f922fa750fed`

## Integrated verified translation inventory

- Startup/recovery regions through `CD96`: **259 bytes**.
- Structured mouse-facing routines `D08C..D153`, verified mouse/event pump `D154..D1E7`, plus overlap-safe copy `D1E8..D239`: **419 bytes**.
- File read/write/seek/delete `D5AD..D8CD`: **801 bytes**.
- DOS/CRT directory and utility routines `D8D0..DA1B`: **332 bytes**.
- Adapter wrapper `DA1C..DA44`: **41 bytes**.
- DOS memory helpers `DA48..DAC8`: **129 bytes**.
- Allocator wrapper `DAC9..DAD8`: **16 bytes**.
- Runtime allocator/decoder/transform `DAD9..DDD7`: **764 bytes**.
- Low-level/runtime regions through `E0C7`: **666 bytes**.
- Dispatcher and INT 8 paths `E3B4..E4C6`: **275 bytes**.
- Timer/PIT setup and state management `E4C7..E6E9`: **547 bytes**.
- Timer slot/control continuation `E6EA..EA0A`: **801 bytes**.
- Registration/event subsystem `EA0B..EBF2`: **488 bytes**.
- 48 exact dispatcher wrappers `EBF3..EDD2`: **480 bytes**.
- Frame no-op `EDE0..EDE6`: **7 bytes**.
- Complete formatter driver/parser/materializer `EDE7..F741`: **2,395 bytes**.

Total integrated verified translated bytes: **8,291**.

`DA45..DA47` is three zero bytes. `EDD3..EDDF` is **13 zero bytes**. Both are classified as data/alignment and are not counted as translated code.

## Startup continuation under reconciliation

`StartupNext.staging.cpp` contains **747 instruction-bounded candidate code bytes** from `CD97..D08B`, excluding the seven zero bytes at `CEE9..CEEF` and the three-byte alignment NOP at `CF41..CF43`. They remain uncounted pending final reconciliation.

The PSP command-tail `REPZ SCASB` mismatch behavior, environment executable-path `ESI += 3` transition, FEFC/FF33 adapter mapping, `FC98` non-returning launch path, no-mouse unconditional dereference, and `CBF0 -> CE8B` termination stack relationship have all been reconciled directly against CODE.1. `StartupExit.reconciled.cpp` retains the detailed exit-path evidence.

## DA1C..DAD8 integration

`DA1C..DA44` and `DAC9..DAD8` are now physically integrated, adding **57 verified bytes** in the latest bridge fold. `DA48..DAC8` was already integrated and retains the reconciled AH=48/AH=4A DOS-returned BX and live AX+CF behavior. `DA1C` remains address-named at the CEC8 boundary because FEFC selector/register state is not represented as an ordinary host call.

## Mouse/event-pump integration

`D154..D178` and `D17C..D1E7` are physically integrated after direct byte-for-byte disassembly reconciliation. The three-byte `D179..D17B` alignment LEA remains excluded. This added **145 verified translated bytes**. The repeated `82A4` call at `D1B6` and the preserved pointer test after `8960` are represented explicitly.

## D23A..D5AC DOS/CRT continuation under reconciliation

`DosCrtNext.staging.cpp` covers **880 functional instruction bytes** in `D23A..D5AC`; `D27D..D27F` is three zero bytes and excluded. It maps attribute/status, find-first/find-next, DTA shuttles, open/create/truncate, descriptor/IOCTL setup, open wrapper, and close wrapper. `D332..D4F1` remains structural staging until descriptor-table representation is reconciled against `103C3..1042C`.

## DE6F..E0C7 accounting audit

The prior `RuntimeGapDE6F.staging.cpp` candidate was compared against the unified file. `DEA7..E0C7` is already represented in `Code1.cpp` and therefore is not new staging. `DE6F..DEA0` is not accepted as a proven ordinary function boundary and remains on the unresolved-boundary queue. It is not counted or treated as integration-ready.

## E0D0..E3B2 runtime/interrupt continuation under reconciliation

`RuntimeGapE0D0.staging.cpp` contains **722 functional instruction bytes** between the integrated low-level/runtime block ending at `E0C7` and the integrated dispatcher block beginning at `E3B4`. It includes allocation/copy helpers, wrappers, constructor-like chains, INT 24h installation, the full IRET trampoline, and signed decimal parsing. The interrupt trampoline remains explicitly machine-boundary staging.

## Active continuation

The unique unintegrated instruction-bounded staging pool is **2,349 bytes**: 747 startup + 880 DOS/CRT + 722 E0D0..E3B2. `DE6F..DEA0` is an unresolved boundary and is deliberately outside that staging total. Continue hardening independently provable subranges and fold them only after exact call-contract reconciliation, preservation checks, and unified syntax checking.

## Integration placement audit

A physical-file audit confirmed that `DAD9..DDD7` was present in `Code1.cpp` but appended near the footer rather than in address order. This exposed a tooling hazard: address-order assumptions about source placement are invalid. Future one-shot folds must assert preservation sentinels for previously integrated regions before committing.

## Larger-batch policy

The working target remains approximately **4,000 newly verified bytes per substantial batch**, with fidelity controlling. Analysis continues past individual dependency edges rather than treating them as stopping points.

`CODE.1` remains controlling evidence whenever documentation and the actual bytes differ.
