# CODE.2 semantic audit checkpoint 40

## Scope

Checkpoint 40 continues source-explicit semantic inspection at `3736D`, immediately after checkpoint 39. It inspects Batch 124 ownership `[3736D,374E0)` (371 bytes total: 320 translated bytes plus the 37-byte alphabet and 14-byte zero-fill tail pending Batch 125 classification).

This checkpoint does **not** grant semantic-audit closure to Batch 124 because direct immutable disassembly establishes stack-slot alias behavior that the committed C++ normalized away.

## Immutable identity

- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`
- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`
- Historical overlap `[3732D,37560)`: `fb03d884626319eb3660df9b273a8fa25074d61efbed16ea3b8b77d00cf2ff6a`
- Batch-owned `[3736D,374E0)`: `f3a491f2e6135bce5c6ed930471e07a9b85a43be572f539af57c1862afe97104`

These reproduce the Batch-124 reconstruction anchors exactly.

## Newly established semantic mismatch

### `3736D` first conversion helper

The immutable binary uses `[ESP+24h]` as a live aliased stack slot:

- `MOV [ESP+24h],EDI`
- `DIV DWORD PTR [EBX]`, with `EBX = ESP+24h`
- `MOV [EBX],EAX`
- the remainder in `EDX` indexes `CS:[EDX+37348h]`
- `MOV EDX,[ESP+24h]` therefore reloads the just-stored quotient

The committed reconstruction instead models an ordinary repeated `value / radix` conversion with a constant radix. That is not instruction-equivalent to the encoded stack-slot mutation.

### `37433` second conversion helper

The independently emitted second family has a different alias pattern:

- `MOV [ESP+24h],EBP`
- `DIV DWORD PTR [EBX]`
- `MOV [EBX],EDX`
- `MOV EDX,[ESP+24h]` reloads the just-stored remainder
- that reloaded low byte indexes `CS:[EDX+3740Eh]`
- the loop condition remains the quotient in `EAX`

The committed C++ again normalizes this into a conventional constant-radix conversion and therefore loses the encoded stack alias/state transition.

This is a concrete semantic defect, not a documentation ambiguity. It falls under the existing partial-width/stack-alias fidelity checklist but exposes a distinct reconstruction pattern: **mutable local divisor/remainder slot normalized into a stable high-level radix variable**.

## Audit accounting

Checkpoint 39 had semantically inspected 224,595 / 239,698 bytes. This pass inspects another 371 bytes, giving:

- semantic inspection coverage: **224,966 / 239,698 = 93.8540%**
- remaining uninspected: **14,732 bytes = 6.1460%**

Because Batch 124 contains a confirmed mismatch, these 371 bytes are inspection coverage, not defect-free semantic closure.

The previously identified `36CEC..372F5` explicit-source gap remains open. The Batch-123 `372F6` gate-condition mismatch also remains open pending source repair.

## Next action

Before treating Batch 124 as semantically closed, reconstruct `3736D` and `37433` from the exact register/stack-slot transitions rather than the conventional radix-conversion intent. After repair, resume source-explicit audit at `374E0` (Batch 125).
