# Part 26 — Original 1F8E0/1F8FC scene and 17A68 actor source-contract checkpoint (2026-10-22)

**Owner:** `Recomp` branch. **Validated source commit:** [`e09be6445416e1340e590a2fdaafa28acc224226`](https://github.com/Hyperbog/RLOFT/commit/e09be6445416e1340e590a2fdaafa28acc224226).
**Status:** SOURCE CONTROL-FLOW REGRESSION ACCEPTED; continuous guest scene rendering and native gameplay are **NOT CONNECTED**.

## New source-execution evidence

`Recomp/src/test_original_code2_1f8fc_17a68.cpp` includes unchanged canonical `Code2/Code2.cpp` and executes compiled original functions with an isolated, synthetic guest-memory fixture:

- `1F8E0 → 1F8FC`: checks changed-mode branch versus unchanged-mode early return and source-mutated dirty flag, selected page, frame temp handles and scene state.
- Checks exact source call order and argument classes: `291B0`, two `2E510`, two `2EFD0`, three `30070`, `325B0`, `29234`, `1A5B0`, `22D70`, `2F868`, `30B2C`. Intermediate HUD/world and cursor callback families not provided remain intentionally untested, not automatically proven.
- `17A68`: executes original `17838` party pre-pass, original `2ACDC` actor kind predicate, original `1C1DC/1C124` quest bit, nonpositive-health flags and type-8 quest-mark branches, on synthetic actor records.
- These callbacks are instrumentation only; `30070` returns test handles rather than real captured images, `2E510` only records requests, and **no original visual output is produced**.

## Verified Actions on source commit

| Workflow | Run | Result |
|---|---|---|
| Linux portable C++ regression | [#39853331288](https://github.com/Hyperbog/RLOFT/actions/runs/39853331288) | PASS, **86/86 CTests** including new scene/actor test |
| Windows MSVC original-media milestone | [#39853331268](https://github.com/Hyperbog/RLOFT/actions/runs/39853331268) | PASS, **86/86 CTests**, retained original-media validation |
| Canonical CODE.1/CODE.2 source link | [#39853331510](https://github.com/Hyperbog/RLOFT/actions/runs/39853331510) | PASS |
| Source world RES4 Linux | [#39853331266](https://github.com/Hyperbog/RLOFT/actions/runs/39853331266) | PASS |
| Original 181B4 saved-level direct-load | [#39853331475](https://github.com/Hyperbog/RLOFT/actions/runs/39853331475) | PASS |

The Windows original-media test log still confirms the Part 25 immutable `LEVELS.DAT` Level 22 **1 moving + 110 primary = 111 actor** records, RES0:62 cursor source draw and two retained `1FC14` keyboard no-key polls. These **remain prior runtime capabilities**, not newly integrated rendering capabilities.

## Next precise implementation boundary

1. On the retained original guest in `Recomp/src/code2_startup_gate.cpp`, `original_181b4_level_load` currently leaves `Application32Boundary::call_1f8e0` null. Connect the true `1F8E0/1F8FC` callback path to the **existing guest machine, original decoded RES archive callbacks and original VGA host**, preserving callback semantics, source resource cache, page state, image handle lifetimes and source call order. Fail closed on each unavailable callback. Never replace a missing original scene with a synthetic page.
2. Original `181B4` calls `17A68` only when `Application32Boundary.app31` is attached. Wire the original `1C1DC` quest bits, `2ACDC` actor predicate and conditionally reachable `30CA2/1C77C/1C328` callbacks, without silently treating zero/null response as DOS-observed behavior. Check all 111 Level 22 records against read-only original archive before and after post-load transforms.
3. Connect `1FC14` movement/timing callbacks, original view raycasting and RES4/RES3/HUD composition to continuously rendered indexed frames. Verify multiple input-driven gameplay frames and compare original DOS behavior wherever independent captures are available.

**Do not claim:** original DOS pixel parity, playable executable, full actor AI, movement, combat, cross-level transitions or R04 acceptance. No original copyrighted media, private indexed frames or preserved save bytes are included in this checkpoint.
