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
is exactly one adjacent, file-backed, register-to-register `MOV BX,r16`
(`89`/`8B` with exact ModRM) on the locally unique guarded path.
The copy does not modify flags. No arbitrary register propagation is
performed.

Supported unsigned guards are `JA` and `JAE` (short or near) following
`CMP r16,imm8/imm16` in the explicitly supported encodings. The imm8
value must be nonnegative after sign extension. Comparisons are bounded
to at most 256 candidate entries. The register copy, optional direct BX
use, shift, conditional branch and CMP must have exact original file
bytes, match their declared physical offsets, and have unique incoming
CFG edges along the local path. A distinct file-backed guard-failure
branch is required. Signed comparisons, joined/bypassed guard paths,
truncated CFGs and stale decoded bytes cannot prove the bound.

16-bit `[BX+disp16]` has **no automatic index scale**. A checked
`SHL BX,1` encodes stride 2 (near pointer) and `SHL BX,2` encodes
stride 4 (far pointer); the supported encodings are `D1 E3` and
`C1 E3 imm8`. This pass rejects unsigned-negative disp16 values
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
memory, branch joins, negative offsets, multi-basic-block propagation,
non-power-of-two strides, source-backed register constants, 32-bit mixed
code, self-modifying tables, overlay loader fixups and emulator traces
remain incomplete. Engine- or game-specific hardcoded recognizers are
explicitly prohibited. The independent regressions on actual original
MZ/COM/OVR game files are pending.

The synthetic fixtures cover JA, JAE, imm16 CMP, sign-extended negative
imm8, AX/CX-to-BX copy, mismatched copy, SHL stride, signed branch,
wrong ModRM, bypassed guards, missing table entries, overlap, stale
file offsets and FAR MZ-selector requirements. **They have not run.**
