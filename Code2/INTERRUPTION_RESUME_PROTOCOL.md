# CODE.2 interruption/resume protocol

This file is mandatory operating procedure whenever CODE.2 work resumes after any interruption: chat/thread rollover, token/context loss, tool timeout, workflow failure, user interruption, model handoff, or any uncertainty about where the previous pass stopped.

The purpose is to prevent duplicate accounting, address-shift mistakes, partial-function cuts, stale-branch edits, and silent corruption. Git history plus immutable CODE.2 bytes are authoritative; chat memory is never authoritative.

## Non-negotiable resume sequence

1. **Establish the live repository state before doing new analysis.**
   - Fetch the current `code1-decompilation` HEAD and at least the previous two commits.
   - Read the current tail/checkpoint sections of `Code2/PROGRESS.md`, `Code2/CODE2_CONTINUITY_ADDENDUM.md`, and `Code2/RESUME_CHECKPOINT.md`.
   - Inspect recent GitHub Actions runs and any surviving Batch staging/workflow files. If an integration is staged or running, finish/repair that exact work instead of starting a duplicate batch.
   - If the branch advanced since the interrupted analysis began, discard stale assumptions and re-anchor to the new HEAD before proceeding.

2. **Revalidate immutable binary identity.**
   - `CODE.2` SHA-256 must be `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
   - Extract object 1 from file offset `0x3AF24`, length `0x3A852`, and verify SHA-256 `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.
   - All tracked CODE.2 reconstruction addresses are **object-relative offsets**. With `objdump -b binary` on the extracted object, disassemble using those offsets directly. Do **not** add `0x10000`.
   - Mapping only when needed: executable virtual address = `0x10000 + object_offset`; original file offset = `0x3AF24 + object_offset`.

3. **Reconstruct the last committed accounting state from repository evidence, not chat.** Cross-check all of:
   - `kVerifiedTranslatedBytes` and `kVerifiedNonCodeBytes` in `Code2/Code2.cpp`.
   - The latest `static_assert` batch sum in `Code2/Code2.cpp`.
   - The total and latest ranges in `Code2/RESIDUAL_LEDGER.md`.
   - The latest checkpoint in `Code2/PROGRESS.md`.
   - The diff of the last substantive batch commit.
   These four/five sources must agree before forward work begins.

4. **Perform a mandatory overlap disassembly before moving forward.**
   - Start with at least `0x40` bytes before the recorded frontier and `0x80` bytes after it.
   - Expand backward/forward as necessary to include the entire final instruction, final basic block, shared tail, jump table, padding/data owner, and the first complete candidate routine.
   - Confirm that the last counted byte is on an instruction/data boundary and that the stated next entry really begins at an instruction boundary.
   - Check direct CALL/JMP/Jcc targets into the overlap so a shared tail or table owner is not split incorrectly.
   - Compare the overlap-window SHA-256 to `Code2/RESUME_CHECKPOINT.md` when a hash is recorded there.

5. **Overlap bytes are verification only, not new coverage.**
   - Never re-count already integrated bytes merely because they were re-disassembled for the overlap check.
   - Never rewrite an already integrated range unless the overlap check proves a real semantic/accounting defect.
   - New accounting starts strictly after the last verified committed frontier, except for separately documented unresolved islands being explicitly closed.

6. **If any mismatch is found, freeze forward progress.**
   - Do not layer a new batch on top of a bad frontier, bad address convention, bad scalar, or incomplete prior routine.
   - Make a dedicated repair commit first.
   - Recompute the affected translated/non-code totals from the immutable bytes, update `Code2.cpp`, `RESIDUAL_LEDGER.md`, `PROGRESS.md`, and the continuity addendum, syntax-check, then verify the repair commit at live HEAD.

7. **Treat uncommitted analysis as disposable.**
   - Notes, local disassembly, staging C++, and chat conclusions are not counted coverage.
   - Only committed, syntax-checked source plus reconciled ledger/accounting is authoritative.
   - If an interruption occurs mid-analysis, the next pass should reuse surviving local notes only as hints and revalidate them against the overlap window before continuing.

8. **Preserve ownership boundaries.**
   - Do not cut through a routine, compiler shared tail, jump table plus owning dispatcher, callback frame, or fall-through ownership unit just to hit a byte target.
   - If a candidate endpoint is uncertain, stop at the previous proven complete ownership boundary.

9. **Before every substantive integration commit, require all gates.**
   - Fresh immutable-object disassembly for every newly counted range.
   - Exact translated-byte sum from instruction-bounded ranges.
   - Exact non-code/data ledger sum with range-length validation.
   - Fidelity assertions for high-risk partial-register, signedness, stack/cdecl, selector, global-reload, and shared-tail behavior.
   - `g++ -std=c++17 -Wall -Wextra -pedantic -fsyntax-only Code2/Code2.cpp`.
   - `git diff --check`.
   - Removal of temporary staging payloads/scripts/workflows from the final integration commit.

10. **After every substantive commit, refresh the resume anchor.**
    - Verify live HEAD contains the integration commit.
    - Update `Code2/RESUME_CHECKPOINT.md` with the substantive commit SHA, exact cumulative accounting, final counted frontier, next unresolved/candidate address, and a small overlap-window SHA-256 around the new frontier.
    - Mark any analysis beyond that frontier as `UNCOUNTED / NOT COMMITTED`.

## Failure rule

If there is any disagreement between chat state and repository/binary evidence, discard the chat state. If there is any disagreement between generated disassembly and the immutable object bytes/hashes, stop and repair the extraction/disassembly process before changing source or accounting.

## Minimal resume checklist

A future pass must be able to answer all of these before adding one byte of coverage:

- What is live HEAD?
- What is the last substantive batch commit?
- Do `Code2.cpp`, `PROGRESS.md`, and `RESIDUAL_LEDGER.md` agree numerically?
- Do CODE.2 and object-1 hashes match?
- Am I using object-relative offsets rather than accidentally adding `0x10000`?
- Does the overlap disassembly prove the previous endpoint and the next instruction boundary?
- Are there any staged/running workflows or unclean temporary files from an interrupted integration?
- Is the next range wholly uncounted and ownership-complete?

Only after all answers are verified should forward decompilation resume.
