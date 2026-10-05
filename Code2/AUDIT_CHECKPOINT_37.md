# CODE.2 semantic audit checkpoint 37

## Scope

- Previous semantic boundary: `0x35F88`
- Audited ownership range: `0x35F88..0x369D7`
- Bytes audited this checkpoint: **2,640**
- Cumulative semantic audit: **223,688 / 239,698 = 93.3208%**
- Semantic defect classes: **6**; no new class established.
- Next independent boundary: `0x369D8`

This pass closes the already-integrated `35F88..369A6` timer/event family plus Batch 120 (`369A7..369D7`).

## Immutable verification

- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`
- object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`
- mandatory Checkpoint-36 overlap `[0x35F48,0x36008)`: `86d66e8c1e699e59f33ffe130941882a72a7b47709978884769cc56660389bc9`
- timer/event family `[0x35F88,0x369A7)`: `bd8579238447a144a160e6d1dcf6d48eadd0cc31efe21745cec92d5e3864c906`
- Batch 120 `[0x369A7,0x369D8)`: `43a18548577373759af9ea7b3ba745c3d8ba6219c755525875d8117c98c7950a`
- combined checkpoint `[0x35F88,0x369D8)`: `0755ead08ae82075950f478945e775df45ec26bddf2bc396ff9367d1597d1c3b`
- next-boundary overlap `[0x36998,0x36A58)`: `47a9a2e78d5769f962ee35723b682b9ffd62ce877ffb83526dff9ec22115dc18`

## Semantic review

### `35F88..369A6` timer/event family

Rechecked the CODE.2 bytes as the controlling evidence. The family retains its address-distinct lookup/forwarding entries, selector-preserving interrupt frame, DS/ES/FS/GS save/restore, reentrancy word gate, bounded timer-record walk, unsigned callback-position comparison, callback invocation through the stored table pointer, PIC EOI write, and IRET return path. The later forwarding stubs remain address-distinct even where they share the `35FB4` dispatcher.

The historical CODE.1 relocation correspondence was used only as a defect checklist/anchor aid, not as controlling proof. CODE.2 continuity records that this span was integrated from direct CODE.2 verification and differs from the CODE.1 analogue only in 101 three-byte address fields.

### `369A7..369D7` Batch 120

`369A7` preserves modular 32-bit `NEG`: negative inputs are negated without introducing a special-case for `INT32_MIN`. `369B2..369BF` remains the directly proven zero separator. `369C0` is not normalized into cdecl argument forwarding: it loads EDI, ESI, and ECX from its frame, invokes the indirect target, then restores the saved registers.

## Result

No new semantic mismatch was established. The next independent semantic boundary is `0x369D8`.
