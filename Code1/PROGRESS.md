# CODE.1 faithful decompilation progress

![Progress](https://img.shields.io/badge/verified%20conversion-98.53%25-red)

**Verified conversion:** 98.53%  
**Remaining:** 1.47%  
**Status color:** RED until verified conversion reaches exactly 100%; GREEN only at 100%.

## Measurement rule

The percentage is the conservative coverage floor: instruction-accounted translated bytes divided by the complete object-1 executable extent (`0x11302` bytes). Embedded data remains in the denominator until code/data separation is proven. Mapping, research, scaffolding, pseudocode, and unintegrated translations do not count.

Current verified translated bytes: **69,365**.  
Current conservative denominator: **70,402 (`0x11302`) bytes**.  
Raw coverage: **98.5270%**, displayed as **98.53%**.

- Mid UI/resource/font/input/vector/render routines `71C8..729F`, selected `767C..80B9`, `8270..8335`, `8398..848F`, `84A0..87C1` with alignment/data/interrupt islands excluded: **3,941 bytes**.
- Mid asset/input lifecycle routines selected from `87C4..8E1E`, `91D8..95A7`, and `95BC..9BD6` with address-table/alignment islands excluded: **4,116 bytes**.
- Batch 12 asset dispatch/startup/interrupt/VGA/image helpers: `6060..6183`, `8E3C..91BB`, `9BE0..A058` selected code, `A558..A6EF`, startup `CD97..D08B` selected code, `E2C3..E35F`, `FEFC..FFD3`: **3,767 bytes**.
- Batch 13 low graphics/resource/audio state routines selected from `AE0C..BE60`, excluding eleven zero bytes and documented alignment islands: **4,129 bytes**.
- Batch 14 early text/presentation plus palette/resource continuation: `00EC..035B`, `10A4..1461`, selected `BE70..C79E`, and `CBD4..CBEE`, with alignment/data excluded; `C7A0..CBD0` was integrated later in batch 18: **3,921 bytes**.
- Batch 15 actor animation/choice and planar VGA helpers: `15B0..1A00`, `2198..24D0`, `266C..28E1`, selected `A21C..A554`, `A6F0..A8F3`, with documented alignment excluded: **3,886 bytes**.
- Batch 16 transition renderer / interrupt / startup exit: `07E4..10A1`, `7F5C..7F89`, `80BC..826D`, `8338..8397`, `CE8B..CEBF`: **2,867 bytes**.
- Batch 17 line/sprite graphics: `A05C..A219`, `A8F4..AA1F`, `AA20..AC0E`, `AC10..AE0A`: **1,748 bytes**.
- Batch 18 self-modifying scaled VGA blitters: `C7A0..C8C0`, `C8C4..C9D1`, `C9D4..CAC9`, `CACC..CBD0`, with alignment islands excluded: **1,066 bytes**.
- Batch 19 fill front end `DE70..DEA0`: **49 bytes**. `DE6F` is a preceding zero byte and `DEA1..DEA6` are alignment NOPs.
- Batch 20 residual/accounting audit: newly represented/accounted `0000..0002`, `7F50..7F5B`, `95A8..95BA`, `9E10..9E11`, shared `9EA7..9EAD`, `1058B..1058F`, `CBEF`, `CBF0..CC18`, plus previously translated-but-omitted `DF13..DF72`: **186 unique bytes**; corrected the historical duplicate count of `10384..10396` (**-19**), net **+167 bytes**.

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
- Palette/tree, actor-table, RNG/DOS/display wrapper routines `59AC..6053`, `6184..6653`, and clipped display wrappers `6654..6851` (alignment excluded): **3,384 bytes**.
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
- Low-level/runtime port/vector/fill/PIT/conversion routines `DDE0..E0C7`, excluding embedded digit tables and alignment: **663 unique bytes**; batch 20 corrects the previously omitted `DF13..DF72` accounting and removes the duplicate `10384..10396` count.
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

Total integrated verified translated bytes: **69,365**.

`DA45..DA47` is three zero bytes. `EDD3..EDDF` is **13 zero bytes**. Both are classified as data/alignment and are not counted as translated code.

## Startup continuation under reconciliation

The startup staging pool has been fully retired into verified integration; no unique startup staging bytes remain uncounted.

The PSP command-tail `REPZ SCASB` mismatch behavior, environment executable-path `ESI += 3` transition, FEFC/FF33 adapter mapping, `FC98` non-returning launch path, no-mouse unconditional dereference, and `CBF0 -> CE8B` termination stack relationship have all been reconciled directly against CODE.1. `StartupExit.reconciled.cpp` retains the detailed exit-path evidence.

## DA1C..DAD8 integration

`DA1C..DA44` and `DAC9..DAD8` are now physically integrated, adding **57 verified bytes** in the latest bridge fold. `DA48..DAC8` was already integrated and retains the reconciled AH=48/AH=4A DOS-returned BX and live AX+CF behavior. `DA1C` remains address-named at the CEC8 boundary because FEFC selector/register state is not represented as an ordinary host call.

## Mouse/event-pump integration

`D154..D178` and `D17C..D1E7` are physically integrated after direct byte-for-byte disassembly reconciliation. The three-byte `D179..D17B` alignment LEA remains excluded. This added **145 verified translated bytes**. The repeated `82A4` call at `D1B6` and the preserved pointer test after `8960` are represented explicitly.

## D23A..D5AC DOS/CRT continuation under reconciliation

`DosCrtNext.staging.cpp` originally covered **880 functional instruction bytes** in `D23A..D5AC`; `D573..D5AC` is now integrated, leaving **0 unique unintegrated bytes** in that staging span; `D27D..D27F` is three zero bytes and excluded. It maps attribute/status, find-first/find-next, DTA shuttles, open/create/truncate, descriptor/IOCTL setup, open wrapper, and close wrapper. `D332..D4F1` is integrated. `D2CD..D312` is now integrated with explicit DS/ES selector-qualified copying around AH=2F.

## DE6F..E0C7 accounting audit

The prior `RuntimeGapDE6F.staging.cpp` candidate was compared against the unified file. `DEA7..E0C7` is already represented in `Code1.cpp` and therefore is not new staging. `DE70..DEA0` is now proven by the direct call at `84B8` and integrated. `DE6F` is not part of the entry; it is the preceding zero byte. `DEA1..DEA6` are alignment NOPs.

## E0D0..E3B2 runtime/interrupt continuation under reconciliation

`RuntimeGapE0D0.staging.cpp` originally contains **722 functional instruction bytes**; `E360..E3B2` is now integrated, leaving **214 unique unintegrated bytes** between the integrated low-level/runtime block ending at `E0C7` and the integrated dispatcher block beginning at `E3B4`. It includes allocation/copy helpers, wrappers, constructor-like chains, INT 24h installation, the full IRET trampoline, and signed decimal parsing. The interrupt trampoline remains explicitly machine-boundary staging.

## Active continuation

The unique instruction-bounded staging pool from the former startup/E2C3 work is now **0 bytes**; those ranges have been integrated. `D280..D2CC` is integrated while its calls into `D2CD/D2F0` remain explicit selector-sensitive boundaries. `DE70..DEA0` is integrated; `DE6F` and `DEA1..DEA6` are classified non-code bytes. Continue hardening independently provable subranges and fold them only after exact call-contract reconciliation, preservation checks, and unified syntax checking.

## Integration placement audit

A physical-file audit confirmed that `DAD9..DDD7` was present in `Code1.cpp` but appended near the footer rather than in address order. This exposed a tooling hazard: address-order assumptions about source placement are invalid. Future one-shot folds must assert preservation sentinels for previously integrated regions before committing.

## Larger-batch policy

The working target remains approximately **4,000 newly verified bytes per substantial batch**, with fidelity controlling. Analysis continues past individual dependency edges rather than treating them as stopping points.

`CODE.1` remains controlling evidence whenever documentation and the actual bytes differ.

- Mid-graphics/DPMI/stream batch `6854..71C7` (selected instruction ranges; allocator `71C8..729F` excluded) plus `72A0..7669` buffered/string helpers with alignment/data excluded: **3,324 bytes**.

## 2026-10-01 mid UI batch 10
Added **3,941 verified bytes**, bringing the conservative total to **43,936 / 70,402 = 62.4073% (62.41%)**. Integrated the guarded 71C8 allocator/free pair, image/resource constructors, font metrics/text drawing state, ordered input queue helpers, vector install/restore wrappers, rectangle/frame helpers, and repeated-object render loops through 87C1. Explicitly excluded 766A..767B jump-table/alignment, interrupt-handler bodies 7F5C..7F89 / 80BC..826D / 8338..8397, and all documented alignment/data islands.


## 2026-10-02 integrity repair: 95BC
A post-interruption audit re-disassembled `95BC..9723` directly from immutable CODE.1 and found four semantic discrepancies in the older batch-11 lift. The integrated C++ now preserves the encoded `E280(95A8, CS)` callback installation before `8398`, calls `64B0(0)` rather than `64DC(0)`, passes `argv[1]` from the guest argv table at `30A0` to `E360`, and preserves the cdecl argument order of the `D10C(0,27E)` and `D130(0,C7)` calls. This changes no coverage count because all affected bytes were already counted; it is a fidelity correction only.


## 2026-10-02 residual classification pass 21

The verified translated-code counter remains **69,652 / 70,402 = 98.93%**; no unverified bytes were promoted to translated coverage in this pass. `RESIDUAL_LEDGER.md` now records **448 of the 750 conservative residual bytes** as explicitly established non-code from current repository evidence: **118 bytes of embedded jump/address/lookup tables** and **330 bytes of alignment, zero-fill, NOP/LEA/MOV-self padding**. This leaves **302 bytes** requiring individual classification or verified translation.

The residual audit also proved that the raw `static_assert` interval gaps are not a coverage ledger: the unified C++ has **64,729 unique bytes represented by those asserts** and **5,673 bytes outside them**, while `kVerifiedTranslatedBytes` is 69,652 because several verified regions are represented without one range assert per machine interval. Do not treat the 5,673-byte diagnostic gap total as untranslated code. The badge stays red and the denominator remains 70,402 until the remaining 302 bytes are resolved without overlap or double-counting.


## 2026-10-02 residual classification pass 22

No translated-code coverage was added: the verified counter remains **69,652 / 70,402 = 98.93%**. Fourteen one-byte islands already had explicit current-source alignment comments but were absent from `RESIDUAL_LEDGER.md`: `4B17`, `4C7F`, `522F`, `550B`, `55DB`, `57DB`, `6303`, `782F`, `7843`, `7B73`, `7D7F`, `7E03`, `7E43`, and `7E6B`. They are now classified as alignment. The curated non-code ledger therefore increases from **448 to 462 bytes** (118 table + 344 alignment/zero/padding), reducing the conservative unresolved residual from **302 to 288 bytes**. The progress badge remains red because translated coverage is unchanged and the remaining bytes still require proof.


## 2026-10-02 residual classification pass 23

Verified translated code remains **69,652 / 70,402 = 98.93%**. Preserved repository evidence explicitly classifies **38 additional residual bytes** as non-code: batch-13 excluded islands `AEA6..AEA7`, `AF4B`, `AF63`, `B4D2..B4D3`, `B51B`, `B596..B597`, `B5E1..B5E3`, `B6BE..B6BF`, `B6EB`, `B75B`, `B76B`; startup `CEE9..CEEF` seven zero bytes and `CF41..CF43` alignment NOP; runtime staging `E187..E18F` nine zero bytes, `E207` single NOP, and `E27F` zero. `RESIDUAL_LEDGER.md` now classifies **500 bytes** (118 table + 382 alignment/zero/padding), leaving **250 bytes** unresolved. No code byte was promoted and the badge remains red.


## 2026-10-02 residual classification pass 24

Verified translated code remains **69,652 / 70,402 = 98.93%**. Full Git-history reconciliation recovered a batch-12 note explicitly identifying `9EA3..9EA7` as lookup data at that checkpoint. Batch 20 later verified `9EA7..9EAD` as code, so only the still-unaccounted prefix **`9EA3..9EA6` (4 bytes)** is classified as table data. The curated non-code ledger is now **504 bytes** (122 table + 382 alignment/zero/padding), leaving **246 bytes** unresolved. `C79F` remains deliberately unclassified because its only historical hit is the stale pre-batch-18 exclusion of the formerly unresolved self-modifying blitter cluster.


## 2026-10-02 residual classification pass 25

Verified translated code remains **69,652 / 70,402 = 98.93%**. Exact integrator arithmetic closed four preserved batches without assuming instruction bytes. Batch 13 proves 4 additional non-code bytes (`BCB3`, `BD83`, `BE26..BE27`); batch 14 proves 39 alignment/data bytes between its exact asserted low-continuation subranges; batch 18 proves 11 alignment bytes (`C79F`, `C8C1..C8C3`, `C9D2..C9D3`, `CACA..CACB`, `CBD1..CBD3`); and batch 11 proves 50 alignment bytes because its exact asserted intervals total the full declared 4,116 verified bytes across three 4,166-byte selected spans. `RESIDUAL_LEDGER.md` now classifies **608 bytes** (122 table + 443 alignment/zero/padding + 43 conservative generic non-code), leaving **142 bytes unresolved**. No translated-code count was increased.


## 2026-10-02 residual classification pass 26

Verified translated code remains **69,652 / 70,402 = 98.93%**. The preserved batch-09 integrator closes exactly: its two selected spans (`6854..71C7` and `72A0..7669`) contain **3,390 bytes**, its exact static-assert intervals contain the declared **3,324 verified code bytes**, and its preserved checkpoint explicitly states that alignment/data islands were excluded. The **66-byte complement** is therefore classified as conservative non-code. No complement byte was previously present in the residual ledger. `RESIDUAL_LEDGER.md` now classifies **674 bytes** (122 table + 443 alignment/zero/padding + 109 generic non-code), leaving **76 bytes unresolved**. No translated-code coverage was added.


## 2026-10-02 residual classification pass 27

Verified translated code remains **69,652 / 70,402 = 98.93%**. Batch-10 reconciliation uses its exact asserted intervals rather than the imprecise prose endpoint around `8490..8491`. In selected span `767C..80B9`, batch 10 asserted **2,518 code bytes** and excluded alignment/data/interrupt islands. Later verification accounts for `7F50..7F5B` (batch 20) and `7F5C..7F89` (batch 16) as code. Subtracting those later code bytes and all existing ledger classifications leaves exactly **`7D67` and `7F8A..7F8B` (3 bytes)** as the batch-10 excluded non-code complement. The ledger now classifies **677 bytes**, leaving **73 bytes unresolved**.


## 2026-10-02 residual classification pass 28

Verified translated code remains **69,652 / 70,402 = 98.93%**. The first small batch-08 inter-function cluster is now classified from preserved integration evidence: `631F`, `633B`, `6357`, and `63A2..63A3` are the alignment bytes between exact asserted function endpoints and the next 4-byte-aligned entries. This adds **5 classified bytes** without changing translated-code coverage. `RESIDUAL_LEDGER.md` now classifies **682 bytes**, leaving **68 bytes unresolved**.


## 2026-10-02 residual classification pass 29

Verified translated code remains **69,652 / 70,402 = 98.93%**. The second small batch-08 inter-function cluster is classified: `63B7` and `63D5..63D7` are alignment bytes between exact asserted endpoints and the next aligned entries. This adds **4 classified bytes**. The ledger now classifies **686 bytes**, leaving **64 bytes unresolved**.


## 2026-10-02 accounting correction pass 30

A preserved-integrator audit found a historical batch-08 scalar overcount. `integrate_mid_runtime_batch_08.py` contains **47 exact non-overlapping translated ranges totaling 3,384 bytes**, while its progress update added **3,671 bytes**. Its own checkpoint text also states that `6060..6183` remained deliberately uncounted, so no hidden translated range justifies the extra 287 bytes. Batches 02 through 07 reconcile exactly to their recorded increments, and batches 09 through 12 were separately reconciled in the residual audit. The verified counter is therefore corrected by **-287**, from **69,652** to **69,365 / 70,402 = 98.5270% (98.53%)**. This pass changes accounting only; no C++ translation semantics or residual classifications are altered.


## 2026-10-02 residual classification pass 31

Verified translated code remains **69,365 / 70,402 = 98.53%**. The next small batch-08 inter-function cluster is classified: `63F1..63F3` and `6409..640B` are alignment bytes between exact asserted endpoints and the next aligned entries. This adds **6 classified bytes**. The ledger now classifies **692 bytes**, leaving **345 bytes unresolved**.


## 2026-10-02 residual classification pass 32

Verified translated code remains **69,365 / 70,402 = 98.53%**. The next small batch-08 inter-function cluster is classified: `648D..648F` and `6632..6633` are alignment bytes between exact asserted endpoints and the next aligned entries. This adds **5 classified bytes**. The ledger now classifies **697 bytes**, leaving **340 bytes unresolved**.


## 2026-10-02 residual classification pass 33

Verified translated code remains **69,365 / 70,402 = 98.53%**. The next small batch-08 inter-function cluster is classified: `6672..6673`, `66A7`, `6743`, and `67C7` are alignment bytes between exact asserted endpoints and the next aligned entries. This adds **5 classified bytes**. The ledger now classifies **702 bytes**, leaving **335 bytes unresolved**.
