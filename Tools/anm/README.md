# Ravenloft preservation/extraction engine

This directory contains the **Python resource decoders and Windows backend**
used by the default [WPF Fluent Asset Extractor](../WpfExtractor/README.md).

**There is no supported Tkinter GUI in this directory.**
The legacy frontend was retired when WPF became the default; it remains
recoverable from Git history for archaeological reference only.

## Architecture

- `wpf_bridge.py`: JSON-lines process interface between the WPF desktop UI
  and the Python engine. Handles `validate`, `graphics`, `audio`,
  `cinematics`, `support`, and `all`.
- `game_source.py` / `media_inventory.py`: installed/loose GOG/Steam
  directories, `game.gog`, ISO/BIN/IMG media, fallback and checksum auditing.
- `res_assets.py` / `asset_dumper.py`: source-accurate RES extraction,
  palette-indexed graphics, fonts, textures, compiled type-7 shapes,
  sound/music.
- `grouped_graphics.py` / `palette_router.py`: multipart creature, object,
  spell/effect, scene-descriptor grouping and documented original palettes.
- `exporter.py` / `rloft_anm.py`: original ANM frame/timing reconstruction,
  synchronized effects and video conversion.
- `support_dumper.py` / `xmidi.py`: text, COLORS, INIT, SINTAB, timbres,
  XMIDI and MIDI derivatives.
- `title_skin.py`: archival indexed original-logo evidence used by
  `render_wpf_logo.py`; it is **not** a live window-skin renderer.
- `render_wpf_logo.py`: regenerates the original crest as a verified PNG
  before WPF compilation. Image samples and original evidence remain in
  `assets/`, but the obsolete Tkinter UI does not ship.

## Reproducibility

Source corpus and format authority:
`Resources/FINAL_RESOURCE_AUDIT.md` and `INTEGRATIONS.md`.
Unknown structures are preserved, never silently filled with guesses.

Python 3.12 x64 + Pillow are required to build the Windows backend. The
Windows release bundles a PyInstaller-frozen backend and verified FFmpeg
alongside the self-contained .NET 10 WPF frontend.

Run the Python regression suite:

```bash
cd Tools/anm
python -m unittest discover -p "test_*.py" -v
```

Create the complete Windows 11 build using
`Tools/WpfExtractor/build-local.ps1`, or use the repository's
`.github/workflows/build-windows-extractor.yml`.

## Native data preservation

Original RES IDs and source resource names are retained in output filenames.
Type-7 semantic group names are stored in folder paths and JSON manifests;
they are not appended to file stems. Preserve original EH, ANM, XMI/VOC and
palette-index bytes whenever a reversible archive is needed.

See `INTEGRATIONS.md` for the decoder specification and modern export
formats.
