# CODE.2 Semantic Audit — Checkpoint 61

## 16-byte ledger discrepancy resolved

Checkpoint 59/60 observed that the source-explicit semantic numerator before `39FED` was 237,533 while object offset `39FED` is 237,549, leaving an unexplained 16-byte deficit.

The deficit is exactly object offsets `00000..0000F`, immediately before the first reconstructed routine at `00010`.

Immutable object-1 bytes:

```text
00000  CC             INT 3
00001  EB FD          JMP 00000
00003..0000F          13 x 90 (NOP)
```

Exact bytes:

`CC EB FD 90 90 90 90 90 90 90 90 90 90 90 90 90`

SHA-256:

`55610836eede1770be575892a20dd3d630b07a220e025e742761ae93c607e26d`

These 16 bytes are a pre-entry trap/alignment sled. They are instruction-bounded and semantically classified, but are not an application routine. The historical semantic pass began at `00010`, so the omission was a ledger-start artifact rather than a gap near `39FED`.

## Accounting consequence

The semantic frontier before the final tail is therefore physically reconciled:

- prior source-explicit semantic numerator: 237,533
- newly classified pre-entry bytes: 16
- reconciled frontier: 237,549 = `0x39FED`

The remaining physical tail is exactly `39FED..3A851`, 2,149 bytes. Once that tail is source-explicitly reconstructed/classified, semantic coverage can legitimately reach 239,698 / 239,698 = 100.0000%.

## Final-tail data island

Checkpoint 60 also established that `3A7F8..3A7FD` is six bytes of embedded ASCII data, `WVIDEO`, jumped over by `3A7F6 -> 3A7FE`. Those six bytes must be counted as non-code/data rather than translated instructions.

## Next

Continue explicit reconstruction of the seven ownership-complete final-tail folds beginning at `39FED`.