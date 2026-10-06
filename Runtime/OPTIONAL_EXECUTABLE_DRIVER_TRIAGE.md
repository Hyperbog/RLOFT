# Optional executable / audio-driver reverse-engineering triage

Status: **TRIAGED — not required for the initial faithful gameplay decompilation.**

This checkpoint decides whether `INSTALL.EXE`, `SINSTALL.EXE`, `LOADPATS.EXE`, and the shipped A32/GUS driver DLLs must be reverse engineered before the first faithful native reconstruction.

## 1. Decision

None of these binaries currently blocks reconstruction of Ravenloft's gameplay/runtime architecture. They should be preserved and documented, but full decompilation should be deferred until after the game-facing CODE/data/resource reconstruction unless a later unknown specifically points into their interfaces.

The one exception is **interface-level sound-system reconstruction**: `sound.cfg`, driver selection, and the CODE-side audio API/configuration contract are relevant to faithful behavior. That does not require decompiling every hardware driver.

## 2. INSTALL.EXE — archive / structural audit only

`INSTALL.EXE` is a 16-bit MZ installer. Its embedded inventory names Ravenloft's shipped payload, including `CODE.1`, `CODE.2`, `LEVELS.DAT`, `NPCS.DAT`, `OBJECTS.DAT`, `LEV022.DAT`, `SAVE.DAT`, `INFO.DAT`, `LOADPATS.EXE`, and the A32/GUS driver set.

This is useful provenance and installation-layout evidence, but it does not appear to contain gameplay logic missing from CODE.1/CODE.2. CODE itself emits `Cannot Find INFO.DAT, rerun INSTALL.EXE`, reinforcing INSTALL's setup role rather than runtime ownership.

**Initial port requirement:** no full decompilation.

Preserve: hashes, file-copy/layout behavior, generated/default files, command-line behavior, and any release/version checks.

## 3. SINSTALL.EXE — preserve configuration semantics, defer implementation RE

`SINSTALL.EXE` is a DOS/4G sound installer and identifies itself as `SSI Sound Flat Model Installer for Ravenloft 4.2.6b`. Strings expose MIDI/digital hardware selection, detection/error paths, `sound.cfg`, and mappings to the shipped A32/GUS drivers.

This binary is important historically because it defines how original hardware choices were converted into runtime configuration. It is **not** necessary to reproduce its 1994 hardware probing/UI in a native port.

**Initial port requirement:** reconstruct the resulting `sound.cfg` schema and CODE-side interpretation, not the entire installer.

Full RE becomes worthwhile only if configuration fields remain unexplained after CODE-side analysis.

## 4. LOADPATS.EXE / PATCHES.INI — Gravis compatibility subsystem

`LOADPATS.EXE` is LZEXE 0.91-compressed and references `PATCHES`/GUS setup. `RLOFT.BAT`, `SOUND.BAT`, `PATCHES.INI`, and the two GUS drivers place it in the Gravis UltraSound native-mode compatibility path.

This is hardware-specific middleware support, not core Ravenloft gameplay. A modern native audio backend does not need to reproduce the loader internally.

**Initial port requirement:** no decompilation. Preserve command/config semantics and document how the original GUS path was launched.

Only reverse engineer it if exact original-hardware emulation or historically exact GUS patch-loading behavior becomes a project goal.

## 5. A32*.DLL — Miles hardware drivers

The A32 DLLs identify themselves as Miles Design drivers and cover AdLib, Sound Blaster variants, Pro Audio Spectrum, Roland MT-32, speaker/Tandy, and related hardware. They are middleware hardware backends.

They should not be treated as DreamForge gameplay source. Reimplementing their low-level ISA/FM/MIDI/DSP behavior would add substantial work while contributing little to the first native Ravenloft reconstruction.

**Initial port requirement:** no driver decompilation.

Required instead: identify the CODE-side audio service calls, asset formats, channel/music semantics, and configuration mapping. Replace the hardware backend with a modern implementation while preserving game-visible timing/selection behavior where practical.

## 6. GUSDIG32.DLL / GUSMID32.DLL

These identify Miles plus Advanced Gravis/Forte Technologies authorship and explicitly describe UltraSound digital/MIDI playback. Their role is again device-specific middleware.

**Initial port requirement:** no full decompilation.

They are worth deeper study only for exact UltraSound emulation, patch-bank behavior, or if a CODE-side audio call cannot be understood without the driver ABI.

## 7. Do these fill current CODE unknowns?

At present, no evidence indicates that these installer/driver binaries contain missing gameplay routines, state-machine ownership, level logic, character logic, resource decoding, or unresolved CODE.1/CODE.2 control flow.

The useful cross-boundary evidence is configuration/interface information:

```text
SINSTALL.EXE
    -> chooses hardware / driver combination
    -> writes sound.cfg

CODE.1 / CODE.2
    -> read sound.cfg
    -> load/use selected audio middleware

A32/GUS DLL
    -> hardware-specific output implementation
```

Thus the correct reconstruction boundary is normally at the CODE-to-audio-middleware interface, not inside each driver.

## 8. Priority

```text
P1  CODE.1 / CODE.2 / DAT / RES / gameplay formats     REQUIRED
P1  sound.cfg schema + CODE-side audio contract        REQUIRED for faithful audio
P2  R.EXE / batch launch protocol                      DONE / architecturally resolved
P3  SINSTALL output/configuration semantics            useful preservation work
P4  INSTALL.EXE installation-layout semantics          archival
P5  LOADPATS + GUS exact patch loader                  optional hardware preservation
P5  A32/GUS driver internals                           optional middleware preservation
```

## 9. Recommendation

Save full installer/audio-driver reverse engineering for the end. Before a first faithful port, perform only a targeted audio-interface pass sufficient to answer:

1. exact `sound.cfg` format and values;
2. which CODE routines consume those values;
3. how music vs digital sound drivers are selected/loaded;
4. what game-visible calls and asset formats cross the middleware boundary;
5. whether any timing/state behavior depends on driver return values.

If those five items are resolved from CODE/configuration evidence, the original driver implementations serve preservation and hardware-emulation goals rather than being dependencies of the faithful gameplay decompilation.
