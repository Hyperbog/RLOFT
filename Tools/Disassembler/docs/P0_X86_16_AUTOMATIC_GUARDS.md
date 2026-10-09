# P0 automatic unsigned-guarded x86-16 indexed dispatch — source checkpoint

**Verification:** source committed; synthetic fixtures written but **not
compiled or executed**. No original DOS game binary or Windows build has
been processed in this checkpoint.

## Scope: one locally unambiguous path

`Dos16AutomaticDispatchRecovery.Inspect` is a read-only, independent
post-CFG evidence pass for 16-bit source-backed mapped regions. It supports:

```asm
cmp bx, 2
ja  default
shl bx, 1
jmp word ptr cs:[bx+table_disp16]
```

or a bounded register copy:

```asm
cmp ax, 2
ja  default
mov bx, ax
shl bx, 1
call word ptr cs:[bx+table_disp16]
```

The bound source may be BX directly or AX, CX, DX, SI, or DI when there
is exactly one file-backed, register-to-register `MOV BX,r16` (`89`/`8B`
with exact ModRM) on the locally unique guarded path. The copy does not
modify flags. A bounded `Dos16UniqueTransparentPath` pass now permits
up to four transparent NOP/near-jump instructions **between** the
compare, conditional guard, optional copy, scaling step, and indirect
call/jump. Each transparent instruction must have exact original bytes,
a physical source-file location consistent with its decoded address,
and a unique incoming CFG edge. Other register and flag mutations,
joins, indirect transfers, and unknown call returns are not transparent.
No arbitrary register propagation is performed.

Supported unsigned guards are `JA`, `JAE`, `JB` and `JBE`
(short or near16) following `CMP r16,imm8/imm16` in the explicitly
supported encodings. The immediate must be nonnegative after sign
extension. Comparisons are bounded to at most 256 candidate entries.

| Guard and proven path | Accepted BX values | Validated entry count |
| --- | --- | --- |
| `JA` not taken | `BX <= N` | `N+1` |
| `JAE` not taken | `BX < N` | `N` |
| `JB` taken | `BX < N` | `N` |
| `JBE` taken | `BX <= N` | `N+1` |

The branch-direction proof uses exactly two original-source-matched
outgoing edges: one taken and one fallthrough. The 8- or 16-bit signed
displacement is decoded independently from the original bytes, and
the opposite edge must lead to a separately decoded, file-backed
instruction rather than rejoining the table's unique path. Both
target and sequential IP must remain within the explicit segment;
16-bit wrapping and artificially added/rewritten edges are rejected. The register copy, optional direct BX
use, shift, conditional branch and CMP must have exact original file
bytes, match their declared physical offsets, and have unique incoming
CFG edges along the local path. A distinct file-backed guard-failure
branch is required. Signed comparisons, joined/bypassed guard paths,
truncated CFGs and stale decoded bytes cannot prove the bound.

16-bit `[BX+disp16]` has **no automatic index scale**. A checked
`SHL BX,1` encodes stride 2 (near pointer) and `SHL BX,2` encodes
stride 4 (far pointer); the supported encodings are `D1 E3` and
`C1 E3 imm8`. Near pointers also accept exact `ADD BX,BX`
encodings `01 DB` or `03 DB`: with a proven at-most-256-entry
unsigned bound, doubling cannot overflow 16 bits. The scaling step
may modify flags because the unsigned branch was already evaluated,
and a transparent path to the indirect transfer does not consume them.
This pass rejects unsigned-negative disp16 values
(`disp16 > 0x7FFF`), any unproven scaling, and table spans that could
wrap or exceed the original file-backed segment.

The target `FF /2,/3,/4,/5` must be an already-decoded,
**CS-overridden** instruction using exact ModRM `mod=10, r/m=111`
(i.e. `[BX+positive_disp16]`). Other 16-bit addressing combinations
such as BP, SI+BX and DI+BP have different semantics and are rejected.

## Evidence handling

The recovered upper bound is passed to the existing
`Dos16DispatchTableRecovery` which validates **every stored entry**.
Near entries must have an original-file-backed same-segment target and
must not be overlapped by a DOS MZ fixup. Far entries additionally need
exactly one MZ selector relocation each and a caller-selected,
nonwrapped load-segment hypothesis. If any entry fails, **no partial
target list is exposed**.

Table storage overlapping candidate decoded instructions is an explicit
code/data rejection, not a silent reclassification.

Successful recovery is labeled
`candidate-guarded-CS-BX-table`, never verified execution.
Candidate destinations remain external annotations; **no CFG edge,
function boundary, or automatic reachability seed is inserted**.
The reported count establishes the local unsigned comparison path only;
pointer storage could be modified at runtime. Unsupported indirect
transfers remain unresolved and diagnostic accounting is preserved.

The report includes total indirect sites, eligible CS/BX sites, table
candidates, unresolved sites, a bounded list of per-site statuses, and
whether the graph or output is truncated. A truncated graph cannot
generate automatically guarded candidates.

## CLI (source interface, pending manual build)

```powershell
DosDis.Cli.exe com-analyze UNKNOWN.COM --origin 100 --auto-tables
DosDis.Cli.exe mz-analyze UNKNOWN.EXE --auto-tables
DosDis.Cli.exe mz-analyze UNKNOWN.EXE --load-segment 2000 --auto-tables
DosDis.Cli.exe overlay-analyze UNKNOWN.OVR --regions source-map.json --region entry --auto-tables
```

The optional `--table-map manifest.json` path remains available for
independently bounded analyst hypotheses. Those bounds are **not**
silently promoted to automatic proof. Real runtime selector state is
never guessed from a file name.

## Known limitations / next generalization

General interval dataflow, variable index increments, aliases through
memory, branch joins, negative offsets, **arbitrary multi-basic-block**
propagation beyond uniquely proven short NOP/JMP bridges,
non-power-of-two strides, source-backed register constants, 32-bit mixed
code, self-modifying tables, overlay loader fixups and emulator traces
remain incomplete. Engine- or game-specific hardcoded recognizers are
explicitly prohibited. The independent regressions on actual original
MZ/COM/OVR game files are pending.

The synthetic fixtures cover JA, JAE, imm16 CMP, sign-extended negative
imm8, AX/CX-to-BX copy, mismatched copy, SHL stride, signed branch,
wrong ModRM, bypassed guards, missing table entries, overlap, stale
file offsets and FAR MZ-selector requirements. **They have not run.**

## Bounded cross-basic-block evidence (next source milestone)

`Dos16UniqueTransparentPath` recognizes only unprefixed `NOP`,
`JMP rel8` (`EB cb`), and `JMP rel16` (`E9 cw`) as transparent
bridge instructions. It verifies the exact original instruction bytes
and physical offset, recomputes each branch destination from its
signed displacement, rejects 16-bit IP wrapping, and requires exactly
one incoming edge at each intermediate instruction. Revisiting an
address, encountering a truncated graph, or exceeding the default
four-intermediate-instruction bound causes rejection.

The guard, copy, stride, and table may therefore live in different
candidate basic blocks, but **only across a single unambiguous chain**.
The original CFG remains unchanged. The separate evidence analyzer
cannot prove that the observed file-backed table is immutable at
runtime or that an indirect branch is actually executed.

New synthetic fixtures include: NOP between guard and stride, NOP
between stride and table, short and near16 JMP bridges, copied-register
bounds across harmless instructions, an alternate incoming edge,
`INC BX` invalidation, over-budget paths, stale bridge bytes, both
`ADD BX,BX` encodings and unrelated-register arithmetic rejection.

**These newly added fixtures have not been compiled or executed.**
Previous Windows preview builds did not include this milestone.

## In-range branch selection (current source checkpoint)

In addition to the existing out-of-range rejection idiom, the generic
source pass recognizes an unsigned branch that explicitly selects the
in-range table path, including a table block reached through separately
validated NOP/near-JMP bridges. `JB` is a strict bound and `JBE`
is inclusive; the code never assumes a signed condition or treats
`JA/JAE` taken as an accepted in-range path. Original-byte verification
covers both edges and the full relative target operand. New synthetic
fixtures include short and near16 `JB/JBE`, signed guard rejection,
wrong-polarity rejection, a fabricated additional edge, a tampered
branch destination, an out-of-range path rejoining the dispatch, a
forged nonadjacent fallthrough, and a graph-entry backedge.

**This is source-only:** no new Windows build, synthetic suite execution
or real original-binary validation occurred in this checkpoint. All
returned targets remain unverified candidates.

## Exact accumulator comparison and two-step far16 stride

The unsigned `CMP AX,imm16` accumulator encoding `3D iw` is recognized
as an alternative to the longer ModRM encoding **only** if an exact
source-backed `MOV BX,AX` follows the proven guard. `CMP AX,imm8` or
8-bit accumulator instructions are not implicitly widened; the immediate
bound remains limited to 0–255 and the branch must use its exact
unsigned condition. A bound on AX cannot be transferred through
`MOV BX,CX`.

Four-byte FAR pointer tables now also accept **two provably linked BX
doublings** (for example `SHL BX,1; SHL BX,1`, or `ADD BX,BX;
SHL BX,1`) instead of only `SHL BX,2`. Each doubling must match one
of `D1 E3`, `C1 E3 01`, `01 DB` or `03 DB`. Both must appear along
a uniquely validated, bounded original-byte path; no intervening
register-writing instruction is allowed. The small unsigned index bound
guarantees fourfold scaling does not wrap a 16-bit register.

**Crucially, neither the extra encoding nor the stride pattern bypasses
far pointer verification:** every 4-byte table entry must still have
independent MZ relocation evidence at its selector field and an
explicitly hypothesized DOS load segment for address mapping.
An unsupported/missing fixup, wrong-register ADD, single doubling,
unsupported count, source overlap or incomplete graph rejects
the table without CFG or function-promotion.

Synthetic cases cover valid accumulator CMP, excessive immediate,
incorrect AX register copy, two SHL variants, mixed ADD/SHL scaling,
unrelated-register arithmetic, a single insufficient scale and
missing relocation simulation. These cases are **added to source but
not compiled or executed**; previous releases predate them.

## Rejected guard exit must be original-byte backed

R1.4 review identified a provenance gap: requiring a decoded instruction
at the guard's rejected target did not itself prove that decoded row
still pointed to its original physical bytes. The rejected edge now
must have exactly one matching decoded instruction, a bounded file
offset, a valid 1-15 byte length, matching original bytes, and no
source mapping discrepancy. This applies to JA/JAE and JB/JBE
acceptance polarity alike. The accepted path continues to require
its separate unique predecessor and exact branch proof.

Synthetic negative fixtures corrupt the exit's physical file offset,
replace its reported opcode bytes, or duplicate its decoded row.
All must reject automatic table recovery without changing the
underlying graph. These are source-only regressions until G1.

## R1.4-R1.6 source integrity and report normalization

The unique-path verifier now uses the pinned Iced x86-16 decoder to
re-derive the instruction's length and control-flow category directly
from original bytes. An instruction row with correct address and
OriginalBytes but forged FlowControl (for example, a NOP mislabeled
as an indirect jump) is rejected. This shared check covers CMP,
register copy, scaling, NOP/near-JMP bridges, and decoded opposite
guard exits; source rows are never trusted only because their
reported hex string looks plausible.

Graph snapshots with duplicate instruction starts now produce an
unresolved result without throwing from the predecessor-map lookup.
An automatic table's EntryCount appears only after **every pointer
entry** passes the independent storage/relocation checker. Rejected
tables retain that checker's exact reason and have no exposed target
list or validated entry count. Input-graph truncation and capped
per-site findings appear distinctly in the summary report.

All new safety cases are source-only. Before any claim that this
milestone is compiled or verified, run G1 as documented in
[G1_X86_16_BATCH_VERIFICATION_MATRIX.txt](G1_X86_16_BATCH_VERIFICATION_MATRIX.txt)
**after explicit owner authorization**. The prior Windows preview
does not contain these changes.
