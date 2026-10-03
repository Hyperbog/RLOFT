# CODE.1 canonical decompilation folder

`CODE.1` object 1 is semantically closed and byte-accounted at 100% on the `code1-decompilation` branch. This directory is intentionally kept small after closure.

## Canonical source

- `Code1.cpp` — the current faithful C++ reconstruction. This is the only live CODE.1 source file.

## Durable decompilation records

- `DECOMPILATION_CONTINUITY.md` — project methodology, fidelity rules, source identity, handoff instructions, and continuation notes.
- `FINAL_BINARY_AUDIT.md` — sequential semantic-audit record through object end, including post-closure hardening.
- `PROGRESS.md` — verified coverage/accounting history and final completion state.
- `RESIDUAL_LEDGER.md` — byte-level classification record for proven non-code/data islands.
- `FormatterMaterializer.map.md` — retained technical map for the formatter/materializer block.
- `STAGING_LEDGER.md` — historical staging provenance only. **There is no active CODE.1 staging after semantic closure.** References in this file to former staging sources describe historical work; those intermediate files were intentionally removed from the live branch.

## Removed after closure

One-shot `*.py` integration/accounting scripts and superseded `*.staging.cpp` / `*.reconciled.cpp` intermediate lifts were removed from the live folder after semantic closure. Their history remains available through Git if forensic backtracking is ever required.

Do not recreate those intermediate files unless new binary evidence requires a targeted backtrack. Any future CODE.1 change should still be reconciled directly against immutable `CODE.1` and followed by the C++17 syntax check documented in `DECOMPILATION_CONTINUITY.md`.
