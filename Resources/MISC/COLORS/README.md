# COLORS

- Bytes: 11,521
- SHA-256: `e366986bf701ba832aa769bf99f0db90a76af658300ce3d02e7039a970a9baf5`
- Source upload: `RES+OTHER.zip`
- Family: MISC / VGA palette bank

## Reconstruction status

`COLORS` is reconstructed as a 15-record VGA palette bank. Each record is exactly 256 RGB triplets / 768 bytes and begins at `1 + palette_index*768`. CODE.2 `325B0` proves that seek/read rule and then applies the in-memory palette brightness/intensity transform at `32150` before callers submit the palette through the graphics subsystem.

The serialized first byte is `0x0F`, and the remaining file is exactly 15 palette records. Current CODE.2 does not read that byte as a bounds check, so it is preserved as a structurally reconstructed palette-count field rather than an executable-consumed limit.

See `FORMAT.md` and `analyze_colors.py`.
