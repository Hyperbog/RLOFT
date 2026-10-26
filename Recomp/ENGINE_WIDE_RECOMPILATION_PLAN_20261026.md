# Original-source recompilation — engine-wide execution plan
**Decision:** 2026-10-26. **Branch:** `Recomp`. **Owner-original game media:** immutable in `DnD-Classics/MASTER`.

## Engineering objective

Deliver a **single general-purpose original CODE.1/CODE.2 runtime**, with faithful DOS/4GW, heap, input/timers, resource I/O, VGA, audio, world records, actor and scripting callbacks. Original `LEVELS.DAT`, `INFO.DAT`, `NPCS.DAT`, `OBJECTS.DAT`, original RES archives, saved game files and original compiled program logic determine the loaded world and behavior.

**No level-by-level implementations.** Individual levels and saves are **fixtures**, not engineering milestones. A verified saved Level 23 → 26 → 23 trace demonstrates several shared code paths but **does not complete the engine**. Never promote “levels visited ÷ 32” to a completion percentage.

### Architecture and fidelity rules

1. Prefer original reconstructed `code2::function_XXXXX` / CODE.1 routines. Map the authentic callback and ABI edges they require. Bridge only facilities provided by DOS/hardware/OS, not game-state decisions.
2. One persistent original guest machine, heap, DGROUP and graphics/audio resource state survives all frames, files and level transitions. Treat source game state and archives as authoritative; do not invent actor coordinates, scripts, successful callback return values, sprites, doors or sounds.
3. **No hard-coded level IDs or portal pairs in general engine paths.** Source-selected level IDs and transition records must be validated against original archive/runtime state, guest-memory bounds and original game constraints. Legacy 22/23/26 restrictions are **test-fixture-only constraints** until replaced by proven generalized source invariants. Removing safety checks without replacing them with source-driven validation is not permitted.
4. Original game-specific exceptional behavior is fine when it comes from *original code or content*. Host implementations that switch on a level number to implement gameplay are not.
5. Missing source capabilities stop at the exact missing dependency; no fabricated success. Explicit opt-in diagnostic commands and save injection may remain in private tests but are not player input or completed device support.
6. Preserve old strict regression traces and exact source SHA evidence. Reuse them to prevent regressions, **not** to set the next architectural target.

## Audited shared execution path and current blockers

| Shared subsystem / intended behavior | Existing original reconstructed entry and guest evidence | Source-side gap requiring general implementation |
| --- | --- | --- |
| Core DOS/4GW guest services | CODE.1 initialization and CODE.2 source initialization, stackful guest worker, DPMI/heap, DOS file adapter, source video/IRQ boundaries | Complete DOS/4GW selector and heap lifetime fidelity, input IRQ/timer services, consistent saved/world I/O for all source-selected paths, bounded guest-worker shutdown/diagnostic timeout |
| Content-driven world loading | Original CODE.2 `181B4` loads `LEVELS.DAT`, `171FC` saves world, `07938` restores a saved state; 32-level archive index; source `20850` actor postload | Main live transition adapter currently gates on `original_saved_level_policy::archived_pair()` (22↔23 and 23↔26 only); saved restore policy `requested==23` and `level_overlay.has(23)`; private portal helper explicitly only accepts 23↔26. Replace with general authenticated original state/file/portal validation, not another table of approved pairs |
| RES and palette loading | Compiled `2F29C` resource lookup, type-6/RES and raster decoders, `325B0` palette and original `1F8E0/1F8FC` compositor callbacks | Generalize source resource ownership and failure handling for all asset types/valid lookups; confirm palette/mode transitions rather than matching a specific scene fixture |
| 3D / polygon / exterior rendering | `1F1C8`, world-ray `335DC` and source VGA `3263C`; polygon `34DE8`, `33E6C`, `34110`; sprite `2C294`, `36CEC`; exterior `314B0`; compass `274A4`; page `1A510` | Common original callbacks still fail closed at `2C294` clip-save `29738`, clip `296C0`, overlay `2978C`, and exterior `314B0` clipped `2A6B0`. Complete them through real original routines/VGA bridge. Keep legal empty-raster passes, avoid per-level geometry expectations |
| Gameplay movement, events and actors | Original `1FC14` gameplay loop, `30B40` keyboard input boundary; camera `2E120`; collision/events `2D5D0`; archived type-8 portal `2D3D8`; original actor updates `1BDAC`, `0D550`, source event `002A8` | Missing common actor movement `ORIGINAL_1BDAC_MOVE_PENDING`, actor removal predicate, special actor actions, complete script dispatch, dialogue `1C77C`, and game-over `0CBC8` callbacks. Enumerate and implement complete action/AI/combat/inventory/spell/equipment/menu paths from original CODE.2; do not implement an enemy or puzzle uniquely for one test level |
| Audio pipeline | `28110` request queue, `28168` update, `27F24` sampled packet flow, AIL-shaped native WinMM engineering bridge; partial `28F58`, `29008`, `28AE0`, `288FC` | Complete original music/SFX channel, resource, timing, queue and device callback lifecycle; source currently stops if the original active audio driver is required. WinMM is a device adapter, **not** proof of original Miles AIL IRQ/DMA/audio parity |
| Full lifecycle and save/handoff | Real original `07938/171FC/181B4` source save chain on one guest, R.EXE supervisor work, CODE.1 source initializers | Implement ordinary user-driven save/load, game-over and CODE.1 character creation/handoff without special saved-slot injection. General game loop/service exits must be owned by the same runtime |

**Source inspection anchors (at report date):**
`Recomp/src/original_code2_saved_level_policy.hpp`;
`Recomp/src/original_code2_live_level_transition.inc`;
`Recomp/src/original_code2_live_full_frame.inc`;
`Recomp/src/original_code2_live_07938_restore.inc`;
`Recomp/src/code2_startup_gate.cpp`;
`Recomp/src/original_source_winmm_bridge.hpp`.
Full canonical reconstructed routines remain in `Code1/` and `Code2/`.

## Prioritized work packages — subsystem acceptance, not levels

### 1. Inspect the *complete* original call graph and remove fixture assumptions from runtime design

- Inventory authentic `call_*` callback edges, conditional `nullptr` wiring, and fail-closed `*_PENDING`/`*_NOT_CONNECTED` stubs in the canonical source path. Distinguish generic runtime dependencies from private diagnostic instrumentation and deliberate unsupported DOS/hardware behavior.
- Build a **dependency matrix**: original address → caller → callback ABI → source implementation → host device or file dependency → coverage fixture → known unsupported branch. Prioritize highly shared callbacks, not a chosen map or creature.
- Trace original `181B4` dispatch and `2D3D8` encounter path. Replace level-pair whitelists with source-selected archive records and validated live guest state, with strict memory/record bounds. Keep invalid records rejected and original files read-only.
- Verify one generalized loader against original `LEVELS.DAT` index for **all 32 records** in a parameterized regression. This asserts loader format compatibility only, **not** each level's playability.

### 2. Finish OS and resource infrastructure that every subsystem uses

Implement faithful DOS file, heap, DPMI/selector, timer and input services; preserve one guest worker without concurrent host access or uncontrolled waits. Follow original `2F29C` resource and source palette/load paths for every legitimate resource request. Native strict acceptance must include process exit and source guards; distinguish device emulation from game logic.

### 3. Close common renderer and gameplay callback edges

Wire the `1FC14` source frame's original clip, overlay, polygon, sprite, actor and compass branches **generically**, including frames containing no visible geometry. Then finish `1BDAC/0D550` actor movement, `2D5D0` interactions, original `002A8` scripts, dialogue, combat, spells and item/inventory callbacks as discovered. Each missing dependency gets its own ABI and source-state regression; no source callback bypass.

### 4. Complete event/audio/UX and state persistence

Wire actual player keyboard/mouse/timer behavior into original input scheduling. Implement original music/SFX request and playback lifecycle through a bounded host audio device. Complete ordinary save/load, CODE.1 character creation and transitions. The game must no longer depend on private `0xFFF*` keyboard probes to reach normal content.

### 5. Verify breadth by *systems* and original-DOS comparison

Acceptance should exercise **one unchanged engine binary and same original source callbacks** on generated/parameterized fixtures from many different archive records and saves. Cover indoor/outdoor views, empty/occupied scenes, textures, sprites, animations, gameplay actions, music, scripts, inventory, collisions and world transitions. Then synchronize retail DOS and recomp frames/input/audio at matching states, with reproducible indexed VGA/palette and event traces. Failing branches remain open; full success cannot be inferred from fixture counts.

## Progress measurement policy (effective immediately)

- Keep the existing **3/12 = 25%** major-gate acceptance ledger for continuity until a *whole gate* satisfies its original scope. It is **not 25% game completion**.
- Track shared **subsystem callback coverage**: inventory denominator is all reachable original callback obligations, not levels visited. Coverage percentage is **NOT YET MEASURED** until the call graph and reachable-branch census have been independently generated and source-audited.
- Track **generalized runtime service completion** and independently captured original-DOS equivalence by ABI/service type, with explicit numerator and denominator once proven.
- Retain Level 22/23/26, saved-slot 07938, and portal scanout hashes as **regression fixture names and evidence**, not a roadmap or percentage-of-game metric.
- A green artifact-build workflow, synthetic CTests, a private portal command or successful native VGA self-hash is not independent DOS gameplay equivalence.
- [Latest quantified checkpoint](PROGRESS_REPORT_20261026.md): 93/93 CTests but a comprehensive Windows stdin integration timeout after 2/3 source input polls at `ed001028`. Treat that as **runtime reliability** rather than another level-specific failure.

**Next concrete implementation task:** source-dependency census and generalized `181B4/2D3D8` validation design, **alongside** diagnosis of the existing guest-worker/third-input timeout. Only remove hard-coded diagnostic constraints when independent tests prove the generalized original archive behavior.
