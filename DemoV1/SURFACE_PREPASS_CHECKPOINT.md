# CODE.2 surface-prepass SINTAB entry boundary — 2026-10-07

**Scope:** Small fidelity-and-diagnostics checkpoint for the portable prepass planner, not completion of the original surface rasterizer.

## Controlling original binary

Original `CODE.2`: 558,689 bytes, SHA-256 `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`; executable object-1 begins at file offset `0x3AF24`. The original `337C8..33806` lookup corresponds to file offsets `0x6E6EC..0x6E72A`.

The direct original instruction sequence (logical object-1 offsets; x86 listing displayed with the `0x10000` virtual base added):

- `337DA`: `MOVSX EBX, byte [25621]`, `337E1`: `SHL EBX,1` — signed table slot.
- `337E3`: `ADD CX, word [EDI+EBX+102h]` — **16-bit wrapping** accumulation, not 32-bit.
- `337EB`: `CMP ECX, EDX`; `337ED`: signed `JGE` exit on reaching the threshold.
- `337EF`: `SUB EBX,2`; `337F2`: signed `JGE` loop test.
- `337F4`: **second** `SUB EBX,2`; `337F7`: unsigned `JAE` test; `337F9`: zero the index only when that *second* subtraction borrows.
- `337FE`: `MOVZX EAX, word [EDI+EBX]`.

This establishes two distinct zero-like boundaries:

1. **Successful slot-zero match:** initial slot 0 reaches the threshold. The second subtraction is `0 - 2`, borrows, and the original chooses prefix selector **zero**.
2. **Complete search exhaustion:** slot 0 still does not reach the threshold. The first subtraction yields `EBX=-2`, the second yields `EBX=-4` **without unsigned borrow**, so the original reads a word at **`EDI-4`** (outside the current SINTAB block). **It does not clamp to zero.**

The second path's actual pre-block bytes and whether it is reachable in the supplied game's ordinary wall rendering are not established by this checkpoint. The native planner has no source-context bytes before its extracted scale block, so it intentionally rejects this unsupported original pointer underflow rather than returning an invented valid entry.

## Native implementation and regression

- `DemoV1/src/surface_prepass.cpp`: keeps a guarded rejection for genuine exhaustion, explicitly documents the distinct original signed and unsigned jumps.
- `DemoV1/tests/surface_prepass_tests.cpp`: checks (a) ordinary threshold hit, (b) exhausted lookup rejection, (c) a **successful** final-slot lookup clamps to entry zero, and (d) a `0xFFFF` step is evaluated as unsigned 16-bit `CX`.
- `.github/workflows/demo-v1-build.yml`: the existing Windows CI job now also syntax-checks `Code2/Code2.cpp` with strict `g++ -std=c++17 -Wall -Wextra -pedantic -fsyntax-only`. Linux remains manual opt-in; workflow scheduling, C++ compilation and tests are separate from committing these changes.

**Evidence boundary:** the original binary was independently hashed and disassembled; source/tests were committed. A passing new Windows build or the full `surface_prepass_tests` result has **not** yet been independently observed for these commits. This change does **not** implement SINTAB pixels for prepass or ordinary in-range wall runs, and it does not establish DOS-vs-native frame equivalence.

## Next renderer slice

Translate the original `32620` surface SINTAB call ABI and `3263C` prepass mask/page sequence to a validated, transactional VGA trace before running SINTAB bytecode. Verify both the selected entry/stop and the exact original source texture bytes. Compare plane output to original DOS frames before decrementing any pending-surface coverage count.


## Implemented native prepass raster stage (subsequent October 7 checkpoint)

**Original call evidence:** CODE.2 `32620..3263A` takes three stack arguments: generated-code entry pointer, destination EDI and lookup/subtract ECX; ESI is the live decoded RES4 column from dgroup `255E4`. The parent `3263C` computes the destination using `EF4D` page selection, `(C6F8>>2)+C6E0`, and the prepass's `row_shift*80` subtraction. It obtains three sequencer masks at `C668/C688/C6A8`, temporarily installs `RET` at the SINTAB entry for vertical slot `signed_byte[25621]+1`, invokes `32620` in active mask-pass order, and restores that byte.

The new `SurfaceSintabPrepassExecutor::draw` in `DemoV1/src/surface_sintab_prepass_executor.cpp` uses the already decoded original type-6 RES4 column, original SINTAB six-opcode interpreter, exact CODE.2 plane-mask tables, and the existing SINTAB prepass entry/stop plan. It works on a copy of all four VGA planes, so failure after an earlier successful store cannot corrupt the caller's video memory. Its argument checks reject incompatible surface extents, run widths outside the modeled 1..7 mask-table domain, invalid source offsets, code-selector mismatches and unsupported code execution.

`ExplorationPresenter` now draws supported prepass runs **before** the ordinary main wall pass and before type-7 scene sprites; it caches decoded RES4 textures and emits per-run rasterization counts, indexed write counts and resource/first-ray-specific failure reasons. `ExplorationRuntime::build_frame` records strict `SurfacePrepassModel::build` failures without suppressing an otherwise valid world/navigation frame. A prepass is counted as rendered only on a fully committed VGA write; failed and unplanned work remains pending in surface coverage.

New registered synthetic test `surface_sintab_prepass_executor_tests` exercises a real SINTAB opcode sequence, phase-three three-pixel VGA masks spanning two memory bytes, shade subtraction, page-one isolation, invalid selectors/widths and rollback on an invalid second source read. The earlier `surface_prepass_tests` continue to cover successful/underflow scale-table search.

**Evidence/acceptance boundary:** source and tests are committed, and the original CODE.2 instructions were independently disassembled. A passing Windows CTest run, original RES4+SINTAB pixel fingerprint match, complete ordinary main-pass SINTAB drawing, and DOS screenshot equivalence are **not yet established**. Do not mark the renderer or demo 100% complete.

### Revised next steps

1. Complete ordinary `FCD0=0` surface SINTAB drawing for extents at or below `C730`, retaining the `338C8` generic fallback at greater extents.
2. Validate prepass/main sequencing, sprite overdraw and original generated-code call geometry against instrumented original DOS indexed VGA traces.
3. Ensure all supported/unsupported surface runs remain accurately represented in pending coverage and diagnostic reports.
4. Run a complete Windows C++ build and the new CTest suite on the updated branch, then verify original resources with the existing optional asset-gated pixel regression harness.
