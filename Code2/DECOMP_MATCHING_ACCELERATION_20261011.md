# Ravenloft: source-matching acceleration with decomp.me and decomp.dev

Research and first tooling adoption, 2026-10-11. Canonical decompilation
branch: `code1-decompilation`. Normal gameplay and Win32
runtime changes belong to `demo-v1`.

## What the faster decompilation projects actually automate

- **decomp.me** is a *function* scratch workspace: target assembly,
  contextual structs/prototypes, C/C++ hypothesis, compiler and flags,
  fast recompile, instruction differences and collaboration via forks.
  It does not ingest an entire game to automatically recover source.
- **decomp.dev** is primarily the *progress display and GitHub bot*
  for reports from objdiff. It does not decompile game binaries.
- **objdiff** supports x86 and provides automatic rebuild, instruction
  comparison and progress reporting. However, the original Ravenloft
  files are **DOS/4GW Professional bound LE executables**, not
  conventional unlinked relocatable `.obj`/`.o`
  files. Native object matching needs verified object segmentation
  and LE fixup/relocation handling, not a misleading raw-byte score.
- **splat / decomp-toolkit / Ghidra / m2c / decomp-permuter** accelerate
  segmentation, initial prototypes, type context and candidate
  generation. Their console-specific parsers cannot simply be
  redirected at a bound LE file. Reuse the *process*, not unsupported
  target-format assumptions.

References:
- https://www.decomp.me/faq
- https://www.decomp.me/platform
- https://github.com/decompme/compilers/blob/main/values.yaml
- https://github.com/encounter/objdiff
- https://github.com/encounter/decomp.dev
- https://github.com/encounter/decomp-toolkit
- https://github.com/decompals/decompedia
- https://github.com/Fuuzetsu/tenchu-decomp/blob/master/docs/decomp-dev.md

## The strongest immediate opportunity: identify original Watcom codegen

The decomp.me Microsoft DOS x86 compiler definitions include:
**Watcom C 10.0a, 10.5, 10.5a, 10.6 and 11.0**, plus Borland and
Microsoft alternatives. That matters because the original shipped
Ravenloft executables contain DOS/4GW Professional and Watcom
runtime idioms (see `Runtime/CODE_OBJECT_MAP.md`).

Neither exact original compiler **version**, optimization flags,
register calling convention nor complete LE relocation semantics
are yet independently identified. Watcom 10.0a is a **hypothesis**
for first testing, not a determined fact. Comparing its output
against diverse *small* authentic machine functions is much more
informative than assuming the compiler from the game year alone.

The local first-pass probe queue
`Code2/tools/first_watcom_probes.json` contains five
authenticated CODE.2 selections:

| Object-relative half-open range | Role / reason |
| --- | --- |
| `2E740..2E74A` | F0CC setter, argument/ABI probe |
| `2E74C..2E756` | F0C8 setter, closely related control |
| `2E758..2E768` | conditional indirect callback |
| `3058C..30592` | simple DWORD global getter |
| `30ABC..30AD7` | modifier-mask getter and Space-latch reader |

The final selection includes **two adjacent small functions**; split
into exact instruction-bounded functions before asserting a compiler
match. Original machine bytes may have relocation fixups, embedded
tables and shared tails. A textual asm or instruction similarity
score is not itself a verified byte-exact rebuilt image.

## Adopted offline packet generator

`Code2/tools/make_function_packets.py`:

1. Reads a user-supplied original `CODE.1` or
   `CODE.2`, requiring exact file length, original
   SHA-256, and known executable object-1 identity.
2. Validates explicit *half-open object-relative* targets and
   fails closed for invalid, duplicate or out-of-range spans.
3. Creates a private packet per function with original identity,
   logical and physical file offsets, slice SHA-256, and explicit
   `compiler_match=not_tested` and
   `native_runtime_parity=not_tested` states.
4. Optionally calls local GNU `objdump` to capture
   tentative instruction text. This is a **linear-sweep candidate**
   rather than proof of actual instruction starts or GAS-ready
   assembly for decomp.me.
5. Never auto-uploads a scratch, copies copyrighted original game
   bytes into Git or creates public matching percentages.

Portable Python3 / Windows developer usage (GNU binutils optional):

```powershell
python Code2/tools/test_make_function_packets.py
python Code2/tools/make_function_packets.py --binary "C:\Games\Ravenloft\CODE.2" --targets Code2/tools/first_watcom_probes.json --output "$env:TEMP\rloft-function-probes" --disassemble --objdump objdump.exe
```

The local prototype passed five source-only unit tests, validated
the exact supplied CODE.1 and CODE.2 original binaries, and emitted
five sample function packets containing the original selections'
hashes and candidate disassembly. **No exact recompilation match
or complete Watcom compiler pilot has yet been run.**

## Why our existing 100% accounting must be measured separately

`Code2/PROGRESS.md` records 239,698 / 239,698
executable object-1 bytes accounted (235,022 instruction bytes,
4,676 proven non-code/data bytes), with 100% historical semantic
audit coverage. Post-certification defects still required repairs.

This is *not* 100% Watcom-recompiled source equivalence, not
100% engine integration, and not a DOS-equivalent native playthrough.
Track five independent gates in all future reports:

1. Original **byte ownership**: instruction/data classification.
2. Original **semantic audit**: control flow, register aliases,
   selectors, interrupts, state, original quirks.
3. **Compiler-exact** matched instructions under pinned toolchain
   and relocation-normalization proof.
4. **Native integrated** build/runtime and test coverage.
5. **Original DOS parity**: pixels, timing, input, save/load,
   all-game progression.

No decomp.dev/objdiff v2 report should be published by projecting
gate 1 or 2 into gate 3. A real progress report requires a verified
compiler and acceptable original relocatable matching units.

## Working process and next experiments

- Claim a bounded function/cluster and record owner and original
  version to avoid parallel duplicated work.
- Use canonical `Code1/Code1.cpp` /
  `Code2/Code2.cpp` and prior source byte tests
  before producing a compiler hypothesis.
- Test Watcom version+flags locally against 3–5 known tiny
  routines first, then one callback/indirect-call routine, recording
  original byte SHA, emitted bytes, relocation normalization and
  mismatching instructions for every experiment.
- If byte-exact matches become reproducible, build local x86
  objdiff work units only after validating original LE→relocatable
  transformation. A future decomp.dev dashboard may consume
  honest objdiff v2 reports from appropriately approved workflows.
- Continue original-game native integration separately on
  `demo-v1`; do not trade runtime fidelity for an
  instruction-similarity score.

**External service restrictions:** decomp.me's FAQ explicitly
asks visitors not to automate repeated requests or hook an LLM
to the service. Its privacy policy states submitted scratch data
is retained and may be part of public research exports. No raw
game assembly, binaries, traces or source should be uploaded
automatically or publicly shared without express owner direction.
These project-local tools make **no network calls**.

**Still open:** exact Watcom version/flags, original object/LE
relocation normalization, any proven recompiled match, external
scratch submission, decomp.dev registration, current Windows Full
build and actual DOS runtime parity.
