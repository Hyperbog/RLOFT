# CHARS xref investigation checkpoint

Status: **ACTIVE — binary xref reconstruction**

## Primary binary observations

The original shipped binaries were rechecked directly for literal `CHARS` occurrences:

- `CODE.1`: size 277,787 bytes; one literal occurrence at file offset `0x3E7B8`.
- `CODE.2`: size 558,689 bytes; two literal occurrences at file offsets `0x76634` and `0x77064`.

The second CODE.2 offset corrects the earlier provisional note that placed it at `0x77062`; the exact literal begins at `0x77064`.

Both binaries identify as DOS/4G-bound MS-DOS executables. GNU `objdump` in the current analysis environment does not recognize their bound executable format directly, so a trustworthy code-xref cannot be obtained by naively treating file offsets as flat 32-bit virtual addresses.

## Important repository finding

`Code2/Code2.cpp` is a 1,304,770-byte reconstruction in Git, but the GitHub text-content wrapper returns an empty body for this large file. Therefore repository-side semantic correlation must use either targeted indexed search, continuity/audit artifacts, or a binary/decompiler-derived address map rather than assuming that an empty fetch means the reconstruction is empty.

## Current evidentiary limit

At this checkpoint the following remain proven:

1. R.EXE supervises CODE.1/CODE.2 and interprets child return values.
2. Status 10 participates in continuation/module handoff.
3. CODE.2 status 12 causes R.EXE to delete `CHARS`, normalize the supervisor state to 10, and continue.
4. Both CODE modules contain `CHARS`, with CODE.2 placing one occurrence in its character-generation/save-state string neighborhood.

The exact CODE-side operations associated with each literal — open/read, create/write, existence test, rename, or another library-mediated file operation — are **not yet certified**. Likewise, semantic names for exit values 10 and 12 remain intentionally withheld.

## Next proof step

Build/obtain the executable object/segment map for CODE.1 and CODE.2, translate the `CHARS` data locations into runtime addresses, then locate code references to those runtime addresses. Correlate those xrefs with the already reconstructed functions and trace control flow to termination/status producers.

This avoids the common reverse-engineering error of searching raw file offsets as if they were runtime pointers in a DOS/4G bound image.