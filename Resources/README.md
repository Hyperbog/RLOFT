# Ravenloft resource-file reverse-engineering workspace

This tree is the canonical workspace for faithful analysis, extraction, format reconstruction, and eventual round-trip tooling for the non-DAT resource files supplied from the Ravenloft: Strahd's Possession game/CD image.

## Layout

- `ANM/` — cinematic animation files; one subdirectory per original `.ANM` file.
- `RES/` — resource archives; one subdirectory per original `RES*` archive.
- `BNK/` — bank files; one subdirectory per original `.BNK` file.
- `MISC/` — remaining resource/support files that do not yet justify a dedicated family.

The immutable originals are source evidence. They are not rewritten to fit format hypotheses. Each child directory begins with exact byte size and SHA-256 identity so later parser/serializer work can be tied to a specific source image.

## Evidence standard

1. Original bytes outrank external tools, comments, inferred names, and prior hypotheses.
2. Reconcile format claims against the certified CODE.1/CODE.2 reconstructions wherever a consumer/producer exists.
3. Keep confirmed structure separate from tentative semantics.
4. Preserve unknown bytes and padding exactly.
5. Build inspection/extraction tooling before serializers.
6. Do not claim round-trip support until decode -> encode reproduces the corresponding original byte-for-byte.
7. Community tools and documentation are secondary evidence and must be independently checked against Ravenloft's bytes and executable behavior.

## Uploaded source set

The 2026-10-05 source set contains 33 files: 13 ANM cinematics, 13 RES archives, 2 BNK files, and 5 miscellaneous/support files.
