# DOSDis independent validation record — TEMPLATE

**Record status:** TEMPLATE / NOT AN EXECUTED TEST. Copy for each independently reviewed exact build and scope. Do **not** fill with guessed success. Keep proprietary game files offline and outside hosted CI/public artifacts.

## A. Gate and traceability

| Field | Required value |
| --- | --- |
| Gate / phase / feature | G2/G3/G4/G5/G6/G7/G8 and precise feature |
| Independent evidence status | PENDING / IN PROGRESS / INDEPENDENT VERIFIED / BLOCKED |
| Reviewed date (UTC) | ISO 8601 |
| DOSDis branch and exact commit SHA | Full 40-character SHA |
| Compiled executable identifier | Build SHA/artifact manifest + compile/test run link |
| OS/toolchain | Platform, .NET version, architecture |
| Owner execution authorization | Link/date or "not authorized; not run" |
| Analysis command/options | Full invocation excluding private source paths, seeds, limits, relocated/source view |
| Original file identity | Game, edition/version, internal path and source archive (private paths redacted) |
| Original SHA-256 and byte length | Full independently measured SHA-256, decimal length |
| Address namespace | File, COM origin, MZ segment:offset or LE object:offset; physical mapping |
| External oracle A | Class, producer, independently captured expectation, date/revision, version |
| External oracle B | Different evidence class, producer, independently captured expectation, date/revision |
| Independent reviewer | Reviewer and disposition; not solely the fixture author |
| Result limits | Scope, blind spots, unsupported features, unbenchmarked candidate count |

## B. Predeclared expectation scope

Define the sample before comparing against the DOSDis build; include a reproducible sampling method. Do not quietly discard difficult or conflicting addresses. If selected from prior RLOFT research, specify the exact audit revision and original byte evidence. If from another disassembler, record tool/version, x86 mode and decode settings.

| Expected ID | Original object:offset or segment:offset | Expectation kind | Ground-truth evidence ID | Expected interpretation | Independent origin |
| --- | --- | --- | --- | --- | --- |
| TO FILL | TO FILL | instruction/branch/fixup/code/non-code/error | TO FILL | TO FILL | TO FILL |

## C. Comparison results

| Finding class | Expectations (denominator) | Matched | Missing | Conflicting | Not observed / not classified | Unsupported (explicit) |
| --- | ---: | ---: | ---: | ---: | ---: | ---: |
| Instruction boundaries and operand semantics | — | — | — | — | — | — |
| Calls/conditional branches/CFG | — | — | — | — | — | — |
| Object/page/segment/source-file maps | — | — | — | — | — | — |
| Fixups/relocations/overlapping writes | — | — | — | — | — | — |
| Known code/data/unknown boundaries | — | — | — | — | — | — |
| Unsupported/malformed/negative controls | — | — | — | — | — | — |

Record candidate instructions not included in the benchmark: **NOT MEASURED**. A partial sample does not justify whole-image or runtime accuracy percentages. Preserve all discrepancies, including ones from third-party oracles.

## D. Independent secondary-decoder checks (x86)

- Independently selected byte windows and original SHA/mapping:
- DOSDis/Iced version and x86-16/x86-32 mode:
- Second independently implemented disassembler/version and configuration (e.g. Capstone/Ghidra):
- Exact instruction starts/lengths, operand widths, prefixes and direct targets compared:
- Disagreements; authoritative Intel/AMD/manual check and disposition:
- Whether code reachability is a static *candidate* rather than runtime-proven:

## E. Discrepancy ledger

| Finding ID | Actual vs expected | Suspected cause | Separate byte/spec/trace evidence | Resolution/issue link | Rechecked build SHA | Disposition |
| --- | --- | --- | --- | --- | --- | --- |
| TO FILL | TO FILL | TO FILL | TO FILL | TO FILL | TO FILL | OPEN |

Do not mark any unexplained required mismatch as passed. If a reference source is wrong, preserve its correction history and re-review it independently.

## F. Final gate decision

- Compilation verified at **this** source/build SHA? YES / NO / NOT RUN
- Synthetic test suites executed at **this** SHA? YES / NO / NOT RUN
- Original exact-hash content sampled under owner authorization? YES / NO / NOT RUN
- Independently authored expectations and second oracle class available? YES / NO
- All mandatory samples checked; zero unexplained conflicts/missing? YES / NO
- Negative/unsupported cases fail closed? YES / NO
- Reviewer has independently approved interpretations? YES / NO
- Proprietary originals excluded from source/CI/artifacts? YES / NO
- **Decision:** INDEPENDENT VERIFIED (named scope only) / PENDING / BLOCKED
- Residual unsupported features and runtime uncertainty:
- Evidence links, manifest hashes and reproduced commands:
- Owner release acceptance (G8 only): APPROVED / PENDING

See [normative independent validation policy](INDEPENDENT_VALIDATION_POLICY.md) and [roadmap gates](ROADMAP_BUILD_GATES.txt). This template is a record format, not a claim of execution.
