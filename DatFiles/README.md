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

**Part 3 DAT File Reconstruction is 100% complete to the supplied evidence boundary.** The cross-format certification is recorded in `FINAL_AUDIT_2026-10-06.md`.

Final per-format certifications:

- `OBJECTS.DAT/FINAL_AUDIT_2026-10-06.md` — archival-complete; semantic byte-exact round-trip verified.
- `NPCS.DAT/FINAL_AUDIT_2026-10-06.md` — archival-complete; semantic byte-exact round-trip verified, including post-NUL name residue.
- `INFO.DAT/FINAL_AUDIT_2026-10-06.md` — archival-complete; active RES0..RES10 routing and historical slots 11..12 bounded; semantic byte-exact round-trip verified.
- `SAVE.DAT/FINAL_AUDIT_2026-10-06.md` — archival-complete.
- `LEVELS.DAT/FINAL_AUDIT_2026-10-06.md` — preservation-grade archival reconstruction complete for LEVELS and LEVnnn direct-save form.
- `LEV022.DAT` — certified mutable level-save fixture covered by the combined LEVELS audit; the known 40-cell cache discrepancy remains a bounded provenance issue.

Across all formats, bounded opaque, compatibility, and historical fields are preserved explicitly rather than assigned speculative semantics.

Reconstruction completion and future authoring-tool completion remain separate milestones. Independent semantic byte-exact serializers are already verified for OBJECTS, NPCS, and INFO; additional general-purpose SAVE/LEVELS serializer/editor work is optional implementation follow-up, not a reconstruction blocker.

## Post-reconstruction work

The original six-format reconstruction queue is closed.

Optional follow-up work is now limited to implementation or historical enrichment:

- generic authoring/serializer tooling where not already demonstrated;
- compatibility-fix modes kept explicitly separate from faithful original behavior;
- older/newer build comparisons;
- recovery of original source/editor terminology or provenance.

Each child directory remains the canonical home for format notes, parsers/serializers, fixtures/manifests, and archival evidence.