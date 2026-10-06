# Audio interface dependency audit

Status: **CORRECTED / DEPENDENCY BOUNDARY BINARY-PROVEN.** Full driver decompilation is not required for the initial faithful gameplay reconstruction.

This audit distinguishes what is proved from the shipped CODE modules, what is proved from the supplied `SOUND.CFG` sample, and what remains installer/hardware preservation work.

## 1. CODE.1 and CODE.2 directly read `sound.cfg`

This is stronger than a string-only correlation.

### CODE.1

`SOUND.CFG` is stored in CODE.1 data object 2 at file `0x3EA10`, object-relative offset `0x2EC`. At logical `0x9652` CODE.1 pushes `0x2EC`, opens the file, transfers exactly `0x3B` bytes into its audio-configuration block, then closes the handle:

```text
9652  push 2ECh          ; sound.cfg pathname
9657  call 62A8          ; open
9662  push 3Bh           ; 59 bytes
9664  push [2AA0]        ; configuration buffer
966A  call 6304          ; transfer
9672  push ebx
9673  call 62F0          ; close
```

### CODE.2

`SOUND.CFG` is stored in CODE.2 data object 2 at file `0x77360`, object-relative offset `0x143C`. At logical `0x2F37F` CODE.2 independently performs the same 59-byte load:

```text
2F37F  push 143Ch        ; sound.cfg pathname
2F384  call 312A0        ; open
2F38D  push 3Bh          ; 59 bytes
2F38F  push [F050]       ; configuration buffer
2F397  call 312EC        ; transfer
2F39F  push ebx
2F3A0  call 312DC        ; close
```

Thus both game modules genuinely consume the same fixed-size audio configuration record.

## 2. Supplied `SOUND.CFG` binary baseline

The supplied sample is preserved here as a byte-level baseline without asserting installation provenance beyond the supplied file itself.

- size: **59 bytes (`0x3B`)**;
- SHA-256: `b373f84879c0dfe524009526989a3b6562f518a9a64280b4ee282673cb0474cc`;
- binary, not INI/text;
- driver string at offset `0x16`: `a32mt32.dll`;
- driver string at offset `0x24`: `a32sbpdg.dll`.

Bytes:

```text
0000: 30 03 02 00 FF FF FF FF 78 00 20 02 07 00 01 00
0010: 01 00 7A 00 01 00 61 33 32 6D 74 33 32 2E 64 6C
0020: 6C 00 00 00 61 33 32 73 62 70 64 67 2E 64 6C 6C
0030: 00 00 04 00 03 00 08 00 02 00 00
```

The filenames are consistent with the installer/device inventory: `A32MT32.DLL` is an MT-32 music/MIDI backend and `A32SBPDG.DLL` is a Sound Blaster Pro-family digital-audio backend.

## 3. Partial field structure is executable-proven

Immediately after loading the record, CODE.2 treats the block as structured configuration rather than opaque data.

It passes:

- words `+0x00`, `+0x02`, `+0x04`, `+0x06` together with driver-name pointer `+0x16` to one audio initialization routine;
- words `+0x0A`, `+0x0C`, `+0x0E`, `+0x10` together with driver-name pointer `+0x24` to another audio initialization routine when word `+0x14` is nonzero;
- word `+0x08` through additional device-special-case logic.

This certifies two separate game-facing audio configuration groups matching the installer's Music versus Digital Sound Effects distinction. Exact names for each numeric field remain pending; assigning I/O/IRQ/DMA labels solely from plausible values would be premature.

## 4. SINSTALL evidence and boundary

`SINSTALL.EXE` identifies itself as:

```text
SSI Sound Flat Model Installer for Ravenloft 4.2.6b
```

Its strings include `sound.cfg`, `[MIDI]`, separate Music and Digital Sound Effects selection UI, hardware-address/IRQ/DMA prompts, and the shipped A32/GUS driver names.

This strongly establishes SINSTALL as the configuration-selection program associated with `SOUND.CFG`. The exact writer routine has not yet been reconstructed, so this audit does not claim a byte-for-byte SINSTALL writer specification.

## 5. Driver role

All 18 shipped audio DLLs identify structurally as 32-bit LX/OS2-style DLL images used by the DOS/4G/Miles environment. Their names and embedded identities correspond to hardware backends for AdLib, Sound Blaster variants, Pro Audio Spectrum, MT-32, speaker/Tandy, and Gravis UltraSound.

No evidence found in this audit places Ravenloft gameplay, level, save, character, resource-decoding, or CODE-module state-machine ownership inside those drivers.

## 6. What the initial faithful reconstruction needs

Required at the game-facing boundary:

1. preserve Music versus Digital Sound Effects configuration semantics;
2. reproduce CODE-side initialization/shutdown and play/stop behavior;
3. preserve audio asset identity and triggering;
4. preserve any completion/status behavior actually observed by game logic;
5. optionally support the 59-byte legacy configuration record for compatibility.

Not required for initial gameplay fidelity:

- ISA port programming;
- DMA/IRQ implementation;
- original FM synthesis driver internals;
- MPU/MT-32 hardware access;
- Gravis patch-loader internals;
- full A32/GUS driver decompilation.

## 7. Remaining bounded work

The only unresolved core-facing audio-format task is semantic naming of the numeric fields and any game-visible audio timing/status dependency. Those can be resolved by tracing the two CODE initialization callees and/or comparing additional differently configured `SOUND.CFG` samples.

This does not currently justify reverse engineering the complete installer or hardware-driver suite.
