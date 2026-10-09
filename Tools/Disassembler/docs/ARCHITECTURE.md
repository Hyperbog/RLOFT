# DOSDis — multi-format DOS executable and bytecode analysis architecture

**Scope:** a general-purpose, evidence-first DOS reverse-engineering platform.
This document supersedes the Part 14 loader-specific outline.
**Verification:** new source is not Windows-compiled or regression-tested.

## Separated responsibilities

| Component | Contract |
| --- | --- |
| Original source | `BinaryArtifact` snapshots immutable original bytes; SHA-256 and byte length anchor identity |
| Format detector | `BinaryFormatDetector` recognizes signatures but never assumes a valid loader or executable instructions |
| File intelligence | `FileIntelligence` derives only structural facts and optional external catalog metadata, with distinct evidence statuses |
| Optional game metadata | `SourceIdentityRegistry` reads versioned, size-limited JSON; no hardcoded game fingerprints |
| MZ parser | `DosMzImage` validates original DOS MZ header and relocation layout; explicitly requested x86-16 linear decoding is only a candidate |
| LE parser | `LeImage` maps validated MZ-bound LE objects/pages and fixups; 32-bit reachability is conservative and evidence-tagged |
| Future OVR loaders | FBOV/TPOV markers are recognized but require complete mapping of overlay directory and load behavior |
| Future LX/NE loaders | Recognize signatures, then parse distinct page/object/segment and relocation formats; never treat as MZ flat code |
| Future headerless loaders | COM and DRV need explicit load segment, CPU mode, entry point, origin and environment assumptions |
| Resource/VM parsers | AESOP/16, GFF, DAX, Gold Box scripts and other data are separate instruction/container formats |
| Analyzer engines | Current iced-x86 x86-16 requested decode and x86-32 LE recursive candidate analysis; VM engines separate |
| Research/export | Source-bound known answers, independent project symbol tiers, gap ledger, diagnostics and provenance |

## Load/address policy

Original physical offsets, DOS MZ load-module offsets, 16-bit segment:offset
addresses, LE object-relative offsets, and process linear addresses are
distinct types of facts. No operation may silently substitute one for another.
File-backed versus virtual-only storage remains explicit; original data is
never rewritten by relocation resolution or speculative analysis.

The detector favors validated structural evidence over filenames. A file
extension is a suggestion at most. Ambiguous headerless data must remain
unclassified until an analyst explicitly supplies load and decode context.

A recognized `FBOV`, `TPOV`, `LX`, or `AESOP/16` signature is **not**
permission to decode from offset zero with x86. Unknown content retains a
read-only physical hex view.

## Research isolation

All original executables are distinguished by full SHA-256 and length.
External catalogs, source editions and archived research can identify a
particular game/version, but cannot cause the analyzer to insert a known
function boundary, jump target, byte classification or expected conclusion.
An analyst-approved verified name is separately stored with a source
reference, never propagated by matching filenames or structural similarity.
No proprietary originals are committed to the tool or CI.

## Verification and release policy

Current release target: Windows desktop and CLI, manual-only build workflow.
No builds unless the user explicitly requests one. Source-level structural
checks are not compilation or runtime acceptance.

Initial integration fixtures should include standalone DOS MZ, LE, FBOV,
TPOV, LX, raw COM/DRV, AESOP/16, GFF, and eventually other bytecode
containers, each with positive, negative, truncated, ambiguous and identity
collision checks. Generalized recovery takes priority over interface polish.
