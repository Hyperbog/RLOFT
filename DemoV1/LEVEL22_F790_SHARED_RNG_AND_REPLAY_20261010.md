# Original CODE.2 F790 RNG reseed and prepared RES2:3 raster replay (2026-10-10)

**Source identity:** original user-provided CODE.2 length 558,689, SHA-256 853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314; original object-1 base 0x3AF24. Prior F66C ribbon tile layout and F790 event-cadence audits remain prerequisites.

## Critical original F790 distinction

This checkpoint corrects the previous isolated epoch adapter: ordinary held-item F790 **does** call F66C, and has two original shared RNG state resets. Original CODE.2 disassembly and nine exact verified binary windows prove:

| F790 path | Original instruction range | Shared RNG effects |
| --- | --- | --- |
| 6DAC pending | F7B4→F7BD→F66C→F7C7/FD79 | F66C generates original RES2:3 ribbon; random interior tiles advance existing shared RNG without reseeding |
| No pending, empty held ID (-1) | F7DB→FD7C | No F66C, RNG tile draw or RNG reseed |
| No pending, held ID != -1 | FD58→FD66→FD6E→FD74 | Call original 30CC4(held ID) before F66C; after tile draws read original 30700 IRQ tick and seed 30CC4(current tick) |

Additional original exact-byte windows: F7B4..F7E3, FD58..FD83 and 30CC4..30CD3; their physical CODE.2 offsets are 0x4A6D8, 0x4AC7C and 0x6BBE8 respectively. The verified binary audit retains the earlier six windows and all **17 direct E8-rel32 F790 callsites**. Original 30CC4 writes the single shared RNG state at 0xF6F4 via the fixed 30C9C accessor.

Original F790 also calls 2932C to restore the background **before** checking the pending flag. This is a separate, still unintegrated raster dependency.

## Implemented isolated code

OriginalFeedbackRibbonEpoch now directly accepts the actual native OriginalRandom object and a lazily evaluated game-tick callback. This makes the original order explicit: ordinary held ID seed, F66C random tiles, observe 30700, tick seed. Pending feedback never invokes either seed. Empty hand never invokes F66C. Prepared raster frames remain cached across repeated VGA refreshes with zero additional RNG calls.

OriginalFeedbackRibbonRaster::draw_prepared is a functional indexed-raster adapter, **currently not connected to live gameplay**. It consumes cached, already-selected frames in exact F66C source order, dispatches RES2 archive slot 2 / entry 3 using the existing 2975C native single-frame wrapper at VGA row 0x62, and draws original RES0 font 1 text at row 0x65 with verified signed 30508 centering. It does not synthesize replacement game artwork or alter RNG state.

The revised source-derived epoch regression uses the same original_random.cpp as ExplorationRuntime and verifies empty-hand, two pending draws, held-item reseeding from item ID then post-draw IRQ tick, pending expiry followed by ordinary F790, and 1,000 VGA refresh reads. An isolated raster replay and synthetic six-frame indexed-raster test verify frame sequencing, source archive/resource ID, 0x62/0x65 row placement, centered text, and preflight validation. Local tests **passed**: strict C++17 epoch against actual native OriginalRandom; callback replay and indexed pixels using a test-only framebuffer substitute. The original immutable CODE.2 binary verifier passed **9/9 windows and 17 direct caller references**. Full native raster-linked regression is registered but not yet compiled as part of Windows Full.

## Status and unresolved gates

**187 registered CTests when Python is present** (was 186); last completed Windows Full #651 passed 182/182 at an earlier source revision. No unauthorized Windows build was dispatched. Native PartyModalPresenter is still unchanged, and original-DOS pixel and gameplay comparisons remain pending.

Before live integration: reconstruct F790's 2932C background path; source-check all interaction-specific F790 calls; expose the existing ExplorationRuntime original_random_ without adding another PRNG; pass authentic RES2:3 and font-1 resources; ensure Win32 70-Hz repaints only replay cached work, never invoke F790 or 30CA2 themselves; and obtain original DOS/Windows screen comparisons. No independent Level23 game behavior or modernization before faithful Level22 completion.
