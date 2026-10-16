# Persistent source-media cache (integration candidate)

This is a **C++17 integration into DemoV1**, not a separate executable or a finished mod loader. `GameSource::auto_locate(true)` first verifies an existing local original-format cache and otherwise imports from the original loose installation or `game.gog` through the existing `GameSource` adapter. The gameplay, renderer and audio code continue using the existing `GameSource` and `ResArchive` interfaces.

## First start and following starts

1. Find an original installation containing the game's required media. The importer copies only recognized **read-only original game media** into a staging directory; it never edits original binaries, DAT/RES archives or the original installation.
2. Extract `LEVELS.DAT` into 32 record files. Use the existing resource decoders to export raw `.eh` entries, indexed image buffers, type-7 sprite pixels and masks, XMIDI and VOC streams, and decoded supported type-6 blocks. Unsupported entries and decoding failures are recorded explicitly; they are not silently claimed as fully decoded.
3. Only after a complete import, swap the staging directory into the active cache. Preserve the previous working cache on a failed replacement. Place the ready marker last.
4. On following launches, validate the cached source files against recorded file sizes and 64-bit FNV-1a corruption-detection digests, then load the cached copies instead of rescanning or reextracting the original installation.

Default Windows location: `%LOCALAPPDATA%\\RavenloftEnhanced\\AssetCache\\v3`. Other platforms use `XDG_CACHE_HOME/rloft-enhanced/v3` or `~/.cache/rloft-enhanced/v3`.

## Directory layout

- `v3/media/`: unmodified **original-format** game resource containers, used by the current game. Do not edit these to mod the game; integrity validation rejects modifications.
- `v3/decoded/RESn/`: exported original resource entries, decoded indexed images, native sprite masks, XMIDI and VOC. `.r8i` and `.r8m` are deliberately simple *internal* binary formats, not PNG and not a stable mod SDK.
- `v3/levels/`: 32 extracted raw level records. These are inspection assets, not a separate live level-authoring schema.
- `v3/complete.tsv`: ready marker, source file sizes and cache digests.
- `v3/decode_report.tsv`: per-bank decoder outcomes.
- `v3/origin.bin`: UTF-8 path identifying the original media installation from which this cache was imported, used to reject mismatched explicit source selections.
- `runtime/`: mutable native runtime files, intentionally separate from `v3/media/` and imported level archives.

## Controls, guarantees and limitations

- `RLOFT_CACHE_DIR` selects a different cache root (for development/tests).
- `RLOFT_REIMPORT_ASSETS=1` forces discovery and a new extraction. Use it after **changing original media**, as the cache deliberately does not read the original installation on every boot.
- Source-cache hashes guard against accidental cached-media corruption, **not adversarial tampering**. A future SHA-256 asset manifest would be needed as a cryptographic integrity guarantee.
- The cache currently validates source files on startup, which incurs local disk reads; it avoids reads/extraction from original media. Decoded files are **not yet used directly** by the renderer/audio runtime. No public mod package schema, resolver, dependency ordering or save migration is implemented.
- Existing `Ravenloft.exe` workflows need a pinned Windows compile, test and owner gameplay acceptance before the feature can be called field-verified.
- Existing `game_source_tests` and `persistent_media_cache_tests` run with no licensed original media. The latter verifies import, reload, exclusion of `SAVE.DAT`/`LEV*.DAT`, same-size corruption detection and reimport repair.
