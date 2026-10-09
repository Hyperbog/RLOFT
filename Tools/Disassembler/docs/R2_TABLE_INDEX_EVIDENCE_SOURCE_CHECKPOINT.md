# R2.4 — read-only 16-bit table index evidence, source checkpoint

**Branch:** `disassembler-foundation` | **Status:** partial R2.4
SOURCE COMPLETE for external-bound, finite BX index intersection only.
**NOT COMPILED OR TESTED IN THIS CHECKPOINT.** G1 manual Windows build
has been explicitly requested by the owner; this session's GitHub
connector does not expose a workflow_dispatch operation, and the local
container does not have dotnet. An actual new Actions run has **not**
been started or verified by this session. The owner can select
`disassembler-foundation` in GitHub Actions > Windows DOS disassembler
core smoke tests > Run workflow. Do not claim G1 acceptance until the
named run, HEAD SHA, logs and artifacts are verified.

## API and evidence boundaries

`Dos16TableIndexEvidence.Inspect(region, graph, manifest, mz?, simulation?)`
is an optional, read-only layer combining:

1. `Dos16DataflowAnalysis.Analyze` — independently validated original
   instruction bytes, complete candidate CFG edge accounting and bounded
   conservative register propagation. Rejects incomplete dataflow and
   publishes no entries if the worklist cannot finish.
2. `Dos16DispatchTableRecovery.Inspect` — separate SHA-bound analyst
   manifest with an explicit 2..256 entry bound, exact
   `CS:[BX+disp16]` indexed CALL/JMP encoding, original file-backed
   near/far pointer words, MZ relocation context where required and
   code/data overlap rejection.
3. The value of **BX before the indirect instruction** as a *byte
   offset*, not an ordinal table index. A candidate set is accepted
   only if it is a small exact set or singleton (at most eight distinct
   values), and **every** possible BX value is aligned to pointer
   width (2 bytes near16, 4 bytes far16), less than the independently
   declared `count * pointerWidth`, and matches a validated stored
   entry's original physical byte offset and ordinal.

Unknown BX, intervals, misaligned offsets, out-of-bound offsets,
uncertain table storage, code/data conflicts and unreached sites all
produce explicit unresolved statuses. No interval is narrowed using a
CMP without proven flag provenance. Any stale, spoofed, duplicate or
truncated candidate graph blocks the entire joined-value report.

A candidate stored target is **not** validated executable code or proof
of real runtime memory state; game code can modify writable pointer
tables. This component does **not** create or rewrite any CFG edges,
seed branch targets, classify functions, infer memory immutability, or
modify project databases or WPF application behavior.

## Source-only regression fixtures

`Dos16TableIndexEvidenceSmoke.Run()` is wired into the existing Windows
smoke suite, but has **not been executed**. Synthetic cases cover:
- exact BX=2 selecting the second two-byte source-backed near pointer;
- BX=0 selecting the first entry;
- BX=1 rejected as unaligned and BX=4 rejected as out of count;
- partial-byte register instruction leaving BX Unknown;
- forged CFG edge and truncated graph suppressing all output;
- mismatched external original SHA-256 rejected by storage validator;
- synthetic code/data storage overlap refused as target evidence.

No original game files, executable packages, UI or original-DOS
code paths have been executed or modified.

## What remains for full R2.4 / G2

R2.4 currently supports only external-bound finite index intersections.
It has **not** demonstrated automatic bounds proof over every incoming
path, unsigned flag guarding, runtime table immutability or segment
selector propagation. Continue with adversarial branch-join and memory
clobber fixtures and a conflict ledger before enabling any wider
index-bound synthesis. Preserve the independent existing guarded-index
pipeline until an authorized compile/run confirms behavior.

**Owner action:** manually dispatch the existing Windows Actions workflow
on the `disassembler-foundation` branch if a build is wanted now.
Then provide its run link or ID for verification; no new GitHub Actions
trigger was added. No application GUI testing is needed for this
source-only component until a current Windows preview is validated.
