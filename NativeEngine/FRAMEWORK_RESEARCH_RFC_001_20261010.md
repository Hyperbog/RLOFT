# Framework research RFC-001 — scalable, mod-friendly native engine

**Date:** 2026-10-10. **Owner:** `native-engine/NativeEngine`. **Status:** researched recommendation; **NOT an irreversible framework selection or a completed benchmark**.
**Goal:** replace the DOS runtime using the player's original Strahd installation, preserve an independently testable *original-faithful* executable, and leave clean extension points for an eventual OpenMW-scale ecosystem. No modernization before the original complete-game recreation.

## Recommendation

Use a **headless C++ game/simulation core** with an independent native application, **SDL3** for cross-platform window/input/gamepad/audio/filesystem integration and **bgfx** behind an `IRenderBackend` abstraction for initial GPU output and eventual optional 3D scenes. Keep an **SDL_GPU** implementation as a serious competing backend to validate before permanent adoption. Both are graphics/platform building blocks, **not full game engines**; content formats, mod tooling and scripting APIs must be designed by this project.

The first renderer must accept an unchanged source-correct **indexed 320×400 framebuffer + original palette**. Upload for display with selectable nearest-neighbor scaling and preserved aspect ratio; do not move gameplay calculations into shaders. A later shader/material/3D backend may render enhanced content without altering the source DOS simulation. Separate renderer and simulation time to retain original tick/RNG/IRQ and deterministic replay testing.

## Why not adopt an existing complete game engine immediately?

| Candidate | Strength | Project-specific cost / assessment |
| --- | --- | --- |
| **SDL3 + bgfx + native C++ core** | Modern platform APIs, multiple render APIs, no imposed scene or save system; enables indexed VGA plus substantial later 3D rendering | Own application/editor/content infrastructure; shader tools/build setup. **Preferred provisional design** |
| **SDL3 + SDL_GPU + native C++ core** | More integrated dependency stack; official Vulkan/Direct3D12/Metal 3D/compute API; shadercross | Explicit GPU resource/pipeline implementation; narrower backend/device coverage than bgfx. **Benchmark alternative** |
| **Godot + C++ GDExtension** | Complete high-quality 2D/3D renderer, UI, 3D scene editor, plugins, PCK/ZIP mod packs and import plugins | DOS indexed renderer, irregular binary loaders and exact source tick/lifecycle behavior need a custom native layer; engine/runtime/editor/GDExtension compatibility and packaged-mod constraints. **Strong toolchain/alternate-host candidate** |
| **Unreal / Unity / O3DE** | Feature-rich authoring, 3D tools and ecosystems | High integration/runtime complexity relative to initial 320×400 palette/raycaster and strict source semantics. **Not preferred for source-faithful core** |

Do **not** embed Godot, bgfx or SDL GPU types into the world, save, script, level, party or rules APIs. This avoids renderer lock-in and allows a later Godot-based mod editor or alternative front end. OpenMW's historical move from Ogre3D to OpenSceneGraph is a practical reminder that rendering dependencies can outlive their original suitability; separate them at the boundary.

## Modular target

```text
rloft_native (standalone launcher / installer / safe profile)
  ├─ platform         SDL3 input, window, clocks, audio, dialogs
  ├─ original_media   verified GameSource, DAT/RES/ANM/CODE evidence readers
  ├─ virtual_files    immutable original installs -> enabled mod mounts
  ├─ simulation       portable deterministic world/party/events/RNG/IRQ
  ├─ rendering_api    IRenderBackend, indexed-VGA baseline + optional scenes
  │    ├─ bgfx_backend          [benchmark first]
  │    └─ sdl_gpu_backend       [benchmark comparator]
  ├─ mod_runtime      manifests, load order, data/schema, Lua API/events
  ├─ persistence      stable IDs, original save import, versioned native saves
  └─ tools            dedicated asset validators, later world/editor applications
```

Existing `DemoV1/include/rloft/game_source.hpp` already identifies `LooseDirectory` and `GogCdImage`. The resource layer decodes multiple proprietary types. Retain them as versioned, regression-tested **source adapters** rather than requiring users to export assets first. Do not treat the copied `DemoV1/` sources as a shipping engine or require its synthetic-party debug shortcuts.

## Modding must be a core data model, not a late plug-in

1. **VFS and load order:** read original installation *without modification*; overlay enabled loose mod folders or archives, apply explicit stable priority and dependency resolution, validate paths to prevent traversal, and provide profiles and file-origin diagnostics. OpenMW's multiple data directories and ordered mod precedence provide a useful design precedent.
2. **Stable content schema:** preserved original numeric IDs plus namespaced new IDs; declarative data patches for NPCs, items, dialogue, effects, maps, quests, UI and resources. Each entry carries a schema/API version and provenance. No hardcoded level-22-only registries.
3. **Scriptable events:** embed **Lua** through a narrow versioned API (sol2 is a candidate C++ bridge). Allow safe `on_enter`, `on_interact`, `on_tick`, `on_quest` and UI callbacks. Keep original event-bytecode interpretation independent; unsupported original opcodes stay unimplemented until source-proven. Default Lua scripts should have no unrestricted OS/file/network access and must obey load-identity, save and execution-budget rules.
4. **Extensible content:** permit new levels, campaigns, tiles, textures, meshes (e.g. glTF), sounds, shaders, UI and authored events *in future enhanced/content modes*. Do not promise seamless conversion of Strahd's original 2.5D authored levels to arbitrary 3D gameplay. Original and extended worlds may use different visualization backends while sharing game interfaces.
5. **Tools and diagnostics:** machine-readable import/export and validated editor schemas first; a lightweight dockable developer inspector (Dear ImGui) soon after. Evaluate a separate Godot editor/import plug-in for modern scene authoring, instead of binding core simulation to the Godot runtime. Mod API documentation/examples are release deliverables.
6. **Persistence:** script state is explicit, namespaced and migration-versioned; original-game-compatible saves are a distinct gate. Use hashes of ordered enabled mods, protected per-profile saves and graceful unresolved-mod handling. Never allow untrusted mod assets to silently overwrite the user's original files.

**ECS choice:** **optional**, not foundational. The current EnTT mainline requires **C++20** and recent CMake, whereas the inherited native code is **C++17**; either isolate an entity adapter, test a specifically pinned older compatible release or migrate the relevant component deliberately. Do not refactor all faithfully reconstructed state into ECS without byte/source semantics evidence.

**Licenses:** SDL3 (zlib), bgfx (BSD-2), Godot (MIT), Lua (MIT), and optional libraries are permissively licensed subject to attribution. OpenMW itself is **GPLv3**: learn from its exposed architecture/modding design but **do not copy source** into the repository without a deliberate compatible-license decision. User-supplied game assets must not be included in engine releases.

## Proof-of-choice benchmark (prior to committing to the renderer)

Implement two **disposable, non-gameplay proof hosts** around the *same* immutable media reader and indexed frame test:

- SDL3+bgfx versus SDL3+SDL_GPU: on the same Windows machine, compare **cold build/rebuild complexity**, first-frame time, frame pacing, CPU/GPU memory, device/backends, shader build reproducibility and the ability to pixel-preserve the original indexed buffer/palette. Benchmark on at least one additional OS if cross-platform is a requirement.
- Verify: exact reference indexed frame SHA-256 before upload, palette swaps, nearest scaling at multiple resolutions, aspect ratio/alt-tab/window resize, no simulation changes, and identical output under a headless renderer.
- Load original `RES4[655]` through installed `GameSource` for one verified test and then mount one synthetic replacement via a namespaced VFS without editing the installation; demonstrate collision reporting, load ordering and SHA/provenance.
- Independently spike Godot GDExtension: the same exact indexed source frame through a native extension and a simple editable glTF/scene asset; measure project/export complexity and hot-reload/API friction. **If Godot wins decisively**, reevaluate the host while keeping the C++ core fixed.

## Decision gate

**Provisional choice:** **C++ portable core + SDL3 + bgfx, renderer interface, early VFS/schema/Lua contracts**. **No ECS framework lock-in.** Do not select the permanent renderer on preference alone. The proof host must compile and show a real original indexed resource while preserving deterministic simulation. Independently track correct original-game operation, enhanced rendering capability, and mod-system integration; these are separate outcomes. Full Windows workflows remain owner-authorized and manual. **No framework binaries built or benchmark run by this research RFC.**

## Primary sources reviewed

- SDL3 GPU API: https://wiki.libsdl.org/SDL3/CategoryGPU
- SDL3 license: https://github.com/libsdl-org/SDL/blob/main/LICENSE.txt
- bgfx overview/backends/license: https://github.com/bkaradzic/bgfx/blob/master/docs/overview.rst
- Godot official native bindings: https://docs.godotengine.org/en/stable/tutorials/scripting/cpp/about_godot_cpp.html
- Godot mod packs: https://docs.godotengine.org/en/stable/tutorials/export/exporting_pcks.html
- OpenMW Lua/sandbox: https://openmw.readthedocs.io/en/latest/reference/lua-scripting/overview.html
- OpenMW mod data precedence: https://openmw.readthedocs.io/en/latest/reference/modding/differences.html
- OpenMW historical renderer switch: https://openmw.org/2015/announcing-switch-openscenegraph/
- EnTT current C++ requirements: https://github.com/skypjack/entt/blob/main/CMakeLists.txt
- Lua manual: https://www.lua.org/manual/5.4/manual.html
- Dear ImGui docking/editor: https://github.com/ocornut/imgui/wiki/Docking
