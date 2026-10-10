# DOSDis G2 — independently identified DOS original-source selection (2026-10-10)

**Status:** ORIGINAL BYTE IDENTITY AND HEADER GEOMETRY CHECKED (read-only).
**NOT a DOSDis comparison, decoder run, runtime test, independently reviewed
G2 pass, or owner authorization for additional builds/CI.**

## Authority and source branch

All selected original files are fetched on the authenticated project GitHub
branch `DnD-Classics`, under `MASTER/`. The original input path must remain
title-qualified; names like `START.EXE` and `CODE.2` are not globally unique.

Selection and sizing sources:
- `MASTER/<game>/SOURCE_INVENTORY.csv` (original archive size/SHA-256).
- `MASTER/IMPORT_STATUS.md` (collection import's currently unresolved
  hash/size audit and missing *oversized disc images*).

Four selected executable files were retrieved as repository **binary blobs**;
their raw bytes were independently hashed with a separate in-memory SHA-256
implementation and compared to the corresponding original inventory entries.
All **four exact-source SHA and byte-length comparisons MATCHED**.
The verified subset does not certify the broader 1,533-file collection or
its 104 previously identified size mismatches.

## Verified source-qualified binaries

| Game | Source path under MASTER/ | Original bytes | SHA-256 | Proposed scope |
| --- | --- | ---: | --- | --- |
| Dungeon Hack | `Dungeon Hack/SBLASTER.COM` | 3,904 | `da5568671a4992b05a97b3d269900671fc0c0ab1332e7ba4ec4efe509ad97757` | Headerless COM with independently confirmed in-file near entry jump |
| Dungeon Hack | `Dungeon Hack/CHECKSYS.EXE` | 15,882 | `60070e69c2ca61e1adb6fc82bf4a3ed2652a576e40d610ec4f2b83df237fdc22` | DOS MZ, relocation table and separate x86-16 entry |
| Pool of Radiance | `Pool of Radiance/POOLRAD/START.EXE` | 65,231 | `d825df2b174675c9088ba1489488bdeebe66ad2a22943f17d3a198e60b6a07bd` | Gold Box source mappings; shared entry-prefix dedup control |
| Curse of the Azure Bonds | `Curse of the Azure Bonds/START.EXE` | 57,789 | `42fcefd6785679568b00bc1fb5b1d81ac2773416be4a35e0363920ddf6b78acd` | Gold Box source mappings; shared entry-prefix dedup control |

**No new files are needed from the owner for these selected cases.**
None of these four byte arrays, executable slices or decoded instructions
are committed to the project branch. Only source identity, mapping metadata
and observed duplication counts are recorded here.

## Independent read-only MZ/COM header preflight

| Source | Header / COM origin | Relative entry | Entry original file offset | Declared MZ bytes | Declared file-backed load bytes | MZ relocation count |
| --- | --- | --- | ---: | ---: | ---: | ---: |
| Dungeon Hack SBLASTER.COM | Headerless, explicit `0x100` COM origin | `0100h` initial entry | 0 | N/A | 3,904 COM bytes | N/A |
| Dungeon Hack CHECKSYS.EXE | MZ header 512 bytes | `0000:0000` | 512 | 15,882 | 15,370 | 43 |
| Pool of Radiance START.EXE | MZ header 512 bytes | `0F72:0012` | 63,794 | 65,231 | 64,719 | 0 |
| Curse of the Azure Bonds START.EXE | MZ header 512 bytes | `0DAB:0012` | 56,514 | 57,789 | 57,277 | 0 |

All three MZ source headers have entry CS:IP mapping to file-backed load
bytes. For Dungeon Hack CHECKSYS, the 43 records occupy the original MZ
header relocation table from byte offset **62 through 233**, all entirely
within its 512-byte header. **43/43 relocation targets** lie in the
file-backed 15,370-byte load image, with **43 unique target starts**
and zero duplicates; original load-image offsets run from **1 to 15,358**.
This is **storage geometry only**: it does not validate relocation
runtime values, DOS segment assignments, or their instruction uses.

The headerless SBLASTER.COM's first original instruction encodes a near
relative JMP from COM IP `0100h` to the source-backed target IP
`0CF6h`, corresponding to original file offset **3,062**.
This is independently checked arithmetic and byte backing,
**not proven executed code**, and this file is explicitly opted into COM
interpretation rather than inferred from its extension.

## Shared Gold Box entry prefix: not independent instruction coverage

Pool of Radiance and Curse of the Azure Bonds have different original
SHA-256 identities and relative MZ code segments, but their first
**283 stored bytes beginning at their respective MZ CS:IP entrypoints
are identical**. The *next original stored byte* differs. Their
entrypoints are at physical offsets **63,794** and **56,514**,
respectively, and both use initial `IP=0012h`.

This 283-byte source-byte prefix is a **shared implementation
hypothesis / byte identity**, not 283 independently decoded instructions,
not an established function boundary and not a runtime sharing claim.
Do not count the same independently decoded instruction sample twice
because it appears in both Gold Box titles. No code/data classification
or valid instruction boundary at the 283-byte divergence is asserted.
The differing bytes are useful for **future selection**, once an
independent decoder and a separate ISA/format review establish
what the entry routine really reaches.

## G2 predeclared next original-source test families

These samples were selected from original inventory and bytes
**before** conducting any new DOSDis candidate comparison:

1. **Dungeon Hack COM entry and source-backed near transfer:** original
   physical offset 0 / IP 0100h near jump, independently calculated
   target IP 0CF6h, file offset 3062. Select and record a bounded,
   separately decoded instruction window at the entry and at the
   independently chosen target before viewing DOSDis output.
   Keep intervening embedded string/data bytes **unproven data** until
   separately classified; a sequential linear sweep is not a
   runtime path.
2. **Dungeon Hack MZ entry + relocation safety:** physical entry
   offset 512, relative 0000:0000, source-backed module length 15370,
   43 independently parsed relocation records, 43 distinct valid
   storage targets. Compare a preselected instruction window and
   physical MZ mapping, then independently check all relocation
   table entries and at least one bounded relocation simulation at
   an *explicitly supplied* load segment. Do not infer an actual DOS
   load segment from this record. Record unsupported/overlap behavior
   separately.
3. **Pool/Curse Gold Box differential control:** the initial 283
   stored bytes are one shared code/data byte window. Predeclare
   mapped physical source positions and a common-entry oracle
   while keeping instruction semantics and branch targets
   separately researched. Choose distinct **independently verified
   code** windows elsewhere in each title before assigning separate
   cross-game correctness denominators; do not blindly decode at the
   first differing byte.

For all future original samples: pin original game, format, full-file
SHA, source path and original archive, MZ/COM mapping, external decoder
version/configuration, independently selected instructions, expected
operand/flow/relocation semantics and **every** negative/unknown case.
Use `INDEPENDENT_VALIDATION_G2_PREDECLARED_SCOPE.md` and
`INDEPENDENT_VALIDATION_RECORD_TEMPLATE.md`. No original bytes go to
GitHub Actions, project source commits or published binaries.

**Evidence state after this audit:** source selections and exact hashes
verified. New synthetic mapping/dedup test code remains **SOURCE ONLY**
since last owner-authorized Windows/Linux run #40; no DOSDis build,
synthetic test, external-tool instruction comparison or game execution
was performed in this batch. G2 independent accuracy remains PENDING.

## Reproducible, read-only local preflight (source-authored)

`Tools/Disassembler/research/original_source_preflight.py` independently
recalculates exact original SHA-256, parses only bounded MZ/COM disk
geometry, checks MZ relocation storage, and compares **unclassified
entry bytes** without decoding instructions. It has **no** dependency
on DOSDis or Iced, never executes input bytes, and prints no original
data, private paths, decoded instructions or relocation values.
Caller-supplied public identifiers are included in JSON instead.

Example with two owner-local verified Gold Box files (paths stay private):

```sh
python3 research/original_source_preflight.py \
  --source "<PRIVATE>/Pool of Radiance/POOLRAD/START.EXE" \
  --source-sha256 d825df2b174675c9088ba1489488bdeebe66ad2a22943f17d3a198e60b6a07bd \
  --mode mz --label pool-radiance \
  --peer "<PRIVATE>/Curse of the Azure Bonds/START.EXE" \
  --peer-sha256 42fcefd6785679568b00bc1fb5b1d81ac2773416be4a35e0363920ddf6b78acd \
  --peer-mode mz --peer-label curse-azure-bonds \
  --entry-prefix-limit 512
```

To inspect the explicitly selected COM, use `--mode com`;
headerless files are **not** inferred to be COM from a name alone.
MZ mode reports storage-only relocation geometry, not live segment
values. Both sources must pass exact SHA-256 input checks before any
matching-byte count is reported. An unexpected source hash causes
controlled rejection rather than output with degraded confidence.

A companion synthetic-only fixture
`research/test_original_source_preflight.py` covers changed
whole-file identity but identical mapped MZ code, distinct initial
CS/physical entry mapping, a bounded relocation-site positive/negative,
explicit COM origin, wrong input SHA and a 283-byte **synthetic**
prefix divergence. Its fixture bytes are invented, not copies of
Gold Box or Dungeon Hack binaries. Both files are **SOURCE ONLY**;
no Python test invocation, DOSDis build, or original-game run has
occurred in this continuation.

The next owner-authorized execution may run this preflight only
as part of explicitly recorded external-original validation.
Its results remain a separate source mapping evidence class,
not confirmation of CPU behavior or full G2 correctness.
