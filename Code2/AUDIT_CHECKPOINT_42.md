# CODE.2 semantic audit checkpoint 42

Checkpoint 42 inspects Batch 127 ownership `37810..379CF` (448 bytes: 433 translated + 15 separator) against immutable object-1 bytes.

## Immutable identity
- CODE.2 SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`
- object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`
- `[37810,379D0)` SHA-256: `7fcceb6317fa5961e00707cc63f90351f9c469bd0ddbdeb43d314e2d98bf9acf`
- next overlap `[37990,37A10)` SHA-256: `297e30b1a3abf4c17a61d84980c5aa1167bb3a141e1b2cad6d182eff3d11ab8d`

## Result
A new concrete semantic mismatch is established in `function_37810`.

The immutable binary's permutation loop loads `BX = table[n]`, searches forward for the table entry equal to `n` with `REPNZ SCASW`, backs EDI up to the matched entry, loads `AX = table[pos]`, writes the old `BX` into `table[pos]`, computes and saves `pos = (EDI-10610h)/2`, restores the original `table[n]` address, then writes `AX` into `table[n]`. Critically, the subsequent payload address calculation uses the saved **matched position `pos`** from `[EBP-20h]`.

The committed Batch-127 C++ instead searches for `n`, swaps table entries, but then derives the second payload location from `chosen`, which after the swap logic is the displaced table value (normally `n` at the matched slot), not from the matched position. This collapses an encoded position/index distinction and is not instruction-equivalent.

The DOS set/get vector wrappers `37958..379C0` retain the encoded extender-mode 2..8 split and no additional mismatch was established there. `379C1..379CF` remains the proven 15-byte separator.

## Audit accounting
- Newly inspected: 448 bytes.
- Cumulative semantic inspection: `226230 / 239698 = 94.3813%`.
- Remaining uninspected: 13,468 bytes = 5.6187%.
- Open repairs now include the `36CEC..372F5` explicit-reconstruction gap, the `372F6` PIT gate predicate, the `3736D/37433` mutable radix stack-slot aliasing, and the `37810` matched-position permutation error.
- Next frontier: `379D0`.
