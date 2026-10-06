# ANM cinematic resources

This directory contains one analysis workspace per supplied `CINE00.ANM` through `CINE12.ANM` source file.

## Current status

The outer container, all ten executable chunk handlers, the type-2 full-frame RLE grammar, the type-3 sparse-delta grammar, and type-8 planar geometry have now been reconstructed directly from CODE.2 and validated across the complete supplied ANM set.

See:

- `FORMAT.md` — executable-backed format and chunk-dispatch specification;
- `INVENTORY.md` — complete supplied-file inventory and chunk counts;
- `analyze_anm.py` — non-destructive parser, structural validator, and byte-exact parse/serialize verifier.

The second 16-bit file-header word is documented as `default_wait`, not `version`: CODE.2 uses it as a timing value during playback. `CINE10.ANM` and `CINE11.ANM` store zero; the other supplied ANMs store one.

A major geometry correction is also confirmed. Type-2 packets decode to native 320-pixel scanlines, while type-8 packets contain four VGA planes for a 320x256 region beginning at framebuffer scanline 51. External 640-pixel-wide exports are presentation scaling, not encoded native width.

## Evidence policy

The immutable game files and CODE.2 bytes are controlling evidence. `DatFiles` documentation is used as a primary project cross-reference where it has already established subsystem roles such as the music-sequence family and sound-effect queue/playback paths. Community tools such as `LaZar00/LoftANM` remain secondary references and are corrected when their export-oriented interpretation differs from the original engine.

## Next work

- recover the remaining high-level meaning of chunk 4 from executable/file-state behavior;
- map chunk-5/6/7/9 parameters to exact game-visible effects and resource IDs;
- implement faithful image extraction without changing native geometry;
- cross-link subtitle IDs into `T` and audio IDs into the BNK/SAMPLE/resource investigation;
- add a mutation encoder only after its output rules are proved and byte-exact regeneration can be tested.
