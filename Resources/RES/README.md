# RES resource archives

This directory contains one analysis workspace per supplied `RES0` through `RES12` archive.

## Current reconstruction status

The outer count/absolute-offset container is fully reconstructed and byte-exact round-trippable for all 13 archives. CODE.2's inner `EH` dispatcher has now been traced directly, and every supplied instance of types 1, 2, 3, 4, 6, and 7 has been structurally validated.

See:

- `FORMAT.md` — executable-backed inner/outer format specification and evidence;
- `INVENTORY.md` — complete supplied-corpus counts, compression distribution, and anomalies;
- `analyze_res.py` — lossless outer parser plus exhaustive confirmed-family validator.

## Evidence rules

The original RES bytes and original CODE.2 machine code control all claims. Certified `Code2/Code2.cpp`, current DAT analysis, and CODE.1 are primary cross-reference evidence. Community tools such as `LoftRES` are useful leads only and do not override the shipped bytes or executable behavior.

Unknown or unsupported-in-corpus behavior remains explicit. In particular:

- executable type 5 has no supplied instance;
- type-1 compression method 1 exists in CODE.2 but no supplied resource uses it;
- 77 non-EH records are preserved but not yet semantically classified;
- exact type-6 raster layout and the remainder of the type-7 structure are still under reconstruction.

No binary asset is silently repaired or normalized. The two duplicated type-4 resources whose declared dword length exceeds their stored archive entry by exactly `0x10000` are documented as shipped anomalies.
