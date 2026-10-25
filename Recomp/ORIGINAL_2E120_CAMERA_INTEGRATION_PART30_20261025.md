# Part 30 — Original CODE.2 camera integration frontier (2026-10-25)

**Branch:** `Recomp`. **Target:** movement from actual host-provided scan words through retained source CODE.2 camera coordinates and collision logic. **Retail DOS indexed pixel parity, authentic AIL/32 driver completion, and full original gameplay are separate OPEN gates.**

## Proven by bounded source inspection, not reverse-engineering guesses

The original reconstructed `CODE.2` `function_1fc14` does not itself write camera coordinates from the keyboard accumulators. Its dispatch is:

```text
30B40 original keyboard scan word → 1FC14 original velocity setters
    → C000/C004 16-bit signed movement accumulators
    → C6E8/C6EC/C6F0 per-tick movement/turn inputs
    → call_2E120
    → compiled function_2E120 original 16.16 camera/heading integration
    → compiled function_2DF2C original SINTAB/world-step vector
    → compiled function_2DBAC original camera writes and tile/actor tests
    → compiled function_2D5D0 original primary tile collision
    → guest DS:255D0/255D4 camera position, DS:C658 heading
```

The other near-loop calls `0893C`, `1E4A0` and `1E534` are event/status deadline management; `20B40` updates temporary animation/effect records. They are **not** the camera movement integrator.

Proof sources are **real reconstructed original CODE.2 function bodies**, exposed by `.github/workflows/recomp-movement-source-inspection.yml` in [original-source run #40165403659](https://github.com/Hyperbog/RLOFT/actions/runs/40165403659) and [original-source run #40165350730](https://github.com/Hyperbog/RLOFT/actions/runs/40165350730), **not retail emulator execution**.

### Reconstructed original semantics

- `2E120` consumes `C6E4+C6EC` turning accumulator / 20, stores the remainder, updates `C658` (degrees 0–359). Grid mode (`C02F!=0`) converts nonzero signed turn to a snapped ±90° source change, not a smooth degree delta.
- `2E120` processes signed forward motion `C6E8`, uses `369A7` absolute-value logic for a special turn-motion branch, and obtains a source displacement vector through `2DF2C`.
- `2DF2C` implements original SINTAB/cosine fixed-point multiply via `35680` in smooth mode; in grid mode it uses the original 8-entry 45° direction table.
- `2DBAC` modifies original `255D4` then `255D0` **inside the original reconstructed code**. It checks `2D5D0`, retries per coordinate axis if blocked, reverts if both axes are blocked, and runs original tile-transition logic on crossing boundaries. It can request original SFX through `28110` on special tiles.
- `2D5D0` checks source map `198E0`, records returned from `1E8C8`, marker logic from `1E930`, and original built-in extra collision routines `2C800`, `2C9DC`. More complex secondary `2CC88` actor collision requires an `Application65Boundary` **not yet wired in this opt-in probe**, and must not be called fully parity-compliant.

## New source integration

`Recomp/src/original_code2_live_full_frame.inc` now contains `original_full_2e120_movement_service`, invoking the **real reconstructed original routines** on the unchanged retained guest. It wires `code2::Application67Boundary` to `code2::function_35680` and `code2::function_369a7`, and embeds `code2::Application66Boundary` for the original primary movement collision and record lookup callbacks. Unknown portal, script, actor and scene-change callbacks **fail closed**. The missing `Application65Boundary` is not silently replaced with a synthetic pass response; full original-world collision parity is still OPEN.

`Recomp/src/code2_startup_gate.cpp` wires the original `1FC14` `call_2e120` only if `RLOFT_RECOMP_EXPERIMENTAL_1FC14_MOVEMENT_SOURCE=1`. Default original source loop is unchanged.

The source callback reports `CODE2_ORIGINAL_LIVE_GAMEPLAY=2E120_MOVEMENT_SOURCE_RETURN` with before/after 16.16 camera X/Y, heading, C6E8/C6EC/C6F0 movement inputs, and explicit disclaimer `secondary_actor_collision=NOT_CONNECTED`.

A separate source CTest `canonical_code2_original_2e120_2dbac_camera_source` **compiles the actual reconstructed original source** and tests synthetic guest movement. It verifies a real source-driven guest camera coordinate update, 90° grid turning, and rejection of a solid map tile; no host code assigns the resulting game pose and no original copyright media are embedded in this test. [Canonical Windows #40165919417](https://github.com/Hyperbog/RLOFT/actions/runs/40165919417) **PASS (90/90)** and [Linux #40165919419](https://github.com/Hyperbog/RLOFT/actions/runs/40165919419) **PASS**.

### Live original media Level 22 acceptance

The Windows milestone has a **separate opt-in** original-media `S,D,W` probe with the 2E120 flag and `RLOFT_RECOMP_REQUIRE_CONSOLE_REAL_POSE=1`. It passes only if the retained original guest itself changes `255D0/255D4/C658`, not merely `C000/C004` accumulators. This remains separately assessed from source-only CTest; do not automatically equate original source synthetic fixtures with the authentic Level 22. Run-specific result must be read from the milestone CI log.

## Parallel retail DOS oracle remains independent

User-owned screenshot, matching DOSBox-X 2026.10.01 SDL2 emulator VGA savestate, original CPU/DS debugger memory and genuine original `30B40` input word trace **were not provided in this task**. No source-side indexed native frame, synthetic CTest, or owner-unreviewed debugger JSON can replace original retail DOS runtime. The capture and strict comparator protocols are in `Recomp/OWNER_LEVEL22_DOS_DEBUGGER_PCM_COMPLETION_CAPTURE_20261024.md`. Keep private game resources and screenshots out of Git and published CI ZIPs. Until a synchronized retail capture is received, **indexed DOS pixel parity stays OPEN**.

## Later active audio, still distinct

Original source `28110 → 28168 → 27F24` ABI is source-regression tested. The opt-in WinMM unsigned 8-bit Creative VOC decoder can request real host audio playback and verify WinMM `WHDR_DONE` on a device; it is **not** original Miles/AIL hardware or real source game driver completion. Original AIL/32 status, sample-finish callback, real-time service cadence and `RES6/RES10` resource provenance must be validated before claiming faithful CODE.2 audible playback. The old empty/disabled audio branches are genuine but do not prove any active sound.

## Known bounded next steps

1. Require sustained positive camera motion, heading, reentry and collision in the original-media Level 22 session before removing the experimental flag. Test both grid and smooth modes, repeated source input and safely handling actor collision/portal callbacks.
2. Obtain one **independent** same-pause retail DOSBox-X PNG + SAV + original code debugger DS/tick/input trace; run native-vs-original exact indexed and DAC comparators.
3. Only after the movement gate passes, connect active original SFX request → live host audio output with genuine status/completion semantics. Never fake success from missing AIL hardware.
