# Audio interface dependency audit

Status: **DEPENDENCY BOUNDARY ESTABLISHED — genuine `SOUND.CFG` sample now available; full driver decompilation not required for initial faithful reconstruction.**

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

## 2. Genuine `SOUND.CFG` binary baseline

A genuine generated `SOUND.CFG` sample has now been supplied and inspected directly.

- exact size: **59 bytes (`0x3B`)**;
- binary, not an INI/text configuration;
- contains two NUL-terminated driver filenames:
  - offset `0x16`: `a32mt32.dll`;
  - offset `0x24`: `a32sbpdg.dll`.

Complete sample bytes:

```text
0000: 30 03 02 00 FF FF FF FF 78 00 20 02 07 00 01 00
0010: 01 00 7A 00 01 00 61 33 32 6D 74 33 32 2E 64 6C
0020: 6C 00 00 00 61 33 32 73 62 70 64 67 2E 64 6C 6C
0030: 00 00 04 00 03 00 08 00 02 00 00
```

The filenames are consistent with the installer's independent Music/Digital Sound Effects split: `A32MT32.DLL` is the MT-32 music backend and `A32SBPDG.DLL` is a Sound Blaster-family digital backend. The remaining numeric fields are configuration/device parameters and should not be assigned semantic field names until their CODE/SINSTALL readers/writers are traced or additional differently configured samples provide controlled comparisons.

This closes the previous uncertainty about whether `sound.cfg` was textual and proves that Ravenloft persists selected Miles driver identities directly in a compact binary record.

## 3. Driver role

The shipped DLL set separates recognizable music/FM/MIDI and digital-audio hardware backends (AdLib, Sound Blaster families, Pro Audio Spectrum, MT-32, speaker/Tandy and Gravis UltraSound variants). `LOADPATS.EXE`, `PATCHES.INI`, `RLOFT.BAT`, and `SOUND.BAT` form the special Gravis UltraSound native-mode support path.

Nothing in this boundary provides evidence that the hardware drivers own Ravenloft gameplay, event, resource, save, character, or module-transition logic. Their preservation value is hardware/middleware fidelity.

## 4. What the initial faithful reconstruction actually needs

The required compatibility contract is game-facing rather than device-facing:

1. parse or replace the semantics represented by `sound.cfg`;
2. preserve the distinction between music and digital sound effects;
3. reproduce CODE-side requests to initialize/shutdown audio, start/stop music, and play/stop digital samples as those call sites are reconstructed;
4. preserve any game-visible completion/status behavior if a CODE path actually depends on it;
5. map those operations to a modern backend.

A native port does **not** need to reproduce ISA I/O, DMA, IRQ handling, FM synthesis driver internals, MPU/MT-32 hardware access, or Gravis patch-loader implementation in order to reproduce Ravenloft's game logic.

## 5. Risk assessment

The only reason to promote a particular driver to core reverse-engineering scope would be discovery of a CODE call whose externally visible semantics cannot be determined at the Miles-facing boundary. No such blocker is currently established.

Accordingly:

```text
CORE / INITIAL FAITHFUL DECOMP
    CODE-side audio control semantics
    music-vs-digital configuration semantics
    audio asset identity/trigger behavior
    59-byte SOUND.CFG compatibility record (where legacy compatibility is desired)

OPTIONAL / END-OF-PROJECT PRESERVATION
    SINSTALL.EXE full decompilation
    LOADPATS.EXE full decompilation
    A32*.DLL full decompilation
    GUSDIG32.DLL / GUSMID32.DLL full decompilation
    exact original sound-card detection
    exact ISA/DMA/IRQ/hardware behavior
```

## 6. Remaining `SOUND.CFG` work

The former missing-sample limitation is closed. What remains is narrower: assign semantics to the non-string fields of the 59-byte record.

That can be done by either:

1. tracing the CODE reader / SINSTALL writer for these offsets; or
2. comparing additional genuine `SOUND.CFG` samples generated with different music/digital devices and I/O/IRQ/DMA settings.

This is a bounded format-reconstruction task. It does **not** justify decompiling the installer or driver suite.

## 7. Decision

**No optional executable/audio driver needs full reverse engineering before the initial faithful decompilation/port proceeds.**

The supplied `SOUND.CFG` strengthens that conclusion: the installer-to-game boundary is a small binary configuration record containing explicit backend filenames and numeric device parameters. Preserve all binaries and hashes, retain the current inventory, and defer deep hardware-driver work until the final preservation phase unless a future CODE-side audio call produces a concrete unresolved dependency.

For the initial port, treat the Miles/Gravis layer as a replaceable platform backend behind a Ravenloft-facing audio interface.
