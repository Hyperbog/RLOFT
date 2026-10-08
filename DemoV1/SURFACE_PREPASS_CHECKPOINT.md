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
