# CODE.2 continuity addendum

This addendum supplements the exact copied `Code2/DECOMPILATION_CONTINUITY.md` with the current CODE.2-specific operating state.

CODE.1 is semantically closed. Current work is CODE.2. Read `Code2/README.md` and `Code2/PROGRESS.md` after the copied playbook. Use `Code2/Code2.cpp` as the canonical reconstruction source and syntax-check it with:

```sh
g++ -std=c++17 -Wall -Wextra -pedantic -fsyntax-only Code2/Code2.cpp
```

The immutable CODE.2 binary is controlling evidence. CODE.1/`Code1.cpp` should be used aggressively for byte-signature matching, runtime identification, calling-convention hypotheses, and reusable semantic patterns, but no CODE.1 address or semantic result is proof for CODE.2 until re-established from CODE.2 bytes.

## CODE.2 working rules

1. Verify the CODE.2 SHA-256 and regenerate object 1 before each substantive batch.
2. Regenerate authoritative disassembly for every candidate range.
3. Use CODE.1 matches as leads only; reconcile instruction-by-instruction against CODE.2.
4. Preserve widths, signedness, partial-register aliases, selector/far-pointer behavior, exact stack-slot consumption, wrap/truncation, shift masking, shared tails, callback/interrupt frames, REP string semantics, and entry-before-`DEC/JNE` loops.
5. Do not normalize address-distinct wrappers, error-mapping calls, global reloads across opaque calls, or self-modifying immediates.
6. Count only integrated verified instruction bytes or directly proven non-code/data. Staging and similarity matches remain uncounted.
7. Target substantial batches without stopping at isolated blockers; maintain an unresolved queue.
8. Commit substantive work and update `Code2/PROGRESS.md` before reporting.
9. Keep temporary scripts/staging out of the permanent folder after integration; Git history is the archive.

## CODE.1 evidence worth consulting

- `Code1/Code1.cpp`: completed semantic model and reusable boundary patterns.
- `Code1/FINAL_BINARY_AUDIT.md`: catalog of normalization mistakes caught by the end-to-end audit.
- `Code1/RESIDUAL_LEDGER.md`: evidence standard for code/data/padding classification.
- `Code1/FormatterMaterializer.map.md`: detailed runtime/formatter mapping.
- `Code1/PROGRESS.md`: final accounting model.

The CODE.1 audit specifically warns against wrong cdecl push/slot order, full-register substitution for byte/word writes, signed/unsigned branch drift, host C++ overflow/overshift UB, pre-checking encoded `DEC/JNE` loops, flattening selectors, losing DS/ES/SS in interrupt frames, caching globals the binary reloads, removing address-distinct thunks, simplifying self-modifying code, and bypassing encoded DOS/runtime error wrappers.

## Continuation behavior

When the user says `continue`, perform actual CODE.2 binary analysis and commit substantive work before replying. Begin by reading the copied playbook, this addendum, `README.md`, `PROGRESS.md`, and the current `Code2.cpp`; then verify the branch head and CODE.2 source identity, regenerate disassembly, work, syntax-check, commit, and report exact ranges/bytes/coverage/commit SHA.
