# Alpha A1 — Original CINE00/CINE12 full-motion intro acceptance

**Compiled source SHA:** `5532bedc95ca705f1f25322881b0732098f6f13d`
**CI:** [Windows original-source validation #38346464375](https://github.com/Hyperbog/RLOFT/actions/runs/38346464375), **38/38 CTests passed**.
**Windows EXE SHA256:** `d2d827a64f1c9feec90eae398862a24b97776f794d756e8e3b9f304b5b28aeee`.
**Original game media included in artifact:** **None**. [GitHub artifact #11789381548](https://github.com/Hyperbog/RLOFT/actions/runs/38346464375/artifacts/11789381548).

## Working source-backed Alpha A1 changes

- Reused existing DemoV1 `AnmCinematic` ANM command interpreter by moving it into `Shared/StartupPresentation/{include/rloft/anm_cinematic.hpp,src/anm_cinematic.cpp}`. DemoV1's former public header now forwards to the shared implementation, its duplicate `src/anm_cinematic.cpp` is removed. No original DOS executable is executed.
- Extended the neutral `rloft::StartupVisualSource` read-only interface to `res0_font(index)` and `text(index)`. `DemoV1::RavenloftBootAssets` retains existing behavior and implements them. The common `Type2Font` is owned by the neutral shared visual module.
- `NativeStartupVisualSource` independently reads original `RES0` EH2 font records and obfuscated `T` text through **the existing NativeMedia read-only loader and native LZSS decoder**. No demo-media reader, duplicate font/text resource store, or second game-file scanner is needed.
- `NativeEngine/src/win32_native_startup.cpp` plays original source **CINE00.ANM, then CINE12.ANM**, before existing CODE.2 logos, title art and 14-stage original menu panels whenever both numbered CD files exist in the chosen owner installation. The shared ANM interpreter supplies actual palettes, 320×400 pixel changes, text/subtitles, audio-command records and frame waits. The Win32 host waits at the original BIOS PIT cadence (1193182/65536 Hz) and allows Space/Enter to skip to the previously completed title/menu. A loose DOS-only folder without both ANM media sources still opens the original RES0 startup/menu without inventing replacement video.
- Sound is **not yet played**: the ANM 5/6/9 music/VOC events are tracked by the source interpreter, but the playback bridge has not been moved into neutral native code. This must remain an explicit Alpha A1 gap, not advertised as complete cinematic audiovisual fidelity.
- The existing CODE.1 character-creation handoff **still does not** produce a party or move into normal game world; original CODE.1 presenter/0x920 CHARS/session integration remains open. Mouse-led menu selection still routes via `Shared/BootFlow`.

## Real original files audited in Windows acceptance

A separate sparse checkout of `DnD-Classics` `MASTER/Ravenloft - Strahd's Possession/CD_Extracted/RLOFT` provided user-owned `CINE00.ANM` (13,781,936 bytes), `CINE12.ANM` (1,446,666 bytes), `RES0`, `T`, and `COLORS` in CI; it was not included in the release build. `rloft_native_anm_audit.exe` re-read them with `NativeMedia` and executed the shared decoder on **every frame**. Result:

| Original cinematic | Authored frames | Visually changed frame hashes |
| --- | ---: | ---: |
| CINE00.ANM | 405 | 165 |
| CINE12.ANM | 91 | 80 |
| **Total** | **496** | **245** |

Source-traced audio events: **13**, actual native audio playback: **not yet implemented**. Frame aggregate hashes `1200994125776912763` (CINE00) and `12391062496700487297` (CINE12); frame size validated 320×400; no original DOS-presentation parity claimed. CI asserts original `CINE00.ANM`, `CINE12.ANM`, `RES0`, `T` and `COLORS` SHA256 are unchanged. `NATIVE_ORIGINAL_CINE00_CINE12_FRAME_PASS` and `NATIVE_ORIGINAL_ASSET_ENGINE_WINDOWS_BUILD_PASS` were logged. The prior CODE.2 RES0 logo/menu audit also passed, with **38/38** Windows native tests.

The exact distributable `rloft_native.exe` is **594,944 bytes**, SHA256 above. Inner verified media-free ZIP SHA256 `e42a3af5fbc676c13e952b46f23ea99fc2bbf8acd810410ceb5170f0ef7c0a8f`. This is an A1 **cinematic visual alpha preview**; **physical owner Windows runtime/visual/sound tests are pending**, as are the full original DOS fades/70Hz frame comparisons.

## Next Alpha gates, in order

1. **Original sound from numbered cinematic cues:** move just the existing VOC/XMIDI decoding and Win32 player into neutral source-backed interfaces, with `NativeMedia` loading original `RES5/RES6/RES10`. Preserve sound disabled/missing-driver fallback and a playable silent route; never treat MIDI as generic sound effect.
2. **CODE.1 character UI:** extract and attach existing `DemoV1` interactive character presenter, fonts/raster/cursor and selection input through the same neutral resource seam.
3. **CODE.1 → CODE.2 original CHARS transfer**: correct actor slot/party entry, player actor and level start from source, profile-writable saves separate from immutable installation.
4. **Playable Alpha route**: authentic interactive authored scene/entity/door, event, encounter and save/reload, with actual Windows field-play verification.

The release roadmap remains [NativeEngine/README.md](README.md). The authoritative development branch is **`native-engine` only**; DemoV1 and DnD-Classics branches are preserved.
