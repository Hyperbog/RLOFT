# Compiler fingerprint pilot — relocation-checked ELF32 controls (2026-10-11)

Branch: `code1-decompilation`. **Original-game binaries remain unchanged and
private. This experiment does not identify the historical compiler.**

## Local measured result

The immutable game input is shipped `CODE.2` (558,689 bytes,
SHA-256 `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`).
We compiled source-only test hypotheses with GCC 14.2.0 and
Clang 17.0.0, targeting 32-bit x86 ELF relocatable objects:

```text
-m32 -O2 -fno-pic -fno-pie -ffreestanding
-fno-asynchronous-unwind-tables -fno-unwind-tables
-fno-stack-protector -fomit-frame-pointer
-fno-function-sections -c
```

All comparison results require (a) pinned original CODE.2 identity,
(b) a single true ELF32/i386 `STT_FUNC` symbol,
(c) the **actual** `R_386_32` relocation whose named target has an
explicit original-data address, and (d) equal length and every byte
equal after resolving only that recorded relocation. No blanket
four-byte address masks are permitted.

| Original CODE.2 object-1 | Hypothesis | GCC 14.2 | Clang 17 |
| --- | --- | --- | --- |
| `2E740..2E749` | F0CC setter | 10/10 exact | 10/10 exact |
| `2E74C..2E755` | F0C8 setter | 10/10 exact | 10/10 exact |
| `3058C..30591` | DWORD getter 2B684 | 6/6 exact | 6/6 exact |
| `30594..30599` | DWORD getter 2B680 | 6/6 exact | 6/6 exact |
| `2E758..2E767` | guarded indirect F0C8 call | **NOT exact**: 17 versus 16 bytes | **NOT exact**: 16 bytes, first difference at +10 |

**Conclusion:** 4/5 matches for **both** unrelated modern compiler
families demonstrate that simple getters/setters cannot establish
Watcom compiler provenance. The original indirect-call trampoline
uses `FF 15` (call through memory) with guarded return, while
optimized modern output favored a tail transfer; control-flow
and optimization options matter.

**Scope restriction:** The addresses filled into `R_386_32`
relocations are independently observed machine-code literals.
The full **original LE fixup/relocation tables have not been
reconstructed** by this tool, so its result is *relocation-resolved
byte equality of one emitted ELF function*, **not proof that the
historical executable was produced by that compiler**. The
source-only diagnostic C is not certified as a faithful historical
reconstruction.

## New repeatable local tools

- `Code2/tools/compare_fingerprint_elf32.py`: strictly
  parse little-endian i386 ELF relocatable symbol/section/REL
  records, verify exact original CODE.2 identity, reconstruct
  each approved global relocation using a symbol-specific
  address, and report unrelocated hashes, location/type, exact
  equality and the first mismatch. Explicitly reject unsupported
  RELA, ELF64, non-`R_386_32` fixups, unknown symbols,
  missing fixups, and arbitrary executable versions.
- `Code2/tools/run_elf32_controls.py`: rerun 5 probes
  under each locally installed GCC/Clang, capturing version,
  command options and metadata-only JSON outcomes.
- `Code2/tools/probes/elf32_negative_controls.c`: tiny,
  independent compiler hypotheses, not original Ravenloft
  source and containing no copyrighted game binary bytes.
- `Code2/tools/test_compare_fingerprint_elf32.py`:
  six local tests verify ELF rejection, bounds, identity
  failure, original-byte relocation resolution, an indirect-call
  mismatch, and rejection of an intentionally tampered
  `R_386_PC32` relocation.
- Existing packet generator now rejects **overlapping**, not
  only identical, assignments. Adjacent spans are allowed.

Locally verified commands:

```powershell
python Code2/tools/test_make_function_packets.py
$env:RLOFT_ORIGINAL_CODE2="C:\Games\Ravenloft\CODE.2"
python -m unittest discover -s Code2/tools -p "test_compare_fingerprint_elf32.py"
python Code2/tools/run_elf32_controls.py --original-code2 "$env:RLOFT_ORIGINAL_CODE2" --compiler gcc --compiler clang --output "$env:TEMP\rloft-elf32-controls.json"
```

These are **development tools**, not a command-line edition of
the Windows desktop game. No CI workflow has been dispatched,
and the original executable is never committed or uploaded.

## Watcom toolchain provisioning and OMF gate

The decomp.me maintained compiler catalogue explicitly includes
DOS/x86 Watcom C **10.0a, 10.5, 10.5a, 10.6, 11.0**:
https://github.com/decompme/compilers/blob/main/values.yaml

Confirmed release references (download manually into a private
directory, verify before executing):

| Version | Archive source | Size / pin |
| --- | --- | --- |
| 10.0a | https://github.com/decompme/compilers/releases/download/compilers/wcc10.0a.tar.gz | 9,247,235 bytes; SHA-256 `920a9674d1b10f227ae65171c187212b06d7fc38b263313922615c4f5e9b24a6` |
| 10.5 | https://github.com/OmniBlade/decomp.me/releases/download/wcc10.5/wcc10.5.tar.gz | 61,776,575 bytes; no published digest in release API |
| 10.5a | https://github.com/OmniBlade/decomp.me/releases/download/wcc10.5/wcc10.5a.tar.gz | 61,755,170 bytes; digest requires pinning |
| 10.6 | https://github.com/OmniBlade/decomp.me/releases/download/wcc10.5/wcc10.6.tar.gz | 54,523,142 bytes; digest requires pinning |
| 11.0 | https://github.com/OmniBlade/decomp.me/releases/download/wcc10.5/wcc11.0.tar.gz | 43,187,977 bytes; digest requires pinning |

This execution environment has no installed Watcom compiler,
no Docker/Podman/Wine/DOSBox driver, and **cannot resolve external
download hosts**. No Watcom compile run or exact Watcom matching
claim is made. **Next requirement:** provision an approved
historical `wcc386` toolchain and validate a Watcom
**OMF32** object disassembly/relocation pathway (or a proven
OMF→COFF converter with fixup preservation).

The normal `objdiff` matching workflow expects
relocatable objects, but its separate OMF-support discussion
shows why **Watcom OMF is not automatically covered by x86
architecture support**:
https://github.com/encounter/objdiff/issues/97

A proposed OMF conversion must prove segment, group, symbol,
and FIXUPP32 relocation conservation before any original
LE-byte matching percentages are published. The community's
OMF-aware binutils work is referenced in that issue. Do not
publish 4/5 ELF negative controls as progress toward historical
Watcom codegen matching.

## Next high-value experiment

After provisioning a verified 10.0a archive, compile these
original-source hypotheses with its 32-bit C compiler and
test across multiple optimization/calling-convention flags.
Use the native Watcom `wdis` and a strict OMF fixup
extractor rather than assuming an ELF32 section layout.
Record compiler executable hash, precise flags, object hashes,
original-source identity, relocation proof, and function-by-function
differences. Repeat with 10.5/10.5a/10.6/11.0 only when their
archive and runnable compiler identities are independently pinned.

Continue native gameplay parity separately on `demo-v1`;
neither source byte accounting nor a compiled opcode match
certifies rendering/input/save/timing equivalence.
