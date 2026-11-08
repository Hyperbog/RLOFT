# Recomp game-system architecture (production migration, phases 1–2)

**Owning branch:** `Recomp`. **Language:** C++17 (existing CODE.1/CODE.2 and host ABI).
**Status:** restructuring in progress, **not** a release certification.

The executable must retain **one authoritative CODE.1/CODE.2 guest, heap, resource cache, graphics device and save state**. The new `Recomp/src/game/` paths organize host-owned game systems; they do not replace original behavior with a second engine. Historical source remains at `Code1/Code1.cpp` and `Code2/Code2.cpp`, identical across the research and runtime workstreams. Original media remains on `DnD-Classics`, never in packages.

## Systems and ownership

| Game system | Canonical implementation / current source owner | Next boundary |
| --- | --- | --- |
| **Core / runtime lifecycle** | `src/game/core/Game.hpp` (persistent guest session API), `src/canonical_code2_runtime_owner.hpp`, original CODE.1/CODE.2 | Define source-state ownership and orderly shutdown |
| **Memory / DOS extender** | `src/dos_paragraph_memory.hpp`, `src/dpmi_*memory.hpp`, `src/dpmi_selector_descriptors.hpp`, `src/extender_data_break.hpp` | Shared DOS/4GW service facade; no synthetic allocations |
| **Filesystem / asset I/O** | `src/game/filesystem/Filesystem.hpp` (read-only DOS transport), `src/original_code2_checked_dos_seek.hpp` | Source-selected file reads, overlay policy, DOS errors |
| **Resources / asset cache** | `src/game/resources/Resources.hpp` (dependency-free scene-bank selection), `src/game/resources/PaletteLoader.hpp` (original 325B0 COLORS I/O), `src/code2_original_res_startup.cpp`, `Code2/graphics/original_res_image_archive.hpp` | Extract archive resolver/decompression/cache without copying guest |
| **Graphics / VGA** | `src/game/graphics/Graphics.hpp`, `Code2/graphics/`, compiled CODE.2 drawing | Retained real framebuffer, palette, original title glyphs |
| **UI / fonts / menus** | `src/game/ui/UI.hpp` (original 5E38 GAME.OPT load and settings), CODE.2 `301A8/303C8/30508/192CC/19464` | Connect decoded RES0 font and physical menu selection |
| **Input / cursor** | CODE.2 `308F8/30B40/1A288/1A1F4`, `src/original_dpmi_real_mode_mouse.hpp` | Authentic physical INT33 and IRQ9 lifetime |
| **Timing / interrupts** | `src/game/timing/Timing.hpp`, `src/original_code2_int08_vector_preflight.hpp` | Authenticate IRQ8 cadence before live scheduling |
| **Audio / music / SFX** | `src/game/audio/Audio.hpp` (merged VOC and source packet parsers), `src/original_source_winmm_bridge.hpp` | Miles AIL and sound-driver parity |
| **Cinematics** | `src/code2_original_cine00_player.cpp`, original CODE.2 `19888` | Same-guest intro-to-menu presentation |
| **World / levels / scene** | `src/game/world/World.hpp` (original 36CEC clipping and raster), `src/code2_original_level_loader.cpp`, `src/code2_original_world_frame.cpp`, original `181B4/1FC14` | General world loading and scene rendering, not per-level forks |
| **Party / characters / inventory** | Original CODE.1 character creation and CODE.2 actor/party routines, saved actor bridges | Continuous source handoff and original equipment behavior |
| **Combat / spells / gameplay** | CODE.2 reconstructed actor, damage, spell and effect routines | Live interactive source gameplay with no host outcome overrides |
| **Scripts / dialogue / events** | CODE.2 `002A8/1C368` and RES9 wordcode | Complete active event dispatch and text display |
| **Saving / loading** | `src/game/save/Save.hpp` (WORK/LEV snapshot overlay, 181B4 actor proof and 07938 restore policy), `src/code2_original_save_game.cpp` | All legitimate source-selected saves, no test-only level IDs |
| **Modding / extensions** | `MOD_FORWARD_METHODOLOGY.md`, read-only original parsers | Stable symbolic IDs and separate optional override layer |

## What was actually consolidated

- **Graphics**: moved the functioning VGA host into `game/graphics/Graphics.hpp`.
- **Audio**: merged `original_voc_pcm8.hpp` and `original_code2_pcm_packet.hpp` into **one** `game/audio/Audio.hpp` implementation.
- **Timing**: moved the original IRQ8 elapsed scheduler into `game/timing/Timing.hpp`.
- **Filesystem**: moved the bounded DOS file transport into `game/filesystem/Filesystem.hpp`.
- **Core**: moved the persistent CODE.2 guest-session API into `game/core/Game.hpp`.
- **Resources**: extracted scene palette-bank selection to `game/resources/Resources.hpp` and original 325B0 COLORS I/O to `game/resources/PaletteLoader.hpp`. The independent headers keep palette metadata tests free of CODE.2 executable dependencies.
- **World**: moved the original 36CEC scene clipping/raster adapter into `game/world/World.hpp`.
- **UI**: moved original 5E38 GAME.OPT guest-state and device-settings handling into `game/ui/UI.hpp`.
- **Save**: merged WORK/LEV file transport, saved actor verification and original source-selected restore/transition checks into `game/save/Save.hpp`.
- Former `src/*.hpp` entry points are **short compatibility forwards**, not duplicate implementations. Principal resource, world, options and save tests now include owned paths directly.

All module moves preserve existing namespaces, functions, data structures and the original guest. The main source consumers and relevant tests now use the canonical headers. No original gameplay branches or file formats were rewritten.

## Independent C++ translation units and modular CMake (phase 3)

- `src/game/audio/Audio.cpp`: the original strict Creative VOC and CODE.2 27F24 packet decoders have **out-of-line** implementations; `Audio.hpp` exposes their stable declarations. Static library: `rloft_game_audio`.
- `src/game/resources/Resources.cpp`: the data-driven 32-scene palette-bank selector has an **out-of-line** implementation; `Resources.hpp` exposes the selector interface. Static library: `rloft_game_resources`.
- `cmake/GameSubsystems.cmake`: owns these static libraries, C++17 compilation requirements and explicit executable links. The top-level CMake file includes it after defining executable targets. The source-inventory audit now walks CMake module includes and reports orphan sources.
- The old CODE.2 `.cpp`-to-`.cpp` embedding in resource startup and world-frame integration is **not** yet a separable ABI; moving those files without proving symbol and state ownership would duplicate the compiled original game routines. Extract their host callback contracts into independent modules only after confirming the same retained guest and original function ordering.
- Remaining World, UI, Save, Graphics, Filesystem and Timing adapter implementations still use header-defined functionality. Keep compatibility forwarding headers until all consumers have been moved and regression-tested.

No commercial game data is linked or packaged with these libraries.

## Deletion and release rules

**Do not delete** original `MASTER` media, research evidence, `Code1/Code1.cpp`, `Code2/Code2.cpp`, or compatibility headers still included by existing build targets. Git already deduplicates identical blobs across branches. A file is removable only after verifying *all* CMake targets, source includes, tooling and GitHub workflows no longer refer to it, and after the relevant builds pass.

Keep separate **engineering/preview artifacts** and a future **production release**. Production requires every gate in `Recomp/ACCEPTANCE_GATES.json` to be accepted and independent Windows game-visible testing. A successful CTest or packaging job alone is not that evidence. Never ship commercial resources, archives, saves, or original executables.

## Phase 4: real resource-cache and Save/World binary interfaces

- `game/resources/ResourceCache.cpp` resolves and authenticates RES directory records for the **existing** original CODE.2 31334 stream callback. Registered image pointers, heap allocations, decoder state and cache remain owned by the single guest.
- `game/save/Save.cpp` implements the existing WORK/LEV overlay object methods, saved actor validation and source-selected restore/transition predicates out of line. The template-only exception-injection helper remains header-defined. No save bytes or original-media content are embedded.
- `game/world/World.cpp` holds the existing 36CEC rasterization, table and clipping helpers. The generic `original_36cec_clip_run` template stays in the header. VGA and guest memory are passed by reference, retaining prior transactional rendering.
- Libraries `rloft_game_resource_cache`, `rloft_game_save`, and `rloft_game_world` are linked only into their relevant host/test consumers. The CODE.2 reconstruction is not compiled twice.

**Unchanged blocker:** The large CODE.2 startup and world diagnostic translation units still embed original CODE.2 source and `.cpp` harnesses; disassembling those into independent object files requires establishing CODE.2's non-inline symbol and guest-global ownership first.

## Phase 5: borrowed CODE.2 guest callback boundary

- `src/game/core/GuestCallbacks.hpp/.cpp` isolates the host-owned DOS IVT get/set service and the original 171FC/181B4 guest-memory-to-private-save-overlay transfers. It receives the **existing** vector table, original DGROUP vector and session-owned `live_level_files::Overlay` by reference.
- The `code2_startup_gate.cpp` original callback signatures, original source STOP reasons, IRQ callback counters and original non-overlay DOS read fallback remain intact.
- `rloft_game_guest_callbacks` compiles separately, links the existing Save library and is required by the original startup, stacked mouse, resource startup, staged boot and Windows player entrypoints. A media-free ABI test verifies IRQ8/IRQ9 vector preservation, save roundtrips, and fail-closed bounds.
- This is **not** independent compilation of `Code2/Code2.cpp`. Original non-inline game functions, suspended guest stack, source CPU and heap remain in the current canonical translation unit; splitting those needs a separately authenticated per-function ABI and single-owner dependency graph.

## Phase 6: DOS services, IRQ vector lifecycle, and registration extraction

- `game/filesystem/DosServices.hpp/.cpp`: independent original 139AC CINEdd.ANM CD-prefix/filename validation and source-address-specific COLORS. alias. Original DOS `seg8` guest reads, read-only file handles, return codes and source STOP reasons remain in the owning callback.
- `game/timing/InterruptLifecycle.hpp/.cpp`: independent host-only legacy IVT fixture seeds and source 30BB4/308F8 installed-vector checks. The original ISR `interrupt_30708`, IRQ9 make/break, DOS interrupt service and elapsed scheduler **remain in the one original guest**.
- `src/original_code2_host_callback_registration.inc`: relocated original CODE.2 function-pointer hookups (DOS open/read/close, INT21/INT31, mouse, heap, graphics entry) verbatim into a dedicated source fragment, compiled inside `code2_startup_gate.cpp`. This is deliberate: these callback ABI types are generated by the original monolithic CODE.2 translation unit and must not independently compile another CODE.2 copy.
- Libraries `rloft_game_host_dos_services` and `rloft_game_interrupt_lifecycle` are linked to every startup/staged/Windows player guest owner; a new media-free regression validates exact DOS path restrictions, synthetic IVT fixture and source-installed vector contract. This does not certify actual physical DOS or BIOS interrupt parity.

## Phase 7: independent read-only DOS transport and VGA device

- `game/filesystem/Filesystem.cpp` now owns `ReadOnlyDosFiles`' existing DOS AH=3D/3F/42/3E file transport, virtual 0x920-byte CHARS and read-only CINE CD fallback. `Filesystem.hpp` retains the same state layout, constants and declarations; method bodies and error codes are unchanged. `rloft_game_dos_filesystem` links into the identified DOS file consumers.
- `game/graphics/Graphics.cpp` now implements `OriginalVgaHost`'s bounded VGA ports, four-plane latches, DAC6 palette, retrace and scanout. `Graphics.hpp` retains field layout, constants and signatures. `rloft_game_vga` is independently compiled; `rloft_game_world` links to it transitively.
- `src/original_code2_vga_callback_registration.inc` contains original `attach_original_vga` presentation, cursor, resource, sound, source movement and experimental 1FC14 callback assignments byte-identically. The fragment is compiled INSIDE the original owning guest function to preserve its single CODE.2 state, source callback ABI and order.
- Existing DOS file, VGA, source 290F4, saved-world and Windows native acceptance tests remain the behavioral gates. This refactor does not certify DOS/BIOS or analog VGA parity.

## Phase 8: presentation services and interactive input

- `game/ui/Presentation.hpp/.cpp` validates source `20294` page push, two 320×400 planar fills, page pop and `291B0` selection against borrowed guest counters and DGROUP depth. Only the original `2E590/29B40/2E5D4` routines actually change the VGA device.
- `game/input/Input.hpp/.cpp` holds the bounded host 308F8 port 60h/61h/PIC acknowledgment latch, exact original 20440 INT33 mouse service signatures, and the private E0ss physical-scan preflight. Original `308F8` make/break events and `30B40/F590` key mapping still execute on the same guest. No second input queue or mouse driver is introduced.
- Static targets `rloft_game_presentation` and `rloft_game_input` link to retained source owners and the new media-free `original_code2_presentation_input_host_abi` regression.
- This boundary refactor does not establish retail DOS interrupt cadence, mouse callback delivery, live `19464` selection or pixel-identical VGA presentation. Existing original-game acceptance gates remain open.
