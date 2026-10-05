# NPCS.DAT

Status: source captured; format reconstruction not yet started.

- Size: 5,731 bytes
- SHA-256: `f2dd1175bf42e8291b95952282dfc0cfb84455aaa532987cdd7380094c8eb30b`
- Initial bytes: `53 00 00 00` (`0x53` little-endian), followed by named structured data beginning `ZOMBIE`; `DOOM GUARD` is visible in the early records.

Goal: prove record boundaries, field widths, lookup/index semantics, and any variable tails from CODE consumers; implement lossless decode/encode with unknown bytes retained exactly.