# Optional executable / audio-driver reverse-engineering triage

Status: **AUDITED / DEFERRED — not required for the initial faithful gameplay decompilation.**

This decision covers `INSTALL.EXE`, `SINSTALL.EXE`, `LOADPATS.EXE`, `PATCHES.INI`, the launch/configuration batch files, and the shipped A32/GUS driver DLLs.

## 1. Decision

No evidence found in this audit places missing Ravenloft gameplay, event, character, save, level, resource-decoding, or CODE.1/CODE.2 state-machine logic inside these optional binaries.

The initial faithful reconstruction needs the **game-facing audio contract**, not complete hardware-driver implementations.

## 2. INSTALL.EXE

Observed shipped binary:

- 16-bit DOS MZ executable;
- size `26,400` bytes;
- SHA-256 `c14b38d43fe98ae7ba29afe4f70dea4a0bb1ac826d2276ca81637717305c8829`.

Its embedded inventory names the principal Ravenloft payload, including CODE/data files, `SOUND.BAT`, `SINSTALL.EXE`, `LOADPATS.EXE`, `PATCHES.INI`, and the audio drivers.

**Initial-port requirement:** none beyond preserving installation/layout provenance. Full decompilation is archival work unless a release-layout question later depends on it.

## 3. SINSTALL.EXE

Observed shipped binary:

- DOS/4G-bound executable;
- size `381,660` bytes;
- SHA-256 `dcb28a9a38c822ded4392f6ff0ae7213b0e5e9a76dc69aa56bd0d1d48fd1ba4a`;
- identifies itself as `SSI Sound Flat Model Installer for Ravenloft 4.2.6b`.

Strings expose separate Music and Digital Sound Effects selection, address/IRQ/DMA prompts, `sound.cfg`, and mappings to the A32/GUS backends.

Both CODE modules are now independently proven to read exactly `0x3B` bytes from `sound.cfg`, so the correct reconstruction boundary is firmly on the CODE side.

**Initial-port requirement:** reproduce configuration semantics actually consumed by CODE. Full SINSTALL UI/hardware detection is not required.

## 4. LOADPATS.EXE / PATCHES.INI / Gravis path

Observed `LOADPATS.EXE`:

- MZ executable compressed with LZEXE 0.91;
- size `24,705` bytes;
- SHA-256 `17f40351cf53e445dbb62a041ff90b18dd755f6e1f3a216354e14aaeaa50f8ea`.

`PATCHES.INI` explicitly identifies itself as the default UltraSound LOADPATS configuration for AIL 32 and contains General MIDI/percussion patch mappings.

### Important audit correction

`RLOFT.BAT` and `SOUND.BAT` contain comments describing a `LOADPATS` failure/revision check, but **neither supplied batch file actually invokes `LOADPATS.EXE`** before testing `ERRORLEVEL`.

They do:

```text
if ULTRADIR empty -> normal path
goto GUS_LOAD
:GUS_LOAD
if errorlevel 1 -> OLD_REVISION
...
run r.exe / sinstall.exe
```

Therefore the batch files alone do not prove where or whether `LOADPATS.EXE` is executed in the shipped installation flow. Its association with UltraSound/AIL32 is clear from the executable/configuration set, but the exact activation path remains an archival question.

This is **not** a blocker for a native port.

## 5. A32/GUS DLLs

All 18 supplied driver files identify structurally as 32-bit LX/OS2-style DLL images used by the DOS/4G/Miles audio environment. Names and strings correspond to AdLib, Sound Blaster-family, Pro Audio Spectrum, MT-32, speaker/Tandy, and Gravis UltraSound hardware backends.

No driver decompilation is required for the first native implementation unless a specific CODE-facing ABI behavior remains unresolved after tracing the game-side callers.

## 6. What is required for faithful audio

Required:

1. the fixed 59-byte `SOUND.CFG` load contract;
2. Music versus Digital Sound Effects configuration groups;
3. CODE-side initialization/shutdown calls;
4. sound/music asset triggering;
5. any completion/status/timing behavior visible to gameplay.

Deferred:

- exact original card detection;
- ISA port I/O;
- IRQ/DMA setup;
- FM/MPU/MT-32 driver internals;
- exact Gravis patch-loading implementation;
- complete installer UIs.

## 7. Priority

```text
P1  CODE.1 / CODE.2 / DAT / RES / gameplay formats     REQUIRED
P1  CODE-side audio contract + SOUND.CFG semantics      REQUIRED for faithful audio
P2  R.EXE supervisor/module protocol                    DONE
P3  SINSTALL writer/configuration internals             optional targeted follow-up
P4  INSTALL.EXE installation mechanics                  archival
P5  LOADPATS exact launch/patch behavior                optional hardware preservation
P5  A32/GUS driver internals                            optional middleware preservation
```

Full reverse engineering of this optional group should remain end-of-project work unless a concrete unresolved dependency crosses the CODE-to-middleware boundary.
