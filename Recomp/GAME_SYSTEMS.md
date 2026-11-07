# Recomp game-system architecture (production migration, phase 1)

**Owning branch:** `Recomp`. **Language:** C++17 (existing CODE.1/CODE.2 and host ABI).
**Status:** restructuring in progress, **not** a release certification.

The executable must retain **one authoritative CODE.1/CODE.2 guest, heap, resource cache, graphics device and save state**. The new `Recomp/src/game/` paths organize host-owned game systems; they do not replace original behavior with a second engine. Historical source remains at `Code1/Code1.cpp` and `Code2/Code2.cpp`, identical across the research and runtime workstreams. Original media remains on `DnD-Classics`, never in packages.

## Systems and ownership

| Game system | Canonical implementation / current source owner | Next boundary |
| --- | --- | --- |
| **Core / runtime lifecycle** | `src/game/core/Game.hpp` (persistent guest session API), `src/canonical_code2_runtime_owner.hpp`, original CODE.1/CODE.2 | Define source-state ownership and orderly shutdown |
| **Memory / DOS extender** | `src/dos_paragraph_memory.hpp`, `src/dpmi_*memory.hpp`, `src/dpmi_selector_descriptors.hpp`, `src/extender_data_break.hpp` | Shared DOS/4GW service facade; no synthetic allocations |
| **Filesystem / asset I/O** | `src/game/filesystem/Filesystem.hpp` (read-only DOS transport), `src/original_code2_checked_dos_seek.hpp` | Source-selected file reads, overlay policy, DOS errors |
| **Resources / asset cache** | `src/code2_original_res_startup.cpp`, `Code2/graphics/original_res_image_archive.hpp` | Separate archive resolver, decoder and cache without copying guest |
| **Graphics / VGA** | `src/game/graphics/Graphics.hpp`, `Code2/graphics/`, compiled CODE.2 drawing | Retained real framebuffer, palette, original title glyphs |
| **UI / fonts / menus** | CODE.2 `301A8/303C8/30508/192CC/19464`, existing source callbacks | Connect decoded RES0 font and physical menu selection |
| **Input / cursor** | CODE.2 `308F8/30B40/1A288/1A1F4`, `src/original_dpmi_real_mode_mouse.hpp` | Authentic physical INT33 and IRQ9 lifetime |
| **Timing / interrupts** | `src/game/timing/Timing.hpp`, `src/original_code2_int08_vector_preflight.hpp` | Authenticate IRQ8 cadence before live scheduling |
| **Audio / music / SFX** | `src/game/audio/Audio.hpp` (merged VOC and source packet parsers), `src/original_source_winmm_bridge.hpp` | Miles AIL and sound-driver parity |
| **Cinematics** | `src/code2_original_cine00_player.cpp`, original CODE.2 `19888` | Same-guest intro-to-menu presentation |
| **World / levels / scene** | `src/code2_original_level_loader.cpp`, `src/code2_original_world_frame.cpp`, original `181B4/1FC14` | General world loading and scene rendering, not per-level forks |
| **Party / characters / inventory** | Original CODE.1 character creation and CODE.2 actor/party routines, saved actor bridges | Continuous source handoff and original equipment behavior |
| **Combat / spells / gameplay** | CODE.2 reconstructed actor, damage, spell and effect routines | Live interactive source gameplay with no host outcome overrides |
| **Scripts / dialogue / events** | CODE.2 `002A8/1C368` and RES9 wordcode | Complete active event dispatch and text display |
| **Saving / loading** | `src/code2_original_save_game.cpp`, retained original saved-level policy | All legitimate source-selected saves, no test-only level IDs |
| **Modding / extensions** | `MOD_FORWARD_METHODOLOGY.md`, read-only original parsers | Stable symbolic IDs and separate optional override layer |

## What was actually consolidated

- **Graphics**: moved the functioning VGA host into `game/graphics/Graphics.hpp`.
- **Audio**: merged `original_voc_pcm8.hpp` and `original_code2_pcm_packet.hpp` into **one** `game/audio/Audio.hpp` implementation.
- **Timing**: moved the original IRQ8 elapsed scheduler into `game/timing/Timing.hpp`.
- **Filesystem**: moved the bounded DOS file transport into `game/filesystem/Filesystem.hpp`.
- **Core**: moved the persistent CODE.2 guest-session API into `game/core/Game.hpp`.
- Former `src/*.hpp` entry points are **short compatibility forwards**, not duplicate implementations. Production cinematic/resource/audio sources now include owned paths directly.

All module moves preserve the existing namespaces, public functions, data structures and test call sites. No source-backed gameplay branches or original file formats were rewritten.

## Deletion and release rules

**Do not delete** original `MASTER` media, research evidence, `Code1/Code1.cpp`, `Code2/Code2.cpp`, or compatibility headers still included by existing build targets. Git already deduplicates identical blobs across branches. A file is removable only after verifying *all* CMake targets, source includes, tooling and GitHub workflows no longer refer to it, and after the relevant builds pass.

Keep separate **engineering/preview artifacts** and a future **production release**. Production requires every gate in `Recomp/ACCEPTANCE_GATES.json` to be accepted and independent Windows game-visible testing. A successful CTest or packaging job alone is not that evidence. Never ship commercial resources, archives, saves, or original executables.
