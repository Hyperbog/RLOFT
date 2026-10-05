# CODE.2 Semantic Audit — Checkpoint 56

## Scope

Forward semantic inspection from `39A2C` encountered split historical ownership:

- `39A2C..39A89` — 94 translated bytes, Batch 141
- `39A8A..39BBB` — 306-byte historical gap, deliberately unowned by Batch 141
- `39BBC..39C2F` — 116 translated bytes, Batch 141
- `39C30..39D4C` — 285-byte Batch-142 reconstruction exists as staged explicit source

This checkpoint credits only the two explicit Batch-141 folds, 210 bytes total. Cumulative semantic inspection: 236,270 / 239,698 = 98.5699%. Remaining not yet semantically inspected/credited: 3,428 bytes.

## Batch 141 result

No new definite mismatch is established in the two explicit folds.

`39A2C` walks the runtime arena list and only releases records satisfying the encoded exhausted-arena relationship. It preserves the successor before unlinking, calls the existing `399EB` list unlink helper, derives the allocation-owner header at node-8, and dispatches release according to whether the stored DOS selector is zero (DPMI backing) or nonzero (DOS selector backing).

`39BBC` preserves address-sorted insertion into the runtime arena list, predecessor/successor publication, head replacement, sentinel initialization, arena/free-block placement at node+2C, payload size `total-2C`, and the terminal `FFFFFFFF` block marker.

## Historical gap

`39A8A..39BBB` is 306 bytes and was explicitly left unowned by Batch 141. It must not be skipped merely because later batches start at `39C30`. This is now the next source-explicitness problem in the forward audit.

## Batch 142 source status

A staged explicit reconstruction exists for `39C30..39D4C` in commit `79b7d37cf0588b6e2e374649bda10d27621b8c38`, comprising 75-byte `39C30` and 210-byte `39C7B` helpers. Because the forward frontier is blocked by the unowned `39A8A..39BBB` interval, these 285 bytes are not credited in this checkpoint.

## Next

Resolve/reconstruct `39A8A..39BBB` from immutable object-1 before continuing through `39C30`. This preserves contiguous source-explicit semantic coverage and avoids silently bypassing the resize/grow helper.