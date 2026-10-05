# CODE.2 certification blocker — source-integrity audit

## Finding

The semantic-inspection ledger reaches 239,698 / 239,698 bytes, but the repository's actual `Code2/Code2.cpp` blob on branch `code1-decompilation` is zero bytes.

Git object evidence:

- path: `Code2/Code2.cpp`
- current blob SHA: `e69de29bb2d1d6434b8b29ae775ad8c2e48c5391`
- blob size: 0

`e69de29...` is the Git empty-file blob. Inspection of the file's commit history shows the same empty blob at the historical reconstruction commits checked during certification. Therefore the historical batch `*_integrate.py` commits recorded/staged integration machinery, but did not leave a populated `Code2.cpp` in Git.

## Consequence

CODE.2 must **not** be called source-integrated, build-verified, or semantically certified yet. The 100% figure established by `AUDIT_FINAL_TAIL_CLOSURE.md` is specifically 100% immutable-object semantic inspection/classification, not a 100% populated/repaired C++ translation unit.

The existing staged fragments and integration scripts are useful evidence, but they cannot be treated as proof that the main source exists. Recent fragments explicitly contain source for audited folds; older translated source must be recovered from repository history/artifacts if present elsewhere, or regenerated from the immutable binary/audit record.

## Required recovery gate

1. Recover or regenerate the complete C++ translation unit for all 235,022 translated bytes.
2. Preserve the 4,676 proven non-code/data bytes as explicit ownership/classification metadata.
3. Integrate every defect in the semantic-audit ledger, including the formatter ABI rewrite and CF/register-result corrections.
4. Verify exact ownership totals against 239,698 object bytes.
5. Compile the resulting translation unit.
6. Run structural/static verification for duplicate/missing address owners and ABI boundaries.
7. Only after those gates pass may CODE.2 be labeled `100% semantically certified`.

## Status correction

- Immutable object semantic inspection: **100.0000%**.
- Actual `Code2.cpp` population/integration: **0-byte repository blob; certification blocked**.
- Final semantic certification: **not yet achieved**.

This blocker supersedes any earlier wording that could be read as implying that staged batch integration commits populated the repository's main `Code2.cpp`.
