# SINTAB

- Bytes: 333,757
- SHA-256: `19b6bc15f1be616bb8ff49dc9029995a1a0413fae06addc9fd41ddef761d0e87`
- Source upload: `RES+OTHER.zip`
- Family: MISC / first-person renderer support

## Current reconstruction status

`SINTAB` is now structurally reconstructed as a renderer support package rather than a simple sine lookup file. CODE.2 `3838..3A63` consumes its fixed tables and 127 variable scale blocks; the type-7 compiled scene-shape renderer later selects those blocks through runtime table `0x18FE0`.

The file contains trigonometric/projection lookup data plus generated 32-bit x86 rasterizer microcode. Each scale block contains two 129-word control tables followed by executable rasterizer bytes. A final 512-byte zero trailer is present in the supplied file but is not consumed by the reconstructed loader.

See `FORMAT.md` and `analyze_sintab.py`.
