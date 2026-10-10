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
