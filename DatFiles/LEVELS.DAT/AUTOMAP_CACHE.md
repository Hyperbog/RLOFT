# LEVELS.DAT automap/cache replay audit

Status: **CODE.2 cache generator and header lookup-bank behavior are instruction-proven; supplied `LEV022.DAT` contains a reproducible cache/header-state inconsistency and is therefore not treated as proof of a fresh CODE.2 writer pass.**

The controlling executable evidence is the certified CODE.2 object-1 binary. This document tightens the earlier `CELL_WORD.md` conclusions around routine `01558`, the header tables at DGROUP `0x25622` / `0x256A2` / `0x25722` / `0x257A2`, and the supplied standalone `LEV022.DAT` sample.

## Four 64-word header lookup banks

The fixed `0x2A4` level header is loaded at DGROUP `0x25618`. CODE.2 indexes the following regions with a six-bit cell code (`0..63`) multiplied by two:

```text
header +0x00A  DGROUP 0x25622  uint16 primary_bank_0[64]
header +0x08A  DGROUP 0x256A2  uint16 companion_bank_0[64]
header +0x10A  DGROUP 0x25722  uint16 primary_bank_1[64]
header +0x18A  DGROUP 0x257A2  uint16 companion_bank_1[64]
```

The 64-entry extent is proved by direct renderer indexing with `(cell_code & 0x3F) * 2`; the four bases are exactly `0x80` bytes apart.

Renderer routines `33124..332B2`, `332B4..333B9`, and the following sibling path read a primary/companion pair and place those values into render work records. The safest current name is therefore **render/resource lookup bank**. Exact editor-facing meanings of the two banks and the primary/companion distinction remain unresolved.

## Dynamic mutation routine `1EDAC..1EEFA`

CODE.2 mutates both primary banks in place. For each of the first 63 entries (`0..62`), it scans ten source/replacement word pairs from DGROUP `0xBFBC` and replaces the first matching source value. The immutable object-2 data contains:

```text
348 -> 349
349 -> 350
350 -> 348
170 -> 171
171 -> 170
283 -> 284
284 -> 285
285 -> 283
257 -> 258
258 -> 257
```

The same routine also rotates primary values `0x242..0x244` and `0x245..0x247`. When it performs either rotation it writes the matching companion entry as `primary + 6`.

This is direct proof that at least part of the header lookup state is mutable animation/render state rather than immutable level geometry.

## Exact `01558` rules relevant to ordinary cells

Routine `01558..01830` rebuilds the byte cache at DGROUP `0x1760`.

For the ordinary paths relevant to supplied Level 22:

- `cell & 0x0400` is the explored test.
- `cell & 0x003F` supplies the six-bit lookup index.
- `primary_bank_0[cell_code] == 0x0093` selects cache class `0x2E` or `0x2F`.
- This `0x0093` test occurs in **both explored and unexplored nonzero-cell paths**.
- An unexplored nonzero cell whose primary lookup is not `0x0093` is assigned boundary class `0x01` when any of its eight neighbors is both explored (`0x0400`) and code zero; otherwise it remains `0xFF`.
- Explored code-zero cells select `0x2E` or `0x2F`.
- Other branches handle `0x0800`, `0x0080`, the 10-byte record lookup, and the party-position override.

The party-position override cannot explain the Level 22 inconsistency below because the contradictory cells are unexplored and `0x0093` itself already deterministically selects `0x2E/0x2F`.

## Supplied `LEV022.DAT`: deterministic contradiction

The serialized Level 22 header contains:

```text
primary_bank_0[1] = 0x0093
primary_bank_1[1] = 0x0093
```

The serialized cell layer contains 79 code-1 cells:

```text
46 explored code-1 cells
40 unexplored code-1 cells
```

(`33 + 7 = 40` unexplored code-1 cells; the remaining code-1 count is explored.)

Its byte cache contains:

```text
0x2E : 313
0xFF : 130
0x01 :   7
```

Replaying only the deterministic `01558` rules against the file's own serialized header and cells yields **40 contradictions**:

```text
33 code-1 cells cached as 0xFF
 7 code-1 cells cached as 0x01
```

Because `primary_bank_0[1] == 0x0093`, all forty of those cells would have to become `0x2E` or `0x2F` in a fresh `01558` pass.

The seven `0x01` locations are nevertheless highly structured: they are **exactly** the seven unexplored code-1 cells having at least one explored code-zero neighbor under the eight-neighbor boundary rule. If code 1 is provisionally treated as an ordinary solid instead of a `0x0093` class, the predicted boundary set is exactly those seven cells.

Conversely, all 46 explored code-1 cells are cached as `0x2E`, which is the result expected when lookup entry 1 is `0x0093`.

Therefore the supplied cache behaves as a hybrid of two mutually incompatible lookup classifications for code 1. A single fixed `primary_bank_0[1]` value cannot generate both behaviors during one execution of the certified `01558` routine.

## What this proves — and what it does not

This does **not** invalidate the standalone file format. `LEV022.DAT` still validates the structural layout, counts, expanded 22-byte records, and the previously proved 110/110 family-B compact-to-runtime conversion.

It does change the evidentiary status of the final byte cache:

- CODE.2 proves that its writer calls `01558` immediately before serializing DGROUP `0x1760`.
- The supplied `LEV022.DAT` cache is not internally consistent with a fresh execution of that routine using the header and cell state serialized in the same file.
- Therefore this specific file must not be used as proof that its cache is a byte-for-byte specimen freshly generated by the certified CODE.2 writer.
- At least one of the header lookup state, cell state, cache state, or producing executable/runtime state differs from the single-pass state assumed by a direct replay.

Possible provenance explanations (for example an authored/stale cache or a different runtime build/state) remain hypotheses and are not promoted to facts without additional samples.

## Safe reconstruction rule

A parser/repacker should preserve the serialized cache losslessly. A future recompiler may regenerate it with the certified `01558` algorithm only when the complete live lookup state, special 10-byte-record state, and party-position state are available and the intended behavior is explicitly to rebuild rather than preserve the original bytes.

Use `audit_automap_cache.py` to reproduce the deterministic consistency check.