# CODE.2 resume checkpoint

Durable handoff anchor; live repository/binary evidence wins on disagreement.

## Last committed substantive decompilation state
- Last substantive batch commit: `6569106a2a6f6b98f39aa1e629e8f4467b58f6f2` — `Integrate CODE.2 decompilation batch 78`.
- Verified translated instruction bytes: **207,836**.
- Proven non-code/data bytes: **4,240**.
- Reconciled object bytes: **212,076 / 239,698 = 88.4763% (88.48%)**.
- Unresolved object bytes: **27,622**.
- Last integrated ownership fold: `320F8..3214F`.
- Next independently decoded instruction boundary: **`32150`**.

## Immutable identity
- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.

## Mandatory overlap anchor for next resume
- Window `[0x320D0,0x321B0)` — 224 bytes; SHA-256 `37ab4e55928af5efc10bd4661abff639b716850bbf1084c2d2718e8a23654fb9`.
- Narrow `[0x32100,0x32160)` — 96 bytes; SHA-256 `7e2e5f194b0ebc34875052d68c50e22e7a5f10bf0a04060b64198d1c4b68a4f1`.
- Narrow bytes: `5a0189188a5c240c881aff40105bc35383ec048d44241468f8200300894424048d442404508b542418528b5c241853e87c64000089da83c410c604020083c4045bc300000000000000000000000000005356575583ec0ca150f000000fb6406c`.

Interpretation: `32141` is the final RET; `32142..3214F` is fourteen zero bytes with no incoming direct reference; `32150` begins the next independent routine. No Batch 79 bytes are counted.
