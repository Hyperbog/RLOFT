# Ravenloft DAT reverse-engineering workspace

This tree is the canonical workspace for faithful analysis, extraction, format reconstruction, and eventual round-trip tooling for Ravenloft: Strahd's Possession `.DAT` files.

## Evidence standard

The original game files are immutable source evidence. Their bytes outrank inferred structures, field names, comments, and prior hypotheses. Do not edit an original to make a hypothesis fit.

For each format:

1. Record exact byte size and SHA-256 before analysis.
2. Identify consumers/producers in the reconstructed CODE.1/CODE.2 sources and reconcile hypotheses against the executable behavior.
3. Distinguish proven fields from tentative interpretations.
4. Preserve unknown bytes and padding exactly.
5. Build extraction/inspection tooling before writing a serializer.
6. Do not claim recompilation/round-trip support until `decode -> encode` reproduces the original byte-for-byte for known samples.
7. Keep address/offset provenance for every established field and document cross-file relationships.

## Uploaded source inventory (2026-10-05)

| File | Bytes | SHA-256 | Initial classification |
|---|---:|---|---|
| `OBJECTS.DAT` | 21,844 | `f23f0e7c1856904aa0156b0bf174cab794a41a327352b6ca00c3591ae5f2eed0` | Structured object/item definition database; begins with a little-endian count-like value `0x150` followed by fixed-looking named records (`POUCH`, `SACK`, ...). High-value candidate for full decode/encode tooling. |
| `INFO.DAT` | 17 | `4ffa6ac5d5b5afd103bd67d84d3a372b401b3b19407b0c68c44bb0dfd9e32c30` | Tiny configuration/install metadata rather than a large game database; still worth exact field identification because it may affect path/install behavior. |
| `NPCS.DAT` | 5,731 | `f2dd1175bf42e8291b95952282dfc0cfb84455aaa532987cdd7380094c8eb30b` | Structured NPC/creature definition database; begins with count-like `0x53` and named records (`ZOMBIE`, `DOOM GUARD`, ...). High-value candidate for full decode/encode tooling. |
| `LEV022.DAT` | 6,028 | `f88b033ff4cdafdd89620e0fac51ff2fb81bce103a6d31a26b91464532c9f51e` | Per-level/per-save level-state sample. Binary header contains multiple little-endian offsets/lengths. Important for separating immutable level geometry from mutable level state. |
| `SAVE.DAT` | 9,288 | `f898cb92fca6447954f5e0b2bf6f8ad2435e4a64ae3b9de74f8bf75e0ed10d5d` | Save-game/party state; starts with the human-readable party description `QUICK START PARTY; ELVEN F/M - DWARVEN F/C`. CODE.2 contains reconstructed save serialization logic, making this a strong format-recovery target. |
| `LEVELS.DAT` | 499,878 | `50d0b1d13f81733a1ac55296d00fcba9aef002202bc00341f1062c37fd017d91` | Master level archive. Begins with a 32-bit `0x20`, followed by a monotonic little-endian offset table. CODE.2 explicitly contains a `levels.dat` fallback path, so this should be decoded alongside the level loader/save-state routines. |

## Current status

The supplied-build `SAVE.DAT` reconstruction is archival-complete. Its final certification is recorded in `SAVE.DAT/FINAL_AUDIT_2026-10-06.md`.

The supplied-build `LEVELS.DAT` master archive and `LEVnnn.DAT` standalone mutable form are also preservation-grade / archival reconstructed. Their combined certification is recorded in `LEVELS.DAT/FINAL_AUDIT_2026-10-06.md`. The known 40-cell LEV022 automap-cache discrepancy is retained as a bounded provenance issue rather than normalized away.

In both cases, bounded opaque fields are preserved explicitly rather than assigned speculative semantics. These reconstruction-complete labels do not claim that independent byte-exact encoders are already implemented unless separate round-trip tests demonstrate that milestone.

## Initial priority

All six uploaded `.DAT` files are worth preserving and reverse engineering, but they are data formats, not executable machine code. The correct analogue to CODE.1/CODE.2 is therefore **format reconstruction + parser/serializer + byte-exact round-trip verification**, not disassembly into C++ instructions.

Recommended order: `LEVELS.DAT` + `LEV022.DAT` first (world/level loading), then `OBJECTS.DAT` and `NPCS.DAT` (gameplay databases), then `SAVE.DAT` (state serialization), with `INFO.DAT` handled opportunistically because it is only 17 bytes.

Each child directory is reserved for its own format notes, parser/serializer, fixtures/manifests, and progress ledger.