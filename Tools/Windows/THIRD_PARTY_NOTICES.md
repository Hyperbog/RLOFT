# Third-party components

The Windows executable is frozen with PyInstaller and includes FFmpeg as a
separate bundled executable used for media conversion.

## PyInstaller

Project: https://pyinstaller.org/

The Windows build workflow currently uses PyInstaller 6.22.3.

## FFmpeg

Project: https://ffmpeg.org/

FFmpeg itself provides source code and links Windows executable builds from its
official download page. The build workflow uses the FFmpeg release essentials
Windows build linked there from gyan.dev, verifies the published SHA-256 for the
downloaded ZIP, and bundles only `ffmpeg.exe`.

FFmpeg source corresponding to the bundled release is available from the FFmpeg
project. The build workflow records the bundled FFmpeg version in its logs.

This application invokes FFmpeg as a separate process for VOC conversion and
rendered cinematic export.
