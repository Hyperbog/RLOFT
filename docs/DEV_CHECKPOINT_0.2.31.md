# Development checkpoint 0.2.31

## Scope

First verified viewport/presentation checkpoint after the 0.2.30 mechanical-demo closure. Mechanical behavior remains locked at 100%; this checkpoint advances the separate viewport/presentation ledger from 30% to 31%.

## Original-code corrections

- `CODE.2 0x2B3EF..0x2B40E` and `0x2BC1C..0x2BC53`: scenery render records now distinguish `field14` direction bits 3..5 from animation-phase bits 6..8. The previous renderer incorrectly used direction for both, pushing Level-22 foliage into unrelated bare-trunk resource groups.
- `CODE.2 0x16DD8` / `0x2B6D0`: NPC render category 0 is based at `DS:0x27100 == INIT global[3] == 0`. `NPCS.DAT +0x14` therefore supplies the assassin's RES3 base directly (`315`), rather than adding the object-category base at INIT global[5].
- `CODE.2 0x36CF1..0x36D23`: ordinary sprite drawing now accepts every positive `(scale >> 3)` value and clamps the renderer scale-table index to the shipped SINTAB `C738` limit of 127. The earlier software path incorrectly reused the special `0x21BF0` interaction minimum for ordinary drawing and omitted the 127 clamp.

## Level-22 evidence

- Scenery RES3 `2513..2544` decode as the shipped orange/yellow forest canopy groups.
- Assassin RES3 `315..354` decode as the shipped assassin directional/animation frames.
- At the Level-22 heading-90 diagnostic camera, the render pass now rasterizes 173 of 199 queued sprite draw calls rather than only two under the previous interaction-threshold gate.
- The near-combat diagnostic resolves the live assassin to RES3:317 through the reconstructed view selector.

## Verification

- Incremental source build: green.
- Full `ravenloft_data_tests` against the read-only original-data regression view: green.
- Output: `PASS: core data, Quick Start opening flow, and shipped cinematic decode validated.`

The adventure HUD compositor remains incomplete and is not counted as faithful yet. The 31% milestone is specifically the first verified correction of Level-22 world-scene asset selection and ordinary sprite rendering.
