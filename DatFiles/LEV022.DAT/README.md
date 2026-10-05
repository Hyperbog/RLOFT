# LEV022.DAT

Status: source captured; analyze jointly with `LEVELS.DAT` and reconstructed level/save code.

- Size: 6,028 bytes
- SHA-256: `f88b033ff4cdafdd89620e0fac51ff2fb81bce103a6d31a26b91464532c9f51e`
- Header contains several plausible little-endian offsets/lengths (`0x1DDF`, `0x645`, `0x722`, `0x738`, `0x10AE`, `0x15C2`, ...), but no field meaning is assigned yet.

Goal: determine whether this is mutable level-state, extracted level data, or a compound of both; map every section against the loader/serializer and compare with the corresponding `LEVELS.DAT` entry before implementing lossless decode/encode.