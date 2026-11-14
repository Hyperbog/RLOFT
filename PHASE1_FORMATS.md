# Phase 1 — Cross-title original binary format survey (foundation)

**Status: first structural component complete.** The original media were inspected read-only from GitHub branch `DnD-Classics`, preservation SHA `30b5a3025f4eea711380c416667bc350fd5c5902`. The [successful survey workflow](https://github.com/Hyperbog/RLOFT/actions/runs/43619854357) used source SHA `940e8ef5c6c31e434a78596b6808b466ee6a22b5`. Its [metadata-only artifact](https://github.com/Hyperbog/RLOFT/actions/runs/43619854357/artifacts/14123463336) contains a per-archive JSON report and complete machine-generated matrix, without copyrighted asset contents.

The existing [Strahd-format documentation](Resources/RES/FORMAT.md), [LEVELS.DAT source audit](DatFiles/LEVELS.DAT/FORMAT.md), and [Phase 0 provenance audit](PHASE0_INVENTORY.md) remain authoritative for their verified Strahd scopes. New cross-game hypotheses are never silently promoted to proved gameplay semantics.

## Reproducing the survey

```sh
python Tools/phase1_compare.py \
  --root preservation/MASTER \
  --json audit/phase1-formats.json \
  --markdown audit/PHASE1_FORMATS.md
```

The [Phase 1 workflow](.github/workflows/phase1-format-audit.yml) checks out the three preserved game directories and executes the command automatically. This is a read-only research scanner, **not a second runtime decoder**. For analysis, it chooses the installed copy when present, otherwise CD_Extracted; that choice is *not* proven DOS loader precedence.

## Shared structures and proven variation

| Property | Strahd | Menzo | Stone | Evidence / status |
| --- | ---: | ---: | ---: | --- |
| Initialized RES groups | 0..10 | 0..10 | 0..10 | Original CODE.2 instruction witnesses, Phase 0 |
| RES0 indexed records | 141 | 175 | 174 | Actual archive offset tables |
| LEVELS.DAT directory count | 32 | 56 | 43 | Exact indexed-directory validation |
| Per-level preamble | 29 bytes | 29 bytes | 29 bytes | All 131 records match |
| Candidate fixed level header | 676 / `0x2A4` bytes | 2,084 / `0x824` bytes | 2,084 / `0x824` bytes | Exact inferred span in 32/56/43 records |
| Event-script section length | `0x514` | `0x514` | `0x514` | All 131 record boundaries match |
| NPCS.DAT records | 83 | 98 | 110 | 4-byte LE count / exact file extent |
| NPC record stride | 69 / `0x45` bytes | 88 / `0x58` bytes | 88 / `0x58` bytes | Exact count × stride |
| OBJECTS.DAT records | 336 | 523 | 516 | 4-byte LE count / exact file extent |
| Object record stride | 65 / `0x41` bytes | 65 / `0x41` bytes | 65 / `0x41` bytes | Exact count × stride |
| INFO.DAT selector/path shape | Fits | Fits | File absent from checked roots | File-byte check only |
| INIT file bytes | 1,879 | 2,830 | 2,970 | Opaque pending interpretation |

All **1,375 object records** have NUL-terminated printable ASCII in their first 40 bytes; all **291 NPC records** have NUL-terminated printable ASCII in their first 20 bytes. These prefix observations support a common field location, **not** identical later fields or rule meaning.

All three LEVELS archives have a valid 4-byte count, an offset directory beginning at +4, and record-local seven-dword boundaries beginning at +1. Each record contains plausible signed 16-bit width/height at `boundary_0 + 1` and `boundary_0 + 3`, with the observed equation:

```text
boundary_1 = 0x1D + per_title_fixed_header_bytes + 2 * width * height
```

The exact fixed-header byte counts were **676 for every Strahd level**, **2,084 for every Menzo level**, and **2,084 for every Stone level**. Strahd's header fields and gameplay meaning have prior executable evidence. Menzo/Stone's expanded `0x824` header fields have **not yet** been assigned semantic names or CODE.2 read-site proofs. The shared `0x514` event-script *span* does not certify identical opcodes.

## RES archive and envelope interoperability

All **35 observed RES archives** (13 Strahd, 11 Menzo, 11 Stone) use the common little-endian count-plus-absolute-offset outer grammar, with non-decreasing offsets and zero failed EH-envelope extent checks. Zero-length entries are valid. Typed records preserve `EH` magic, type byte at record-relative +2, and signed 16-bit header length at +3.

The critical **per-title type/header differences** are:

| Case | Strahd | Menzo | Stone |
| --- | --- | --- | --- |
| RES3 | 4,234 Type-7 records, header length 0 | 3,167 Type-6, header length 5 | 2,542 Type-6, header length 5 |
| RES4 | 667 Type-6, header length 0 | 559 Type-5 + 502 Type-6, header length 5 | 963 Type-5 + 410 Type-6, header length 5 |
| RES5 | Type-3 entries, header length 0 | Type-3 entries, header length 0 | Type-3 entries, header length 0 |
| RES9 | Type-1 + raw records | Type-1 + raw + empty | Type-1 + raw + empty |

Type-1 envelope header length is 8 across observed files, and Type-2 is 136; those common physical spans are **not** proof of identical raster semantics. **Type-5 appears in Menzo and Stone but not Strahd**; neither Type-5 payload meaning nor the changed Type-6 header layout can be borrowed from Strahd's unobserved Type-5 runtime-loader branch without further executable analysis.

The active RES archive *index number* must **never** be treated as a game-independent semantic resource class. The shared reader may own archive directory validation, byte preservation, EH envelope parsing and title-aware dispatch. Asset interpretation and resource lookup tables must remain title-specific until their original consumers are reconstructed.

## Media provenance and unresolved formats

- **Strahd RES11/RES12:** Legacy/pre-split CD archives, not opened by the observed CODE.1/CODE.2 eleven-group bootstrap. Preserve as optional research inputs, not default gameplay dependencies.
- **Stone RES9:** Installed/CD archive copies differ in exactly one raw record (index 574, five byte changes). File-opening precedence and changed-record behavior are not established. Preserve both versions.
- **SAVE.DAT / CHARS:** Observed Strahd save sizes include 9,288 and 9,310 bytes; Menzo's sample is 27,959; Stone's is 39,002. Only Strahd's checked-out installation includes a 2,336-byte CHARS sample. No shared save/character schema is asserted.
- **Cinematics:** Strahd has ANM files, while Menzo/Stone have DFA and some ACT files. These files were inventoried but **not decoded** in this component.
- **INIT and resource lookup:** Different INIT sizes, Stone's absence of INFO.DAT in the inspected roots, and the title-specific RES5/RES9 raw records remain unresolved.

## Phase 1 boundary and next investigation

**Established:** shared RES outer grammar, common EH envelope fields, indexed level directory/preamble/event-span boundaries, title-specific fixed-header sizes, two NPC striding families, and a common 65-byte object-record geometry. **Not established:** shared level event semantics, field-level NPC compatibility, Type-5/Type-6 decoding across titles, title-specific archive routing, or playability.

**Next Phase 1 component:** trace Menzo/Stone CODE.2 consumers for the extra `0x824` level-header region, the 88-byte NPC record fields, and Type-5/6 resource envelopes; investigate Stone RES9 record 574. Validate these against the original code before defining common C++ runtime structs.

**No executable gameplay implementation, commercial asset modification, or user playtest occurred in this survey.**
