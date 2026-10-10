# Original CODE.2 1F1C8 staged upper/lower mode dispatch — 2026-10-10

**Branch:** `demo-v1` · **Workstream:** B, faithful native engine. **State:** source-verified C++17 **isolated** upper/lower dispatch model and authored regressions, **not wired** to live Windows VGA / DOS presentation. Original DOS frame equivalence remains unaccepted. All gameplay changes require exact original-byte provenance; no speculative rendering was added.

## Why FF50 arithmetic alone is insufficient

Prior [source-verified FF50 arithmetic](LEVEL22_ORIGINAL_FF50_SOURCE_CLIPPING_SPAN_20261010.md) established the original generated lookup `15618`, `EBX` source table value, initial upper `FF50 = 128 - EBX/2 + EBX/8`, and lower `FF50 = 256 - (EBX + upper)`. But **CODE.2 `1F2B1..1F369` invokes opaque overlay/texture/fill callbacks between these calculations**. Original `1F377` then reloads the **mutable DWORD** `FF50`, not a cached local from before the callbacks. An adapter that always substitutes pre-callback `upper` into the lower formula would hide a possible original callback side effect.

Independently disassembled the immutable original Strahd `CODE.2`, SHA-256 `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`, using executable object-1 logical base `0x3AF24`. The new original-byte oracle validates **13 complete instruction-aligned windows and 15 direct E8-rel32 targets**, locally passed against the supplied binary, including the upper texture (`1F352→34A74`), 29B40/314B0 dynamic overlays, default `33E28` upper fill, lower original texture (`1F3C3→34A74`), lower flat fill (`1F3D2→33DE0`) and the unconditional compositing-tail calls `34DE8/1BDAC/2C294/1EA04/274A4/2F868`. Compare canonical `Code2/Code2.cpp::function_1f1c8` and source `1F1C8..1F3F9`.

## Source branch order

1. `1F227..1F255`: call `34998` iff **either mutable BYTE `C026` or `C027` is nonzero** AND previous heading `BFF0` differs from current `C658`. If both mode bytes are zero, do **not** recalculate perspective merely because heading changes.
2. `1F255..1F2B1`: calculate initial upper `FF50` and `EBX` from original `22220` and source-generated `15618`; preserve original `0x800` sentinel bypass, signed depth division/lookup and 32-bit arithmetic.
3. `1F2B1..1F369`: choose exactly one source *upper* action, in order of priority:
   - If `BYTE 258A8 != 0xFF` and `BYTE 258B0 == 0x2C`, call **29B40**; this path overrides the texture mode.
   - Otherwise, if `258A8 != 0xFF` and `C028 != 0`, call **314B0(258A8)**.
   - Otherwise, for an active overlay, call **33E28(258B0)**.
   - If no active overlay (`258A8 == 0xFF`) and `C027 == 0`, call **33E28(258B0)** regardless of upper `FF50`.
   - If no active overlay and `C027 != 0`, call **2F29C(bank4,base+signed(2561D)) then 34A74 upper** only when initial `FF50 > 0`; with `FF50 <= 0`, **skip** this draw and do not replace it with 33E28.
4. `1F369..1F3DA`: only now check **BYTE C026**. If zero, call **33DE0(BYTE 258B1)**, *without recomputing `FF50`*. If nonzero, compute `DWORD FF50 = 0x100 - (EBX + CURRENT DWORD[FF50])` with original unsigned 32-bit wrapping arithmetic, interpreted as signed by `1F384 TEST; 1F386 JLE`. Call **2F29C(bank4,base+signed(2561F)) then 34A74 lower** iff the computed signed value is strictly positive; otherwise skip lower texture entirely.
5. `1F3DA..1F3F9`: original six-call compositing/scene tail executes after the lower path regardless of which upper/lower branches occurred. Do not allow host rendering to reorder these without an independent source-backed dependency audit.

New `OriginalGroundSurfaceDispatch::plan_upper` gives the verified action choice and conditional `34998` flag after consulting the already-source-verified bounded `OriginalGroundFF50SourceModel`. **`plan_lower` explicitly requires `FF50` reread AFTER upper callbacks**, with C026 and source `258B1` bytes, reproducing 32-bit wrap and signed `JLE`. These pure plan objects contain no function callbacks, change no state and do not invoke native VGA. Unsupported `22220`/C738 states remain `std::nullopt`.

## Tests, original proof and acceptance gates

- New `original_ground_surface_dispatch_tests` exercises every upper action, active-overlay priority, C026/C027/C028/heading combinations, positive/nonpositive `FF50` gates, flat-fill branch and **post-callback FF50 mutations** (including extreme signed wrap). Source-specific constant-expression paths are static-asserted.
- New `original_ground_surface_dispatch_binary_contract_tests` checks the isolated native source and test/CMake registration; `--original-code2 <original>` authenticates all **13 instruction windows** and **15 source near-call targets**. Local immutable binary check passed. Compiled C++/full Windows CTest acceptance remains pending manual authorization.
- Including the previous original 22220 provenance pair committed concurrently, this batch registers **220 CTests with Python enabled** (last accepted Full #658 **214/214** on a prior SHA). Source-only test registration is **not** evidence that those tests have passed on MSVC.
- The previous user Build #658 Level22 field session independently confirmed `RES4[655]`, 64/64 native original full-open ground captures, 32,768 indexed texture writes per frame. No live drawing changed here, preserving that positive result.
- The native ray provenance and original `335C8/32F4C/32F8C/33124/332B4/333BC` source paths are being resolved in parallel. **Do not connect this planner to the actual renderer until the source `22220` producer and mutable `C026/C027/C028`, `258A8/258B0`, `FF50` effects, callback and `34DE8` ordering are accounted for.**
- No original DOS indexed framebuffer/palette, input/IRQ, RNG, state-parity or Level22 mist-end acceptance is claimed. Weighted subsystem readiness stays at **49.8%**, DOS gate 0. No Windows build was initiated.

Reproduce the independent binary check locally:

```sh
python DemoV1/tools/verify_original_ground_surface_dispatch.py \
  --original-code2 /path/to/CODE.2
```
