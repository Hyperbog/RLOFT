# Part 17 — Indirect-control-flow and data-reference checkpoint

**Status: first conservative implementation, not a complete jump-table recovery or certification.** 2026-10-08.

## Implementation

`ReferenceRecovery` is an incremental, evidence-linked pass executed for each candidate x86-32 instruction in `ControlFlowAnalysis`. The input artifact remains immutable; it consumes the verified `LeImage` object/page mapping and internal LE relocation records. It emits new `DataReferences` and `JumpTables` sections in the `analyze` JSON output, along with `candidate-indirect-call` and `candidate-indirect-jump` flow edges when strong pointer evidence exists.

### Supported forms

| Encoded form | Decision | Evidence |
|---|---|---|
| Absolute memory operand `[disp32]` | Object-relative data cross-reference candidate | Unambiguous preferred-base mapping; file-backed target; no base, index or segment override |
| Relocation within an instruction | Object-relative relocation operand cross-reference | Parsed internal 32-bit absolute LE fixup at the encoded address bytes |
| `call/jmp dword ptr [disp32]` | Candidate indirect code target | Absolute memory slot has matching 32-bit internal LE fixup to executable, stored destination |
| `call/jmp dword ptr [index*4 + disp32]` | Candidate jump-table targets | Table entries are contiguous 32-bit LE fixups to file-backed executable addresses, at least two entries, max 128 |
| On-disk `disp32` without preferred base | Recover resolved table base when last four instruction bytes have an internal absolute fixup | LE source relocation location, target object and offset |
| Indirect register branch, base-register addressing, scaled non-4 indexing or missing fixups | Remain unresolved | No trustworthy static evidence |

`FlowAddress` always includes the LE object ID and offset. Original bytes and the relocated memory view remain separate. Relocation-backed pointer candidates are not source-code facts, proven array bounds, or guaranteed runtime targets.

### Conservative limits and conflict handling

- Only x86-32 LE objects whose loader representation is already supported; no MZ real-mode, far pointers, imports, aliases, runtime-loaded pages, or segment-state inference.
- Only absolute/DS-default memory references without a base register and without explicit segment override. Objects with overlapping preferred bases are deliberately ambiguous.
- Indexed indirect transfers need a known pointer-table base and **at least two** consecutive relocation-backed executable destinations. Stop at the first missing/invalid entry, the object's boundary, or the 128-entry cap.
- The analyzer never automatically marks table bytes **verified data**; the original unknown/protected classifications are preserved. Analyst-protected table data may still supply relocation evidence without being executed.
- No proof is made that an index is in range, that the full table was found, or that runtime memory has not been changed. The existing `unresolved-indirect-*` edge is preserved **even when candidate targets are emitted**.
- At present candidate indirect targets are traversed to make additional potential paths visible, but their edge evidence is `candidate-...`, not verified. Therefore instruction counts after Part 17 cannot be compared as proof of increased accuracy or runtime coverage.
- Unknown backing, uncertain operand mode, conflicting fixups and nonmatching table entries do not generate inferred targets.
- `maxInstructions` and `--max-output` remain bounded. Consumers must check `Truncated` and output truncation summary fields.

## Windows CLI usage

```powershell
.\DosDis.Cli.exe analyze "C:\Games\Ravenloft\CODE.2" --max-output 200
.\DosDis.Cli.exe analyze "C:\Games\Ravenloft\CODE.2" --seed 1:0x2975C --max-instructions 10000
```

Output summary now includes `DataReferences` and `JumpTables` counts. Detailed records preserve evidence and the originating instruction address. Candidate table targets and edge kinds allow navigation and review without modifying the original bytes.

## Regression tests

The synthetic fixture suite adds:

1. Three relocation-backed dword table entries with 3 candidate targets and an explicitly unresolved residual transfer.
2. Relocation attached to an unrelocated `disp32` instruction to recover the table's object-relative base.
3. Tables that *look* like pointers but have no LE fixups are not recovered.
4. An invalid second pointer blocks two-entry minimum identification.
5. Absolute non-control-flow memory operands create object-relative data references.
6. A relocated absolute indirect call produces a candidate function and retains an unresolved call edge.
7. Explicit analyst-protected table bytes remain protected while still serving as pointer evidence.
8. Original stored bytes remain unchanged after relocated pointer inspection.

**Windows-only CI** compiles and runs synthetic fixtures; the authorized CODE.1/CODE.2 end-to-end regression should be executed with the original hashes whenever the package is available locally. Do not commit commercial game binaries or upload them to CI.

## Unresolved research for subsequent milestones

- Recovering register-computed indirect jump targets, switch-index bounds, shared dispatch tables, offset-only tables, and intra-module relative arrays.
- Full producer/consumer data-flow analysis to associate load instructions with indirect register jumps.
- Static reference classification for global variables, strings, structure fields, relocation imports and VM script jump tables.
- Conflict-aware cross-object address aliases and segment/selector state.
- Performance measurements and precision/recall against the verified RLOFT CODE.1/CODE.2 byte inventories and unrelated held-out DOS executables.
- UI review and persistent evidence history; these are future Windows milestones.

**AI assistance remains explicitly deferred until after the final release. No AI scaffolding or dependencies are part of this milestone.**
