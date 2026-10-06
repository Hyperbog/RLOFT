# R.EXE Binary Baseline and Verified Launcher Evidence

Status: **ACTIVE / evidence-first reconstruction**

This document records observations made directly against the shipped `R.EXE`. Interpretive names are intentionally withheld where the bytes do not yet prove gameplay semantics.

## File identity

- Size: **12,368 bytes**
- Container: **MS-DOS MZ executable**
- MZ header size: **0x200 bytes** (`e_cparhdr = 0x20` paragraphs)
- Initial CS:IP: **0000:0000**, therefore the load-module entry begins at file offset **0x200**.
- Relocation count: **24**.
- Embedded compiler/runtime identification: `Borland C++ - Copyright 1991 Borland Intl.`

These observations support classifying R.EXE as the small real-mode launcher/supervisor rather than one of the large protected-mode gameplay modules.

## Embedded runtime/game strings

The following strings occur in the shipped file at the indicated file offsets:

| File offset | String |
|---:|---|
| `0x2AC0` | `Cannot find WORK subdirectory` |
| `0x2ADF` | `No Mouse Installed.  Ravenloft requires a mouse to run.` |
| `0x2B18` | `WORK` |
| `0x2B2C` | `DOS/4GW Professional Protected Mode Run-time  Version 1.96` |
| `0x2B68` | `Copyright (c) Rational Systems, Inc. 1990-1994` |
| `0x2B98` | `DOS4G=QUIET` |
| `0x2BA4` | `WORK` |
| `0x2BA9` | `AUTOMAPS` |
| `0x2BB2` | `code.2` |
| `0x2BB9` | `code.2` |
| `0x2BC0` | `code.1` |
| `0x2BC7` | `code.1` |
| `0x2BCE` | `CHARS` |
| `0x2BD4` | `CHARS` |
| `0x2BDA` | `DOS4G=` |

The duplicate CODE and CHARS strings are significant but their exact call-site roles remain subject to instruction-level tracing.

## Proven architectural boundary

The launcher contains names for both `code.1` and `code.2`, the DOS/4GW runtime banner, DOS4G environment strings, and game working-directory names. This is consistent with a supervisor that prepares the DOS environment and invokes the separately reconstructed CODE modules.

The project must distinguish three layers:

1. **R.EXE** — real-mode launch/supervision and process-level orchestration.
2. **CODE.1 / CODE.2** — protected-mode game modules already reconstructed separately.
3. **DOS/4GW** — third-party protected-mode runtime/extender used by the CODE executables.

The presence of the DOS/4GW banner inside R.EXE must not by itself be interpreted as proof that R.EXE's own application body is a protected-mode CODE module; its MZ structure, small size, Borland runtime identity, and module-dispatch strings support the launcher classification.

## Exit-protocol status

Previous launcher tracing established a control protocol involving child return values `10` and `12`, including a CODE.2 return path that deletes `CHARS` before cycling again. Those numeric transitions remain useful machine-level facts.

**Gameplay meanings for 10 and 12 are not yet certified.** Repository-wide text search did not expose an already documented source-level semantic assignment for those values. The next certification step is therefore binary/source cross-correlation against the CODE.1/CODE.2 termination paths, not inference from launcher behavior alone.

Similarly, `CHARS` is proven to participate in the launcher lifecycle, but its exact file format and semantic role must be established from CODE producer/consumer paths before assigning an archival name beyond `CHARS handoff file`.

## Immediate reconstruction tasks

1. Locate CODE.1/CODE.2 termination wrappers and all callers that can supply status 10 or 12.
2. Trace backwards from those callers to user/game-state conditions.
3. Locate all `CHARS` open/create/read/write references in the protected-mode modules.
4. Determine which module produces CHARS and which consumes it.
5. Prove whether CHARS represents character creation/import, party transfer, temporary state, or another handoff mechanism.
6. Only after those proofs, replace numeric protocol labels with semantic names.

## Preservation rule

Do not infer semantics merely because a state transition resembles a familiar game flow. Numeric exit values, filenames, and process ordering remain the canonical labels until CODE-side evidence closes the loop.
