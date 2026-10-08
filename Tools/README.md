# Ravenloft tools

## Default Windows application: WPF Fluent Asset Extractor

The supported Windows 11 extraction suite is the native
**[Ravenloft WPF Fluent Asset Extractor](WpfExtractor/README.md)**.

- **Download:** [Latest Windows 11 application](https://github.com/Hyperbog/RLOFT/releases/latest)
- **Windows frontend:** [`Tools/WpfExtractor/`](WpfExtractor/)
- **Original-game extraction engine:** [`Tools/anm/`](anm/)
- **Version control:** WPF is the only supported Windows UI in the default branch.

Unzip the entire Windows distribution and launch `RavenloftExtractor.exe`.
The packaged Python backend and .NET runtime files must remain alongside it.
The application does not contain the original game files.

This repository's asset extraction core remains source-faithful and preserves
the reverse-engineered RES/ANM/COLORS formats, native indices, grouped original
filenames, and provenance metadata. The `anm/` directory is named for the
initial cinematic research; today it is the shared extraction engine.

The prior Python/Tkinter GUI and its Windows EXE are retired. Historical
source builds are retained in Git history, not as active tools. Other project
branches (demo, decompilation research, archival work) are unaffected.
