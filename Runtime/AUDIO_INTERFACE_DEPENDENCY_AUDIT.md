# Audio interface dependency audit

Status: **DEPENDENCY BOUNDARY ESTABLISHED — full driver decompilation not required for initial faithful reconstruction.**

This checkpoint tests whether `SINSTALL.EXE`, `LOADPATS.EXE`, or the shipped Miles/Gravis driver DLLs conceal game logic needed to complete CODE.1/CODE.2.

## 1. Direct shipped-binary evidence

Both protected-mode CODE modules contain the audio configuration filename:

- CODE.1: `sound.cfg` at file offset `0x3EA10`;
- CODE.2: `sound.cfg` at file offset `0x77360`.

Both modules also contain the common DOS/4G/Miles runtime `.dll` search machinery. This proves audio configuration is consumed on the game side rather than existing only inside the installer.

`SINSTALL.EXE` independently contains:

- literal `sound.cfg`;
- literal `[MIDI]`;
- user-facing separation between **Music** and **Digital Sound Effects**;
- the shipped `A32*.DLL` driver names and device-selection data.

Therefore the installer/configuration boundary is structurally:

```text
SINSTALL.EXE
    -> chooses music device + digital-effects device
    -> writes sound.cfg
    -> CODE.1 / CODE.2 consume sound.cfg
    -> Miles runtime loads selected A32/GUS backend
```

The installer is a configuration producer. It is not the owner of the game's runtime audio decisions.

## 2. Driver role

The shipped DLL set separates recognizable music/FM/MIDI and digital-audio hardware backends (AdLib, Sound Blaster families, Pro Audio Spectrum, MT-32, speaker/Tandy and Gravis UltraSound variants). `LOADPATS.EXE`, `PATCHES.INI`, `RLOFT.BAT`, and `SOUND.BAT` form the special Gravis UltraSound native-mode support path.

Nothing in this boundary provides evidence that the hardware drivers own Ravenloft gameplay, event, resource, save, character, or module-transition logic. Their preservation value is hardware/middleware fidelity.

## 3. What the initial faithful reconstruction actually needs

The required compatibility contract is game-facing rather than device-facing:

1. parse or replace the semantics represented by `sound.cfg`;
2. preserve the distinction between music and digital sound effects;
3. reproduce CODE-side requests to initialize/shutdown audio, start/stop music, and play/stop digital samples as those call sites are reconstructed;
4. preserve any game-visible completion/status behavior if a CODE path actually depends on it;
5. map those operations to a modern backend.

A native port does **not** need to reproduce ISA I/O, DMA, IRQ handling, FM synthesis driver internals, MPU/MT-32 hardware access, or Gravis patch-loader implementation in order to reproduce Ravenloft's game logic.

## 4. Risk assessment

The only reason to promote a particular driver to core reverse-engineering scope would be discovery of a CODE call whose externally visible semantics cannot be determined at the Miles-facing boundary. No such blocker is currently established.

Accordingly:

```text
CORE / INITIAL FAITHFUL DECOMP
    CODE-side audio control semantics
    music-vs-digital configuration semantics
    audio asset identity/trigger behavior

OPTIONAL / END-OF-PROJECT PRESERVATION
    SINSTALL.EXE full decompilation
    LOADPATS.EXE full decompilation
    A32*.DLL full decompilation
    GUSDIG32.DLL / GUSMID32.DLL full decompilation
    exact original sound-card detection
    exact ISA/DMA/IRQ/hardware behavior
```

## 5. `sound.cfg` limitation

The supplied working set does not currently include an actual generated `sound.cfg` sample. Therefore its exact serialized byte/text schema is not certified here. `SINSTALL.EXE` proves the file is generated and associates it with separate music/digital selections, while CODE.1/CODE.2 prove it is consumed at runtime.

The exact schema can be recovered later either from a genuine installed-game sample or by narrowly reconstructing the SINSTALL writer/CODE reader. That is a bounded compatibility task, not a reason to decompile the entire installer or driver suite now.

## 6. Decision

**No optional executable/audio driver needs full reverse engineering before the initial faithful decompilation/port proceeds.**

The audio subsystem does not presently fill an unknown gameplay/code hole. Preserve all binaries and hashes, retain the current inventory, and defer deep hardware-driver work until the final preservation phase unless a future CODE-side audio call produces a concrete unresolved dependency.

For the initial port, treat the Miles/Gravis layer as a replaceable platform backend behind a Ravenloft-facing audio interface.
