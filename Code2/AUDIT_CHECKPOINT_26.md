# CODE.2 semantic audit checkpoint 26

Status: **PASS**

## Range and accounting

- Audited object-relative range: **`25DFC..29413`**.
- Bytes closed by this checkpoint: **13,848**.
- Cumulative semantic audit: **168,964 / 239,698 = 70.4904%**.
- Semantic defect classes remain **6**; no new semantic defect was established in this checkpoint.
- Immutable CODE.2 SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
- Immutable object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.
- Audited span SHA-256 (`25DFC..29413`): `b528a11833cf694b8b089f1e91e07ec6ec5aebd55c52a3c43595cdfe79ca19e0`.
- Mandatory overlap around the next frontier `[293D4,294D4)`: `a84c239ed70c8dced2ec2902a5d4d786316105b9bb33d04caa6bfe1c3f95d595`.

## Ownership/family coverage

This deliberately larger pass closes original reconstruction batches **53 through 56** as one ownership-complete semantic checkpoint.

- **Batch 53 (`25DFC..273E6`)**: revalidated the exact 135-entry CS dword table at `25DFC..26017` and its `26018` item dispatcher. Preserved unsigned `A7h`-based dispatch gating, cooldown early return, load-before-`-1` kind override, AL-only peer-slot XOR behavior, callback-sensitive global reloads, partial-word status tests, the item-`73h` pre-pushed `B02C` frame, and shared `273DA` cleanup.
- **`273E7..273EF`**: confirmed as the previously proven nine-byte separator immediately after the `273E6` RET; no semantic ownership was invented for it.
- **Batch 54 (`273F0..27F23`)**: rechecked VGA plane/register ordering, byte/word register effects, interrupt stack/selector swapping at `27894`, screenshot filename retry behavior, streamed planar/RLE cursor ownership, dword zero-detect loads at `27C70`, and the `27D20/27ECC` multimedia initialization/teardown lifecycle.
- **Batch 55 (`27F24..2873B`)**: rechecked REP-copy remainder semantics, signed queue-priority arbitration, `28210` stack-copy/error fall-through, the four-argument stack walk into `366BB/366C5`, stale/live ESI distinctions, repeated-close behavior at `28564`, and the explicit `EE8C` reload on the `28670` match path.
- **Batch 56 (`2873C..29413`)**: rechecked the zero-map early exit, current-slot guard interaction, `29058` non-boolean live-state return, BX-only/high-half behavior in `290D0`, partial-width state writes in `290F4`, signed clipping distinctions, and the final graphics wrappers' shared earlier RET/tail-transfer behavior.

## CODE.1 cross-reference checklist

The fully audited CODE.1 defect catalogue was applied across the complete span: cdecl stack-slot order, live-register call inputs, partial-register aliasing, signed/unsigned branch drift, modulo-32-bit arithmetic, encoded loop-entry behavior, selector/interrupt-frame preservation, global reloads after opaque calls, address-distinct wrappers/shared tails, and self-modifying/low-level machine semantics. No additional CODE.1 defect class reproduced in this CODE.2 span.

## Boundary result

Fresh disassembly confirms `29414` begins the next independent graphics routine. The next semantic-audit frontier is therefore **`29414`**.
