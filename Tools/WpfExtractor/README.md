# Ravenloft Asset Extraction Suite — Windows 11

**WPF Fluent is the default, supported Windows frontend.**

The previous Tkinter/Win98-style interface is retired. This application
uses a native Windows title bar, resize borders, standard cursors and text
editing; its visual identity retains the original Ravenloft crest, burgundy,
charcoal, antique gold and gothic headings. Settings scroll independently,
while Run, Cancel and progress remain accessible at all window sizes.

## Download and install

**[Get the latest release](https://github.com/Hyperbog/RLOFT/releases/latest)**

Download `RavenloftExtractor-Windows11-x64.zip`, extract the complete ZIP
and run `RavenloftExtractor.exe`. Do **not** extract and run just the EXE:
the WPF frontend requires its .NET files and the frozen
`RavenloftExtractorBackend.exe` in the same folder.

The distribution is self-contained on Windows 11 x64, and does not ship any
copyrighted game source archives. The release includes the helper used to
convert or encode video, when bundled in the build.

## Supported workflows

- Source scan/validation for installed GOG, Steam, loose media directories,
  mixed/modded files, `game.gog`, ISO, BIN and IMG CD data.
- Archive-faithful PNG/TIFF/BMP/WebP, fonts, textures, RES shapes and
  preservation indices.
- Semantic multipart extraction of creature/NPC, object, effects/spells and
  scene descriptor families without altering original output filenames.
- Contact sheets, animated previews, original-resource alias maps, palette
  routing and raw/grouped/both layouts.
- Creative Voice samples, FLAC/WAV/VOC, original XMIDI/XDIR and MIDI outputs.
- Original ANM cinematic frames, native timing, cinematic audio synchronization
  and optional rendered movies.
- Game tables: COLORS, INIT, SINTAB, OPL/ARIA and CP437 text records.

The Python decoders under `../anm/` remain authoritative. They are invoked
from the WPF app using a versioned JSON-lines backend protocol.

## Build locally

Windows 11; install the .NET 10 SDK and Python 3.12 x64.

```powershell
.\Tools\WpfExtractor\build-local.ps1
```

That script runs regression tests, reconstructs the authentic title logo,
builds `RavenloftExtractorBackend.exe` with PyInstaller, and publishes the
WPF frontend into a self-contained Windows ZIP.

Optional cinematic video encoding requires `ffmpeg.exe`. The CI workflow
downloads a verified copy before packaging; local builds may use `-FFmpegPath`
or a copy on PATH.

### Continuous integration

`.github/workflows/build-windows-extractor.yml` is the **single supported
Windows extractor workflow**. On relevant engine/UI changes and manual
dispatch it tests, builds, uploads an Actions artifact and publishes the
current GitHub Release asset at
`RavenloftExtractor-Windows11-x64.zip`. A successful automated build is not
a substitute for interactive Windows 11 mouse/resize testing, which should
also be performed before future UI redesigns.

The historical Tkinter frontend, old Windows binaries and obsolete automatic
PyInstaller-only release workflow have been removed from the active tree.
