# P0 generic x86-16 indirect pointer and bounded dispatch-table evidence

**Status:** committed source, synthetic cases added, **not compiled or run**.

## Automatically recognized narrow pointer form

`Dos16IndirectPointerRecovery.Inspect` examines already-reached x86-16
candidate instructions. The initial, deliberately narrow recovery path is
the unprefixed exact-byte form `2E FF /2,/3,/4,/5 disp16`, with ModRM
`mod=00 r/m=110`: a direct word or far-pointer memory operand using
**explicit CS**. The pointer must lie inside the source-backed mapped region.

- Near pointers retain original physical pointer locations and a candidate
  target IP in the same declared CS. For MZ, any relocation overlapping the
  pointer invalidates this nonrelocated-word interpretation.
- Far pointers require the exact MZ original file and a simulated load
  segment, with exactly one DOS loader relocation on the stored selector
  word and an original-file-backed, non-A20-wrapped destination.
- Dynamic writes could change even a stored pointer word. Therefore every
  result is **candidate-unverified**, and the prior unresolved CFG edge
  remains unchanged.
- DS/ES/SS/FS/GS dependencies, register-derived pointers, indexed sites,
  runtime-modified tables, pointer aliasing and unverified selector state
  are recorded as unresolved rather than guessed.

The report counts `IndirectSites`, `CandidateSites`,
`UnresolvedSites` and per-site reasons. There is no source filename,
game ID or game address hardcoded into the resolver.

## Explicit, bounded indexed-table manifest

`Dos16DispatchTableRecovery` is an additional **analyst-evidence gateway**.
It accepts a versioned JSON manifest tied to the original 64-character
SHA-256. A manifest specifies each exact indexed CALL/JMP instruction
offset, its source-relative CS table offset, a count of 2–256 entries,
the pointer format and a supporting research evidence ID. There can be
at most 128 tables per manifest and the JSON size limit is 1 MiB.

Example for a *synthetic* input (replace with its actual full SHA-256):

```json
{
  "schemaVersion": 1,
  "sourceSha256": "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
  "tables": [
    {
      "site": 256,
      "table": 288,
      "count": 3,
      "pointerKind": "near16",
      "evidenceId": "independently-checked-bounds"
    }
  ]
}
```

Only an exact explicit CS `FF` indirect CALL or JMP with ModRM `mod=10`,
16-bit displacement and an instruction already present in the CFG can
use the table. Every stored entry must be file-backed; every near target
must be within the declared region. Far table entries require one
distinct, correctly located MZ selector relocation per pointer plus
the explicitly assumed loader base. If **any** entry fails validation,
the table has zero exposed targets and an explicit rejection status.

Even when all stored entries validate, the supplied count and any
runtime register index remain **unproven**. No extra basic block, function
or CFG edge is added. These table candidates are not independently
verified jump tables and are not proof of complete executable reachability.

CLI usage, after a future manual Windows build:

```powershell
DosDis.Cli.exe com-analyze SOUND.COM --origin 100 --indirect
DosDis.Cli.exe mz-analyze MAIN.EXE --indirect --table-map bounds.json
DosDis.Cli.exe mz-analyze MAIN.EXE --load-segment 2000 --table-map bounds.json
DosDis.Cli.exe overlay-analyze GAME.OVR --regions overlay-map.json --region entry --table-map bounds.json
```

The MZ `--load-segment` is a hypothesis, not a runtime observation.
A user-provided table manifest is not an independent validation of the
bounds it claims. A narrow automatic unsigned-guard and BX-stride pass now exists as
a separate opt-in evidence-only analyzer, described in
[automatic dispatch](P0_X86_16_AUTOMATIC_GUARDS.md). Broader work includes
multi-block interval/index propagation, table discovery beyond that narrow
pattern, dynamic dataflow,
actual segment registers, confidence-ranked automatic reachability,
and reproducing results on original MZ, COM and overlay files.
