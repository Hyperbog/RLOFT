# CODE.2 resume checkpoint

Durable handoff anchor; live repository/binary evidence wins.

## Last committed substantive decompilation state
- Last substantive batch commit: `85f1b2b4b35ed476183e04305c0b98ec8f3c65e2` — `Integrate CODE.2 decompilation batch 132`.
- Verified translated instruction bytes: **229,073**.
- Proven non-code/data bytes: **4,672**.
- Reconciled object bytes: **233,745 / 239,698 = 97.5165% (97.52%)**.
- Unresolved object bytes: **5,953**.
- Last integrated ownership fold: `38106..384A9`.
- Next independently decoded instruction boundary: **`384AA`**.
- The earlier large helper `36CEC..372F5` remains unresolved.

## Immutable identity
- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.

## Mandatory overlap anchors for next resume
- Window `[0x3846A,0x3852A)` — 192 bytes; SHA-256 `417d6fbacd24084f2ccbcb551422463d4718f73fac8b417c7224f30c43a47e69`.
- Window `[0x3848A,0x3850A)` — 128 bytes; SHA-256 `f31e8bfb9dd2aa79ced424606f15a6cd29295b197193b5707ac2c6acc434a28e`.
- Batch-owned `38106..384A9` SHA-256: `ccf5a4605203508d0499c54a07100d92e9a8b93599fe1a90f908efc246978360`.
- Batch-owned bytes must not be recounted; overlap bytes are verification only.

## Batch 132 interpretation
- `38106..3821E`: descriptor read / text translation.
- `3821F..383EC`: descriptor write / append and LF-to-CRLF translation.
- `383ED..38451`: seek wrappers.
- `38452..38473`: unlink/delete.
- `38474..384A9`: current/default drive wrappers.
- `384AA` begins the next independently decoded routine.
