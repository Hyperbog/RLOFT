# Alpha 0.12 — legacy v0.7 internal rendering resolution, 3D controls and native branch cleanup

**Date:** 2026-10-12 Eastern / GitHub Windows acceptance 2026-10-13 UTC.

## Controlling result

The legacy owner-supplied `/mnt/data/ravenloft_viewer_v07.zip` Python source was **inspected directly**, not inferred. `opengl_viewer.py` specifies `RESOLUTIONS=[None,(1280,720),(1920,1080),(2560,1440),(3840,2160)]` and renders into a **separate offscreen color+depth framebuffer** before fitting to the current window with correct letterboxing. This is emphatically not a cycle of window sizes.

The independent C++/Win32 native GPU view now implements this same actual five-position **internal render resolution** cycle on F2. New portable/Windows-independent API under `NativeEngine/include/rloft_native/native_win32_gl_target.hpp` + `src/native_win32_gl_target.cpp` acquires actual WGL OpenGL framebuffer extensions when available; allocates source image and depth targets at each selected size, renders 3D geometry into those targets, and presents with aspect-preserving letterboxing. Auto mode renders to the actual window and needs no FBO. FBO unavailable or resolution above GPU limits produces a meaningful user-facing error and preserves the previous configuration.

**Actual Windows/MSVC test:** [run #38340993133](https://github.com/Hyperbog/RLOFT/actions/runs/38340993133), **34/34 native CTests passed**. All 32 original levels accepted, **444 distinct original RES4 raster IDs** decoded and checked against the uploader contract, 38,419 lower wall faces, 8,936 upper faces, 87,983 original floor faces and 76,761 original ceiling faces. Seven original media files `INIT`, `RES0`, `T`, `LEVELS.DAT`, `CODE.2`, `RES4`, `COLORS` preserved byte-for-byte.

**Canonical engine source:** `native-engine:43210345950622abb3ed3f0432bbb49919f71c64` before documentation-only commits. **Exact Windows compiled acceptance branch SHA:** `8ed3337213a9fbdb2c2224db07d1e2cde9922040` (deleted after CI success; branch differs from canonical source only by isolated build/cleanup workflow configuration). **Windows EXE SHA256:** `697ff473b586bfe2f4ea1dac2cfc8385fe5421104d132a3093f6381ecc0c132b`. Original CI six-file ZIP SHA256 `3000098a5ad3fc5dc5a6324be49f24e679a282f7372948f51725be81e797c8ba`. Outer GitHub artifact ZIP SHA256 `af402799c3d1d3c0e8948f9508e152ba5d2495158f904fcdec548e81ae1ecd35`. User-friendly 7-file Alpha0.12 ZIP (extra README only) SHA256 `bfa2d78fa68ac647ac1aacb30c8f34f724f943f7e1ff18ea139c454ec570e3bc`. [Official artifact #11786796709](https://github.com/Hyperbog/RLOFT/actions/runs/38340993133/artifacts/11786796709) original game-file-free.

## User's actual v0.7 feature mapping

| Input | Current native 3D GPU view | Legacy source behavior |
| --- | --- | --- |
| W/S, A/D | Walk and strafe (source level collision) | Continuous held-key sample, bounded `dt` |
| Left/Right Shift | Run at 3.5 cells/s vs walking 2.05 cells/s | v0.7 sprint speed 3.5 vs walk 1.8 |
| Q/E | Yaw left/right; no arrow-key turning | Preserves owner's requested newer native controls |
| M or right-click | Toggle optional relative mouse yaw/pitch | Relative mouse capture; off by default, on focus loss releases |
| Horizontal mouse | **Reversed from Alpha 0.11** in response to reported inversion | Review live GPU direction during owner field test |
| F1 | Toggle actual in-GPU controls HUD via WGL font | v0.7 help visibility |
| F2 | **Native/Auto → 1280×720 → 1920×1080 → 2560×1440 → 3840×2160** | Exact legacy `RESOLUTIONS` modes and offscreen+letterbox design |
| F3 | Nearest → bilinear → trilinear mipmapped presentation | 3 modes (legacy third is a GLSL derivative-aware *crisp* shader; source pixels preserved, not exactly reimplemented) |
| F4 | Off → 2× MSAA → 4× MSAA, driver capabilities permitting | Old target samples `[0,2,4]` |
| F8 | Map size 150 → 210 → 300 pixels | Exact legacy panel cycle |
| F11 | Independent monitor fullscreen / restore | Legacy fullscreen independent of F2 internal resolution |
| F12 | Save frontbuffer BMP screenshot with displayed map/HUD | Legacy saved PNG instead; screenshot functionally restored |
| Tab | Toggle read-only per-session minimap | Legacy M mapped; M reserved for owner's optional mouse-look preference |
| +/- | Zoom map | Legacy zoom |
| N | Diagnostic noclip | Legacy noclip |
| PageUp/PageDown | Other original level | Earlier request; differs from v0.7 bracket binding |
| R | Reset original-world exploration spawn | Earlier request; differs from v0.7 texture reload |
| Esc | Release mouse on first press, then close preview | Legacy Esc releases capture first |

Modern GL4.x, GLSL 3.3 `GLRenderer`, source PNG override editing, v0.7's F6 editor, F5 export, dynamic original RES3/EH7 trees/animated doors, exact third GLSL edge-preserving Crisp filter and capture-to-PNG are **not yet ported** and MUST NOT be claimed feature-complete. The current GPU backend is OpenGL 1.1-compatible WGL with optional framebuffer/multisample extensions.

## Reported GPU error

The owner's Alpha 0.11 screenshot showed a `GPU preview` generic exception, hiding its `what()` detail, while the incomplete CODE.2 VGA view was visible behind it. Alpha 0.12 changes `WM_CREATE`, `WM_TIMER`, `WM_KEYDOWN`, and `WM_PAINT` to report underlying exception strings rather than swallowing them or displaying only a generic warning. An original-media C++ smoke now decodes/validates **all 444 distinct original RES4 IDs used by the source GPU 32-level mesh** (EH6 32/64×128 single-frame contract). This rules out malformed texture shapes **for the original branch media only**; different installed/patched GOG media and specific GPU/driver behavior can still fail. Runtime GPU rendering/mouse/screenshot/fullscreen and FBO quality modes **have not been physically field-tested on the owner's Windows GPU**. Do not claim the original error resolved until that run passes; the actual new exception text is critical if it recurs.

The second **CODE.2 VGA reference view remains incomplete** because original live 32954 camera/raycast and wall/actor draw scheduling are not yet reconstructed. Keep it an isolated research diagnostic; use `Explore Original World (3D GPU)` for interactive exploration. Native GPU also lacks full original scenery, split outdoor trees, source actor placements, animations, complete game UI/combat/party/save progression. First-playable milestone stays open.

## Native branch consolidation — VERIFIED SUCCESS

The one-time Windows acceptance workflow ran a separate `prune-obsolete-native-branches` job **after the 34/34 build and asset audit passed**. The job used a narrowly whitelisted list of names (not blanket wildcard Git branches), requested scoped `contents: write`, successfully deleted **11** former native-engine acceptance branches (including the current temporary acceptance branch last), and wrote `NATIVE_OBSOLETE_ACCEPTANCE_BRANCH_PRUNE_PASS`. [Audit trail](https://github.com/Hyperbog/RLOFT/actions/runs/38340993133).

Remaining repository branches independently fetched after cleanup:

- `native-engine` — **only surviving native engine branch** and canonical active development
- `code1-decompilation` — repository default branch, NOT to be deleted
- `DnD-Classics` — original game data source, NOT to be deleted
- `demo-v1` — separate gameplay project branch
- `disassembler-foundation` — independent tool branch
- `extractor-wpf-fluent` — independent tool branch

The normal `native-engine` original Windows release workflow remains manual-only. Original game resources must never enter compiled redistribution archives. Next development requires **live GPU user field acceptance and source-placed RES3/INIT scenery**, while original DOS VGA scene parity remains a separate preservation milestone.
