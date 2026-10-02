# CODE.1 faithful decompilation progress

![Progress](https://img.shields.io/badge/verified%20conversion-56.81%25-red)

**Verified conversion:** 56.81%  
**Remaining:** 43.19%  
**Status color:** RED until verified conversion reaches exactly 100%; GREEN only at 100%.

## Measurement rule

The percentage is the conservative coverage floor: instruction-accounted translated bytes divided by the complete object-1 executable extent (`0x11302` bytes). Embedded data remains in the denominator until code/data separation is proven. Mapping, research, scaffolding, pseudocode, and unintegrated translations do not count.

Current verified translated bytes: **39,995**.  
Current conservative denominator: **70,402 (`0x11302`) bytes**.  
Raw coverage: **56.8095%**, displayed as **56.81%**.

## Immutable source identity

- File: `CODE.1`
- Size: `277,787` bytes (`0x43D1B`)
- SHA-256: `fd9b709ceee190e6a70870f9012ebea3bc437cae6bc74e0150d6f922fa750fed`

## Integrated verified translation inventory

- Early-game arithmetic/record helpers `0010..0048` and `004C..00EA`: **216 bytes**.
- Early-game hit-test/input routines `035C..043A` and `043C..04AF`: **339 bytes**.
- Early-game staged UI renderers `04B0..0648` and `064C..07E1`: **815 bytes**.
- Mid-game UI configuration/selection routines `1464..15AD`, `1A04..1EE0`, and byte-swap helper `1EE4..1EFE`: **1,597 bytes**.
- Actor generation/editor/stat routines `2A64..2DF8`, `2DFC..301F`, `3020..3134`, `3138..3204`, `3208..32F3`, `32F4..3349`, `334C..3403`, `3404..37BD`, `37C0..3881`, `3884..38A3`: **3,633 bytes**.
- Actor normalization/render/choice routines `1F24..2196`, `24D4..266B`, `28E4..2990`, `2994..2A60`: **1,413 bytes**.
- Actor stat/presentation routines `38A4..3C54`, `3C58..40FA`, `40FC..44E0`, `44E4..451E`, `4520..45EE`, `45FC..4976`: **4,286 bytes**.
- Post-generation/palette/resource/cursor/tree routines `4978..4A09`, `4A0C..4AFF`, `4B00..4B16`, `4B18..4C7E`, `4C80..4CAF`, `4CB0..4D50`, `4D54..4DC3`, `4DC4..4EED`, `4EF0..5013`, `5014..5089`, `508C..50BF`, `50C0..522E`, `5230..5279`, `527C..52A9`, `52AC..538B`, `538C..53B0`, `53B4..53E1`, `53E4..5450`, `5454..550A`, `550C..55DA`, `55DC..56DB`, `56DC..577C`, `5780..57DA`, `57DC..5878`, `5880..59AB`: **4,111 bytes**.
- Palette/tree, actor-table, RNG/DOS/display wrapper routines `59AC..6053`, `6184..6653`, and clipped display wrappers `6654..6851` (alignment excluded): **3,671 bytes**.
- Startup/recovery regions through `CD96`: **259 bytes**.
- Structured mouse-facing routines `D08C..D153`, verified mouse/event pump `D154..D1E7`, plus overlap-safe copy `D1E8..D239`: **419 bytes**.
- Attribute/status wrapper `D23A..D27C`: **67 bytes**.
- Find-first/find-next wrappers `D280..D2CC`: **77 bytes**.
- Mode-9 selector-sensitive DTA shuttles `D2CD..D312`: **70 bytes**.
- Open front/fixed wrappers `D313..D331` and `D55E..D572`: **52 bytes**.
- Descriptor/IOCTL setup `D4F2..D55D`: **108 bytes**.
- Close wrapper `D573..D5AC`: **58 bytes**.
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
- Runtime object clone `E0D0..E173`: **164 bytes**.
- INT 24h trampoline installer `E280..E2C2`: **67 bytes**.
- Runtime constructor readers `E190..E1F0` and `E208..E268`: **194 bytes**.
- Signed decimal parser `E360..E3B2`: **83 bytes**.
- Complete formatter driver/parser/materializer `EDE7..F741`: **2,395 bytes**.
- Resource/source reader helper `F750..F7B2`: **99 bytes**.
- Resource image-size parser `F7B8..F8BC`: **261 bytes**.
- Resource image loader/relocator `F8C4..FC96`: **979 bytes**.
- Runtime launch wrapper `FC98..FCE6`: **79 bytes**.
- Runtime record dispatchers `FCE8..FD6D`: **134 bytes**.
- Argv/environment builder `FD6E..FE20`: **179 bytes**.
- Command-line token scanner `FE21..FEFB`: **219 bytes**.
- Complete interrupt veneer table `FFD4..102D3`: **768 bytes**.
- CRT error/descriptor/stack/selector helpers `102D4..104DD` excluding four zero alignment bytes: **520 bytes**.
- Guest heap allocator/free-list core `104E0..1083E`: **863 bytes**.
- Heap resize/growth core `1083F..10973`: **309 bytes**.
- Heap extent query `10974..109C0`: **77 bytes**.
- Raw arena acquisition `109C1..10AA8`: **232 bytes**.
- Runtime heap-extension driver `10AA9..10C45`: **413 bytes**.
- Runtime tail helpers `10C46..10C6C`: **39 bytes**.

Total integrated verified translated bytes: **39,995**.

`DA45..DA47` is three zero bytes. `EDD3..EDDF` is **13 zero bytes**. Both are classified as data/alignment and are not counted as translated code.

## Startup continuation under reconciliation

`StartupNext.staging.cpp` contains **747 instruction-bounded candidate code bytes** from `CD97..D08B`, excluding the seven zero bytes at `CEE9..CEEF` and the three-byte alignment NOP at `CF41..CF43`. They remain uncounted pending final reconciliation.

The PSP command-tail `REPZ SCASB` mismatch behavior, environment executable-path `ESI += 3` transition, FEFC/FF33 adapter mapping, `FC98` non-returning launch path, no-mouse unconditional dereference, and `CBF0 -> CE8B` termination stack relationship have all been reconciled directly against CODE.1. `StartupExit.reconciled.cpp` retains the detailed exit-path evidence.

## DA1C..DAD8 integration

`DA1C..DA44` and `DAC9..DAD8` are now physically integrated, adding **57 verified bytes** in the latest bridge fold. `DA48..DAC8` was already integrated and retains the reconciled AH=48/AH=4A DOS-returned BX and live AX+CF behavior. `DA1C` remains address-named at the CEC8 boundary because FEFC selector/register state is not represented as an ordinary host call.

## Mouse/event-pump integration

`D154..D178` and `D17C..D1E7` are physically integrated after direct byte-for-byte disassembly reconciliation. The three-byte `D179..D17B` alignment LEA remains excluded. This added **145 verified translated bytes**. The repeated `82A4` call at `D1B6` and the preserved pointer test after `8960` are represented explicitly.

## D23A..D5AC DOS/CRT continuation under reconciliation

`DosCrtNext.staging.cpp` originally covered **880 functional instruction bytes** in `D23A..D5AC`; `D573..D5AC` is now integrated, leaving **0 unique unintegrated bytes** in that staging span; `D27D..D27F` is three zero bytes and excluded. It maps attribute/status, find-first/find-next, DTA shuttles, open/create/truncate, descriptor/IOCTL setup, open wrapper, and close wrapper. `D332..D4F1` is integrated. `D2CD..D312` is now integrated with explicit DS/ES selector-qualified copying around AH=2F.

## DE6F..E0C7 accounting audit

The prior `RuntimeGapDE6F.staging.cpp` candidate was compared against the unified file. `DEA7..E0C7` is already represented in `Code1.cpp` and therefore is not new staging. `DE6F..DEA0` is not accepted as a proven ordinary function boundary and remains on the unresolved-boundary queue. It is not counted or treated as integration-ready.

## E0D0..E3B2 runtime/interrupt continuation under reconciliation

`RuntimeGapE0D0.staging.cpp` originally contains **722 functional instruction bytes**; `E360..E3B2` is now integrated, leaving **214 unique unintegrated bytes** between the integrated low-level/runtime block ending at `E0C7` and the integrated dispatcher block beginning at `E3B4`. It includes allocation/copy helpers, wrappers, constructor-like chains, INT 24h installation, the full IRET trampoline, and signed decimal parsing. The interrupt trampoline remains explicitly machine-boundary staging.

## Active continuation

The unique unintegrated instruction-bounded staging pool is **904 bytes**: 747 startup + 157 remaining E2C3..E35F. `D280..D2CC` is integrated while its calls into `D2CD/D2F0` remain explicit selector-sensitive boundaries. `DE6F..DEA0` is an unresolved boundary and is deliberately outside that staging total. Continue hardening independently provable subranges and fold them only after exact call-contract reconciliation, preservation checks, and unified syntax checking.

## Integration placement audit

A physical-file audit confirmed that `DAD9..DDD7` was present in `Code1.cpp` but appended near the footer rather than in address order. This exposed a tooling hazard: address-order assumptions about source placement are invalid. Future one-shot folds must assert preservation sentinels for previously integrated regions before committing.

## Larger-batch policy

The working target remains approximately **4,000 newly verified bytes per substantial batch**, with fidelity controlling. Analysis continues past individual dependency edges rather than treating them as stopping points.

`CODE.1` remains controlling evidence whenever documentation and the actual bytes differ.

- Mid-graphics/DPMI/stream batch `6854..71C7` (selected instruction ranges; allocator `71C8..729F` excluded) plus `72A0..7669` buffered/string helpers with alignment/data excluded: **3,324 bytes**.
