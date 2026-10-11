# NativeEngine handoff — reuse the existing decomp demo startup pipeline
**Recorded:** 2026-10-11
**Owner instruction:** The decomp demo **already plays the intro cinematic, enters the animated original menu, and proceeds through character creation with timing and animations**. Treat that capability as existing work to **reuse**, not as an unimplemented subsystem to reinvent.

## Two separate current executable states

| Codebase | Demonstrated integration |
| --- | --- |
| `DemoV1/` (original decomp demo) | Original-source-traced live intro `CINE00`/`CINE12`, logos, palette fades, menu animation and input, CODE.2 supervisor → CODE.1 character-creation workflow, animated creation/timing, serialized `CHARS` transfer and attempted normal CODE.2 exploration. User explicitly reports this working through character creation. |
| `NativeEngine/` | Independently built, verified Windows x64 bootstrap reading an original GOG `game.gog`; user-observed **static** original CODE.2 menu compositing. This bootstrap currently omits the **already implemented demo runtime** and is deliberately non-playable. |

**Do not confuse these descriptions:** absence of gameplay in `rloft_native_bootstrap.exe` is an *integration deficit*, not an absence of implemented introductory gameplay systems in the repository.

## Exact inherited integration points (reviewed on the native-engine checkout)

1. **Boot dispatch:** `DemoV1/include/rloft/boot_flow.hpp` + `DemoV1/src/boot_flow.cpp`: `BootFlow::boot`, `finish_code2_startup`, `finish_cold_start_presentation`, `select_startup_menu`, `supervisor_dispatch`, `finish_character_creation`. Original-source-traced result status 10 routes CODE.2 selection 2 into CODE.1; character creation's completed status 10 returns CODE.1 → CODE.2.
2. **Cinematic decoding/playback:** `DemoV1/src/win32_main.cpp` `play_cinematic` (around source line 2267); `AnmCinematic::render_next` supplies original indexed framebuffer/palette, per-frame `wait_ticks` and original audio commands. On normal cold start (around 2606), `CINE00.ANM` then `CINE12.ANM` run with sampled audio and XMIDI dispatch.
3. **Original logo, fade, timing and menu:** same source around 2630–2715. Original logo sequence/fades, `StartupPresenter::menu_animation_frame(1..14)` at nominal VGA presentation cadence, `menu_frame(selection)`, `Host::selection/consume_click`, and menu cursor transformations are already wired. `Host` owns Windows window/input, original 320×400 4:3-aspect indexed presentation, waitable-timer scheduling and audio integration elsewhere in the same TU.
4. **CODE.1 entry/animation:** `DemoV1/src/win32_main.cpp` around 2713 onward: normal selection drives `BootFlow::supervisor_dispatch`, switches to raw RES0 cursor, instantiates `CharacterCreationPresenter`, runs the original-source-traced opening frame sequence and palette reveal with BIOS/PIT tick intervals, then the existing `CharacterCreationFlow` pages, input and MIDI/SFX/animations. This is **already implemented in the demo** and should not be recreated as a new native-engine subsystem.
5. **Actual transfer and re-entry:** around source 3670–3765, completed actors serialize a **0x920-byte original CHARS handoff** via `build_character_transfer`, then `BootFlow::finish_character_creation(ContinueModuleCycle)` + `supervisor_dispatch` return to CODE.2 and the original startup import/world-entry code path. User-supplied evidence establishes the demo runs through character creation; later gameplay/DOS parity are separately gated.
6. **Resource codecs and presenters:** `GameSource`, `RavenloftBootAssets`, `StartupPresenter`, `AnmCinematic`, `CharacterCreationPresenter`, `CharacterCreationFlow`, and corresponding indexed palette/timing contracts already exist. The bootstrap's `original_menu_preview.hpp` already reuses `StartupPresenter` correctly but stops at `menu_frame(-1)`.

## Corrected native-engine engineering priority

**Replace the bootstrap-only journey with the existing demo's real live startup sequence by reusing shared code and behavior.** Do not develop a duplicate cinematic player, duplicate character-generation state machine, speculative input mapping, new timing constants, or substitute sprite animations.

- **First**, inventory the demo's runtime dependencies and draw/input/audio/audio-timing lifecycle; introduce shared reusable translation units/interfaces where feasible, with the demo still compiling and its independent test suite unmodified. Avoid moving/copying the 3,857-line `win32_main.cpp` wholesale into `NativeEngine/` or relabeling the demo binary as a native engine.
- **Second**, connect the independent original-install-backed Windows launcher to this already-tested shared startup/character path. Preserve the exact source-traced CODE.2→CODE.1→CODE.2 status semantics, audio event timing, original palettes, interactive input and normal-route `CHARS` handling. Ensure writable profiles **never overwrite the user's installed CHARS/SAVE.DAT**, including imported existing-party paths.
- **Third**, run an actual Windows MSVC build/tests and **user-owned original install** end-to-end from `CINE00`/CINE12 through menu/creation, logging the first demonstrable runtime difference from the demo or original DOS game.
- **Fourth**, continue ordinary CODE.2 world/exploration/source-faithful systems, treating demo's existing gameplay implementation as the implementation base, not an external black box.

**Release identity remains independent:** `rloft_native.exe` is a new, separately branded original-install-backed product target, not a renamed `ravenloft_demo_v1.exe`. Temporary `rloft_native_bootstrap.exe` remains a diagnostic tool until shared live startup integration is verified. No original copyrighted game bytes are distributed.

## Fidelity boundary

The owner explicitly reports accurate timing and animation in their decomp demo. Source comments support specific PIT tick/70Hz timing reconstruction. **This does not by itself certify bit-perfect DOS VGA frames, IRQ scheduling, audio timing on all Windows hosts or complete-original-game equivalence.** Preserve separate original DOS validation and exact-source run test gates. The next milestone is **integrating already working behavior**, not starting cinematic/menu/creation development from scratch.

**Continuity refs:** [DemoV1 README](../DemoV1/README.md), [BootFlow](../DemoV1/include/rloft/boot_flow.hpp), [Original execution charter](../DemoV1/ORIGINAL_EXECUTION_FIDELITY_CHARTER.md), [first real native-engine Windows bootstrap](WINDOWS_VERIFIED_BUILD_20261011.md), [user original GOG install field test](WINDOWS_GOG_FIELD_TEST_20261011.md).
