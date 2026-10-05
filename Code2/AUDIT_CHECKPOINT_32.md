# CODE.2 semantic audit checkpoint 32

## Scope

- Previous frontier: `0x32620`
- Audited ownership-complete range: `0x32620..0x339A1`
- Bytes audited this checkpoint: **4,994**
- Cumulative semantic audit: **211,346 / 239,698 = 88.1722%**
- Semantic defect classes: **6** (no new class established in this checkpoint)
- Next frontier: `0x339A2`

This pass closes reconstruction batches **82 through 95** in one semantic-audit checkpoint. The smaller original reconstruction batches are intentionally combined here to keep audit throughput materially higher than the decompilation phase while preserving ownership boundaries.

## Immutable source revalidation

Revalidated directly from `/mnt/data/CODE.2` before audit:

- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`
- extracted object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`

Mandatory previous-frontier overlap:

- `[0x325E0,0x326A0)` = 192 bytes
- SHA-256: `58de01293de72c3c9b2b4f6e43920ba483aca96fb88e5c143fd9e29525b9a3ad`
- reproduces checkpoint 31 exactly.

Checkpoint span:

- `[0x32620,0x339A2)` = 4,994 bytes
- SHA-256: `f7eb6bc72adcd0d7789f8c4c06842a684180718754e15c44cf8ccf69f4935f3`

Next-frontier overlap:

- `[0x33962,0x33A22)` = 192 bytes
- SHA-256: `9439f44a8c3bb0656f1edffefca7ab474c87436f02e18c7238f30dfb08d4f0c4b`
- confirms the `3395C` wrapper/return boundary, alignment at `339A0..339A1`, and independent routine entry at `339A2`.

## Ownership families reviewed

### `32620..32953` — compact wrappers and descriptor/state helpers

Preserved the exact wrapper boundaries rather than folding address-distinct entry points. The audit rechecked live register inputs, low-width stores, signed/unsigned extension, and stack-slot consumption against the immutable instructions.

### `32954..32F4B` — state/table transformation family

Rechecked table indexing and pointer arithmetic at encoded width, including modulo-32-bit address arithmetic and byte/word truncation at stores. No booleanization of exact indices or sentinel values was introduced.

### `32F4C..33123` — graphics/palette helper family

Verified partial-register behavior and the distinction between full-register arithmetic and low-byte/low-word updates. Call boundaries were checked for cdecl order and for values intentionally kept live across calls.

### `33124..335DB` — compact dispatch/draw helpers

Preserved address-distinct wrappers and encoded control flow. Rechecked repeated global/table reloads rather than assuming cached C++ values, and verified signed branch conditions separately from unsigned tests.

### `335DC..339A1` — transformation/wrapper tail family

Rechecked narrow loads/stores, wrapped arithmetic, stack ordering, and alignment ownership. The final wrapper returns before the two-byte alignment at `339A0..339A1`; `339A2` is independently owned by the next routine.

## CODE.1 cross-reference defect checklist

The fully audited CODE.1 reconstruction was used only as a defect checklist, never as controlling proof. This span was specifically rechecked for:

- cdecl argument-slot order;
- byte/word partial-register aliases;
- signed versus unsigned branch drift;
- modulo-32-bit arithmetic;
- sentinel widening;
- global reloads across calls;
- address-distinct wrappers/thunks;
- encoded odd control flow and ownership boundaries.

No new CODE.2 mismatch was established.

## Result

Checkpoint 32 establishes semantic closure through object offset **`0x339A1`**. No reconstruction edit is required from this pass. The next independent semantic frontier is **`0x339A2`**.
