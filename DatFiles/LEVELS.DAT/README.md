# LEVELS.DAT

Status: source captured; highest-priority format-reconstruction target.

- Size: 499,878 bytes
- SHA-256: `50d0b1d13f81733a1ac55296d00fcba9aef002202bc00341f1062c37fd017d91`
- Header begins with `20 00 00 00` (`0x20`), followed by a monotonic sequence of 32-bit little-endian values (`0x84`, `0x33EA`, `0x4D1E`, ...), strongly indicating an archive/directory structure. This remains a hypothesis until reconciled with the loader.
- CODE.2 reconstruction already contains an explicit `levels.dat` fallback path in the save/load family, providing executable evidence for cross-checking this format.

Goal: establish the directory/header semantics, enumerate contained level records, map record offsets and lengths, correlate them with game level IDs and `LEVxxx.DAT` state files, then build lossless extraction/repacking tools.