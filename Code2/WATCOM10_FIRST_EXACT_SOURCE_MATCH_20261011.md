# First genuine Watcom C 10.0a matching pilot — original CODE.2

Date: 2026-10-11. Canonical branch: `code1-decompilation`. Native-game integration remains independent.

## Executed historical compiler: two Windows probe runs

An isolated, source-only `windows-2022` GitHub Actions workflow was added at `.github/workflows/watcom10-compiler-probe.yml`. It checks out repository source with read-only permissions, verifies the owner-approved Watcom 10.0a release by its pinned SHA-256, safely extracts the compiler, compiles five newly authored function hypotheses, and uploads only those OMF objects and diagnostic metadata. It **does not dispatch Windows Full CI**, compile the native game, upload CODE.2, publish game bytes, or run downloaded binaries without first validating the published compiler archive.

- [Initial genuine Watcom compilation](https://github.com/Hyperbog/RLOFT/actions/runs/38169552232), four successful OMF objects; published artifact SHA-256 `2214b3ec801cab5dbe8dd63d158ec5ac72d2d5719d7fa1ba093b686eaf2d3fe5`.
- [No-stack-checking comparison](https://github.com/Hyperbog/RLOFT/actions/runs/38169969921), four successful OMF objects; published artifact SHA-256 `54ce4e30bd6763bd940ec242dfc63b33ce0143b608041d697a4e9429ebe26616`.

Both artifacts were downloaded for independent local analysis. The original CODE.2 binary is 558,689 bytes, SHA-256 `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.

## Explanation of the first discrepancy

All four initial profiles (without `-s`) emitted a stack-overflow checking prelude containing a call via an OMF relocation to `__CHK`. The original five CODE.2 functions do not contain that prelude. Source inspection showed that the remainder of the generated optimized 386 routines was identical after resolving external variable addresses, but **manually removing instructions was not accepted as a compiler match**.

Watcom documentation identifies `-s` as the option that disables generated stack-overflow checking. Running genuine `wcc386.exe` again with `-s` eliminated both the `__CHK` call and its relocation. Each optimized 386 OMF object now has five public functions and exactly **six 32-bit absolute external-variable fixups**, as expected from the original instructions.

References: https://ftp.openwatcom.org/ftp/manuals/current/tools.pdf and https://open-watcom.github.io/open-watcom-v2-wikidocs/cguide.pdf

## Pinned original spans, including explicit alignment

Original object-1 offsets below are half-open and measured from CODE.2's executable object (physical file base `0x3AF24`). The first four target spans include the verified **`8B C0` two-byte interfunction padding**, not merely the routine body. This distinction matters: treating PUBDEF-to-next-PUBDEF as function body alone causes a false mismatch.

| Target | Object-1 span | Size | Original span SHA-256 |
| --- | --- | ---: | --- |
| F0CC setter | `2E740..2E74C` | 12 | `4f04d3978215298d3dfb158f4d8b8476c9b1df1f01505cbf21153d0fddf54a3e` |
| F0C8 setter | `2E74C..2E758` | 12 | `ee71c3d92af7ab0a952c3dd6243542249d199a9ed3320d78a449711c06ba21e0` |
| DWORD getter A | `3058C..30594` | 8 | `dcf75714bd9f236c0923f4cae20b5377105cb8b5c24ed371d8cae18c29295acc` |
| DWORD getter B | `30594..3059C` | 8 | `576ae5640d8d4b10a3d95da6ccf9098113db564171c0936d476a8f8fa5bd00fd` |
| F0C8 indirect trampoline | `2E758..2E768` | 16 | `dab1ea08314669c9073f77d62e5ef9cbeec0045753bf87746def521896a22858` |

Total: **56 identical bytes per fully matched 386 profile**, 48 instruction-body bytes and 8 preserved alignment bytes. These are authenticated original machine-code hashes, not copied original bytes.

## Experimental result after real OMF relocation resolution

| Watcom 10.0a options | Exact spans | Interpretation |
| --- | ---: | --- |
| `-bt=dos -3s -ot -s -zq` | **5/5** | all five including exact Watcom 386 alignment |
| `-bt=dos -3r -ot -s -zq` | **5/5** | same five; functions do not discriminate ABI |
| `-bt=dos -4s -ot -s -zq` | **1/5** | 486 alignment sequences differ on four routines |
| `-bt=dos -3s -od -s -zq` | **0/5** | unoptimized code generation differs |

`Code2/tools/match_watcom_omf.py` is a fail-closed offline matching verifier. It authenticates OMF record/checksum/segments using the shared inspector, reconstructs fully covered CODE LEDATA ranges and PUBDEF spans, resolves **only** appropriate OMF location-9 absolute external fixups with independently verified original DGROUP addresses, then hashes the complete reconstructed span against the pinned CODE.2 span. It does not mask arbitrary bytes, ignore unknown code-bearing records, or report a match with unsupported/missing fixups. The corresponding synthetic OMF regression tests reject corrupted instructions, missing fixups, self-relative fixups and incorrect targets. The initial historical `__CHK` objects remain 0/5 without manual surgery.

**Scope:** This establishes genuine Watcom C 10.0a **recompilation instruction equality** for these five small probes. It does **not** uniquely identify the game's compiler version across other Watcom releases, prove the calling convention (stack and register profiles both match), reconstruct original LE fixup records, match a large branching/gameplay routine, certify CODE.1, or establish original-DOS/native runtime parity. Track these gates separately.

## Next experiment

Choose several *nontrivial* source-authenticated routines with branches, loops, local stack frames, multiple arguments and internal call edges. Expand OMF handling only for relocation types observed in genuinely emitted objects, and normalize original LE relocation evidence. Test Watcom 10.0a versus 10.5/10.5a and varied optimization switches only where these larger functions discriminate them. Keep normal Windows Full CI owner-authorized and separate.
