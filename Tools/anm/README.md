# Ravenloft ANM Tool

Native inspector, frame extractor, and video exporter for the reconstructed *Ravenloft: Strahd's Possession* ANM cinematic format.

Source specification: `Resources/ANM/FORMAT.md`.

## GUI

Run:

```bash
python gui.py
```

The GUI provides independent outputs for:

- **individual frames** — PNG, BMP, TIFF, JPEG, or WebP;
- **video** — MP4/H.264, MKV/H.264, lossless MKV/FFV1, MOV/H.264, AVI/MPEG-4, WebM/VP9, or GIF;
- **timeline metadata** — optional `timeline.json` preserving ANM chunk events.

Frame and video destinations are chosen independently. Video export requires FFmpeg to be installed and available on `PATH`.

### Optional Strahd's Possession theme

Choose **Strahd's Possession** from the Theme selector. The theme is built at runtime from the user's original Ravenloft resources rather than shipping copied artwork in this repository.

Minimum files in the selected Ravenloft folder:

```text
RES0
COLORS
```

The current skin uses original RES0 UI-raster assets and the original COLORS VGA palette bank. The GUI can auto-detect these files when the selected ANM is inside the game directory, or the user can choose the Ravenloft folder manually.

The shipped source files remain external dependencies: no extracted Ravenloft graphics are committed under `Tools/`.

## Command line

```bash
python rloft_anm.py inspect path/to/CINE00.ANM
python rloft_anm.py extract path/to/CINE00.ANM output_dir
```

`inspect` prints stream metadata and a per-frame chunk summary.

`extract` reconstructs the 320x400 indexed framebuffer, applies palette/reset/full-frame/delta/raw-planar image commands, and writes PNG frames plus `timeline.json`.

## Requirements

- Python 3.10+
- Pillow: `python -m pip install Pillow`
- Tk/Tkinter for the GUI (normally included with standard Windows/macOS Python; some Linux distributions package it separately)
- FFmpeg for video output

## Fidelity notes

- Type 2 is decoded as 320x400 row RLE exactly as reconstructed from CODE.2.
- Type 3 applies sparse planar updates directly to the current framebuffer.
- Type 8 writes the 320x256 four-plane region at framebuffer scanline 51.
- Type 0 updates the active 256-entry VGA palette.
- Type 1 clears the framebuffer and palette.
- Chunk type 4 is rejected during rendering because none of the supplied Ravenloft ANMs use it and its higher-level semantics remain deliberately conservative.
- Music/SFX/subtitle commands are retained in the timeline but are **not yet synthesized or muxed into exported video**.

## Theme resource provenance

The optional GUI skin follows the certified RES reconstruction instead of treating the resources as generic image files. `RES0` entries 0..14 are fonts and the UI raster namespace begins at entry 15; type-1 method-3 graphics use the reconstructed 4 KiB LZSS decoder. `COLORS` is read as the certified 15-record VGA palette bank.
