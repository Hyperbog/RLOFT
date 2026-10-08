# Third-party components — current WPF Windows release

The default Ravenloft Windows extractor uses a self-contained Microsoft .NET
10 WPF frontend with the built-in Fluent theme and a Python extraction backend
frozen with PyInstaller.

## Microsoft .NET, WPF and Fluent

Project: https://dotnet.microsoft.com/
The frontend uses .NET 10 WPF and the standard Windows 11 Fluent resource
dictionary from Microsoft. The published ZIP includes required .NET runtime
files and remains subject to Microsoft's .NET licenses.

## PyInstaller

Project: https://pyinstaller.org/
The Windows build workflow uses PyInstaller 6.22.3 to freeze the independent
Python backend. The original resource decoders remain project source files.

## Pillow

Project: https://python-pillow.github.io/
Pillow handles decoded image export and authentic indexed-logo generation.

## FFmpeg

Project: https://ffmpeg.org/
The build downloads release-essentials for Windows linked by the FFmpeg
project, verifies the archive's published SHA-256, and bundles `ffmpeg.exe`
as a separate executable for audio/video conversion. Corresponding FFmpeg
source is available from the project.

No copyrighted Ravenloft source archives are included in the application.
