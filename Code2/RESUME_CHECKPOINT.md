# CODE.2 resume checkpoint

Durable handoff anchor; live repository/binary evidence wins.

## Last committed substantive decompilation state
- Last substantive batch commit: `79ceac8f1bfa63bf6a8ae162b11ba0e6e15a5445` — `Integrate CODE.2 decompilation batch 113`.
- Verified translated instruction bytes: **221,243**.
- Proven non-code/data bytes: **4,449**.
- Reconciled object bytes: **225,692 / 239,698 = 94.1568% (94.16%)**.
- Unresolved object bytes: **14,006**.
- Last integrated ownership fold: `3562F..3567F`.
- Next independently decoded instruction boundary: **`35680`**.

## Immutable identity
- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.

## Mandatory overlap anchor for next resume
- Window `[0x35620,0x35700)` — 224 bytes; SHA-256 `1f7dfc7ad7f8d551f353c14f75af9cc9005a511f6909b6360028374a6297a189`.
- Narrow `[0x35660,0x356C0)` — 96 bytes; SHA-256 `4a1e5e83fb5ec8680492ba456bfc2cca91c1a56a6ce9f6dfbfcc13a8473b7e4b`.
- Narrow bytes: `e8c83f000058b44ccd218bc0662e8e1d75560300c30000000000000000000000c8000000665366566657668b5d0c668b750e66837d0a007d08f75d086681f60080668b450866f7e3668bfa668b450a66f7e36603c76683d2006683fe007d0a66`.

Interpretation: `3562F..35674` is the translated shared termination/console-output/DS-restore family. `35675..35676` is the selector-qualified saved-DS runtime word and `35677..3567F` is proven zero alignment. `35680` begins an independently called `ENTER 0,0` arithmetic helper.
