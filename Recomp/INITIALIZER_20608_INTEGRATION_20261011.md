# Original CODE.2 four-initializer execution to game-entry callback — 2026-10-11

**Owner:** canonical `code1-decompilation:Recomp/`. Implementation: `src/code2_startup_gate.cpp`; four-test project: `Recomp/CMakeLists.txt`. This is a **synthetic-hosted execution gate of reconstructed canonical functions**, not DOS runtime emulation and not game boot.

## Exact source path now executed

The original executable's entry and startup are still actually invoked from the canonical reconstructed `Code2/Code2.cpp` source:

```text
353A0 → 35418
         ↳ DOS 21h/30h and FF00h bounded test responses
         ↳ PSP command tail and NO87 environment staging
         ↳ 395E2 original initializer priority scheduler (EDX=00FFh)
             1. 3A16E  save original SS at DGROUP 126EC
             2. 3A2AD  parse/copy environmental strings; allocate table at 12688/1268C
                ↳ original 37B42 allocator request checking and alignment
             3. 3A1B3  stream descriptors, five linked 8-byte nodes at 2BCF0
                ↳ original 37B42 allocator request checking and alignment
             4. 3968E  original tokenizer 39752, vector construction/argv publication
                ↳ original 37B42 allocator request checking and alignment
         ↳ 39589 original post-initialization launcher
             ↳ 3A165 stack capacity check
             ↳ 3A38A original initialization flag publication (1270C = 8000h)
             ↳ 20608 original-game entry callback: STOP BEFORE game logic
```

The scheduler table itself is the original `12720..12737` group, with equal priority `0x20` entries. The original `JA` dispatch condition selects the **last** equal-priority record first, producing `3A16E, 3A2AD, 3A1B3, 3968E`. Every callback must receive the original `0xFF` argument; deviation fails closed. The first/default CTest retains its earlier intentional stop at `3A2AD`, so the longer path is a **separate fourth test**, not a silent weakening of the original gate.

The extended test checks four actual initializer calls, eight allocation requests through the real source `function_37b42`, five original-backed 26-byte stream descriptors and five linked nodes, environment string `no87=1` and terminator, constructed `argc=2/argv` including `/?`, NULL at `argv[2]`, and `1270C=0x8000` set by `function_3a38a`. It intercepts original `20608` at the entry boundary and does **not** call game logic or return a fabricated game result.

## Mandatory fixture limitations

The original memory manager is **not** operational yet. The original request-validation and size-alignment function `37B42` runs, but its external `arena_alloc` boundary uses a bounded guest-memory **test-only bump arena** `0x2C000..0x2D500`. It refuses an unexpected arena argument, nonaligned size, null allocator result or exhausted capacity. The original DOS/WATCOM free-list/heap growth/fragmentation and DOS/4GW DPMI allocator services are **not** simulated, so allocator parity is false.

Original `3A2AD` uses `FS`/environment selector from DGROUP `12669` with offset `12665`. The reconstructed canonical function currently accepts a DGROUP offset. The test copies only the bounded original environment variable string list from the explicit environment selector `0x2C` into an unallocated scratch DGROUP region at `0x2D700` before invoking **actual** `function_3a2ad`; this adapter does not prove segment-alias behavior in real DOS. Supported PSP, environment, code, data and stack selectors are finite and explicit; any unsupported DOS/DPMI request stops execution.

The synthetic LE image stores the five **verified original CODE.2** stream descriptor dwords `[1,2,2,3,2]` at `F6FC+i*1A+0C` and a zero sixth descriptor. Original media mode optionally uses actual parsed CODE.2 DGROUP instead. The test's synthetic environment contains one `no87=1` variable and a `C:\\RLOFT\\CODE.2` program path. The synthetic non-DOS heap allocation results do not authenticate DOS returned addresses. Dynamic timing, interrupts, timer services, video, exact DOS selector/memory allocation, guest executable stack frames and full application entry remain unresolved.

## Original-file witness

Uploaded original CODE.2 is **558,689 bytes**, SHA-256 `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`. `Recomp/tools/verify_code2_startup_oracle.py` checks full file SHA-256, five original source-window hashes from the prior startup milestone, **six additional pinned original runtime spans** (initializer `3A1B3`, argument builder `3968E`, tokenizer `39752`, core allocator `37B42`, post initializer `39589`, and `3A38A`), the original initializer table, original stream descriptor dwords, and zero initial environment table. All new hashes/data values were independently checked against the uploaded original file in the private environment.

Run locally against owner-original bytes, separately from CI:

```sh
python3 Recomp/tools/verify_code2_startup_oracle.py /private/original/CODE.2
```

Original executable bytes are **not** checked into Git. The Windows source-only gate uses the synthetic image and fails closed on unexpected guest callbacks. Optional `rloft_code2_startup_gate --through-initializers /private/original/CODE.2` reads a user-local LE image, but its built-in optional-image length/geometry checks **do not substitute for the separate full SHA-256 Python oracle**.

## Windows CI acceptance

[Canonical Windows source compilation/link/run #38186500373](https://github.com/Hyperbog/RLOFT/actions/runs/38186500373), commit `7af91b0efe974ff781c1eb4b6642e92ee9b001b5`, **passed 4 of 4 CTests**:

1. canonical CODE.1 + CODE.2 complete source compile/link/leaf execution;
2. original LE loader geometry/fixup simulated integrity;
3. earlier first initializer boundary, deliberately stopping before allocator;
4. **new four-initializer actual source execution through post initializer to STOP at original game entry `20608`**.

The preceding diagnostic run confirmed exactly four callbacks/eight allocations/five streams, then exposed a stale unconditional harness stop after the scheduler's `RET`. The explicit mode-gated correction allows source `35418` to continue naturally into `39589`. No alteration to canonical CODE.2 reconstructed source was needed; only its execution adapter, tests, and original-byte oracle changed.

## Next fidelity frontier

Replace the guest bump arena with source-backed original allocator and arena/free-list state `37B42 → 39838` and establish original DOS selectors and environment segment behavior, then implement an independently observable portion of actual game entry `20608` with bounded dependencies and original-game reference execution. Until original DOS/natively captured state matches, mark **runtime/game parity unverified** and **game not booted**.

## Original termination callback source execution — 2026-10-11

The CODE.2 static LE table resolver independently authenticated an otherwise easily missed **termination callback at `0x3A228`**. Original DGROUP record `0x12738..0x1273D` has **five file-backed bytes and one LE zero-fill byte**, producing target `0x3A228`, priority `0x20`, initial flag `0`. The terminating dispatcher executes the real original `function_3962d` callback scanning routine, which calls the reconstructed original `function_3a228` (stream close then exit) when its source callback is selected.

`Recomp/src/code2_startup_gate.cpp` now exposes a separate `--termination-probe` test path, using the original static table values with a zeroed source stream list and a **fail-closed synthetic OS exit handler**. It checks the selected original `0x3A228` callback and the original scheduler's argument `0xFF00` before stopping at the unimplemented DOS exit `0x3A63E`. A full original stream-close path and actual DOS process termination are **not executed**.

[Windows canonical source CI run #38186890574](https://github.com/Hyperbog/RLOFT/actions/runs/38186890574), source commit `07e70d64e493dc1fd0a4a74547aca00706828e2a`, passed **all 5/5 CTests**, including the existing four-initializer game-entry test and new original shutdown scheduler probe. The original media were **not included** in Windows CI. The owner-uploaded `CODE.2` was separately authenticated locally with its full SHA-256, original scheduler instruction-span SHA-256s, exact indirect `FF D0` call opcodes, the four initialization records, and the six-byte shutdown record (five original file bytes plus one virtual zero-fill byte). Source analysis does not establish dynamic DOS target immutability.
