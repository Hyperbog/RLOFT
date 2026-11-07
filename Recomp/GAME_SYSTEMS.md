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

All module moves preserve existing namespaces, functions, data structures and the original guest. The main source consumers and relevant tests now use the canonical headers. No original gameplay branches or file formats were rewritten. These are still header-based modules; separate translation-unit boundaries require follow-up ABI and build dependency extraction.

## Deletion and release rules

**Do not delete** original `MASTER` media, research evidence, `Code1/Code1.cpp`, `Code2/Code2.cpp`, or compatibility headers still included by existing build targets. Git already deduplicates identical blobs across branches. A file is removable only after verifying *all* CMake targets, source includes, tooling and GitHub workflows no longer refer to it, and after the relevant builds pass.

Keep separate **engineering/preview artifacts** and a future **production release**. Production requires every gate in `Recomp/ACCEPTANCE_GATES.json` to be accepted and independent Windows game-visible testing. A successful CTest or packaging job alone is not that evidence. Never ship commercial resources, archives, saves, or original executables.
