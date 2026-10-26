# Shared original CODE.2 execution — source dependency inventory and closure checkpoint

**2026-10-26 | branch `Recomp` | original game media kept read-only in `DnD-Classics`.**
This checkpoint reports source-wide engineering, not level-based implementation. The authoritative whole-game R01–R12 ledger remains **3/12 accepted**. Neither original DOS parity nor all mechanics are certified.

## Source call graph: scope and reproducibility

Run the deterministic original-source audit from the repository root:

```sh
python Recomp/tools/audit_original_source_callbacks.py --root . --out build/original-source-callback-census.json
python Recomp/tools/audit_recomp_source_layout.py --root .
```

The first command emits a JSON dependency ledger covering:
- canonical reconstructed `Code2/Code2.cpp` `function_XXXXX` tokens and per-function **direct** calls;
- separately listed indirect `call_XXXXX` callback ABI references, which are not assumed to be resolved direct calls;
- five retained live guest owners, callback assignments with exact source-file line numbers, conditional or explicit `nullptr` paths, and source stop/guard locations.

This is a **static C++ lexical graph**, not a complete dynamic/reachable DOS call graph. Function pointers, conditional runtime states, external DOS/4GW services, self-modifying VGA code and original game branches require separate execution witnesses. The graph does **not** certify that every discovered function is exercised or completed.

[Windows canonical source-link #40400460500](https://github.com/Hyperbog/RLOFT/actions/runs/40400460500) after closing the common source clip and actor sound edges passed **95/95 registered CTests**. Its integrated inventory reported **985** distinct original function-address tokens, **429** live-guest callback assignments, **11** nullable callback assignments and **56** fail-closed guard sites. **A stop-site count is not a count of missing features:** some guards intentionally enforce correct prerequisites and invalid-media rejection, whereas others point to genuine unfinished device/gameplay callbacks. Source-local counts can evolve as source changes. The later direct-call graph provides edge lists in the JSON report and must not be described as an independent original DOS trace.

## Code changes that close shared runtime edges

| Source subsystem | Previously restricted or fail-closed | Source-faithful shared implementation |
| --- | --- | --- |
| World lifecycle / original `181B4` | Host policy admitted only 22↔23 and 23↔26; 07938 authentic save restoration was hardcoded to destination 23 | `original_saved_level_policy::source_selected_transition` admits any **distinct original 0..31 archive indices**, and `authenticated_no_departing_level` accepts an indexed saved-world destination **only with real original sentinel and save-loader witnesses**. Before departing-state save, the live `181B4` adapter parses original archive source/destination grids, checks live departing grid dimensions, checks destination bounds, and preserves source-owned I/O. This is format/index safety, not arbitrary source-initiated level travel |
| Original type-8 portal `2D3D8` | Private authenticated portal fixture whitelisted 23↔26 | The private original-portal helper now searches the **unmodified `LEVELS.DAT`** for a matching type-8 destination record, verifies the matching record survives in the live guest's own dynamic column list, checks source/destination archive coordinates and calls original `2D3D8`; no fabricated portal record or guest camera mutation. Ordinary physically entering the portal through original movement/event input is **not** certified |
| Source polygon/exterior raster `2A6B0` | Original `314B0` callback immediately stopped for clipped transparent planar graphics | Executes canonical `code2::function_2a6b0` with the original planar VGA Application61 bridge; authentic clip/image descriptors, source bytes, port callbacks and source guest state retained |
| Scene clip/save/overlay `296C0/29738/2978C` | Original `2C294` callback graph stopped when a clipping frame or overlay was needed | Invokes the original rectangle setter, exact 16-byte clip-save, and original `2978C → 29414 → 2A6B0/2A380` dispatch. The caller's stack scratch is marshaled to guest memory and restored, without pretending these callback results are emitted by the host |
| Animated-world actor audio `1BDAC → Application36.call_28110` | Callback was mistakenly called `original_full_actor_missing_movement` and stopped with `ORIGINAL_1BDAC_MOVE_PENDING` | Canonical `1B4F8` confirms this callback is **the original `28110` four-argument sound request**, not movement. It now executes `code2::function_28110` on the retained guest's source priority queue, preserving original actor sound behavior **without claiming physical Miles AIL playback** |

These changes do not modify original media, invent world geometry, inject actor decisions, synthesize audio completion, change the original source renderer's callback order or loosen whole-game acceptance.

## Breadth of original content exercised by one loader

[Original-media Linux saved-world gate #40400676966](https://github.com/Hyperbog/RLOFT/actions/runs/40400676966) at `87841956` **passed**:
- original compiled `181B4` direct archive loading for **32/32 original `LEVELS.DAT` indexed worlds**;
- original compiled `181B4` saved-world direct-load branch for **17/17** original `SAVE10/LEVnnn.DAT` files;
- existing original source save-camera and resource checks;
- no per-level engine source or rebuilt world files.

Separate inspection of the same original indexed `LEVELS.DAT` (owner file SHA-256 `50d0b1d13f81733a1ac55296d00fcba9aef002202bc00341f1062c37fd017d91`) found **32 structurally valid level entries, 1,659 per-column dynamic records, 311 original type-8 portal records and 68 distinct directed source/destination level pairs**. These are *archive metadata*, not 311 player-tested transitions. It includes both cross-level and same-level portals. None of these counts is an implementation-completion percentage.

The independent retained saved-level original VGA source diagnostic [#40400460281](https://github.com/Hyperbog/RLOFT/actions/runs/40400460281) remained green after the shared source callback changes. This proves the common-case regression did not break the authenticated saved-slot roundtrip; it does not prove every original level presents a frame.

## Current high-impact source/host gaps (do not silently count as closed)

| Owner/source route | Still missing or insufficiently generalized | Source-safe next boundary |
| --- | --- | --- |
| `1BDAC` actors / `0D550` near/special actions | `ORIGINAL_1BDAC_0D550_NEAR_ACTION_PENDING`, `...SPECIAL_ACTION_PENDING`; actor removal predicate path can still stop | Trace original `0D550` Application19 callback signatures and their original CODE.2 child functions; implement actual combat, attack, item effects, record removal and source state changes. No fabricated AI return |
| `002A8` script/event interpreter | Source script preflight can reject valid unconnected event callbacks | Map original script dispatch table, guest stack and side-effect callbacks to existing reconstructed functions; validate varied original event types across the archive |
| `1C77C` dialogue/modal presentation | `ORIGINAL_17A68_1C77C_DIALOGUE_CALLBACK_NOT_CONNECTED`; original `1C77C` has its own Application41/App39/App40 and UI/resource input/output graph | Wire the full original modal callback contract including text/resource reads, player selection and original VGA page/audio, rather than producing host dialogue substitutes |
| Original AIL/audio device | Source `28110` request queue and partial WinMM sample bridge exist, but active guest audio driver `EE78` and original `28DCC/28EF0/28E58/28F58` device paths are rejected | Implement exact source driver ABI, timing, voice/mixer resource requests and host device lifecycle. Distinguish native waveOut SFX from independent DOS Miles AIL parity |
| Input/timers and worker supervision | External stdin keyboard + `30B40` is not physical DOS IRQ9; historical third-event timeouts remain intermittent | Preserve deterministic worker ownership and bounded cancellation, implement original keyboard/timer/interrupt semantics and exercise long source sessions |
| CODE.1 handoff / DOS persistence | Source save/loading checked only in narrow native fixtures; original CODE.1 character creation and ordinary menu save/load not accepted | Complete shared supervisor/guest and source DOS filesystem services and test ordinary UI actions |
| Rendering remainder | Source clip and overlays are now connected, but `314B0` view/res archive variants, special `274A4` image dispatch and other uncommon paths still fail at evidenced callbacks | Exhaust source rendering callback/wall/sprite/overlay variations under source-authoritative tests, then compare synchronized retail DOS indexed scanout |

**Next engineering checkpoint:** turn the census JSON into a triaged callback ledger (direct reconstructed, live-bridge complete, host DOS/device pending, source gameplay pending, feature intentionally fail-closed). Drive the highest-shared remaining callback families by original source ABI, not by maps, enemies or individual levels. Only certify a completed dependency once the original source callback returns on varied real source data and tests fail correctly on invalid data.

**Progress metric:** authoritative coarse major gates remain **3 / 12 = 25%**. Static callback counts, levels loaded and CTest count are valuable *engineering evidence* but do not form a valid “percent game recompiled” denominator.
