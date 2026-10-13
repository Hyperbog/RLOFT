# Alpha A1: source-original CODE.2 logos, title and interactive menu

**Source:** `native-engine`, October 13, 2026. This is a **partial original startup presentation**, not the finished Alpha or full original DOS intro.

## Implemented

- Extracted `DemoV1`'s existing `StartupPresenter` and `IndexedFramebuffer` to a single source owner in `Shared/StartupPresentation`, with a `StartupVisualSource` interface exposing only original RES0 type-1 images and COLORS DAC6 palettes. The old demo public header forwards into that shared library. Its resource provider derives from the new interface. The old duplicate presenter source was removed from the demo's compilation.
- Added `NativeStartupVisualSource`: adapts the independent **read-only** `NativeMedia` reader and already verified native EH1 decoder to that interface. Does **not** import `GameSource`, use DOS binaries, create an unrelated loader, transcode original asset files, or link the demo executable.
- Added a separate **Original CODE.2 Startup** button to the existing independent Windows app, opening `win32_native_startup.cpp`. It draws the source indexed 320×400 framebuffer and selected palette (14/9/12) through an explicit top-down BGRA copy.
- Reuses original CODE.2 artist-supplied RES0 IDs **94 (first logo), 95 (second logo), 96 (third logo), 97 (title), 98–101 (animated menu panels), 102–105 (selected options)** at the original C++ compositing positions from source CODE.2. Source menu animation steps **1–14**, 70-Hz nominal presentation timer, original `StartupPresenter::menu_selection_from_y` mapping. **Space** skips to menu; **Esc** closes. Mouse movement highlights source menu selections; clicking passes actual menu index through the neutral `BootFlow`. Clicking character workflow advances the original supervisor to CODE.1 and **explicitly stops at the not-yet-integrated character presenter** rather than inventing a party.
- Startup window is a **cold-start visual preview**. On source installations with CHARS already present, this standalone inspection window still demonstrates source art; it is not claiming that the entire normal saved-party boot sequence is integrated.
- Original DOS palette fade-to-black/in, full CINE00/CINE12 ANM playback and audio, keyboard original-menu focus/timing, original CODE.1 character graphics and transfer, party/world launch, and source-authored gameplay remain open; visually drawn startup frames alone do not satisfy Alpha A1 completion.

## Verification

- `Shared/StartupPresentation/tests/startup_presenter_tests.cpp`: deterministic non-copyrighted synthetic fixtures assert actual logo draw coordinates, four panel endpoints, the frame-14 target, four menu hit regions, palette fade arithmetic and out-of-range rejection.
- `NativeEngine/tests/native_startup_visuals_tests.cpp`: independent synthetic loose-media RES0/COLORS/INIT/T filesystem fixture passes through **actual NativeMedia** and native EH1 decoder into **the same shared startup presenter**, tests pixels and preserves the original input files.
- `NativeEngine/src/native_original_startup_audit_main.cpp`: read-only CLI renders source RES0 IDs 94–105, all 14 menu animation frames and four selected menu states, checks real 320×400 indices and three original COLORS banks; explicitly reports that this is *not* DOS frame parity.
- Windows CI workflow `.github/workflows/native-startup-windows-ci.yml` builds the independent native x64 EXE, executes native CTests, then loads actual immutable original `DnD-Classics/MASTER/Ravenloft - Strahd's Possession/RLOFT` (separate sparse checkout), runs the real startup audit and confirms before/after SHA256 for RES0, COLORS and INIT. The Windows distribution never includes proprietary original media.

## Next A1 milestone

Connect `DemoV1/AnmCinematic` through a similarly narrow original **RES0 EH2 font and T text** visual provider and add original CINE00/CINE12 frames using CD-extracted media when available. Preserve original sound cues and timing without simulating the entire DOS executable. Then extract and integrate `CharacterCreationPresenter` and original CHARS 0x920 transfer, joining the supervisor's CODE.1 state to a real party and the source-world renderer.

No new native-engine branch or standalone demo fork is required. This step remains explicitly **source-presentation Alpha A1 partial**.
