# CODE.2 semantic audit checkpoint 51

## Scope
- Immutable object-1 range inspected: `[0x38BC1,0x3908C)` / `38BC1..3908B`.
- Bytes inspected this checkpoint: **1,227**.
- Historical reconstruction ownership: Batch 136, 1,227 translated bytes.
- Cumulative semantic inspection: **233,596 / 239,698 = 97.4543%**.
- Remaining uninspected: **6,102 bytes = 2.5457%**.
- Next inspection boundary: **`0x3908C`**, but this is **not a valid function boundary** in the immutable binary.

## Immutable identity
- CODE.2 SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`
- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`
- `[38BC1,3908C)` SHA-256: `5b5221cdd8a32a4c15f7befa6724a482254a42385d0f3a073354029d2393a074`
- overlap `[3904C,3910C)` SHA-256: `f6ec789800a13c40bf9d894df752810792fb6f2084fb4ac337bd4c0c83e29253`

## Critical Batch-136 reconstruction failure
The exact integrated Batch-136 source describes `38BC1..3906B` as a printf conversion dispatcher and `3906C..3908B` as an in-place uppercase-string helper. Direct immutable disassembly proves that description is not the code encoded at those addresses.

The range instead contains a family of runtime/state-management routines operating heavily on globals around `EE74h..F050h`, calling boundaries such as `468FDh`, `46907h`, `468DFh`, `468CBh`, `468C1h`, `468E9h`, `3F29Ch`, `40CD4h`, `3878Ch`, `38884h`, `388FCh`, and `389E4h`.

Representative immutable start at `38BC1`:
```asm
38BC1  PUSH 0
38BC3  PUSH DWORD PTR [EF24h]
38BC9  PUSH DWORD PTR [EE84h]
38BCF  CALL 468FDh
38BD4  MOV EAX,[F050h]
38BD9  MOV [270F0h],EAX
38BDE  MOVSX EAX,WORD PTR [EAX+32h]
...
```
This has no semantic relationship to the reconstructed formatter dispatcher.

## Boundary failure at 3908C
The historical Batch-136/137 split is itself invalid. Immutable code has one routine beginning at `39058` and continuing through `39094`:
```asm
39058  PUSH EBX
39059  XOR EDX,EDX
3905B  CMP DWORD PTR [EE78h],0
39062  JE 39091
39064  CMP DWORD PTR [EF2Ch],FFFFFFFFh
3906B  JE 39091
3906D  PUSH DWORD PTR [EF24h]
39073  PUSH DWORD PTR [EE84h]
39079  CALL 468DFh
3907E  ADD ESP,8
39081  MOV EDX,EAX
39083  TEST EAX,EAX
39085  JE 3908C
39087  CMP EAX,2
3908A  JNE 39091
3908C  MOV EDX,1
39091  MOV EAX,EDX
39093  POP EBX
39094  RET
```
Therefore `3908C` is an **internal basic block**, not the start of the Batch-137 DOS-error wrapper claimed by the reconstruction. Likewise `3906C` is not the start of an uppercase helper; it falls inside this same state-query routine (`3906D` is a PUSH instruction after the conditional at `39064`).

## Consequence
This is not a localized predicate/alias repair. Batch 136 is a **wholesale semantic misreconstruction** and must be replaced by a fresh explicit reconstruction from immutable CODE.2. The Batch-137 leading ownership must also be re-partitioned because its advertised start `3908C` begins in the middle of the immutable `39058..39094` routine.

No semantic-closure credit is granted to the historical Batch-136 C++ despite the range now counting as inspected. The translated-byte accounting remains historical byte ownership only.

## Batch-135 repair state
The three Checkpoint-50 Batch-135 defects remain deterministic repair items:
1. `38A61`: signed residual-width behavior; avoid unsigned `width-n` wrap when width is smaller than the digit count.
2. `38AC3`: third argument is a mutable format-state pointer, not precision by value; default precision `FFFFFFFFh -> 4` is written back through `[ESI+08h]`.
3. `38AC3`: rounding bit test uses the normalized magnitude after negative-input `NEG`, not the original signed bit pattern.

These should be integrated together with the fresh reconstruction rather than adapting later code to the invalid Batch-136 ABI.

## Repair status
- Former `36CEC..372F5` explicit-source gap: closed by direct reconstruction.
- Localized earlier repair-ready items: PIT gate, stack-slot aliases, permutation matched-position, stream shared tail, `37EFA` predicate.
- Batch-135 repair-ready items: 3.
- New structural repair: replace Batch 136 wholesale and re-establish the true Batch-137 entry partition.

## Resume
Continue by reconstructing the immutable runtime/state family beginning at `38BC1`, preserving true function boundaries. Do not use the historical Batch-136 formatter types or its `3906C/3908C` boundaries as controlling evidence. Once the true boundary beyond `39094` is established, re-audit/repartition Batch 137 from immutable code.