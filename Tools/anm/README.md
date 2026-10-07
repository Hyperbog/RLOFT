# Ravenloft Asset Extraction Suite

Master preservation/remastering extractor for *Ravenloft: Strahd's Possession*.

The tool uses the repository's certified reverse-engineering work as the
controlling description of Ravenloft-native formats and targets the complete
33-file media/resource corpus documented by
`Resources/FINAL_RESOURCE_AUDIT.md`.

> Directory note: the suite grew out of the original ANM extractor, so the
> implementation remains under `Tools/anm/`.

## GUI

Run:

```bash
python gui.py
```

The Windows executable and Python GUI use one baked-in Strahd's Possession
visual design. The interface theme does **not** depend on RES0, COLORS, or any
other game resource. The resource folder is needed only for extraction.

The window is freely resizable and the interface adapts to the available size.

### Game / media source

The extractor accepts the location users actually have rather than requiring a
pre-extracted CD directory. You can select:

- a normal Ravenloft install directory, including nested GOG/Steam-style layouts;
- a directory containing loose CD files such as `RES0..RES12`, `INIT`, and `COLORS`;
- a GOG install directory containing `game.gog` (the original CD image);
- an ISO-9660 `.gog`, `.iso`, `.bin`, or `.img` CD image directly;
- a mixed/modded install where loose files override matching files inside the CD image.

Folder scans are case-insensitive and recurse through common install-directory
layouts. For image/mixed layouts the tool builds a temporary flat source view
so the certified decoders still receive the original filenames. `NPCS.DAT`
and `OBJECTS.DAT` are carried into that view when present for semantic
multipart grouping.

**Scan / Validate** reports how many of the audited 33 media files are present
and whether they byte-match the preservation reference. A hash mismatch is
reported as provenance information rather than rejecting the source, so other
store releases, modified installs, and loose preservation copies remain usable.
Missing categories are reported and only affect extraction paths that require
those files.

## Extraction tabs

### Cinematics

The file list supports one cinematic, several cinematics, or all 13.

Outputs:

- individual frames with an independent destination;
- rendered movies with an independent destination;
- optional original ANM preservation;
- optional JSON event/timing timeline;
- optional sampled-SFX synchronization.

Frame formats: PNG, BMP, TIFF, JPEG, WebP.

Video formats: MP4/H.264, MKV/H.264, **MKV/FFV1 lossless**, MOV/H.264,
AVI/MPEG-4, WebM/VP9, GIF.

#### Native timing

The default movie mode is **Native engine timing**, reconstructed from the
CODE.2 ANM frame loop and wait routine rather than a guessed constant FPS.

The exporter preserves per-frame wait durations. For compatibility/testing, a
custom constant FPS can still be selected as a presentation derivative.

The timeline records frame start time, frame duration, wait ticks, and the
reconstructed timer frequency.

#### Cinematic audio

ANM sampled-audio commands are synchronized when movie export has
**Sync sampled SFX into video** enabled:

- queued chunk 6 resolves through the certified RES6 cinematic/SFX base;
- direct chunk 9 follows the original archive switch and resolves directly into RES10;
- ignored IDs from the original handler remain ignored.

These RES6/RES10 Creative Voice File effects are decoded by FFmpeg, delayed to their ANM
frame timestamps, mixed, and muxed with the rendered movie.

ANM music commands are retained in the timeline and the suite's XMI/MIDI
exports. They are not rendered to PCM through an arbitrary modern soundfont,
because that would not be a faithful reconstruction of the original MIDI/FM
hardware path.

For preservation, original ANM + timeline + PNG frames remain the preferred
master. FFV1/MKV is the preferred lossless rendered-video derivative.

### Graphics & Textures

The graphics list supports individual RES archives or the complete category.

Included visual families:

- type 1 — raster/UI/object/portrait/interaction graphics;
- type 2 — fonts and per-glyph widths;
- type 6 — 32x128 / 64x128 column-major first-person textures;
- type 7 — compiled scene-shape/display-list records.

Lossless image outputs: indexed PNG (default), TIFF/LZW, BMP, lossless WebP.
Canonical palette-index bytes and native EH records can be retained beside
viewable exports.

Intentional zero-count raster records in the certified legacy RES12 bank are
preserved as empty resources instead of being treated as decoder failures.

Type 7 is **not** falsely converted to an ordinary bitmap. Its decoded shape
bytes and structural JSON are exported until the SINTAB-coupled original
rasterizer is translated into a faithful portable renderer.

### Audio & Music

Sampled-audio and music RES archives can be selected individually or as complete
categories.

Sampled audio:

- all type-4 Creative Voice File sounds/voices;
- original VOC retention;
- lossless FLAC (default) or WAV/PCM conversion through FFmpeg.

Music:

- original XMIDI/XDIR;
- Standard MIDI Format 0 derivative;
- or both.

Standard MIDI conversion follows the original Miles Design MIDIFORM/XMIDI
behavior and uses SMPTE -30 fps with 4 ticks/frame: exactly 120 ticks/second.
Original XMI remains canonical because AIL FOR/NEXT loop control is not
losslessly representable as ordinary linear SMF playback.

Additional music support:

- all 233 `SAMPLE.AD` YM3812/OPL2 timbres;
- exact `GM1.BNK` / `GM2.BNK` ARIA banks plus identification metadata.

### Support Assets

Individual support categories can be enabled or disabled:

- all 15 `COLORS` VGA palettes, including canonical 6-bit RGB bytes;
- all 1,073 decoded `T` records plus reversible CP437 convenience views;
- `SAMPLE.AD` OPL2 timbres;
- ARIA banks;
- INIT resource namespace mappings;
- SINTAB renderer/scale packages.

### Dump All

Runs the complete media-extraction pipeline into one destination:

- all graphics/textures/fonts/shape structures;
- all sampled audio;
- all XMI/MIDI material;
- all cinematics and timelines;
- optional native-timed FFV1 movies with synchronized sampled SFX;
- all standalone support material.

Source, section, and master manifests are written with the dump.

## Background operation on Windows

All FFmpeg/helper processes are created with Windows' no-console flags. Long
audio/video exports therefore run silently in the background instead of
opening/closing terminal windows or repeatedly taking foreground focus.

## Requirements

- Python 3.10+
- Pillow
- Tk/Tkinter
- FFmpeg for rendered video and WAV/FLAC conversion

Install the Python dependency:

```bash
python -m pip install -r requirements.txt
```

## Command-line ANM compatibility

The original inspector remains available:

```bash
python rloft_anm.py inspect path/to/CINE00.ANM
python rloft_anm.py extract path/to/CINE00.ANM output_dir
```

## Format authority

See `INTEGRATIONS.md`.

Ravenloft formats are controlled by the certified repository reconstruction.
Modern output integrations follow official project/specification sources
(FFmpeg, Pillow, Xiph/RFC FLAC, and the MIDI Association).

Unknown or middleware-specific structures are preserved rather than normalized.

## Windows 11 executable

A reproducible x64 Windows build is produced by
`.github/workflows/build-windows-extractor.yml`.

The build:

- uses a Windows GitHub runner and Python 3.12 x64;
- freezes `app.py` with PyInstaller into `RavenloftAssetExtractor.exe`;
- bundles the Python/Tk/Pillow runtime;
- downloads the FFmpeg release-essentials Windows package linked from the
  official FFmpeg download page;
- verifies the published FFmpeg package SHA-256;
- bundles `ffmpeg.exe` inside the one-file application;
- runs the complete Python regression suite;
- runs `RavenloftAssetExtractor.exe --self-test` after freezing;
- publishes the EXE, ZIP, SHA-256 files, build information, and notices into
  `Tools/Windows/` and as a GitHub Actions artifact.

The executable does not bundle the game's extractable resource corpus.


## Current GUI behavior

### Baked Strahd banner

The Strahd interface is the only GUI theme. Its banner is composed from the
original Ravenloft frame, parchment, and corrected title artwork baked into the
application. The artwork is tiled/cropped at native pixel scale rather than
stretched and does not require RES0/COLORS at runtime.

### Native-rate display

Native cinematic timing is variable-rate. The GUI displays the selected
cinematic's effective average displayed-frame rate plus the underlying image
cadence and engine timer.

Certified timing values are based on the reconstructed IRQ0 path:

- engine timer: approximately 18.2065 Hz;
- CINE00..CINE11 image frames: approximately 3.0344 FPS;
- CINE12 image frames: approximately 9.1033 FPS;
- one-tick command frames: approximately 18.2065 FPS;
- zero-wait command frames add no display time.

The exported timeline/manifest retains exact per-frame waits.

### Complete RES3 scene graphics

Active RES3 type-7 compiled scene shapes are reconstructed to transparent
native-resolution images. All 4,234 active RES3 records render successfully and
are separated into:

- NPC / creature graphics;
- placed object graphics;
- loose / pickable item graphics;
- transient / moving-effect graphics.

PNG is the recommended modern-port visual derivative. Optional associated
outputs can retain canonical palette-index bytes, structural metadata/compiled
shape bytes, standalone transparency masks, and original EH records.

### Semantic multipart grouping

Graphics extraction can keep the raw archive layout, create grouped multipart
folders, or write both. Grouping follows the reconstructed renderer tables
instead of image similarity:

- NPC/creature definitions use NPCS.DAT local shape bases and frame stride,
  with the class-0 allocation window bounded by the next distinct NPC base;
- OBJECTS.DAT definitions use the class-2 local shape base and shape stride;
- the 175 INIT class-1 descriptors are split into exact variant/view subranges;
- the 12 INIT class-3 transient/moving-effect descriptors are split into exact
  variant/view subranges and use certified spell/item producer labels where
  known.

Each multipart folder contains native-resolution frames plus a manifest mapping
every file back to its original RES3 ID. Optional contact sheets, animated GIF
previews, and shared-resource alias maps are intended for modding, batch
upscaling, and remaster workflows. Generic names such as `variant_00` and
`view_00` are retained where the executable does not prove a narrower label.

NPC and object grouping requires `NPCS.DAT` and `OBJECTS.DAT` beside the
selected resource files. If either is absent, the raw extraction and INIT-based
groups still complete and the grouped manifest records the skipped family.

### Hover help

Interactive controls have delayed floating tooltips. They appear only after a
short hover and disappear on mouse leave or click.
