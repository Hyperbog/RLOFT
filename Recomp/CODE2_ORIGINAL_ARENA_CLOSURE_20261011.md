# Original CODE.2 arena allocation, coalescing, rollback, and argument ABI — 2026-10-11

Owner: canonical `code1-decompilation:Recomp/` and shared `Code2/Code2.cpp`. This milestone is **authenticated original-byte evidence plus executable canonical reconstructed-C++ regressions**, not original DOS/WATCOM heap parity or game boot.

## Original-file identity and exact instruction proof

Authenticated owner-original CODE.2: **558,689 bytes**, SHA-256 `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`. The original logical code object and individual spans were checked against these exact SHA-256 values:

| Original object-1 range | Purpose | SHA-256 |
| --- | --- | --- |
| `37B42..37C36` | Top-level allocator and chain/growth loop | `f5e4e045387d36c2d8615cd13d6f9e11bfea4c14c60672607e5b9a13446d4737` |
| `39838..398DA` | Arena first-fit/split routine | `e296f5ba2bc6f038404bcf31e0e6d87e07284883ec293f9e1b04d083d627a9c5` |
| `398E0..399E5` | Arena free/adjacent-block coalescing | `5c5a84c8a78495f92bba9cafe6754f6f68fe8ed90c33c0f86944141ea1ea2460` |

Critical instructions: `37BC2: 89 F8` = `MOV EAX,EDI`; `37BC4: 89 F3` = `MOV EBX,ESI`; `37BC6: E8 6D 1C 00 00` = direct `CALL 39838`. Original function `39838` adds 7 to the **unaltered** EAX request at `39842: 83 C0 07` and clears the low two bits. These instructions directly prove that the main allocator forwards the **raw requested size**, not the rounded 12-byte minimum used by its independent arena-cache search test. The previous canonical C++ forwarded that rounded number, causing double-rounding. For `requested=7`, the authentic low-level arena block occupies **12 bytes**; the previous source path erroneously occupied **16 bytes**.

The correction to `Code2/Code2.cpp` removed the unused cache-search surrogate and forwards `size` to its external `arena_alloc` callback. The untouched original still has arena-chain/cached-largest-free logic that the current reconstructed top-level callback model does **not** fully execute. This is a precise verified **call ABI fix**, not a claim the top-level allocator is finished.

The source-only full-file/individual-span/opcode oracle `Recomp/tools/verify_code2_startup_oracle.py` includes these additional original spans and instructions. Separately, the authenticated original data section confirms that the initial arena-chain globals at DGROUP `12624..1262F` are zero, before the original runtime creates its arenas.

## Bounded original-code execution now in Windows CTest

Previously the CODE.2 startup accepted allocation requests through a **host-written bump-pointer allocator**. It now uses a test-only initial guest arena header at `0x2B800` and one bounded free block `0x2C000..0x2D500`. The callback rooted at DGROUP `12624` dispatches actual reconstructed `function_39838`, which owns first-fit block selection, splitting, free-list prev/next links, used-bit marking and allocation-count updates.

The original source `function_37b42` performs its argument checks and calls that allocator with the original unrounded request. The startup test checks **eight** allocations; it still reaches the real reconstructed `39589` post-initializer and stops before executing the game-entry `20608`. The stream count and environment/argv invariants remain enforced.

A separate probe executes source `39838` for consecutive allocations, verifies block splitting/linked-list state, executes actual original `398e0` to free adjacent blocks in reverse order, verifies they merge into the initial large free block, and confirms first-fit address reuse. It includes the `37b42(raw 7) → 39838(total 12)` specific regression and invalid request rejection.

A second new probe forces **only the second** environment allocation to fail while executing real reconstructed `3a2ad`. The routine releases the first allocated environment string through its actual `37c37→37c45` free forwarder, into `398e0`. The test verifies the root free-block restoration, zero published pointer-table fields, no lingering allocated blocks, and exactly one free invocation. The **failure injection** is test-only; no DOS allocation error has been observed.

### Verified Windows CI

[Windows source/recompilation gate #38190078813](https://github.com/Hyperbog/RLOFT/actions/runs/38190078813) at commit `16a2ca0f93adb2e90403e1e45995edccbc22bd72` **passed 7/7 CTests**, retaining these older gates:

1. Whole canonical CODE.1 + CODE.2 C++ compile, link and leaf execution.
2. Simulated original LE image loader/fixup integrity.
3. Original startup stopped deliberately at first allocator initializer.
4. Original four initializer callback bodies, argv and post-initialization to the unexecuted game-entry `20608` callback.
5. Original termination callback to the DOS-exit boundary.
6. Actual reconstructed arena allocate → split → free → coalesce → reuse and raw-request ABI.
7. Environment allocation failure → original source free/rollback without fabricated DOS success.

The Windows runner uses **synthetic memory and DOS fixtures only**, not copyrighted user originals. Actual original instruction hashes and opcodes were separately independently verified against uploaded CODE.2. This is **not** a DOS-native executable, not DOS interrupt/service parity, not a working game, and not a benchmark of real DOS malloc/free behavior.

## Synchronization across project owners

The correction to original `Code2/Code2.cpp` has canonical exact Git blob `b6d4fb44bda916f892b65d84cf4ec3005affdc88`, shared with `demo-v1`, `native-engine`, and `disassembler-foundation`. The independently diverged extractor keeps its other changes, but this one original-verified call argument is patched there in place: `a2312a69bf21f760165bd8214a306db310a7b433`.

| Branch | Exact source change commit | Source identity |
| --- | --- | --- |
| `code1-decompilation` | `de0c76f2f6694dfadc36c84c0f03694551638438` | `b6d4fb44…` |
| `demo-v1` | `e8417aea3fa3722e21eea5ef56a7341f6d53485b` | `b6d4fb44…` |
| `native-engine` | `c162e579ac4ba39090def5ad59b45b39b7b71e1a` | `b6d4fb44…` |
| `disassembler-foundation` | `1f8b6b9d4a8f58965c9cd4979809e49db6558bf6` | `b6d4fb44…` |
| `extractor-wpf-fluent` | `04e5a5ef19fdb0af5978df8221ef7cfdfeb4832d` | `a2312a69…`, other source drift preserved |

The immutable original `DnD-Classics` media branch and independent unmerged G2 review candidate are unchanged. Source propagation does **not** establish native Windows Full, DOSDis GUI, or extractor build equivalence on those downstream branch HEADs. Only the canonical Windows source/recompilation CI acceptance above is claimed.

## Remaining faithful runtime work

- Replace artificially initialized arena header/free block with original `39D4D/39E1C` heap establishment, DOS/DPMI allocation/resize, arena-chain traversal, correct selector aliases, original heap metadata limits and errors.
- Implement source-backed selector-qualified environment reads rather than staging it into a temporary DGROUP string list.
- Execute bounded original `20608` application entry with host subsystems added **only as proven original behavior**.
- Compare DOS runtime captures and native sources at exact accepted versions; user-original data remains private.

Do not equate a seven-test pass with DOS game boot or complete recompilation.
