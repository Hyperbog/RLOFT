# Ravenloft Asset Extractor — native Windows 11 WPF Fluent preview

This is a **new frontend**, not a re-skin of the Tkinter application. The
existing Python extraction modules remain the authoritative resource decoders.

## Why WPF

The older Tkinter window required a simulated title bar, custom resizing
hit-tests and expensive image repainting. The user recording demonstrated
clipped controls at smaller sizes. This frontend uses:

- .NET 10 WPF with the Microsoft-provided Windows 11 **Fluent.Dark** theme.
- A **real Windows title bar**, maximize/restore behavior, resize cursors and
  standard system context menus. There is no borderless-window code.
- Original Ravenloft logo artwork with restrained burgundy, charcoal, gold and
  gothic heading accents, but native modern controls.
- A permanent navigation rail, scrollable per-page content and an always-visible
  extraction toolbar with Start, Cancel, status and progress.
- Windows 11 folder/image pickers, standard clipboard operations, mouse/keyboard
  accessibility and display scaling.

The frontend calls `Tools/anm/wpf_bridge.py` as a separate, frozen Python
executable. This preserves source-accurate images, palette indices, multipart
grouping, original output names, audio/cinematic outputs and game.gog/ISO
scanning without rewriting game decoders.

## Local Windows build — no GitHub Actions needed

Install **.NET 10 SDK** and **Python 3.12 x64**. Open PowerShell in the
repository and run:

```powershell
./Tools/WpfExtractor/build-local.ps1
```

The script runs the Python regressions, freezes the Python backend using
PyInstaller and publishes the self-contained WPF Windows x64 app. The ZIP is
written at:

```text
Tools/WpfExtractor/build-output/RavenloftExtractor-Windows11-x64.zip
```

Unpack the complete ZIP and run `RavenloftExtractor.exe`. Do not move the
frontend EXE alone; its bundled .NET runtime files and
`RavenloftExtractorBackend.exe` are required. Optional movie encoding also
needs `ffmpeg.exe`; place it on PATH before the build or run the script with
`-FFmpegPath C:\path\to\ffmpeg.exe`.

The alternate manual-only GitHub workflow is
`.github/workflows/build-wpf-fluent.yml` (Actions > Build native WPF Fluent
extractor > Run workflow). No automatic push-triggered builds are configured,
so other project threads will not consume GitHub minutes for this preview.

## Release criteria

- Windows build completes, Python tests and frozen backend self-test pass.
- App launches on Windows 11 without side-loaded dependencies.
- Real Windows frame shows proper pointer shapes; resizing/maximizing works.
- 780×520 minimum-size viewport still exposes navigation and the fixed
  Start/Cancel/progress footer. Long options scroll without clipping.
- CD/loose/GOG scanning works; extraction categories produce the same results
  and original filenames as the stable v0.6.3 engine.
- The frontend and backend remain in one distributable folder.

**Current status:** Source implementation, build scripts and static tests.
The GitHub-hosted Windows runner previously failed before executing any
steps because of account/usage restrictions. Do not describe this preview as
compiled, tested on Windows or ready to replace v0.6.3 until a Windows
build and real desktop checks complete.

Original Tkinter v0.6.3 remains the last verified Windows binary and the
`extractor-winui3` branch remains archived as a separate experiment.
