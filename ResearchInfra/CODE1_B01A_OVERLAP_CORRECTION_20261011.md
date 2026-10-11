# CODE.1 B01A overlapping x86 instruction and source-fidelity correction — 2026-10-11

**Owner:** canonical `Code1/` and `ResearchInfra/`. This is a source-backed semantic correction, discovered by the authenticated broad CFG survey. The game was not booted.

## Exact original evidence

The owner-uploaded original CODE.1 has length **277,787** and SHA-256 `fd9b709ceee190e6a70870f9012ebea3bc437cae6bc74e0150d6f922fa750fed`. The logical code-object slice **`0xB01A..0xB054`** has SHA-256 `581ed03cd65d437d3601f3fd2511c1c82d444aba9d8de5dee5a365987a39ec09`.

Two exact conditional instructions branch into **`0xB04C`**, which is not a linear instruction boundary. At `0xB04B`, bytes `A8 AA` linearly decode to `TEST AL,AAh`; the low-targeted byte at **`0xB04C = AA`** instead decodes to **`STOSB [ES:EDI],AL`**. When AL is zero, the original appends one NUL byte before restoring the destination pointer and returning. The original branches are:

- `0xB037: 74 13` — `JE 0xB04C` after testing the first byte of the loaded source word.
- `0xB043: 74 07` — `JE 0xB04C` on the second source-word iteration.
- `0xB04C: AA` — `STOSB` on the alternate entry.
- `0xB04D` — common epilogue after the alternate NUL write.

This is a **real x86 overlapping-entry pattern**, not proof of corrupted bytes or an invented runtime jump. `Code1/verify_b01a_overlapping_stosb.py` checks the complete original identity, full instruction-span hash, exact branch/opcode windows and alternate `STOSB` decode. It must be supplied the owner-local original file; CI does not include game bytes.

## Source mismatch and correction

Previously canonical `code1::function_b01a` performed `if(low8(AX)==0)break` with **no write**, so an even-length source such as `"AB"` appended to `"X"` could leave an old nonzero destination byte after the copied pair. The reconstructed original does explicitly `STOSB` there. The corrected source now executes `lg13w8(boundary,dest_end,0)` before breaking on low-byte-zero, and retains the original 16-bit word writes for other paths. No gameplay design, host renderer or file format is changed.

`Recomp/src/code1_compilation_unit.cpp` adds an actual compiled callback-memory witness (not simply a source-text assertion) checking:

- **even source** `"AB"` appends `"XAB\0"`, with one word write and one byte-terminator write;
- **odd source** `"ABC"` appends `"XABC\0"`, with two word writes and no extra byte write;
- **empty source** writes exactly one NUL at the existing destination terminator.

The canonical link-gate test entry invokes that witness and refuses success on a missing original terminator. This is synthetic memory parity of one verified routine, not whole-game DOS parity.

[Windows canonical CODE.1 + CODE.2 test #38184924773](https://github.com/Hyperbog/RLOFT/actions/runs/38184924773) **passed** on commit `40a84aa45d6217b0b9165a80c0dc494251bdec52`. [Linux Research CI #38184924793](https://github.com/Hyperbog/RLOFT/actions/runs/38184924793) also **passed** on this exact commit. Both runs are original-media-free.

## Propagation to active owners

Original CODE.1 source Git blob SHA **`bdfe208db205fc3da467c3bf9d6710869096ddea`** is synchronized from canonical to four active consumers, each of which had exactly the previous source blob `226c563007ac4daef8f732505341175914ba2b94`:

| Active branch | Targeted source-sync commit |
| --- | --- |
| `demo-v1` | `6a473e66353f518e98cedb9a6bed23a2430a4deb` |
| `native-engine` | `90d8a0dd04a348c438082216249996fc9e4d92da` |
| `disassembler-foundation` | `87a4656b0aeed11e4ccd10fc1dccc3cf6286b344` |
| `extractor-wpf-fluent` | `3dac6389f2bd35a2b0e2ad37dec398d638468c26` |

The immutable `DnD-Classics` original-media branch, historical G2 candidate snapshot, and archived recovery tags were not changed. C++ source identity synchronizes an original semantic correction; **it does not prove that any current native executable routes its game state through the corrected function**.

## Full-survey integrity correction

The initial 1,543-address bulk survey of 142,696 original bytes reported 294 decode conflicts. A systematic audit found **292 are only artificial 160-byte truncation artifacts**, independently confirmed by decoding the original code past the synthetic window boundary. `bulk_cfg.py` now preserves those separately under `artificial_window_tail_sites` with a checked lookahead instruction size; the only **two surviving linear-decode conflicts represent the one intentionally overlapping source target at B04C**.

The corrected survey output (private downloadable JSON artifact, not original code bytes) reports **1,543 source-address candidates, 142,696 selected original bytes, 46,793 linear instruction candidates, 44,143 statically traversed instruction candidates, 10,189 candidate blocks, 292 window-tail artifacts, 2 overlapping-target records, and 59 indirect-edge observations**. It used the corrected CI C++ index and the existing CODE.1 1,041-byte protected noncode ledger.

[Source-only research CI #38184771603](https://github.com/Hyperbog/RLOFT/actions/runs/38184771603) passed the improved classification tests: 37 ResearchInfra tests and 77 SharedDecomp tests. **None** of these candidate counts are whole-game runtime or byte-matching acceptance.

## Remaining boundaries

The legitimate B04C alternate-entry pattern should become a separate static CFG multi-entry test case, not be suppressed by blanket ignore rules. Other CODE.1/CODE.2 hidden instruction overlaps require independent proof per source slice; do not generalize one witness to every mid-instruction target. Resolve the separate 59 indirect-edge sites through original fixup/jump tables and DOSDis candidate xrefs; continue source-backed DOS initialization and heap allocator work.
