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
| MZ parser | `DosMzImage` validates original DOS MZ header, relocations, and original source offsets; requested x86-16 linear decoding is a candidate |
| LE parser | `LeImage` maps validated MZ-bound LE objects/pages and fixups; 32-bit reachability is conservative and evidence-tagged |
| Future OVR loaders | FBOV/TPOV markers are recognized but require complete mapping of overlay directory and load behavior |
| Future LX/NE loaders | Recognize signatures, then parse distinct page/object/segment and relocation formats; never treat as MZ flat code |
| Explicit COM loader | `DosComImage` supports analyst-selected 16-bit origin and request-only decode without claiming a DOS runtime segment |
| Future driver loaders | DRV and other special modules need proven calling conventions and runtime load rules |
| Resource/VM parsers | AESOP/16, GFF, DAX, Gold Box scripts and other data are separate instruction/container formats |
| Analyzer engines | Shared `X86RealModeDecoder` supplies bounded x86-16 MZ/COM candidate decode; iced-x86 x86-32 LE recursive analysis remains separate; VM engines require their own instruction sets |
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

## P0 x86-16 CFG on loader-proven original storage

The common `Dos16CodeRegion` source contract is now supported by MZ,
explicit COM, and source-bound overlay regions. `Dos16ControlFlowAnalysis`
performs conservative, bounded, same-segment recursive candidate decoding
without modifying either source or assuming cross-segment execution.
`DosOverlayFixupAudit` inventories paired unit code and opaque fixup
storage with original-source hashes, but overlay relocation *semantics*
remain unknown. CPU decoding, loader geometry, and game identity still
occupy independent layers.

## DOS MZ load-base simulation and far flow (P0 source)

The optional MZ loader model replays relocations in table order to a
private copy of original file-backed bytes, keeping per-write evidence and
full original source SHA-256. Only explicitly assumed load bases are used.
A separate review can annotate far immediate operands with exact selector
fixups and inspect further source-backed 16-bit regions as candidates.
Source byte ownership collisions are reported. Actual runtime segment
values and the meaning of opaque overlay relocation records are not known.

## Indexed dispatch and source evidence (P0)

The shared 16-bit instruction decoder now has **separate evidence-only
passes** for exact-CS direct pointer operands and explicitly described
indexed tables. Both require source file positions and preserve unknown
register/segment values; neither makes CFG edges verified or uses
game-specific addresses. This remains deliberately incomplete until
general bounded value/range propagation can validate index limits.
