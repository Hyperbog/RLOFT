# CODE.2 resume checkpoint

Durable handoff anchor; live repository/binary evidence wins.

## Last committed substantive decompilation state
- Last substantive batch commit: `bbfff3f9b9949e81fbb372f7199c605dec246940` — `Integrate CODE.2 decompilation batch 115`.
- Verified translated instruction bytes: **221,820**.
- Proven non-code/data bytes: **4,468**.
- Reconciled object bytes: **226,288 / 239,698 = 94.4055% (94.41%)**.
- Unresolved object bytes: **13,410**.
- Last integrated ownership fold: `35878..358D3`.
- Next independently decoded instruction boundary: **`358D4`**.

## Immutable identity
- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.

## Mandatory overlap anchor for next resume
- Window `[0x35870,0x35950)` — 224 bytes; SHA-256 `47bb1fa97187d5a3b11f020a3c0567e35ab9a9ff135ca8780fc9587e42213a0b`.
- Narrow `[0x358C0,0x35920)` — 96 bytes; SHA-256 `eada8f8c1b1484303fc1478ed3bb2f86b5d497154163e3310838d790b78efc25`.
- Narrow bytes: `000083c40c56e8b13d000083c4045f5ec38d400056575581eccc000000a17058030031ff898424c8000000f68424e00000000174098bac24dc000000eb226800020000ffb424e0000000e86825000083c40889c583f8ff750731c0e9df000000`.

Interpretation: `35878..358D0` is the translated selector-aware copy/file-I/O helper and `358D1..358D3` is proven alignment. `358D4` begins an independently called loader routine.
