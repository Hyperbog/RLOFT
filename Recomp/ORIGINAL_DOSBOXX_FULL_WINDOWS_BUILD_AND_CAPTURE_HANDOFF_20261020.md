# Part 24 — real Windows DOSBox-X DAC hook build / original capture handoff

**Date:** 2026-10-20. **Owner:** `Recomp`. **Scope:** emulator instrumentation only.

## Authenticated successful full Windows build

GitHub Actions [Windows build #39390556673](https://github.com/Hyperbog/RLOFT/actions/runs/39390556673)
completed successfully. It cloned the exact upstream DOSBox-X source commit
`b6abbd5980a885f5f310a4088c59a8688d1b116c` at
`dosbox-x-v2026.10.01`, prevented Windows checkout newline conversion,
verified original `vga_dac.cpp` Git blob
`509d42fbead67a56594e49cec55549bc6fcc6d7c`,
patched the original `write_p3c8` / `write_p3c9` handlers,
and compiled `vs/dosbox-x.sln` as **Release SDL2|x64** using MSBuild/v143
on Windows Server 2022. The build job and artifact upload both passed.

A complete emulator executable was produced at
`bin/x64/Release SDL2/dosbox-x.exe` (21,545,984 bytes).
Its built SHA256 is
`9c43fab72976b5bbda68ff4319c343fa90494f2b381524dd7860c1eb7598a6c8`.
The build's ZIP artifact `rloft-dosboxx-dac-win64` is downloadable
through GitHub Actions; an independent local inspection of the extracted
21,545,984-byte Windows PE confirmed `MZ`, the expected SHA256,
`RLOFT_DAC_CAPTURE_JSONL` and the compiled
`dosboxx-v2026.10.01-dac-v1` marker.

**This proves the whole emulator builds and contains the opt-in recorder.**
It does not prove the executable opens on the owner's PC, that the original
Ravenloft runs under it, or that the retail DAC sequence has been captured.
No private original media was used for the Windows build or included in the
artifacts.

## Local capture handoff

Owner-facing helper scripts and usage instructions:

- [Capture setup and constraints](tools/dosboxx_dac_capture/README.md)
- [Start instrumented emulator](tools/dosboxx_dac_capture/start_original_capture.ps1)
- [Freeze paired evidence](tools/dosboxx_dac_capture/freeze_original_capture.ps1)
- [Strict DAC replay converter](tools/dosboxx_dac_capture/convert_capture.py)
- [Snapshot and DOS I/O validator](tools/verify_dosboxx_palette_port_trace.py)

The owner must run original Ravenloft in the *instrumented* emulator,
from a fresh launch, reach the matching Level-5 camera, pause emulation
without advancing interrupts, save a new emulator `.sav`, and while
still paused copy the raw `.jsonl` plus that exact `.sav` using the
freeze helper. The helper refuses overwrites, SHA-checks copied snapshot,
and rejects incomplete or discrepant I/O streams. It never constructs
port events from the SAV, COLORS, or the native renderer.

**Outstanding:** owner-side independent runtime capture, original CODE.2
instruction/caller ancestry of palette indices 246..253, and original vs.
native same-phase VRAM/actor pixel parity. The provisional 8-entry bank-3
palette correction remains provisional. Do not claim R04/gameplay parity.
