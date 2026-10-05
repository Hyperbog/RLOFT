# OBJECTS.DAT

Status: source captured; format reconstruction not yet started.

- Size: 21,844 bytes
- SHA-256: `f23f0e7c1856904aa0156b0bf174cab794a41a327352b6ca00c3591ae5f2eed0`
- Initial bytes: `50 01 00 00` (`0x150` little-endian), followed by named structured data beginning `POUCH` and later `SACK`.

Goal: prove the record/table layout from the bytes and CODE consumers, implement a lossless parser, then a serializer that reproduces the source byte-for-byte. Preserve unknown fields without interpretation until proven.