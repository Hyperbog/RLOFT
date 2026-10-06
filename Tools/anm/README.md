# Ravenloft Asset Extraction Suite

Master preservation/remastering extractor for *Ravenloft: Strahd's Possession*.

The tool uses the repository's certified reverse-engineering work as the
controlling description of Ravenloft-native formats. It targets the complete
33-file resource/support corpus documented by `Resources/FINAL_RESOURCE_AUDIT.md`.

> Directory note: this suite grew out of the ANM extractor, so the implementation
> currently remains under `Tools/anm/`.

## GUI

Run:

```bash
python gui.py
```

The window is freely resizable. Path fields, output panels, tabs, progress
controls, explanatory text, and the optional theme banner resize with it.

### Source validation

Choose a folder containing the complete audited corpus:

- `RES0` .. `RES12`
- `CINE00.ANM` .. `CINE12.ANM`
- `GM1.BNK`, `GM2.BNK`
- `COLORS`, `INIT`, `SAMPLE.AD`, `SINTAB`, `T`

**Validate 33 files** checks size and SHA-256 against the certified source set.

## Extraction tabs

### Cinematics

- one ANM or all 13 ANMs;
- individual frames with an independent destination;
- rendered movies with an independent destination;
- optional original ANM preservation;
- optional JSON event/timing timeline.

Frame formats: PNG, BMP, TIFF, JPEG, WebP.

Video formats: MP4/H.264, MKV/H.264, **MKV/FFV1 lossless**, MOV/H.264,
AVI/MPEG-4, WebM/VP9, GIF.

For preservation, original ANM + timeline + PNG frames are preferred. FFV1/MKV
is the preferred lossless rendered-video derivative.

### Graphics & Textures

Covers every supplied visual RES family:

- type 1 — raster/UI/object/portrait/interaction graphics;
- type 2 — fonts and per-glyph widths;
- type 6 — 32x128 / 64x128 column-major first-person textures;
- type 7 — compiled scene-shape/display-list records.

Lossless image outputs: indexed PNG (default), TIFF/LZW, BMP, lossless WebP.
Canonical palette-index bytes and native EH records can be preserved beside
viewable files.

Type 7 is **not** falsely converted to a bitmap. Its decoded shape bytes and
structural JSON are exported until the SINTAB-coupled original rasterizer is
translated into a faithful portable renderer.

### Audio & Music

- all type-4 Creative Voice File sounds/voices;
- original VOC retention;
- lossless FLAC (default) or WAV/PCM conversion through FFmpeg;
- all type-3 XMIDI/XDIR sequences as original XMI;
- optional Standard MIDI Format 0 derivatives using exact 120 Hz SMPTE timing;
- all 233 `SAMPLE.AD` YM3812/OPL2 timbres;
- exact `GM1.BNK` / `GM2.BNK` ARIA banks plus identification metadata.

Standard MIDI export is implemented from the original Miles Design MIDIFORM
writer/runtime behavior and checked against the MIDI Association SMF model.
The derivative uses SMPTE -30 fps with 4 ticks/frame, giving exactly 120
ticks/second. Original XMI is always the preservation authority because AIL
FOR/NEXT loop control flow cannot be represented losslessly by a linear SMF.
The Ravenloft corpus contains 172 XMIDI resources / 350 sequences and uses
infinite AIL loops extensively.

### Support Assets

- all 15 `COLORS` VGA palettes, including canonical 6-bit RGB bytes;
- all 1,073 decoded `T` records plus reversible CP437 convenience views;
- `SAMPLE.AD` OPL2 timbres;
- ARIA banks;
- INIT resource namespace mappings;
- SINTAB renderer/scale packages.

### Dump All

Runs the complete extraction pipeline into one destination and writes source,
section, and master manifests.

## Optional Strahd's Possession theme

Select **Strahd's Possession** in the GUI.

The artwork is decoded directly from the selected original `RES0` and
`COLORS` files. Current UI art uses certified RES0 entries including the
original interface frame, menu strip, ornament, parchment treatment, and
Ravenloft title/logo. Artwork scales with the GUI while retaining nearest-
neighbor DOS pixel character.

## Requirements

- Python 3.10+
- Pillow
- Tk/Tkinter
- FFmpeg for rendered video and WAV/FLAC conversion

Install Python dependency:

```bash
python -m pip install -r requirements.txt
```

## Command-line ANM compatibility

The original ANM inspector remains available:

```bash
python rloft_anm.py inspect path/to/CINE00.ANM
python rloft_anm.py extract path/to/CINE00.ANM output_dir
```

## Format authority

See `INTEGRATIONS.md`.

Ravenloft formats are controlled by the certified repository reconstruction.
Modern output integrations follow official project/specification sources
(FFmpeg, Pillow, Xiph/RFC FLAC, and the MIDI Association).

Unknown or middleware-specific structures are preserved instead of normalized.
