# CODE.2 semantic audit checkpoint 38

## Scope

- Previous semantic frontier: `0x369D8`
- Audited ownership-complete range: `0x369D8..0x36CEB`
- End-exclusive span: `[0x369D8,0x36CEC)`
- Bytes audited this checkpoint: **788**
- Cumulative semantic audit: **224,476 / 239,698 = 93.6495%**
- Remaining: **15,222 bytes = 6.3505%**
- Semantic defect classes: **6** (no new class established)
- Next ownership frontier: `0x36CEC`

This pass closes reconstruction batches **121 and 122**.

## Immutable source revalidation

Revalidated directly from `/mnt/data/CODE.2` and a fresh object-1 extraction:

- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`
- object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`
- mandatory Checkpoint-37 overlap `[0x36998,0x36A58)`: `47a9a2e78d5769f962ee35723b682b9ffd62ce877ffb83526dff9ec22115dc18`
- checkpoint span `[0x369D8,0x36CEC)`: `2796d6609ff165ef1db0a37e36a88bda77d7525e0ed9481f08145418619e06c5`
- next-boundary overlap `[0x36CAC,0x36D6C)`: `46ad7bec0ade95f6e224d7bd3cfac826c4380ea42a3f8238824bfd4cc451577e`

## Reconstruction evidence reviewed

The audit used direct immutable disassembly plus the exact staged C++ fragments preserved in the substantive Batch-121 and Batch-122 integration commits. Those fragments are source-level reconstruction evidence and include the byte-range/static-assert fidelity guards used by the integration workflow.

## Batch 121 — `369D8..36B9B`

The runtime callback renderer retains the binary's destination computation from page, row and phase globals; signed six-byte callback-record fields; unsigned callback-image word offsets; temporary replacement of the callback entry byte with `C3h`; exact saved-byte restoration; and the address-distinct callback target resolved through the `369C0` register adapter.

The three plane masks remain phase-derived from `C668h/C688h/C6A8h`. The first plane is unconditional, while the second and third are conditional on their mask bytes. Destination increments of +1 and +2 remain distinct.

The `36B14` wrapper preserves the full-dword AL/AH exchange rather than a 16-bit normalization. Its multi-pass path saves `105E3/105DF`, forces `105DF=1`, performs the entry-before-decrement loop, selects AL/AH ordering from phase parity, increments the live phase after each render, and restores both globals. `36B13` and `36B99..36B9B` remain four proven separator/alignment bytes.

## Batch 122 — `36B9C..36CEB`

The reverse and forward callback-table searches preserve **16-bit AX accumulation** while leaving the upper EAX half unchanged, followed by the encoded signed 32-bit threshold comparison. Reverse-search index saturation at `7Fh` is retained.

The patch helpers preserve the two distinct scratch regions: byte saves at `10454h` and word saves at `104D4h`. `36C10` installs `C3h` bytes; `36C44` installs `EB rel8` short jumps using the next table offset and 16-bit subtraction/truncation. `36C90` and `36CBC` restore the saved entries in reverse order. Alignment at `36C0F`, `36C8F`, `36CB9..36CBB`, and `36CE9..36CEB` remains non-code.

## Defect-checklist review

Rechecked cdecl slot order, partial-register aliasing, signed/unsigned branch behavior, modulo-width arithmetic, global reloads, self-modifying callback bytes, register-call ABI, address-distinct entries, reverse restoration order, and alignment ownership. No new mismatch was established and no reconstruction edit is required.

## Result

Semantic closure is established through `0x36CEB`. The audit is **224,476 / 239,698 = 93.6495%** complete. The next ownership boundary is `0x36CEC`; reconstruction history later reconciles the large `36CEC..372F5` region through aggregate-baseline accounting, so the next audit must identify the actual committed source representation before crediting that span rather than treating the old Batch-123 unresolved note as controlling.
