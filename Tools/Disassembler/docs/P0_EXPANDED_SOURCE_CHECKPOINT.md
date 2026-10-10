# P0 expanded source checkpoint — provenance, gaps and indirect dispatch

**Status:** source-only, not Windows-compiled or dynamically tested.
GitHub Actions remains `workflow_dispatch` only; no build triggered.

## Implemented source changes

1. **Source identity:** typed provenance with full SHA-256, file length,
   archive/member paths only when the analyst supplies them, and game/hash
   namespace. Previously archived Strahd CODE.1/2 hashes prohibit conflicting
   game IDs. Unrecognized games remain analyst-declared/unverified.
2. **Object gap ledger:** exhaustive candidate/unknown/protected/unbacked
   byte accounting; ranked file-backed unknown spans with relocation sources/
   destinations, xrefs, incoming edges and source-file mapping. Aggregate
   totals remain complete even if details are limited. Both desktop and CLI
   reports include these fields and do not reclassify a single byte.
3. **CS-relative dispatch:** indexed CS memory references can recover
   relocation-backed or tightly unsigned-guarded literal CALL/JMP table
   **candidates**. Offsets are scoped to the current LE object. Other segment
   overrides remain unresolved; no runtime claim is made.
4. **Persistent names:** V1 SQLite projects gain a V2 source identity and
   three independent named symbol tiers while preserving legacy annotations.
   The new tier APIs are source-only, not yet wired into desktop symbol editing.
5. **Strahd references:** 282 canonical non-code ranges exactly account for
   575 alignment + 204 table + 262 generic non-code = 1,041 verified
   non-code bytes, plus a documented single-byte callback at 1:0xFC97.
   The reference manifest stores no commercial executable.

## Still unresolved and verification gates

- Bounded register-value propagation across indirect call sites (Stone);
  CS segment candidate behavior unverified on actual Stone bytes.
- Menzo suspected dispatch at 0x008D not re-tested on its original.
- Comprehensive LE fixup/source metadata beyond supported internal types.
- Complete known-answer function/edge/fixup truth labels and version-to-version
  regression differencing.
- Independent Windows compile, synthetic test execution, GUI acceptance and
  cross-game original binary checks. Source-level checks do NOT replace these.

**Project completion estimate: 43%, unchanged. No builds requested.**
