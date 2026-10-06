# Ravenloft Runtime / Executable Reconstruction

Status: **ACTIVE — Phase 1 inventory and classification**

This tree preserves the executable-side reconstruction of *Ravenloft: Strahd's Possession* separately from `Code1`, `Code2`, `DatFiles`, and `Resources`.

## Evidence policy

The project's existing preservation rule remains controlling: shipped Ravenloft bytes are primary evidence. Toolchain/platform documentation explains mechanisms only after those mechanisms are observed in the binaries. Third-party libraries are identified and documented before deciding whether full decompilation is necessary.

## Current source set

### Executables
- `R.EXE` — main game launcher/runtime; **highest-priority reverse-engineering target**.
- `INSTALL.EXE` — Ravenloft installer; contains the shipped-file manifest and DreamForge identification strings.
- `SINSTALL.EXE` — SSI sound installer/configurator; DOS/4GW/Watcom executable.
- `LOADPATS.EXE` — Gravis UltraSound patch loader; LZEXE 0.91-compressed DOS executable.

### Runtime scripts/configuration
- `RLOFT.BAT` — game launch wrapper and Gravis UltraSound environment handling.
- `SOUND.BAT` — sound-installer wrapper and Gravis UltraSound environment handling.
- `MAKSAV.BAT` — save bootstrap: creates `SAVE01`, copies `LEV022.DAT` and `SAVE.DAT`, then removes the working copies.
- `README.BAT` — displays `README.TXT` through `MORE`.
- `PATCHES.INI` — UltraSound `LOADPATS.EXE` patch-selection table for AIL 32.
- `README.TXT` — shipped release/readme documentation.
- `SAMPLE.AD` — **confirmed Miles AIL 2.x YM3812/OPL2 Global Timbre Library (GTL)**. Its canonical resource-format reconstruction lives at `Resources/MISC/SAMPLE.AD/`; duplicate runtime/media copies are the same middleware asset and do not create a second workstream.

### Audio DLLs
18 supplied DLLs have been identified as 32-bit Intel LX-format DLLs. Embedded copyright/device strings identify the family as Miles Design audio drivers (with the two GUS drivers additionally crediting Advanced Gravis and Forte Technologies). These are initially treated as third-party runtime dependencies, not DreamForge gameplay code.

Driver set:

`A32ADLIB.DLL`, `A32ALGDG.DLL`, `A32ALGFM.DLL`, `A32ARDG.DLL`, `A32ARXM.DLL`, `A32MT32.DLL`, `A32PASDG.DLL`, `A32PASFM.DLL`, `A32PASOP.DLL`, `A32SBDG.DLL`, `A32SBFM.DLL`, `A32SBPDG.DLL`, `A32SP1FM.DLL`, `A32SP2FM.DLL`, `A32SPKR.DLL`, `A32TANDY.DLL`, `GUSDIG32.DLL`, `GUSMID32.DLL`.

The Resources track has additionally confirmed that shipped `GM1.BNK` / `GM2.BNK` are Sierra Semiconductor ARIA General MIDI banks associated with the ARIA XMIDI path; Ravenloft's corresponding 32-bit driver is `A32ARXM.DLL`. Their inner bank grammar remains a Runtime/driver-side question rather than a DreamForge resource-format requirement.

## Confirmed initial findings

1. `R.EXE` is an MZ DOS executable containing the DOS/4GW Professional Protected Mode Runtime 1.96 and explicit references to both `code.1` and `code.2`. This directly confirms that the executable is part of the CODE-module loading/runtime chain and warrants full structural analysis.
2. `INSTALL.EXE` is an MZ DOS executable and contains a Borland C++ 1991 runtime/compiler identification string. Its embedded shipped-file names include `CODE.1`, `CODE.2`, `LEVELS.DAT`, `NPCS.DAT`, `OBJECTS.DAT`, `INFO.DAT`, `LEV022.DAT`, `SAVE.DAT`, the audio DLL set, and the sound-launch infrastructure.
3. `SINSTALL.EXE` is a DOS/4G-bound Watcom program. Embedded strings identify it as `SSI Sound Flat Model Installer for Ravenloft 4.2.6b` and expose configuration for MIDI/digital devices including Sound Blaster variants, AdLib, Roland/MPU-401, Gravis UltraSound, Aria, Soundscape, PAS, and PC speaker.
4. `LOADPATS.EXE` is LZEXE v0.91 compressed. `PATCHES.INI` explicitly describes itself as the default UltraSound patch table used by `LOADPATS.EXE` for `AIL 32`.
5. `RLOFT.BAT` and `SOUND.BAT` are authored/revisioned scripts (Ralph Thomas; revision history dated 1994-01-24 and 1994-02-07) that test `ULTRADIR`, handle obsolete Gravis software revisions, and ultimately invoke `R.EXE` or `SINSTALL.EXE`.

## Reverse-engineering priority

1. **R.EXE** — full header/extender map, entry path, CODE.1/CODE.2 loader, memory/file/runtime services, boundary with reconstructed modules.
2. **SINSTALL.EXE** — recover `SOUND.CFG` format and exact driver-selection/configuration contract needed for faithful compatibility.
3. **INSTALL.EXE** — reconstruct installation/file-layout behavior where it contributes preservation evidence.
4. **LOADPATS.EXE** — unpack and document its Gravis/AIL contract; decompile only to the depth needed for faithful runtime behavior.
5. **Audio DLLs** — document format, driver identity, interface/exports and call contract. Full source-level decompilation is deferred unless Ravenloft depends on undocumented behavior that a source port must reproduce. `A32ARXM.DLL` is the specific future source of truth if byte-level GM1/GM2 ARIA bank semantics become necessary.
6. **BAT/INI/TXT files** — preserve verbatim and annotate; no decompilation is required.

## Next checkpoint

Phase 1 continues with a byte-level structural map of `R.EXE`: DOS header, extender/bound executable structure, entry point, embedded runtime regions, strings/file references, and the first proven loader relationships to `CODE.1` and `CODE.2`.
