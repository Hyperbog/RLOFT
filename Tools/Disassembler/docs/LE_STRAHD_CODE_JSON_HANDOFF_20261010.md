# LE/x86-32 Strahd CODE.1/CODE.2 disassembler handoff
**2026-10-10 source-only implementation checkpoint**

## Original input identity and evidence level

The uploaded `Code1.json` and `Code2.json` snapshots in the DOSDis
feedback handoff correspond **specifically to Ravenloft: Strahd's
Possession**. Other D&D titles, including Stone Prophet and
Menzoberranzan, also have `CODE.1` / `CODE.2` names; the basename alone
does **not** identify the game. The original Strahd CODE.2 source is
reported as 558,689 bytes with SHA-256
`853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
This hash and the addresses below are *external handoff evidence*, not
hardcoded engine inputs, and should be rechecked against the actual file.

The uploaded JSON snapshots are dated 2026-10-10 07:07–07:09 UTC.
They **do not** demonstrate the result of the newer source changes
documented here. G1 Windows build #35
([run 37987465368](https://github.com/Hyperbog/RLOFT/actions/runs/37987465368))
verified only source SHA `9f605e36581b2c6db72a6a47248ea5af31611506`.
No post-G1 Windows build, synthetic suite, original game-file execution
or new JSON export was performed for this checkpoint.

## Finding A — possible source-backed orphan island

Handoff: original CODE.2 LE object 1 `0x130A0..0x13117` includes a
routine not selected by the older candidate CFG. At object offset
`0x130DE` a verified original five-byte E8 near call targets `F790`.
The full `[0x1309F,0x13118)` gap is 121 unknown source-backed bytes
(including an alignment NOP) and correctly remains unknown. The
handoff found **no independently established direct inbound call to
`0x130A0`**. It does not establish an executable runtime entry.

Generic smoke fixture now covers an unseeded LE island with a real E8
call. It must remain unknown under default traversal, appear in a
64-byte optional gap report, and decode as a **candidate** when
explicitly seeded alongside the original LE entry. The test does not
copy Strahd code bytes or add automatic gap-sweeping.

Pending original-file follow-up after a verified build: compare
normal analysis against optional `--seed 1:0x130A0 --include-entry`.
Require a correct original-byte E8 target from `1:0x130DE` before
considering any more seeds or incoming indirect reference hypotheses.

## Finding B — bounded byte-width guarded table

The old Strahd CODE.2 snapshot reports at object 1:

- `F8BF` SUB AL,2 (8-bit wrap semantics).
- `F8C1` CMP AL,9.
- `F8C3` JA unsigned default.
- `F8C9` MOVZX EAX,AL.
- `F8CC` CS:[EAX*4 + F768] indirect jump; ten DWORD entries.

It lists the backed candidate destinations but previously tagged the
index as `index-bound-unproven`. Those stored destinations were not
invalid; the missing proof concerned the runtime index bound.

The existing x86-32 analyzer only recognized adjacent
`CMP index,N -> JA default -> JMP [index*4+base]`.
A **generic** guarded proof now also recognizes
`CMP byte,N -> JA default -> MOVZX index,byte -> indexed JMP`.
No pre-CMP SUB semantics are inferred: CMP overwrites prior flags and
the unsigned JA fallthrough bounds the **resulting byte** to 0..N.
Only an exact same-byte-register MOVZX to the actual dword index
register qualifies, with unique edges and no intervening instructions.
Signed branch guards, MOVSX, mismatched source/destination registers,
unexpected segment prefixes or extra predecessors remain unproven.

Relocation-backed pointer tables now preserve independently confirmed
index bounds in their evidence and select only the bounded entry prefix.
If the proven bound exceeds the source-backed relocation run, they
retain the existing `index-bound-unproven` label and never invent
missing pointer slots. The unrelocated-literal branch retains its
original source-backed pointer and target validation. Both remain
**unverified runtime-execution candidates**; indirect-jump edges are
never considered completely resolved.

Synthetic tests cover the generic ten-way unsigned byte switch,
signed/inequality branches, MOVSX, wrong byte source, wrong register
target, wrong segment, three relocation-backed entries, narrower
two-entry upper bounds, and insufficient original stored pointers.

Pending original-byte validation: confirm the old handoff's
`1:F8CC`, `1:F768` findings and the three reported destination
groups on a **new export at its exact built SHA**. Do not claim this
original switch has been verified merely because the new synthetic
source cases were authored.

## Finding C — unknown span visibility without promotion

The earlier snapshot reports:

| Strahd export | Unknown source-backed bytes | Unknown spans | 64–255 byte spans hidden by default `256` minimum |
| --- | ---: | ---: | ---: |
| CODE.2 | 21,471 | 587 | 51 |
| CODE.1 | 15,253 | 231 | 37 |

`ExecutableGapReport` now adds `UnknownSpans64To255`,
`UnknownBytes64To255`, and `SpansBelowMinimumGap` to the full
independently source-accounted ledger and human-readable report.
These do not change the default `minimumGapBytes=256`, gap ranking
or candidate code/unknown classifications.

Generic smoke compares the counters against the independently
classified ranges; a separate unseeded 64..255-byte island confirms
it is hidden from ranked default gaps but exposed in both statistics
and a `minimumGapBytes=64` report.

## On-demand original-file validation (only after build authorization)

Use a local authorized copy of Strahd CODE.2; do not commit or attach
original proprietary bytes to Windows CI or the open-source repo.

```sh
dotnet run --project src/DosDis.Cli -- report /path/to/strahd/CODE.2 \
  --seed 1:0x130A0 --include-entry --min-gap-bytes 64 \
  --max-gaps 500 --out /path/to/strahd-code2-seeded.json
```

Compare to an entry-only report with the same executable hash, build
SHA and options other than `--seed`; verify 130DE's call bytes/target
against original CODE.2 and preserve candidate reachability labels.
For `1:F8CC`, inspect the recovered table's proof evidence and
retained unresolved jump edge; do not assume that source-byte-bounded
indices prove runtime execution, immutable table memory or game logic.

The handoff's near-call correctness controls were 4,640/4,640 CODE.2
and 1,205/1,205 CODE.1 direct E8 calls in **those dated snapshots**.
Preserve them as regression baselines, not current-build claims.

## Build gate

All changes in this checkpoint are **SOURCE ONLY**: no compilation,
test run, workflow dispatch, generated artifact or original-file
acceptance. The code must pass the separately authorized manual
Windows G2 test gate before marking any new result compiled or
synthetically verified.

## Subsequent source-only integrity hardening

A further static audit found two independent acceptance risks:

1. **Alternate analyst seeds bypass a guard.** A candidate
   `CMP -> JA -> [MOVZX] -> indexed JMP` sequence can exist in
   source bytes even when the analyst seeds the MOVZX or indexed JMP
   as a separate executable-entry hypothesis. The guard does not
   dominate that entry. The bounded-index recognizer now explicitly
   refuses bound promotion when the indexed jump, or its intermediary
   MOVZX, is independently seeded. Regressions exercise both seed
   orders, direct `CMP EAX` and byte `CMP AL` variants. The generic
   analyzer still does **not** implement a complete dominator model
   for all future discovered alternate paths, so runtime index bounds
   remain candidate evidence.
2. **Gap totals can conceal a forged overlap/hole.** The prior
   `ExecutableGapAnalysis` checked total executable bytes, but
   overlapping ranges and a compensating missing byte can preserve
   that total. Every executable object is now checked for an exact
   source-object-local `[0, VirtualSize)` partition, with no
   overlapping or unclassified offsets. This is independent of
   ranking thresholds and accepts any ordering of serialized ranges.
   Regression tests deliberately create an overlap and a hole while
   keeping the old aggregate count unchanged; the ledger must reject
   the input before publishing statistics.

A byte-width guarded-table test also now covers the six-byte
`0F 87 rel32` near unsigned JA encoding, in addition to the
original two-byte `77 rel8` form. Both are expected to preserve
the same source-backed selected entry count and the correct
original default edge.

These changes are **source only**, with no new compiler or CI
evidence. In particular, do not report the original Strahd
`1:F8CC` table as newly verified until the authorized Windows
regression suite and an original-source re-export are checked.

## Guard taken-edge and provenance follow-up (source-only)

The LE/x86-32 local proof now also rejects independently seeded
`JA` instructions: the compare does not dominate an entry starting
at the conditional branch itself, where input flags are unknown.
This complements prior exclusions for independent `MOVZX` and
indirect-JMP seeds.

A guard's **out-of-range/taken target** must be an original decoded
branch target in the same object at or after the byte immediately
following the entire indirect-jump instruction. A `JA` directed into
the `MOVZX`, into the `CS:` prefix / indexed-jump bytes, or
backwards cannot license an upper bound: one of its runtime arms
could enter the indexed transfer without a bounded index. This
is a **narrow forward-skip requirement**, not a general proof that
the complete dynamic CFG has a dominating guard; later-discovered
routes and mutable runtime state remain unresolved.

New synthetic negative controls include independently seeded JA
with both seed orders; three explicit non-skipping JA displacements;
a complete 0..10 displacement sweep landing inside the byte
MOVZX/indirect-JMP sequence; three analogous relocation-backed
cases that must retain `index-bound-unproven`; long-form
`0F 87 rel32` jumping into the index transfer; and near signed
`0F 8F` masquerading as unsigned JA. Positive controls pin the
exact original short/near instruction hex bytes and verify that
near and short unsigned JA skip completely over their indexed
instruction and retain candidate-only runtime edges.

The source remains independent of Strahd-specific addresses and
bytes. The uploaded `CODE.1`/`CODE.2` findings still refer to
Strahd's Possession, not Stone Prophet or Menzoberranzan.
No Windows build, executable smoke regression, original-byte
re-export, GitHub workflow dispatch or game-runtime parity test
has been performed for these new cases. G2 remains unverified.

## LE complete-graph index proof audit (source-only)

A local unsigned JA pattern can look unique when its indirect-jump
instruction is first decoded but gain an additional predecessor
as the decoder continues exploring. Final reporting now checks
the full decoded edge inventory before accepting that local proof.

The implementation records provisional compare, JA and optional
MOVZX addresses. After traversal it verifies that each expected
fallthrough or branch-not-taken edge is the sole incoming edge to
the guarded step. The JA taken target must still skip the whole
indexed transfer. A truncated graph with provisional bounds is
not accepted as a complete proof.

A contradicted provisional finding causes the analysis to be
recomputed from the immutable original bytes with that site's
guarded hint disabled. All contradicted sites found in the same
pass are disabled together. The disabled-site set only grows, so
the repeated procedure has a finite cap. Invalid provisional
literal-table edges and any case code discovered only through
those edges do not survive the replacement graph. Source-backed
relocation tables may remain as unbounded candidates.

Single-use diagnostic traces are produced only for the stable
final analysis, not for discarded intermediate interpretations.
New synthetic cases exercise late branches back into JA, MOVZX
or the indexed jump. A positive case loops back into the CMP
itself: CMP refreshes its flags, retaining a valid local bound.
Another case exercises a worklist limit after provisional table
discovery.

The approach audits the complete *decoded* graph, not every
conceivable runtime path. It does not prove dynamic memory state,
execution, or complete interprocedural dominance. It increases
analysis work and still awaits an explicitly authorized Windows
compile and synthetic test run. All provenance for the referenced
CODE.1/CODE.2 handoff remains Strahd-specific.

### Late edges into instruction interiors

The final graph audit additionally collects exact destination offsets
by LE object. A direct branch into a **non-start byte** of the
retained CMP, JA, MOVZX or indexed JMP is a competing decode
interpretation, not a clean predecessor of the retained instruction.
Even if byte-count and instruction-start predecessor checks are
otherwise satisfied, this conflicting candidate path disqualifies
the finite-index guard and causes a source-only replay with the
guarded hint disabled.

The synthetic late-branch matrix covers instruction starts at
object offsets 4, 6 and 9, and conflicting interior entries at
offsets 3, 5, 7 and 10, in addition to the direct compare-loop
positive control. The test requires the replayed report and
single-use final trace to omit unsupported literal table claims,
phantom case code and superseded pointer evidence.

This is a conservative response to ambiguous code ownership,
not proof that the alternate interior decoding actually executes.
It remains source-only until G2.

## LE relocation source-slot overlap audit (source-only)

LE fixup records identify four-byte writes within an LE object's
source-relative address range. A code/data pointer slot starting at
offset X cannot be treated as a unique original relocated value if
another fixup starts at any offset X-3 through X+3. The other fixup's
four-byte destination then overlaps at least one byte of the slot,
even when no other fixup begins exactly at X.

The source-backed pointer validator now indexes **all** parsed LE
fixup origins, including self-relative fixups, by object-relative
address. For a relocation-backed pointer it requires an unambiguous
absolute fixup exactly at X and no other origin within X-3..X+3.
For literal-table fallback it requires no LE fixup origin anywhere
in that window. The original four-byte backing and target checks
remain mandatory. Consecutive legitimate DWORD fixups at X and
X+4 do not overlap and remain acceptable.

New synthetic feedback covers all seven fixup origins X-3..X+3
around an otherwise valid literal table and all six competing
non-identical origins around a relocation-backed table slot.
Every overlapping pattern must refrain from publishing a
jump-table candidate and preserve the unresolved runtime branch.
The fixtures contain generated LE headers and records only, not
original Strahd, Stone Prophet or Menzoberranzan bytes.

This check prevents one specific category of ambiguous original
pointer-storage evidence. It is not a guarantee of immutable
runtime data, loader behavior for unsupported fixup types or
full runtime control-flow coverage. Source changes and negative
tests are committed but have not been compiled or executed.

### LE indexed instruction operand relocation overlap

The same four-byte fixup ownership rule now applies to the encoded
displacement of an indirect indexed jump. In the source-only LE
recognizer, an ambiguous or partially overlapping displacement
fixup prevents fallback to the original unrelocated displacement,
because the loader may change its runtime effective address.

An exact, isolated supported absolute fixup can still identify
an object-relative table base. A self-relative, non-start,
ambiguous or competing displacement fixup cannot establish it.
Evidence-linked instruction operand cross-references are likewise
suppressed when the source fixup window overlaps another fixup.

The generic ten-entry byte guarded LE switch fixture has six
negative displacement origins at offsets X-3..X+3 excluding X,
and a positive exact fixup at X. The table's original DWORD
entries and remaining control-flow guards are deliberately
unchanged, isolating source pointer ownership from guard shape.
Original runtime addresses remain unverified.

These checks are source-only, not G2 execution results.

### Guard instruction relocation provenance

The post-traversal LE guard audit now checks whether any original
four-byte fixup destination overlaps the decoded CMP, JA, MOVZX, or
indexed-JMP instruction bytes. An original stored instruction may
decode as the expected unsigned guard while the LE loader rewrites
its immediate, branch displacement, opcode or register operand at
runtime. Such evidence is inadequate to license a finite index bound.

Fixup source origins are indexed by original LE object and logical
offset. A four-byte write beginning anywhere from three bytes before
an instruction through its last byte intersects the instruction.
The compare, guard, and optional MOVZX must be unmodified. The
indexed JMP may have exactly one relocation beginning at its
final four-byte displacement, because the separate table-base
validator requires that displacement fixup to be supported and
unambiguous. Competing, opcode-touching and overlapping fixups
disqualify the bounded proof and force a source-only replay.

Generic smoke fixtures install a single synthetic LE absolute
fixup at each offset 0 through 12 of the byte switch and require
that none publish a guarded literal table, including when the
stored x86 instructions look correct. A separate positive case
uses the supported exact jump displacement fixup and separately
decodes the relocated view. A negative case uses a self-relative
source fixup at that identical operand offset; the source must
not be mistaken for a supported absolute table base. A
nonoverlapping relocation beginning exactly after the indexed
jump is a positive precision control: unrelated default-path
loader bytes must not invalidate the guarded table candidate.
None of these synthetic cases proves the behavior of Strahd's
original CODE exports.

No compiler, Windows smoke test, original Strahd execution,
GitHub Action or production artifact has been run for this batch.

## LE guard replay resource limits and one-shot input safety

The complete decoded-CFG reconciliation can discard a provisional
analysis and decode it again when later edges contradict an unsigned
index guard. Such a replay can traverse the full bounded executable
inventory, so the number of passes must be independent of the
maximum number of analyst entry seeds.

The source orchestrator now permits at most **32 complete analysis
passes**, including the initial traversal. Each unsuccessful pass
must disable at least one newly contradicted guarded-dispatch site.
A nonconvergent or exhausted sequence terminates with an explicit
invalid-analysis error; no partial final candidate graph is
published as a successful run.

The public analyzer also validates the image and instruction budget
before enumerating analyst collections, and consumes at most
4,097 supplied seed rows or 100,001 supplied protected-range rows
before rejecting an over-limit request. Repeated identical rows
count toward these raw resource limits. Finite caller-provided
enumerables are materialized only once, so later source-only
replays and final optional trace generation never re-enumerate
single-use seed or protected-range iterators.

New synthetic smoke cases exercise single-use seed/protection
generators with a final decision trace, oversized repeated seed
rows, and oversized repeated protected-range rows. Both over-limit
cases must fail before consuming a trace. This is a resource
safety constraint, not a new instruction heuristic, a claim of
runtime dominance, or a Strahd-specific loader assumption.

These changes have not been compiled or executed on Windows and
do not advance G2 acceptance.
