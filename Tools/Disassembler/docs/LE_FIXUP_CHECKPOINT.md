# Part 15 — LE relocation and mapped-memory checkpoint

This milestone implements a **bounded subset** of LE fixups, not every kind allowed by the linear executable specification. No external symbol/selector resolver or executable code/data classifier is present yet.

## Original evidence

- `CODE.1`: 2,617 records, 2,617 internal source-type `0x07` absolute 32-bit offsets; 1 record uses a negative source offset crossing a page.
- `CODE.2`: 9,813 records, all internal source-type `0x07`; 6 records use negative source offsets.
- The record page table is at LE header `+0x68` (page count + 1 32-bit indexes); record table at `+0x6C`; these offsets are **LE-header-relative**, unlike stored page bytes referenced from the nested application-module origin.
- The historical CODE files are not shipped with the tool. Local historical tests accept user-supplied copies and verify the expected SHA-256 hashes before analysis.

See IBM's *Linear eXecutable Module Format* §§2.11–2.12 and Open Watcom `exeflat.h` fixup constants. Treat original bytes as controlling evidence.

## Current supported fixups

- Internal target only (type `00`), source `0x07` absolute 32-bit offset or `0x08` self-relative 32-bit offset.
- 8-bit or 16-bit target-object numbers.
- 16-bit or 32-bit target offsets, 16-bit or 32-bit additive values.
- One signed 16-bit source offset or a list of signed 16-bit offsets.
- Duplicate boundary fixups are accepted only when they produce identical relocated bytes. Conflicting patches fail without modifying original evidence.
- A lazily reconstructed memory view copies stored object pages, zero-initializes virtual tails, then overlays relocation values. `ReadStoredBytes()` always returns original file-backed bytes.

Unsupported fixup kinds (imports, selectors, far pointers, chaining, aliases, source types other than `0x07` and `0x08`) explicitly fail loading with `NotSupportedException` rather than generating guessed bytes. Non-standard/iterated/compressed LE page representations still fail in the Part 14 loader. Selector IDs and runtime load-base changes cannot be inferred from these raw binaries alone.

## Commands

```sh
dosdis inspect CODE.2
dosdis fixups CODE.2 --max 25
dosdis memory CODE.2 --object 1 --offset 0xBA7 --count 4
dosdis disasm CODE.2 --object 1 --offset 0x2975C --count 16
dosdis disasm CODE.2 --object 1 --offset 0x2975C --count 16 --relocated
```

`disasm` defaults to original file bytes. Using `--relocated` explicitly requests a loaded-memory representation; the distinction is surfaced in JSON. Neither mode assumes an arbitrary address contains executable instructions.

## Acceptance

Synthetic fixtures validate source lists, self-relative fixups, 32-bit targets/addends, signed negative offsets crossing pages, malformed tables, and the exact separation of original versus relocated memory. Authorized local fixture tests validate all 2,617 + 9,813 original records, entry maps, and representative relocated pointer bytes. CI runs no proprietary game files.

## Remaining work

Finish the broader LE fixup variants, real-mode MZ loader and execution contexts; add typed relocation references to the analysis database and a verified code/data classifier; implement a complete WPF desktop application. No high-level language generation is part of this work.
