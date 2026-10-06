# Ravenloft resource-file reverse-engineering workspace

This tree is the canonical workspace for faithful analysis, extraction, format reconstruction, and round-trip tooling for the non-DAT resource files supplied from *Ravenloft: Strahd's Possession*.

**Current checkpoint:** the supplied Ravenloft resource set is functionally reconstructed for the initial faithful engine/native-port foundation. See [`STATUS.md`](STATUS.md) for the completion ledger, validation coverage, and explicitly bounded optional/deferred work.

## Layout

- `ANM/` — cinematic animation formats and validator.
- `RES/` — RES0..RES12 archive/container, typed-resource, renderer, and interaction-wordcode reconstruction.
- `BNK/` — Sierra Semiconductor ARIA General MIDI support-bank classification.
- `MISC/` — COLORS, INIT, SAMPLE.AD, SINTAB, and T support formats.

The immutable originals are source evidence. They are not rewritten to fit format hypotheses. Child directories preserve exact byte size and SHA-256 identity so every parser/specification claim remains tied to a specific source image.

## Evidence standard

1. Original Ravenloft bytes outrank external tools, comments, inferred names, and prior hypotheses.
2. Reconcile format claims against the certified CODE.1/CODE.2 reconstructions wherever a consumer/producer exists.
3. Keep confirmed structure separate from tentative or historical semantics.
4. Preserve unknown bytes, slack, anomalies, and padding exactly.
5. Build inspection/validation tooling before claiming serializers.
6. Do not claim round-trip support until decode -> encode reproduces the corresponding original byte-for-byte.
7. Community tools and documentation are secondary evidence and must be independently checked against Ravenloft bytes/executable behavior.
8. Identify third-party middleware assets as such; do not absorb vendor-driver formats into DreamForge-native architecture.
9. Other DreamForge titles may later provide comparative evidence, but never substitute for Ravenloft primary evidence.

## Uploaded source set

The 2026-10-05 source set contains 33 files:

```text
13 ANM cinematics
13 RES archives
 2 BNK files
 5 miscellaneous/support files
```

## Major completion points

- ANM packet grammar and all supplied cinematic compression/delta paths validated and byte-round-trippable.
- RES outer archive grammar reconstructed across all 12,562 entries.
- Supplied EH types 1/2/3/4/6/7 decoded and exhaustively validated.
- First-person type-6 textures and type-7 compiled scene-shape/display-list renderer formats reconstructed.
- RES3 source-bank mapping tied to INIT/NPCS/OBJECTS/transient effects.
- Active 38-program interaction/display wordcode VM reconstructed as a complete 42-opcode machine.
- T text archive, SINTAB renderer microcode package, COLORS palette bank, and INIT file structurally closed.
- SAMPLE.AD identified/reconstructed as Miles AIL OPL2 GTL.
- GM1.BNK / GM2.BNK identified as ARIA MIDI support banks; byte-level inner ARIA semantics deliberately deferred to the shipped third-party `A32ARXM.DLL` runtime-driver track.
- Executable-only type-1 method-1 RLE and type-5 loader contract documented despite having no Ravenloft sample.

See `STATUS.md` before starting new resource-format work; remaining questions there are intentionally classified as historical/alternate-build/vendor-driver follow-up rather than missing active Ravenloft resource behavior.
