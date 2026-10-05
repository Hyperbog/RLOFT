# CODE.2 resume checkpoint

Durable handoff anchor; live repository/binary evidence wins.

## Last committed substantive decompilation state
- Last substantive batch commit: `dcb158916249aff5d401a108ca852f053ca0856e` — `Integrate CODE.2 decompilation batch 136`.
- Verified translated instruction bytes: **231,371**.
- Proven non-code/data bytes: **4,672**.
- Reconciled object bytes: **236,043 / 239,698 = 98.4752% (98.48%)**.
- Unresolved object bytes: **3,655**.
- Batch 135 owned ranges: `38A0A..38A2F`, `38A61..38AC2`, `38AC3..38BA5`, `38BA6..38BC0`.
- `38A30..38A60` remains unowned because it shares formatter tail `38862`.
- Next independently decoded instruction boundary: **`38BC1`**.
- `385B0..38866` and the earlier large helper `36CEC..372F5` remain unresolved.

## Immutable identity
- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.

## Mandatory overlap anchors for next resume
- Window `[0x38B81,0x38C41)` — 192 bytes; SHA-256 `35863b072d431c3c529680fec9c5f44156db6232df57e1b8d0998885b1f43181`.
- Window `[0x38BA1,0x38C21)` — 128 bytes; SHA-256 `6009b726c3895066d334c0ea766f4321fc1788113b82d29f04e767819f76b27c`.
- Span `38A0A..38BC0` (includes deliberately unowned `38A30..38A60`) SHA-256: `1848e3d68a7418be3b0afcd556af42198689d1eb5bf5c6d56c99de3668d73532`.
- Owned bytes must not be recounted; overlap bytes are verification only.

## Batch 135 interpretation
- `38A0A..38A2F`: selector-qualified bounded byte-string length.
- `38A61..38AC2`: zero-filled exact-width hexadecimal materializer.
- `38AC3..38BA5`: signed 16.16 decimal/fraction materializer and rounding path.
- `38BA6..38BC0`: indirect three-argument forwarding wrapper through `[12700h]`.
- `38BC1` begins the next large conversion/dispatch helper.


## Batch 136 handoff
- Owned fold: `38BC1..3908B` (1,227 translated bytes).
- Next independent boundary: `3908C`.
- `3908C..39146` begins DOS-error/carry translation helpers and is not counted by Batch 136.
- `385B0..38866` and `36CEC..372F5` remain unresolved.


## Batch 137 handoff
- Substantive commit: `834feac5d18dd13d5a2c0817fc0f1c73ef74b43e`.
- Owned fold: `3908C..39588` (1,274 translated + 3 proven non-code bytes).
- Accounting: 232,645 translated + 4,675 non-code/data = 237,320 / 239,698 = 99.0079% (99.01%); unresolved 2,378.
- Next independent boundary: `39589`.
- `385B0..38866` and `36CEC..372F5` remain unresolved.
