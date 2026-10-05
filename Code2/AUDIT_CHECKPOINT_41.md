# CODE.2 semantic audit checkpoint 41

## Scope

Continues from semantic-inspection frontier `0x374E0`.

This checkpoint inspects reconstruction Batches 125 and 126 together:

- Batch 125 ownership: `374D2..375FD`; new semantic-inspection span here is `374E0..375FD`.
- Batch 126 ownership: `375FD..3780F`.
- Combined newly inspected end-exclusive span: `[0x374E0,0x37810)` = **816 bytes**.

Cumulative semantic inspection becomes **225,782 / 239,698 = 94.1944%**. This is inspection coverage, not a claim that every inspected byte is defect-free; open repairs from checkpoints 39-40 remain open.

## Immutable identity

Revalidated from uploaded `CODE.2`:

- full CODE.2 SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`
- object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`

Historical Batch-125 overlap `[0x375BE,0x37680)` reproduces exactly:
`b7d8fd0dd4201b0fd0ec96304b97b027adb2d9d5a8c7d641ce4be754bbef9ee2`.

Additional audit anchors:

- `[0x374E0,0x375FE)` — 286 bytes — SHA-256 `7b8a129fa2db4a19c0b6ad6224d0b0510eeb62b104ffd698ff74dff69aac941f`
- `[0x375FE,0x37810)` — 530 bytes — SHA-256 `d631e48e39a773dd5a62ffebb69fe2686bf5ac12f30ee51d814f8de84f209535`
- next-frontier overlap `[0x377D0,0x37850)` — 128 bytes — SHA-256 `16297f79261fe4a98b5f1de48e80ad8539ffef4e60defdb94d67359846456aff`

## Batch 125 semantic review

Compared immutable object-1 disassembly against the exact Batch-125 integration source in substantive commit `5fa532dcb06fd93d9395810c43cd02a95ea63fd0`.

Verified:

- `374E0` loads the source pointer from descriptor `+5`.
- Allocation size is exactly `count + 0x1B` where count is source dword `+16h`.
- The fixed header clone is exactly 28 bytes (`0x1C`) with the encoded word/dword/byte access widths retained by the reconstruction's bytewise copy.
- Variable-tail read uses destination `dst+0x1C` and **`count-1`**, distinct from allocation's `count+0x1B`.
- `3757C` and `375F0` remain address-distinct free wrappers.
- `37590` reads an exact two-byte length, allocates `length+2`, stores the 16-bit prefix, performs the `2F960` / `2F96C` setup sequence, then calls the five-argument payload fill path with `dst+2` and the zero-extension of the 16-bit count.
- Alignment ownership remains `374D2..374DF`, `37589..3758F`, and `375EF`; `375FD` belongs to the following Batch-126 separator accounting.

No new Batch-125 semantic mismatch was established.

## Batch 126 semantic review

Compared immutable disassembly against the exact Batch-126 source integrated by substantive commit `a528fbd28850e49f1fdeadb3a0433740e8dd2714`.

Verified major invariants:

- `375FE..37641` preserves the encoded extender-mode 2..6 split for installing the INT-24h bridge.
- `37642..376E8` is an interrupt-frame bridge, not a normal cdecl helper: ES/DS and all interrupt-visible general registers are saved, extender state is prepared, old SS:ESP is persisted, a private stack is selected, `BP:SI` is linearized as `(BP<<4)+(SI&FFFFh)`, the saved handler is invoked through a far call, and stack restoration is performed with `LSS` before the saved register image and `IRET` path.
- `376F0` is the address-taken one-byte RET stub; the surrounding zero islands remain separators.
- `37700` initializes only `0xFEE` dictionary bytes to `FEh`, leaving the remainder untouched at initialization.
- Decoder flag state uses 16-bit SI semantics; refill combines an input byte with `FF00h`.
- Literal path writes the byte both to output and the 4-KiB ring dictionary, then advances with 16-bit `&0FFFh` wrapping.
- Back-reference construction and inclusive copy count retain 16-bit ring arithmetic and the encoded `+2` low-nibble length rule.

The C++ boundary model intentionally abstracts the literal private-stack register image and far/IRET machinery behind callbacks; within that stated boundary model, no additional semantic mismatch was established in Batch 126.

## Open repairs retained

Checkpoint 41 does not close or hide previously established defects:

1. `36CEC..372F5` — 1,546-byte aggregate-baseline ownership has no explicit semantic reconstruction.
2. `372F6` — PIT gate test reconstruction uses `(gate&3)!=3`; immutable binary requires the `TEST AL,3 / JNE` behavior, i.e. initialization only when `(gate&3)==0`.
3. `3736D` / `37433` — the committed radix helpers normalize away encoded mutable stack-slot aliasing of divisor/quotient/remainder.

No fourth open repair was found in Batches 125-126.

## Next frontier

Next semantic-inspection boundary: **`0x37810`** (Batch 127).
