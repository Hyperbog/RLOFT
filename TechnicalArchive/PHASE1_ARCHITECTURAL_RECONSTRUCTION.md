# Ravenloft: Strahd's Possession — Technical Preservation Archive

## Phase I: Architectural Reconstruction

Status: **ACTIVE**  
Evidence baseline: certified CODE.1 and CODE.2 faithful reconstructions and their final audits.  
Working branch: `code1-decompilation`

## Purpose

Phase I reconstructs and preserves the architecture of the shipped DOS game from primary binary evidence. The objective is not to redesign the engine into a cleaner modern architecture. It is to describe the technology that actually exists in the Ravenloft binaries, including awkward boundaries, compiler/runtime artifacts, segmented-memory behavior, shared tails, address-distinct wrappers, self-modifying operands, bugs, and unresolved behavior.

The completed CODE.1/CODE.2 reconstructions are executable semantic evidence, but the immutable original binaries remain controlling evidence whenever a documentation claim conflicts with a reconstruction, name, comment, compiler convention, or architectural hypothesis.

## Evidence classes

Every substantive architectural claim in this archive must carry one of these classes:

- **CONFIRMED** — directly demonstrated by original machine code, encoded data, container metadata, or an instruction-accounted reconstruction already certified against those bytes.
- **RECONSTRUCTED** — behavior established by combining multiple confirmed observations into a complete mechanism or subsystem relationship.
- **INFERRED** — a strongly supported architectural interpretation that is useful but not directly proved by the available evidence.
- **UNKNOWN** — evidence is insufficient or contradictory. No semantic certainty is asserted.

`CONFIRMED` and `RECONSTRUCTED` claims must be traceable to concrete evidence: original logical/file addresses, reconstructed function/range, data offsets, audit records, or other primary artifacts. `INFERRED` claims must state the evidence that motivates the inference. Unknowns are preserved rather than silently filled in.

## Fidelity rules

1. Do not modernize the historical architecture while documenting it.
2. Do not promote reconstructed names into historical source names unless independent evidence proves the original name.
3. Preserve integer widths, signedness, truncation/wrap behavior, partial-register semantics, selector/far-pointer behavior, calling conventions, shared control-flow ownership, and runtime/compiler idioms when architecturally relevant.
4. Preserve address-distinct wrappers and duplicated routines when the binary preserves them.
5. Preserve observed bugs, leaks, asymmetries, hard-coded limits, and unusual behavior as historical facts.
6. Distinguish game/application architecture from compiler runtime, DOS extender, operating-system, driver, and library mechanisms.
7. Separate original-engine behavior from any later modern-port implementation.
8. Never use another DreamForge title as proof of Ravenloft behavior. Cross-title similarities may be recorded later as comparative evidence only.
9. Never convert uncertainty into a clean diagram merely to make the architecture easier to explain.
10. When later DAT/RES/resource analysis changes an architectural interpretation, amend the archive and retain enough provenance to explain why.

## Phase I deliverables

Phase I will build the archive incrementally rather than as one monolithic document.

### A. System architecture map

Document process startup, executable/container organization, CODE.1/CODE.2 relationship, major application subsystems, runtime boundaries, global-state regions, and shutdown/control transfer.

### B. Execution-flow atlas

Document startup-to-gameplay control flow, principal dispatchers, game-loop/timing relationships, UI/input paths, rendering invocation, simulation/update paths, level/resource transitions, and termination/error paths.

### C. Subsystem specifications

Create evidence-backed specifications for at least:

- executable/runtime/bootstrap
- memory and segmented/far-pointer model
- timing/events
- input and UI
- rendering/graphics
- world/map/level state
- objects/items/inventory
- actors/party/character systems
- creatures/AI
- combat/damage/effects
- spells/magic where established
- dialogue/text/script-like dispatch where established
- file/resource I/O
- save/load
- sound/music
- error handling and diagnostics

Subsystem boundaries will be revised when the evidence requires it; this list is an investigation index, not a claim that the original source tree used these modules.

### D. Function and global-state atlas

Map reconstructed routines and significant globals to architectural roles while retaining original logical addresses. Semantic names remain aliases layered over address identities unless historical names are independently recovered.

### E. Data-flow and ownership maps

Record important state producers/consumers, persistent versus transient state, buffers, caches, handles, tables, callback relationships, and cross-subsystem dependencies.

### F. Binary/data-format integration

As `LEVELS.DAT`, level files, `OBJECTS.DAT`, `NPCS.DAT`, `INFO.DAT`, `SAVE.DAT`, RES archives, and other assets are decoded, link each proven field back to the code paths that consume or produce it. File-format documentation and engine architecture must cross-validate each other.

### G. Historical implementation notes

Preserve DOS/DOS-extender assumptions, compiler/runtime signatures, hardware-facing behavior, memory constraints, performance-oriented implementation choices, self-modification, and other period-specific techniques that a modern port may eliminate.

### H. Uncertainty and contradiction ledger

Maintain a permanent ledger for unresolved meanings, competing interpretations, suspicious reconstruction names, unclassified data, and evidence conflicts. Resolution requires evidence; absence of an answer is preferable to an invented one.

## Traceability format

Architectural entries should use this compact record where practical:

```text
Claim: <precise statement>
Class: CONFIRMED | RECONSTRUCTED | INFERRED | UNKNOWN
Evidence:
  - CODE.1 <logical range/address> / Code1.cpp <routine or section>
  - CODE.2 <logical range/address> / Code2.cpp <routine or section>
  - <audit/data-file/primary-source evidence>
Dependencies: <related state/functions/files>
Notes: <quirks, alternatives, unresolved edges>
```

Diagrams must follow the same evidence standard. A box or arrow represents a claim and must be supportable from the archive text or evidence index.

## Phase I work order

1. **Freeze and index the certified CODE baseline.** Record final CODE.1/CODE.2 identities, object/container organization, audit artifacts, reconstruction files, and final accounting.
2. **Build the top-level call/subsystem map.** Start from entry/bootstrap and identify major long-lived state and dispatch boundaries without prematurely assigning speculative module names.
3. **Reconstruct lifecycle architecture.** Startup, initialization, main interactive/gameplay loops, transitions, teardown, fatal/error paths.
4. **Build subsystem maps.** Work outward from proven call/data clusters and globals.
5. **Cross-link external DAT/resource formats.** Use producer/consumer code to establish semantics rather than documenting file offsets in isolation.
6. **Audit the architecture documentation.** Every strong claim must have evidence; inferred claims must be visibly marked; contradictions and unknowns remain explicit.
7. **Publish Phase I baseline.** Freeze a versioned architectural snapshot suitable for community review before modernization/port documentation is allowed to blur the historical model.

## Initial architectural constraints already established by the decompilation project

The archive begins with several methodological facts established during CODE reconstruction and auditing:

- CODE.1 and CODE.2 must be analyzed as distinct binaries; CODE.1 similarities are leads for CODE.2, not proof.
- Selector-qualified/far-memory behavior is semantically significant and cannot be flattened into ordinary host pointers in the historical model.
- Partial-register accesses and exact integer widths can alter behavior and therefore belong in subsystem specifications where they affect state or algorithms.
- Shared tails and cross-boundary control flow mean reconstructed C++ function boundaries are not automatically equivalent to original source-level module boundaries.
- Runtime/compiler mechanisms must be separated from game-specific mechanisms even when both reside in the same executable object.
- Self-modifying operands and address-distinct veneers/wrappers are part of the shipped implementation and must not disappear from the historical record.

These are constraints on documentation, not yet a complete architecture.

## Phase I completion criterion

Phase I is complete only when the major executable subsystems and lifecycle are documented with traceable evidence, CODE.1/CODE.2 relationships are mapped, significant cross-subsystem state is cataloged, known external data formats are connected to their consumers/producers, and all remaining material uncertainties are explicitly recorded.

A document being readable or internally consistent is not sufficient for certification. Historical claims must survive a final evidence audit against the original binaries and certified reconstruction records.

## Next artifact

`TechnicalArchive/01_BINARY_BASELINE_AND_SYSTEM_MAP.md`

This document will establish the immutable binary/audit baseline and begin the first evidence-backed top-level architecture map. It is the first substantive analysis unit of Phase I.
