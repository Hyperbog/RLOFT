# P0 — Original-byte evidence for nested DOS executable modules

**Source change:** generic signature-first binary classification, not a
game-specific parser. **No Windows build or smoke suite run.**

The original Strahd binaries already supplied in this Project were
inspected read-only to cross-check the nested-header layout:
 
| Original | Length | Full SHA-256 |
| --- | ---: | --- |
| CODE.1 | 277,787 | `fd9b709ceee190e6a70870f9012ebea3bc437cae6bc74e0150d6f922fa750fed` |
| CODE.2 | 558,689 | `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314` |

Both start with MZ, but the outer DOS MZ at physical offset zero
does **not** provide a usable direct `e_lfanew` to the LE image.
Both instead contain a *separate* bounded MZ at original file
offset `0x25D24`. That nested MZ's `e_lfanew` is `0x1EB8`,
pointing to the unique `LE 00 00` header at original offset
`0x27BDC`. This is original binary evidence for the
**generic** nested-module detector, not a rule for a specific
game or engine.

The MZ scanner requires a second MZ header, a bounded offset relative
to **that** module, and the proper extension signature. It reports
`le-signature` before the validated LE loader has checked object
and page geometry; validated `LeImage` remains authoritative.
Detection of multiple independent MZ-bound extended headers is
`ambiguous-mz-extended`, not first-match-wins.

Regression source coverage now includes synthetic direct/nested
extended modules, ambiguous multiple modules, MZ loader refusal,
and optional exact-full-hash original-file checks for CODE.1 and
CODE.2. These tests have **not** been compiled or executed.

## Collection availability and tooling boundary

The connected GitHub repository confirms the collection's primary
file entries by name, size and SHA-1 Git blob ID. Arbitrary binary
reads through the current GitHub connector fail its UTF-8 decode
constraint, and direct container networking is unavailable.
Consequently additional original families (Gold Box OVR, Hillsfar
MZ, Dark Sun, LX audio drivers) remain **available in the repository
but not independently inspected as bytes in this session**.

Neither the GitHub source branch nor CI receives proprietary files.
Actual original-file replay is the next verification gate once a
binary-safe transport path is provided.

## Outer DOS launcher metadata (original-byte inspection)

The original CODE.1 and CODE.2 files were inspected read-only for
their **outer** DOS MZ headers, independently of the later LE image.

| Field | Both original sources |
| --- | ---: |
| DOS MZ declared executable length | `62,532` bytes |
| DOS header / load-module file start | `512` bytes (`0x200`) |
| Initial real-mode CS:IP | `01BD:2382` |
| Outer MZ relocation entries | `78` |
| Relocation table end in outer DOS header | `342` (`0x156`) |
| DOS MZ declared extent vs nested MZ origin | `62,532 < 0x25D24` |

All 78 source relocation locations were confirmed to fit within the
outer declared file-backed load module. The checks establish *stored
DOS header geometry*, not actual runtime execution or a known DOS load base.

`MzExtendedModuleLocator` now supplies one format-neutral set of MZ-
bound module locations to both `BinaryFormatDetector` and `LeImage`.
The LE loader fails if a second extended module makes the source
ambiguous, rather than selectively trusting the single LE candidate.

`DosMzImage.LoadLauncherStub` provides **explicit opt-in** mapping
of only the outer DOS MZ declared load module. It requires a nested,
separately MZ-bound extended module beyond the declared DOS extent
and refuses outer headers that directly identify LE/LX/NE/PE themselves.
Launcher-stub analysis uses the existing source-backed x86-16 engine;
it does not decode LE/LX/NE/PE contents or mix source address spaces.

The CLI supports `--dos-stub` with `mz`, `mz-disasm`,
`mz-analyze`, `mz-relocate` and `mz-cross`.
Normal `mz` remains the standalone DOS loader and rejects nested
protected-mode images. `inspect` also reports the recognized
`ExtendedModuleCandidates` without decoding any of them.

Synthetic ambiguous LE/LX and malformed DOS-extent fixtures, plus
optional exact-source original regression assertions, were added.
**They have not been run.**
