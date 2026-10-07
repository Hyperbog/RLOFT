# CODE.2 title cursor color audit — 2026-10-07

## Symptom

The native Demo v1 title/menu displayed the default cursor with the raw RES0
indices under COLORS palette 12, producing green/orange colors. The original
DOS title menu displays the same cursor in white/gray/red. Character creation
was already correct.

## Controlling CODE.2 path

The original executable does not draw the untouched cursor on the title menu.

- `1A0B0` loads `RES0[INIT word 0 + 0x2F]` and copies it to runtime cursor
  buffer `880D`.
- `19464` immediately calls `19378`.
- `19378` clears a 256-byte translation table at `759C`, patches nineteen
  entries, then calls `2AA9C(0, 759C, 880D)`.
- `2AA9C` translates cursor record 0 in place while leaving the transparent
  key unchanged.
- `1A288` subsequently draws that translated cursor.

The exact `19378` substitutions are:

```
08->EB  1B->FF  16->F9  09->EC  1A->FE
0B->EF  12->F5  10->F3  0C->EF  0D->F0
0A->ED  5C->AC  58->53  55->44  53->0A
56->45  57->45  54->42  59->53
```

All unpatched nontransparent indices map to zero because `19378` clears the
table rather than initializing it as an identity table.

## Palette cross-check

CODE.2 `325B0` reads COLORS row 12 from file offset `1 + 12*768`, matching
DemoV1's `colors_palette(12)` exactly. Its subsequent `32150` rescaler uses
environment byte `+0x6C`. On fresh startup that byte remains zero, so
`32150` returns without changing the title palette. The palette loader is not
the source of this discrepancy.

## Fix

DemoV1 now creates a separate CODE.2 menu cursor by reproducing
`19378 -> 2AA9C` exactly. The raw cursor remains intact and is restored on the
CODE.2 -> CODE.1 handoff, preserving the already-correct character-creation
cursor.
