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


## Batch 138 handoff
- Last substantive batch commit: `4c40291c472f2417d81aa692a5d262e7b7074abf` — `Integrate CODE.2 decompilation batch 138`.
- Verified translated instruction bytes: **232,906**.
- Proven non-code/data bytes: **4,675**.
- Reconciled: **237,581 / 239,698 = 99.1168% (99.12%)**.
- Unresolved: **2,117**.
- Owned fold: `39589..3968D` (261 translated bytes).
- Batch-owned SHA-256: `753921c7dc798149f0f647667ee771115032702827972404a1749bf91d7735b7`.
- Mandatory overlap `[0x3964E,0x3970E)` SHA-256: `d386cc964a16feca18cba8d61fb27cbb2fc22384883ba79f2440327b3bf57169`.
- Mandatory overlap `[0x3966E,0x396EE)` SHA-256: `fa45f1826f22bebbbe2dd92b2837aaf1b2eb9760f4806e0433f91291bc9f25d0`.
- Next independent boundary: `3968E`.
- `385B0..38866` and `36CEC..372F5` remain unresolved.


## Batch 139 handoff
- Last substantive batch commit: `b50507bab2cd9e9a61f51d2695d958f1a85ce7d7` — `Integrate CODE.2 decompilation batch 139`.
- Verified translated instruction bytes: **233,331**.
- Proven non-code/data bytes: **4,675**.
- Reconciled: **238,006 / 239,698 = 99.2941% (99.29%)**.
- Unresolved: **1,692**.
- Owned fold: `3968E..39836` (425 translated bytes).
- Batch-owned SHA-256: `9361cfa6660db35e1f38bafff110a3fe1c64336808c941e5b7d7e2ed2cf458fd`.
- Mandatory overlap `[0x397F7,0x398B7)` SHA-256: `38dec662da86650cbf9cfc26e2d0f65fb608879a5aedf711fc4c76ecf1557128`.
- Mandatory overlap `[0x39817,0x39897)` SHA-256: `e99dd5778c0474da4d54ca658eaa8e664f9473eadec59b57408973c629e529a0`.
- Next independent boundary: `39838`; byte `39837` remains unowned.
- `385B0..38866` and `36CEC..372F5` remain unresolved.


## Batch 140 handoff
- Owned fold: `39838..39A2B` (**500 translated bytes**) plus `39837` (**1 proven non-code byte**).
- Accounting: **233,831 translated + 4,676 non-code/data = 238,507 / 239,698 = 99.5031% (99.50%)**; unresolved **1,191**.
- Batch-owned code SHA-256: `d47a1b4f97620485574578a3883bd31dd1dd3bbbd10c2010f24f0efdeaa6594c`.
- Mandatory overlap `[0x399EC,0x39AAC)` SHA-256: `c8e7676423dd051bdc1f2f04843cca4241946102176c137f767b00a5ace53198`.
- Mandatory overlap `[0x39A0C,0x39A8C)` SHA-256: `76dbf5536b67442f881886f1f68c91cc9be1e2ccb6c54464c471ec40bcab6159`.
- Next independent boundary: `39A2C`.
- `385B0..38866` and `36CEC..372F5` remain unresolved.


## Batch 141 handoff
- Owned folds: `39A2C..39A89` (**94 translated bytes**) and `39BBC..39C2F` (**116 translated bytes**), total **210 translated bytes**.
- `39A8A..39BBB` remains deliberately unowned.
- Accounting: **234,041 translated + 4,676 non-code/data = 238,717 / 239,698 = 99.5907% (99.59%)**; unresolved **981**.
- Owned hashes: `39A2C..39A89` = `451a5752be1bf4c604227216aa9ebfe58f9fb7d1e165a79b646669d652908cee`; `39BBC..39C2F` = `4912611d6dd6dc6e7dcad4f22ecb29896a60cfc5c7f988e95a6b24dc34339b4e`.
- Mandatory overlap `[0x39BF0,0x39CB0)` SHA-256: `e4582468c6f6ce35e793943b3c01172300d5d5749873d32a894ba0031c89c5a8`.
- Mandatory overlap `[0x39C10,0x39C90)` SHA-256: `39e834ab542dfc7cda72bc2512b3d551cc53085ce61da03780c0a57ac7bf62c5`.
- Next independent boundary: `39C30`.
- `385B0..38866`, `36CEC..372F5`, and `39A8A..39BBB` remain unresolved.


## Batch 143 final handoff
- CODE.2 object-1 accounting is closed: **235,022 translated + 4,676 proven non-code/data = 239,698 / 239,698 = 100.0000%**.
- Unresolved bytes: **0**.
- Final explicit folds: pending Batch 142 `39C30..39D4C` (285 bytes), formatter `385B0..38866` (695 bytes), and shared-tail word-string helper `38A30..38A60` (49 bytes).
- Historical baseline correction: `159155 -> 159107` removes 48 implicitly credited bytes now owned explicitly by `38A30..38A60`; the 49-byte explicit fold contributes net +1 byte.
- Immutable CODE.2 SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
- Immutable object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.
- This checkpoint supersedes stale earlier notes describing `36CEC..372F5`, `38A30..38A60`, or `39A8A..39BBB` as accounting-unresolved; those notes predated the aggregate-baseline reconciliation.


## Semantic audit checkpoint 5 handoff
- Object accounting remains closed at **235,022 translated + 4,676 proven non-code/data = 239,698 / 239,698**.
- Semantic audit: **13,876 / 239,698 = 5.7890%**, closed through **`03633`**.
- Semantic defect classes found so far: **2**; checkpoint 5 found no new correction.
- Checkpoint-5 span `02D70..03633` SHA-256: `34d1aab387aaafb288749013481c975620f3d0ed2819a3cdb03d04b72a3ca373`.
- Mandatory prior-frontier overlap `[0x02D30,0x02DF0)` SHA-256: `11b1b378f281c2685f0fad8f9046c8c7af5a83cfe35d2f3f8641c9fa2280171`.
- Next semantic-audit frontier: **`03634`**.


## Semantic audit checkpoint 6 handoff
- Object accounting remains **239,698 / 239,698**.
- Semantic audit: **20,876 / 239,698 = 8.7093%**, closed through **`0518B`**.
- Semantic defect classes: **4**; checkpoint 6 corrected two new classes in `043F4` and `044D0`.
- Span SHA-256: `8fb9e029a196bcfae47729e08ad90d08bfc26cd8671f5b7f006d74cf65abe9ae`.
- Overlap `[0x035F4,0x036F4)` SHA-256: `c0a5b070819dc0380fc634dc1f8445dd684f6e699958fad502ff9c1fa7962645`.
- Next semantic-audit frontier: **`0518C`**.
